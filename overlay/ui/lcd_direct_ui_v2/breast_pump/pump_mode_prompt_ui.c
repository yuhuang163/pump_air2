/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_popup.h" // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep
/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_ROOT_MODE_PROMPT_BG                      (0)
#define MODE_PROMPT_ICON_MAX_COUNT                         (4)
#define MODE_PROMPT_TEXT_COUNT                              (4)

#define STIMULATION_PROMPT_ICON_X                       (148)
#define STIMULATION_PROMPT_ICON_Y                       (130)
#define EXPRESSION_PROMPT_ICON_X                       (148)
#define EXPRESSION_PROMPT_ICON_Y                       (130)
#define MIX_PROMPT_ICON_X                       (148)
#define MIX_PROMPT_ICON_Y                       (130)
#define RELAX_PROMPT_ICON_X                       (148)
#define RELAX_PROMPT_ICON_Y                       (130)


#define PROMPT_TEXT_COLOR                             (0xE6DFDF)


typedef struct {
    lv_obj_t * root;
    lv_obj_t * backgroud;
    res_img_info_t background_info;
    lv_draw_buf_t *background_snapshot;  /// 背景快照，避免重复解码
    lv_obj_t * text;
    lv_obj_t * mode_prompt_icon;
    res_img_info_t mode_prompt_icon_info;
    uint8_t force_update;
    uint8_t current_mode;
} mode_prompt_display_info_t;

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} mode_prompt_icon_info_t;

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
    const lv_font_t *font;
    uint16_t line_space; /// 行间距（像素），用于换行
} mode_prompt_text_info_t;
/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static mode_prompt_display_info_t *mode_prompt_info;
const static mode_prompt_icon_info_t mode_prompt_icon_infos[MODE_PROMPT_ICON_MAX_COUNT] = {
    { "stimulation_prompt_icon", RESOURCE_ID_MODE_STIMULATION_PROMPT_ICON,
      STIMULATION_PROMPT_ICON_X,              STIMULATION_PROMPT_ICON_Y, 
    },
    { "expression_prompt_icon", RESOURCE_ID_MODE_EXPRESSION_PROMPT_ICON,
      EXPRESSION_PROMPT_ICON_X,              EXPRESSION_PROMPT_ICON_Y, 
    },
    { "mix_prompt_icon", RESOURCE_ID_MODE_MIX_PROMPT_ICON,
      MIX_PROMPT_ICON_X,              MIX_PROMPT_ICON_Y, 
    },
    { "relax_prompt_icon", RESOURCE_ID_MODE_RELAX_PROMPT_ICON,
      RELAX_PROMPT_ICON_X,              RELAX_PROMPT_ICON_Y, 
    },
};

