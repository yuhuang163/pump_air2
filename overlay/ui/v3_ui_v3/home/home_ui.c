/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_home.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_ROOT_HOME_COMP_BG                (0)

#define IDLE_LABEL_Y_OFFSET                         (18)
#define IDLE_LABEL_AREA_ICON_X                      (0)
#define IDLE_LABEL_AREA_ICON_Y                      (84 + IDLE_LABEL_Y_OFFSET)
#define IDLE_BUTTON_ICON_X                          (70)
#define IDLE_BUTTON_ICON_Y                          (119 + IDLE_LABEL_Y_OFFSET)

#define IDLE_LABEL_TEXT_X                           (98)
#define IDLE_LABEL_TEXT_Y                           (114 + IDLE_LABEL_Y_OFFSET)
#define IDLE_TEXT_COLOR                             (0xB25962)

#define HOME_IDLE_ANIMATION_X                       (0)
#define HOME_IDLE_ANIMATION_Y                       (162 + 4)

#define HOME_LIGHT_ANIMATION_X                      (0)
#define HOME_LIGHT_ANIMATION_Y                      (162 + 4)

#define HOME_CLOCK_HOUR_X                           (44 - 1)
#define HOME_CLOCK_MINUTE_X                         (192 - 1)
#define HOME_CLOCK_TIME_Y                           (141 + 7)
#define HOME_CLOCK_COLON_X                          (174 - 4)
#define HOME_CLOCK_COLON_Y                          (141 + 20)

#define HOME_WITH_CLOCK_LABLE_Y_OFFSET              (29)
#define HOME_CLOCK_TIP_LABLE_Y_OFFSET               (7)
#define HOME_RECORD_TEXT_X_OFFSET                   (14)
#define HOME_RECORD_TEXT_Y_OFFSET                   (20)
#define HOME_LIGHT_TIP_LABLE_Y_OFFSET               (28)

typedef struct {
    uint8_t home_ui_type;
    uint32_t timestamp_offset;
    uint32_t record_hour;
    uint8_t record_minute;
    uint8_t clock_hour_value;
    uint8_t clock_minute_value;
    uint16_t animation_index;
    uint8_t popup_ui_index;
    uint8_t force_update;
    uint8_t rsv[9];
} home_display_info_t;


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static home_display_info_t *home_info;
#if IS_SUPPORT_ROOT_HOME_COMP_BG
static PSRAM_NC_DATA_LOC uint8_t home_bg_data[] = {
    #include "comparison_png2txt/home_ip1_360x360.txt"
};
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void home_root_init(void)
{

}

static void home_root_bg_update(ui_manager_context_t *thiz)
{
}

static void home_button_ui_update(ui_manager_context_t *thiz, uint16_t x_offset, uint16_t y_offset, bool is_show)
{

}

static void home_label_ui_update(ui_manager_context_t *thiz, uint16_t img_id, uint16_t x_offset, uint16_t y_offset, bool is_show)
{

}

static void home_root_animation_update(ui_manager_context_t *thiz, 
    uint16_t start_resource_id, uint16_t animation_size, uint16_t x_offset, uint16_t y_offset)
{  

}

static void home_record_time_ui_update(ui_manager_context_t *thiz, uint32_t timestamp_offset,
    uint16_t x_offset, uint16_t y_offset, bool is_show) {

}

static void home_clock_ui_update(ui_manager_context_t *thiz, int16_t y_offset, bool is_show)
{

}

static home_ui_display_type_e home_ui_display_type_update(ui_manager_context_t *thiz, ui_proc_home_t *proc)
{
    return HOME_UI_DISPLAY_DEFAULT;

}

