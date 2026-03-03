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
#define IS_SUPPORT_PUMP_STATUS_PROMPT_COMP_BG                      (0)
#define PUMP_STATUS_PROMPT_ICON_MAX_COUNT                         (1)
#define PUMP_STATUS_PROMPT_TEXT_COUNT                              (2)

#define PUMP_START_PROMPT_ICON_X                       (148)
#define PUMP_START_PROMPT_ICON_Y                       (118)
#define PUMP_PAUSE_PROMPT_ICON_X                       (148)
#define PUMP_PAUSE_PROMPT_ICON_Y                       (118)

#define PROMPT_TEXT_COLOR                             (0xFFFFFF)

typedef struct {
    const char *name;
    uint16_t img_id;
    uint16_t icon_x;
    uint16_t icon_y;
} pump_status_prompt_icon_info_t;

typedef struct {
    const char *name;
    uint16_t text_x;
    uint16_t text_y;
    const lv_font_t *font;
    uint16_t line_space; /// 行间距（像素），用于换行
} pump_status_prompt_text_info_t;

typedef struct {
  int8_t status; /// 当前泵状态
  uint8_t is_show; /// 是否显示
} pump_status_prompt_parameter_info_t;

typedef struct {
    lv_obj_t * sub_root;
#if IS_SUPPORT_PUMP_STATUS_PROMPT_COMP_BG
    lv_obj_t * background;
#endif
    lv_obj_t * pump_status_prompt_icon[PUMP_STATUS_PROMPT_ICON_MAX_COUNT];
    res_img_info_t pump_status_prompt_icon_info[PUMP_STATUS_PROMPT_ICON_MAX_COUNT];
    lv_obj_t * pump_status_prompt_text[PUMP_STATUS_PROMPT_TEXT_COUNT];
    uint8_t force_update;
    uint8_t current_status;                 // 当前泵状态
} pump_status_prompt_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PUMP_STATUS_PROMPT_COMP_BG
static PSRAM_DATA_LOC uint8_t pump_status_prompt_bg_data[] = {
    #include "comparison_png2txt/pump_status_prompt_bg_360x360.txt"
};
#endif

static pump_status_prompt_display_info_t *pump_status_prompt_info;

