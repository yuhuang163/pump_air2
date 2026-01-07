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

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static lv_obj_t *logo_root;
static res_img_info_t logo_img_info;
static uint32_t start_time = 0;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void sys_logo_label_update(ui_manager_context_t *thiz)
{
    common_ui_img_resource_init(thiz, RESOURCE_ID_START_LOGO_ICON, &logo_img_info);
    EXCEPTION_CHECK_VOID(logo_img_info.data);
    if (logo_root == NULL || !lv_obj_is_valid(logo_root)) {
        logo_root = lv_image_create(lv_screen_active());
    }
    lv_obj_set_size(logo_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(logo_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(logo_root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(logo_root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(logo_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_fs_path_ex_t logo_img_path;
    lv_fs_make_path_from_buffer(&logo_img_path, LV_FS_MEMFS_LETTER, logo_img_info.data,
        logo_img_info.size, "baj");
    lv_image_set_src(logo_root, (const void *)&logo_img_path);
    lv_obj_align(logo_root, LV_ALIGN_CENTER, 0, 0);    /// 设置模式标签位置
    lv_obj_clear_flag(logo_root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(logo_root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_logo_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
}

void ui_display_logo_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(logo_root);
    common_ui_img_resource_deinit(thiz, &logo_img_info);
}

int ui_display_logo_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    sys_logo_label_update(thiz);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> logo fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}
