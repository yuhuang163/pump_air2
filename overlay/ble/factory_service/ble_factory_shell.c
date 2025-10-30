/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ble_manager.h" // IWYU pragma: keep
#include "ble_factory_message.h" // IWYU pragma: keep
#include <stdint.h>
#include "ble_factory_shell.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define BLE_PUMP_POLICY_SHELL_CMD              "#pump"
#define BLE_PUMP_CTRL_SHELL_CMD                "#pCtrl"
#define BLE_VALVE_CTRL_SHELL_CMD               "#vCtrl"
#define BLE_SYSTEM_RESET_SHELL_CMD             "#sysRst"
#define BLE_FORBID_SLEEP_SHELL_CMD             "#forbidSlp"

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static void ble_shell_pump_policy(const uint8_t *param, uint16_t len);
static void ble_shell_pump_ctrl(const uint8_t *param, uint16_t len);
static void ble_shell_valve_ctrl(const uint8_t *param, uint16_t len);
static void ble_shell_system_reset(const uint8_t *param, uint16_t len);
static void ble_shell_forbid_sleep(const uint8_t *param, uint16_t len);
static const ble_factory_shell_t shell_list[] = {
    {BLE_PUMP_POLICY_SHELL_CMD,   ble_shell_pump_policy},
    {BLE_PUMP_CTRL_SHELL_CMD,     ble_shell_pump_ctrl},
    {BLE_VALVE_CTRL_SHELL_CMD,    ble_shell_valve_ctrl},
    {BLE_SYSTEM_RESET_SHELL_CMD,  ble_shell_system_reset},
    {BLE_FORBID_SLEEP_SHELL_CMD,  ble_shell_forbid_sleep},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ble_shell_pump_ctrl(const uint8_t *param, uint16_t len)
{
    motor_manager_context_t *motor = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    motor_debug_para_t para = {0};
    int duty = 0, hold_time = 0;
    sscanf((const char *)param, "%d %d", &duty, &hold_time);
    LOG_BLE("shell pump ctrl duty %d, hold_time %d", duty, hold_time);
    para.duty = duty;
    para.hold_time = hold_time;
    motor->hook->motor_debug_with_user_para(motor, para.duty == 0 ? MOTOR_USER_DEBUG_PUMP_ON : MOTOR_USER_DEBUG_PUMP_OFF, &para);
}

static void ble_shell_valve_ctrl(const uint8_t *param, uint16_t len)
{
    motor_manager_context_t *motor = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    motor_debug_para_t para = {0};
    int duty = 0, hold_time = 0;
    sscanf((const char *)param, "%d %d", &duty, &hold_time);
    LOG_BLE("shell valve ctrl duty %d, hold_time %d", duty, hold_time);
    para.duty = duty;
    para.hold_time = hold_time;
    motor->hook->motor_debug_with_user_para(motor, para.duty == 0 ? MOTOR_USER_DEBUG_VALVE_ON : MOTOR_USER_DEBUG_VALVE_OFF, &para);
}

static void ble_shell_pump_policy(const uint8_t *param, uint16_t len)
{
    motor_manager_context_t *motor = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    motor_policy_original_para_t para = {0};
    int pump_level = 0, pumping_time = 0, valve_on_time = 0, sleep_time = 0, hold_time = 0;
    sscanf((const char *)param, "%d %d %d %d %d", \
        &pump_level, &pumping_time, &valve_on_time, &sleep_time, &hold_time);
    LOG_BLE("shell pump policy pump_level %d, pumping_time %d, valve_on_time %d, sleep_time %d, hold_time %d", \
        pump_level, pumping_time, valve_on_time, sleep_time, hold_time);
    para.pump_level = pump_level;
    para.pumping_time = pumping_time;
    para.valve_on_time = valve_on_time;
    para.sleep_time = sleep_time;
    para.hold_time = hold_time;
    motor->hook->motor_debug_with_user_para(motor, MOTOR_USER_DEBUG_POLICY, &para);
}

static void ble_shell_system_reset(const uint8_t *param, uint16_t len)
{
    storage_manager_context_t *st_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    int reset_type = 0;
    sscanf((const char *)param, "%d", &reset_type);
    LOG_BLE("shell system reset type %d", reset_type);
    if (st_ctx->factory_reset) {
        st_ctx->factory_reset(reset_type == 0 ? RESET_NOT_REBOOT : RESET_AND_REBOOT);
    }
}

static void ble_shell_forbid_sleep(const uint8_t *param, uint16_t len)
{
    int forbid_sleep = 0;
    sscanf((const char *)param, "%d", &forbid_sleep);
    LOG_BLE("shell forbid sleep %d", forbid_sleep);
}

static int ble_factory_shell_cmd_handle(const uint8_t *param, uint16_t len)
{
    uint16_t str_len = 0;
    for (uint8_t i = 0; i < LENGTH_OF_ARRAY(shell_list); i++) {
        str_len = strlen(shell_list[i].cmd_str);
        if (str_len > len) {
            continue;
        }

        if (memcmp(param, shell_list[i].cmd_str, strlen(shell_list[i].cmd_str)) == 0) {
            shell_list[i].cmd_fun(param + str_len, len - str_len);
            return 0;
        }
    }
    return -1;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ble_factory_shell_handle(uint8_t *p_data, uint16_t len)
{
    if (p_data[0] != '#') {
        return;
    }

    power_manager_context_t *pm = (power_manager_context_t *)find_module(ID_MODULE_POWER);
    if (pm->is_pm_suspend()) {
        pm->pm_wakeup(PM_WAKEUP_REASON_BLE_REQ);
    }

    if (ble_factory_shell_cmd_handle(p_data, len) != 0) {
        LOGE("no ble factory shell cmd handle");
    }
}
