/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_SPEED_COMP_BG                (0)
#define SPEED_COUNT                             (3)
#define SPEED_CIRCLE_SLOT_COUNT                 (3)   /// 仅保留左/中/右 3 个圆槽位
#define SPEED_VIEW_W                            (360)
#define SPEED_VIEW_H                            (360)
#define SPEED_CIRCLE_W                          (250)
#define SPEED_CIRCLE_H                          (250)
#define SPEED_CIRCLE_CENTER_X_GAP               (SPEED_CIRCLE_W + 20)  /// 圆圈中心间距
#define SPEED_SIDE_CIRCLE_Y_OFFSET              (24)  /// 左右两个圆都比中间圆高 24
#define SPEED_DOT_COLOR                         (0xB25962)
#define SPEED_TEXT_COLOR                        (0xB25962)
#define SPEED_CENTER_CIRCLE_GRAD_TOP_COLOR      (0xEB7581)
#define SPEED_CENTER_CIRCLE_GRAD_BOTTOM_COLOR   (0xF0868A)
#define SPEED_SIDE_CIRCLE_GRAD_TOP_COLOR        (0xF7A1A8)
#define SPEED_SIDE_CIRCLE_GRAD_BOTTOM_COLOR     (0xF7A1A8)
#define SPEED_TITLE_TEXT_COLOR                  (0xFFFFFF)
#define SPEED_DESC_TEXT_COLOR                   (0xFFDDDF)
#define SPEED_CENTER_DESC_TEXT_COLOR            (0xFFEAEA)

#define SPEED_PAGE_TITLE_Y_OFFSET               (34)
#define SPEED_COMPONENT_CENTER_Y_OFFSET         (-70)
#define SPEED_TITLE_BASE_Y_OFFSET               (-26)
#define SPEED_DESC_BASE_Y_OFFSET                (6)
#define SPEED_SWITCH_ANIM_TIME_MS               (260)  /// 切换动画时间，单位：毫秒

typedef enum {
    SPEED_SLOT_LEFT = 0,
    SPEED_SLOT_CENTER,
    SPEED_SLOT_RIGHT,
    SPEED_SLOT_MAX,
} speed_circle_slot_e;

typedef struct {
    uint16_t slot_res_id[SPEED_CIRCLE_SLOT_COUNT];
    int16_t slot_speed_idx[SPEED_CIRCLE_SLOT_COUNT];
    uint8_t cur_speed;
    uint8_t force_update;
    uint8_t speed_obj_init_done;
    uint8_t popup_ui_index;
    uint8_t rsv[8];
} speed_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static speed_display_info_t *speed_info;

static const uint16_t speed_component_ids[SPEED_COUNT] = {
    // RESOURCE_ID_FREQ_PAGE_COMFORT_ICON, /// 舒适
    // RESOURCE_ID_FREQ_PAGE_REGULAR_ICON, /// 常规
    // RESOURCE_ID_FREQ_PAGE_BOOST_ICON, /// 快速
};

static const uint16_t dots_img_ids[SPEED_COUNT] = {
    // RESOURCE_ID_SELECTED_LOCATE_3_START,
    // RESOURCE_ID_SELECTED_LOCATE_3_START + 1,
    // RESOURCE_ID_SELECTED_LOCATE_3_END,
};

static const char *speed_name[SPEED_COUNT] = {
    "Slow", /// 慢速
    "Regular", /// 常规
    "Fast", /// 快速
};

static const char *speed_desc[SPEED_COUNT] = {
    "Efficient yet gentle.",
    "Suitable for most moms.",
    "Stronger yet safe for faster\npumping.",
};

#if IS_SUPPORT_SPEED_COMP_BG
static PSRAM_DATA_LOC uint8_t speed_bg_data[] = {
    #include "comparison_png2txt/Regular_360x360.txt"
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

static void update_slot_content(ui_manager_context_t *thiz, speed_circle_slot_e slot, int16_t speed_idx)
{

}

static void circle_layout_apply(int8_t direction, bool anim)
{

}

static void circle_carousel_set_speed(ui_manager_context_t *thiz, uint8_t idx, bool anim)
{

}

static void speed_root_init(ui_manager_context_t *thiz)
{

}

static void speed_bg_update(ui_manager_context_t *thiz)
{

}


static void speed_ui_update(ui_manager_context_t *thiz, uint8_t speed_idx)
{
    if (speed_idx > BREAST_PUMP_SPEED_MAX) {
        speed_idx = BREAST_PUMP_SPEED_LOW;
    }

}

static void speed_text_update(ui_manager_context_t *thiz)
{
    if (speed_info->force_update == false) {
        return;
    }

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void common_pump_speed_ui_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (speed_info == NULL) {
    //     speed_info = (speed_display_info_t *)u_malloz(sizeof(speed_display_info_t));
    //     EXCEPTION_CHECK_VOID(speed_info);
    // }
    // speed_root_init(thiz);
    // common_ui_bg_init(thiz);
    // speed_info->force_update = true;
    // speed_info->speed_obj_init_done = false;
    // speed_info->popup_ui_index = ID_POPUP_UI_NONE;
}

void common_pump_speed_ui_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz && speed_info);
    // for (uint8_t i = 0; i < SPEED_CIRCLE_SLOT_COUNT; i++) {
    //     common_ui_img_resource_deinit(thiz, &speed_info->speed_component_info[i]);
    // }
    // common_ui_img_resource_deinit(thiz, &speed_info->dots_img_info);
    // common_ui_bg_deinit(thiz);
    // ui_common_operation_tip_remove(thiz);
    // LVGL_OBJ_DELETE(speed_info->root);
    // u_free(speed_info);
    // speed_info = NULL;
}

int common_pump_speed_ui_process(ui_manager_context_t *thiz, uint8_t speed, uint16_t popup_ui_type)
{
    // EXCEPTION_CHECK_UINT(thiz && speed_info && speed_info->root);
    // speed_bg_update(thiz);
    // speed_ui_update(thiz, speed);
    // speed_text_update(thiz);
    // ui_common_operation_tip_update(thiz, popup_ui_type, speed_info->root, &speed_info->popup_ui_index);
    // speed_info->force_update = false;
    
    return 0;
}