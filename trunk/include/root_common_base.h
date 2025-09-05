#ifndef __ROOT_COMMON_BASE_H__
#define __ROOT_COMMON_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h> // IWYU pragma: keep
#include <stdbool.h> // IWYU pragma: keep
#include <stddef.h> // IWYU pragma: keep
#include <string.h> // IWYU pragma: keep

#include "root_list_api.h" // IWYU pragma: keep
#include "root_log_define.h" // IWYU pragma: keep

// 内存申请宏定义.
#define MALLOC_FOR_PTR(ptr, __struct_type) \
    __struct_type *ptr = u_malloz(sizeof(__struct_type)); \
    do { \
        if (!ptr) { \
            LOGE("u_malloz(%d)fail!\n", sizeof(__struct_type)); \
        } \
    } while (0)

// 全局查找模块.
#define GLOBAL_FIND_MODULE(__P_MODULE, __TYPE_T, __ID) \
    __TYPE_T *__P_MODULE = (__TYPE_T *)find_module(__ID)

// 全局获取模块.
#define GLOBAL_GET_MODULE(__PP_MODULE, __TYPE_T, __ID) \
    *(__PP_MODULE) = (__TYPE_T *)find_module(__ID)

// 经过x时间执行一次closure的代码.
#define EXECUTE_EVERY_X_DELAY(dest_time, closure) \
do { \
    static uint32_t __t0 = 0; \
    uint32_t __t1 = get_timestamp(); \
    if (__t0 == 0) { \
        __t0 = __t1; \
    } else if (__t1 - __t0 > (dest_time)) { \
         __t0 = __t1; \
        { closure } \
    } \
} while (0)   

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif 

