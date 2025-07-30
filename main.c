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

static uint32_t deadzoneLeft, deadzoneOuterLeft, slowTravelLeft, slowMaxLeft, deadzoneRight, deadzoneOuterRight, slowTravelRight, slowMaxRight;
static char buffer[32];
static char rescaleLeft, rescaleRight, widePatch;
static uint8_t apply_wide_patch = 0;

static void (*patchFuncLeft)(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax);
static void (*patchFuncRight)(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax);

// Courtesy of rsn8887
// Thanks u/lizin5ths for outer deadzone idea
void rescaleAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax) {
    //radial and scaled deadzone
    //http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
    //input and output values go from 0...255;

    // [CASE 1] Full Deadzone, aways Centre
    // stick disabled (stick always centre)
    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    // [CASE 2] If both Inner & Outer Deazone OFF, no more calculation needed.
    // outer deadzone disabled
    if (deadOuter <= 0 || deadOuter > 127) deadOuter = 127;
    // both inner and outer deadzone disabled (stick works like normal)
    if (dead == 0 && deadOuter == 127) return;

    // [CASE 3] Calculate Magnitude, if Magnitude within Inner Deadzone, set output to Centre
    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);

        // after slowTrv value is checked later, "float slowEnd = float (dead + slowTrv)" will be the physical starting magnitude for rescaling
        // instead of next line's old "float deadZone"
        // float deadZone = (float) dead;

    // within inner deadzone, stick is centred
    // "<=" instead of "<" here for rescaling ON, since minimum deadOuter here is (dead + 1)
    if (magnitude <= (float) dead){
        *x = 127;
        *y = 127;
        return;
    }

    // [CASE 4] Check Slow Mode config, if Magnitude within Slow Mode, calculate Slow Mode logical output
    // ensure inner deadzone and outer deadzone never overlap
    // unlike "void deadzoneAnalogs()", use "deadOuter = dead + 1" here, to avoid "deadOuter == dead"
    // if "deadOuter == dead", may run into "a/0" (divide by 0) error in later rescaling code "(magnitude - deadZone) / ((maximum - deadZoneOuter) - deadZone)"
    if (deadOuter <= dead) deadOuter = dead + 1;

    // check Slow Mode Range setting
    // doesn't meet Slow Mode minimum travel range requirement, Slow Mode = OFF (not available)
    if ((deadOuter-dead) < 10) slowTrv = 0;
    // Slow Mode ON, but config is lower than minimun travel range, set to minimum range
    else if (slowTrv != 0 && slowTrv < 5) slowTrv = 5;
    // Slow Mode ON, but config is higher than maximum travel range (50% of usable travel), set to maximum range
    else if (slowTrv > (deadOuter-dead)/2) slowTrv = (deadOuter-dead)/2;

    float slowEnd = float (dead + slowTrv);

    // if Slow Mode OFF, logical rescaling output magnitude should start from mag(0) (aka 127,127 centre) instead of mag(slowMax)
    float slowMaximum = 0.0f;

    // when Slow Mode ON
    if (slowTrv != 0){
        // FIRST, check Slow Mode Max Output setting
        // slowMax legit range = 5 ~ slowTrv
        if (slowMax < 5) slowMax = 5;
        else if (slowMax > slowTrv) slowMax = slowTrv;
        float slowTravel = (float) slowTrv;
        slowMaximum = (float) slowMax;

        // THEN, calculate slow mode output
        // slow mode, rescaling = ON
        if (magnitude <= slowEnd){
            // [Work in Progress]
            // blah, blah, blah
            return;
        }
    }

    // [CASE 5] Finally, Magnitude is beyond Inner-DZ & SlowMode range, calculate rescaling output
    // for more readable rescaling code, convert outward (centre->edge) "deadOuter" into inward (edge->center) "deadZoneOuter"
    float deadZoneOuter = 127.0f - (float) deadOuter;
    // reminder: "float slowEnd" is "float (dead + slowTrv)", probably should come up with a better name I guess...
    if (magnitude > slowEnd){

            // [Work in Progress]
            // blah, blah, blah
        
        //adjust maximum magnitude
        float absAnalogX = fabs(analogX);
        float absAnalogY = fabs(analogY);
        float maximum;
        if (absAnalogX > absAnalogY)
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogY) / absAnalogX) * ((127.0f * analogY) / absAnalogX));
        else
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogX) / absAnalogY) * ((127.0f * analogX) / absAnalogY));

        if (maximum > 1.25f * 127.0f) maximum = 1.25f * 127.0f;
        // So 255 (aka +128) can be reached
        if (maximum < magnitude) maximum = magnitude;

        // find scaled axis values with magnitudes between zero and maximum (or outer deadzone if enabled)
        float scalingFactor = maximum / magnitude * (magnitude - deadZone) / ((maximum - deadZoneOuter) - deadZone);
        analogX = (analogX * scalingFactor);
        analogY = (analogY * scalingFactor);

        // clamp to ensure results will always lie between 0 and 255
        float clampingFactor = 1.0f;
        absAnalogX = fabs(analogX);
        absAnalogY = fabs(analogY);

        // use 128.0f instead of 127.0f, so the allowed range is (temporarily) -128 to 128 (i.e. -1 to 255) instead of -127 to 127 (i.e. 0 to 254)
        if (absAnalogX > 128.0f || absAnalogY > 128.0f){
            if (absAnalogX > absAnalogY)
                clampingFactor = 128.0f / absAnalogX;
            else
                clampingFactor = 128.0f / absAnalogY;
        }

        analogX = (clampingFactor * analogX);
        analogY = (clampingFactor * analogY);

        // use clamping factor 0.992188f (= 127.0f / 128.0f) to fix '< -127' (i.e. '< 0') issue introduced by above code, so the range is no longer -1 to 255 but 0 to 255
        if (analogX < -127.0f || analogY < -127.0f){
            analogX = 0.992188f * analogX;
            analogY = 0.992188f * analogY;
        }

        // convert -127 ~ 128 back to 0 ~ 255
        *x = (uint8_t) (analogX + 127.0f);
        *y = (uint8_t) (analogY + 127.0f);
        return;
    }
}

void deadzoneAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter, int slowTrv, int slowMax) {

    // [CASE 1] Full Deadzone, aways Centre
    // stick disabled (stick always centre)
    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    // [CASE 2] If both Inner & Outer Deazone OFF, no more calculation needed.
    // outer deadzone disabled
    if (deadOuter <= 0 || deadOuter > 127) deadOuter = 127;
    // both inner and outer deadzone disabled (stick works like normal)
    if (dead == 0 && deadOuter == 127) return;

    // [CASE 3] Calculate Magnitude, if Magnitude within Inner Deadzone, set output to Centre
    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);

        // after slowTrv value is checked later, "float slowEnd = float (dead + slowTrv)" will be the physical starting magnitude for rescaling
        // instead of next line's old "float deadZone"
        // float deadZone = (float) dead;

    // within inner deadzone, stick is centred
    // "<" instead of "<=" here for rescaling OFF 
    if (magnitude < deadZone){
        *x = 127;
        *y = 127;
        return;
    }

    // [CASE 4] Check Slow Mode config, if Magnitude within Slow Mode, calculate Slow Mode logical output
    // ensure inner deadzone and outer deadzone never overlap
    if (deadOuter < dead) deadOuter = dead;

    // check Slow Mode Range setting
    // doesn't meet Slow Mode minimum travel range requirement, Slow Mode = OFF (not available)
    if ((deadOuter-dead) < 10) slowTrv = 0;
    // Slow Mode ON, but config is lower than minimun travel range, set to minimum range
    else if (slowTrv != 0 && slowTrv < 5) slowTrv = 5;
    // Slow Mode ON, but config is higher than maximum travel range, set to maximum range
    else if (slowTrv > (deadOuter-dead)/2) slowTrv = (deadOuter-dead)/2;

    // when Slow Mode ON
    if (slowTrv != 0){
        // FIRST, check Slow Mode Max Output setting
        // slowMax legit range = 5 ~ slowTrv
        if (slowMax < 5) slowMax = 5;
        else if (slowMax > slowTrv) slowMax = slowTrv;
        float slowTravel = (float) slowTrv;
        float slowMaximum = (float) slowMax;

        // THEN, calculate slow mode output
        // slow mode, rescaling = OFF, (magnitude == slowMaximum)
        if (magnitude <= deadZone + slowTravel){

            // [Work in Progress]
            // blah, blah, blah
            
            //set maximum magnitude
            float absAnalogX = fabs(analogX);
            float absAnalogY = fabs(analogY);
            float maximum = slowMaximum;

            // find scaled axis values with magnitudes between zero and maximum (or outer deadzone if enabled)
            float scalingFactor = maximum / magnitude * (magnitude - deadZone) / ((maximum - deadZoneOuter) - deadZone);
            analogX = (analogX * scalingFactor);
            analogY = (analogY * scalingFactor);

            // clamp to ensure results will always lower than slow mode maximum
            float clampingFactor = 1.0f;
            absAnalogX = fabs(analogX);
            absAnalogY = fabs(analogY);

            if (absAnalogX > maximum || absAnalogY > maximum){
                if (absAnalogX > absAnalogY)
                    clampingFactor = maximum / absAnalogX;
                else
                    clampingFactor = maximum / absAnalogY;
            }
            *x = (uint8_t) ((clampingFactor * analogX) + 127.0f);
            *y = (uint8_t) ((clampingFactor * analogY) + 127.0f);
            return;
        }
    }
    
    // [CASE 5+6] Finally, Magnitude is beyond Inner-DZ & SlowMode range, check if stick output normal value (CASE 5) or transform into pseudo 8-way digital (CASE 6)
    float deadZoneOuter = (float) deadOuter;
    // when outer daedzone enabled
    if (deadOuter != 127){
        // within outer-DZ, stick turn into pseudo 8-way digital
        if (magnitude >= deadZoneOuter){
            // 176 and 78 means 127 +/- 49, since 127*sin(22.5deg) = about 49
            if (*x >= 176) *x = 255;
            else if (*x <= 78) *x = 0;
            else *x = 127;
            if (*y >= 176) *y = 255;
            else if (*y <= 78) *y = 0;
            else *y = 127;
            return;
        }
    }
    // By playing with inner and outer deadzone value, this will make analog into pseudo 8-way digital with customizable actuation point
    // Diagonal directions (NE/SE/NW/SW) are only triggered when both x and y over +/- 49
    // Example 1: Inner Deadzone = 100 ; Outer Deadzone = 100
    // >> Stick is now pure 8-way digital with actuation point "magnitude 100"
    // Example 2: Inner Deadzone = 10 ; Outer Deadzone = 150
    // >> Stick working as analog with deadzone 10, until 150, then stick transform into pseudo digital
}

