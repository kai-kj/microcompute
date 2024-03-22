#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

void mc_log_cb_sink(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    char const* file,
    int line,
    const char* fmt,
    ...
) {}

void mc_log_cb_simple(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    char const* file,
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

    char* lvlStr;
    switch (lvl) {
        case MC_LOG_LEVEL_DEBUG: lvlStr = "DEBUG"; break;
        case MC_LOG_LEVEL_INFO: lvlStr = "INFO"; break;
        case MC_LOG_LEVEL_WARN: lvlStr = "WARN"; break;
        case MC_LOG_LEVEL_ERROR: lvlStr = "ERROR"; break;
        default: lvlStr = "UNKNOWN"; break;
    }

    printf("%s | %s | %s", lvlStr, src, message);

    if (strlen(file) > 0) printf(" (%s:%d)\n", file, line);
    else printf("\n");
}