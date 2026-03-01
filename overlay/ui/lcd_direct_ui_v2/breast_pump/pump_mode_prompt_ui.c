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

typedef struct {
  int8_t mode; /// 当前泵模式
  uint8_t is_show; /// 是否显示
} mode_prompt_parameter_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static lv_obj_t * sub_root;
static lv_obj_t * backgroud;
static res_img_info_t background_info;
static lv_draw_buf_t *background_snapshot;  /// 背景快照，避免重复解码
static lv_obj_t * text;
static lv_obj_t * mode_prompt_icon;
static res_img_info_t mode_prompt_icon_info;
static uint8_t force_update;
static uint8_t current_mode;

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
    { "Switching to Stimulation Mode", (72), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Expression Mode", (78), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Mix Mode", (106), (210), &lv_font_oppo_sans_16_medium ,0},
    { "Switching to Relax Mode", (98), (210), &lv_font_oppo_sans_16_medium ,0},
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
static void mode_prompt_root_init(lv_obj_t *root)
{
    if (sub_root == NULL || !lv_obj_is_valid(sub_root)) {
        sub_root = lv_obj_create(root);
        EXCEPTION_CHECK_VOID(sub_root);
        lv_obj_set_size(sub_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
        lv_obj_set_style_bg_color(sub_root, lv_color_black(), 0);     /// 设置背景颜色为黑色
        lv_obj_set_style_bg_opa(sub_root, LV_OPA_50, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
        lv_obj_set_style_border_width(sub_root, 0, 0);                /// 设置边框宽度为0（无边框）
        lv_obj_set_style_pad_all(sub_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
        lv_obj_align(sub_root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(sub_root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
        lv_obj_clear_flag(sub_root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
    }
}

static void mode_prompt_root_text_update(ui_manager_context_t *thiz, lv_obj_t *root, mode_prompt_parameter_info_t *mode_prompt_parameter)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(root);
    EXCEPTION_CHECK_VOID(mode_prompt_parameter);
    /// 创建 / 校验子根对象
    mode_prompt_root_init(root);
    if (mode_prompt_parameter->is_show == false) {
        lv_obj_add_flag(sub_root, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(sub_root, LV_OBJ_FLAG_HIDDEN);
    // 边界检查：确保 mode 在有效范围内（0-3）
    if (mode_prompt_parameter->mode >= MODE_PROMPT_ICON_MAX_COUNT) {
        LOG_UI("mode_prompt_root_text_update: invalid mode %d, using default 0", mode_prompt_parameter->mode);
        mode_prompt_parameter->mode = 0;
    }
    
    if (mode_prompt_parameter->mode == current_mode && force_update == false) {
        return;
    }
    current_mode = mode_prompt_parameter->mode;
    /// 设置文本区域icon
    if (mode_prompt_icon == NULL || !lv_obj_is_valid(mode_prompt_icon)) {
        mode_prompt_icon = lv_image_create(sub_root);
        EXCEPTION_CHECK_VOID(mode_prompt_icon);
    }
    
    common_ui_img_resource_init(thiz, mode_prompt_icon_infos[mode_prompt_parameter->mode].img_id, &mode_prompt_icon_info);
    EXCEPTION_CHECK_VOID(mode_prompt_icon_info.data);
    lv_fs_path_ex_t mode_prompt_icon_img_path;
    lv_fs_make_path_from_buffer(&mode_prompt_icon_img_path, LV_FS_MEMFS_LETTER, 
        mode_prompt_icon_info.data, mode_prompt_icon_info.size, "baj");
    lv_image_set_src(mode_prompt_icon, (const void *)&mode_prompt_icon_img_path);
    lv_obj_align(mode_prompt_icon, LV_ALIGN_TOP_LEFT, 
        mode_prompt_icon_infos[mode_prompt_parameter->mode].icon_x, mode_prompt_icon_infos[mode_prompt_parameter->mode].icon_y); /// 设置icon位置
    /// 设置文本label
    if (text == NULL || !lv_obj_is_valid(text)) {
        text = lv_label_create(sub_root);
        EXCEPTION_CHECK_VOID(text);
    }
    lv_label_set_text(text, mode_prompt_text_infos[mode_prompt_parameter->mode].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(text, lv_color_hex(PROMPT_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(text, mode_prompt_text_infos[mode_prompt_parameter->mode].font, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(text, mode_prompt_text_infos[mode_prompt_parameter->mode].line_space, 0);  /// 设置行间距（像素）
    lv_obj_align(text, LV_ALIGN_TOP_LEFT, mode_prompt_text_infos[mode_prompt_parameter->mode].text_x, mode_prompt_text_infos[mode_prompt_parameter->mode].text_y); /// 设置文字位置
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_mode_prompt_init(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    force_update = true;
    current_mode = 0;
}

void ui_display_mode_prompt_deinit(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    common_ui_img_resource_deinit(thiz, &mode_prompt_icon_info);
    //LVGL_OBJ_DELETE(sub_root);
    sub_root = NULL;
    backgroud = NULL;
    text = NULL;
    mode_prompt_icon = NULL;
    force_update = false;
    current_mode = 0;
}

void ui_display_mode_prompt_process(ui_manager_context_t *thiz, lv_obj_t *root)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(root);
    /// TODO: 获取当前泵模式 
    ///TODO: 界面显示时间，是否隐藏
    uint8_t mode = 2; 
    mode_prompt_parameter_info_t mode_prompt_parameter = {
        .mode = mode,
        .is_show = 0,
    };
    mode_prompt_root_text_update(thiz, root, &mode_prompt_parameter);
    force_update = false;
}
