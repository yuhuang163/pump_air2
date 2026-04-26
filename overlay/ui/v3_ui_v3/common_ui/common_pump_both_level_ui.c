/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_PUMP_LEVEL_COMP_BG                         (0)

#define PUMP_ARC_COLOR                                        (0xBF646E)
#define PUMP_LEVEL_CIRCLE_COLOR                               (0xFFFFFF) /// (0xFFEDEF)
#define PUMP_TITLE_TEXT_COLOR                                 (0xFFFFFF)
#define PUMP_LEVEL_TEXT_COLOR                                 (0xC06670)
#define PUMP_LEVEL_OFFSET_TEXT_COLOR                          (0x533129)
#define PUMP_LEVEL_RING_SIZE                                  (314)
#define PUMP_LEVEL_RING_R                                     (PUMP_LEVEL_RING_SIZE / 2)
#define PUMP_LEFT_ARC_START_ANGLE                             (120)
#define PUMP_LEFT_ARC_END_ANGLE                               (241 - 10)
#define PUMP_RIGHT_ARC_START_ANGLE                            (299 + 10)
#define PUMP_RIGHT_ARC_END_ANGLE                              (420)
#define PUMP_LEVEL_ARC_WIDTH                                  (20)
#define PUMP_LEVEL_CIRCLE_SIZE                                (40)
#define PUMP_LEVEL_MAX                                        (15)

#define PUMP_LEVEL_LEFT_CIRCLE_X                              (80 - 23)
#define PUMP_LEVEL_LEFT_CIRCLE_Y                              (13)
#define PUMP_LEVEL_RIGHT_CIRCLE_X                             (240 - 23)
#define PUMP_LEVEL_RIGHT_CIRCLE_Y                             PUMP_LEVEL_LEFT_CIRCLE_Y

#define PUMP_LEVEL_TITLE_TEXT_X                               (156)
#define PUMP_LEVEL_TITLE_TEXT_Y                               (28 + 4)

#define PUMP_LEVEL_SYMBOL_X                                   (99)
#define PUMP_LEVEL_SYMBOL_Y                                   (167)

/// 符号状态
typedef enum {
    PUMP_BOTH_LEVEL_SYMBOL_NONE = 0,  
    PUMP_BOTH_LEVEL_SYMBOL_ADD,   /// 加
    PUMP_BOTH_LEVEL_SYMBOL_SUB,   /// 减
} pump_both_level_symbol_status_e;

typedef struct {
    uint8_t force_update;
    uint8_t popup_ui_index;
    int8_t left_level_change;
    int8_t right_level_change;
    int8_t level_change;
    uint8_t symbol_status; /// 符号状态
    uint8_t rsv[10];
} level_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PUMP_LEVEL_COMP_BG
static PSRAM_DATA_LOC uint8_t setting_bg_data[] = {
    #include "comparison_png2txt/LR_level_adjust2_360x360.txt"
};
#endif

static level_display_info_t *level_info;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void pump_both_level_root_init(void)
{

}

static void pump_both_level_root_bg_update(ui_manager_context_t *thiz)
{
}

static void pump_both_level_text_update(ui_manager_context_t *thiz)
{    

}

static void pump_both_level_symbol_update(ui_manager_context_t *thiz, int8_t level_change)
{   

}

static int pump_left_level_circle_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static int pump_left_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{

    return 0;
}

static int pump_left_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static void pump_left_level_update(ui_manager_context_t *thiz, uint8_t level, int8_t level_change)
{

}

static int pump_right_level_circle_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static int pump_right_level_text_update(ui_manager_context_t *thiz, uint8_t level, uint16_t end_angle)
{

    return 0;
}

static int pump_right_level_arc_update(ui_manager_context_t *thiz, uint16_t end_angle)
{

    return 0;
}

static void pump_right_level_update(ui_manager_context_t *thiz, uint8_t level, int8_t level_change)
{

}

static void pump_both_level_offset_text_update(ui_manager_context_t *thiz, int8_t level_change)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void common_pump_both_level_ui_prepare(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // if (level_info == NULL) {
    //     level_info = (level_display_info_t *)u_malloz(sizeof(level_display_info_t));
    //     EXCEPTION_CHECK_VOID(level_info);
    // }
    // pump_both_level_root_init();
    // level_info->force_update = true;
    // level_info->popup_ui_index = ID_POPUP_UI_NONE;
}

void common_pump_both_level_ui_remove(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // EXCEPTION_CHECK_VOID(level_info);
    // common_ui_img_resource_deinit(thiz, &level_info->symbol_info);
    // ui_common_operation_tip_remove(thiz);
    // LVGL_OBJ_DELETE(level_info->root);
    // u_free(level_info);
    // level_info = NULL;
}

int common_pump_both_level_ui_process(ui_manager_context_t *thiz, uint8_t *level, 
    int8_t level_change, uint16_t popup_ui_type)
{
    // EXCEPTION_CHECK_INT(thiz && level_info && level_info->root && level);
    // pump_both_level_root_bg_update(thiz);
    // pump_both_level_text_update(thiz);
    // pump_both_level_symbol_update(thiz, level_change);
    // pump_both_level_offset_text_update(thiz, level_change);
    // pump_left_level_update(thiz, level[0], level_change);
    // pump_right_level_update(thiz, level[1], level_change);
    // ui_common_operation_tip_update(thiz, popup_ui_type, level_info->root, &level_info->popup_ui_index);
    // level_info->force_update = false;

    return 0;
}