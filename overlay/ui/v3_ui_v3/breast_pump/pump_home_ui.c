/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_pump.h" // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_COMP_BG                    (0)
#define HOME_MODE_MAX_COUNT                        (4)
#define HOME_BF_MODE_COUNT                         (6)
#define HOME_SPEED_MAX_COUNT                       (3)
#define HOME_TEXT_COUNT                            (3)
#define HOME_TIME_COLON_COUNT                      (2)
#define PUMP_ARC_COLOR                             (0xB3636D)
#define PUMP_LEVEL_CIRCLE_COLOR                    (0xFFFFFF) /// (0xFFEDEF)
#define PUMP_TIME_TEXT_COLOR                       (0xB25962)
#define MODE_TEXT_SEL_COLOR                        (0xFFFFFF)
#define MODE_TEXT_UNSEL_COLOR                      (0xFFE5E7)
#define PUMP_LEVEL_MAX                             (15)
#define PUMP_LEVEL_RING_SIZE                       (306)
#define PUMP_LEVEL_RING_R                          (PUMP_LEVEL_RING_SIZE / 2)
#define PUMP_LEFT_ARC_START_ANGLE                  (136 + 8)
#define PUMP_LEFT_ARC_END_ANGLE                    (224 + 12)
#define PUMP_RIGHT_ARC_START_ANGLE                 (316 - 12)
#define PUMP_RIGHT_ARC_END_ANGLE                   (404 - 8)
#define PUMP_LEVEL_ARC_WIDTH                       (6)
#define PUMP_LEVEL_CIRCLE_SIZE                     (9)
#define PUMP_SPEED_ICON_X                          (186)
#define PUMP_SPEED_ICON_Y                          (24)
#define HOME_CUSTOM_MODE_TEXT_Y                    (91 + 9)
#define HOME_CUSTOM_LEFT_TEXT_CENTER_X             (136 - 4)
#define HOME_CUSTOM_RIGHT_TEXT_CENTER_X            (224 + 2)
#define NEXT_RING_SIZE                             (309)
#define NEXT_RING_R                                (NEXT_RING_SIZE / 2 - 43)
#define NEXT_TEXT_CENTER_Y_OFFSET                  (60)

#define HOME_TIME_H_64_X                           (21) 
#define HOME_TIME_H_64_Y                           (131)
#define HOME_TIME_M_64_X                           (121 + 9)
#define HOME_TIME_M_64_Y                           HOME_TIME_H_64_Y
#define HOME_TIME_S_64_X                           (220 + 20) 
#define HOME_TIME_S_64_Y                           HOME_TIME_M_64_Y
#define HOME_HOURS_COLON_64_X                      (121)
#define HOME_HOURS_COLON_64_Y                      (126)
#define HOME_MINUTES_COLON_64_X                    (HOME_HOURS_COLON_64_X + 100)
#define HOME_MINUTES_COLON_64_Y                    (HOME_HOURS_COLON_64_Y)

#define HOME_TIME_M_96_X                           (46)
#define HOME_TIME_M_96_Y                           (123)
#define HOME_TIME_S_96_X                           (194)
#define HOME_TIME_S_96_Y                           HOME_TIME_M_96_Y
#define HOME_MINUTES_COLON_96_X                    (171)
#define HOME_MINUTES_COLON_96_Y                    (126)

typedef struct {    
    res_img_info_t speed_icon_info;
    res_img_info_t animation_info;
    res_img_info_t background_info;
    uint8_t force_update;
    uint8_t popup_ui_index; /// 当前弹窗索引.
    uint8_t percent;
    uint8_t is_charging;
    uint8_t mode;
    uint8_t sub_mode;
    uint8_t next_sub_mode;
    uint8_t speed;
    uint8_t left_level;
    uint8_t right_level;
    uint8_t hours;
    uint8_t hours_hidden; /// 隐藏小时标志
    uint8_t minutes;
    uint8_t minutes_size; /// 隐藏分钟标志
    uint8_t seconds;
    uint8_t seconds_size; /// 隐藏秒数标志
    uint8_t hour_colon_hidden; /// 隐藏时间冒号标志
    uint8_t mode_type;
    uint16_t animation_id;
    uint16_t animation_end_id;
} pump_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_HOME_COMP_BG
static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    // #include "comparison_png2txt/pump_home_360x360.txt"
    #include "comparison_png2txt/pump_home3_360x360.txt"
};
#endif

const static char *home_mode_names[HOME_MODE_MAX_COUNT] = {
    "Stimulate",
    "Expression",
    "Mixed",
    "Relax",
};

static const char *bf_mode_name[HOME_BF_MODE_COUNT] = {
    "Cozy Flow", /// 自然韵律模式
    "Milk Boost", /// 2+8奶量提升模式
    "Power Pumping", /// PP追奶模式
    "Milk Start", /// 开奶模式
    "Deep Emptying", /// 追加排空模式
    "Customize", /// 已使用过的个性化模式
};

