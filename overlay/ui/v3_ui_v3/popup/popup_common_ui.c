/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_popup.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define POPUP_TIP_TEXT_COLOR                                    0xE6DFDF
#define POPUP_TIP_TEXT_COUNT                                    2
#define POPUP_STATUS_TEXT_COLOR                                 0xFFFFFF
#define POPUP_STATUS_ARC_SIZE                                   (360)    // 圆弧尺寸
#define POPUP_STATUS_ARC_WIDTH                                  (12)      // 圆弧粗细
#define POPUP_STATUS_ARC_COLOR                                  (0xB25962) // 圆弧颜色（匹配吸奶器粉色）
#define POPUP_STATUS_ARC_START_ANGLE                            (270)    
#define POPUP_STATUS_ARC_FULL_SWEEP                             (360)    
#define POPUP_STATUS_DISPLAY_STEP_TIME                          (100)
#define POPUP_PAUSE_ICON_Y                                      (118)

typedef struct {    
    uint32_t start_time;              /// 开始时间
    uint16_t ring_progress;            /// 圆弧进度
    uint16_t ring_progress_max;       /// 圆弧进度最大值
    uint16_t target_time;             /// 倒计时目标时间 单位：秒
    uint8_t display_number;           /// 显示倒计时数字
    uint8_t force_update;
    uint8_t number_max;         
    uint8_t rsv[13];
} popup_tip_disp_info_t;

typedef struct {
    const char *name;
    uint16_t text_y;
    uint16_t text_opa; /// 文本不透明度
    uint32_t color;
} popup_text_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static popup_tip_disp_info_t *popup_info;

const static popup_text_info_t low_power_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Powering off in", 111, &lv_font_oppo_sans_16_medium, LV_OPA_100, 0xC74B57},
    // { "", 0, &lv_font_oppo_sans_14_medium, LV_OPA_70, 0xC74B57},
};

const static popup_text_info_t will_power_off_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Shut down soon", 111, &lv_font_oppo_sans_16_medium, LV_OPA_100, 0xC74B57},
    // { "", 0, &lv_font_oppo_sans_14_medium, LV_OPA_70, 0xC74B57},
};

const static popup_text_info_t ble_connect_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Device connected", 210, &lv_font_oppo_sans_16_medium, LV_OPA_100, POPUP_STATUS_TEXT_COLOR},
    // { "", 0, &lv_font_oppo_sans_16_medium, LV_OPA_70, POPUP_STATUS_TEXT_COLOR},
};

const static popup_text_info_t ble_disconnect_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Device disconnected", 210, &lv_font_oppo_sans_16_medium, LV_OPA_100, POPUP_STATUS_TEXT_COLOR},
    // { "", 0, &lv_font_oppo_sans_16_medium, LV_OPA_70, POPUP_STATUS_TEXT_COLOR},
};

const static popup_text_info_t ble_connecting_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Connecting", 210, &lv_font_oppo_sans_16_medium, LV_OPA_100, POPUP_STATUS_TEXT_COLOR},
    // { "", 0, &lv_font_oppo_sans_16_medium, LV_OPA_70, POPUP_STATUS_TEXT_COLOR},
};

const static popup_text_info_t device_bind_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "Paired successfully", 210, &lv_font_oppo_sans_16_medium, LV_OPA_100, POPUP_STATUS_TEXT_COLOR},
    // { "", 0, &lv_font_oppo_sans_16_medium, LV_OPA_70, POPUP_STATUS_TEXT_COLOR},
};

