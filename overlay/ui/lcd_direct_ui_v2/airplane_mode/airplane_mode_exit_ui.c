/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "airplane_mode_ui.h" // IWYU pragma: keep
#include "window_screen_airplane_mode.h" // IWYU pragma: keep
/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_AIRPLANE_MODE_COMP_BG                          (0)
#define AIRPLANE_MODE_TEXT_COUNT                                  (2)
#define AIRPLANE_MODE_ICON_COUNT                                  (1)
#define AIRPLANE_MODE_UNLOCK_BUTTON_ICON_INDEX                    (0)
#define AIRPLANE_MODE_TITLE_COLOR                                 0x1C1C20
#define AIRPLANE_MODE_CONTENT_COLOR                               0xB25962
// 新增：圆弧进度条配置
#define COUNTDOWN_ARC_SIZE            360    // 圆弧尺寸（匹配图片视觉）
#define COUNTDOWN_ARC_WIDTH           12      // 圆弧粗细
#define COUNTDOWN_ARC_COLOR           0xB25962 // 圆弧颜色（匹配吸奶器粉色）
#define COUNTDOWN_MAX_VALUE           5      // 倒计时最大值

/*
* TYPEDEFS
*****************************************************************************************
*/
typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
    const lv_font_t *font;
    uint16_t line_space; /// 行间距（像素），用于换行
} airplane_mode_exit_text_info_t;

typedef struct {
    lv_obj_t * root;
    lv_obj_t * backgroud;
    res_img_info_t background_info;
    lv_draw_buf_t *background_snapshot;  /// 背景快照，避免重复解码
    lv_obj_t * unlock_button_icon;
    lv_obj_t * texts[AIRPLANE_MODE_TEXT_COUNT];
    lv_obj_t * countdown_num;
    lv_obj_t * countdown_arc;            // 新增：环形进度条对象
    uint16_t countdown_value;
    res_img_info_t unlock_button_icon_info;
    uint8_t force_update;
    uint32_t start_time;
} airplane_mode_exit_display_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} airplane_mode_exit_icon_info_t;
/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
static PSRAM_DATA_LOC uint8_t complete_tip_bg_data[] = {
    #include "png2txt/airplay_exit_com_bg_360x360.txt"
};
#endif

static airplane_mode_exit_display_info_t *airplane_mode_exit_info;

const static airplane_mode_exit_text_info_t airplane_mode_exit_text_infos[AIRPLANE_MODE_TEXT_COUNT] = {
    { "Device locked", (110), (130), &lv_font_oppo_sans_20_medium ,0},
    { "Press and hold Start/Pause + Mode\n       for 5 seconds to unlock.", (48), (158), &lv_font_oppo_sans_16_medium ,0},
};