const static pump_status_prompt_icon_info_t pump_status_prompt_icon_infos[PUMP_STATUS_PROMPT_ICON_MAX_COUNT] = {
    /////{ "pump_start_prompt_icon", RESOURCE_ID_PUMP_PAUSE_PROMPT_ICON,
    /// PUMP_START_PROMPT_ICON_X,              PUMP_START_PROMPT_ICON_Y, 
    ///},
    { "pump_pause_prompt_icon", RESOURCE_ID_PUMP_PAUSE_PROMPT_ICON,
      PUMP_PAUSE_PROMPT_ICON_X,              PUMP_PAUSE_PROMPT_ICON_Y, 
    },
};
const static pump_status_prompt_text_info_t pump_status_prompt_text_infos[PUMP_STATUS_PROMPT_TEXT_COUNT] = {
    //{ "Pumping paused", 132, 198, &lv_font_oppo_sans_16_medium, 2 },
    //{ "Auto shutoff after 15 minutes of inactivity", 60, 222, &lv_font_oppo_sans_14_medium, 2 },
    { "Pumping paused", 126, 198, &lv_font_oppo_sans_16_medium, 2},
    { "Auto shutoff after 15 minutes of inactivity", 60, 222, &lv_font_oppo_sans_14_medium, 2 },
};
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_status_prompt_root_init(lv_obj_t *root)
{
    if (pump_status_prompt_info->sub_root == NULL || !lv_obj_is_valid(pump_status_prompt_info->sub_root)) {
        pump_status_prompt_info->sub_root  = lv_obj_create(root);
        EXCEPTION_CHECK_VOID(pump_status_prompt_info->sub_root);
        lv_obj_set_size(pump_status_prompt_info->sub_root, LV_PCT(100), LV_PCT(100));          /// 设置容器尺寸为屏幕的100%（自动适配屏幕分辨率）
        lv_obj_set_style_bg_color(pump_status_prompt_info->sub_root, lv_color_black(), 0);     /// 设置背景颜色为黑色
        lv_obj_set_style_bg_opa(pump_status_prompt_info->sub_root, LV_OPA_50, 0);           /// 设置背景不透明度为完全不透明（覆盖下层）
        lv_obj_set_style_border_width(pump_status_prompt_info->sub_root, 0, 0);                /// 设置边框宽度为0（无边框）
        lv_obj_set_style_pad_all(pump_status_prompt_info->sub_root, 0, 0);                     /// 设置所有内边距为0（子对象紧贴边缘）
        lv_obj_align(pump_status_prompt_info->sub_root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(pump_status_prompt_info->sub_root, LV_OBJ_FLAG_CLICKABLE); /// 让 root 不吃事件，防止挡子控件
        lv_obj_clear_flag(pump_status_prompt_info->sub_root, LV_OBJ_FLAG_SCROLLABLE); /// 禁止滚动
    }
}

static void pump_status_prompt_root_text_update(ui_manager_context_t *thiz, lv_obj_t *root, pump_status_prompt_parameter_info_t *pump_status_prompt_parameter)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(root);
    EXCEPTION_CHECK_VOID(pump_status_prompt_parameter);
    /// 创建 / 校验子根对象
    pump_status_prompt_root_init(root);
    if (pump_status_prompt_parameter->is_show == false) {
        lv_obj_add_flag(pump_status_prompt_info->sub_root, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(pump_status_prompt_info->sub_root, LV_OBJ_FLAG_HIDDEN);
    /// 设置文本区域icon
    if (pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status] == NULL || !lv_obj_is_valid(pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status])) {
        pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status] = lv_image_create(pump_status_prompt_info->sub_root);
        EXCEPTION_CHECK_VOID(pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status]);
    }
    
    common_ui_img_resource_init(thiz, pump_status_prompt_icon_infos[pump_status_prompt_parameter->status].img_id, &pump_status_prompt_info->pump_status_prompt_icon_info[pump_status_prompt_parameter->status]);
    EXCEPTION_CHECK_VOID(pump_status_prompt_info->pump_status_prompt_icon_info[pump_status_prompt_parameter->status].data);
    lv_fs_path_ex_t pump_status_prompt_icon_img_path;
    lv_fs_make_path_from_buffer(&pump_status_prompt_icon_img_path, LV_FS_MEMFS_LETTER, 
        pump_status_prompt_info->pump_status_prompt_icon_info[pump_status_prompt_parameter->status].data, pump_status_prompt_info->pump_status_prompt_icon_info[pump_status_prompt_parameter->status].size, "baj");
    lv_image_set_src(pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status], (const void *)&pump_status_prompt_icon_img_path);
    lv_obj_align(pump_status_prompt_info->pump_status_prompt_icon[pump_status_prompt_parameter->status], LV_ALIGN_TOP_LEFT, 
        pump_status_prompt_icon_infos[pump_status_prompt_parameter->status].icon_x, pump_status_prompt_icon_infos[pump_status_prompt_parameter->status].icon_y); /// 设置icon位置
    
    /// 设置文本label UI第一行
    lv_obj_t *text = pump_status_prompt_info->pump_status_prompt_text[pump_status_prompt_parameter->status];
    if (text == NULL || !lv_obj_is_valid(text)) {
        text = lv_label_create(pump_status_prompt_info->sub_root);
        EXCEPTION_CHECK_VOID(text);
        pump_status_prompt_info->pump_status_prompt_text[pump_status_prompt_parameter->status] = text;
    }
    lv_label_set_text(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(text, lv_color_hex(PROMPT_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status].font, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status].line_space, 0);  /// 设置行间距（像素）
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);  /// 设置文本居中对齐
    lv_obj_align(text, LV_ALIGN_TOP_LEFT, pump_status_prompt_text_infos[pump_status_prompt_parameter->status].text_x, pump_status_prompt_text_infos[pump_status_prompt_parameter->status].text_y); /// 设置文字位置

    /// 设置文本label UI第二行
    text = pump_status_prompt_info->pump_status_prompt_text[pump_status_prompt_parameter->status + 1];
    if (text == NULL || !lv_obj_is_valid(text)) {
        text = lv_label_create(pump_status_prompt_info->sub_root);
        EXCEPTION_CHECK_VOID(text);
        pump_status_prompt_info->pump_status_prompt_text[pump_status_prompt_parameter->status + 1] = text;
    }
    lv_label_set_text(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status + 1].name);  /// 设置 label 文字内容和样式
    lv_obj_set_style_text_color(text, lv_color_hex(PROMPT_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status + 1].font, 0);  /// 设置字体
    lv_obj_set_style_text_line_space(text, pump_status_prompt_text_infos[pump_status_prompt_parameter->status + 1].line_space, 0);  /// 设置行间距（像素）
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);  /// 设置文本居中对齐
    lv_obj_align(text, LV_ALIGN_TOP_LEFT, pump_status_prompt_text_infos[pump_status_prompt_parameter->status + 1].text_x, pump_status_prompt_text_infos[pump_status_prompt_parameter->status + 1].text_y); /// 设置文字位置
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_pump_status_prompt_init(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    if (pump_status_prompt_info == NULL) {
        pump_status_prompt_info = (pump_status_prompt_display_info_t *)u_malloz(sizeof(pump_status_prompt_display_info_t));
        EXCEPTION_CHECK_VOID(pump_status_prompt_info);
    }
    pump_status_prompt_info->force_update = true;
    pump_status_prompt_info->current_status = 0;
}

