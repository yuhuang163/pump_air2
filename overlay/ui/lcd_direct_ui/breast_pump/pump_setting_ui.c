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
#define IS_SUPPORT_LEVEL_COMP_BG                        (0)

#define PUMP_LEVEL_ARC_SEL_COLOR                        0xB3686D
#define PUMP_LEVEL_ARC_UNSEL_COLOR                      0xF7EFEF
#define PUMP_LEVEL_LINE_COLOR                           0xD9D9D9
#define PUMP_LEVEL_TEXT_COLOR                           0x393535
#define PUMP_LEVEL_VALUE_COLOR                          0x1C1C20
#define PUMP_LEVEL_LABLE_UNSEL_COLOR                    0xF7EFEF
#define PUMP_LEVEL_LABLE_SEL_COLOR                      0xB3686D
#define PUMP_LEVEL_TEXT_SEL_COLOR                       0xB3686D
#define PUMP_LEVEL_TEXT_UNSEL_COLOR                     0xFFFFFF

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_LEVEL_COMP_BG
static PSRAM_DATA_LOC uint8_t setting_bg_data[] = {
    // #include "../../../resources/img_png_bin/left_level_comp_360x360.txt"
    #include "../../../resources/img_png_bin/right_level_comp_360x360.txt"
    // #include "../../../resources/img_png_bin/speed_comp_360x360.txt"
};
#endif

/// 档位调整主界面对象
static lv_obj_t * level_root;
#if IS_SUPPORT_LEVEL_COMP_BG
static lv_obj_t * level_bg_img;
#endif
static lv_obj_t * level_arc;
static lv_obj_t * level_arc_progress;
static lv_obj_t * left_level_label;
static lv_obj_t * left_level_label_text;
static lv_obj_t * right_level_label;
static lv_obj_t * right_level_label_text;
static lv_obj_t * level_label;
static lv_obj_t * level_label_text;
static lv_obj_t * level_root_value;
static lv_obj_t * level_root_text;

/// 速度调整主界面对象
static lv_obj_t * speed_root;
static lv_obj_t * speed_bg_img;
static lv_obj_t * speed_high_label;
static lv_obj_t * speed_high_label_text;
static lv_obj_t * speed_medium_label;
static lv_obj_t * speed_medium_label_text;
static lv_obj_t * speed_low_label;
static lv_obj_t * speed_low_label_text;

