/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "pump_ui_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_DEBUG_OTHER_PAGE           (1)
#define HOME_MODE_MAX_COUNT                        (7)
#define HOME_SPEED_MAX_COUNT                       (3)
#define HOME_BLE_MAX_COUNT                         (2)
#define HOME_BATTERY_MAX_COUNT                     (6)
#define HOME_CHARGE_BATTERY_MAX_COUNT              (6)

#define PUMP_IMG_SEL_COLOR                         0xB3686D
#define PUMP_IMG_UNSEL_COLOR                       0xF7EFEF
#define PUMP_TEXT_SEL_COLOR                        0xB3686D
#define PUMP_TEXT_UNSEL_COLOR                      0xFFFFFF
#define PUMP_TEXT_BLACK_COLOR                      0x000000
#define PUMP_ARC_SEL_COLOR                         0xB3686D
#define PUMP_ARC_UNSEL_COLOR                       0xF7EFEF
#define PUMP_MODE_SEL_START_COLOR                  0xB3686D
#define PUMP_MODE_SEL_END_COLOR                    0xF1D9D6

#define STIMULATE_MODE_ICON_X                      (109 + 4)
#define STIMULATE_MODE_ICON_Y                      (116 + 1)
#define STIMULATE_MODE_LABEL_TEXT_X                (143 + 3)
#define STIMULATE_MODE_LABEL_TEXT_Y                (110 + 4)

#define LACTATION_MODE_ICON_X                      (110 + 4)
#define LACTATION_MODE_ICON_Y                      (189 + 1)
#define LACTATION_MODE_LABEL_TEXT_X                (144 + 3)
#define LACTATION_MODE_LABEL_TEXT_Y                (186 + 4)

#define MIX_MODE_ICON_X                            (105 + 4)
#define MIX_MODE_ICON_Y                            (263 + 1)
#define MIX_MODE_LABEL_TEXT_X                      (176 + 3)
#define MIX_MODE_LABEL_TEXT_Y                      (263 + 4)

#define INITIAL_MODE_ICON_X                        (105 + 4)
#define INITIAL_MODE_ICON_Y                        (263 + 1)
#define INITIAL_MODE_LABEL_TEXT_X                  (176 + 3)
#define INITIAL_MODE_LABEL_TEXT_Y                  (263 + 4)

#define MILK_MODE_ICON_X                           (105 + 4)
#define MILK_MODE_ICON_Y                           (263 + 1)
#define MILK_MODE_LABEL_TEXT_X                     (176 + 3)
#define MILK_MODE_LABEL_TEXT_Y                     (263 + 4)

#define PP_MODE_ICON_X                             (105 + 4)
#define PP_MODE_ICON_Y                             (263 + 1)
#define PP_MODE_LABEL_TEXT_X                       (176 + 3)
#define PP_MODE_LABEL_TEXT_Y                       (263 + 4)

#define PER_MODE_ICON_X                            (105 + 4)
#define PER_MODE_ICON_Y                            (263 + 1)
#define PER_MODE_LABEL_TEXT_X                      (176 + 3)
#define PER_MODE_LABEL_TEXT_Y                      (263 + 4)

typedef struct {
    const char *name;
    uint16_t icon_x;
    uint16_t icon_y;
    uint16_t label_text_x;
    uint16_t label_text_y;
} home_mode_label_info_t;

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

static PSRAM_DATA_LOC uint8_t home_ble_connect_data[] = {
    #include "../../../resources/img_baj_bin/ble_connect_14x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_ble_disconnect_data[] = {
    #include "../../../resources/img_baj_bin/ble_disconnect_14x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_empty_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_0_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_25_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_50_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_75_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_100_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_battery_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_charge_battery_data[] = {
    #include "../../../resources/img_baj_bin/home_uncharge_battery_50_37x21_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_stimulate_icon[] = {
    #include "../../../resources/img_baj_bin/home_stimulate_icon_19x19_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_lactation_icon[] = {
    #include "../../../resources/img_baj_bin/home_lactation_icon_15x17_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_mix_icon[] = {
    #include "../../../resources/img_baj_bin/home_mix_icon_20x20_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_pp_icon[] = {
    #include "../../../resources/img_baj_bin/home_pp_icon_14x13_a4_q99_422.txt"
};


