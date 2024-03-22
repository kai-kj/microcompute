#ifndef MC_LOG_H
#define MC_LOG_H

#include "instance.h"
#include "microcompute.h"

#define LOG(src, lvl, ...)                                                     \
    src->_instance->log_fn(                                                    \
        src->_instance->logArg,                                                \
        lvl,                                                                   \
        #src,                                                                  \
        __FILE__,                                                              \
        __LINE__,                                                              \
        __VA_ARGS__                                                            \
    )

#define DEBUG(src, ...) LOG(src, MC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define INFO(src, ...) LOG(src, MC_LOG_LEVEL_INFO, __VA_ARGS__)
#define WARN(src, ...) LOG(src, MC_LOG_LEVEL_WARN, __VA_ARGS__)
#define ERROR(src, ...) LOG(src, MC_LOG_LEVEL_ERROR, __VA_ARGS__)

void mc_log_cb_sink(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    char const* file,
    int line,
    const char* fmt,
    ...
);

#endif // MC_LOG_H