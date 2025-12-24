/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"   // IWYU pragma: keep
#include "ui_img_define.h"     // IWYU pragma: keep
#include "ui_page_define.h"    // IWYU pragma: keep
#include "pump_ui_define.h"    // IWYU pragma: keep

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
    #include "../../../resources/img_bin/home_line_2x16_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    // #include "../../../resources/img_bin/home_com_bg_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_bin/ver_img_360x360_a4_q99_422.txt"
    #include "../../../resources/img_bin_422_a8/ver_img_360x360_a8_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_bg_1_data[] = {
    // #include "../../../resources/img_bin/home_com_bg_360x360_a4_q99_422.txt"
    #include "../../../resources/img_bin_png/ver_img_360x360.txt"
    // #include "../../../resources/img_bin/ver_img_360x360_a4_q99_422.txt"
    // #include "../../../resources/img_444_bin/ver_img_360x360_a8_q99_444.txt"
};

static PSRAM_DATA_LOC uint8_t home_ble_data[] = {
    #include "../../../resources/img_bin/ble_connect_14x21_a4_q99_422.txt"
    // #include "../../../resources/img_bin/pp_mode_360x360_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_data[] = {
    #include "../../../resources/img_bin/battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_love_data[] = {
    #include "../../../resources/img_bin/home_love_19x19_a4_q99_422.txt"
};


static ui_img_data_t home_images[HOME_IMG_COUNT] = {
    { home_line_data, sizeof(home_line_data) },
    { home_bg_data, sizeof(home_bg_data) },
    { home_ble_data, sizeof(home_ble_data) },
    { home_battery_data, sizeof(home_battery_data) },
    { home_love_data, sizeof(home_love_data) },
};

/// 泵奶主界面对象（白色背景容器）
static lv_obj_t * pump_root;
/// 动画背景图片对象
static lv_obj_t * pump_bg_img;
/// 电池图标对象
static lv_obj_t * battery_img;
/// 蓝牙图标对象
static lv_obj_t * ble_img;
/// 分割线对象
static lv_obj_t * home_line_img;
/// 时间标签对象
static lv_obj_t * pump_time_label;
/// 速度图标对象
static lv_obj_t * pump_speed_label;
/// 速度标签内部文字对象
static lv_obj_t * pump_speed_label_txt;
/// 左档位图标对象
static lv_obj_t * pump_left_level_img;
/// 左档位标签容器对象
static lv_obj_t * pump_left_level_label;
/// 左档位标签内部文字对象
static lv_obj_t * pump_left_level_label_txt;
/// 右档位图标对象
static lv_obj_t * pump_right_level_img;
/// 右档位标签容器对象
static lv_obj_t * pump_right_level_label;
/// 右档位标签内部文字对象
static lv_obj_t * pump_right_level_label_txt;
/// 模式标签对象
static lv_obj_t * pump_mode_label;
/// 模式图标对象
static lv_obj_t * pump_mode_img;
/// 模式标签内部文字对象
static lv_obj_t * pump_mode_label_txt;
/// 模式背景对象
static lv_obj_t * pump_mode_bg;
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
        // lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_data,
        //     sizeof(home_bg_data), "baj");
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
            sizeof(home_bg_1_data), "png");
    } else {
        // lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
        //  sizeof(home_bg_1_data), "baj");
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
            sizeof(home_bg_1_data), "png");
    }
    lv_image_set_src(pump_bg_img, (const void *)&bg_img_path);
    lv_obj_align(pump_bg_img, LV_ALIGN_CENTER, 0, 0);
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
    if (pump_time_label == NULL || !lv_obj_is_valid(pump_time_label)) {
        pump_time_label = lv_label_create(pump_root);
    }
    
    lv_label_set_text(pump_time_label, "12:54");
    lv_obj_set_style_text_font(pump_time_label, &lv_font_montserrat_48, 0);
    lv_obj_align(pump_time_label, LV_ALIGN_TOP_LEFT, 179, 196);
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
    // lv_obj_set_style_bg_opa(pump_left_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_left_level_label, lv_color_hex(PUMP_IMG_UNSELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_left_level_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_left_level_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_left_level_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_left_level_label, 6, 0);
    lv_obj_set_style_radius(pump_left_level_label, 19, 0);  /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_left_level_label_txt, "L6");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_left_level_label_txt, lv_color_hex(PUMP_TEXT_SELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_left_level_label_txt, &lv_font_montserrat_22, 0);  /// 设置字体
    lv_obj_center(pump_left_level_label_txt); /// 文字居中
    lv_obj_align(pump_left_level_label, LV_ALIGN_TOP_LEFT, 66 + 5, 102);    /// 设置左档位标签位置
}