static PSRAM_DATA_LOC uint8_t home_per_icon[] = {
    #include "../../../resources/img_baj_bin/home_personalization_icon_14x13_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_milk_icon[] = {
    #include "../../../resources/img_baj_bin/home_milk_icon_14x13_a4_q99_422.txt"
};

static PSRAM_DATA_LOC uint8_t home_initial_icon[] = {
    #include "../../../resources/img_baj_bin/home_initia_icon_20x20_a4_q99_422.txt"
};

static ui_img_data_t home_battery_icon[HOME_BLE_MAX_COUNT] = {
    { home_ble_disconnect_data, sizeof(home_ble_disconnect_data)},
    { home_ble_connect_data, sizeof(home_ble_connect_data)},
};

static ui_img_data_t home_ble_icon[HOME_BLE_MAX_COUNT] = {
    { home_ble_disconnect_data, sizeof(home_ble_disconnect_data)},
    { home_ble_connect_data, sizeof(home_ble_connect_data)},
};

static ui_img_data_t home_mode_icon[HOME_MODE_MAX_COUNT] = {
    { home_stimulate_icon, sizeof(home_stimulate_icon)},
    { home_lactation_icon, sizeof(home_lactation_icon)},
    { home_mix_icon, sizeof(home_mix_icon)},
    { home_per_icon, sizeof(home_per_icon)},
    { home_pp_icon, sizeof(home_pp_icon)},
    { home_milk_icon, sizeof(home_milk_icon)},
    { home_initial_icon, sizeof(home_initial_icon)},
};

const static home_mode_label_info_t home_mode_infos[HOME_MODE_MAX_COUNT] = {
    { "Stimulate", 
        STIMULATE_MODE_ICON_X,       STIMULATE_MODE_ICON_Y, 
        STIMULATE_MODE_LABEL_TEXT_X, STIMULATE_MODE_LABEL_TEXT_Y
    },
    { "Lactation", 
        LACTATION_MODE_ICON_X,       LACTATION_MODE_ICON_Y,
        LACTATION_MODE_LABEL_TEXT_X, LACTATION_MODE_LABEL_TEXT_Y
    },
    { "Mix",       
        MIX_MODE_ICON_X,             MIX_MODE_ICON_Y, 
        MIX_MODE_LABEL_TEXT_X,       MIX_MODE_LABEL_TEXT_Y
    },
    { "Per",         
        PER_MODE_ICON_X,             PER_MODE_ICON_Y, 
        PER_MODE_LABEL_TEXT_X,       PER_MODE_LABEL_TEXT_Y
    },
    { "PP",          
        PP_MODE_ICON_X,              PP_MODE_ICON_Y, 
        PP_MODE_LABEL_TEXT_X,        PP_MODE_LABEL_TEXT_Y
    },
    { "Milk",        
        MILK_MODE_ICON_X,            MILK_MODE_ICON_Y, 
        MILK_MODE_LABEL_TEXT_X,      MILK_MODE_LABEL_TEXT_Y
    },
    { "Initial",        
        INITIAL_MODE_ICON_X,         INITIAL_MODE_ICON_Y, 
        INITIAL_MODE_LABEL_TEXT_X,   INITIAL_MODE_LABEL_TEXT_Y
    },
};

const static char *home_speed_names[HOME_SPEED_MAX_COUNT] = {
    "Low",
    "Medium",
    "High",
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
        EXCEPTION_CHECK_VOID(pump_bg_img);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(pump_bg_img, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(pump_bg_img, LV_OBJ_FLAG_HIDDEN);
#if 1
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_data,
         sizeof(home_bg_data), "baj");
#else
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
            sizeof(home_bg_1_data), "png");
