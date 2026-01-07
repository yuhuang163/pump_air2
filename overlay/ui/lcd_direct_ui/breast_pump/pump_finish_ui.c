/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_COMPLETE_TIP_COMP_BG                        (0)
#define COMPLETE_TIP_TEXT_COUNT                                (2)

#define FINISH_TIP_TITLE_COLOR                                 0x1C1C20
#define FINISH_TIP_CONTENT_COLOR                               0x3C3D40

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} complete_tip_text_info_t;

typedef struct {
    lv_obj_t * root;
#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
    lv_obj_t * background;
#endif
    lv_obj_t * icon;
    lv_obj_t * texts[COMPLETE_TIP_TEXT_COUNT];
    res_img_info_t icon_info;
} complete_tip_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
static PSRAM_DATA_LOC uint8_t complete_tip_bg_data[] = {
    #include "comparison_png2txt/pump_finish_comp_360x360.txt"
};
#endif

static complete_tip_display_info_t *finish_info;

const static complete_tip_text_info_t complete_tip_text_infos[COMPLETE_TIP_TEXT_COUNT] = {
    { "Finish", (138 + 3), (151 + 7) },
    { "Please go to MOMCOZY\napp and record the\namount of milk.", (61 + 12), (199 + 3)},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_complete_tip_root_init(void)
{
    if (finish_info->root == NULL || !lv_obj_is_valid(finish_info->root)) {
        finish_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(finish_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(finish_info->root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(finish_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(finish_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(finish_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(finish_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(finish_info->root, LV_OBJ_FLAG_CLICKABLE);           /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(finish_info->root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
static void pump_complete_tip_bg_update(void)
{
    static uint8_t status = 0;
    static uint32_t start_time = 0;
    if (finish_info->background == NULL || !lv_obj_is_valid(finish_info->root)) {
        finish_info->background = lv_image_create(finish_info->root);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(finish_info->background, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(finish_info->background, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, complete_tip_bg_data,
            sizeof(complete_tip_bg_data), "png");
        lv_image_set_src(finish_info->background, (const void *)&bg_img_path);
        lv_obj_align(finish_info->background, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif

static void pump_complete_tip_label_update(ui_manager_context_t *thiz)
{
    if (finish_info->icon == NULL || !lv_obj_is_valid(finish_info->root)) {
        finish_info->icon = lv_image_create(lv_screen_active());
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_FINISH_ICON, &finish_info->icon_info);
    EXCEPTION_CHECK_VOID(finish_info->icon_info.data);
    lv_fs_path_ex_t complete_tip_img_path;
    lv_fs_make_path_from_buffer(&complete_tip_img_path, LV_FS_MEMFS_LETTER,
        finish_info->icon_info.data, finish_info->icon_info.size, "baj");
    lv_image_set_src(finish_info->icon, complete_tip_img_path.path);
    lv_obj_align(finish_info->icon, LV_ALIGN_TOP_LEFT, 148 + 4, 63);
}

static void pump_complete_tip_texts_update(ui_manager_context_t *thiz, uint8_t index)
{
    if (finish_info->texts[index] == NULL || !lv_obj_is_valid(finish_info->texts[index])) {
        finish_info->texts[index] = lv_label_create(finish_info->root);
    }
    lv_label_set_text(finish_info->texts[index], complete_tip_text_infos[index].name);
    lv_obj_set_style_text_color(finish_info->texts[index], 
        lv_color_hex(index == 0 ? FINISH_TIP_TITLE_COLOR : FINISH_TIP_CONTENT_COLOR), 0);
    lv_obj_set_style_text_font(finish_info->texts[index], 
        index == 0 ? &lv_font_roboto_32_regular : &lv_font_roboto_21_regular, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(finish_info->texts[index], 2, 0);  /// 设置行间距（像素）
    lv_obj_set_style_text_align(finish_info->texts[index], LV_TEXT_ALIGN_CENTER, 0);  /// 设置文本居中对齐
    lv_obj_align(finish_info->texts[index], LV_ALIGN_TOP_LEFT,
        complete_tip_text_infos[index].text_x, complete_tip_text_infos[index].text_y);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_complete_tip_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (finish_info == NULL) {
        finish_info = (complete_tip_display_info_t *)u_malloz(sizeof(complete_tip_display_info_t));
        EXCEPTION_CHECK_VOID(finish_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_complete_tip_root_init();
}

void ui_display_pump_complete_tip_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (finish_info == NULL) {
        return;
    }
    common_ui_img_resource_deinit(thiz, &finish_info->icon_info);
    LVGL_OBJ_DELETE(finish_info->root); 
    u_free(finish_info);
    finish_info = NULL;
}

int ui_display_pump_complete_tip(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(finish_info);
    EXCEPTION_CHECK_INT(finish_info->root);
#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
    pump_complete_tip_bg_update();
#endif
    pump_complete_tip_label_update(thiz);
    for (uint8_t i = 0; i < COMPLETE_TIP_TEXT_COUNT; i++) {
        pump_complete_tip_texts_update(thiz, i);
    }
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> pump complete tip fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}