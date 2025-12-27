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
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_COMP_BG                    (0)
#define IS_SUPPORT_HOME_DEBUG_OTHER_PAGE           (0)
#define HOME_MODE_MAX_COUNT                        (7)
#define HOME_SPEED_MAX_COUNT                       (3)
#define HOME_BLE_MAX_COUNT                         (2)
#define HOME_BATTERY_MAX_COUNT                     (6)
#define HOME_CHARGE_BATTERY_MAX_COUNT              (5)

#define PUMP_IMG_SEL_COLOR                         0xB3686D
#define PUMP_IMG_UNSEL_COLOR                       0xF7EFEF
#define PUMP_TEXT_SEL_COLOR                        0xB3686D
#define PUMP_TEXT_UNSEL_COLOR                      0xFFFFFF
#define PUMP_TEXT_BLACK_COLOR                      0x000000
#define PUMP_ARC_SEL_COLOR                         0xB3686D
#define PUMP_ARC_UNSEL_COLOR                       0xF7EFEF
#define PUMP_MODE_SEL_START_COLOR                  0xB3686D
#define PUMP_MODE_SEL_END_COLOR                    0xF1D9D6
#define PUMP_LINE_COLOR                            0x1C1C20

#define STIMULATE_MODE_ICON_X                      (122 + 4)
#define STIMULATE_MODE_ICON_Y                      (283 + 15)
#define STIMULATE_MODE_LABEL_TEXT_X                (149 + 3)
#define STIMULATE_MODE_LABEL_TEXT_Y                (294 + 4)

#define LACTATION_MODE_ICON_X                      (STIMULATE_MODE_ICON_X + 2)
#define LACTATION_MODE_ICON_Y                      (STIMULATE_MODE_ICON_Y)
#define LACTATION_MODE_LABEL_TEXT_X                (STIMULATE_MODE_LABEL_TEXT_X + 2)
#define LACTATION_MODE_LABEL_TEXT_Y                (STIMULATE_MODE_LABEL_TEXT_Y)

#define MIX_MODE_ICON_X                            (STIMULATE_MODE_ICON_X+ 28)
#define MIX_MODE_ICON_Y                            (STIMULATE_MODE_ICON_Y)
#define MIX_MODE_LABEL_TEXT_X                      (STIMULATE_MODE_LABEL_TEXT_X + 28)
#define MIX_MODE_LABEL_TEXT_Y                      (STIMULATE_MODE_LABEL_TEXT_Y)

#define INITIAL_MODE_ICON_X                        (STIMULATE_MODE_ICON_X - 7)
#define INITIAL_MODE_ICON_Y                        (STIMULATE_MODE_ICON_Y)
#define INITIAL_MODE_LABEL_TEXT_X                  (STIMULATE_MODE_LABEL_TEXT_X - 11)
#define INITIAL_MODE_LABEL_TEXT_Y                  (STIMULATE_MODE_LABEL_TEXT_Y)

#define MILK_MODE_ICON_X                           (STIMULATE_MODE_ICON_X - 1)
#define MILK_MODE_ICON_Y                           (STIMULATE_MODE_ICON_Y)
#define MILK_MODE_LABEL_TEXT_X                     (STIMULATE_MODE_LABEL_TEXT_X - 5)
#define MILK_MODE_LABEL_TEXT_Y                     (STIMULATE_MODE_LABEL_TEXT_Y)

#define PP_MODE_ICON_X                             (STIMULATE_MODE_ICON_X + 37)
#define PP_MODE_ICON_Y                             (STIMULATE_MODE_ICON_Y)
#define PP_MODE_LABEL_TEXT_X                       (STIMULATE_MODE_LABEL_TEXT_X + 28)
#define PP_MODE_LABEL_TEXT_Y                       (STIMULATE_MODE_LABEL_TEXT_Y)

#define PER_MODE_ICON_X                            (STIMULATE_MODE_ICON_X - 9)
#define PER_MODE_ICON_Y                            (STIMULATE_MODE_ICON_Y)
#define PER_MODE_LABEL_TEXT_X                      (STIMULATE_MODE_LABEL_TEXT_X - 7)
#define PER_MODE_LABEL_TEXT_Y                      (STIMULATE_MODE_LABEL_TEXT_Y)

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
    uint16_t label_text_x;
    uint16_t label_text_y;
} home_mode_label_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_HOME_COMP_BG
static PSRAM_DATA_LOC uint8_t home_bg_1_data[] = {
    #include "../../../resources/img_png_bin/home_comp_360x360.txt"
};
#endif