static void pump_home_right_level_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    
    /// 如果对象不存在或无效，创建它们
    if (pump_right_level_label == NULL || !lv_obj_is_valid(pump_right_level_label)) {
        pump_right_level_label = lv_obj_create(pump_root);
    }

    if (pump_right_level_label_txt == NULL || !lv_obj_is_valid(pump_right_level_label_txt)) {
        pump_right_level_label_txt = lv_label_create(pump_right_level_label);
    }
    lv_obj_clear_flag(pump_right_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_right_level_label, 57, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_right_level_label, 0, 0);         /// 无边框
    // lv_obj_set_style_bg_opa(pump_right_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_right_level_label, lv_color_hex(PUMP_IMG_SELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_right_level_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_right_level_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_right_level_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_right_level_label, 6, 0);
    lv_obj_set_style_radius(pump_right_level_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_right_level_label_txt, "L9");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_right_level_label_txt, lv_color_hex(PUMP_TEXT_UNSELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_right_level_label_txt, &lv_font_montserrat_22, 0);  /// 设置字体
    lv_obj_center(pump_right_level_label_txt); /// 文字居中
    lv_obj_align(pump_right_level_label, LV_ALIGN_TOP_LEFT, 236 + 2, 102);    /// 设置右档位标签位置
    
}

static void pump_home_speed_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    
    /// 如果对象不存在或无效，创建它们
    if (pump_speed_label == NULL || !lv_obj_is_valid(pump_speed_label)) {
        pump_speed_label = lv_obj_create(pump_root);
    }

    if (pump_speed_label_txt == NULL || !lv_obj_is_valid(pump_speed_label_txt)) {
        pump_speed_label_txt = lv_label_create(pump_speed_label);
    }
    lv_obj_clear_flag(pump_speed_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_speed_label, 98, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_speed_label, 0, 0);         /// 无边框
    // lv_obj_set_style_bg_opa(pump_speed_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_speed_label, lv_color_hex(PUMP_IMG_SELECTED_COLOR), 0);   /// 设置背景颜色
    // lv_obj_set_style_pad_left(pump_speed_label, 12, 0);
    // lv_obj_set_style_pad_right(pump_speed_label, 12, 0);
    // lv_obj_set_style_pad_top(pump_speed_label, 6, 0);
    // lv_obj_set_style_pad_bottom(pump_speed_label, 6, 0);
    lv_obj_set_style_radius(pump_speed_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_speed_label_txt, "Medium");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_speed_label_txt, lv_color_hex(PUMP_TEXT_UNSELECTED_COLOR), 0);
    lv_obj_set_style_text_font(pump_speed_label_txt, &lv_font_montserrat_22, 0);  /// 设置字体
    lv_obj_center(pump_speed_label_txt); /// 文字居中
    lv_obj_align(pump_speed_label, LV_ALIGN_TOP_LEFT, 131 + 2, 102);    /// 设置右档位标签位置
}

static void pump_home_mode_label_update(void)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{
    pump_home_init();        
    pump_home_bg_update();     
    pump_home_ble_update();     
    pump_home_battery_update(); 
    pump_home_line_update();    
    pump_home_time_update();    
    pump_home_left_level_label_update();
    pump_home_right_level_label_update();
    pump_home_speed_label_update();
    // pump_home_mode_label_update();
}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{

}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
    pump_home_bg_update();
    pump_home_ble_update();
    pump_home_battery_update();
    pump_home_line_update();
    pump_home_time_update();
    pump_home_left_level_label_update();
    pump_home_right_level_label_update();
    pump_home_speed_label_update();
    // pump_home_mode_label_update();   
    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        LOGI("===> test 6 baj");
    }
    return 0;
}
