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

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_HOME_COMP_BG                       (1)
#define LEVEL_TEXT_COUNT                              (3)
#define PUMP_ARC_COLOR                                (0xB3636D)
#define PUMP_TIME_TEXT_COLOR                          (0xB25962)

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
    uint16_t angle;
} pump_text_info_t;

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
    lv_obj_t * battery_icon;
    res_img_info_t battery_icon_info;
    lv_obj_t * battery_text;
    lv_obj_t * charge_icon;
    lv_obj_t * time_colon;
    lv_obj_t * time_minute;
    lv_obj_t * time_second;
    lv_obj_t * left_level_arc;
    lv_obj_t * left_level_circle;
    lv_obj_t * left_text[LEVEL_TEXT_COUNT];
    lv_obj_t * right_level_arc;
    lv_obj_t * right_level_circle;
    lv_obj_t * right_text[LEVEL_TEXT_COUNT];
    lv_obj_t * animation;
} pump_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_HOME_COMP_BG
static PSRAM_DATA_LOC uint8_t home_bg_data[] = {
    #include "comparison_png2txt/bg_360x360.txt"
};
#endif

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

static void pump_home_time_update(ui_manager_context_t *thiz, uint8_t minutes, uint8_t seconds)
{
    if (pump_info->time_colon == NULL || !lv_obj_is_valid(pump_info->time_colon)) {
        pump_info->time_colon = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_colon);
    }

    if (pump_info->time_minute == NULL || !lv_obj_is_valid(pump_info->time_minute)) {
        pump_info->time_minute = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_minute);
    }

    if (pump_info->time_second == NULL || !lv_obj_is_valid(pump_info->time_second)) {
        pump_info->time_second = lv_label_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->time_second);
    }
    char minute_value[4] = {0};
    sprintf(minute_value, "%02d", minutes >= 60 ? 59 : minutes);
    lv_label_set_text(pump_info->time_minute, minute_value);
    lv_obj_set_style_text_color(pump_info->time_minute, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->time_minute, &lv_font_quicksand_96_bold, 0);
    lv_obj_align(pump_info->time_minute, LV_ALIGN_TOP_LEFT, 69, 108 + 16);
    lv_label_set_text(pump_info->time_colon, ":");
    lv_obj_set_style_text_font(pump_info->time_colon, &lv_font_roboto_68_medium, 0);
    lv_obj_align(pump_info->time_colon, LV_ALIGN_TOP_LEFT, 175, 179);
    char second_value[4] = {0};
    sprintf(second_value, "%02d", seconds >= 60 ? 59 : seconds);
    lv_label_set_text(pump_info->time_second, second_value);
    lv_obj_set_style_text_color(pump_info->time_second, lv_color_hex(PUMP_TIME_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(pump_info->time_second, &lv_font_quicksand_96_bold, 0);
    lv_obj_align(pump_info->time_second, LV_ALIGN_TOP_LEFT, 194, 108 + 12);
}

static void pump_home_left_level_label_update(ui_manager_context_t *thiz, uint8_t level, uint8_t is_selected)
{

}

static void pump_home_left_level_arc_update(ui_manager_context_t *thiz, uint8_t level)
{
    if (pump_info->left_level_arc == NULL || !lv_obj_is_valid(pump_info->left_level_arc)) {
        pump_info->left_level_arc = lv_arc_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->left_level_arc);
    }
    lv_obj_set_size(pump_info->left_level_arc, 306, 306);
    lv_obj_center(pump_info->left_level_arc);
    
    lv_arc_set_bg_angles(pump_info->left_level_arc, 137, 224);
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = 212;
    } else if (level == 0) {
        end_angle = 160;
    } else {
        end_angle = ((212 - 160) * level / 15) + 160;
    }
    lv_arc_set_angles(pump_info->left_level_arc, 160, end_angle);
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

static void pump_home_right_level_label_update(ui_manager_context_t *thiz, uint8_t level, uint8_t is_selected)
{

}

