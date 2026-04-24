/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_charge.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define CHARGE_ADAPTER_TEXT_COLOR                      0xB25962
#define IS_SUPPORT_CHARGE_ADAPTER_COMP_BG              (0)

typedef struct {  
    uint8_t force_update;
    uint8_t animation_index;
    uint8_t rsv[14];
} charge_adapter_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static charge_adapter_display_info_t *adapter_info;
#if IS_SUPPORT_CHARGE_ADAPTER_COMP_BG
static PSRAM_DATA_LOC uint8_t charge_adapter_bg_data[] = {
    #include "comparison_png2txt/charge_error_360x360.txt"
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
static void charge_adapter_ui_root_init(ui_manager_context_t *thiz)
{
}

static void charge_adapter_bg_update(ui_manager_context_t *thiz)
{

}

static int charge_adapter_object_update(ui_manager_context_t *thiz)
{
    if (adapter_info->force_update == false) {
        return 0;
    }

    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_charge_adapter_screen_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (adapter_info == NULL) {
    //     adapter_info = (charge_adapter_display_info_t *)u_malloz(sizeof(charge_adapter_display_info_t));
    //     EXCEPTION_CHECK_VOID(adapter_info);
    // }
    // charge_adapter_ui_root_init(thiz);  
    // common_ui_bg_init(thiz);
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // adapter_info->force_update = true;
}

void ui_display_charge_adapter_screen_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(adapter_info);
    // common_ui_img_resource_deinit(thiz, &adapter_info->charge_adapter_icon_info);
    // common_ui_bg_deinit(thiz);
    // LVGL_OBJ_DELETE(adapter_info->root);
    // u_free(adapter_info);
    // adapter_info = NULL;
}

int ui_display_charge_adapter_process(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_INT(thiz);
    // EXCEPTION_CHECK_INT(adapter_info);
    // EXCEPTION_CHECK_INT(adapter_info->root);
    // ui_proc_charge_t *proc = (ui_proc_charge_t *)thiz->priv.play_info.user_data;
    // EXCEPTION_CHECK_INT(proc);
    // charge_adapter_bg_update(thiz);
    // charge_adapter_object_update(thiz);
    // thiz->priv.fps_count++;
    // adapter_info->force_update = false; 
    // if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
    //     thiz->priv.fps_start_time = get_timestamp();
    //     LOG_UI("===> charge adapter fps: %d", thiz->priv.fps_count);
    //     thiz->priv.fps_count = 0;
        
    // }
    return 0;
}