#include <Common.h>
#include <orbis/Pad.h>
#include "plugin_common.h"

static float hue = 0.0f;
static bool running = true;

void hsv_to_rgb(float h, uint8_t *r, uint8_t *g, uint8_t *b) {
    int i = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - (int)(h / 60.0f);
    uint8_t q = (uint8_t)(255 * (1.0f - f));
    uint8_t t = (uint8_t)(255 * f);

    switch (i) {
        case 0: *r = 255; *g = t;   *b = 0;   break;
        case 1: *r = q;   *g = 255; *b = 0;   break;
        case 2: *r = 0;   *g = 255; *b = t;   break;
        case 3: *r = 0;   *g = q;   *b = 255; break;
        case 4: *r = t;   *g = 0;   *b = 255; break;
        case 5: *r = 255; *g = 0;   *b = q;   break;
    }
}

void* rainbow_thread(void* arg) {
    OrbisPadColor color;
    color.a = 255;
    while (running) {
        int handle = scePadGetHandle(0, 0, 0);
        if (handle > 0) {
            hsv_to_rgb(hue, &color.r, &color.g, &color.b);
            scePadSetLightBar(handle, &color);
            hue += 2.0f;
            if (hue >= 360.0f) hue = 0.0f;
        }
        sceKernelUsleep(30000);
    }
    return NULL;
}

int module_start(size_t argc, const void *args) {
    sceKernelLoadStartModule("libScePad.sprx", 0, NULL, 0, NULL, NULL);
    OrbisPthread thread;
    scePthreadCreate(&thread, NULL, rainbow_thread, NULL, "rainbow_thread");
    return 0;
}

int module_stop(size_t argc, const void *args) {
    running = false;
    return 0;
}
