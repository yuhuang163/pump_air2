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

#define PUMP_TITLE_TEXT_COLOR                                 (0x533129)
#define PUMP_LEVEL_TEXT_COLOR                                 (0x533129)
#define PUMP_LEVEL_DIR_TEXT_COLOR                             (0xB35C65)
#define PUMP_LEVEL_MAX                                        (15)
#define PUMP_LEVEL_TITLE_TEXT_Y                               (92)
#define PUMP_LEVEL_ANIMATION_TIME                             (100)

typedef struct {
    uint8_t force_update;
    uint8_t popup_ui_index;
    uint8_t direction;
    uint8_t animation_status;
    uint32_t animation_start_time;
    uint8_t display_direction;
    uint8_t level_base;
    int8_t level_change;
    uint8_t rsv[12];
} level_display_info_t;

typedef enum {
    LEVEL_ANIMATION_TYPE_NONE = 0,
    LEVEL_ANIMATION_TYPE_ZOOM_OUT,
    LEVEL_ANIMATION_TYPE_ZOOM_IN,
} level_animation_type_e;


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
#if IS_SUPPORT_PUMP_LEVEL_COMP_BG
static PSRAM_DATA_LOC uint8_t setting_bg_data[] = {
    #include "comparison_png2txt/right_level_adjust_360x360.txt"
};
#endif

static level_display_info_t *level_info;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void pump_level_root_init(void)
{

}

static void pump_level_root_bg_update(ui_manager_context_t *thiz)
{

}

static void pump_level_title_text_update(ui_manager_context_t *thiz)
{    

}

static void pump_level_scale_anim_cb(void *obj, int32_t v)
{

}

static void pump_level_direction_update(ui_manager_context_t *thiz, bool is_left)
{    

}

static int pump_level_progress_update(ui_manager_context_t *thiz, uint8_t level)
{   

    return 0;
}

static int pump_level_text_update(ui_manager_context_t *thiz, uint8_t level)
{

    return 0;
}

static void pump_level_update(ui_manager_context_t *thiz, uint8_t level, int8_t level_change)
{

}

static void pump_level_range_text_update(ui_manager_context_t *thiz)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void common_pump_level_ui_prepare(ui_manager_context_t *thiz)
{

}

void common_pump_level_ui_remove(ui_manager_context_t *thiz)
{

}

int common_pump_level_ui_process(ui_manager_context_t *thiz, uint8_t level, 
    int8_t level_change, bool is_left, uint8_t popup_ui_type)
{
    // EXCEPTION_CHECK_INT(thiz && level_info && level_info->root);
    // pump_level_root_bg_update(thiz);
    // pump_level_title_text_update(thiz);
    // pump_level_direction_update(thiz, is_left);
    // pump_level_update(thiz, level, level_change);
    // pump_level_range_text_update(thiz);
    // ui_common_operation_tip_update(thiz, popup_ui_type, level_info->root, &level_info->popup_ui_index);
    // level_info->force_update = false;
        
    return 0;
}