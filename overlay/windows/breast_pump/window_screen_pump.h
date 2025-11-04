#ifndef __WINDOW_SCREEN_PUMP_H__
#define __WINDOW_SCREEN_PUMP_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" // IWYU pragma: keep
#include "windows_map_define.h" // IWYU pragma: keep
#include "input_key_code.h" // IWYU pragma: keep

/// 判断泵奶时间
#define IS_PUMPTIME_LESS_THEN_30S(result)      ((result)->real_valid_time_sec < 30 )
#define IS_PUMPTIME_LESS_THEN_120S(result)     ((result)->real_valid_time_sec < 120)
/// 循环显示超时时间
#define LOOP_SHOW_TIMEOUT                      (5000) /// 5s

typedef enum {
    HDR_ACTION_START_PUMP = 100,
    HDR_ACTION_CHANGE_PUMP_STATUS,
    HDR_ACTION_LOOP_SHOW,
    HDR_ACTION_SHOW_ONCE,
    HDR_ACTION_REGISTER_KEYEVENT,
    HDR_ACTION_SHOW_THEMES_ANI,
    HDR_ACTION_RECOVER_DEFAULT_ANI,
    HDR_ACTION_RESET_PLAY_ANI_FPS,
    HDR_ACTION_PROGRESS_START,
    HDR_ACTION_PROGRESS_STOP,
    HDR_ACTION_PUMP_COMPLETE,
    HDR_ACTION_GOTO_CHARGE_UI,
    HDR_ACTION_MAX
} pump_ui_hdr_action_e;

typedef enum {
    LOOP_SHOW_SWITCH_MODE = 0,
    LOOP_SHOW_PUMPING,
    LOOP_SHOW_RESULT_RISK_WARN, /// 风险警告
    LOOP_SHOW_RESULT_RISK_COMPARE, /// 风险比较
    LOOP_SHOW_THEMES_ANI, /// 主题动画
    LOOP_SHOW_SETTING_ENTER, /// 设置入口
    LOOP_SHOW_MAX
} pump_loop_show_type_e;

typedef struct ui_pump_info {    
    uint8_t common_mode; /// 普通泵奶模式.
    uint8_t status; /// 泵奶状态.
    uint8_t breastfeeding_mode; /// 追奶模式.
    uint8_t control_type; /// 控制类型. 0: 左, 1: 右. 2: 双侧.
} ui_pump_info_t;

typedef struct ui_proc_pump {    
    uint32_t ani_timer_cnt;
    window_context_t w_ctx;
    // ui_page_handle_t bridge;
    broadcast_obj_t receiver;
    uint32_t margic;
    breast_pump_result_t pump_result; /// 泵奶结果.
    uint8_t need_start_pump; /// 是否需要启动泵奶.
    uint8_t ui_show_state; /// 当前UI显示状态.
    uint8_t user_pause; /// 用户是否暂停.
    uint8_t goto_charge_ui_flag; /// 是否需要跳转到充电界面.
    uint8_t is_mode_changed_by_app; /// 是否是从app中切换模式.
    uint8_t is_mode_changed_by_device; /// 是否是从设备中切换模式.
    uint8_t current_ui_mode; /// 当前UI模式索引.
    uint8_t is_resume_from_pause; /// 是否是从暂停中恢复.
    uint8_t mode_switch_ani_flag; /// 模式切换动画标志.
    uint8_t themes_ani_play_start; /// 主题动画播放标志.
    ui_pump_info_t info; /// 当前泵奶信息.
    pump_setting_t setting; /// 当前泵奶设置信息.
} ui_proc_pump_t;

/// 泵奶模式映射表
typedef struct {
    uint16_t res_id; /// 资源ID.
    uint16_t mode_id; /// 泵奶模式ID.
} ui_pump_mode_map_t;

/// 泵奶模式切换超时时间
#define PUMP_MODE_CHANGE_TIMEOUT (2000)

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void animation_progress_cb(void *user_data, int index, int state);
void start_pump_ui_animation(ui_proc_pump_t *proc);
void draw_buffer_reset(ui_proc_pump_t *proc);
int get_ui_pump_mode_index(int pump_mode);
void init_themes_pump_img_res_id(ui_proc_pump_t *proc);
void generate_pump_mode_map_list(ui_proc_pump_t *proc);
void show_pump_result_loop(ui_proc_pump_t *proc);
int switch_pump_common_mode_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, breast_pump_info_t *pump_info);
int switch_pump_power_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump);
int switch_pump_status_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, int status);
int switch_pump_frequency_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, breast_pump_info_t *pump_info);
int switch_pump_breastfeeding_mode_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, breast_pump_info_t *pump_info);
int switch_pump_control_type_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, uint8_t control_type);
int switch_pump_level_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, breast_pump_info_t *pump_info);
void show_pump_progress_time_policy(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump);

#ifdef __cplusplus
}
#endif

#endif 