static void home_ui_update(ui_manager_context_t *thiz, ui_proc_home_t *proc)
{
    // home_ui_display_type_e home_ui_type = home_ui_display_type_update(thiz, proc);
    // if (home_ui_type != home_info->home_ui_type) {
    //     home_info->home_ui_type = home_ui_type;
    //     home_info->force_update = true;
    //     home_info->record_hour = 0;
    //     home_info->record_minute = 0;
    //     home_info->clock_hour_value = 0;
    //     home_info->clock_minute_value = 0;
    // }
    // switch (home_ui_type) {
    //     default:
    //     case HOME_UI_DISPLAY_DEFAULT:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_IDLE_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y, true);
    //         home_label_text_ui_update(thiz, "Select a program or mode\nto start pumping",
    //             IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y, true, false, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, 
    //             home_info->timestamp_offset, IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y, false);
    //         home_clock_ui_update(thiz, 0, false);
    //         home_root_animation_update(thiz, RESOURCE_ID_HD_ANIM_IP_START,
    //              RESOURCE_ID_HD_ANIM_IP_END - RESOURCE_ID_HD_ANIM_IP_START + 1,
    //             HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    //     case HOME_UI_DISPLAY_CLOCK_AND_RECORD:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_IDLE_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y, false);
    //         home_label_text_ui_update(thiz, "since last pumping session",
    //             IDLE_LABEL_TEXT_X - HOME_RECORD_TEXT_X_OFFSET, 
    //             (IDLE_LABEL_TEXT_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET) + HOME_RECORD_TEXT_Y_OFFSET, 
    //             true, false, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, home_info->timestamp_offset, 
    //                 180, IDLE_LABEL_TEXT_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, true);
    //         home_clock_ui_update(thiz, 0, true);
    //         home_root_animation_update(thiz, RESOURCE_ID_HD_ANIM_IP_START,
    //             RESOURCE_ID_HD_ANIM_IP_END - RESOURCE_ID_HD_ANIM_IP_START + 1,
    //             HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    //     case HOME_UI_DISPLAY_CLOCK:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_IDLE_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, true);
    //         home_label_text_ui_update(thiz, "Select a program or mode\nto start pumping",
    //             IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, true, false, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, home_info->timestamp_offset, 
    //             IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y - HOME_WITH_CLOCK_LABLE_Y_OFFSET, false);
    //         home_clock_ui_update(thiz, 0, true);
    //         home_root_animation_update(thiz, RESOURCE_ID_HD_ANIM_IP_START,
    //              RESOURCE_ID_HD_ANIM_IP_END - RESOURCE_ID_HD_ANIM_IP_START + 1,
    //             HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    //     case HOME_UI_DISPLAY_RECORD:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_IDLE_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y, false);
    //         home_label_text_ui_update(thiz, "since last pumping session",
    //             IDLE_LABEL_TEXT_X - HOME_RECORD_TEXT_X_OFFSET, 
    //             IDLE_LABEL_TEXT_Y + HOME_RECORD_TEXT_Y_OFFSET, true, false, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, home_info->timestamp_offset,
    //             180, IDLE_LABEL_TEXT_Y, true);
    //         home_clock_ui_update(thiz, 0, false);
    //         home_root_animation_update(thiz, RESOURCE_ID_HD_ANIM_IP_START,
    //             RESOURCE_ID_HD_ANIM_IP_END - RESOURCE_ID_HD_ANIM_IP_START + 1,
    //             HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    //     case HOME_UI_DISPLAY_CLOCK_ADJUST_TIP:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_IDLE_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y - HOME_CLOCK_TIP_LABLE_Y_OFFSET, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y, false);
    //         home_label_text_ui_update(thiz, "Time not updated. Connect\nthe device to sync",
    //             IDLE_LABEL_TEXT_X - 12, IDLE_LABEL_TEXT_Y - HOME_CLOCK_TIP_LABLE_Y_OFFSET, true, true, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, home_info->timestamp_offset,
    //              IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y - HOME_CLOCK_TIP_LABLE_Y_OFFSET, false);
    //         home_clock_ui_update(thiz, 0, false);
    //         home_root_animation_update(thiz, RESOURCE_ID_HD_ANIM_IP_START,
    //             RESOURCE_ID_HD_ANIM_IP_END - RESOURCE_ID_HD_ANIM_IP_START + 1,
    //             HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    //     case HOME_UI_DISPLAY_CLOCK_LIGHT_TIP:
    //         home_label_ui_update(thiz, RESOURCE_ID_HOME_LIGHT_LABLE_ICON,
    //             IDLE_LABEL_AREA_ICON_X, IDLE_LABEL_AREA_ICON_Y - HOME_LIGHT_TIP_LABLE_Y_OFFSET, true);
    //         home_button_ui_update(thiz, IDLE_BUTTON_ICON_X, IDLE_BUTTON_ICON_Y, false);
    //         home_label_text_ui_update(thiz, "Press to turn on the light",
    //             IDLE_LABEL_TEXT_X - 18, 
    //             (IDLE_LABEL_TEXT_Y - 2) - HOME_LIGHT_TIP_LABLE_Y_OFFSET,
    //             true, false, &lv_font_oppo_sans_16_medium);
    //         home_record_time_ui_update(thiz, home_info->timestamp_offset,
    //              IDLE_LABEL_TEXT_X, IDLE_LABEL_TEXT_Y - HOME_LIGHT_TIP_LABLE_Y_OFFSET, false);
    //         home_clock_ui_update(thiz, -18, true);
    //         // home_root_animation_update(thiz, RESOURCE_ID_HL_ANIM_IP_START,
    //         //     RESOURCE_ID_HL_ANIM_IP_END - RESOURCE_ID_HL_ANIM_IP_START + 1,
    //         //     HOME_IDLE_ANIMATION_X, HOME_IDLE_ANIMATION_Y);
    //         break;
    // }
}


/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_home_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (home_info == NULL) {
    //     home_info = (home_display_info_t *)u_malloz(sizeof(home_display_info_t));
    //     EXCEPTION_CHECK_VOID(home_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // home_root_init();
    // ble_ui_init(thiz);
    // common_ui_bg_init(thiz);
    // home_info->force_update = true;
    // home_info->animation_index = 0;
    // home_info->popup_ui_index = ID_POPUP_UI_NONE;
}

void ui_display_home_remove(ui_manager_context_t *thiz)
{

}

int ui_display_home_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz && home_info && home_info->root);
    // ui_proc_home_t *proc = (ui_proc_home_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc && proc->ble);
    // home_root_bg_update(thiz);
    // ble_ui_update(thiz, proc->ble, COMMON_BLE_UI_X, COMMON_BLE_UI_Y);
    // home_ui_update(thiz, proc);
    // ui_common_operation_tip_update(thiz, proc->popup_ui_type, home_info->root,
    //     &home_info->popup_ui_index);
    // home_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> home fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
