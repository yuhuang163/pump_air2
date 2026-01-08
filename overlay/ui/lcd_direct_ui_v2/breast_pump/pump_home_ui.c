/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_pump.h" // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_COMP_BG                    (1)
#define LEVEL_TEXT_COUNT                           (3)
#define HOME_MODE_MAX_COUNT                        (7)
#define HOME_SPEED_MAX_COUNT                       (3)
#define HOME_ANIMATION_MAX_COUNT                   (RESOURCE_ID_PUMP_MODE_ANIMATION_END - RESOURCE_ID_PUMP_MODE_ANIMATION_START + 1)
#define PUMP_ARC_COLOR                             (0xB3636D)
#define PUMP_TIME_TEXT_COLOR                       (0xB25962)

#define PUMP_LEVEL_RING_SIZE                       (306)
#define PUMP_LEVEL_RING_R                          (PUMP_LEVEL_RING_SIZE / 2)
#define PUMP_LEFT_ARC_START_ANGLE                  (135 + 1)
#define PUMP_LEFT_ARC_END_ANGLE                    (224)
#define PUMP_RIGHT_ARC_START_ANGLE                 (315 + 1)
#define PUMP_RIGHT_ARC_END_ANGLE                   (404)
               
#define STIMULATE_MODE_ICON_X                      (133 + 3)
#define STIMULATE_MODE_ICON_Y                      (63)
#define STIMULATE_MODE_LABEL_TEXT_X                (115 + 5)
#define STIMULATE_MODE_LABEL_TEXT_Y                (91 + 3)

#define LACTATION_MODE_ICON_X                      (STIMULATE_MODE_ICON_X)
#define LACTATION_MODE_ICON_Y                      (STIMULATE_MODE_ICON_Y)
#define LACTATION_MODE_LABEL_TEXT_X                (STIMULATE_MODE_LABEL_TEXT_X)
#define LACTATION_MODE_LABEL_TEXT_Y                (STIMULATE_MODE_LABEL_TEXT_Y)

#define MIX_MODE_ICON_X                            (STIMULATE_MODE_ICON_X)
#define MIX_MODE_ICON_Y                            (STIMULATE_MODE_ICON_Y)
#define MIX_MODE_LABEL_TEXT_X                      (STIMULATE_MODE_LABEL_TEXT_X + 15)
#define MIX_MODE_LABEL_TEXT_Y                      (STIMULATE_MODE_LABEL_TEXT_Y)

#define PER_MODE_ICON_X                            (STIMULATE_MODE_ICON_X)
#define PER_MODE_ICON_Y                            (STIMULATE_MODE_ICON_Y)
#define PER_MODE_LABEL_TEXT_X                      (STIMULATE_MODE_LABEL_TEXT_X - 22)
#define PER_MODE_LABEL_TEXT_Y                      (STIMULATE_MODE_LABEL_TEXT_Y)

#define PP_MODE_ICON_X                             (STIMULATE_MODE_ICON_X)
#define PP_MODE_ICON_Y                             (STIMULATE_MODE_ICON_Y)
#define PP_MODE_LABEL_TEXT_X                       (STIMULATE_MODE_LABEL_TEXT_X + 18)
#define PP_MODE_LABEL_TEXT_Y                       (STIMULATE_MODE_LABEL_TEXT_Y)

#define MILK_MODE_ICON_X                           (STIMULATE_MODE_ICON_X)
#define MILK_MODE_ICON_Y                           (STIMULATE_MODE_ICON_Y)
#define MILK_MODE_LABEL_TEXT_X                     (STIMULATE_MODE_LABEL_TEXT_X - 15)
#define MILK_MODE_LABEL_TEXT_Y                     (STIMULATE_MODE_LABEL_TEXT_Y)

#define INITIAL_MODE_ICON_X                        (STIMULATE_MODE_ICON_X)
#define INITIAL_MODE_ICON_Y                        (STIMULATE_MODE_ICON_Y)
#define INITIAL_MODE_LABEL_TEXT_X                  (STIMULATE_MODE_LABEL_TEXT_X - 18)
#define INITIAL_MODE_LABEL_TEXT_Y                  (STIMULATE_MODE_LABEL_TEXT_Y)

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
    uint16_t angle;
} pump_text_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
    uint16_t label_text_x;
    uint16_t label_text_y;
} home_mode_label_info_t;

