#ifndef __BREAST_PUMP_MANAGER_API_H__
#define __BREAST_PUMP_MANAGER_API_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "broadcast_manager_api.h"
#include "persist_storage.h" // IWYU pragma: keep

typedef enum {
    BREAST_PUMP_IDLE = 0,
    BREAST_PUMP_START,
    BREAST_PUMP_PAUSE,
    BREAST_PUMP_CONTINUE,
    BREAST_PUMP_FINISH,
    BREAST_PUMP_FACTORY
} breast_pump_status_e;

typedef struct {
    uint8_t breast_pump_state;
    uint32_t real_valid_time_sec;        ///  实际有效泵奶时间 (s)
    uint32_t expected_valid_time_sec;    ///  目标有效泵奶时间(max time) (s)
    uint32_t elapse_time_sec;            ///  自然流逝时间.
    uint32_t expected_elapse_time_sec;   ///  目标自然流逝时间
    uint32_t work_time_total;
} breast_pump_result_t;

typedef void (*on_breast_pump_result_cb_ptr)(void *user_data, breast_pump_result_t *result);

typedef struct {
    uint8_t level;
    uint8_t speed;
} breast_pump_unit_t;

typedef struct {
    uint8_t status;
    uint8_t do_work;
    uint8_t mode;
    breast_pump_unit_t unit_info[BREAST_PUMP_NUM];
} breast_pump_info_t;

struct peripheral_bus_context;
struct motor_manager_context;
struct storage_context_struct;

typedef struct breast_pump_manager_context {
///  private
    struct __breast_pump_manager_private {
        os_mutex_id m_lock;
        os_mutex_id breast_pump_state_lock;
        os_mutex_id breast_pump_mode_lock;
        os_timer_id breast_pump_work_timer;
        breast_pump_result_t m_result;
        breast_pump_info_t m_info;
        thread_context_t breast_pump_thd;
        list_head_t client_list;
        broadcast_obj_t m_bdc;
        struct storage_manager_context *st;
        struct peripheral_bus_context *pbus;
        struct motor_manager_context *motor;
    } priv;

///  public
    int (*is_on_working)(void);
    void (*reset_breast_pump_result)(void);
    int (*add_breast_pump_result_listener)(void *user_data, on_breast_pump_result_cb_ptr cb);
    void (*remove_breast_pump_result_listener)(on_breast_pump_result_cb_ptr cb);
    int (*get_breast_pump_mode)(void);
    void (*set_breast_pump_mode)(uint32_t mode);
    void (*get_breast_pump_unit_info)(uint8_t id, breast_pump_unit_t *unit_info);
    void (*set_breast_pump_unit_info)(uint8_t id, breast_pump_unit_t *unit_info);
    void (*set_breast_pump_status)(int status);
    int (*get_breast_pump_status)(void);
    int (*get_breast_pump_status_async)(void);
    int (*get_last_breast_pump_result)(breast_pump_result_t *result, uint8_t breast_pump_data_id);
    const breast_pump_result_t *(*get_curr_breast_pump_result)(void);

    ///  外部挂接点.
    struct __breast_pump_hook {
        int (*make_progress_route)(struct breast_pump_manager_context *ctx, algo_breast_pump_data_t *algo_brush, int args0, int args1);
        int (*breast_pump_completed_route)(struct breast_pump_manager_context *ctx);
        int (*start_breast_pump_work_route)(struct breast_pump_manager_context *ctx, int breast_pump_state);
        int (*stop_breast_pump_work_route)(struct breast_pump_manager_context *ctx, int breast_pump_state);
        uint32_t (*get_last_breast_pump_complete_time)(struct breast_pump_manager_context *ctx);
    } *hook;
} breast_pump_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 


