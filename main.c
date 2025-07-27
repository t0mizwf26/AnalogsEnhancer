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

static uint32_t deadzoneLeft, deadzoneOuterLeft, deadzoneRight, deadzoneOuterRight;
static char buffer[32];
static char rescaleLeft, rescaleRight, widePatch;
static uint8_t apply_wide_patch = 0;

static void (*patchFuncLeft)(uint8_t *x, uint8_t *y, int dead, int deadOuter);
static void (*patchFuncRight)(uint8_t *x, uint8_t *y, int dead, int deadOuter);

// Courtesy of rsn8887
void rescaleAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter) {
    //radial and scaled deadzone
    //http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
    //input and output values go from 0...255;

    if (dead == 0) return;
    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float deadZone = (float) dead;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);
    if (magnitude >= deadZone){
        //adjust maximum magnitude
        float absAnalogX = fabs(analogX);
        float absAnalogY = fabs(analogY);
        float maximum;
        if (absAnalogX > absAnalogY)
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogY) / absAnalogX) * ((127.0f * analogY) / absAnalogX));
        else
            maximum = sqrt(127.0f * 127.0f + ((127.0f * analogX) / absAnalogY) * ((127.0f * analogX) / absAnalogY));

        if (maximum > 1.25f * 127.0f) maximum = 1.25f * 127.0f;
        if (maximum < magnitude) maximum = magnitude;

        // find scaled axis values with magnitudes between zero and maximum
        float scalingFactor = maximum / magnitude * (magnitude - deadZone) / (maximum - deadZone);
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
    }else{
        *x = 127;
        *y = 127;
    }
}

void deadzoneAnalogs(uint8_t *x, uint8_t *y, int dead, int deadOuter) {

    // ensure inner deadzone and outer deadzone never overlap
    if (deadOuter <= dead) deadOuter = dead + 1;
    // 0 or 128 will disable outer deadzone
    if (deadOuter == 0 || deadOuter > 128) deadOuter = 128;

    // both inner and outer deadzone disabled
    if (dead == 0 && deadOuter == 128) return;

    // stick disabled
    if (dead > 126) {
        *x = 127;
        *y = 127;
        return;
    }

    float analogX = (float) *x - 127.0f;
    float analogY = (float) *y - 127.0f;
    float deadZone = (float) dead;
    float deadZoneOuter = (float) deadOuter;
    float magnitude = sqrt(analogX * analogX + analogY * analogY);

    // when within inner deadzone, stick is centred
    if (magnitude < deadZone){
        *x = 127;
        *y = 127;
    }

    // when exceeding outer deadzone, stick become 8-way digital
    // 176 and 78 means 127 +/- 49
    // 128*sin(22.5deg) = ~49
    if (magnitude >= deadOuter){
        if (*x > 176) *x = 255;
        else if (*x < 78) *x = 0;
        else *x = 127;
        if (*y > 176) *y = 255;
        else if (*y < 78) *y = 0;
        else *y = 127;
    }
    // by playing with inner and outer deadzone value, this will make analog into 8-way digital with customizable actuation point
}

void patchData(uint8_t *data) {
    patchFuncLeft(&data[12], &data[13], deadzoneLeft, deadzoneOuterLeft);
    patchFuncRight(&data[14], &data[15], deadzoneRight, deadzoneOuterRight);
}

void loadConfig(void) {

    // Just in case the folder doesn't exist
    // ksceIoMkdir("ux0:data/AnalogsEnhancer", 0777);

    // Loading generic config file
    SceUID fd = ksceIoOpen("ur0:/tai/AnalogsEnhancerKai.txt", SCE_O_RDONLY, 0777);
    if (fd >= 0){
        ksceIoRead(fd, buffer, 32);
        ksceIoClose(fd);
    }else sprintf(buffer, "left=0,128,n;right=0,128,n;y");
    sscanf(buffer, "left=%lu,%lu,%c;right=%lu,%lu,%c;%c", &deadzoneLeft, &deadzoneOuterLeft, &rescaleLeft, &deadzoneRight, &deadzoneOuterRight, &rescaleRight, &widePatch);

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