typedef struct {    
    lv_obj_t * root;
    lv_obj_t * backgroud;
    res_img_info_t background_info;
    lv_obj_t * mode_icon;
    res_img_info_t mode_icon_info;
    lv_obj_t * mode_text;
    lv_obj_t * speed_icon;
    res_img_info_t speed_icon_info;
    lv_obj_t * speed_text;
    lv_obj_t * time_colon;
    res_img_info_t time_colon_info;
    lv_obj_t * time_minute;
    lv_obj_t * time_second;
    lv_obj_t * left_level_arc;
    lv_obj_t * left_level_circle; /// 圆圈
    lv_obj_t * left_dir_text; /// 方向文字
    lv_obj_t * left_end_text; /// 终点文字
    lv_obj_t * left_progress_text; /// 进度文字
    lv_obj_t * right_level_arc;
    lv_obj_t * right_level_circle; /// 圆圈
    lv_obj_t * right_dir_text; /// 方向文字
    lv_obj_t * right_end_text; /// 终点文字
    lv_obj_t * right_progress_text; /// 进度文字
    lv_obj_t * animation;
    res_img_info_t animation_info;
    uint8_t mode;
    uint8_t speed;
    uint8_t left_level;
    uint8_t right_level;
    uint16_t hold_time;
    uint16_t animation_index;
} pump_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_HOME_COMP_BG
static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    // #include "comparison_png2txt/bg_360x360.txt"
    #include "comparison_png2txt/home_comp_v2_360x360.txt"
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

const static pump_text_info_t speed_text_infos[HOME_SPEED_MAX_COUNT] = {
    { "Comfort", 194 + 3, 91 + 3 },
    { "Regular", 194 + 3, 91 + 3 },
    { "Boost",   194 + 8, 91 + 3 },
};