const static home_mode_label_info_t home_mode_infos[HOME_MODE_MAX_COUNT] = {
    { "Stimulate", RESOURCE_ID_HOME_STIMULATE_ICON, 
      STIMULATE_MODE_ICON_X,       STIMULATE_MODE_ICON_Y, 
      STIMULATE_MODE_LABEL_TEXT_X, STIMULATE_MODE_LABEL_TEXT_Y
    },
    { "Lactation", RESOURCE_ID_HOME_LACTATION_ICON, 
      LACTATION_MODE_ICON_X,       LACTATION_MODE_ICON_Y,
      LACTATION_MODE_LABEL_TEXT_X, LACTATION_MODE_LABEL_TEXT_Y 
    },
    { "Mix",  RESOURCE_ID_HOME_MIX_ICON,     
      MIX_MODE_ICON_X,             MIX_MODE_ICON_Y, 
      MIX_MODE_LABEL_TEXT_X,       MIX_MODE_LABEL_TEXT_Y
    },
    { "Personalization", RESOURCE_ID_HOME_PERSONALIZATION_ICON,     
      PER_MODE_ICON_X,             PER_MODE_ICON_Y, 
      PER_MODE_LABEL_TEXT_X,       PER_MODE_LABEL_TEXT_Y
    },
    { "PP", RESOURCE_ID_HOME_PP_ICON,     
      PP_MODE_ICON_X,              PP_MODE_ICON_Y, 
      PP_MODE_LABEL_TEXT_X,        PP_MODE_LABEL_TEXT_Y
    },
    { "Milk supply...", RESOURCE_ID_HOME_MILK_ICON,     
      MILK_MODE_ICON_X,            MILK_MODE_ICON_Y, 
      MILK_MODE_LABEL_TEXT_X,      MILK_MODE_LABEL_TEXT_Y
    },
    { "Initiating lact...", RESOURCE_ID_HOME_INITIA_ICON,     
      INITIAL_MODE_ICON_X,         INITIAL_MODE_ICON_Y, 
      INITIAL_MODE_LABEL_TEXT_X,   INITIAL_MODE_LABEL_TEXT_Y
    },
};

const static uint16_t home_battery_ids[HOME_BATTERY_MAX_COUNT] = {
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_0,
    RESOURCE_ID_HOME_UNCHARGE_LOW_POWER,
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_25,
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_50,
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_75,
    RESOURCE_ID_HOME_UNCHARGE_BATTERY_100,
};

const static uint16_t home_charge_battery_ids[HOME_CHARGE_BATTERY_MAX_COUNT] = {
    RESOURCE_ID_HOME_CHARGE_0,
    RESOURCE_ID_HOME_CHARGE_25,
    RESOURCE_ID_HOME_CHARGE_50,
    RESOURCE_ID_HOME_CHARGE_75,
    RESOURCE_ID_HOME_CHARGE_100,
};

static lv_obj_t * pump_root;
static lv_obj_t * pump_bg_img;
static res_img_info_t pump_home_bg_info;

static lv_obj_t * battery_img;
static res_img_info_t battery_info;

static lv_obj_t * ble_img;
static res_img_info_t ble_info;

static lv_obj_t * home_line_icon;
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
static res_img_info_t pump_mode_icon_info;
static lv_obj_t * pump_mode_label_txt;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_home_img_init(ui_manager_context_t *thiz, uint16_t id, res_img_info_t *info)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(thiz->priv.res);
    EXCEPTION_CHECK_VOID(info);
    int ret = thiz->priv.res->res_ui_img_data_load(id, info);
    if (ret != 0) {
        LOGE("===> load img data failed, id=%u ret=%d", (unsigned)id, ret);
    }
}

static void pump_home_img_deinit(ui_manager_context_t *thiz, res_img_info_t *info)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(thiz->priv.res);
    EXCEPTION_CHECK_VOID(info);
    thiz->priv.res->res_ui_img_data_unload(info);
}

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

static void pump_home_bg_update(ui_manager_context_t *thiz)
{
    pump_home_img_init(thiz, RESOURCE_ID_PUMP_HOME_BG, &pump_home_bg_info);
    EXCEPTION_CHECK_VOID(pump_home_bg_info.data);
    static uint32_t start_time = 0;
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
#if IS_SUPPORT_HOME_COMP_BG
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_1_data,
            sizeof(home_bg_1_data), "png");
