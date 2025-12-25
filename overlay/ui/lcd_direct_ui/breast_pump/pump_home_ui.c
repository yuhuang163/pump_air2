/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"   // IWYU pragma: keep
#include "ui_img_define.h"     // IWYU pragma: keep
#include "ui_page_define.h"    // IWYU pragma: keep
#include "pump_ui_define.h"    // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#define HOME_IMG_COUNT  5
static PSRAM_DATA_LOC uint8_t home_line_data[] = {
    #include "../../../resources/img_baj_bin/ble_line_icon_2x16_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    #include "../../../resources/img_baj_bin/home_root_bg_360x360_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_bg_1_data[] = {
    #include "../../../resources/img_png_bin/home_comp_360x360.txt"
};

static PSRAM_DATA_LOC uint8_t home_ble_data[] = {
    #include "../../../resources/img_baj_bin/ble_connect_14x21_a4_q99_422.txt"
    // #include "../../../resources/img_baj_bin/pp_mode_360x360_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_stimulate_icon[] = {
    #include "../../../resources/img_baj_bin/home_stimulate_icon_19x19_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_mode_bg_data[] = {
    #include "../../../resources/img_baj_bin/home_mode_background_158x45_a4_q99_422.txt"
};

static ui_img_data_t home_images[HOME_IMG_COUNT] = {
    { home_line_data, sizeof(home_line_data) },
    { home_bg_data, sizeof(home_bg_data) },
    { home_ble_data, sizeof(home_ble_data) },
    { home_battery_data, sizeof(home_battery_data) },
    { home_stimulate_icon, sizeof(home_stimulate_icon) },
};

static lv_obj_t * pump_root;
static lv_obj_t * pump_bg_img;
static lv_obj_t * battery_img;
static lv_obj_t * ble_img;
static lv_obj_t * home_line_img;
static lv_obj_t * pump_time_colon;
static lv_obj_t * pump_time_minute;
static lv_obj_t * pump_time_second;
static lv_obj_t * pump_speed_label;
static lv_obj_t * pump_speed_label_txt;
static lv_obj_t * pump_left_level_arc;
static lv_obj_t * pump_left_level_label;
static lv_obj_t * pump_left_level_label_txt;
static lv_obj_t * pump_right_level_arc;
static lv_obj_t * pump_right_level_label;
static lv_obj_t * pump_right_level_label_txt;
static lv_obj_t * pump_mode_label;
static lv_obj_t * pump_mode_icon;
static lv_obj_t * pump_mode_label_txt;

