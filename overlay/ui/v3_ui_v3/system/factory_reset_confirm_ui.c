/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_factory_reset.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_FACTORY_RESET_COMP_BG                  (0)

#define FACTORY_RESET_CONFIRM_FIRST_TEXT_COLOR            (0x8C8C8C)
#define FACTORY_RESET_CONFIRM_TEXT_COLOR                  (0xFFFFFF)
#define FACTORY_RESET_CONFIRM_ARC_SIZE                    (360)    // 圆弧尺寸
#define FACTORY_RESET_CONFIRM_ARC_WIDTH                   (12)      // 圆弧粗细
#define FACTORY_RESET_CONFIRM_ARC_COLOR                   (0xB25962) // 圆弧颜色（匹配吸奶器粉色）
#define FACTORY_RESET_CONFIRM_ARC_START_ANGLE             (270)    
#define FACTORY_RESET_CONFIRM_ARC_FULL_SWEEP              (360)    
#define FACTORY_RESET_CONFIRM_DISPLAY_TIME                (3000)  
#define FACTORY_RESET_CONFIRM_DISPLAY_STEP_TIME           (100)
#define FACTORY_RESET_CONFIRM_DISPLAY_MAX_VALUE           ((FACTORY_RESET_CONFIRM_DISPLAY_TIME / FACTORY_RESET_CONFIRM_DISPLAY_STEP_TIME) - 2)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    uint32_t start_time;
    uint16_t ring_progress;            /// 圆弧进度
    uint8_t force_update;
    uint8_t display_progress;
    uint8_t rsv[12];
} ui_factory_reset_confirm_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_factory_reset_confirm_info_t *ui_info;
#if IS_SUPPORT_FACTORY_RESET_COMP_BG
static PSRAM_DATA_LOC uint8_t factory_reset_confirm_bg_data[] = {
    #include "comparison_png2txt/factory_reset_popup_360x360.txt"
};
#endif

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_factory_reset_confirm_root_init(void)
{

}

static void ui_factory_reset_confirm_update(ui_manager_context_t *thiz)
{

}

static void ui_factory_reset_confirm_arc_update(ui_manager_context_t *thiz, uint8_t display_progress)
{

}

#if IS_SUPPORT_FACTORY_RESET_COMP_BG
static void ui_factory_reset_confirm_background_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (ui_info->background == NULL || !lv_obj_is_valid(ui_info->background)) {
        ui_info->background = lv_image_create(ui_info->root);
        EXCEPTION_CHECK_VOID(ui_info->background);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(ui_info->background, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(ui_info->background, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, factory_reset_confirm_bg_data,
            sizeof(factory_reset_confirm_bg_data), "png");
        lv_image_set_src(ui_info->background, (const void *)&bg_img_path);
        lv_obj_align(ui_info->background, LV_ALIGN_CENTER, 0, 0);
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
void ui_display_factory_reset_confirm_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (ui_info == NULL) {
    //     ui_info = (ui_factory_reset_confirm_info_t *)u_malloz(sizeof(ui_factory_reset_confirm_info_t));
    //     EXCEPTION_CHECK_VOID(ui_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_factory_reset_confirm_root_init();
    // ui_info->force_update = true;
}

void ui_display_factory_reset_confirm_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(ui_info);
    // LVGL_OBJ_DELETE(ui_info->root);
    // u_free(ui_info);
    // ui_info = NULL;
}

int ui_display_factory_reset_confirm_process(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_INT(thiz);
//     EXCEPTION_CHECK_INT(ui_info);
//     EXCEPTION_CHECK_INT(ui_info->root);
//     ui_proc_factory_reset_t *proc = (ui_proc_factory_reset_t *)thiz->priv.play_info.user_data;
// #if IS_SUPPORT_FACTORY_RESET_COMP_BG
//     ui_factory_reset_confirm_background_update(thiz);
// #endif
//     ui_factory_reset_confirm_update(thiz);
//     if (proc->display_fac_rst_confirm_progress != ui_info->display_progress) {
//         ui_info->display_progress = proc->display_fac_rst_confirm_progress;
//         ui_info->start_time = get_timestamp();
//     }
//     ui_factory_reset_confirm_arc_update(thiz, ui_info->display_progress);
//     ui_info->force_update = false;
//     thiz->priv.fps_count++;
    
//     if (get_timestamp_ms(ui_info->start_time) >= FACTORY_RESET_CONFIRM_DISPLAY_TIME &&
//         ui_info->display_progress == 1) {
//         ui_animation_play_t *ani = &thiz->priv.play_info;
//         EXCEPTION_CHECK_INT(ani);
//         LOG_UI("factory reset confirm process completed");
//         ani->progress_cb(thiz, 0, ANI_TIMER_DRAW_FINISH);
//     }

//     if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
//         thiz->priv.fps_start_time = get_timestamp();
//         LOG_UI("===> factory reset confirm fps: %d", thiz->priv.fps_count);
//         thiz->priv.fps_count = 0;
//     }
    return 0;
}