const static mode_prompt_text_info_t mode_prompt_text_infos[MODE_PROMPT_TEXT_COUNT] = {
    { "Switching to Stimulation Mode", (64), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Expression Mode", (78), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Mix Mode", (88), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Relax Mode", (82), (210), &lv_font_oppo_sans_16_medium ,0},
};
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_ROOT_MODE_PROMPT_BG
static PSRAM_DATA_LOC uint8_t mode_prompt_bg_data[] = {
    // #include "comparison_png2txt/bg_360x360.txt"
    #include "png2txt/mode_prompt_bg_360x360.txt"
    // #include "comparison_png2txt/home_comp_1_v2_360x360.txt"
    // #include "comparison_png2txt/home_comp_80_v2_360x360.txt"
    // #include "comparison_png2txt/home_comp_100_v2_360x360.txt"
};
#endif
/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void mode_prompt_root_init(void)
{
    if (mode_prompt_info->root == NULL || !lv_obj_is_valid(mode_prompt_info->root)) {
        mode_prompt_info->root = lv_obj_create(lv_screen_active());
    }
    lv_obj_set_size(mode_prompt_info->root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
    lv_obj_set_style_bg_color(mode_prompt_info->root, lv_color_black(), 0);     /// 设置背景颜色为黑色
    lv_obj_set_style_bg_opa(mode_prompt_info->root, LV_OPA_50, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
    lv_obj_set_style_border_width(mode_prompt_info->root, 0, 0);                /// 设置边框宽度为0（无边框）
    lv_obj_set_style_pad_all(mode_prompt_info->root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
    lv_obj_align(mode_prompt_info->root, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(mode_prompt_info->root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
    lv_obj_clear_flag(mode_prompt_info->root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
}

static void mode_prompt_root_text_update(ui_manager_context_t *thiz, uint8_t mode)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(mode_prompt_info);
    EXCEPTION_CHECK_VOID(mode_prompt_info->root);
    
    // 边界检查：确保 mode 在有效范围内（0-3）
    if (mode >= MODE_PROMPT_ICON_MAX_COUNT) {
        LOG_UI("mode_prompt_root_text_update: invalid mode %d, using default 0", mode);
        mode = 0;
    }
    
    if (mode == mode_prompt_info->current_mode && mode_prompt_info->force_update == false) {
        return;
    }
    mode_prompt_info->current_mode = mode;
    /// 设置文本区域icon
    if (mode_prompt_info->mode_prompt_icon == NULL || !lv_obj_is_valid(mode_prompt_info->mode_prompt_icon)) {
        mode_prompt_info->mode_prompt_icon = lv_image_create(mode_prompt_info->root);
        EXCEPTION_CHECK_VOID(mode_prompt_info->mode_prompt_icon);
    }
    
    common_ui_img_resource_init(thiz, mode_prompt_icon_infos[mode].img_id, &mode_prompt_info->mode_prompt_icon_info);
    EXCEPTION_CHECK_VOID(mode_prompt_info->mode_prompt_icon_info.data);
    lv_fs_path_ex_t mode_prompt_icon_img_path;
    lv_fs_make_path_from_buffer(&mode_prompt_icon_img_path, LV_FS_MEMFS_LETTER, 
        mode_prompt_info->mode_prompt_icon_info.data, mode_prompt_info->mode_prompt_icon_info.size, "baj");
    lv_image_set_src(mode_prompt_info->mode_prompt_icon, (const void *)&mode_prompt_icon_img_path);
    lv_obj_align(mode_prompt_info->mode_prompt_icon, LV_ALIGN_TOP_LEFT, 
        mode_prompt_icon_infos[mode].icon_x, mode_prompt_icon_infos[mode].icon_y); /// 设置icon位置
    /// 设置文本label
    if (mode_prompt_info->text == NULL || !lv_obj_is_valid(mode_prompt_info->text)) {
        mode_prompt_info->text = lv_label_create(mode_prompt_info->root);
        EXCEPTION_CHECK_VOID(mode_prompt_info->text);
    }
    lv_label_set_text(mode_prompt_info->text, mode_prompt_text_infos[mode].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(mode_prompt_info->text, lv_color_hex(PROMPT_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(mode_prompt_info->text, mode_prompt_text_infos[mode].font, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(mode_prompt_info->text, mode_prompt_text_infos[mode].line_space, 0);  /// 设置行间距（像素）
    lv_obj_align(mode_prompt_info->text, LV_ALIGN_TOP_LEFT, mode_prompt_text_infos[mode].text_x, mode_prompt_text_infos[mode].text_y); /// 设置文字位置
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_mode_prompt_prepare(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (mode_prompt_info == NULL) {
        mode_prompt_info = (mode_prompt_display_info_t *)u_malloz(sizeof(mode_prompt_display_info_t));
        EXCEPTION_CHECK_VOID(mode_prompt_info);
    }
    thiz->priv.fps_count = 0;
    thiz->priv.fps_start_time = get_timestamp();
    mode_prompt_root_init();
    mode_prompt_info->force_update = true;
    mode_prompt_info->current_mode = 0;
}

void ui_display_mode_prompt_remove(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (mode_prompt_info == NULL) {
        return;
    }

    common_ui_img_resource_deinit(thiz, &mode_prompt_info->mode_prompt_icon_info);
    LVGL_OBJ_DELETE(mode_prompt_info->root);
    u_free(mode_prompt_info);
    mode_prompt_info = NULL;
}

int ui_display_mode_prompt_process(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    EXCEPTION_CHECK_INT(mode_prompt_info);
    EXCEPTION_CHECK_INT(mode_prompt_info->root);
    /// TODO: 获取当前泵模式 存在问题
    ui_proc_popup_t *proc = (ui_proc_popup_t *)thiz->priv.play_info.user_data;
    EXCEPTION_CHECK_INT(proc);
    mode_prompt_root_text_update(thiz, 2/*proc->pump->get_breast_pump_mode()*/);
    mode_prompt_info->force_update = false;
    thiz->priv.fps_count++;
    if (get_timestamp() - thiz->priv.fps_start_time >= 1000) {
        thiz->priv.fps_start_time = get_timestamp();
        LOG_UI("===> home fps: %d", thiz->priv.fps_count);
        thiz->priv.fps_count = 0;
    }
    return 0;
}