static void pump_home_right_level_arc_update(ui_manager_context_t *thiz, uint8_t level)
{
    if (pump_info->right_level_arc == NULL || !lv_obj_is_valid(pump_info->right_level_arc)) {
        pump_info->right_level_arc = lv_arc_create(pump_info->root);
        EXCEPTION_CHECK_VOID(pump_info->right_level_arc);
    }
    lv_obj_set_size(pump_info->right_level_arc, 306, 306);
    lv_obj_center(pump_info->right_level_arc);
    
    lv_arc_set_bg_angles(pump_info->right_level_arc, 315, 374);
    uint16_t end_angle = 0;
    if (level > 15) {
        end_angle = 328;
    } else if (level == 0) {
        end_angle = 380;
    } else {
        end_angle = ((380 - 328) * (15 - level) / 15) + 328;
    }
    // lv_arc_set_angles(pump_info->right_level_arc, end_angle, 350);
    lv_arc_set_angles(pump_info->right_level_arc, 328, 334);
    /* 背景圆弧 */
    lv_obj_set_style_arc_color(pump_info->right_level_arc,
         lv_color_hex(PUMP_ARC_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(pump_info->right_level_arc, 5, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(pump_info->right_level_arc, 97, LV_PART_MAIN);  /// 背景弧不透明度38%（97/255≈38%）

    /* 深色前景圆弧 */
    lv_obj_set_style_arc_color(pump_info->right_level_arc,
         lv_color_hex(PUMP_ARC_COLOR), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pump_info->right_level_arc, 5, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(pump_info->right_level_arc, LV_OPA_COVER, LV_PART_INDICATOR);  /// 前景弧完全不透明
    
    /* 去掉旋钮 */
    lv_obj_remove_style(pump_info->right_level_arc, NULL, LV_PART_KNOB);
}

static void pump_home_speed_label_update(ui_manager_context_t *thiz, uint8_t speed)
{

}

static void pump_home_mode_label_update(ui_manager_context_t *thiz, uint8_t mode_index)
{

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
    // thiz->priv.fps_count = 0;
    // thiz->priv.fps_start_time = get_timestamp();
    pump_home_init();  
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
    // battery_icon_deinit(thiz);
    common_ui_img_resource_deinit(thiz, &pump_info->background_info);
    common_ui_img_resource_deinit(thiz, &pump_info->mode_icon_info);
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
    pump_home_bg_update(thiz);
    uint16_t hold_time = 0;
    if (result != NULL) {
        hold_time = result->work_valid_time / 1000;
    }
    hold_time = 610;
    pump_home_time_update(thiz, hold_time / 60, hold_time % 60);
    uint8_t control_type = BREAST_PUMP_CONTROL_TYPE_BILATERAL;
    proc->st->get_store_ram_val(STORE_ID_PUMP_CONTROL_TYPE, 0, 0, &control_type);
    pump_setting_t setting;
    uint8_t time_index = proc->motor->get_runtime_policy_index(MOTOR_TIME_POLICY);
    proc->pump->get_breast_pump_setting(&setting, time_index >= ROOT_BREAST_PUMP_STEP_MAX ? 0 : time_index);
    uint8_t mode = proc->pump->get_breast_pump_mode();
    pump_home_left_level_label_update(thiz, setting.level[0], 
        control_type == BREAST_PUMP_CONTROL_TYPE_LEFT ? true : false);
    pump_home_left_level_arc_update(thiz, setting.level[0]);
    pump_home_right_level_label_update(thiz, setting.level[1], 
        control_type == BREAST_PUMP_CONTROL_TYPE_RIGHT ? true : false);
    pump_home_right_level_arc_update(thiz, setting.level[1]);
    pump_home_speed_label_update(thiz, setting.speed);
    pump_home_mode_label_update(thiz, mode);     
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> pump home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
        
    }
#endif
    return 0;
}