#else
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, pump_home_bg_info.data,
            pump_home_bg_info.size, "baj");
#endif
        lv_image_set_src(pump_bg_img, (const void *)&bg_img_path);
        lv_obj_align(pump_bg_img, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}

static void pump_home_ble_update(ui_manager_context_t *thiz, uint8_t is_connected)
{
    pump_home_img_init(thiz, is_connected == 0 ? RESOURCE_ID_BLE_CONNECT : RESOURCE_ID_BLE_DISCONNECT, &ble_info);
    EXCEPTION_CHECK_VOID(ble_info.data);
    if (ble_img == NULL || !lv_obj_is_valid(ble_img)) {
        ble_img = lv_image_create(pump_root);
        EXCEPTION_CHECK_VOID(ble_img);
    }
    lv_fs_path_ex_t ble_img_path;
    lv_fs_make_path_from_buffer(&ble_img_path, LV_FS_MEMFS_LETTER, ble_info.data,
        ble_info.size, "baj");
    lv_image_set_src(ble_img, (const void *)&ble_img_path);
    lv_obj_align(ble_img, LV_ALIGN_TOP_LEFT, 138 + 6 - 1, 40);
}

static void pump_home_battery_update(ui_manager_context_t *thiz, uint8_t battery_level, uint8_t is_charging)
{
    if (is_charging == 0) {
        if(battery_level >= HOME_BATTERY_MAX_COUNT) {
            battery_level = HOME_BATTERY_MAX_COUNT - 1;
        }
        pump_home_img_init(thiz, home_battery_ids[battery_level], &battery_info);
    } else {
        if(battery_level >= HOME_CHARGE_BATTERY_MAX_COUNT) {
            battery_level = HOME_CHARGE_BATTERY_MAX_COUNT - 1;
        }
        pump_home_img_init(thiz, home_charge_battery_ids[battery_level], &battery_info);
    }
    EXCEPTION_CHECK_VOID(battery_info.data);
    if (battery_img == NULL || !lv_obj_is_valid(battery_img)) {
        battery_img = lv_image_create(pump_root);
        EXCEPTION_CHECK_VOID(battery_img);
    }
    lv_fs_path_ex_t battery_img_path;
    lv_fs_make_path_from_buffer(&battery_img_path, LV_FS_MEMFS_LETTER, battery_info.data,
        battery_info.size, "baj");
    lv_image_set_src(battery_img, (const void *)&battery_img_path);
    lv_obj_align(battery_img, LV_ALIGN_TOP_LEFT, 185 + 5, 40);
}

static void pump_home_line_update(ui_manager_context_t *thiz)
{
    if (home_line_icon == NULL || !lv_obj_is_valid(home_line_icon)) {
        home_line_icon = lv_line_create(pump_root);
        EXCEPTION_CHECK_VOID(home_line_icon);
    }
    static lv_point_precise_t s_home_top_mid_sep_pts[] = { {0, 0}, {0, 15} };   /// 高度 12px 的竖线
    lv_line_set_points(home_line_icon, s_home_top_mid_sep_pts, 2); 
    lv_obj_align(home_line_icon, LV_ALIGN_TOP_LEFT, 168 + 5, 42 + 2); 
    lv_obj_set_style_line_width(home_line_icon, 1, 0); /// 线宽
    lv_obj_set_style_line_color(home_line_icon, lv_color_hex(PUMP_LINE_COLOR), 0); /// 颜色
    lv_obj_set_style_line_rounded(home_line_icon, true, 0); /// 圆角端点(可选)
}

static void pump_home_time_update(ui_manager_context_t *thiz, uint8_t minutes, uint8_t seconds)
{
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
    char minute_value[4] = {0};
    sprintf(minute_value, "%02d", minutes >= 60 ? 59 : minutes);
    lv_label_set_text(pump_time_minute, minute_value);
    lv_obj_set_style_text_font(pump_time_minute, &lv_font_roboto_81_medium, 0);
    lv_obj_align(pump_time_minute, LV_ALIGN_TOP_LEFT, 80, 168);
    lv_label_set_text(pump_time_colon, ":");
    lv_obj_set_style_text_font(pump_time_colon, &lv_font_roboto_68_medium, 0);
    lv_obj_align(pump_time_colon, LV_ALIGN_TOP_LEFT, 175, 179);
    char second_value[4] = {0};
    sprintf(second_value, "%02d", seconds >= 60 ? 59 : seconds);
    lv_label_set_text(pump_time_second, second_value);
    lv_obj_set_style_text_font(pump_time_second, &lv_font_roboto_81_medium, 0);
    lv_obj_align(pump_time_second, LV_ALIGN_TOP_LEFT, 196, 168);
}

