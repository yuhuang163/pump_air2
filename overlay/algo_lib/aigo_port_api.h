#ifndef _AIGO_PORT_API_H_
#define _AIGO_PORT_API_H_
#ifdef __cplusplus
extern "C" {
#endif

#define AIGO_API_PLATFORM_BES 1
#define AIGO_API_PLATFORM_WIN 2

#define AIGO_API_PLATFORM_DEFAULT AIGO_API_PLATFORM_BES



/*******************************************************************************
 * Include files
 ******************************************************************************/
#if AIGO_API_PLATFORM_DEFAULT == AIGO_API_PLATFORM_BES
#include "root_common_base.h" // IWYU pragma: keep

// 操作系统线程
typedef thread_context_t aglo_thread_context_t;

#define AIGO_AIR_PRESSURE_NUM  AIR_PRESSURE_NUM //2 //气压传感器数量

#define AIGO_BREAST_PUMP_NUM  BREAST_PUMP_NUM //2 //吸奶器泵数量
/// 算法线程优先级
#ifndef RT_PRIO_ALGO_THREAD
#define RT_PRIO_ALGO_THREAD                    OS_PRIORITY_REALTIME
#endif

/// 算法线程栈大小
#ifndef ALGO_THREAD_STACKSIZE
#define ALGO_THREAD_STACKSIZE 2048
#endif

#define AIGO_WORK_DISPATCH_MESQ_NUM 3 //算法工作消息队列数量

/// log define
#ifndef AIGO_PRINT_LOG
#define AIGO_PRINT_LOG(...)  platform_printf(__VA_ARGS__)
#endif

#define IS_AIGO_SUPPORT_QUEUE           1
#define IS_AIGO_SUPPORT_TIME            1
#define IS_AIGO_SUPPORT_MUTEX           1
#define IS_AIGO_SUPPORT_THREAD          1


#if IS_AIGO_SUPPORT_THREAD
/// 创建新线程
#define AIGO_RUN_NEW_THREAD(thd, name_s, cb, prio, stack_s)  \
    RUN_NEW_THREAD(thd, name_s, cb, prio, stack_s) 
    
/// 退出线程
#define AIGO_EXIT_THREAD(thd) EXIT_THREAD(thd)
/// 获取线程名称
#define AIGO_THREAD_NAME_GET(thd)   THREAD_NAME_GET(thd)
/// 挂起线程
#define AIGO_THREAD_SUSPEND(thd)   THREAD_SUSPEND(thd)
/// 恢复线程
#define AIGO_THREAD_RESUME(thd)    THREAD_RESUME(thd)
#else
#define AIGO_RUN_NEW_THREAD(thd, name_s, cb, prio, stack_s) do { ; } while (0)
#define AIGO_EXIT_THREAD(thd) do { ; } while (0)
#define AIGO_THREAD_NAME_GET(thd) do { ; } while (0)
#define AIGO_THREAD_SUSPEND(thd) do { ; } while (0)
#define AIGO_THREAD_RESUME(thd) do { ; } while (0)
#endif

// 消息队列宏定义.
#if IS_AIGO_SUPPORT_QUEUE
#define AIGO_CREATE_MSG_TYPE_QUEUE(msg_id_ptr, queue_num, _size) \
    CREATE_MSG_TYPE_QUEUE(msg_id_ptr, queue_num, _size)
#define AIGO_MSG_QUEUE_SEND(msg_id_ptr, msg_data, timeout) \
    MSG_QUEUE_SEND(msg_id_ptr, msg_data, timeout)
#define AIGO_MSG_QUEUE_RECV(msg_id_ptr, msg_data, timeout) \
    MSG_QUEUE_RECV(msg_id_ptr, msg_data, timeout)
#define AIGO_MSG_QUEUE_RESET(msg_id_ptr) \
    MSG_QUEUE_RESET(msg_id_ptr)
#define AIGO_DELETE_MSG_QUEUE(msg_id_ptr) \
    DELETE_MSG_QUEUE(msg_id_ptr)    
#else
#define AIGO_CREATE_MSG_QUEUE(msg_id_ptr, queue_num) do { ; } while (0)
#define AIGO_CREATE_MSG_TYPE_QUEUE(msg_id_ptr, queue_num, _size) do { ; } while (0)
#define AIGO_MSG_QUEUE_SEND(msg_id_ptr, msg_data, timeout) do { ; } while (0)
#define AIGO_MSG_QUEUE_RECV(msg_id_ptr, msg_data, timeout) do { ; } while (0)
#define AIGO_MSG_QUEUE_RESET(msg_id_ptr)              do { ; } while (0)
#define AIGO_DELETE_MSG_QUEUE(msg_id_ptr)                  do { ; } while (0)
#endif

/// 定时器宏定义.
#if IS_AIGO_SUPPORT_TIME
#define AIGO_CREATE_TIMER(timer_id_ptr, name_s, user_data, type, cb) \
    CREATE_TIMER(timer_id_ptr, name_s, user_data, type, (os_ptimer)cb)
#define AIGO_START_TIMER(timer_id, period)   START_TIMER(timer_id, period)
#define AIGO_STOP_TIMER(timer_id)            STOP_TIMER(timer_id)
#define AIGO_DELETE_TIMER(timer_id)          DELETE_TIMER(timer_id)
#else
#define AIGO_CREATE_TIMER(timer_id_ptr, name_s, user_data, type, cb)   do { ; } while (0)
#define AIGO_START_TIMER(timer_id, period)   do { ; } while (0)
#define AIGO_STOP_TIMER(timer_id)            do { ; } while (0)
#define AIGO_DELETE_TIMER(timer_id)          do { ; } while (0)
#endif

/// 互斥锁宏定义.
#if IS_AIGO_SUPPORT_MUTEX
#define AIGO_MUTEX_INIT_HG(x, name)          MUTEX_INIT_HG(x, TO_STR(name))
#define AIGO_MUTEX_LOCK_HG(x, timeout)       MUTEX_LOCK_HG(x, timeout)
#define AIGO_MUTEX_UNLOCK_HG(x)              MUTEX_UNLOCK_HG(x)
#define AIGO_MUTEX_DELETE_HG(x)              MUTEX_DELETE_HG(x)

#define AIGO_MUTEX_INIT_HOLD(x, name, max_hold_time_s) \
        MUTEX_INIT_HOLD(x, name, max_hold_time_s)

#define AIGO_MUTEX_INIT(x, name)             AIGO_MUTEX_INIT_HOLD(x, name, 0)
#define AIGO_MUTEX_LOCK(x)                   AIGO_MUTEX_LOCK_HG(x, OS_WAIT_FOREVER)
#define AIGO_MUTEX_UNLOCK(x)                 AIGO_MUTEX_UNLOCK_HG(x)

#define AIGO_MUTEX_DELETE(x) \
    MUTEX_DELETE(x)

#else
#define AIGO_MUTEX_INIT_HG(x, name)  do { ; } while (0)
#define AIGO_MUTEX_INIT(x, name)     do { ; } while (0)
#define AIGO_MUTEX_LOCK(x)           do { ; } while (0)
#define AIGO_MUTEX_UNLOCK(x)         do { ; } while (0)
#define AIGO_MUTEX_DELETE(x)         do { ; } while (0)
#endif


#elif AIGO_API_PLATFORM == AIGO_API_PLATFORM_WIN
#error "not support platform"
#else
#error "Unsupported platform"
#endif

#ifdef __cplusplus
}
#endif

#endif