static pump_display_info_t *pump_info;

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
    if (pump_info->root == NULL || !lv_obj_is_valid(pump_info->root)) {
        pump_info->root = lv_obj_create(lv_screen_active());
        lv_obj_set_size(pump_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
        lv_obj_set_style_bg_color(pump_info->root, lv_color_white(), 0);     /// 设置背景颜色为白色
        lv_obj_set_style_bg_opa(pump_info->root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
        lv_obj_set_style_border_width(pump_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
        lv_obj_set_style_pad_all(pump_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
        lv_obj_align(pump_info->root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(pump_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
        lv_obj_clear_flag(pump_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
    }
}

static void pump_home_bg_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (pump_info->backgroud == NULL || !lv_obj_is_valid(pump_info->backgroud)) {
        pump_info->backgroud = lv_image_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->backgroud);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(pump_info->backgroud, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(pump_info->backgroud, LV_OBJ_FLAG_HIDDEN);
#if IS_SUPPORT_HOME_COMP_BG
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, home_bg_data,
            sizeof(home_bg_data), "png");
#else
        common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_HOME_BG, &pump_info->background_info);
        EXCEPTION_CHECK_VOID(pump_info->background_info.data);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, 
            pump_info->background_info.data, pump_info->background_info.size, "baj");
#endif
        lv_image_set_src(pump_info->backgroud, (const void *)&bg_img_path);
        lv_obj_align(pump_info->backgroud, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}

static void pump_home_time_update(ui_manager_context_t *thiz, uint16_t hold_time)
{
    if (pump_info->hold_time == hold_time) {
        // return;
    }

    uint8_t minutes = hold_time / 60, seconds = hold_time % 60;
    if (pump_info->time_minute == NULL || !lv_obj_is_valid(pump_info->time_minute)) {
        pump_info->time_minute = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_minute);
    }
    char minute_value[4] = {0};
    sprintf(minute_value, "%02d", minutes >= 60 ? 59 : minutes);
    lv_label_set_text(pump_info->time_minute, minute_value);
    lv_obj_set_style_text_color(pump_info->time_minute, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->time_minute, &lv_font_quicksand_96_bold, 0);
    lv_obj_align(pump_info->time_minute, LV_ALIGN_TOP_LEFT, 69 + 4, 108 + 16);

    if (pump_info->time_second == NULL || !lv_obj_is_valid(pump_info->time_second)) {
        pump_info->time_second = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_second);
    }
    char second_value[4] = {0};
    sprintf(second_value, "%02d", seconds >= 60 ? 59 : seconds);
    lv_label_set_text(pump_info->time_second, second_value);
    lv_obj_set_style_text_color(pump_info->time_second, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->time_second, &lv_font_quicksand_96_bold, 0);
    lv_obj_align(pump_info->time_second, LV_ALIGN_TOP_LEFT, 194 + 4, 108 + 16);

    if (pump_info->time_colon == NULL || !lv_obj_is_valid(pump_info->time_colon)) {
        pump_info->time_colon = lv_image_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_colon);
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_HOME_TIME_COLON, &pump_info->time_colon_info);
    EXCEPTION_CHECK_VOID(pump_info->time_colon_info.data);
    lv_fs_path_ex_t colon_img_path;
    lv_fs_make_path_from_buffer(&colon_img_path, LV_FS_MEMFS_LETTER, 
        pump_info->time_colon_info.data, pump_info->time_colon_info.size, "baj");
    lv_image_set_src(pump_info->time_colon, (const void *)&colon_img_path);
    lv_obj_align(pump_info->time_colon, LV_ALIGN_TOP_LEFT, 174 + 4, 135 + 2);
    pump_info->hold_time = hold_time;
}

static void level_text_place(lv_obj_t * lab, uint16_t angle, uint16_t ring_r, uint8_t is_reverse)
{
    /// 父对象 360x360，圆心(180,180)
    lv_point_t c = { .x = PANLE_DISPLAY_W / 2, .y = PANLE_DISPLAY_H / 2 };
    /// 计算圆周点
    lv_coord_t px = c.x + (lv_trigo_cos(angle) * ring_r) / LV_TRIGO_SIN_MAX;
    lv_coord_t py = c.y + (lv_trigo_sin(angle) * ring_r) / LV_TRIGO_SIN_MAX;
    /// 先把label中心放到该点（固定位置）
    lv_obj_align(lab, LV_ALIGN_CENTER, px - c.x, py - c.y);
    /// 旋转中心设为label自身中心（必须在 set_text 后取宽高）
    lv_coord_t w = lv_obj_get_width(lab);
    lv_coord_t h = lv_obj_get_height(lab);
    lv_obj_set_style_transform_pivot_x(lab, w / 2, 0);
    lv_obj_set_style_transform_pivot_y(lab, h / 2, 0);
    /// 切线方向：angle + 90°（单位0.1°）
    int tangent = angle + 90 + (is_reverse ? 180 : 0);
    lv_obj_set_style_transform_angle(lab, tangent * 10, 0);
}

static void pump_home_left_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{
    if (pump_info->left_dir_text == NULL || !lv_obj_is_valid(pump_info->left_dir_text)) {
        pump_info->left_dir_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->left_dir_text);
        level_text_place(pump_info->left_dir_text,
            PUMP_LEFT_ARC_START_ANGLE - 6, PUMP_LEVEL_RING_R - 8, true);
    }
    lv_label_set_text(pump_info->left_dir_text, "L");
    lv_obj_set_style_text_color(pump_info->left_dir_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->left_dir_text, &lv_font_oppo_sans_14_medium, 0);

    if (pump_info->left_end_text == NULL || !lv_obj_is_valid(pump_info->left_end_text)) {
        pump_info->left_end_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->left_end_text);
        level_text_place(pump_info->left_end_text, 
            PUMP_LEFT_ARC_END_ANGLE + 4, PUMP_LEVEL_RING_R - 8, false);
    }
    lv_label_set_text(pump_info->left_end_text, "15");
    lv_obj_set_style_text_color(pump_info->left_end_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->left_end_text, &lv_font_oppo_sans_14_medium, 0);

    if (pump_info->left_progress_text == NULL || !lv_obj_is_valid(pump_info->left_progress_text)) {
        pump_info->left_progress_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->left_progress_text);
        level_text_place(pump_info->left_progress_text, end_angle, PUMP_LEVEL_RING_R + 10, false);
    }
    char level_text[4] = {0};
    sprintf(level_text, "%d", level);
    lv_label_set_text(pump_info->left_progress_text, level_text);
    lv_obj_set_style_text_color(pump_info->left_progress_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->left_progress_text, &lv_font_oppo_sans_14_medium, 0);
}

