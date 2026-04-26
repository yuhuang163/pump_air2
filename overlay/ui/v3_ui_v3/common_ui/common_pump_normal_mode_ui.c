/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_MODE_COMP_BG                (0)
#define MODE_COUNT                             (3)
#define MODE_CIRCLE_SLOT_COUNT                 (3)   /// 仅保留左/中/右 3 个圆槽位
#define MODE_VIEW_W                            (360)
#define MODE_VIEW_H                            (360)
#define MODE_CIRCLE_W                          (250) /// 圆圈宽度
#define MODE_CIRCLE_H                          (250) /// 圆圈高度
#define MODE_CIRCLE_CENTER_X_GAP               (MODE_CIRCLE_W + 20)  /// 圆圈中心间距
#define MODE_SIDE_CIRCLE_Y_OFFSET              (24) /// 左右两个圆都比中间圆高 24

#define MODE_TEXT_COLOR                        (0xB25962)
#define MODE_CENTER_CIRCLE_GRAD_TOP_COLOR      (0xEB7581)
#define MODE_CENTER_CIRCLE_GRAD_BOTTOM_COLOR   (0xF0868A)
#define MODE_SIDE_CIRCLE_GRAD_TOP_COLOR        (0xF7A1A8)
#define MODE_SIDE_CIRCLE_GRAD_BOTTOM_COLOR     (0xF7A1A8)
#define MODE_TITLE_TEXT_COLOR                  (0xFFFFFF)
#define MODE_DESC_TEXT_COLOR                   (0xFFDDDF)
#define MODE_CENTER_DESC_TEXT_COLOR            (0xFFEAEA)

#define MODE_PAGE_TITLE_Y_OFFSET               (34)
#define MODE_COMPONENT_CENTER_Y_OFFSET         (-70)
#define MODE_TITLE_BASE_Y_OFFSET               (-26)
#define MODE_DESC_BASE_Y_OFFSET                (6)

#define MODE_SWITCH_ANIM_TIME_MS               (260)  /// 切换动画时间，单位：毫秒

typedef enum {
    MODE_SLOT_LEFT = 0,
    MODE_SLOT_CENTER,
    MODE_SLOT_RIGHT,
    MODE_SLOT_MAX,
} mode_circle_slot_e;

typedef struct ui_display_mode_info {
    uint16_t slot_res_id[MODE_CIRCLE_SLOT_COUNT];
    int16_t slot_mode_idx[MODE_CIRCLE_SLOT_COUNT];
    uint8_t cur_mode;
    uint8_t force_update;
    uint8_t mode_obj_init_done;
    uint8_t popup_ui_index;
    uint8_t rsv[8];
} mode_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static mode_display_info_t *mode_info;

static const uint16_t mode_component_ids[MODE_COUNT] = {
    // RESOURCE_ID_MODE_STIMULATE_ICON, /// 舒适
    // RESOURCE_ID_MODE_LACTATION_ICON, /// 常规
    // RESOURCE_ID_MODE_MIX_ICON, /// 快速
};

static const uint16_t dots_img_ids[MODE_COUNT] = {
    // RESOURCE_ID_SELECTED_LOCATE_3_START,
    // RESOURCE_ID_SELECTED_LOCATE_3_START + 1,
    // RESOURCE_ID_SELECTED_LOCATE_3_END,
};

static const char *mode_name[MODE_COUNT] = {
    // "Stimulation", /// 慢速
    // "Expression", /// 常规
    // "Mixed", /// 快速
};

static const char *mode_desc[MODE_COUNT] = {
    "Fast, gentle suction to\ntrigger let-down.",
    "Slow, deep suction for\nefficient milk expression.",
    "Alternates 5 times of fast,\ngentle suction with 1 slow,\ndeep suction for let-downs\nand better emptying.",
};

#if IS_SUPPORT_MODE_COMP_BG
static PSRAM_DATA_LOC uint8_t mode_bg_data[] = {
    #include "comparison_png2txt/stimulation_mode_info_360x360.txt"
};
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void update_slot_content(ui_manager_context_t *thiz, mode_circle_slot_e slot, int16_t mode_idx)
{

}

static void circle_layout_apply(int8_t direction, bool anim)
{

}

static void circle_carousel_set_mode(ui_manager_context_t *thiz, uint8_t idx, bool anim)
{

}

static void mode_root_init(ui_manager_context_t *thiz)
{

}

static void mode_bg_update(ui_manager_context_t *thiz)
{

}


static void mode_ui_update(ui_manager_context_t *thiz, uint8_t mode_idx)
{

}

static void mode_text_update(ui_manager_context_t *thiz)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void common_pump_normal_mode_ui_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (mode_info == NULL) {
    //     mode_info = (mode_display_info_t *)u_malloz(sizeof(mode_display_info_t));
    //     EXCEPTION_CHECK_VOID(mode_info);
    // }
    // mode_info->pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    // if (mode_info->pump_ctx == NULL) {
    //     u_free(mode_info);
    //     mode_info = NULL;
    //     return;
    // }
    // mode_root_init(thiz);
    // common_ui_bg_init(thiz);
    // mode_info->force_update = true;
    // mode_info->mode_obj_init_done = false;
    // mode_info->popup_ui_index = ID_POPUP_UI_NONE;
}

void common_pump_normal_mode_ui_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz && mode_info);
    // common_ui_img_resource_deinit(thiz, &mode_info->dots_img_info);
    // for (uint8_t i = 0; i < MODE_CIRCLE_SLOT_COUNT; i++) {
    //     common_ui_img_resource_deinit(thiz, &mode_info->mode_component_info[i]);
    // }
    // common_ui_bg_deinit(thiz);
    // ui_common_operation_tip_remove(thiz);
    // LVGL_OBJ_DELETE(mode_info->root);
    // u_free(mode_info);
    // mode_info = NULL;
}

int common_pump_normal_mode_ui_process(ui_manager_context_t *thiz, uint8_t mode_idx, uint16_t popup_ui_type)
{
    // EXCEPTION_CHECK_INT(thiz && mode_info && mode_info->root);
    // mode_bg_update(thiz);
    // mode_ui_update(thiz, mode_idx);
    // mode_text_update(thiz);
    // ui_common_operation_tip_update(thiz, popup_ui_type, mode_info->root, &mode_info->popup_ui_index);
    // mode_info->force_update = false;
    
    return 0;
}
