/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_FACTORY_RESET_COMP_BG        (0)

#define PROGRESS_ARC_SIZE                       (38)    /* 直径（原80缩小20%） */
#define PROGRESS_ARC_WIDTH                      (3)    /* 圆弧线宽（原4加粗50%） */
#define PROGRESS_ARC_BG_COLOR                   (0xFFFFFF)    /* 背景圆弧颜色：黑色 */
#define PROGRESS_ARC_BG_OPA                     (38)        /* 背景圆弧透明度 15%（255*15/100≈38） */
#define PROGRESS_ARC_COLOR                      (0xB25962)    /* 圆弧颜色：粉色 */
#define PROGRESS_ARC_LOOP_MS_PER_DEG            (3)    /* ms per degree，越小转得越快（原15约5.4s/圈，现约2.2s/圈） */
#define PROGRESS_ARC_ANGLE_SPAN                 (90)   /* 25% of 360 */
#define FACTORY_RESET_TEXT_COLOR                (0x767676)    /* 文本颜色：灰色 */

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
typedef struct {
    uint32_t start_time;
    uint8_t force_update;
    uint8_t rsv[15];
} ui_factory_reset_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_FACTORY_RESET_COMP_BG
static PSRAM_DATA_LOC uint8_t factory_reset_bg_data[] = {
    #include "comparison_png2txt/factort_reset_360x360.txt"
};
#endif
static ui_factory_reset_info_t *ui_info;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_factory_reset_root_init(void)
{

}

static void ui_factory_reset_update(ui_manager_context_t *thiz)
{

}

#if IS_SUPPORT_FACTORY_RESET_COMP_BG
static void factory_reset_root_bg_update(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(ui_info);
    EXCEPTION_CHECK_VOID(ui_info->root);

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
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, factory_reset_bg_data,
            sizeof(factory_reset_bg_data), "png");
        lv_image_set_src(ui_info->background, (const void *)&bg_img_path);
        lv_obj_align(ui_info->background, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 500) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_factory_reset_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (ui_info == NULL) {
    //     ui_info = (ui_factory_reset_info_t *)u_malloz(sizeof(ui_factory_reset_info_t));
    //     EXCEPTION_CHECK_VOID(ui_info);
    // }
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    // ui_factory_reset_root_init();
    // ui_info->force_update = true;
}

void ui_display_factory_reset_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(ui_info);
    // LVGL_OBJ_DELETE(ui_info->root);
    // u_free(ui_info);
    // ui_info = NULL;
}

int ui_display_factory_reset_process(ui_manager_context_t *thiz)
{
//     EXCEPTION_CHECK_INT(thiz);
//     EXCEPTION_CHECK_INT(ui_info);
//     EXCEPTION_CHECK_INT(ui_info->root);
// #if IS_SUPPORT_FACTORY_RESET_COMP_BG
//     factory_reset_root_bg_update(thiz);
// #endif
//     ui_factory_reset_update(thiz);
//     ui_info->force_update = false;
//     thiz->priv.fps_count++;
//     if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
//         thiz->priv.fps_start_time = get_timestamp();
//         LOG_UI("===> factory reset fps: %d", thiz->priv.fps_count);
//         thiz->priv.fps_count = 0;
//     }
    return 0;
}
