#include <vitasdkkern.h>
#include <taihen.h>
#include <libk/string.h>
#include <libk/stdio.h>
#include <libk/stdlib.h>
#include <math.h>

#define HOOKS_NUM   3  // Hooked functions num

static uint8_t current_hook = 0;
static SceUID hooks[HOOKS_NUM];
static tai_hook_ref_t refs[HOOKS_NUM];

static uint32_t deadzoneLeft, deadzoneOuterLeft, slowTravelLeft, slowMaxLeft, deadzoneRight, deadzoneOuterRight, slowTravelRight, slowMaxRight, diagonalLimiter;
static char buffer[64];
static char rescaleLeft, rescaleRight, widePatch;
static uint8_t apply_wide_patch = 0;

static void (*patchFuncLeft)(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax, int dLimiter);
static void (*patchFuncRight)(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax, int dLimiter);

// Courtesy of rsn8887
// Thanks u/lizin5ths for outer deadzone idea
void rescaleAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax, int dLimiter) {
    //radial and scaled deadzone
    //http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
    //input and output values go from 0...255;

    // [step 1] deadzone covers the entire range, always centre

    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    // [step 2] check if both deadzones off

    // outer deadzone on/off check
    if (dead < 0) dead = 0;
    if (deadOuter <= 0 || deadOuter > 127) deadOuter = 127;
    // if both deadzones are off, return
    if (dead == 0 && deadOuter == 127) return;

    // [step 3] check if stick within inner deadzone range

    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);
    float deadZone = (float) dead;
    // for rescaleAnalogs(), use "<=" instead of "<", since "deadOuter >= dead + 1"
    if (magnitude <= deadZone){
        *x = 127;
        *y = 127;
        return;
    }

    // [step 4] check if stick within slow mode range

    // ensure inner deadzone and outer deadzone not overlapping
    // "deadOuter = dead + 1" to avoid "deadOuter == dead" which would cause "/0" (divide by 0) error later in [step 5]
    if (deadOuter <= dead) deadOuter = dead + 1;

    // check if slow mode config is legit
    // doesn't meet minimum requirement (non-dz range < 10), mode unavailable, force slow mode = off
    if ((deadOuter - dead) < 10) slowTrv = 0;
    // slow mode = on, but lower than min range (5), auto correct to min
    else if (slowTrv != 0 && slowTrv < 5) slowTrv = 5;
    // slow mode = on, but higher than max range (50% of non-dz range), auto correct to max
    else if (slowTrv > (deadOuter - dead) / 2) slowTrv = (deadOuter - dead) / 2;

    float slowZoneEnd = (float) (dead + slowTrv);
    float slowMaximum = 0.0f;

    float absAnalogX = fabs(analogX);
    float absAnalogY = fabs(analogY);

    // slow mode for rescaleAnalogs(), rescale output using slow mode config
    if (slowTrv != 0){
        // check if slow mode max output setting is legit (5 ~ slowTrv), auto correct to min or max
        if (slowMax < 5) slowMax = 5;
        else if (slowMax > slowTrv) slowMax = slowTrv;
        float slowTravel = (float) slowTrv;
        slowMaximum = (float) slowMax;

        // calculate slow mode output
        if (magnitude <= slowZoneEnd){
            // find scaled axis values with magnitudes between zero and slowMaximum
            float scalingFactor = (slowZoneEnd / magnitude) * ((magnitude - deadZone) / slowTravel) * (slowMaximum / slowZoneEnd);
            analogX = (analogX * scalingFactor);
            analogY = (analogY * scalingFactor);

            // clamp to ensure results will always lie between 0 and slowMaximum
            float clampingFactor = 1.0f;
            absAnalogX = fabs(analogX);
            absAnalogY = fabs(analogY);
            if (absAnalogX > slowMaximum || absAnalogY > slowMaximum){
                if (absAnalogX > absAnalogY)
                    clampingFactor = slowMaximum / absAnalogX;
                else
                    clampingFactor = slowMaximum / absAnalogY;
            }
            *x = (uint8_t) ((clampingFactor * analogX) + 127.0f);
            *y = (uint8_t) ((clampingFactor * analogY) + 127.0f);
            return;
        }
    }

    // [step 5] rescaling, outer deadzone

    float deadZoneOuter = (float) deadOuter;

    // if slow mode = on, set "imaginary" inner deadzone value for scaling factor calculation
    if (slowTrv != 0){
        deadZone = deadZoneOuter - 127.0f * (deadZoneOuter - slowZoneEnd) / (127.0f - slowMaximum);
    }
    if (magnitude > slowZoneEnd){
        //adjust maximum magnitude
        float maximum;

        // check and setup "diagonal scaling limiter" value for "maximum magnitude" and "scaled axis values" calculation
        // 100 ~ 141 means limit "maximum magnitude" value to (1.00x ~ 1.41x) of 127.0
        // 142 means limiter off, sqrt(2) > 1.41000f
        if (dLimiter == 0) dLimiter = 125;
        else if (dLimiter < 100) dLimiter = 100;
        else if (dLimiter > 142) dLimiter = 142;
        float maxLimiter = (float) dLimiter * 1.27f;

        if (absAnalogX > absAnalogY)
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogY) / absAnalogX) * ((127.0f * analogY) / absAnalogX));
        else
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogX) / absAnalogY) * ((127.0f * analogX) / absAnalogY));

        if (dLimiter < 142 && maximum > maxLimiter) maximum = maxLimiter;
        if (maximum < magnitude) maximum = magnitude;

        // find scaled axis values with magnitudes between slow mode boundary and maximum
        float scalingFactor = maximum / magnitude * (magnitude - deadZone) / ((maximum - (127.0f - deadZoneOuter)) - deadZone);
        analogX = (analogX * scalingFactor);
        analogY = (analogY * scalingFactor);

        // clamp to ensure results will always lie between 0 and 255
        absAnalogX = fabs(analogX);
        absAnalogY = fabs(analogY);
        // check if result within range "-128 ~ 128" (-1 ~ 255)
        if (absAnalogX > 128.0f || absAnalogY > 128.0f){
            float clampingFactor = 1.0f;
            if (absAnalogX > absAnalogY) clampingFactor = 128.0f / absAnalogX;
            else clampingFactor = 128.0f / absAnalogY;

            analogX = (clampingFactor * analogX);
            analogY = (clampingFactor * analogY);

            // when "result < -127", use 0.992188f (= 127.0f / 128.0f) to bring it back to -127 (0)
            if (analogX < -127.0f || analogY < -127.0f){
                analogX = 0.992188f * analogX;
                analogY = 0.992188f * analogY;
            }
        }

        // convert results back to 0 ~ 255 from -127 ~ 128
        *x = (uint8_t) (analogX + 127.0f);
        *y = (uint8_t) (analogY + 127.0f);
        return;
    }
    // (debug) something is wrong if code execution ended up here
    // return;
}

void deadzoneAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax, int dLimiter) {
    // dLimiter currently unused in deadzoneAnalogs()

    // [step 1] deadzone covers the entire range, always centre

    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    // [step 2] check if both deadzones off

    // outer deadzone on/off check
    if (dead < 0) dead = 0;
    if (deadOuter <= 0 || deadOuter > 127) deadOuter = 127;
    // if both deadzones are off, return
    if (dead == 0 && deadOuter == 127) return;

    // [step 3] check if stick within inner deadzone range

    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);
    float deadZone = (float) dead;
    // for deadzoneAnalogs(), use "<" instead of "<=", since "deadOuter >= dead"
    if (magnitude < deadZone){
        *x = 127;
        *y = 127;
        return;
    }

    // [step 4] check if stick within slow mode range

    // ensure inner deadzone and outer deadzone not overlapping
    if (deadOuter < dead) deadOuter = dead;

    // check if slow mode config is legit
    // doesn't meet minimum requirement (non-dz range < 10), mode unavailable, force slow mode = off
    if ((deadOuter - dead) < 10) slowTrv = 0;
    // slow mode = on, but lower than min range (5), auto correct to min
    else if (slowTrv != 0 && slowTrv < 5) slowTrv = 5;
    // slow mode = on, but higher than max range (50% of non-dz range), auto correct to max
    else if (slowTrv > (deadOuter - dead) / 2) slowTrv = (deadOuter - dead) / 2;

    // slow mode for deadzoneAnalogs(), always output magnitude slow mode max
    if (slowTrv != 0){
        // check if slow mode max output setting is legit (5 ~ slowTrv), auto correct to min or max
        if (slowMax < 5) slowMax = 5;
        else if (slowMax > slowTrv) slowMax = slowTrv;
        float slowZoneEnd = (float) (dead + slowTrv);
        float slowMaximum = (float) slowMax;

        // calculate slow mode output
        if (magnitude <= slowZoneEnd){
            float scalingFactor = slowMaximum / magnitude;
            *x = (uint8_t) ((scalingFactor * analogX) + 127.0f);
            *y = (uint8_t) ((scalingFactor * analogY) + 127.0f);
            return;
        }
    }
    
    // [step 5] outer deadzone (convert output to 8-way) for deadzoneAnalogs()

    float deadZoneOuter = (float) deadOuter;

    // if outer daedzone = on
    if (deadOuter != 127){
        // convert output to 8-way
        if (magnitude >= deadZoneOuter){
            // 127 +/- 49 = 176 & 78, 127*sin(22.5deg) almost = 49
            if (*x >= 176) *x = 255;
            else if (*x <= 78) *x = 0;
            else *x = 127;
            if (*y >= 176) *y = 255;
            else if (*y <= 78) *y = 0;
            else *y = 127;
            return;
        }
    }
    // return, no change made (current input not within inner dz, outer dz, and slow mode's range)
    return;
}

void patchData(uint8_t *data) {
    patchFuncLeft(&data[12], &data[13], deadzoneLeft, deadzoneOuterLeft, slowTravelLeft, slowMaxLeft, diagonalLimiter);
    patchFuncRight(&data[14], &data[15], deadzoneRight, deadzoneOuterRight, slowTravelRight, slowMaxRight, diagonalLimiter);
}

