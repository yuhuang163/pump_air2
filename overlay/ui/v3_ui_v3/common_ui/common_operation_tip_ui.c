/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define OPERATION_TIP_TEXT_COLOR                                    0xE6DFDF
#define OPERATION_TIP_TEXT_COUNT                                    2
#define OPERATION_STATUS_TEXT_COLOR                                 0xFFFFFF
#define OPERATION_STATUS_ARC_SIZE                                   (360)    // 圆弧尺寸
#define OPERATION_STATUS_ARC_WIDTH                                  (12)      // 圆弧粗细
#define OPERATION_STATUS_ARC_COLOR                                  (0xB25962) // 圆弧颜色（匹配吸奶器粉色）
#define OPERATION_STATUS_ARC_START_ANGLE                            (270)    
#define OPERATION_STATUS_ARC_FULL_SWEEP                             (360)    
#define OPERATION_STATUS_DISPLAY_STEP_TIME                          (100)
#define OPERATION_PAUSE_ICON_Y                                      (118)

typedef struct {    
    uint32_t start_time;              /// 开始时间
    uint16_t ring_progress;            /// 圆弧进度
    uint16_t ring_progress_max;       /// 圆弧进度最大值
    uint16_t target_time;             /// 倒计时目标时间 单位：秒
    uint8_t display_number;           /// 显示倒计时数字
    uint8_t force_update;
    uint8_t number_max;
    uint8_t rsv[13];
} common_op_tip_disp_info_t;

typedef struct {
    const char *name;
    uint16_t text_y;
    uint16_t text_opa; /// 文本不透明度
    uint32_t color;
} operation_text_info_t;

typedef struct {
    uint16_t text_y;
    uint16_t img_id;
} operation_icon_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static common_op_tip_disp_info_t *operation_info;

const static operation_text_info_t pump_pause_manual_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t pump_pause_timeout_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t pump_finish_manual_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t pump_finish_timeout_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t operation_success_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t operation_success_preset_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t operation_failed_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t level_adjust_waring_infos[OPERATION_TIP_TEXT_COUNT] = {
};

