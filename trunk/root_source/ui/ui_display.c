/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common.h"
#include "root_common_api.h" // IWYU pragma: keep

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
static void ui_animation_play_status_update(ui_manager_context_t *thiz, bool flag)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.is_playing_ui_animation = flag;
}

static bool is_ui_backlight_on(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_BOOL(thiz);
    return thiz->priv.ui_backlight_val > 0 ? true : false;
}

static void set_ui_backlight(ui_manager_context_t *thiz, uint16_t val)
{
    EXCEPTION_CHECK_VOID(thiz);
    DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_SET_BL, &val);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_ui_display(ui_manager_context_t *thiz)
{
    thiz->ui_animation_play_status_update = ui_animation_play_status_update;
    thiz->is_ui_backlight_on = is_ui_backlight_on;
    thiz->set_ui_backlight = set_ui_backlight;
}