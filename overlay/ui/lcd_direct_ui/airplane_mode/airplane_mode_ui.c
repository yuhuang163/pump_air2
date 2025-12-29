/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "airplane_mode_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_AIRPLANE_MODE_COMP_BG                          (0)
#define AIRPLANE_MODE_TEXT_COUNT                                  (3)
#define AIRPLANE_MODE_TITLE_COLOR                                 0x1C1C20
#define AIRPLANE_MODE_CONTENT_COLOR                               0x3C3D40

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
} airplane_mode_text_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
static PSRAM_DATA_LOC uint8_t complete_tip_bg_data[] = {
    #include "../../../resources/img_png_bin/travel_lock_cmp_360x360.txt"
};
#endif

/// 飞行模式主界面对象
static lv_obj_t * airplane_mode_root;
#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
static lv_obj_t * airplane_mode_bg_img;
#endif
static lv_obj_t * lock_icon;
static res_img_info_t lock_icon_info;
static lv_obj_t * button_icon;
static res_img_info_t button_icon_info;
static lv_obj_t * airplane_mode_texts[AIRPLANE_MODE_TEXT_COUNT];

const static airplane_mode_text_info_t airplane_mode_text_infos[AIRPLANE_MODE_TEXT_COUNT] = {
    { "The breast pump is locked", (68 + 3), (153) },
    { "Long press", (68 + 3), (196)},
    { "for 5 second to unlock", (68 + 3), (239)},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void airplane_mode_root_init(void)
{
    if (airplane_mode_root == NULL || !lv_obj_is_valid(airplane_mode_root)) {
        airplane_mode_root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(airplane_mode_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(airplane_mode_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(airplane_mode_root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(airplane_mode_root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(airplane_mode_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(airplane_mode_root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(airplane_mode_root, LV_OBJ_FLAG_CLICKABLE);           /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(airplane_mode_root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
static void airplane_mode_bg_update(void)
{
    static uint8_t status = 0;
    static uint32_t start_time = 0;
    if (airplane_mode_bg_img == NULL || !lv_obj_is_valid(airplane_mode_root)) {
        airplane_mode_bg_img = lv_image_create(airplane_mode_root);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(airplane_mode_bg_img, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(airplane_mode_bg_img, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, complete_tip_bg_data,
            sizeof(complete_tip_bg_data), "png");
        lv_image_set_src(airplane_mode_bg_img, (const void *)&bg_img_path);
        lv_obj_align(airplane_mode_bg_img, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif

static void pump_lock_icon_update(ui_manager_context_t *thiz)
{
    if (lock_icon == NULL || !lv_obj_is_valid(airplane_mode_root)) {
        lock_icon = lv_image_create(lv_screen_active());
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_TRAVEL_LOCK, &lock_icon_info);
    EXCEPTION_CHECK_VOID(lock_icon_info.data);
    lv_fs_path_ex_t complete_tip_img_path;
    lv_fs_make_path_from_buffer(&complete_tip_img_path, LV_FS_MEMFS_LETTER, lock_icon_info.data,
        lock_icon_info.size, "baj");
    lv_image_set_src(lock_icon, complete_tip_img_path.path);
    lv_obj_align(lock_icon, LV_ALIGN_TOP_LEFT, 148 + 4, 63);
}

static void pump_button_icon_update(ui_manager_context_t *thiz)
{
    if (button_icon == NULL || !lv_obj_is_valid(airplane_mode_root)) {
        button_icon = lv_image_create(lv_screen_active());
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_TRAVEL_LOCK_CONTENT, &button_icon_info);
    EXCEPTION_CHECK_VOID(button_icon_info.data);
    lv_fs_path_ex_t complete_tip_img_path;
    lv_fs_make_path_from_buffer(&complete_tip_img_path, LV_FS_MEMFS_LETTER, button_icon_info.data,
        button_icon_info.size, "baj");
    lv_image_set_src(button_icon, complete_tip_img_path.path);
    lv_obj_align(button_icon, LV_ALIGN_TOP_LEFT, 172 + 4, 185);
}

static void pump_airplane_mode_texts_update(ui_manager_context_t *thiz, uint8_t index)
{
    if (airplane_mode_texts[index] == NULL || !lv_obj_is_valid(airplane_mode_texts[index])) {
        airplane_mode_texts[index] = lv_label_create(airplane_mode_root);
    }
    lv_label_set_text(airplane_mode_texts[index], airplane_mode_text_infos[index].name);
    lv_obj_set_style_text_color(airplane_mode_texts[index], lv_color_hex(AIRPLANE_MODE_CONTENT_COLOR), 0);
    lv_obj_set_style_text_font(airplane_mode_texts[index], &lv_font_roboto_19_regular, 0);  /// 设置字体
    lv_obj_align(airplane_mode_texts[index], LV_ALIGN_TOP_LEFT,
        airplane_mode_text_infos[index].text_x, airplane_mode_text_infos[index].text_y);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_airplane_mode_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    airplane_mode_root_init();
}

void ui_display_airplane_mode_remove(ui_manager_context_t *thiz)
{
    common_ui_img_resource_deinit(thiz, &lock_icon_info);
    common_ui_img_resource_deinit(thiz, &button_icon_info);
    LVGL_OBJ_DELETE(airplane_mode_root); 
#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
    airplane_mode_bg_img = NULL;
#endif
    lock_icon = NULL;
    button_icon = NULL;
    for (uint8_t i = 0; i < AIRPLANE_MODE_TEXT_COUNT; i++) {
        airplane_mode_texts[i] = NULL;
    }
}

int ui_display_airplane_mode_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(airplane_mode_root);
#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
    airplane_mode_bg_update();
#endif
    pump_lock_icon_update(thiz);
    pump_button_icon_update(thiz);
    for (uint8_t i = 0; i < AIRPLANE_MODE_TEXT_COUNT; i++) {
        pump_airplane_mode_texts_update(thiz, i);
    }
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        thiz->priv.fps_count = 0;
        LOG_UI("===> airplane mode fps=%d", thiz->priv.fps_count);
    }
    return thiz->priv.fps_count;
}