static void pump_home_left_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{
    if (pump_info->left_level_arc == NULL || !lv_obj_is_valid(pump_info->left_level_arc)) {
        pump_info->left_level_arc = lv_arc_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->left_level_arc);
    }
    lv_obj_set_size(pump_info->left_level_arc, PUMP_LEVEL_RING_SIZE, PUMP_LEVEL_RING_SIZE);
    lv_obj_center(pump_info->left_level_arc);
    
    lv_arc_set_bg_angles(pump_info->left_level_arc, PUMP_LEFT_ARC_START_ANGLE, PUMP_LEFT_ARC_END_ANGLE);
    lv_arc_set_angles(pump_info->left_level_arc, PUMP_LEFT_ARC_START_ANGLE, end_angle);
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_info->left_level_arc,
        lv_color_hex(PUMP_ARC_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_info->left_level_arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(pump_info->left_level_arc, 97, LV_PART_MAIN);  /// 背景弧不透明度38%（97/255≈38%）

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_info->left_level_arc,
        lv_color_hex(PUMP_ARC_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_info->left_level_arc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(pump_info->left_level_arc, LV_OPA_COVER, LV_PART_INDICATOR);  /// 前景弧完全不透明
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_info->left_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_left_level_update(ui_manager_context_t *thiz, uint8_t level)
{
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = PUMP_LEFT_ARC_END_ANGLE;
    } else if (level == 0) {
        end_angle = PUMP_LEFT_ARC_START_ANGLE;
    } else {
        end_angle = ((PUMP_LEFT_ARC_END_ANGLE - PUMP_LEFT_ARC_START_ANGLE) * level / 15) + PUMP_LEFT_ARC_START_ANGLE;
    }
    pump_home_left_level_arc_update(thiz, end_angle);
    pump_home_left_level_text_update(thiz, level, end_angle);
}

static void pump_home_right_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{
    if (pump_info->right_dir_text == NULL || !lv_obj_is_valid(pump_info->right_dir_text)) {
        pump_info->right_dir_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->right_dir_text);
        level_text_place(pump_info->right_dir_text, 
            PUMP_RIGHT_ARC_END_ANGLE + 7, PUMP_LEVEL_RING_R - 3, true);
    }
    lv_label_set_text(pump_info->right_dir_text, "R");
    lv_obj_set_style_text_color(pump_info->right_dir_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->right_dir_text, &lv_font_oppo_sans_14_medium, 0);

    if (pump_info->right_end_text == NULL || !lv_obj_is_valid(pump_info->right_end_text)) {
        pump_info->right_end_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->right_end_text);
        level_text_place(pump_info->right_end_text,
            PUMP_RIGHT_ARC_START_ANGLE - 5, PUMP_LEVEL_RING_R + 3, false);
    }
    lv_label_set_text(pump_info->right_end_text, "15");
    lv_obj_set_style_text_color(pump_info->right_end_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->right_end_text, &lv_font_oppo_sans_14_medium, 0);

    if (pump_info->right_progress_text == NULL || !lv_obj_is_valid(pump_info->right_progress_text)) {
        pump_info->right_progress_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->right_progress_text);
        level_text_place(pump_info->right_progress_text, end_angle, PUMP_LEVEL_RING_R + 10, true);
    }
    char level_text[4] = {0};
    sprintf(level_text, "%d", level);
    lv_label_set_text(pump_info->right_progress_text, level_text);
    lv_obj_set_style_text_color(pump_info->right_progress_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->right_progress_text, &lv_font_oppo_sans_14_medium, 0);
}