const static uint16_t home_speed_icon_ids[HOME_SPEED_MAX_COUNT] = {
    // RESOURCE_ID_PUMP_SPEED_SLOW_ICON,
    // RESOURCE_ID_PUMP_SPEED_REGULAR_ICON,
    // RESOURCE_ID_PUMP_SPEED_BOOST_ICON,
};

static pump_display_info_t *pump_info;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_home_init(void)
{

}

static void pump_home_bg_update(ui_manager_context_t *thiz, uint8_t is_regular_mode)
{

}

static void pump_home_time_colon_update(uint16_t hold_time, uint16_t y_offset, bool is_hour_colon_hidden)
{

}

static void pump_home_time_h_update(uint8_t hours, uint16_t y_offset, bool is_hour_hidden)
{

}

static void pump_home_time_m_update(uint8_t minutes, uint16_t y_offset, uint8_t minutes_size)
{

}

static void pump_home_time_s_update(uint8_t seconds, uint16_t y_offset, uint8_t seconds_size)
{

}

static void pump_home_time_update(ui_manager_context_t *thiz, uint16_t hold_time, bool is_regular_mode)
{

}

static int pump_home_left_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{

    return 0;
}

static int pump_home_left_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static int pump_home_left_level_circle_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static void pump_home_left_level_update(ui_manager_context_t *thiz, uint8_t level)
{

}

static int pump_home_right_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{
    return 0;
}

static int pump_home_right_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{
    return 0;
}

static int pump_home_right_level_circle_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static void pump_home_right_level_update(ui_manager_context_t *thiz, uint8_t level)
{

}

static void pump_home_speed_label_update(ui_manager_context_t *thiz, uint8_t speed)
{

}

static void pump_home_common_mode_update(ui_manager_context_t *thiz, uint8_t mode_index)
{

}

static void pump_home_custom_mode_update(ui_manager_context_t *thiz, uint8_t mode_index, 
    uint8_t sub_mode, uint8_t next_sub_mode)
{    

}

static void pump_home_animation_update(ui_manager_context_t *thiz, uint8_t mode_type)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{

}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(pump_info);
    // common_ui_img_resource_deinit(thiz, &pump_info->speed_icon_info);
    // common_ui_img_resource_deinit(thiz, &pump_info->animation_info);
    // common_ui_img_resource_deinit(thiz, &pump_info->background_info);
    // ble_ui_deinit(thiz);
    // ui_common_operation_tip_remove(thiz);
    // LVGL_OBJ_DELETE(pump_info->root);
    // u_free(pump_info);
    // pump_info = NULL;
}

int ui_display_pump_home_screen_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz);
    // EXCEPTION_CHECK_INT(pump_info && pump_info->root);
    // ui_proc_pump_t *proc = (ui_proc_pump_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc && proc->pump && proc->motor && proc->ble);
    // const pump_result_info_t *result_info = NULL;
    // READ_PUMP_RESULT_INFO(proc->pump, BREAST_PUMP_RESULT_PUMPING, &result_info);
    // EXCEPTION_CHECK_INT(result_info);
    // uint16_t hold_time = result_info->work_total_time / 1000;
    // pump_setting_t setting;
    // int time_index = 0;
    // READ_PUMP_MODE_STEP_INDEX(proc->pump, time_index);
    // READ_PUMP_SETTING_INFO(proc->pump, time_index, setting);
    // int pump_mode = 0;
    // READ_PUMP_WORK_MODE(proc->pump, pump_mode);
    // uint8_t is_regular_mode = PUMP_IS_COMMON_MODE(pump_mode);
    // pump_home_bg_update(thiz, is_regular_mode);
    // ble_ui_update(thiz, pump_info->root, proc->ble, PUMP_HOME_BLE_UI_X, PUMP_HOME_BLE_UI_Y);
    // pump_home_time_update(thiz, hold_time, is_regular_mode);
    // uint8_t sub_mode = 0;
    // uint8_t next_sub_mode = 0;
    // if (proc->motor->get_runtime_mode_type) {
    //     sub_mode = proc->motor->get_runtime_mode_type(false);
    //     next_sub_mode = proc->motor->get_runtime_mode_type(true);
    // }
    // pump_home_animation_update(thiz, sub_mode);
    // if (is_regular_mode) {
    //     pump_home_common_mode_update(thiz, pump_mode);  
    // } else {
    //     pump_home_custom_mode_update(thiz, pump_mode, sub_mode, next_sub_mode);
    // }
    // pump_home_left_level_update(thiz, setting.level[0]);
    // pump_home_right_level_update(thiz, setting.level[1]);
    // pump_home_speed_label_update(thiz, setting.speed);
    // pump_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> pump home fps: %d, mode:%d/%d", thiz->priv.fps_count, pump_mode, proc->popup_ui_type);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