static uint32_t start_time = 0;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_level_root_init(void)
{
    if (level_root == NULL || !lv_obj_is_valid(level_root)) {
        level_root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(level_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(level_root, lv_color_white(), 0);     /// 设置背景颜色为白色
    lv_obj_set_style_bg_opa(level_root, LV_OPA_COVER, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(level_root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(level_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(level_root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(level_root, LV_OBJ_FLAG_CLICKABLE);           /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(level_root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

#if IS_SUPPORT_LEVEL_COMP_BG
static void pump_home_bg_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    static uint8_t status = 0;
    if (level_bg_img == NULL || !lv_obj_is_valid(level_root)) {
        level_bg_img = lv_image_create(level_root);
        EXCEPTION_CHECK_VOID(level_bg_img);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(level_bg_img, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(level_bg_img, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, setting_bg_data,
            sizeof(setting_bg_data), "png");
        lv_image_set_src(level_bg_img, (const void *)&bg_img_path);
        lv_obj_align(level_bg_img, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#endif

static void pump_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_arc == NULL || !lv_obj_is_valid(level_arc)) {
        level_arc = lv_scale_create(level_root);
        EXCEPTION_CHECK_VOID(level_arc);
        lv_scale_set_mode(level_arc, LV_SCALE_MODE_ROUND_INNER);
    }

    lv_obj_set_size(level_arc, 264, 264);                              /// 设置刻度盘控件的宽高（越大，同样 tick 数看起来越“稀疏”）
    lv_obj_center(level_arc);                                          /// 让刻度盘在父容器内居中
    lv_obj_set_style_bg_opa(level_arc, LV_OPA_TRANSP, 0);              /// 背景填充透明（不画背景色，避免遮挡底图）
    lv_obj_set_style_border_width(level_arc, 0, 0);                    /// 边框宽度 0（不画边框）
    lv_obj_set_style_pad_all(level_arc, 0, 0);                         /// 内边距 0（tick 计算用内容区域，padding 会影响半径/位置）
    lv_obj_set_style_arc_width(level_arc, 0, LV_PART_MAIN);            /// 主弧线宽度 0（隐藏 scale 的主弧/主线）
    lv_obj_set_style_arc_opa(level_arc, LV_OPA_TRANSP, LV_PART_MAIN);  /// 主弧线透明（双保险：即使有宽度也不显示）
    lv_scale_set_rotation(level_arc, 154);                             /// 设置刻度盘起始角度（整体旋转；决定第 0 个 tick 从哪个角度开始画）
    lv_scale_set_angle_range(level_arc, 232);                          /// 设置刻度覆盖角度范围（扇区角度；越大 tick 越分散）
    lv_scale_set_range(level_arc, 0, 24);                              /// 设置刻度映射的数值范围（tick_idx 会线性映射到 [0,24]）

    lv_scale_set_total_tick_count(level_arc, 12);                      /// 总 tick 数（包含两端）；圆形模式下 tick 角度间隔 = angle_range/(total_tick_count-1)
                                                                       /// 觉得“刻度间隔太小/太密”：把 total_tick_count 调小，或把 angle_range 调大，或把控件尺寸调大
    lv_scale_set_major_tick_every(level_arc, 0);                       /// 设定“主刻度”频率：0=不使用主刻度；N>0 表示每 N 个 tick 一个主刻度（主刻度样式用 LV_PART_INDICATOR）
    lv_scale_set_label_show(level_arc, false);                         /// 是否显示主刻度的文字标签（false=只画刻度线，不画数字）
    lv_obj_set_style_line_width(level_arc, 2, LV_PART_ITEMS);          /// 次刻度线宽（像素；作用于 LV_PART_ITEMS）
    lv_obj_set_style_length(level_arc, 5, LV_PART_ITEMS);              /// 次刻度线长度（半径方向长度；作用于 LV_PART_ITEMS）
    lv_obj_set_style_line_color(level_arc, lv_color_hex(PUMP_LEVEL_LINE_COLOR), LV_PART_ITEMS); /// 次刻度线颜色
}

static void pump_level_arc_progress_update(uint8_t level)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_arc_progress == NULL || !lv_obj_is_valid(level_arc_progress)) {
        level_arc_progress = lv_arc_create(level_root);
        EXCEPTION_CHECK_VOID(level_arc_progress);
    }
    lv_obj_set_size(level_arc_progress, 308, 308);
    lv_obj_center(level_arc_progress);
    
    lv_arc_set_bg_angles(level_arc_progress, 152, 388);
    lv_arc_set_angles(level_arc_progress, 152, 388);
    
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(level_arc_progress, lv_color_hex(PUMP_LEVEL_ARC_UNSEL_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(level_arc_progress, 17, LV_PART_MAIN);

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(level_arc_progress, lv_color_hex(PUMP_LEVEL_ARC_SEL_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(level_arc_progress, 17, LV_PART_INDICATOR);
    
    /* 去掉旋钮 */
    lv_obj_remove_style(level_arc_progress, NULL, LV_PART_KNOB);
}

static void pump_level_root_text_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_root_text == NULL || !lv_obj_is_valid(level_root_text)) {
        level_root_text = lv_label_create(level_root);
        EXCEPTION_CHECK_VOID(level_root_text);
    }
    lv_label_set_text(level_root_text, "Level");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(level_root_text, lv_color_hex(PUMP_LEVEL_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(level_root_text, &lv_font_roboto_24_regular, 0);  /// 设置字体
    lv_obj_align(level_root_text, LV_ALIGN_TOP_LEFT, 155, 113); /// 设置文字位置
}

static void pump_level_root_value_update(uint8_t level)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_root_value == NULL || !lv_obj_is_valid(level_root_value)) {
        level_root_value = lv_label_create(level_root);
        EXCEPTION_CHECK_VOID(level_root_value);
    }
    lv_label_set_text(level_root_value, "12");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(level_root_value, lv_color_hex(PUMP_LEVEL_VALUE_COLOR), 0);
    lv_obj_set_style_text_font(level_root_value, &lv_font_roboto_79_semibold, 0);  /// 设置字体
    lv_obj_align(level_root_value, LV_ALIGN_TOP_LEFT, 138, 150); /// 设置文字位置
}

static void pump_left_level_lable_update(bool is_selected)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (left_level_label == NULL || !lv_obj_is_valid(left_level_label)) {
        left_level_label = lv_obj_create(level_root);
        EXCEPTION_CHECK_VOID(left_level_label);
    }

    if (left_level_label_text == NULL || !lv_obj_is_valid(left_level_label_text)) {
        left_level_label_text = lv_label_create(left_level_label);
        EXCEPTION_CHECK_VOID(left_level_label_text);
    }
    lv_obj_clear_flag(left_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(left_level_label, 98, 55); /// 设置容器尺寸
    lv_obj_set_style_border_width(left_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(left_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(left_level_label, 
        lv_color_hex(is_selected ? PUMP_LEVEL_LABLE_SEL_COLOR : PUMP_LEVEL_LABLE_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(left_level_label, 28, 0);   /// 高度 55 → 圆角设 28（或更大）
    lv_label_set_text(left_level_label_text, "Left");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(left_level_label_text, 
        lv_color_hex(is_selected ? PUMP_LEVEL_TEXT_UNSEL_COLOR : PUMP_LEVEL_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_opa(left_level_label_text, 
        is_selected ? LV_OPA_20 : LV_OPA_COVER, LV_PART_MAIN);  /// 文字不透明度=20%（LV_OPA_20=51/255≈20%），视觉效果约为 80% 透明
    lv_obj_set_style_text_font(left_level_label_text, &lv_font_roboto_27_regular, 0);  /// 设置字体
    lv_obj_center(left_level_label_text); /// 文字居中
    lv_obj_align(left_level_label, LV_ALIGN_TOP_LEFT, 75, 254);    /// 设置右档位标签位置
}

static void pump_right_level_lable_update(bool is_selected)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (right_level_label == NULL || !lv_obj_is_valid(right_level_label)) {
        right_level_label = lv_obj_create(level_root);
        EXCEPTION_CHECK_VOID(right_level_label);
    }
    
    if (right_level_label_text == NULL || !lv_obj_is_valid(right_level_label_text)) {
        right_level_label_text = lv_label_create(right_level_label);
        EXCEPTION_CHECK_VOID(right_level_label_text);
    }
    lv_obj_clear_flag(right_level_label, LV_OBJ_FLAG_SCROLLABLE);   /// 不要滚动条
    lv_obj_set_size(right_level_label, 98, 55); /// 设置容器尺寸
    lv_obj_set_style_border_width(right_level_label, 0, 0);         /// 无边框
    lv_obj_set_style_bg_opa(right_level_label, LV_OPA_COVER, 0);    /// 背景不透明
    lv_obj_set_style_bg_color(right_level_label, 
        lv_color_hex(is_selected ? PUMP_LEVEL_LABLE_SEL_COLOR : PUMP_LEVEL_LABLE_UNSEL_COLOR), 0);   /// 设置背景颜色
    lv_obj_set_style_radius(right_level_label, 28, 0);   /// 高度 55 → 圆角设 28（或更大）
    lv_label_set_text(right_level_label_text, "Right");  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(right_level_label_text, 
        lv_color_hex(is_selected ? PUMP_LEVEL_TEXT_UNSEL_COLOR : PUMP_LEVEL_TEXT_SEL_COLOR), 0);
    lv_obj_set_style_text_opa(left_level_label_text, 
        is_selected ? LV_OPA_20 : LV_OPA_COVER, LV_PART_MAIN);  /// 文字不透明度=20%（LV_OPA_20=51/255≈20%），视觉效果约为 80% 透明
    lv_obj_set_style_text_font(right_level_label_text, &lv_font_roboto_27_regular, 0);  /// 设置字体
    lv_obj_center(right_level_label_text); /// 文字居中
    lv_obj_align(right_level_label, LV_ALIGN_TOP_LEFT, 195, 254);    /// 设置右档位标签位置
}
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_left_level_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    pump_level_root_init();
}

void ui_display_pump_left_level_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(level_root);
}

int ui_display_pump_left_level(ui_manager_context_t *thiz)
{
#if IS_SUPPORT_LEVEL_COMP_BG
    pump_home_bg_update();
#endif
    pump_level_arc_update();
    pump_level_arc_progress_update(6);
    pump_level_root_text_update();
    pump_level_root_value_update(6);
    pump_left_level_lable_update(false);
    pump_right_level_lable_update(true);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> pump left level fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}

void ui_display_pump_right_level_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    pump_level_root_init();
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
}

void ui_display_pump_right_level_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(level_root);
}

int ui_display_pump_right_level(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
#if IS_SUPPORT_LEVEL_COMP_BG
    pump_home_bg_update();
#endif
    pump_level_arc_update();
    pump_level_arc_progress_update(6);
    pump_level_root_text_update();
    pump_level_root_value_update(6);
    pump_left_level_lable_update(false);
    pump_right_level_lable_update(true);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> pump right level fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}

void ui_display_pump_speed_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
}

void ui_display_pump_speed_remove(ui_manager_context_t *thiz)
{
    // LVGL_OBJ_DELETE(level_root);
}

int ui_display_pump_speed(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOGI("===> pump right level fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}