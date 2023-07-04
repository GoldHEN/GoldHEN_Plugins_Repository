// FrameTime Logger: Log frametime statistics.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <Common.h>
#include <stdbool.h>
#include <orbis/Pad.h>
#include <time.h>
#include <orbis/UserService.h>
#include <orbis/SystemService.h>
#include <orbis/Sysmodule.h>
#include "plugin_common.h"

#define PLUGIN_NAME "frame_logger"
#define LOG_FOLDER "/data/" PLUGIN_NAME

attr_public const char *g_pluginName = PLUGIN_NAME;
attr_public const char *g_pluginDesc = "Log frametime statistics.";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

int32_t sceGnmSubmitAndFlipCommandBuffers(uint32_t count, void *dcbGpuAddrs[], uint32_t *dcbSizesInBytes, void *ccbGpuAddrs[], uint32_t *ccbSizesInBytes, uint32_t videoOutHandle, uint32_t displayBufferIndex, uint32_t flipMode, int64_t flipArg);

HOOK_INIT(sceGnmSubmitAndFlipCommandBuffers);

FILE *g_LogFILE = NULL;
bool g_isRecording = false;
uint64_t g_TimeStart = 0;
uint64_t g_CurrentDelta = 0;
double g_TscTick = 0;
bool g_GnmHook = false;
bool g_RunningThread = false;

void doStats(void)
{
    if (!g_GnmHook)
    {
        return;
    }
    uint64_t current_time = sceKernelGetProcessTimeCounter();
    if (g_isRecording && g_LogFILE)
    {
        fprintf(g_LogFILE, "%lf,%lf\n", ((current_time - g_TimeStart) / g_TscTick), ((current_time - g_CurrentDelta) / g_TscTick));
    }
    g_CurrentDelta = current_time;
}

int32_t sceGnmSubmitAndFlipCommandBuffers_hook(uint32_t count, void *dcbGpuAddrs[], uint32_t *dcbSizesInBytes, void *ccbGpuAddrs[], uint32_t *ccbSizesInBytes, uint32_t videoOutHandle, uint32_t displayBufferIndex, uint32_t flipMode, int64_t flipArg)
{
    if (!g_GnmHook)
    {
        g_GnmHook = true;
    }
    doStats();
    return HOOK_CONTINUE(sceGnmSubmitAndFlipCommandBuffers,
                         int32_t(*)(uint32_t, void **, uint32_t *, void **, uint32_t *, uint32_t, uint32_t, uint32_t, int64_t),
                         count, dcbGpuAddrs, dcbSizesInBytes, ccbGpuAddrs, ccbSizesInBytes, videoOutHandle, displayBufferIndex, flipMode, flipArg);
}

struct tm get_local_time(void)
{
    int32_t tz_offset = 0;
    int32_t tz_dst = 0;
    int32_t ret = 0;

    if ((ret = sceSystemServiceParamGetInt(ORBIS_SYSTEM_SERVICE_PARAM_ID_TIME_ZONE, &tz_offset)) < 0)
    {
        final_printf("Failed to obtain ORBIS_SYSTEM_SERVICE_PARAM_ID_TIME_ZONE! Setting timezone offset to 0\n");
        final_printf("sceSystemServiceParamGetInt: 0x%08x\n", ret);
        tz_offset = 0;
    }

    if ((ret = sceSystemServiceParamGetInt(ORBIS_SYSTEM_SERVICE_PARAM_ID_SUMMERTIME, &tz_dst)) < 0)
    {
        final_printf("Failed to obtain ORBIS_SYSTEM_SERVICE_PARAM_ID_SUMMERTIME! Setting timezone daylight time savings to 0\n");
        final_printf("sceSystemServiceParamGetInt: 0x%08x\n", ret);
        tz_dst = 0;
    }

    time_t modifiedTime = time(NULL) + ((tz_offset + (tz_dst * 60)) * 60);
    return (*gmtime(&modifiedTime));
}

