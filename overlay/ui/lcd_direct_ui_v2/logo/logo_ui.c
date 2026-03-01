/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_LOGO_COMP_BG                                (0)

typedef struct {
    lv_obj_t * root;
    lv_obj_t * backgroud;
    lv_obj_t * logo_icon; 
    res_img_info_t img_info;
    res_img_info_t background_info;
    lv_draw_buf_t *background_snapshot;  /// 背景快照，避免重复解码
    uint8_t force_update;
    uint32_t start_time;
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
    if (logo_info->root == NULL || !lv_obj_is_valid(logo_info->root)) {
        logo_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(logo_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(logo_info->root, lv_color_white(), 0);     /// 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(logo_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(logo_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(logo_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(logo_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(logo_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(logo_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

#if IS_SUPPORT_LOGO_COMP_BG
static void ui_logo_bg_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (logo_info->backgroud == NULL || !lv_obj_is_valid(logo_info->backgroud)) {
        logo_info->backgroud = lv_image_create(logo_info->root);
        EXCEPTION_CHECK_VOID(logo_info->backgroud);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(logo_info->backgroud, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(logo_info->backgroud, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, logo_bg_data,
            sizeof(logo_bg_data), "png");
        lv_image_set_src(logo_info->backgroud, (const void *)&bg_img_path);
        lv_obj_align(logo_info->backgroud, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#else
static void ui_logo_bg_update(ui_manager_context_t *thiz)
{
    if (logo_info->force_update == false) {
        return;
    }

    if (logo_info->backgroud == NULL || !lv_obj_is_valid(logo_info->backgroud)) {
        logo_info->backgroud = lv_image_create(logo_info->root);
        EXCEPTION_CHECK_VOID(logo_info->backgroud);
#if LV_USE_SNAPSHOT
        if (logo_info->background_snapshot != NULL) {
            lv_draw_buf_destroy(logo_info->background_snapshot);
            logo_info->background_snapshot = NULL;
        }
#endif
        common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_HOME_BG, &logo_info->background_info);
        EXCEPTION_CHECK_VOID(logo_info->background_info.data);
        lv_fs_path_ex_t background_path;
        lv_fs_make_path_from_buffer(&background_path,
                                    LV_FS_MEMFS_LETTER,
                                    logo_info->background_info.data,
                                    logo_info->background_info.size,
                                    "png");
        lv_image_set_src(logo_info->backgroud, (const void *)&background_path);
        lv_obj_align(logo_info->backgroud, LV_ALIGN_CENTER, 0, 0);
#if LV_USE_SNAPSHOT
        lv_refr_now(NULL); /// 等待渲染完成后再创建快照
        logo_info->background_snapshot = lv_snapshot_take(logo_info->backgroud, LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
#endif
    }

#if LV_USE_SNAPSHOT
    if (logo_info->background_snapshot != NULL) {
        lv_image_set_src(logo_info->backgroud, logo_info->background_snapshot);
    }
#endif
    // lv_obj_move_background(logo_info->backgroud);
    // lv_obj_move_to_index(logo_info->backgroud, 0);
}
#endif

static void sys_logo_label_update(ui_manager_context_t *thiz)
{
    if (logo_info->force_update == false) {
        return;
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_START_LOGO_ICON, &logo_info->img_info);
    EXCEPTION_CHECK_VOID(logo_info->img_info.data);
    if (logo_info->logo_icon == NULL || !lv_obj_is_valid(logo_info->logo_icon)) {
        logo_info->logo_icon = lv_image_create(logo_info->root);
        EXCEPTION_CHECK_VOID(logo_info->logo_icon);
    }
    lv_fs_path_ex_t logo_img_path;
    lv_fs_make_path_from_buffer(&logo_img_path, LV_FS_MEMFS_LETTER, logo_info->img_info.data,
        logo_info->img_info.size, "baj");
    lv_image_set_src(logo_info->logo_icon, (const void *)&logo_img_path);
    lv_obj_align(logo_info->logo_icon, LV_ALIGN_CENTER, 0, 0);    /// 设置logo图标位置
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_logo_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (logo_info == NULL) {
        logo_info = (logo_display_info_t *)u_malloz(sizeof(logo_display_info_t));
        EXCEPTION_CHECK_VOID(logo_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    ui_logo_root_init(); /// 初始化 root 容器
    logo_info->force_update = true;
}

void ui_display_logo_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(logo_info->root);
    common_ui_img_resource_deinit(thiz, &logo_info->img_info);
    common_ui_img_resource_deinit(thiz, &logo_info->background_info);
    u_free(logo_info);
    logo_info = NULL;
}

int ui_display_logo_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(logo_info);
    EXCEPTION_CHECK_INT(logo_info->root);
    ui_logo_bg_update(thiz);
    sys_logo_label_update(thiz);
    logo_info->force_update = false;
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> logo fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}