airplane_mode_exit_icon_info_t airplane_mode_exit_icon_infos[AIRPLANE_MODE_ICON_COUNT] = {
    { "unlock_button_icon", RESOURCE_ID_AIRPLANE_UNLOCK_BUTTON_ICON, 106, 188 },
};
/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void airplane_mode_exit_root_init(void)
{
    if (airplane_mode_exit_info->root == NULL || 
        !lv_obj_is_valid(airplane_mode_exit_info->root)) {
        airplane_mode_exit_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(airplane_mode_exit_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%
    lv_obj_set_style_bg_color(airplane_mode_exit_info->root, lv_color_black(), 0);     /// 黑色背景
    lv_obj_set_style_bg_opa(airplane_mode_exit_info->root, LV_OPA_50, 0);           /// 背景透明度50%
    lv_obj_set_style_border_width(airplane_mode_exit_info->root, 0, 0);                /// 无边框
    lv_obj_set_style_pad_all(airplane_mode_exit_info->root, 0, 0);                     /// 无内边距
    lv_obj_align(airplane_mode_exit_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(airplane_mode_exit_info->root, LV_OBJ_FLAG_CLICKABLE);           /// 不拦截点击事件
    lv_obj_clear_flag(airplane_mode_exit_info->root, LV_OBJ_FLAG_SCROLLABLE);          /// 禁止滚动
}

// 环形进度条更新函数（参照 pump_level_arc_progress_update）
static void airplane_mode_exit_arc_update(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(airplane_mode_exit_info);
    EXCEPTION_CHECK_VOID(airplane_mode_exit_info->root);

    /* 1. 创建 / 校验圆弧对象 */
    if (airplane_mode_exit_info->countdown_arc == NULL ||
        !lv_obj_is_valid(airplane_mode_exit_info->countdown_arc)) {

        lv_obj_t *arc = lv_arc_create(airplane_mode_exit_info->root);
        EXCEPTION_CHECK_VOID(arc);
        airplane_mode_exit_info->countdown_arc = arc;

        /* 基础配置：范围、背景角度、尺寸和位置 */
        lv_arc_set_range(arc, 0, COUNTDOWN_MAX_VALUE);      // 0 ~ 5
        lv_arc_set_bg_angles(arc, 0, 360);                  // 背景整圆

        lv_obj_set_size(arc, COUNTDOWN_ARC_SIZE, COUNTDOWN_ARC_SIZE);
        lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);

        /* 样式：隐藏背景弧和 knob，只显示指示弧 */
        lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_MAIN);          // 背景弧透明
        lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);              // knob 透明

        lv_obj_set_style_arc_color(arc, lv_color_hex(COUNTDOWN_ARC_COLOR),
                                   LV_PART_INDICATOR);                       // 指示弧颜色
        lv_obj_set_style_arc_width(arc, COUNTDOWN_ARC_WIDTH,
                                   LV_PART_INDICATOR);                       // 指示弧宽度
    }

    // progress: 0..COUNTDOWN_MAX_VALUE
    int16_t progress = (int16_t)COUNTDOWN_MAX_VALUE -
                       (int16_t)airplane_mode_exit_info->countdown_value;
    if (progress < 0)
      progress = 0;
    if (progress > (int16_t)COUNTDOWN_MAX_VALUE)
      progress = (int16_t)COUNTDOWN_MAX_VALUE;

    // 0~5 的值同步给 arc
    lv_arc_set_value(airplane_mode_exit_info->countdown_arc, progress);

    int16_t start, end;
    if (progress >= COUNTDOWN_MAX_VALUE) {
      // 倒计时=0 → 满圆
      start = 0;
      end = 359; // 用 359° 而不是 360°，避免 wrap
    } else {
      // 按比例从 270° 开始画
      start = 270;
      int16_t sweep =
          (int16_t)((360 * progress) / COUNTDOWN_MAX_VALUE); // 0..288
      end = start + sweep;
    }

    lv_arc_set_angles(airplane_mode_exit_info->countdown_arc, start, end);
}

