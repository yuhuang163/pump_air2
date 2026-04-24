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
#define IS_SUPPORT_ROOT_HOME_SCREEN_SAVER_COMP_BG                (0)

#define HOME_SCREEN_SAVER_ICON_MAX_COUNT                         (1)
#define HOME_SCREEN_SAVER_ICON_INDEX                             (0)

#define HOME_SCREEN_SAVER_ICON_X                                 (0)
#define HOME_SCREEN_SAVER_ICON_Y                                 (108 + 2)
#define IDLE_TEXT_COLOR                                          (0xB25962)
#define HOME_CLOCK_HOUR_X                                        (44 + 2)
#define HOME_CLOCK_MINUTE_X                                      (192 + 2)
#define HOME_CLOCK_TIME_Y                                        (141 - 26)
#define HOME_CLOCK_COLON_X                                       (174 )
#define HOME_CLOCK_COLON_Y                                       (141 - 14)

typedef struct {
    res_img_info_t saver_img_info;
    res_img_info_t clock_colon_info;
    uint8_t home_screen_saver_display_type;
    uint8_t hour;
    uint8_t minute;
    uint8_t force_update;
    uint8_t rsv[13];

} home_screen_saver_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static home_screen_saver_display_info_t *saver_info;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_ROOT_HOME_SCREEN_SAVER_COMP_BG
static PSRAM_DATA_LOC uint8_t home_screen_saver_bg_data[] = {
    #include "comparison_png2txt/home_idle_360x360.txt"
};
#endif
/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void home_screen_saver_root_init(void)
{

}

static void home_screen_saver_root_bg_update(ui_manager_context_t *thiz)
{

}


static void home_screen_saver_dummy_icon_update(ui_manager_context_t *thiz, int16_t y_offset)
{

}

static void home_screen_saver_clock_ui_update(ui_manager_context_t *thiz, bool is_show)
{
}
static home_screen_saver_display_type_e home_screen_saver_display_type_update(ui_manager_context_t *thiz, ui_proc_home_t *proc)
{
    return HOME_SCREEN_SAVER_DISPLAY_DEFAULT;
}

static void home_screen_saver_ui_update(ui_manager_context_t *thiz, ui_proc_home_t *proc)
{
    // home_screen_saver_display_type_e home_ui_type = home_screen_saver_display_type_update(thiz, proc);
    // if (home_ui_type != saver_info->home_screen_saver_display_type) {
    //     saver_info->home_screen_saver_display_type = home_ui_type;
    //     saver_info->force_update = true;
    //     saver_info->hour = 0;
    //     saver_info->minute = 0;
    // }

    // if (home_ui_type == HOME_SCREEN_SAVER_DISPLAY_CLOCK) {
    //     home_screen_saver_clock_ui_update(thiz, true);
    //     home_screen_saver_dummy_icon_update(thiz, 29);
    // } else {
    //     home_screen_saver_clock_ui_update(thiz, false);
    //     home_screen_saver_dummy_icon_update(thiz, 0);
    // }
}

/*  
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_home_screen_saver_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (saver_info == NULL) {
    //     saver_info = (home_screen_saver_display_info_t *)u_malloz(sizeof(home_screen_saver_display_info_t));
    //     EXCEPTION_CHECK_VOID(saver_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // home_screen_saver_root_init();
    // common_ui_bg_init(thiz);
    // ble_ui_init(thiz);
    // saver_info->force_update = true;
}

void ui_display_home_screen_saver_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz && saver_info);
//     common_ui_img_resource_deinit(thiz, &saver_info->saver_img_info);
// #if (ROOT_PRODUCT_ID == ROOT_PRODUCT_ID_V3PRO)
//     common_ui_img_resource_deinit(thiz, &saver_info->clock_colon_info);
// #endif
//     ble_ui_deinit(thiz);
//     common_ui_bg_deinit(thiz);
//     LVGL_OBJ_DELETE(saver_info->root);
//     u_free(saver_info);
//     saver_info = NULL;
}

int ui_display_home_screen_saver_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz && saver_info && saver_info->root);
    // ui_proc_home_t *proc = (ui_proc_home_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc && proc->ble);
    // home_screen_saver_root_bg_update(thiz);
    // ble_ui_update(thiz, proc->ble, COMMON_BLE_UI_X, COMMON_BLE_UI_Y);
    // home_screen_saver_ui_update(thiz, proc);
    // saver_info->force_update = false;
    // thiz->priv.fps_count++;
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> home screen saver fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
    // }
    return 0;
}
