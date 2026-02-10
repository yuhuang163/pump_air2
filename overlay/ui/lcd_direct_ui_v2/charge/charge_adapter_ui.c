/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "resource_id_define.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h"   // IWYU pragma: keep
#include "window_screen_charge.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define CHARGE_ADAPTER_TEXT_COLOR                    0x3C3D40

typedef struct {  
    lv_obj_t * root;
    lv_obj_t * charge_adapter_icon;
    lv_obj_t * text;
    res_img_info_t charge_adapter_icon_info;
    uint8_t force_update;
    uint8_t animation_index;
} charge_adapter_display_info_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static charge_adapter_display_info_t *adapter_info;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void charge_adapter_ui_root_init(ui_manager_context_t *thiz)
{

}

static int charge_adapter_object_update(ui_manager_context_t *thiz)
{

    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_charge_adapter_screen_prepare(ui_manager_context_t *thiz)
{


}

void ui_display_charge_adapter_screen_remove(ui_manager_context_t *thiz)
{

}

int ui_display_charge_adapter_process(ui_manager_context_t *thiz)
{
    return 0;
}