#endif
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
        EXCEPTION_CHECK_VOID(ble_img);
    }
    lv_fs_path_ex_t ble_img_path;
    lv_fs_make_path_from_buffer(&ble_img_path, LV_FS_MEMFS_LETTER, home_ble_connect_data,
         sizeof(home_ble_connect_data), "baj");
    lv_image_set_src(ble_img, (const void *)&ble_img_path);
    lv_obj_align(ble_img, LV_ALIGN_TOP_LEFT, 138 + 6 - 1, 40);
}

static void pump_home_battery_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (battery_img == NULL || !lv_obj_is_valid(battery_img)) {
        battery_img = lv_image_create(pump_root);
        EXCEPTION_CHECK_VOID(battery_img);
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
        EXCEPTION_CHECK_VOID(home_line_img);
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
        EXCEPTION_CHECK_VOID(pump_time_colon);
    }

    if (pump_time_minute == NULL || !lv_obj_is_valid(pump_time_minute)) {
        pump_time_minute = lv_label_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_time_minute);
    }

    if (pump_time_second == NULL || !lv_obj_is_valid(pump_time_second)) {
        pump_time_second = lv_label_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_time_second);
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
        EXCEPTION_CHECK_VOID(pump_left_level_label);
    }

    if (pump_left_level_label_txt == NULL || !lv_obj_is_valid(pump_left_level_label_txt)) {
        pump_left_level_label_txt = lv_label_create(pump_left_level_label);
        EXCEPTION_CHECK_VOID(pump_left_level_label_txt);
    }
    lv_obj_clear_flag(pump_left_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_left_level_label, 57, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_left_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_left_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_left_level_label, lv_color_hex(PUMP_IMG_SEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(pump_left_level_label, 19, 0);  /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_left_level_label_txt, "L6");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_left_level_label_txt, lv_color_hex(PUMP_TEXT_UNSEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_left_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_left_level_label_txt); /// 文字居中
    lv_obj_align(pump_left_level_label, LV_ALIGN_TOP_LEFT, 66 + 4, 102);    /// 设置左档位标签位置
}