void loadConfig(void) {

    // load config file AnaEnKaiCfg.txt from ur0:tai
    SceUID fd = ksceIoOpen("ur0:/tai/AnaEnKaiCfg.txt", SCE_O_RDONLY, 0777);
    if (fd >= 0){
        ksceIoRead(fd, buffer, 64);
        ksceIoClose(fd);
    }else {
        // no config in ur0:tai, check config.txt from ux0:data/AnalogsEnhancerKai
        // in case folder doesn't exist
        ksceIoMkdir("ux0:data/AnalogsEnhancerKai", 0777);
        fd = ksceIoOpen("ux0:/data/AnalogsEnhancerKai/config.txt", SCE_O_RDONLY, 0777);
        if (fd >= 0){
            ksceIoRead(fd, buffer, 64);
            ksceIoClose(fd);
        }else sprintf(buffer, "l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=125");
        // if no config file present, use default, everything off
    }
    sscanf(buffer, "l=%lu,%lu,%c,s=%lu,%lu;r=%lu,%lu,%c,s=%lu,%lu;%c;d=%lu", &deadzoneLeft, &deadzoneOuterLeft, &rescaleLeft, &slowTravelLeft, &slowMaxLeft, &deadzoneRight, &deadzoneOuterRight, &rescaleRight, &slowTravelRight, &slowMaxRight, &widePatch, &diagonalLimiter);

    // config explained
    // l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=125 {
    //     l={ left inner dz boundary (0 , 1 ~ 126 , 127) }, { left outer dz boundary (0 , 1 ~ 126 , 127) }, { use left rescaling (y/n) }
    //     ,
    //     s={ left slow mode boundary (0, 5 ~ "50% non-dz range") }, { left slow mode max output (0, 5 ~ "left slow mode boundary") }
    //     ;
    //     r={ right inner dz boundary (0 , 1 ~ 126 , 127) }, { right outer dz boundary (0 , 1 ~ 126 , 127) }, { use right rescaling (y/n) }
    //     ,
    //     s={ right slow mode boundary (0, 5 ~ "50% non-dz range") }, { right slow mode max output (0, 5 ~ "right slow mode boundary") }
    //     ;
    //     { use ANALOG_WIDE mode (y/n) }
    //     ;
    //     d={ diagonal scaling limiter (100 ~ 142) }
    // }
    // {
    //     inner dz boundary = 0 >> inner dz OFF ; > 126 >> always centre (x=127,y=127)
    //     outer dz boundary = 0 or = 127 >> outer dz OFF
    //     use rescaling >> y = yes, n = no
    //     slow mode boundary = 0 >> slow mode OFF ; inner dz OFF && outer dz OFF >> slow mode OFF ; "50% of non-dz range" less than 5 >> slow mode OFF
    //     slow mode max output >> when slow mode ON && slow mode max output value out of range, will auto correct to legit min or max
    //     use ANALOG_WIDE mode >> y = yes, n = no
    //     diagonal scaling limiter >> default & recommended 125 ; 0 = 125 ; will auto correct to legit min or max
    // }

    if (rescaleLeft == 'y') patchFuncLeft = rescaleAnalogs;
    else patchFuncLeft = deadzoneAnalogs;
    if (rescaleRight == 'y') patchFuncRight = rescaleAnalogs;
    else patchFuncRight = deadzoneAnalogs;
    if (widePatch == 'y') apply_wide_patch = 1;
}

// Simplified generic hooking functions
void hookFunctionExport(uint32_t nid, const void *func, const char *module) {
    hooks[current_hook] = taiHookFunctionExportForKernel(KERNEL_PID, &refs[current_hook], module, TAI_ANY_LIBRARY, nid, func);
    current_hook++;
}

int ksceCtrlSetSamplingMode_patched(SceCtrlPadInputMode mode) {
    if (mode == SCE_CTRL_MODE_ANALOG) mode = SCE_CTRL_MODE_ANALOG_WIDE;
    return TAI_CONTINUE(int, refs[2], mode);
}

int ksceCtrlPeekBufferPositive_patched(int port, SceCtrlData *ctrl, int count) {
    int ret = TAI_CONTINUE(int, refs[0], port, ctrl, count);
    patchData((uint8_t*)ctrl);
    return ret;
}

int ksceCtrlReadBufferPositive_patched(int port, SceCtrlData *ctrl, int count) {
    int ret = TAI_CONTINUE(int, refs[1], port, ctrl, count);
    patchData((uint8_t*)ctrl);
    return ret;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {

    // Setup stuffs
    loadConfig();

    // Hooking functions
    hookFunctionExport(0xEA1D3A34, ksceCtrlPeekBufferPositive_patched, "SceCtrl");
    hookFunctionExport(0x9B96A1AA, ksceCtrlReadBufferPositive_patched, "SceCtrl");
    if (apply_wide_patch) hookFunctionExport(0x80F5E418, ksceCtrlSetSamplingMode_patched, "SceCtrl");

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {

    // Freeing hooks
    while (current_hook-- > 0){
        taiHookReleaseForKernel(hooks[current_hook], refs[current_hook]);
    }

    return SCE_KERNEL_STOP_SUCCESS;

}
