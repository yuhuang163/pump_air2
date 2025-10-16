/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "framework_hook.h" // IWYU pragma: keep
#include "root_common_api.h"

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
    motor_manager_context_t *motor_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    CHECK_IF_VALID(motor_ctx, {
        register_motor_hook(motor_ctx);
    });

    sensor_manager_context_t *sensor_ctx = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    CHECK_IF_VALID(sensor_ctx, {
        register_sensor_hook(sensor_ctx);
    });

}