const static operation_text_info_t pumping_only_infos[OPERATION_TIP_TEXT_COUNT] = {
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void common_operation_tip_text_update(ui_manager_context_t *thiz, const operation_text_info_t *text_info)
{
    EXCEPTION_CHECK_VOID(text_info);
    
    if (operation_info->force_update == false) {
        return;
    }


}

static void common_operation_tip_icon_update(ui_manager_context_t *thiz, uint16_t img_id, uint16_t y_offset)
{
    if (operation_info->force_update == false) {
        return;
    }

}

static void common_operation_arc_update(ui_manager_context_t *thiz)
{
    uint16_t ring_progress = get_timestamp_ms(operation_info->start_time) / OPERATION_STATUS_DISPLAY_STEP_TIME;
    if (operation_info->force_update == false && operation_info->ring_progress == ring_progress) {
        return;
    }

}

static void common_operation_num_update(ui_manager_context_t *thiz)
{
    // if (operation_info->number == NULL || !lv_obj_is_valid(operation_info->number)) {
    //     operation_info->number = lv_label_create(operation_info->root);
    //     EXCEPTION_CHECK_VOID(operation_info->number);
    // }
    // char num_buf[8] = {0};
    // uint8_t num = get_timestamp_ms(operation_info->start_time) / 1000;
    // uint8_t show_num = (num > operation_info->number_max ? 0 : operation_info->number_max - num);
    // if (operation_info->force_update == false &&
    //     operation_info->display_number == show_num) {
    //     return;
    // }

}

static void common_operation_tip_mask_layer_update(ui_manager_context_t *thiz)
{
    if (operation_info->force_update == false) {
        return;
    }

}

static const char *common_operation_tip_name_get(ui_manager_context_t *thiz, uint16_t popup_type)
{
    switch (popup_type) {
        case ID_POPUP_UI_PUMP_FINISH_TIMEOUT:
            return "pump Ending";
        case ID_POPUP_UI_PUMP_FINISH_MANUAL:
            return "pump Manual Ending";
        case ID_POPUP_UI_PUMP_PAUSE_TIMEOUT:
            return "pump 30 Minutes Pausing";
        case ID_POPUP_UI_PUMP_PAUSE_MANUAL:
            return "pump Manual Pausing";
        case ID_POPUP_UI_OP_SUCCESS:
            return "operation success";
        case ID_POPUP_UI_OP_SUCCESS_PRESET:
            return "operation success preset";
        case ID_POPUP_UI_OP_FAILED:
            return "operation failed";
        case ID_POPUP_UI_LEVEL_ADJUST_WARNING:
            return "pump level adjust error";
        case ID_POPUP_UI_ONLY_AVAILABLE_IN_PUMPING:
            return "no pumping";
        default:
           return "Unknown";
    }
}

static void common_operation_tip_display_complete(ui_manager_context_t *thiz, uint16_t popup_type)
{
    // uint32_t time_diff = get_timestamp_ms(operation_info->start_time) + OPERATION_STATUS_DISPLAY_STEP_TIME;
    // if (time_diff >= operation_info->target_time) {
    //     ui_animation_play_t *ani = &thiz->priv.play_info;
    //     EXCEPTION_CHECK_VOID(ani);
    //     LOG_UI("%s display complete", common_operation_tip_name_get(thiz, popup_type));
    //     ani->progress_cb(thiz, popup_type, ANI_POPUP_DISPLAY_FINISH);
    // }
}

static void common_operation_ui_update(ui_manager_context_t *thiz, popup_ui_id_e popup_type)
{
    // switch (popup_type) {
    //     case ID_POPUP_UI_PUMP_PAUSE_TIMEOUT:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, pump_pause_timeout_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_PUMP_PAUSE_PROMPT_ICON, 118);
    //         break;
    //     case ID_POPUP_UI_PUMP_PAUSE_MANUAL:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, pump_pause_manual_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_PUMP_PAUSE_PROMPT_ICON, 118);
    //         break;
    //     case ID_POPUP_UI_PUMP_FINISH_TIMEOUT:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, pump_finish_timeout_infos);
    //         common_operation_arc_update(thiz);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_PUMP_FINISH_MANUAL:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, pump_finish_manual_infos);
    //         common_operation_arc_update(thiz);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_OP_SUCCESS:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, operation_success_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_ALLOW_ICON, 130);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_OP_SUCCESS_PRESET:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, operation_success_preset_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_ALLOW_ICON, 130);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_OP_FAILED:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, operation_failed_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_BAN_ICON, 130);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_LEVEL_ADJUST_WARNING:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, level_adjust_waring_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_WARNING_ICON, 130);
    //         break;
    //     case ID_POPUP_UI_ONLY_AVAILABLE_IN_PUMPING:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_tip_text_update(thiz, pumping_only_infos);
    //         common_operation_tip_icon_update(thiz, RESOURCE_ID_WARNING_ICON, 130);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_AIRPLANE_MODE_PROMPT:
    //         common_operation_tip_mask_layer_update(thiz);
    //         common_operation_arc_update(thiz);
    //         common_operation_num_update(thiz);
    //         common_operation_tip_display_complete(thiz, popup_type);
    //         break;
    //     default:
    //         break;
    // }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

void ui_common_operation_tip_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (operation_info == NULL) {
    //     return;
    // }
    // common_ui_img_resource_deinit(thiz, &operation_info->img_info);
    // LVGL_OBJ_DELETE(operation_info->root);
    // u_free(operation_info);
    // operation_info = NULL;
}

void ui_common_operation_tip_process(ui_manager_context_t *thiz, popup_ui_id_e type)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (operation_info == NULL || operation_info->root == NULL ||
    //     !lv_obj_is_valid(operation_info->root)) {
    //     return;
    // }
    // common_operation_ui_update(thiz, type);
    // operation_info->force_update = false;
}