void ui_display_pump_status_prompt_deinit(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(pump_status_prompt_info);
    for (uint8_t i = 0; i < PUMP_STATUS_PROMPT_ICON_MAX_COUNT; i++) {
        common_ui_img_resource_deinit(thiz, &pump_status_prompt_info->pump_status_prompt_icon_info[i]);
    }
    for (uint8_t i = 0; i < PUMP_STATUS_PROMPT_ICON_MAX_COUNT; i++) {
        pump_status_prompt_info->pump_status_prompt_icon[i] = NULL;
    }
    for (uint8_t i = 0; i < PUMP_STATUS_PROMPT_TEXT_COUNT; i++) {
        pump_status_prompt_info->pump_status_prompt_text[i] = NULL;
    }
    LVGL_OBJ_DELETE(pump_status_prompt_info->sub_root);
    pump_status_prompt_info->sub_root = NULL;
#if IS_SUPPORT_PUMP_STATUS_PROMPT_COMP_BG
    pump_status_prompt_info->background = NULL;
#endif
    pump_status_prompt_info->force_update = false;
    pump_status_prompt_info->current_status = 0;
    u_free(pump_status_prompt_info);
    pump_status_prompt_info = NULL;
}

void ui_display_pump_status_prompt_process(ui_manager_context_t *thiz, lv_obj_t *root)
{
    EXCEPTION_CHECK_VOID(thiz);
    EXCEPTION_CHECK_VOID(root);
    EXCEPTION_CHECK_VOID(pump_status_prompt_info);
    /// TODO: 获取当前泵状态 
    ///TODO: 是否隐藏
    int8_t status = 0; //1 START, 0 PAUSE
    pump_status_prompt_parameter_info_t pump_status_prompt_parameter = {
        .status = 0, //1 START, 0 PAUSE 只做了一个资源 强制赋值0
        .is_show = 1,
    };
    pump_status_prompt_root_text_update(thiz, root, &pump_status_prompt_parameter);
    pump_status_prompt_info->force_update = false;
}
