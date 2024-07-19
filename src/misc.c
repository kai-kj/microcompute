#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "microcompute.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

double mc_get_time() {
    SYSTEMTIME st;
    GetSystemTime(&st);

    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);

    uint64_t sec
        = (uint64_t)ft.dwLowDateTime + ((uint64_t)ft.dwHighDateTime << 32);

    uint64_t usec = st.wMilliseconds;

    return (double)(1000000 * sec + usec) / 1000000.0;
}

#else

#include <sys/time.h>

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}

#endif

const char* mc_log_level_to_str(mc_LogLevel level) {
    switch (level) {
        case MC_LOG_LEVEL_DEBUG: return "MC_LOG_LEVEL_DEBUG";
        case MC_LOG_LEVEL_INFO: return "MC_LOG_LEVEL_INFO";
        case MC_LOG_LEVEL_WARN: return "MC_LOG_LEVEL_WARN";
        case MC_LOG_LEVEL_ERROR: return "MC_LOG_LEVEL_ERROR";
        default: return "MC_LOG_LEVEL_UNKNOWN"; // just in case
    }
}

const char* mc_device_type_to_str(mc_DeviceType type) {
    switch (type) {
        case MC_DEVICE_TYPE_DGPU: return "MC_DEVICE_TYPE_DGPU";
        case MC_DEVICE_TYPE_IGPU: return "MC_DEVICE_TYPE_IGPU";
        case MC_DEVICE_TYPE_VGPU: return "MC_DEVICE_TYPE_VGPU";
        case MC_DEVICE_TYPE_CPU: return "MC_DEVICE_TYPE_CPU";
        default: return "MC_DEVICE_TYPE_OTHER";
    }
}