static void pump_home_left_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_left_level_arc == NULL || !lv_obj_is_valid(pump_left_level_arc)) {
        pump_left_level_arc = lv_arc_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_left_level_arc);
    }
    lv_obj_set_size(pump_left_level_arc, 310, 310);
    lv_obj_center(pump_left_level_arc);
    
    lv_arc_set_bg_angles(pump_left_level_arc, 160, 212);
    lv_arc_set_angles(pump_left_level_arc, 160, 180);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_UNSEL_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_SEL_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_left_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_right_level_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_right_level_label == NULL || !lv_obj_is_valid(pump_right_level_label)) {
        pump_right_level_label = lv_obj_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_right_level_label);
    }

    if (pump_right_level_label_txt == NULL || !lv_obj_is_valid(pump_right_level_label_txt)) {
        pump_right_level_label_txt = lv_label_create(pump_right_level_label);
        EXCEPTION_CHECK_VOID(pump_right_level_label_txt);

    }
    lv_obj_clear_flag(pump_right_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_right_level_label, 57, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_right_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_right_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_right_level_label, lv_color_hex(PUMP_IMG_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(pump_right_level_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_right_level_label_txt, "R12");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_right_level_label_txt, lv_color_hex(PUMP_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_right_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_right_level_label_txt); /// 文字居中
    lv_obj_align(pump_right_level_label, LV_ALIGN_TOP_LEFT, 236 + 2, 102);    /// 设置右档位标签位置
    
}

static void pump_home_right_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_right_level_arc == NULL || !lv_obj_is_valid(pump_right_level_arc)) {
        pump_right_level_arc = lv_arc_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_right_level_arc);
    }
    lv_obj_set_size(pump_right_level_arc, 310, 310);
    lv_obj_center(pump_right_level_arc);
    
    lv_arc_set_bg_angles(pump_right_level_arc, 328, 380);
    lv_arc_set_angles(pump_right_level_arc, 328, 380);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_UNSEL_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_SEL_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_right_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_speed_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_speed_label == NULL || !lv_obj_is_valid(pump_speed_label)) {
        pump_speed_label = lv_obj_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_speed_label);
    }

    if (pump_speed_label_txt == NULL || !lv_obj_is_valid(pump_speed_label_txt)) {
        pump_speed_label_txt = lv_label_create(pump_speed_label);
        EXCEPTION_CHECK_VOID(pump_speed_label_txt);
    }
    lv_obj_clear_flag(pump_speed_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(pump_speed_label, 98, 38); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_speed_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(pump_speed_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_speed_label, lv_color_hex(PUMP_IMG_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(pump_speed_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    lv_label_set_text(pump_speed_label_txt, "Medium");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_speed_label_txt, lv_color_hex(PUMP_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_speed_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_speed_label_txt); /// 文字居中
    lv_obj_align(pump_speed_label, LV_ALIGN_TOP_LEFT, 131 + 2, 102);    /// 设置右档位标签位置
}

static void pump_home_mode_label_update(void)
{
    EXCEPTION_CHECK_VOID(pump_root);
    if (pump_mode_label == NULL || !lv_obj_is_valid(pump_mode_label)) {
        pump_mode_label = lv_obj_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_mode_label);
    }
    lv_obj_clear_flag(pump_mode_label, LV_OBJ_FLAG_SCROLLABLE);    /// 不要滚动条
    lv_obj_set_size(pump_mode_label, 158, 44); /// 设置容器尺寸
    lv_obj_set_style_border_width(pump_mode_label, 0, 0);          /// 无边框
    lv_obj_set_style_bg_opa(pump_mode_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(pump_mode_label, lv_color_hex(PUMP_MODE_SEL_START_COLOR), 0);
    lv_obj_set_style_bg_grad_color(pump_mode_label, lv_color_hex(PUMP_MODE_SEL_END_COLOR), 0); 
    lv_obj_set_style_bg_grad_dir(pump_mode_label, LV_GRAD_DIR_HOR, 0); /// 横向渐变
    lv_obj_set_style_radius(pump_mode_label, 22, 0);   /// 高度 44 → 圆角设 22（或更大）
    lv_obj_align(pump_mode_label, LV_ALIGN_TOP_LEFT, 101 + 4, 283);    /// 设置模式标签位置

    if (pump_mode_label_txt == NULL || !lv_obj_is_valid(pump_mode_label_txt)) {
        pump_mode_label_txt = lv_label_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_mode_label_txt);
    }
    lv_label_set_text(pump_mode_label_txt, "Stimulate");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_mode_label_txt, lv_color_hex(PUMP_TEXT_UNSEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_mode_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_align(pump_mode_label_txt, LV_ALIGN_TOP_LEFT, 149 + 3, 294 + 4);

    if (pump_mode_icon == NULL || !lv_obj_is_valid(pump_mode_icon)) {
        pump_mode_icon = lv_image_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_mode_icon);
    }
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
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_home_init();  
#else
    // ui_display_pump_left_level_prepare(thiz);
    ui_display_pump_mode_prepare(thiz);
#endif
}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    LVGL_OBJ_DELETE(pump_root);
#else
    // ui_display_pump_left_level_remove(thiz);
    ui_display_pump_mode_remove(thiz);
#endif
}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    pump_home_bg_update();
    static uint16_t status = 0;
    status++;
    if (status > 10){
        goto UI_EXIT;
    }
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
UI_EXIT:    
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> pump home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
#else
    // ui_display_pump_left_level(thiz);
    ui_display_pump_mode(thiz);
#endif
    return 0;
}
