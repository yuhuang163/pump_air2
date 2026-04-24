/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ota_ui.h"
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_ota.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_OTA_COMP_BG                      (0)

#define OTA_PROGRESS_ARC_SIZE                       (38)    
#define OTA_PROGRESS_ARC_WIDTH                      (3)    
#define OTA_PROGRESS_ARC_BG_COLOR                   (0xFFFFFF)    
#define OTA_PROGRESS_ARC_BG_OPA                     (38)        
#define OTA_PROGRESS_ARC_COLOR                      (0xB25962)    
#define OTA_PROGRESS_ARC_LOOP_MS_PER_DEG            (3)    
#define OTA_PROGRESS_ARC_ANGLE_SPAN                 (90)   
#define OTA_PROGRESS_ARC_TEXT_COLOR                 (0x767676)    

typedef struct {
    uint32_t start_time;
    uint32_t display_time;
    uint8_t force_update;
    uint8_t ota_status;
    uint8_t progress;
    uint8_t ota_object;
    uint8_t rsv[12];
    char text[64];
} ota_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ota_display_info_t *ota_info = NULL;
#if IS_SUPPORT_OTA_COMP_BG
static PSRAM_DATA_LOC uint8_t factory_reset_confirm_bg_data[] = {
    #include "comparison_png2txt/ota_running_360x360.txt"

    
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
static void ui_ota_root_init(void)
{

}

static void ui_ota_progress_arc_update(ui_manager_context_t *thiz, uint8_t ota_status)
{

}

static const char *get_ota_object_name(ota_object_e ota_object)
{
    if (ota_object == OTA_OBJECT_MAIN_FIRMWARE) {
        return "main firmware";
    } else if (ota_object == OTA_OBJECT_TOUCH_KEY_FIRMWARE) {
        return "touch firmware";
    } else if (ota_object == OTA_OBJECT_IMAGE_RESOURCE) {
        return "image resource";
    }
    return "unknown";
}

static void ui_ota_text_update(ui_manager_context_t *thiz, uint8_t ota_status, uint8_t progress)
{
    if (ota_info->force_update == false && ota_info->ota_status == ota_status && 
        ota_info->progress == progress) {
        return;
    }

}

static void ui_ota_icon_update(ui_manager_context_t *thiz, uint8_t ota_status)
{
    if (ota_info->force_update == false && ota_info->ota_status == ota_status) {
        return;
    }

}

#if IS_SUPPORT_OTA_COMP_BG
static void ui_ota_background_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (ota_info->background == NULL || !lv_obj_is_valid(ota_info->background)) {
        ota_info->background = lv_image_create(ota_info->root);
        EXCEPTION_CHECK_VOID(ota_info->background);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(ota_info->background, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(ota_info->background, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, factory_reset_confirm_bg_data,
            sizeof(factory_reset_confirm_bg_data), "png");
        lv_image_set_src(ota_info->background, (const void *)&bg_img_path);
        lv_obj_align(ota_info->background, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_ota_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (ota_info == NULL) {
    //     ota_info = (ota_display_info_t *)u_malloz(sizeof(ota_display_info_t));
    //     EXCEPTION_CHECK_VOID(ota_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_ota_root_init(); /// 初始化 root 容器
    // ota_info->force_update = true;
    // ota_info->progress = 0;
    // ui_proc_ota_t *proc = (ui_proc_ota_t *)thiz->priv.play_info.user_data;
    // if (proc && proc->ota) {
    //     ota_info->ota_object = proc->ota->get_ota_object();
    // }
}

void ui_display_ota_remove(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_VOID(thiz && ota_info && ota_info->root); 
//     common_ui_img_resource_deinit(thiz, &ota_info->icon_info);
//     LVGL_OBJ_DELETE(ota_info->root);
//     u_free(ota_info);
//     ota_info = NULL;
}

int ui_display_ota_process(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_INT(thiz && ota_info && ota_info->root);
//     ui_proc_ota_t *proc = (ui_proc_ota_t *)thiz->priv.play_info.user_data;
//     EXCEPTION_CHECK_INT(proc);
//     uint8_t ota_status = proc->ota_status;
//     uint8_t progress = proc->ota->get_ota_upgrade_progress();
// #if IS_SUPPORT_OTA_COMP_BG
//     ui_ota_background_update(thiz);
// #endif
//     ui_ota_progress_arc_update(thiz, ota_status);
//     ui_ota_text_update(thiz, ota_status, progress);
//     ui_ota_icon_update(thiz, ota_status);
//     ota_info->ota_status = ota_status;
//     ota_info->progress = progress;
//     ota_info->force_update = false;
//     thiz->priv.fps_count++;
//     if (ota_status == OTA_UI_STATUS_DOWNLOADING) {
//         ota_info->display_time = get_timestamp();
//     } else {
//         if (get_timestamp_ms(ota_info->display_time) > OTA_RESULT_DISPLAY_TIME) {
//             ui_animation_play_t *ani = &thiz->priv.play_info;
//             EXCEPTION_CHECK_INT(ani);
//             LOG_UI("ota result display timeout");
//             ani->progress_cb(thiz, 0, ANI_TIMER_DRAW_FINISH);
//         }
//     }

//     if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
//         thiz->priv.fps_start_time = get_timestamp();
//         LOG_UI("===> ota fps: %d", thiz->priv.fps_count);
//         thiz->priv.fps_count = 0;
//     }
    
    return 0;
}