static uint32_t start_time = 0;
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_home_init(void)
{
    if (pump_root == NULL || !lv_obj_is_valid(pump_root)) {
        pump_root = lv_obj_create(lv_screen_active());
        lv_obj_set_size(pump_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
        lv_obj_set_style_bg_color(pump_root, lv_color_white(), 0);     /// 设置背景颜色为白色
        lv_obj_set_style_bg_opa(pump_root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
        lv_obj_set_style_border_width(pump_root, 0, 0);                /// 设置边框宽度为0（无边框）
        lv_obj_set_style_pad_all(pump_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
        lv_obj_align(pump_root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(pump_root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
        lv_obj_clear_flag(pump_root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
    }
}

static void pump_home_bg_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    static uint8_t status = 0;
    if (pump_bg_img == NULL || !lv_obj_is_valid(pump_bg_img)) {
        pump_bg_img = lv_image_create(pump_root);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(pump_bg_img, LV_OBJ_FLAG_HIDDEN);
        // lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_data,
        //     sizeof(home_bg_data), "baj");
        // lv_image_set_src(pump_bg_img, (const void *)&bg_img_path);
        // lv_obj_align(pump_bg_img, LV_ALIGN_CENTER, 0, 0);
    } else {
        // lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
        //  sizeof(home_bg_1_data), "baj");
        lv_obj_clear_flag(pump_bg_img, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
            sizeof(home_bg_1_data), "png");
        lv_image_set_src(pump_bg_img, (const void *)&bg_img_path);
        lv_obj_align(pump_bg_img, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}

static void pump_home_ble_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (ble_img == NULL || !lv_obj_is_valid(ble_img)) {
        ble_img = lv_image_create(pump_root);
    }
    lv_fs_path_ex_t ble_img_path;
    lv_fs_make_path_from_buffer(&ble_img_path, LV_FS_MEMFS_LETTER, home_ble_data,
         sizeof(home_ble_data), "baj");
    lv_image_set_src(ble_img, (const void *)&ble_img_path);
    // lv_obj_align(ble_img, LV_ALIGN_TOP_LEFT, 138, 39);
    lv_obj_align(ble_img, LV_ALIGN_TOP_LEFT, 138 + 6 - 1, 40);
}

static void pump_home_battery_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (battery_img == NULL || !lv_obj_is_valid(battery_img)) {
        battery_img = lv_image_create(pump_root);
    }
    lv_fs_path_ex_t battery_img_path;
    lv_fs_make_path_from_buffer(&battery_img_path, LV_FS_MEMFS_LETTER, home_battery_data,
         sizeof(home_battery_data), "baj");
    lv_image_set_src(battery_img, (const void *)&battery_img_path);
    // lv_obj_align(battery_img, LV_ALIGN_TOP_LEFT, 185, 39);
    lv_obj_align(battery_img, LV_ALIGN_TOP_LEFT, 185 + 6 -1, 40);
}

static void pump_home_line_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (home_line_img == NULL || !lv_obj_is_valid(home_line_img)) {
        home_line_img = lv_image_create(pump_root);
    }
    lv_fs_path_ex_t home_line_img_path;
    lv_fs_make_path_from_buffer(&home_line_img_path, LV_FS_MEMFS_LETTER, home_line_data,
         sizeof(home_line_data), "baj");
    lv_image_set_src(home_line_img, (const void *)&home_line_img_path);
    lv_obj_align(home_line_img, LV_ALIGN_TOP_LEFT, 168 + 5, 42);
}

static void pump_home_time_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_time_colon == NULL || !lv_obj_is_valid(pump_time_colon)) {
        pump_time_colon = lv_label_create(pump_root);
    }
    if (pump_time_minute == NULL || !lv_obj_is_valid(pump_time_minute)) {
        pump_time_minute = lv_label_create(pump_root);
    }
    if (pump_time_second == NULL || !lv_obj_is_valid(pump_time_second)) {
        pump_time_second = lv_label_create(pump_root);
    }
    lv_label_set_text(pump_time_minute, "12");
    lv_obj_set_style_text_font(pump_time_minute, &lv_font_roboto_81_medium, 0);
    lv_obj_align(pump_time_minute, LV_ALIGN_TOP_LEFT, 80, 168);
    lv_label_set_text(pump_time_colon, ":");
    lv_obj_set_style_text_font(pump_time_colon, &lv_font_roboto_68_medium, 0);
    lv_obj_align(pump_time_colon, LV_ALIGN_TOP_LEFT, 175, 179);
    lv_label_set_text(pump_time_second, "54");
    lv_obj_set_style_text_font(pump_time_second, &lv_font_roboto_81_medium, 0);
    lv_obj_align(pump_time_second, LV_ALIGN_TOP_LEFT, 196, 168);
}

static void pump_home_left_level_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_left_level_label == NULL || !lv_obj_is_valid(pump_left_level_label)) {
        pump_left_level_label = lv_obj_create(pump_root);
    }
    if (pump_left_level_label_txt == NULL || !lv_obj_is_valid(pump_left_level_label_txt)) {
        pump_left_level_label_txt = lv_label_create(pump_left_level_label);
    }
    
    lv_obj_clear_flag(pump_left_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_left_level_label, 57, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_left_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_left_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_left_level_label, lv_color_hex(PUMP_IMG_SELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_left_level_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_left_level_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_left_level_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_left_level_label, 6, 0);
    lv_obj_set_style_radius(pump_left_level_label, 19, 0);  /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_left_level_label_txt, "L6");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_left_level_label_txt, lv_color_hex(PUMP_TEXT_UNSELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_left_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_left_level_label_txt); /// 文字居中
    lv_obj_align(pump_left_level_label, LV_ALIGN_TOP_LEFT, 66 + 4, 102);    /// 设置左档位标签位置
}

static void pump_home_left_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_left_level_arc == NULL || !lv_obj_is_valid(pump_left_level_arc)) {
        pump_left_level_arc = lv_arc_create(pump_root);
    }
    lv_obj_set_size(pump_left_level_arc, 310, 310);
    lv_obj_center(pump_left_level_arc);
    
    lv_arc_set_bg_angles(pump_left_level_arc, 160, 212);
    lv_arc_set_angles(pump_left_level_arc, 160, 180);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_UNSELECTED_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_SELECTED_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_left_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_right_level_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_right_level_label == NULL || !lv_obj_is_valid(pump_right_level_label)) {
        pump_right_level_label = lv_obj_create(pump_root);
    }

    if (pump_right_level_label_txt == NULL || !lv_obj_is_valid(pump_right_level_label_txt)) {
        pump_right_level_label_txt = lv_label_create(pump_right_level_label);
    }
    lv_obj_clear_flag(pump_right_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_right_level_label, 57, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_right_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_right_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_right_level_label, lv_color_hex(PUMP_IMG_UNSELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_right_level_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_right_level_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_right_level_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_right_level_label, 6, 0);
    lv_obj_set_style_radius(pump_right_level_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_right_level_label_txt, "R12");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_right_level_label_txt, lv_color_hex(PUMP_TEXT_SELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_right_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_right_level_label_txt); /// 文字居中
    lv_obj_align(pump_right_level_label, LV_ALIGN_TOP_LEFT, 236 + 2, 102);    /// 设置右档位标签位置
    
}