void patchData(uint8_t *data) {
    patchFuncLeft(&data[12], &data[13], deadzoneLeft, deadzoneOuterLeft, slowTravelLeft, slowMaxLeft);
    patchFuncRight(&data[14], &data[15], deadzoneRight, deadzoneOuterRight, slowTravelRight, slowMaxRight);
}

void loadConfig(void) {

    // try load config file from ur0:tai
    SceUID fd = ksceIoOpen("ur0:/tai/AnalogsEnhancerKai.txt", SCE_O_RDONLY, 0777);
    if (fd >= 0){
        ksceIoRead(fd, buffer, 32);
        ksceIoClose(fd);
    }else {
        // when no config in ur0:tai, go check ux0:data/
        // in case folder doesn't exist
        ksceIoMkdir("ux0:data/AnalogsEnhancerKai", 0777);
        fd = ksceIoOpen("ux0:/data/AnalogsEnhancerKai/config.txt", SCE_O_RDONLY, 0777);
        if (fd >= 0){
            ksceIoRead(fd, buffer, 32);
            ksceIoClose(fd);
        }else sprintf(buffer, "l=0,127,n,s=0,5;r=0,127,n,s=0,5;n");
        // if no config file present too, everything disabled by default, sticks will work like normal
    }
    sscanf(buffer, "l=%lu,%lu,%c,s=%lu,%lu;r=%lu,%lu,%c,s=%lu,%lu;%c", &deadzoneLeft, &deadzoneOuterLeft, &rescaleLeft, &slowTravelLeft, &slowMaxLeft, &deadzoneRight, &deadzoneOuterRight, &rescaleRight, &slowTravelRight, &slowMaxRight, &widePatch);

    // Config Explained:
    // l=0,127,n,s=0,5;r=0,127,n,s=0,5;n
    // {
    //     InnerDZ Edge > 0 = OFF
    //     OuterDZ Edge > 0 or 127 = OFF
    //     SlowMode Range > 0 = OFF
    //     SlowMode Max Output > if "SlowMode Range = OFF" then OFF , if "SlowMode Range = ON" then ON & will be auto correct to at minimum 5
    // }
    // {
    //     l={ Left InnerDZ Edge (0 ~ 126) }, { Left OuterDZ Edge (0 ~ 127) }, { Left Rescaling (y/n) }
    //     ,
    //     s={ Left SlowMode Range (0, 5 ~ "50% Non-DZ Usable Range") }, { Left SlowMode Max Output (5 ~ "Left SlowMode Range") }
    //     ;
    //     r={ Right InnerDZ Edge (0 ~ 126) }, { Right OuterDZ Edge (0 ~ 127) }, { Right Rescaling (y/n) }
    //     ,
    //     s={ Right SlowMode Range (0, 5 ~ "50% Non-DZ Usable Range" )}, { Right SlowMode Max Output (5 ~ "Right SlowMode Range") }
    //     ;
    //     { use ANALOG_WIDE mode (y/n) }
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
