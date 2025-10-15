/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "framework_hook.h" // IWYU pragma: keep

/*
 * DEFINES
 *****************************************************************************************
 */
#define POPUP_WM_TIME_BY_APP                ( 1500 )  // APP 点击 弹窗时间

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

// hook点入口.
void framework_extra_hook(void)
{
    motor_manager_context_t *mt_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    CHECK_IF_VALID(mt_ctx, {
        register_motor_hook(mt_ctx);
    });

}
