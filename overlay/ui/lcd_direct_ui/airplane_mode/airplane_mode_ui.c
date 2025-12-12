/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static lv_obj_t *airplane_mode_root;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/


/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ui_display_airplane_mode_prepare(ui_manager_context_t *thiz)
{
    LOG_UI("ui_display_airplane_mode_prepare");
}

void ui_display_airplane_mode_remove(ui_manager_context_t *thiz)
{
    LVGL_OBJ_DELETE(airplane_mode_root);
}

int ui_display_airplane_mode_process(ui_manager_context_t *thiz)
{
    return 0;
}