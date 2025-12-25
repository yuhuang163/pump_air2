/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "pump_ui_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static PSRAM_DATA_LOC uint8_t setting_bg_data[] = {
    #include "../../../resources/img_png_bin/left_level_comp_360x360.txt"
    // #include "../../../resources/img_png_bin/right_level_comp_360x360.txt"
    // #include "../../../resources/img_png_bin/speed_comp_360x360.txt"
};


/// 档位调整主界面对象
static lv_obj_t * level_root;
static lv_obj_t * level_bg_img;
static lv_obj_t * level_arc;
static lv_obj_t * level_arc_progress;
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

static void pump_home_bg_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    static uint8_t status = 0;
    if (level_bg_img == NULL || !lv_obj_is_valid(level_root)) {
        level_bg_img = lv_image_create(level_root);
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

static void pump_level_arc_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_arc == NULL || !lv_obj_is_valid(level_arc)) {
        level_arc = lv_scale_create(level_root);
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

static void pump_level_arc_progress_update(void)
{
    EXCEPTION_CHECK_VOID(level_root);
    if (level_arc_progress == NULL || !lv_obj_is_valid(level_arc_progress)) {
        level_arc_progress = lv_scale_create(level_root);
        lv_scale_set_mode(level_arc_progress, LV_SCALE_MODE_ROUND_INNER);
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


static void pump_level_lable_update(void)
{

}

static void pump_level_value_update(void)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_left_level_prepare(ui_manager_context_t *thiz)
{
    pump_level_root_init();
}

void ui_display_pump_left_level_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(level_root);
}

int ui_display_pump_left_level(ui_manager_context_t *thiz)
{
    pump_home_bg_update();
    pump_level_arc_update();
    return 0;
}

void ui_display_pump_right_level_prepare(ui_manager_context_t *thiz)
{
    pump_level_root_init();
}

void ui_display_pump_right_level_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(level_root);
}

int ui_display_pump_right_level(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}

void ui_display_pump_speed_prepare(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_speed_prepare");
}

void ui_display_pump_speed_remove(ui_manager_context_t *thiz)
{
    // LOGI("ui_display_pump_speed_remove");
}

int ui_display_pump_speed(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_UINT(thiz);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    return 0;
}