void toggleRecording(void)
{
    g_isRecording = !g_isRecording;
    if (!g_GnmHook)
    {
        NotifyStatic(TEX_ICON_SYSTEM, "g_GnmHook is false! cannot start data capture\nPlugin (" PLUGIN_NAME ") will now quit.");
        g_isRecording = false;
        g_RunningThread = false;
        return;
    }
    if (g_isRecording)
    {
        sceKernelMkdir(LOG_FOLDER "/", 0777);
        char LogFilePath[MAX_PATH_] = {0};
        g_LogFILE = NULL;
        struct tm t = get_local_time();
        snprintf(LogFilePath, sizeof(LogFilePath), LOG_FOLDER "/" PLUGIN_NAME "-data-%d-%02d-%02d_%02d-%02d-%02d.csv", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        g_LogFILE = fopen(LogFilePath, "w");
        if (g_LogFILE)
        {
            g_TscTick = (double)sceKernelGetProcessTimeCounterFrequency();
            g_TimeStart = sceKernelGetProcessTimeCounter();
            g_CurrentDelta = g_TimeStart;
            Notify(TEX_ICON_SYSTEM, "Recording start:\n%s", LogFilePath);
            fputs("//Ignore=true\n"
                  "TimeInSeconds,msBetweenPresents\n",
                  g_LogFILE);
        }
        else
        {
            Notify(TEX_ICON_SYSTEM, "Failed to create:\n%s", LogFilePath);
            g_isRecording = false;
        }
    }
    else
    {
        fflush(g_LogFILE);
        fclose(g_LogFILE);
        NotifyStatic(TEX_ICON_SYSTEM, "Recording stop");
    }
}

bool checkRecordingButton(OrbisPadData *pData)
{
    return (pData->buttons & ORBIS_PAD_BUTTON_L3) &&
           (pData->buttons & ORBIS_PAD_BUTTON_L1) &&
           (pData->buttons & ORBIS_PAD_BUTTON_TRIANGLE);
}

bool checkKillButton(OrbisPadData *pData)
{
    return (pData->buttons & ORBIS_PAD_BUTTON_L3) &&
           (pData->buttons & ORBIS_PAD_BUTTON_R3) &&
           (pData->buttons & ORBIS_PAD_BUTTON_L1) &&
           (pData->buttons & ORBIS_PAD_BUTTON_R1) &&
           (pData->buttons & ORBIS_PAD_BUTTON_SQUARE);
}

int32_t GetHandle(void)
{
    int32_t padHandle = 0;
    int32_t userID = 0;
    OrbisUserServiceInitializeParams param;
    param.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;
    sceUserServiceInitialize(&param);
    sceUserServiceGetInitialUser(&userID);
    if ((padHandle = scePadOpen(userID, 0, 0, 0)) == ORBIS_PAD_ERROR_ALREADY_OPENED)
    {
        padHandle = scePadGetHandle(userID, 0, 0);
    }
    else if ((padHandle = scePadOpen(userID, 0, 0, 0)) < 0)
    {
        final_printf("scePadOpen: return 0x%08x\n", padHandle);
        padHandle = 0;
    }
    return padHandle;
}

void *frame_logger_input_thread(void *args)
{
    final_printf("%s: Started\n", __func__);
    final_printf("%s: Waiting 10 seconds to start input lisener\n", __func__);
    sleep(10);
    int32_t ret = 0;
    bool prevTogglePressed = false;
    if ((ret = scePadInit() < 0))
    {
        NotifyStatic(TEX_ICON_SYSTEM, "Failed to init pad");
        g_RunningThread = false;
    }
    OrbisPadData pData;
    final_printf("Start listening for controller inputs\n");
    while (g_RunningThread)
    {
        int32_t PadHandle = GetHandle();
        int32_t ret = scePadReadState(PadHandle, &pData);
        debug_printf("scePadReadState: 0x%08x\n", ret);
        if (ret == 0 && PadHandle > 0 && pData.connected)
        {
            bool currentTogglePressed = checkRecordingButton(&pData);
            if (currentTogglePressed && !prevTogglePressed)
            {
                toggleRecording();
            }
            prevTogglePressed = currentTogglePressed;
            if (checkKillButton(&pData))
            {
                NotifyStatic(TEX_ICON_SYSTEM, "User requested exit for Plugin (" PLUGIN_NAME ")");
                g_RunningThread = false;
            }
        }
        else
        {
            final_printf(STRINGIFY(PadHandle)": 0x%08x\n", PadHandle);
            final_printf(STRINGIFY(ret)": 0x%08x\n", ret);
            final_printf(STRINGIFY(pData.connected)": 0x%02x (%s)\n", pData.connected, pData.connected ? "true" : "false");
        }
        // periodically flush the stream every second to avoid data loss
        if (g_LogFILE)
        {
#if (__FINAL__) == 0
            int32_t flush_ret = fflush(g_LogFILE);
            debug_printf("fflush: 0x%08x\n", flush_ret);
#else
            fflush(g_LogFILE);
#endif
        }
        sleep(1);
    }
    final_printf("%s: Exit\n", __func__);
    scePthreadExit(NULL);
    return NULL;
}

s32 attr_public plugin_load(s32 argc, const char *argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    g_GnmHook = false;
    g_RunningThread = true;
    g_TimeStart = sceKernelGetProcessTimeCounter();
    g_TscTick = (double)sceKernelGetProcessTimeCounterFrequency();

    OrbisPthread thread;
    scePthreadCreate(&thread, NULL, frame_logger_input_thread, NULL, STRINGIFY(frame_logger_input_thread));
    HOOK32(sceGnmSubmitAndFlipCommandBuffers);
    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char *argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceGnmSubmitAndFlipCommandBuffers);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}