static void pump_home_right_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_right_level_arc == NULL || !lv_obj_is_valid(pump_right_level_arc)) {
        pump_right_level_arc = lv_arc_create(pump_root);
    }
    lv_obj_set_size(pump_right_level_arc, 310, 310);
    lv_obj_center(pump_right_level_arc);
    
    lv_arc_set_bg_angles(pump_right_level_arc, 328, 380);
    lv_arc_set_angles(pump_right_level_arc, 328, 380);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_UNSELECTED_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_SELECTED_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_right_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_speed_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_speed_label == NULL || !lv_obj_is_valid(pump_speed_label)) {
        pump_speed_label = lv_obj_create(pump_root);
    }

    if (pump_speed_label_txt == NULL || !lv_obj_is_valid(pump_speed_label_txt)) {
        pump_speed_label_txt = lv_label_create(pump_speed_label);
    }
    lv_obj_clear_flag(pump_speed_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_speed_label, 98, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_speed_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_speed_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_speed_label, lv_color_hex(PUMP_IMG_UNSELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_speed_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_speed_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_speed_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_speed_label, 6, 0);
    lv_obj_set_style_radius(pump_speed_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_speed_label_txt, "Medium");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_speed_label_txt, lv_color_hex(PUMP_TEXT_SELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_speed_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_speed_label_txt); /// 文字居中
    lv_obj_align(pump_speed_label, LV_ALIGN_TOP_LEFT, 131 + 2, 102);    /// 设置右档位标签位置
}

static void pump_home_mode_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_mode_label == NULL || !lv_obj_is_valid(pump_mode_label)) {
        pump_mode_label = lv_image_create(pump_root);
    }

    if (pump_mode_icon == NULL || !lv_obj_is_valid(pump_mode_icon)) {
        pump_mode_icon = lv_image_create(pump_root);
    }

    if (pump_mode_label_txt == NULL || !lv_obj_is_valid(pump_mode_label_txt)) {
        pump_mode_label_txt = lv_label_create(pump_mode_label);
    }

    lv_fs_path_ex_t home_mode_bg_path;
    lv_fs_make_path_from_buffer(&home_mode_bg_path, LV_FS_MEMFS_LETTER, home_mode_bg_data,
        sizeof(home_mode_bg_data), "baj");
    lv_image_set_src(pump_mode_label, (const void *)&home_mode_bg_path);
    lv_label_set_text(pump_mode_label_txt, "Stimulate");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_mode_label_txt, lv_color_hex(PUMP_TEXT_UNSELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_mode_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_align(pump_mode_label_txt, LV_ALIGN_TOP_LEFT, 48, 15);
    lv_obj_align(pump_mode_label, LV_ALIGN_TOP_LEFT, 101 + 4, 283);    /// 设置模式标签位置

    lv_fs_path_ex_t mode_icon_img_path;
    lv_fs_make_path_from_buffer(&mode_icon_img_path, LV_FS_MEMFS_LETTER, home_stimulate_icon,
        sizeof(home_stimulate_icon), "baj");
    lv_image_set_src(pump_mode_icon, (const void *)&mode_icon_img_path);
    lv_obj_align(pump_mode_icon, LV_ALIGN_TOP_LEFT, 122 + 4, 283 + 15);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{
#if 0
    pump_home_init();  
#else
    ui_display_pump_left_level_prepare(thiz);
#endif
}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
#if 0
    LVGL_OBJ_DELETE(pump_root);
#else
    ui_display_pump_left_level_remove(thiz);
#endif
}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
#if 0
    pump_home_bg_update();
    pump_home_ble_update();
    pump_home_battery_update();
    pump_home_line_update();
    pump_home_time_update();
    pump_home_left_level_label_update();
    pump_home_left_level_arc_update();
    pump_home_right_level_label_update();
    pump_home_right_level_arc_update();
    pump_home_speed_label_update();
    pump_home_mode_label_update();   
    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        LOGI("===> test 12 baj");
    }
#else
    ui_display_pump_left_level(thiz);
#endif
    return 0;
}
