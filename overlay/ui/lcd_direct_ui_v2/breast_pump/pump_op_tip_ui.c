/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h"   // IWYU pragma: keep
#include "ui_page_define.h" // IWYU pragma: keep
#include "ui_manager.h" // IWYU pragma: keep
#include "window_screen_pump.h" // IWYU pragma: keep
#include "breast_pump_ui.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

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
uint16_t pump_tip_popup_type_convert(uint16_t popup_ui_type)
{
    switch (popup_ui_type) {
        default:
        case PUMP_POPUP_UI_NONE:
            return COMMON_OP_TIP_NONE;
        case PUMP_POPUP_UI_OPERATION_SUCCESS:
            return COMMON_OP_TIP_SUCCESS;
        case PUMP_POPUP_UI_OPERATION_FAILED:
            return COMMON_OP_TIP_FAILED;
        case PUMP_POPUP_UI_LEVEL_ADJUST_TOO_LARGE:
            return COMMON_OP_TIP_LEVEL_ADJUST_WARNING;
    }
}