const static popup_text_info_t device_unbind_infos[POPUP_TIP_TEXT_COUNT] = {
    // { "", 0, &lv_font_oppo_sans_20_medium, LV_OPA_100, POPUP_STATUS_TEXT_COLOR},
    // { "", 0, &lv_font_oppo_sans_16_medium, LV_OPA_70, POPUP_STATUS_TEXT_COLOR},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void popup_window_ui_root_init(ui_manager_context_t *thiz)
{

}

static void popup_window_ui_text_update(ui_manager_context_t *thiz, const popup_text_info_t *text_info)
{
    EXCEPTION_CHECK_VOID(text_info);
    
    if (popup_info->force_update == false) {
        return;
    }

}

static void popup_window_ui_icon_update(ui_manager_context_t *thiz, uint16_t img_id, uint16_t y_offset)
{
    if (popup_info->force_update == false) {
        return;
    }

}

static void popup_window_ui_arc_update(ui_manager_context_t *thiz)
{

}

static void popup_window_ui_num_update(ui_manager_context_t *thiz)
{

}

static void popup_window_ui_mask_layer_update(ui_manager_context_t *thiz)
{
    if (popup_info->force_update == false) {
        return;
    }


}

static const char *popup_window_ui_name_get(ui_manager_context_t *thiz, uint16_t popup_type)
{
    switch (popup_type) {
        case ID_POPUP_UI_BT_CONNECT:
            return "ble connect";
        case ID_POPUP_UI_BT_DISCONNECT:
            return "ble disconnect";
        case ID_POPUP_UI_BT_CONNECTING:
            return "ble connecting";
        case ID_POPUP_UI_DEVICE_BIND:
            return "device bind";
        case ID_POPUP_UI_DEVICE_UNBIND:
            return "device unbind";
        case ID_POPUP_UI_COUNT_DOWN:
            return "count down";
        case ID_POPUP_UI_LOW_POWER:
            return "low power";
        case ID_POPUP_UI_WILL_POWER_OFF:
            return "will power off";
        default:
           return "Unknown";
    }
}

static void popup_window_ui_display_complete(ui_manager_context_t *thiz, uint16_t popup_type)
{
    // uint32_t time_diff = get_timestamp_ms(popup_info->start_time) + POPUP_STATUS_DISPLAY_STEP_TIME;
    // if (time_diff >= popup_info->target_time) {
    //     ui_animation_play_t *ani = &thiz->priv.overlay_play_info;
    //     EXCEPTION_CHECK_VOID(ani);
    //     LOG_UI("%s display complete", popup_window_ui_name_get(thiz, popup_type));
    //     ani->progress_cb(thiz, popup_type, ANI_POPUP_DISPLAY_FINISH);
    // }
}

static void popup_window_ui_update(ui_manager_context_t *thiz, popup_ui_id_e popup_type)
{
    // switch (popup_type) {
    //     case ID_POPUP_UI_BT_CONNECT:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, ble_connect_infos);
    //         popup_window_ui_icon_update(thiz, RESOURCE_ID_BLE_CONNECT_POPUP_ICON, 130);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_BT_DISCONNECT:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, ble_disconnect_infos);
    //         popup_window_ui_icon_update(thiz, RESOURCE_ID_BLE_DISCONNECT_POPUP_ICON, 130);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_BT_CONNECTING:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, ble_connecting_infos);
    //         popup_window_ui_icon_update(thiz, RESOURCE_ID_BLE_DISCONNECT_POPUP_ICON, 130);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_DEVICE_BIND:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, device_bind_infos);
    //         popup_window_ui_icon_update(thiz, RESOURCE_ID_BIND_ICON, 130);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_DEVICE_UNBIND:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, device_unbind_infos);
    //         popup_window_ui_icon_update(thiz, RESOURCE_ID_UNBIND_ICON, 130);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_COUNT_DOWN:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_arc_update(thiz);
    //         popup_window_ui_num_update(thiz);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_LOW_POWER:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, low_power_infos);
    //         popup_window_ui_arc_update(thiz);
    //         popup_window_ui_num_update(thiz);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     case ID_POPUP_UI_WILL_POWER_OFF:
    //         popup_window_ui_mask_layer_update(thiz);
    //         popup_window_ui_text_update(thiz, will_power_off_infos);
    //         popup_window_ui_arc_update(thiz);
    //         popup_window_ui_num_update(thiz);
    //         popup_window_ui_display_complete(thiz, popup_type);
    //         break;
    //     default:
    //         break;
    // }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void popup_window_ui_prepare(ui_manager_context_t *thiz, popup_ui_id_e type, uint16_t time)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (popup_info == NULL) {
    //     popup_info = (popup_tip_disp_info_t *)u_malloz(sizeof(popup_tip_disp_info_t));
    //     EXCEPTION_CHECK_VOID(popup_info);
    // }
    // popup_window_ui_root_init(thiz);
    // popup_info->ring_progress = 0;
    // popup_info->target_time = time;
    // popup_info->number_max = time / 1000;
    // popup_info->ring_progress_max = (time / POPUP_STATUS_DISPLAY_STEP_TIME) - 2;
    // popup_info->force_update = true;
    // popup_info->start_time = get_timestamp();
    LOG_UI("popup: %s, time:%d", popup_window_ui_name_get(thiz, type), time);
}

void popup_window_ui_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz);
//     if (popup_info == NULL) {
//         return;
//     }
//     common_ui_img_resource_deinit(thiz, &popup_info->img_info);
//     LVGL_OBJ_DELETE(popup_info->root);
//     u_free(popup_info);
//     popup_info = NULL;
}

int popup_window_ui_process(ui_manager_context_t *thiz, popup_ui_id_e type)
{
    // EXCEPTION_CHECK_INT(thiz && popup_info && popup_info->root);
    // // ui_proc_popup_t *proc = (ui_proc_popup_t *) thiz->priv.overlay_play_info.user_data;
    // popup_window_ui_update(thiz, type);
    // popup_info->force_update = false;
    return 0;
}