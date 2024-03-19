#include <stdio.h>

#include "microcompute_internal.h"

void mc_default_log_cb(
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    int message_len = vsnprintf(NULL, 0, fmt, args);
    va_start(args, fmt);
    char* message = malloc(message_len + 1);
    vsnprintf(message, message_len + 1, fmt, args);
    va_end(args);

    printf("%s | %s | %s", mc_log_level_to_str(lvl), src, message);

    if (strlen(file) > 0) printf(" (%s:%d)\n", file, line);
    else printf("\n");
}

size_t mc_read_file(const char* path, char* contents) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (contents) (void)!fread(contents, 1, length, fp);
    fclose(fp);
    return length;
}

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}

const char* mc_log_level_to_str(mc_LogLevel_t level) {
    switch (level) {
        case MC_LOG_LEVEL_DEBUG: return "MC_LOG_LEVEL_DEBUG";
        case MC_LOG_LEVEL_INFO: return "MC_LOG_LEVEL_INFO";
        case MC_LOG_LEVEL_WARN: return "MC_LOG_LEVEL_WARN";
        case MC_LOG_LEVEL_ERROR: return "MC_LOG_LEVEL_ERROR";
        default: return "MC_LOG_LEVEL_UNKNOWN"; // just in case
    }
}

const char* mc_device_type_to_str(mc_DeviceType_t type) {
    switch (type) {
        case MC_DEVICE_TYPE_DGPU: return "MC_DEVICE_TYPE_DGPU";
        case MC_DEVICE_TYPE_IGPU: return "MC_DEVICE_TYPE_IGPU";
        case MC_DEVICE_TYPE_VGPU: return "MC_DEVICE_TYPE_VGPU";
        case MC_DEVICE_TYPE_CPU: return "MC_DEVICE_TYPE_CPU";
        default: return "MC_DEVICE_TYPE_OTHER";
    }
}