static void pump_home_right_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{
    if (pump_info->right_level_arc == NULL || !lv_obj_is_valid(pump_info->right_level_arc)) {
        pump_info->right_level_arc = lv_arc_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->right_level_arc);
    }
    lv_obj_set_size(pump_info->right_level_arc, PUMP_LEVEL_RING_SIZE, PUMP_LEVEL_RING_SIZE);
    lv_obj_center(pump_info->right_level_arc);
    
    lv_arc_set_bg_angles(pump_info->right_level_arc, PUMP_RIGHT_ARC_START_ANGLE, PUMP_RIGHT_ARC_END_ANGLE);
    lv_arc_set_angles(pump_info->right_level_arc, end_angle, PUMP_RIGHT_ARC_END_ANGLE);
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_info->right_level_arc,
         lv_color_hex(PUMP_ARC_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_info->right_level_arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(pump_info->right_level_arc, 97, LV_PART_MAIN);  /// 背景弧不透明度38%（97/255≈38%）

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_info->right_level_arc,
         lv_color_hex(PUMP_ARC_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_info->right_level_arc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(pump_info->right_level_arc, LV_OPA_COVER, LV_PART_INDICATOR);  /// 前景弧完全不透明
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_info->right_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_right_level_update(ui_manager_context_t *thiz, uint8_t level)
{
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = PUMP_RIGHT_ARC_START_ANGLE;
    } else if (level == 0) {
        end_angle = PUMP_RIGHT_ARC_END_ANGLE;
    } else {
        end_angle = ((PUMP_RIGHT_ARC_END_ANGLE - PUMP_RIGHT_ARC_START_ANGLE) * (15 - level) / 15) + PUMP_RIGHT_ARC_START_ANGLE;
    }
    pump_home_right_level_arc_update(thiz, end_angle);
    pump_home_right_level_text_update(thiz, level, end_angle);
}

static void pump_home_speed_label_update(ui_manager_context_t *thiz, uint8_t speed)
{
    // speed = 1; /// TODO: 测试用
    if (pump_info->speed_icon == NULL || !lv_obj_is_valid(pump_info->speed_icon)) {
        pump_info->speed_icon = lv_image_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->speed_icon);
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_HOME_SPEED_ICON, &pump_info->speed_icon_info);
    EXCEPTION_CHECK_VOID(pump_info->speed_icon_info.data);
    lv_fs_path_ex_t speed_icon_img_path;
    lv_fs_make_path_from_buffer(&speed_icon_img_path, LV_FS_MEMFS_LETTER, 
        pump_info->speed_icon_info.data, pump_info->speed_icon_info.size, "baj");
    lv_image_set_src(pump_info->speed_icon, (const void *)&speed_icon_img_path);
    lv_obj_align(pump_info->speed_icon, LV_ALIGN_TOP_LEFT, 204 + 3, 63 + 1);

    if (speed >= HOME_SPEED_MAX_COUNT) {
        return;
    }

    if (pump_info->speed_text == NULL || !lv_obj_is_valid(pump_info->speed_text)) {
        pump_info->speed_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->speed_text);
    }  

    lv_label_set_text(pump_info->speed_text, speed_text_infos[speed].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_info->speed_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->speed_text, &lv_font_oppo_sans_14_regular, 0);
    lv_obj_align(pump_info->speed_text, LV_ALIGN_TOP_LEFT, 
        speed_text_infos[speed].text_x, speed_text_infos[speed].text_y);
}

static void pump_home_mode_label_update(ui_manager_context_t *thiz, uint8_t mode_index)
{
    // mode_index = 3; /// TODO: 测试用
    if (mode_index >= HOME_MODE_MAX_COUNT) {
        return;
    }

    if (pump_info->mode_icon == NULL || !lv_obj_is_valid(pump_info->mode_icon)) {
        pump_info->mode_icon = lv_image_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->mode_icon);
    }
    common_ui_img_resource_init(thiz, home_mode_infos[mode_index].img_id, &pump_info->mode_icon_info);
    EXCEPTION_CHECK_VOID(pump_info->mode_icon_info.data);
    lv_fs_path_ex_t mode_icon_img_path;
    lv_fs_make_path_from_buffer(&mode_icon_img_path, LV_FS_MEMFS_LETTER, 
        pump_info->mode_icon_info.data, pump_info->mode_icon_info.size, "baj");
    lv_image_set_src(pump_info->mode_icon, (const void *)&mode_icon_img_path);
    lv_obj_align(pump_info->mode_icon, LV_ALIGN_TOP_LEFT, 
        home_mode_infos[mode_index].icon_x, home_mode_infos[mode_index].icon_y); /// 设置icon位置

    if (pump_info->mode_text == NULL || !lv_obj_is_valid(pump_info->mode_text)) {
        pump_info->mode_text = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->mode_text);
    }  
    lv_label_set_text(pump_info->mode_text, home_mode_infos[mode_index].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(pump_info->mode_text, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->mode_text, &lv_font_oppo_sans_14_regular, 0);
    lv_obj_align(pump_info->mode_text, LV_ALIGN_TOP_LEFT, 
        home_mode_infos[mode_index].label_text_x, home_mode_infos[mode_index].label_text_y); /// 设置label文字位置
}

