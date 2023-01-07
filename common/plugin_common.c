#include "plugin_common.h"
#include <Common.h>

// Thanks to OSM
// https://github.com/OSM-Made/PS4-Notify/blob/c6d259bc5bd4aa519f5b0ce4f5e27ef7cb01ffdd/Notify.cpp

// For pre formatted strings
void NotifyStatic(const char* IconUri, const char* text) {
    OrbisNotificationRequest Buffer;
    final_printf("Notify text:\n%s\n", text);
    Buffer.type = NotificationRequest;
    Buffer.unk3 = 0;
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.message, text);
    strcpy(Buffer.iconUri, IconUri);
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}

// For formatted strings
void Notify(const char* IconUri, const char *FMT, ...) {
    OrbisNotificationRequest Buffer;
    va_list args;
    va_start(args, FMT);
    vsprintf(Buffer.message, FMT, args);
    va_end(args);
    final_printf("Notify message:\n%s\n", Buffer.message);
    Buffer.type = NotificationRequest;
    Buffer.unk3 = 0;
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.iconUri, IconUri);
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}
