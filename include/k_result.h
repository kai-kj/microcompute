//====================================================================================================================//
// k_result
//====================================================================================================================//

//--------------------------------------------------------------------------------------------------------------------//
// interface
//--------------------------------------------------------------------------------------------------------------------//

#ifndef K_RESULT_INCLUDE_GUARD
#define K_RESULT_INCLUDE_GUARD

#include <stdint.h>
#include <stdio.h>

#include "k_bool.h"

typedef struct k_Result {
    k_Bool ok;
    const char* file;
    int32_t line;
    const char* message;
} k_Result;

#define K_OK                                                                   \
    (k_Result) { K_TRUE, __FILE__, __LINE__, "no errors here :)" }
#define K_ERROR(message)                                                       \
    (k_Result) { K_FALSE, __FILE__, __LINE__, message }

k_Result k_ok_(char* file, int32_t line);
k_Result k_error_(char* file, int32_t line, char* message);

#define K_ASSERT_DO(cond, action)                                              \
    do {                                                                       \
        if (!(cond)) {                                                         \
            do {                                                               \
                action;                                                        \
            } while (0);                                                       \
        }                                                                      \
    } while (0)

#define K_ASSERT_DO_RET(cond, action, value)                                   \
    K_ASSERT_DO((cond), action; return (value));

#define K_ASSERT_DO_ERR(cond, action, message)                                 \
    K_ASSERT_DO_RET((cond), (action), K_ERROR(message))

#define K_ASSERT_RET(cond, value) K_ASSERT_DO((cond), return (value))
#define K_ASSERT_ERR(cond, message) K_ASSERT_RET((cond), K_ERROR(message))
#define K_ASSERT(cond) K_ASSERT_ERR((cond), ERROR("K_ASSERT failed"))

#define K_RESULT_PRINT(result)                                                 \
    do {                                                                       \
        printf(                                                                \
            "\e[1m%s:%d\e[0m, \e[1m%s\e[0m: %s\n",                             \
            (result).file,                                                     \
            (result).line,                                                     \
            (result).ok ? "\e[32mOK\e[39m" : "\e[31mERROR\e[39m",              \
            (result).message                                                   \
        );                                                                     \
    } while (0)

#endif // K_RESULT_INCLUDE_GUARD