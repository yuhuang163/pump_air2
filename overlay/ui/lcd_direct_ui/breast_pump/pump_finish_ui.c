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
#define COMPLETE_TIP_TEXT_COUNT                                (4)

#define FINISH_TIP_TITLE_COLOR                                 0x1C1C20
#define FINISH_TIP_CONTENT_COLOR                               0x3C3D40

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} complete_tip_text_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
static PSRAM_DATA_LOC uint8_t complete_tip_bg_data[] = {
    #include "../../../resources/img_png_bin/pump_finish_comp_360x360.txt"
};
#endif

/// 完成提示主界面对象
static lv_obj_t * complete_tip_root;
#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
static lv_obj_t * complete_tip_bg_img;
#endif
static lv_obj_t * complete_tip_label;
static res_img_info_t complete_tip_icon_info;
static lv_obj_t * complete_tip_texts[4];

const static complete_tip_text_info_t complete_tip_text_infos[COMPLETE_TIP_TEXT_COUNT] = {
    { "Finish", (138 + 3), (151 + 7) },
    { "Please go to MOMCOZY", (61 + 12 + 1), (199 + 3)},
    { "app and record the", (61 + 38 + 1), (220 + 4)},
    { "amount of milk.", (61 + 49 + 2), (241 + 4)},
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
    if (complete_tip_root == NULL || !lv_obj_is_valid(complete_tip_root)) {
        complete_tip_root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(complete_tip_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(complete_tip_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(complete_tip_root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(complete_tip_root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(complete_tip_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(complete_tip_root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(complete_tip_root, LV_OBJ_FLAG_CLICKABLE);           /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(complete_tip_root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

#if IS_SUPPORT_COMPLETE_TIP_COMP_BG
static void pump_complete_tip_bg_update(void)
{
    static uint8_t status = 0;
    static uint32_t start_time = 0;
    if (complete_tip_bg_img == NULL || !lv_obj_is_valid(complete_tip_root)) {
        complete_tip_bg_img = lv_image_create(complete_tip_root);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(complete_tip_bg_img, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(complete_tip_bg_img, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, complete_tip_bg_data,
            sizeof(complete_tip_bg_data), "png");
        lv_image_set_src(complete_tip_bg_img, (const void *)&bg_img_path);
        lv_obj_align(complete_tip_bg_img, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif

static void pump_complete_tip_label_update(ui_manager_context_t *thiz)
{
    if (complete_tip_label == NULL || !lv_obj_is_valid(complete_tip_root)) {
        complete_tip_label = lv_image_create(lv_screen_active());
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_FINISH_ICON, &complete_tip_icon_info);
    EXCEPTION_CHECK_VOID(complete_tip_icon_info.data);
    lv_fs_path_ex_t complete_tip_img_path;
    lv_fs_make_path_from_buffer(&complete_tip_img_path, LV_FS_MEMFS_LETTER, complete_tip_icon_info.data,
        complete_tip_icon_info.size, "baj");
    lv_image_set_src(complete_tip_label, complete_tip_img_path.path);
    lv_obj_align(complete_tip_label, LV_ALIGN_TOP_LEFT, 148 + 4, 63);
}

static void pump_complete_tip_texts_update(ui_manager_context_t *thiz, uint8_t index)
{
    if (complete_tip_texts[index] == NULL || !lv_obj_is_valid(complete_tip_texts[index])) {
        complete_tip_texts[index] = lv_label_create(complete_tip_root);
    }
    lv_label_set_text(complete_tip_texts[index], complete_tip_text_infos[index].name);
    lv_obj_set_style_text_color(complete_tip_texts[index], lv_color_hex(index == 0 ? FINISH_TIP_TITLE_COLOR : FINISH_TIP_CONTENT_COLOR), 0);
    lv_obj_set_style_text_font(complete_tip_texts[index], 
        index == 0 ? &lv_font_roboto_32_regular : &lv_font_roboto_21_regular, 0);  /// 设置字体
    lv_obj_align(complete_tip_texts[index], LV_ALIGN_TOP_LEFT,
        complete_tip_text_infos[index].text_x, complete_tip_text_infos[index].text_y);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_complete_tip_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_complete_tip_root_init();
}

void ui_display_pump_complete_tip_remove(ui_manager_context_t *thiz)
{
    common_ui_img_resource_deinit(thiz, &complete_tip_icon_info);
    LVGL_OBJ_DELETE(complete_tip_root); 
    complete_tip_label = NULL;
    for (uint8_t i = 0; i < COMPLETE_TIP_TEXT_COUNT; i++) {
        complete_tip_texts[i] = NULL;
    }
}

int ui_display_pump_complete_tip(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(complete_tip_root);
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
        LOGI("===> pump complete tip fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}