static void pump_home_animation_update(ui_manager_context_t *thiz)
{
    if (pump_info->animation == NULL || !lv_obj_is_valid(pump_info->animation)) {
        pump_info->animation = lv_image_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->animation);
    }  

    if (pump_info->animation_index >= HOME_ANIMATION_MAX_COUNT) {
        pump_info->animation_index = 0;
    }

    common_ui_img_resource_init(thiz, 
        RESOURCE_ID_PUMP_MODE_ANIMATION_START + pump_info->animation_index, &pump_info->animation_info);
    EXCEPTION_CHECK_VOID(pump_info->animation_info.data);
    lv_fs_path_ex_t animation_img_path;
    lv_fs_make_path_from_buffer(&animation_img_path, LV_FS_MEMFS_LETTER, 
        pump_info->animation_info.data, pump_info->animation_info.size, "baj");
    lv_image_set_src(pump_info->animation, (const void *)&animation_img_path);
    lv_obj_align(pump_info->animation, LV_ALIGN_TOP_LEFT, 77 + 10, 162 - 4); 
    pump_info->animation_index++;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_home_screen_prepare(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    // ui_display_pump_left_level_prepare(thiz);
    // ui_display_pump_mode_prepare(thiz);
    // ui_display_breastfeeding_mode_prepare(thiz);
    // ui_display_pump_complete_tip_prepare(thiz);
    // ui_display_airplane_mode_prepare(thiz);
    // ui_display_home_prepare(thiz);
    // ui_display_product_tests_home_prepare(thiz);
    ui_display_pump_speed_prepare(thiz);
#else
    EXCEPTION_CHECK_VOID(thiz);
    if (pump_info == NULL) {
        pump_info = (pump_display_info_t *)u_malloz(sizeof(pump_display_info_t));
        EXCEPTION_CHECK_VOID(pump_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_home_init();  
    pump_home_bg_update(thiz);
    pump_info->animation_index = 0;
#endif
}

void ui_display_pump_home_screen_remove(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    // ui_display_pump_left_level_remove(thiz);
    // ui_display_pump_mode_remove(thiz);
    // ui_display_breastfeeding_mode_remove(thiz);
    // ui_display_pump_complete_tip_remove(thiz);
    // ui_display_airplane_mode_remove(thiz);
    // ui_display_home_remove(thiz);
    // ui_display_product_tests_home_remove(thiz);
    ui_display_pump_speed_remove(thiz);
#else
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(pump_info);
    battery_icon_deinit(thiz);
    common_ui_img_resource_deinit(thiz, &pump_info->background_info);
    common_ui_img_resource_deinit(thiz, &pump_info->mode_icon_info);
    common_ui_img_resource_deinit(thiz, &pump_info->speed_icon_info);
    common_ui_img_resource_deinit(thiz, &pump_info->animation_info);
    common_ui_img_resource_deinit(thiz, &pump_info->time_colon_info);
    LVGL_OBJ_DELETE(pump_info->root);
    u_free(pump_info);
    pump_info = NULL;
#endif
}

int ui_display_pump_home_screen(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_HOME_DEBUG_OTHER_PAGE
    // ui_display_pump_left_level(thiz);
    // ui_display_pump_mode(thiz);
    // ui_display_breastfeeding_mode(thiz);
    // ui_display_pump_complete_tip(thiz);
    // ui_display_airplane_mode_process(thiz);
    // ui_display_home_process(thiz);
    // ui_display_product_tests_home_process(thiz);
    ui_display_pump_speed(thiz);
#else

    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(pump_info);
    EXCEPTION_CHECK_INT(pump_info->root);
    ui_proc_pump_t *proc = (ui_proc_pump_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc);
    EXCEPTION_CHECK_INT(proc->pump);
    EXCEPTION_CHECK_INT(proc->motor);
    EXCEPTION_CHECK_INT(proc->ble);
    EXCEPTION_CHECK_INT(proc->misc);
    EXCEPTION_CHECK_INT(proc->st);

    const pump_result_info_t *result = proc->pump->get_breast_pump_result(BREAST_PUMP_RESULT_PUMPING);
    battery_icon_update(thiz, pump_info->root, 
        proc->misc->get_battery_percent(), proc->misc->is_charge_plug_in());
    uint16_t hold_time = 0;
    if (result != NULL) {
        hold_time = result->work_valid_time / 1000;
    }
    hold_time = 610;
    pump_setting_t setting;
    uint8_t time_index = proc->motor->get_runtime_policy_index(MOTOR_TIME_POLICY);
    proc->pump->get_breast_pump_setting(&setting, time_index >= ROOT_BREAST_PUMP_STEP_MAX ? 0 : time_index);
    uint8_t mode = proc->pump->get_breast_pump_mode();
    if (thiz->priv.fps_count == 0) {
        pump_home_time_update(thiz, hold_time);
        pump_home_left_level_update(thiz, setting.level[0]);
        pump_home_right_level_update(thiz, setting.level[1]);
        pump_home_speed_label_update(thiz, setting.speed);
        pump_home_mode_label_update(thiz, mode);  
    }   
    pump_home_animation_update(thiz);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> pump home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
        
    }
#endif
    return 0;
}
