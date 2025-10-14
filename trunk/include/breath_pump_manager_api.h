#ifndef __BREATH_PUMP_MANAGER_API_H__
#define __BREATH_PUMP_MANAGER_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "broadcast_manager_api.h"
#include "persist_storage.h"

typedef enum {
    BREATH_PUMP_IDLE = 0,
    BREATH_PUMP_START,
    BREATH_PUMP_PAUSE,
    BREATH_PUMP_CONTINUE,
    BREATH_PUMP_FINISH,
    BREATH_PUMP_FACTORY
} breath_pump_status_e;

typedef enum {
    ONESHOT_MASK_INVALID,
    ONESHOT_MASK_OVERPRESS,
} oneshot_mask_e;

typedef struct {

    uint8_t oneshot_mask;     // 仅执行单次有效标记. oneshot_mask_e
    uint8_t breath_pump_state;
    uint32_t real_valid_time_sec;        ///  实际有效刷牙时间 (s)
    uint32_t expected_valid_time_sec;    ///  目标有效刷牙时间(max time) (s)
    uint32_t elapse_time_sec;            ///  自然流逝时间.
    uint32_t expected_elapse_time_sec;   ///  目标自然流逝时间
    uint32_t work_time_total;
} breath_pump_result_t;

typedef void (*on_breath_pump_result_cb_ptr)(void *user_data, breath_pump_result_t *result);

typedef struct {
    uint8_t status;
    uint8_t do_work;
    uint8_t mode;
    uint8_t level;
} breath_pump_info_t;

struct peripheral_bus_context;
struct motor_manager_context;
struct storage_context_struct;

typedef struct breath_pump_manager_context {
///  private
    struct __breath_pump_manager_private {
        os_mutex_id m_lock;
        os_mutex_id breath_pump_state_lock;
        os_mutex_id breath_pump_mode_lock;
        os_timer_id breath_pump_work_timer;
        breath_pump_result_t m_result;
        breath_pump_info_t breath_pump_info;
        thread_context_t breath_pump_thd;
        list_head_t client_list;
        broadcast_obj_t m_bdc;
        struct storage_manager_context *st;
        struct peripheral_bus_context *pbus;
        struct motor_manager_context *motor;
    } priv;

///  public
    int (*is_on_working)(void);
    void (*reset_breath_pump_result)(void);
    int (*add_breath_pump_result_listener)(void *user_data, on_breath_pump_result_cb_ptr cb);
    void (*remove_breath_pump_result_listener)(on_breath_pump_result_cb_ptr cb);
    int (*get_breath_pump_mode)(void);
    void (*set_breath_pump_mode)(uint32_t mode);
    int (*get_breath_pump_level)(void);
    void (*set_breath_pump_level)(breath_pump_mode_e mode);
    
    void (*set_breath_pump_status)(int status);
    int (*get_breath_pump_status)(void);
    int (*get_breath_pump_status_async)(void);
    int (*get_last_breath_pump_result)(breath_pump_result_t *result, uint8_t breath_pump_data_id);
    const breath_pump_result_t *(*get_curr_breath_pump_result)(void);
    bool (*is_on_breath_pumping)(void);
    void (*dispatch_to_client)(uint32_t oneshot_mask);

    ///  外部挂接点.
    struct __breath_pump_hook {
        int (*make_progress_route)(struct breath_pump_manager_context *ctx, algo_breath_pump_data_t *algo_brush, int args0, int args1);
        int (*breath_pump_completed_route)(struct breath_pump_manager_context *ctx);
        int (*start_breath_pump_work_route)(struct breath_pump_manager_context *ctx, int breath_pump_state);
        int (*stop_breath_pump_work_route)(struct breath_pump_manager_context *ctx, int breath_pump_state);
        uint32_t (*get_last_breath_pump_complete_time)(struct breath_pump_manager_context *ctx);
    } *hook;
} breath_pump_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 