static void pump_home_left_level_label_update(ui_manager_context_t *thiz, uint8_t level, uint8_t is_selected)
{
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
    lv_obj_set_style_bg_color(pump_left_level_label, lv_color_hex(is_selected ? PUMP_IMG_SEL_COLOR : PUMP_IMG_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(pump_left_level_label, 19, 0);  /// 高度 38 → 圆角设 19（或更大）
    char level_value[4] = {0};
    sprintf(level_value, "L%d", level);
    lv_label_set_text(pump_left_level_label_txt, level_value);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_left_level_label_txt, lv_color_hex(is_selected ? PUMP_TEXT_UNSEL_COLOR : PUMP_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_left_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_left_level_label_txt); /// 文字居中
    lv_obj_align(pump_left_level_label, LV_ALIGN_TOP_LEFT, 66 + 4, 102);    /// 设置左档位标签位置
}

static void pump_home_left_level_arc_update(ui_manager_context_t *thiz, uint8_t level)
{
    if (pump_left_level_arc == NULL || !lv_obj_is_valid(pump_left_level_arc)) {
        pump_left_level_arc = lv_arc_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_left_level_arc);
    }
    lv_obj_set_size(pump_left_level_arc, 310, 310);
    lv_obj_center(pump_left_level_arc);
    
    lv_arc_set_bg_angles(pump_left_level_arc, 160, 212);
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = 212;
    } else if (level == 0) {
        end_angle = 160;
    } else {
        end_angle = ((212 - 160) * level / 15) + 160;
    }
    lv_arc_set_angles(pump_left_level_arc, 160, end_angle);
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_UNSEL_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_left_level_arc, lv_color_hex(PUMP_ARC_SEL_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_left_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_left_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_right_level_label_update(ui_manager_context_t *thiz, uint8_t level, uint8_t is_selected)
{
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
    lv_obj_set_style_bg_color(pump_right_level_label, lv_color_hex(is_selected ? PUMP_IMG_SEL_COLOR : PUMP_IMG_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(pump_right_level_label, 19, 0);   /// 高度 38 → 圆角设 19（或更大）
    char level_value[4] = {0};
    sprintf(level_value, "R%d", level);
    lv_label_set_text(pump_right_level_label_txt, level_value);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_right_level_label_txt, lv_color_hex(is_selected ? PUMP_TEXT_UNSEL_COLOR : PUMP_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_right_level_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_right_level_label_txt); /// 文字居中
    lv_obj_align(pump_right_level_label, LV_ALIGN_TOP_LEFT, 236 + 2, 102);    /// 设置右档位标签位置
}

static void pump_home_right_level_arc_update(ui_manager_context_t *thiz, uint8_t level)
{
    if (pump_right_level_arc == NULL || !lv_obj_is_valid(pump_right_level_arc)) {
        pump_right_level_arc = lv_arc_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_right_level_arc);
    }
    lv_obj_set_size(pump_right_level_arc, 310, 310);
    lv_obj_center(pump_right_level_arc);
    
    lv_arc_set_bg_angles(pump_right_level_arc, 328, 380);
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = 328;
    } else if (level == 0) {
        end_angle = 380;
    } else {
        end_angle = ((380 - 328) * (15 - level) / 15) + 328;
    }
    lv_arc_set_angles(pump_right_level_arc, end_angle, 380);
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_UNSEL_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_right_level_arc, lv_color_hex(PUMP_ARC_SEL_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_right_level_arc, 10, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_right_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_speed_label_update(ui_manager_context_t *thiz, uint8_t speed)
{
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
    if(speed == 0) {
        lv_label_set_text(pump_speed_label_txt, "Comfort");  /// 设置 label 文字内容和样式
    } else if(speed == 1) {
        lv_label_set_text(pump_speed_label_txt, "Regular");  /// 设置 label 文字内容和样式
    } else {
        lv_label_set_text(pump_speed_label_txt, "Boost");  /// 设置 label 文字内容和样式
    }
    lv_obj_set_style_text_color(pump_speed_label_txt, lv_color_hex(PUMP_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_font(pump_speed_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    lv_obj_center(pump_speed_label_txt); /// 文字居中
    lv_obj_align(pump_speed_label, LV_ALIGN_TOP_LEFT, 131 + 2, 102);    /// 设置右档位标签位置
}

static void pump_home_mode_label_update(ui_manager_context_t *thiz, uint8_t mode_index)
{
    if (mode_index >= HOME_MODE_MAX_COUNT) {
        mode_index = HOME_MODE_MAX_COUNT - 1;
    }

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
    lv_label_set_text(pump_mode_label_txt, home_mode_infos[mode_index].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_mode_label_txt, lv_color_hex(PUMP_TEXT_UNSEL_COLOR), 0);
    if(mode_index < 3) {
        lv_obj_set_style_text_font(pump_mode_label_txt, &lv_font_roboto_21_medium, 0);  /// 设置字体
    } else {
        lv_obj_set_style_text_font(pump_mode_label_txt, &lv_font_roboto_16_medium, 0);  /// 设置字体
    }
    lv_obj_align(pump_mode_label_txt, LV_ALIGN_TOP_LEFT, 
        home_mode_infos[mode_index].label_text_x, home_mode_infos[mode_index].label_text_y);    /// 设置label文字位置

    pump_home_img_init(thiz, home_mode_infos[mode_index].img_id, &pump_mode_icon_info);
    EXCEPTION_CHECK_VOID(pump_mode_icon_info.data);
    if (pump_mode_icon == NULL || !lv_obj_is_valid(pump_mode_icon)) {
        pump_mode_icon = lv_image_create(pump_root);
        EXCEPTION_CHECK_VOID(pump_mode_icon);
    }
    lv_fs_path_ex_t mode_icon_img_path;
    lv_fs_make_path_from_buffer(&mode_icon_img_path, LV_FS_MEMFS_LETTER, pump_mode_icon_info.data,
        pump_mode_icon_info.size, "baj");
    lv_image_set_src(pump_mode_icon, (const void *)&mode_icon_img_path);
    lv_obj_align(pump_mode_icon, LV_ALIGN_TOP_LEFT, 
        home_mode_infos[mode_index].icon_x, home_mode_infos[mode_index].icon_y);    /// 设置icon位置
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    ui_display_pump_left_level_prepare(thiz);
    // ui_display_pump_mode_prepare(thiz);
#else
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_home_init();  
#endif
}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    ui_display_pump_left_level_remove(thiz);
    // ui_display_pump_mode_remove(thiz);
#else
    LVGL_OBJ_DELETE(pump_root);
    pump_home_img_deinit(thiz, &pump_home_bg_info);
    pump_home_img_deinit(thiz, &ble_info);
    pump_home_img_deinit(thiz, &battery_info);
#endif
}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    ui_display_pump_left_level(thiz);
    // ui_display_pump_mode(thiz);
#else
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(pump_root);
    static uint8_t minutes = 0, seconds = 0;
    static uint8_t level_left = 0, level_right = 0;
    static uint8_t speed = 0;
    static uint8_t mode_index = 0;
    static uint8_t is_connected = 0;
    static uint8_t battery_level = 0;
    static uint8_t is_charging = 0;
    static uint8_t is_left_level_selected = 0;
    // pump_home_bg_update(thiz);
    pump_home_ble_update(thiz, is_connected);
    pump_home_battery_update(thiz, battery_level, is_charging);
    pump_home_line_update(thiz);
    pump_home_time_update(thiz, minutes, seconds);
    pump_home_left_level_label_update(thiz, level_left, is_left_level_selected == 0 ? false : true);
    pump_home_left_level_arc_update(thiz, level_left);
    pump_home_right_level_label_update(thiz, level_right, is_left_level_selected == 0 ? true : false);
    pump_home_right_level_arc_update(thiz, level_right);
    pump_home_speed_label_update(thiz, speed);
    pump_home_mode_label_update(thiz, mode_index);     
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> pump home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
            }
        }
        level_left++;
        if (level_left > 15) {
            level_left = 0;
            is_left_level_selected = (is_left_level_selected == 0) ? 1 : 0;
        }
        level_right++;
        if (level_right > 15) {
            level_right = 0;
        }
        speed++;
        if (speed >= 3) {
            speed = 0;
        }
        mode_index++;
        if (mode_index >= 7) {
            mode_index = 0;
        }
        is_connected = (is_connected == 0) ? 1 : 0;
        battery_level++;
        if (battery_level >= 6) {
            battery_level = 0;
            is_charging = (is_charging == 0) ? 1 : 0;
        }
    }
#endif
    return 0;
}