#if IS_SUPPORT_AIRPLANE_MODE_COMP_BG
static void airplane_mode_exit_bg_update(ui_manager_context_t *thiz)
{
    static uint32_t start_time = 0;
    static uint8_t status = 0;
    if (airplane_mode_exit_info->backgroud == NULL || !lv_obj_is_valid(airplane_mode_exit_info->backgroud)) {
        airplane_mode_exit_info->backgroud = lv_image_create(airplane_mode_exit_info->root);
        EXCEPTION_CHECK_VOID(airplane_mode_exit_info->backgroud);
    }
    lv_fs_path_ex_t bg_img_path;
    if (status == 0) {
        lv_obj_add_flag(airplane_mode_exit_info->backgroud, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(airplane_mode_exit_info->backgroud, LV_OBJ_FLAG_HIDDEN);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, complete_tip_bg_data,
            sizeof(complete_tip_bg_data), "png");
        lv_image_set_src(airplane_mode_exit_info->backgroud, (const void *)&bg_img_path);
        lv_obj_align(airplane_mode_exit_info->backgroud, LV_ALIGN_CENTER, 0, 0);
    }

    if (get_timestamp() - start_time > 1000) {
        start_time = get_timestamp();
        status = (status == 0) ? 1 : 0;
    }
}
#else
static void airplane_mode_exit_bg_update(ui_manager_context_t *thiz)
{
    if (airplane_mode_exit_info->force_update == false) {
        return;
    }

    lv_fs_path_ex_t bg_img_path;
    if (airplane_mode_exit_info->backgroud == NULL || !lv_obj_is_valid(airplane_mode_exit_info->backgroud)) {
        airplane_mode_exit_info->backgroud = lv_image_create(airplane_mode_exit_info->root);
        EXCEPTION_CHECK_VOID(airplane_mode_exit_info->backgroud);
#if LV_USE_SNAPSHOT
        if (airplane_mode_exit_info->background_snapshot != NULL) {
            lv_draw_buf_destroy(airplane_mode_exit_info->background_snapshot);
            airplane_mode_exit_info->background_snapshot = NULL;
        }
#endif
        common_ui_img_resource_init(thiz, RESOURCE_ID_PUMP_HOME_BG, &airplane_mode_exit_info->background_info);
        EXCEPTION_CHECK_VOID(airplane_mode_exit_info->background_info.data);
        lv_fs_make_path_from_buffer(&bg_img_path, LV_FS_MEMFS_LETTER, 
            airplane_mode_exit_info->background_info.data, airplane_mode_exit_info->background_info.size, "png");
        lv_image_set_src(airplane_mode_exit_info->backgroud, (const void *)&bg_img_path);
        lv_obj_align(airplane_mode_exit_info->backgroud, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_img_opa(airplane_mode_exit_info->backgroud, LV_OPA_20, 0);
#if LV_USE_SNAPSHOT
        lv_refr_now(NULL); /// 等待渲染完成后再创建快照
        airplane_mode_exit_info->background_snapshot = lv_snapshot_take(airplane_mode_exit_info->backgroud, LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
#endif
    }

#if LV_USE_SNAPSHOT
    if (airplane_mode_exit_info->background_snapshot != NULL) {
        lv_image_set_src(airplane_mode_exit_info->backgroud, airplane_mode_exit_info->background_snapshot);
    }
#endif
}
#endif

static int airplane_mode_exit_countdown_num_update(ui_manager_context_t *thiz)
{
    if (get_timestamp() - airplane_mode_exit_info->start_time < 1000) {
        return 0;
    }
    if (airplane_mode_exit_info->countdown_num == NULL ||
        !lv_obj_is_valid(airplane_mode_exit_info->countdown_num)) {
        airplane_mode_exit_info->countdown_num = lv_label_create(airplane_mode_exit_info->root);
    }

    // 把数字转成字符串
    char num_buf[8];
    snprintf(num_buf, sizeof(num_buf), "%d", airplane_mode_exit_info->countdown_value);

    lv_label_set_text(airplane_mode_exit_info->countdown_num, num_buf);
    // 大号白色字体，居中
    lv_obj_set_style_text_color(airplane_mode_exit_info->countdown_num, lv_color_white(), 0);
    lv_obj_set_style_text_font(airplane_mode_exit_info->countdown_num, &lv_font_quicksand_96_bold, 0);
    lv_obj_set_style_text_align(airplane_mode_exit_info->countdown_num, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(airplane_mode_exit_info->countdown_num, LV_ALIGN_CENTER, 0, 0);

    // 新增：更新圆弧进度条
    airplane_mode_exit_arc_update(thiz);

    if (airplane_mode_exit_info->countdown_value > 0) {
        airplane_mode_exit_info->countdown_value--;
    }
    airplane_mode_exit_info->start_time = get_timestamp();
    return 0;
}

static void airplane_mode_exit_button_icon_update(ui_manager_context_t *thiz)
{
    if (airplane_mode_exit_info->force_update == false) {
        return;
    }
    if (airplane_mode_exit_info->unlock_button_icon == NULL || !lv_obj_is_valid(airplane_mode_exit_info->root)) {
        airplane_mode_exit_info->unlock_button_icon = lv_image_create(lv_screen_active());
    }
    common_ui_img_resource_init(thiz, RESOURCE_ID_AIRPLANE_UNLOCK_BUTTON_ICON, &airplane_mode_exit_info->unlock_button_icon_info);
    EXCEPTION_CHECK_VOID(airplane_mode_exit_info->unlock_button_icon_info.data);
    lv_fs_path_ex_t unlock_button_img_path;
    lv_fs_make_path_from_buffer(&unlock_button_img_path, LV_FS_MEMFS_LETTER,
        airplane_mode_exit_info->unlock_button_icon_info.data, airplane_mode_exit_info->unlock_button_icon_info.size, "baj");
    lv_image_set_src(airplane_mode_exit_info->unlock_button_icon, unlock_button_img_path.path);
    lv_obj_set_style_img_opa(airplane_mode_exit_info->unlock_button_icon, LV_OPA_20, 0);
    lv_obj_align(airplane_mode_exit_info->unlock_button_icon, LV_ALIGN_TOP_LEFT,
        airplane_mode_exit_icon_infos[AIRPLANE_MODE_UNLOCK_BUTTON_ICON_INDEX].icon_x,
        airplane_mode_exit_icon_infos[AIRPLANE_MODE_UNLOCK_BUTTON_ICON_INDEX].icon_y);
}

static void airplane_mode_exit_texts_update(ui_manager_context_t *thiz, uint8_t index)
{
    if (airplane_mode_exit_info->force_update == false) {
        return;
    }
    if (airplane_mode_exit_info->texts[index] == NULL || !lv_obj_is_valid(airplane_mode_exit_info->texts[index])) {
        airplane_mode_exit_info->texts[index] = lv_label_create(airplane_mode_exit_info->root);
    }
    lv_label_set_text(airplane_mode_exit_info->texts[index], airplane_mode_exit_text_infos[index].name);
    lv_obj_set_style_text_color(airplane_mode_exit_info->texts[index], 
        lv_color_hex(AIRPLANE_MODE_CONTENT_COLOR), 0);
    lv_obj_set_style_text_font(airplane_mode_exit_info->texts[index], airplane_mode_exit_text_infos[index].font, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(airplane_mode_exit_info->texts[index], airplane_mode_exit_text_infos[index].line_space, 0);  /// 设置行间距
    lv_obj_set_style_text_opa(airplane_mode_exit_info->texts[index], LV_OPA_20, 0);
    lv_obj_align(airplane_mode_exit_info->texts[index], LV_ALIGN_TOP_LEFT,
        airplane_mode_exit_text_infos[index].text_x, airplane_mode_exit_text_infos[index].text_y);
}
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_airplane_mode_exit_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (airplane_mode_exit_info == NULL) {
        airplane_mode_exit_info = (airplane_mode_exit_display_info_t *)u_malloz(sizeof(airplane_mode_exit_display_info_t));
        EXCEPTION_CHECK_VOID(airplane_mode_exit_info);
    }
    airplane_mode_exit_info->force_update = true;
    airplane_mode_exit_info->countdown_value = 5;
    airplane_mode_exit_info->start_time = get_timestamp(); // 初始化倒计时起始时间
#if 0
    ble_ui_init(thiz);
#endif
    airplane_mode_exit_root_init();
    // 新增：初始化圆弧进度条（首次显示）
    airplane_mode_exit_arc_update(thiz);
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
}

void ui_display_airplane_mode_exit_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (airplane_mode_exit_info == NULL) {
        return;
    }
#if 0
    ble_ui_deinit(thiz);
    common_ui_img_resource_deinit(thiz, &airplane_mode_exit_info->background_info);
    common_ui_img_resource_deinit(thiz, &airplane_mode_exit_info->unlock_button_icon_info);
#endif
    ///删除对象
    LVGL_OBJ_DELETE(airplane_mode_exit_info->root); 
    u_free(airplane_mode_exit_info);
    airplane_mode_exit_info = NULL;
}

int ui_display_airplane_mode_exit_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(airplane_mode_exit_info);
    EXCEPTION_CHECK_INT(airplane_mode_exit_info->root);
#if 0
    ui_proc_airplane_mode_t *proc = (ui_proc_airplane_mode_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc);
    EXCEPTION_CHECK_INT(proc->ble);
    airplane_mode_exit_bg_update(thiz);
    airplane_mode_exit_button_icon_update(thiz);
    ble_ui_update(thiz, airplane_mode_exit_info->root, proc->ble);
    for (uint8_t i = 0; i < AIRPLANE_MODE_TEXT_COUNT; i++) {
        airplane_mode_exit_texts_update(thiz, i);
    }
#endif
    // 数字更新时自动同步圆弧进度
    airplane_mode_exit_countdown_num_update(thiz);
    airplane_mode_exit_info->force_update = false;
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        thiz->priv.fps_count = 0;
        LOG_UI("===> airplane mode fps=%d", thiz->priv.fps_count);
    }
    return thiz->priv.fps_count;
}