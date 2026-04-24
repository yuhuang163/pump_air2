/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static uint8_t s_force_update = 0;

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
void common_ui_bg_init(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    s_force_update = true;
}

void common_ui_bg_deinit(ui_manager_context_t *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // backgroud = NULL;
    s_force_update = false;
}


