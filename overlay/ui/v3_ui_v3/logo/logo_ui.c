/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_logo.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#if IS_SUPPORT_LOGO_BG_COLOR_TEST
#define IS_SUPPORT_LOGO_COMP_BG                                (0)
#define IS_SUPPORT_LOGO_IMG_TEST                               (1)
#else
#define IS_SUPPORT_LOGO_COMP_BG                                (0)
#define IS_SUPPORT_LOGO_IMG_TEST                               (0)
#endif

typedef struct {
    uint32_t start_time;
    uint8_t force_update;
    uint8_t rsv[15];
} logo_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static logo_display_info_t *logo_info = NULL;
#if IS_SUPPORT_LOGO_COMP_BG
static PSRAM_DATA_LOC uint8_t logo_bg_data[] = {
    #include "comparison_png2txt/start_logo_360x360.txt"
};
#endif
#if IS_SUPPORT_LOGO_IMG_TEST
static PSRAM_DATA_LOC uint8_t logo_img_data1[] = {
    #include "comparison_png2txt/pump_home_360x360.txt"
};    
#define LOGO_IMG_COUNT 1
static ui_img_data_t logo_img[LOGO_IMG_COUNT] = {
    {logo_img_data1, sizeof(logo_img_data1)},
    // {logo_img_data2, sizeof(logo_img_data2)},
    // {logo_img_data3, sizeof(logo_img_data3)},
    // {logo_img_data4, sizeof(logo_img_data4)},
    // {logo_img_data5, sizeof(logo_img_data5)},
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
static void ui_logo_root_init(void)
{

}

static void ui_logo_bg_update(ui_manager_context_t *thiz)
{
}

static void sys_logo_label_update(ui_manager_context_t *thiz)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_logo_prepare(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz);
//     if (logo_info == NULL) {
//         logo_info = (logo_display_info_t *)u_malloz(sizeof(logo_display_info_t));
//         EXCEPTION_CHECK_VOID(logo_info);
//     }
//     thiz->priv.fps_count = 0;
//     thiz->priv.fps_start_time = get_timestamp();
//     ui_logo_root_init(); /// 初始化 root 容器
// #if (IS_SUPPORT_LOGO_BG_COLOR_TEST == 0 && IS_SUPPORT_LOGO_IMG_TEST == 0) 
//     common_ui_bg_init(thiz);
// #endif
//     logo_info->force_update = true;
}

void ui_display_logo_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz);
//     EXCEPTION_CHECK_VOID(logo_info);
//     EXCEPTION_CHECK_VOID(logo_info->root);
// #if (IS_SUPPORT_LOGO_BG_COLOR_TEST == 0 && IS_SUPPORT_LOGO_IMG_TEST == 0) 
//     common_ui_img_resource_deinit(thiz, &logo_info->img_info);
//     common_ui_bg_deinit(thiz);
// #endif
//     LVGL_OBJ_DELETE(logo_info->root);
//     u_free(logo_info);
//     logo_info = NULL;
}

int ui_display_logo_process(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_INT(thiz);
//     EXCEPTION_CHECK_INT(logo_info);
//     EXCEPTION_CHECK_INT(logo_info->root);
// #if (IS_SUPPORT_LOGO_BG_COLOR_TEST == 0 && IS_SUPPORT_LOGO_IMG_TEST == 0) 
//     ui_logo_bg_update(thiz);
//     sys_logo_label_update(thiz);   
// #elif (IS_SUPPORT_LOGO_IMG_TEST == 1 && IS_SUPPORT_LOGO_COMP_BG == 0)
//     sys_logo_label_update(thiz);
// #else
//     ui_proc_logo_t *proc = (ui_proc_logo_t *)thiz->priv.play_info.user_data;
//     uint8_t is_white_color = (proc->test_page_count % 2) ? false : true;
//     if (is_white_color) {
//         lv_obj_set_style_bg_color(logo_info->root, lv_color_white(), 0);    
//     } else {
//         lv_obj_set_style_bg_color(logo_info->root, lv_color_black(), 0);    
//     }
// #endif
//     logo_info->force_update = false;
//     thiz->priv.fps_count++;
//     if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
//         thiz->priv.fps_start_time = get_timestamp();
//         LOG_UI("===> logo fps: %d", thiz->priv.fps_count);
//         thiz->priv.fps_count = 0;
//     }
    return 0;
}
