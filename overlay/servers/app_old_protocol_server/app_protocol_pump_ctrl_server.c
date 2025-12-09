#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_handler.h"
#include "protocol_manager_api.h"
#include "storage_manager_api.h"
#include "app_protocol_pump_ctrl_server.h"
#include "breast_pump_manager_api.h"
#include "pump_mode_default_config.h"
#include "breast_pump_mode.h"
#include "pump_mode_default_config.h"
#include <stdint.h>
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

uint8_t app_old_comm_get_sys_status(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_UINT(pbus);
    struct rt_device_node *battery = pbus->find_device(ID_DEVICE_BATTERY);
    EXCEPTION_CHECK_UINT(battery);
    uint8_t chage_state = DEVICE_IOCTL(battery, OP_CODE_BAT_GET_CHARGE_STATUS);

    if (BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_CONTINUE == pump_ctx->get_breast_pump_status()) {
        LOG_SRV("APP_OLD_COMM_SYS_RUN");
        return APP_OLD_COMM_SYS_RUN;
    }

    if (BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_FINISH == pump_ctx->get_breast_pump_status()
        || BREAST_PUMP_START == pump_ctx->get_breast_pump_status()) {
        LOG_SRV("APP_OLD_COMM_SYS_STOP");
        return APP_OLD_COMM_SYS_STOP;
    }

    if (chage_state == CHARGE_STATE_CHARGING || chage_state == CHARGE_STATE_FULL) {
        LOG_SRV("APP_OLD_COMM_SYS_CHANRGE");
        return APP_OLD_COMM_SYS_CHANRGE;
    }
    if ((BREAST_PUMP_START == pump_ctx->get_breast_pump_status()
         || BREAST_PUMP_CONTINUE == pump_ctx->get_breast_pump_status())
        && BREAST_PUMP_CUSTOM_MODE == pump_ctx->get_breast_pump_mode()) {
        LOG_SRV("APP_OLD_COMM_PUMP_USE_DEFINE_RUN");
        return APP_OLD_COMM_PUMP_USE_DEFINE_RUN;
    }
}

uint8_t app_old_comm_get_pump_running_status(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    LOG_SRV("pump_ctx->is_on_working =%d", pump_ctx->is_on_working());
    return pump_ctx->is_on_working();
}

uint8_t app_old_comm_get_battery_percent(void)
{
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_UINT(pbus);
    struct rt_device_node *battery = pbus->find_device(ID_DEVICE_BATTERY);
    uint8_t percent = 0;
    EXCEPTION_CHECK_UINT(battery);
    DEVICE_READ_ARGS(battery, &percent, sizeof(percent));
    LOG_SRV("percent=%d", percent);
    return percent;
}

bool app_old_comm_check_if_low_battery(void)
{
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_UINT(pbus);
    struct rt_device_node *battery = pbus->find_device(ID_DEVICE_BATTERY);
    EXCEPTION_CHECK_UINT(battery);
    int percent = 0;
    DEVICE_READ_ARGS(battery, &percent, sizeof(percent));
    LOG_SRV("percent=%d", percent);
    if (percent <= LOW_BAT_PERCENTAGE) {
        return 1;
    }
    return 0;
}

uint8_t app_old_comm_get_current_pump_mode(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    int pump_mode = pump_ctx->get_breast_pump_mode();
    LOG_SRV("pump mode: %s", pump_ctx->get_breast_pump_mode_name(pump_mode));
    return (uint8_t)pump_mode;
}

uint8_t app_old_comm_get_current_custom_pump_step_mode(servers_old_protocol_publish_data_id_e id)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    //当前不是运行的不是自定义模式步骤，默认上报第一步参数,启动自定义模式从第一步开始
    if(pump_ctx->get_breast_pump_mode() != BREAST_PUMP_CUSTOM_MODE) {
        time_index = 0;
    }
    //获取自定义模式参数
    int pump_mode;
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    user_def_pump_steps_data_t custom_steps_parameter ={0};
    storage_ctx->get_store_ram_val(STORE_ID_PUMP_CUSTOM_STEPS_PARAMETER_INFO, 0, 0, &custom_steps_parameter);
    pump_mode = custom_steps_parameter.data[time_index].mode;
    LOG_SRV("custom_steps_parameter.data[%d].mode=%d", time_index, pump_mode);
    return (uint8_t)pump_mode;
}

uint8_t app_old_comm_get_current_mode_pump_level(app_old_commd_l_r_ctrl_e lr_ct, servers_old_protocol_publish_data_id_e id)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    int time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    ///如果是模式变化触发上报，则上报第一步参数，否则上报当前步骤参数
    if (id == OLD_PUBLISH_DATA_PUMP_MODE_CHANGE) {
        time_index = 0;
    }
    ///当前系统在处于自定义模式步骤，应该上报什么，待定
    /*
    if (pump_ctx->get_breast_pump_mode() == BREAST_PUMP_CUSTOM_MODE) {

    } else {*/
        //
        pump_setting_t setting;
        pump_ctx->get_breast_pump_setting(&setting, time_index);
        LOG_SRV("setting.level [%d] =%d", lr_ct, setting.level[lr_ct]);
        return (uint8_t)setting.level[lr_ct];
 /*}*/
}

uint8_t app_old_comm_get_current_custom_pump_level(app_old_commd_l_r_ctrl_e lr_ct, servers_old_protocol_publish_data_id_e id)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    int time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    ///如果是模式改变触发上报，则上报第一步参数，否则上报当前步骤参数
    if (id == OLD_PUBLISH_DATA_PUMP_MODE_CHANGE) {
        time_index = 0;
    }

    pump_setting_t setting;
    //当前系统在处于自定义模式步骤
    if (pump_ctx->get_breast_pump_mode() == BREAST_PUMP_CUSTOM_MODE) {
        pump_ctx->get_breast_pump_setting(&setting, time_index);
        LOG_SRV("setting.level [%d] =%d", lr_ct, setting.level[lr_ct]);
        return (uint8_t)setting.level[lr_ct];
    } else { //默认上报flash中自定义模式第一步参数,启动自定义模式从第一步开始
        storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
        EXCEPTION_CHECK_UINT(storage_ctx);
        user_def_pump_steps_data_t custom_steps_parameter = {0};
        storage_ctx->get_store_ram_val(STORE_ID_PUMP_SETTING_INFO, BREAST_PUMP_CUSTOM_MODE, 0, &setting);
        LOG_SRV("setting.level [%d] =%d", lr_ct, setting.level[lr_ct]);
        return (uint8_t)setting.level[lr_ct];
    }
}

//左右需要一致，不区分
uint8_t app_old_comm_get_current_mode_pump_speed(servers_old_protocol_publish_data_id_e id)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    int time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    ///如果是模式改变触发上报，则上报第一步参数，否则上报当前步骤参数
    if (id == OLD_PUBLISH_DATA_PUMP_MODE_CHANGE) {
        time_index = 0;
    }
    ///当前系统在处于自定义模式步骤，应该上报什么，待定
    /*
    if (pump_ctx->get_breast_pump_mode() == BREAST_PUMP_CUSTOM_MODE) {

    } else {*/
        pump_setting_t setting;
        pump_ctx->get_breast_pump_setting(&setting, time_index);
        LOG_SRV("setting.speed =%d", setting.speed);
        return (uint8_t)setting.speed;
    /*}*/
}

uint8_t app_old_comm_get_current_custom_pump_speed(servers_old_protocol_publish_data_id_e id)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    ///如果是模式改变触发上报，则上报第一步参数，否则上报当前步骤参数
    if (id == OLD_PUBLISH_DATA_PUMP_MODE_CHANGE) {
        time_index = 0;
    }
    //当前不是运行的不是自定义模式步骤，默认上报第一步参数,启动自定义模式从第一步开始
    if (pump_ctx->get_breast_pump_mode() != BREAST_PUMP_CUSTOM_MODE) {
        time_index = 0;
    }
    //不允许修改自定义模式频率，只能修改等级，所以从app设置缓存中获取
    int pump_speed_level;
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    user_def_pump_steps_data_t custom_steps_parameter = {0};
    storage_ctx->get_store_ram_val(STORE_ID_PUMP_CUSTOM_STEPS_PARAMETER_INFO, 0, 0, &custom_steps_parameter);
    pump_speed_level = custom_steps_parameter.data[time_index].level;
    LOG_SRV("custom_steps_parameter.data[%d].pump_speed_level=%d", time_index, pump_speed_level);
    return (uint8_t)pump_speed_level;
}

uint32_t app_old_comm_get_current_pump_mode_run_time(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    pump_result_info_t *pumping_info = pump_ctx->get_breast_pump_result(BREAST_PUMP_RESULT_PUMPING);
    LOG_SRV("pumping_info->current_mode_work_time = %d", pumping_info->current_mode_work_time);
    return (uint32_t)pumping_info->current_mode_work_time/1000;
}

uint32_t app_old_comm_get_pump_mode_run_total_time(void)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    pump_result_info_t *pumping_info = pump_ctx->get_breast_pump_result(BREAST_PUMP_RESULT_PUMPING);
    LOG_SRV("pumping_info->work_total_time = %d", pumping_info->work_total_time);
    return (uint32_t)pumping_info->work_total_time/1000;
}

uint32_t app_old_comm_get_current_custom_pump_mode_total_time(void)
{
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    user_def_pump_steps_data_t custom_steps_parameter = {0};
    storage_ctx->get_store_ram_val(STORE_ID_PUMP_CUSTOM_STEPS_PARAMETER_INFO, 0, 0, &custom_steps_parameter);
    uint32_t custom_total_time = custom_steps_parameter.custom_total_time;
    LOG_SRV("custom_steps_parameter.custom_total_time= %d", custom_total_time);
    return (uint32_t)custom_total_time;
}

uint8_t app_old_comm_get_system_is_airplane_mode(void)
{
    airplane_mode_context_t *ap = (airplane_mode_context_t *)find_module(ID_MODULE_AIRPLANE_MODE);
    EXCEPTION_CHECK_UINT(ap);
    int air_play_mode = ap->is_on_airplane_mode();
    return (uint8_t)air_play_mode;
}

uint8_t app_old_comm_set_custom_pump_step_parameter_infor(user_def_pump_steps_data_t *pump_steps_data)
{
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    storage_ctx->put_store_ram_val(STORE_ID_PUMP_CUSTOM_STEPS_PARAMETER_INFO, 0, 0, pump_steps_data);
    LOG_SRV("put custom_steps_parameter to ram");
    return true;
}

uint8_t app_old_comm_set_nommal_pump_level(app_old_comm_pump_ctrl *app_old_comm_pump_ctrl_date)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }

    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);

    pump_setting_t setting; /// 当前泵奶设置信息.

    pump_ctx->get_breast_pump_setting(&setting, time_index);

    if (app_old_comm_pump_ctrl_date->pump_mode_lr_or_all == APP_OLD_COMM_LEFT_CT+1) {
        if (app_old_comm_pump_ctrl_date->normal_pump_level >= 0) {
            setting.level[0] = app_old_comm_pump_ctrl_date->normal_pump_level;
            LOG_SRV("APP_OLD_COMM_LEFT_CT pump level %d",setting.level[0]);
        }
    } else if (app_old_comm_pump_ctrl_date->pump_mode_lr_or_all == APP_OLD_COMM_RIGHT_CT+1) {
        if (app_old_comm_pump_ctrl_date->normal_pump_level <= BREAST_PUMP_LEVEL_15) {
            setting.level[1] = app_old_comm_pump_ctrl_date->normal_pump_level;
            LOG_SRV("APP_OLD_COMM_RIGHT_CT pump level %d",setting.level[1]);
        }
    } else {
        if (app_old_comm_pump_ctrl_date->normal_pump_level >= 0
            && app_old_comm_pump_ctrl_date->normal_pump_level <= BREAST_PUMP_LEVEL_15) {
            setting.level[1] = app_old_comm_pump_ctrl_date->normal_pump_level;
            setting.level[0] = app_old_comm_pump_ctrl_date->normal_pump_level;
            LOG_SRV("APP_OLD_COMM_ALL_CT pump level %d",setting.level[0]);
            LOG_SRV("APP_OLD_COMM_ALL_CT pump level %d",setting.level[1]);
        }
    }

    pump_ctx->set_breast_pump_setting(&setting, time_index);
    storage_ctx->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, app_old_comm_get_current_pump_mode(), time_index, &setting);
    return true;
}

uint8_t app_old_comm_set_nommal_pump_speed(app_old_comm_pump_ctrl *app_old_comm_pump_ctrl_date)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);

    pump_setting_t setting; /// 当前泵奶设置信息.

    pump_ctx->get_breast_pump_setting(&setting, time_index);
    //只允许常规模式设置 speed，并且左右同步控制
    setting.speed = app_old_comm_pump_ctrl_date->normal_pump_speed;
    LOG_SRV("pump speed  %d", setting.speed);
    pump_ctx->set_breast_pump_setting(&setting, time_index);
    storage_ctx->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, app_old_comm_get_current_pump_mode(), time_index,
                                   &setting);
    return true;
}


uint8_t app_old_comm_set_custom_pump_level(app_old_comm_pump_ctrl *app_old_comm_pump_ctrl_date)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);

    pump_setting_t setting; /// 当前泵奶设置信息.

    pump_ctx->get_breast_pump_setting(&setting, time_index);

    if (app_old_comm_pump_ctrl_date->pump_mode_lr_or_all == APP_OLD_COMM_LEFT_CT+1) {
        if (app_old_comm_pump_ctrl_date->user_define_pump_level >= 0) {
            setting.level[0] = app_old_comm_pump_ctrl_date->user_define_pump_level;
            LOG_SRV("APP_OLD_COMM_LEFT_CT pump level %d",setting.level[0]);
        }
    } else if (app_old_comm_pump_ctrl_date->pump_mode_lr_or_all == APP_OLD_COMM_RIGHT_CT+1) {
        if (app_old_comm_pump_ctrl_date->user_define_pump_level <= BREAST_PUMP_LEVEL_15) {
            setting.level[1] = app_old_comm_pump_ctrl_date->user_define_pump_level;
            LOG_SRV("APP_OLD_COMM_RIGHT_CT pump level %d",setting.level[1]);
        }
    } else {
        if (app_old_comm_pump_ctrl_date->user_define_pump_level >= 0
            && app_old_comm_pump_ctrl_date->user_define_pump_level <= BREAST_PUMP_LEVEL_15) {
            setting.level[1] = app_old_comm_pump_ctrl_date->user_define_pump_level;
            setting.level[0] = app_old_comm_pump_ctrl_date->user_define_pump_level;
            LOG_SRV("APP_OLD_COMM_ALL_CT pump level %d",setting.level[0]);
            LOG_SRV("APP_OLD_COMM_ALL_CT pump level %d",setting.level[1]);
        }
    }

    pump_ctx->set_breast_pump_setting(&setting, time_index);
    storage_ctx->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, app_old_comm_get_current_pump_mode(), time_index, &setting);
    return true;
}

uint8_t app_old_comm_pump_control_handler(app_old_comm_pump_ctrl *app_old_comm_pump_ctrl_date)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_UINT(motor_ctx);
    uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
    if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
        time_index = 0;
    }
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    //appbug，app不会同时设置模式和参数
    //如果模式不变，说明只设置当前模式参数,
    if (app_old_comm_pump_ctrl_date->pump_mode == app_old_comm_get_current_pump_mode()) {
        switch (app_old_comm_get_current_pump_mode()) {
            case BREAST_PUMP_PUMP_MODE:
                /// 常规的吸乳模式允许调频率
                LOG_SRV("app set normal pump speed");
                app_old_comm_set_nommal_pump_speed(app_old_comm_pump_ctrl_date);
            case BREAST_PUMP_STIMULATION_MODE: // 传统模式
            case BREAST_PUMP_MIX_MODE:
            case BREAST_PUMP_PP_MILK_TRACK_MODE:
            case BREAST_PUMP_MILK_BOOST_MODE:
            case BREAST_PUMP_LACTATION_START_MODE: {
                app_old_comm_set_nommal_pump_level(app_old_comm_pump_ctrl_date);
                break;
            }
            case BREAST_PUMP_CUSTOM_MODE: {
                app_old_comm_set_custom_pump_level(app_old_comm_pump_ctrl_date); //设置运行中的参数
                //todo                                                          //将缓存的cust para也更新下
                break;
            }
            default:
                break;
        }
    } else //如果模式改变说明主要用于改变模式
    {
        pump_ctx->set_breast_pump_mode(app_old_comm_pump_ctrl_date->pump_mode);
        //prd 模式不记忆
        LOG_SRV("app change pump_mode %d",app_old_comm_pump_ctrl_date->pump_mode);
    }
    return true;
}

uint8_t app_old_comm_pump_run_status_ctrl(uint8_t status)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_UINT(pump_ctx);
    LOG_SRV("run_status_ctrl %d", status);
    if (app_old_comm_get_pump_running_status() != status)
    {
        if (status == APP_OLD_COMM_SYS_STOP)
        {
            pump_ctx->set_breast_pump_status(BREAST_PUMP_PAUSE);
        }
        else if (status == APP_OLD_COMM_SYS_RUN)
        {
            pump_ctx->set_breast_pump_status(BREAST_PUMP_START);
        }
        else
        {
            LOG_SRV("unknow status");
            return false;
        }
    }
    return true;
}

uint8_t app_old_comm_pump_custom_step_para_to_local_motor_policy(user_def_pump_steps_data_t *pump_steps_data)
{
    motor_policy_cfg_t *custom_mode_policy_cfg = custom_mode_policy_cfg_get();
    //memset(&custom_mode_policy_cfg, 0, sizeof(motor_policy_cfg_t)*CUSTOM_MODE_MOTOR_POLICY_NUMBER);
    uint8_t cfg_num = 0;
    for (uint8_t i = 0; i < pump_steps_data->num; i++)
    {
        if (cfg_num >= CUSTOM_MODE_MOTOR_POLICY_NUMBER) {
            LOG_SRV("custom pump step will over max");
            return false;
        }
        LOG_SRV("custom pump step mode %d", pump_steps_data->data[i].mode);
        if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_SHORT) {
            motor_policy_cfg_t stimulation_default_temp[STIMULATION_MODE_LEN] = {0};
            memcpy(stimulation_default_temp, &stimulation_default, sizeof(stimulation_default));
            stimulation_default_temp[0].duration = pump_steps_data->data[i].time;
            stimulation_default_temp[0].core.un.mapping.level = pump_steps_data->data[i].level;
            stimulation_default_temp[0].core.un.mapping.speed = pump_steps_data->data[i].freq;
            memcpy(custom_mode_policy_cfg + cfg_num, &stimulation_default_temp,
                   sizeof(stimulation_default_temp));
            cfg_num += 1;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_LONG) {
            motor_policy_cfg_t pump_default_temp[PUMP_MODE_LEN] = {0};
            memcpy(pump_default_temp, &pump_default, sizeof(pump_default));
            pump_default_temp[0].duration = pump_steps_data->data[i].time;
            pump_default_temp[0].core.un.mapping.level = pump_steps_data->data[i].level;
            pump_default_temp[0].core.un.mapping.speed = pump_steps_data->data[i].freq;
            memcpy(custom_mode_policy_cfg + cfg_num, &pump_default_temp,
                   sizeof(pump_default_temp));
            cfg_num += 1;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_MIX1) {
            motor_policy_cfg_t mix_default_temp[MIX_MODE_LEN] = {0};
            memcpy(mix_default_temp, &mix_default, sizeof(mix_default));
            for (uint8_t j = 0; j < MIX_MODE_LEN; j++) {
                mix_default_temp[j].duration = pump_steps_data->data[i].time;
                mix_default_temp[j].core.un.mapping.level = pump_steps_data->data[i].level;
                mix_default_temp[j].core.un.mapping.speed = pump_steps_data->data[i].freq;
            }
            memcpy(custom_mode_policy_cfg + cfg_num, &mix_default_temp,
                   sizeof(mix_default_temp));
            cfg_num += 2;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_MIX2) {
            motor_policy_cfg_t mix_default2_temp[MIX2_MODE_LEN] = {0};
            memcpy(mix_default2_temp, &mix_default2, sizeof(mix_default2));
            for (uint8_t j = 0; j < MIX2_MODE_LEN; j++) {
                mix_default2_temp[j].duration = pump_steps_data->data[i].time;
                mix_default2_temp[j].core.un.mapping.level = pump_steps_data->data[i].level;
                mix_default2_temp[j].core.un.mapping.speed = pump_steps_data->data[i].freq;
            }
            memcpy(custom_mode_policy_cfg + cfg_num, &mix_default2_temp,
                   sizeof(mix_default2_temp));
            cfg_num += 2;
        } else {
            LOG_SRV("unknow custom pump step mode");
        }
    }
    print_pump_custom_cfg(custom_mode_policy_cfg, cfg_num);
    //motor_manager_context_t *motor_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    //EXCEPTION_CHECK_UINT(motor_ctx);
    //EXCEPTION_CHECK_UINT(motor_ctx->hook);
    //EXCEPTION_CHECK_UINT(motor_ctx->hook->motor_debug_with_user_para);
    //motor_ctx->hook->generate_motor_para(motor_ctx, BREAST_PUMP_CUSTOM_MODE, custom_mode_policy_cfg, cfg_num*sizeof(motor_policy_cfg_t));
    custom_mode_parameter_update(custom_mode_policy_cfg, cfg_num);
    return true;
}

uint8_t app_old_comm_update_storage_custom_setting(user_def_pump_steps_data_t *pump_user_defined_step_data_temp)
{
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    if(pump_user_defined_step_data_temp->num > ROOT_BREAST_PUMP_STEP_MAX)
    {
       LOG_SRV(" over max step");
       return false;
    }
    for (uint8_t i = 0; i < pump_user_defined_step_data_temp->num; i++) {
        pump_setting_t setting;
        setting.speed = pump_user_defined_step_data_temp->data->freq;
        for (uint8_t j = 0; j < BREAST_PUMP_NUM; j++) {
            setting.level[j] = pump_user_defined_step_data_temp->data->level;
        }
        storage_ctx->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, BREAST_PUMP_CUSTOM_MODE, i, &setting);
    }
    return true;
}

//0xd0
void app_old_comm_report_device_status(app_old_commd_l_r_ctrl_e lr_ct, servers_old_protocol_publish_data_id_e id, void *val)
{
    LOG_SRV(" Reporting device status id %d", id);
    uint8_t report_data[45] = {0};
    uint16_t data_len = 0;

    report_data[data_len++] = app_old_comm_get_battery_percent();
    report_data[data_len++] = app_old_comm_get_sys_status();
    report_data[data_len++] = lr_ct;
    report_data[data_len++] = app_old_comm_get_current_pump_mode() + 1; //0x01~0x05（按摩-吸乳-混合-自定义-智能吸乳）
    report_data[data_len++] = app_old_comm_get_current_mode_pump_level(lr_ct, id); //0x01~0x0F（1档~15档）
    report_data[data_len++] = app_old_comm_get_current_custom_pump_step_mode(id);
    report_data[data_len++] = app_old_comm_get_current_mode_pump_level(lr_ct, id); //0x01~0x0F（1档~15档）
    report_data[data_len++] = app_old_comm_get_current_custom_pump_speed(id) + 1;     //频次（1-低 2-中 3-高）（1字节）
    report_data[data_len++] = (app_old_comm_get_pump_mode_run_total_time() >> 0) & 0xFF; //单位s
    report_data[data_len++] = (app_old_comm_get_pump_mode_run_total_time() >> 8) & 0xFF; //单位s
    report_data[data_len++] = (app_old_comm_get_current_pump_mode_run_time() >> 0) & 0xFF;//单位s
    report_data[data_len++] = (app_old_comm_get_current_pump_mode_run_time() >> 8) & 0xFF;//单位s
    report_data[data_len++] = 0; //v3无储奶检测
    report_data[data_len++] = app_old_comm_check_if_low_battery();
    report_data[data_len++] = (app_old_comm_get_current_custom_pump_mode_total_time() >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_get_current_custom_pump_mode_total_time() >> 8) & 0xFF;
    report_data[data_len++] = app_old_comm_get_system_is_airplane_mode();
    report_data[data_len++] = app_old_comm_get_current_mode_pump_speed(id) + 1; //normal speed //频次（1-低 2-中 3-高）（1字节）
    report_data[data_len++] = 0x00; //预留
    report_data[data_len++] = 0x00; //预留

    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_NOTIFY, APP_OLD_COMM_CID_NOTIFY_STATUS, data_len, report_data);
}

//0xb0
void app_old_comm_handle_get_status_req(app_old_commd_l_r_ctrl_e lr_ct)
{
    LOG_SRV(" Handling get device status request\n");
    uint8_t ack_data[45] = {0};
    uint16_t data_len = 0;

    ack_data[data_len++] = app_old_comm_get_battery_percent();
    ack_data[data_len++] = app_old_comm_get_sys_status();
    ack_data[data_len++] = lr_ct;
    ack_data[data_len++] = app_old_comm_get_current_pump_mode() + 1;
    ack_data[data_len++] = app_old_comm_get_current_mode_pump_level(lr_ct, OLD_PUBLISH_DATA_POLL_REPORT);
    ack_data[data_len++] = app_old_comm_get_current_custom_pump_step_mode(OLD_PUBLISH_DATA_POLL_REPORT);
    ack_data[data_len++] = app_old_comm_get_current_mode_pump_level(lr_ct, OLD_PUBLISH_DATA_POLL_REPORT);
    ack_data[data_len++] = app_old_comm_get_current_custom_pump_speed(OLD_PUBLISH_DATA_POLL_REPORT) + 1;
    ack_data[data_len++] = (app_old_comm_get_pump_mode_run_total_time() >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_get_pump_mode_run_total_time() >> 8) & 0xFF;
    ack_data[data_len++] = (app_old_comm_get_current_pump_mode_run_time() >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_get_current_pump_mode_run_time() >> 8) & 0xFF;
    ack_data[data_len++] = app_old_comm_check_if_low_battery();
    ack_data[data_len++] = (app_old_comm_get_current_custom_pump_mode_total_time() >> 0) & 0xFF;
    ack_data[data_len++] = (app_old_comm_get_current_custom_pump_mode_total_time() >> 8) & 0xFF;
    ack_data[data_len++] = app_old_comm_get_system_is_airplane_mode();
    ack_data[data_len++] = app_old_comm_get_current_mode_pump_speed(OLD_PUBLISH_DATA_POLL_REPORT) + 1; //normal speed
    ack_data[data_len++] = 0x00; //预留
    ack_data[data_len++] = 0x00; //预留
    LOG_SRV(" Sending device status ACK (data len: %d)\n", data_len);
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_STATUS, data_len, ack_data);
}

//0xc0
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab)
{
    LOG_SRV(" Handling device control request\n");
    EXCEPTION_CHECK_VOID(cab);
    if (cal < 0x0A) {
        LOG_SRV(" Invalid data length for CTRL_DEV (expected >=0X0A, got %d)\n", cal);
    }
    app_old_comm_pump_run_status_ctrl(cab[0]);
    app_old_comm_pump_ctrl app_old_comm_pump_ctrl_date;
    app_old_comm_pump_ctrl_date.pump_mode = cab[1] - 1; //0x01~0x05（按摩-吸乳-混合-自定义-智能吸乳）
    app_old_comm_pump_ctrl_date.normal_pump_level = cab[2];
    app_old_comm_pump_ctrl_date.user_define_pump_step_mode = cab[3];
    app_old_comm_pump_ctrl_date.user_define_pump_level= cab[4];
    app_old_comm_pump_ctrl_date.user_define_pump_speed= cab[5] - 1; //频次（1-低 2-中 3-高）（1字节）
    app_old_comm_pump_ctrl_date.pump_mode_lr_or_all= cab[6];
    app_old_comm_pump_ctrl_date.normal_pump_speed= cab[7]-1 ;//频次（1-低 2-中 3-高）（1字节）
    app_old_comm_pump_control_handler(&app_old_comm_pump_ctrl_date);
    LOG_HEX("device control hex", cab, cal);
    // TODO: Send parameters to pump valve component
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_CTRL_DEV, 1, &cab[6]);
}

//0xc4
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab)
{
	user_def_pump_steps_data_t pump_user_defined_step_data_temp = {0};

    uint8_t section_num = 0;
    uint8_t i = 0, n = 0;

    EXCEPTION_CHECK_VOID(cab);
    memset(&pump_user_defined_step_data_temp, 0, sizeof(pump_user_defined_step_data_temp));// 清除掉临时变量

	pump_user_defined_step_data_temp.id.dat[0] = cab[0]; // data[3];
	pump_user_defined_step_data_temp.id.dat[1] = cab[1]; // data[2];
	pump_user_defined_step_data_temp.id.dat[2] = cab[2]; // data[1];
	pump_user_defined_step_data_temp.id.dat[3] = cab[3]; // data[0];
	LOG_SRV("cmd ud-mode id = 0X%x", pump_user_defined_step_data_temp.id.dat32);

	if (cab[4] <= ROOT_BREAST_PUMP_STEP_MAX)
	{
		section_num = cab[4];
		pump_user_defined_step_data_temp.num = section_num;
        LOG_SRV("pump_user_defined_step_data_temp.num = %d", pump_user_defined_step_data_temp.num);
	}

	pump_user_defined_step_data_temp.custom_total_time = 0;

	for (i = 0; i < section_num; i++)
	{
		n = 5 + 10 * i;
		if (cab[n] <= ROOT_BREAST_PUMP_STEP_MAX)
		{
			pump_user_defined_step_data_temp.data[i].mode = cab[n + 1];
			pump_user_defined_step_data_temp.data[i].level = cab[n + 2];
			if (pump_user_defined_step_data_temp.data[i].mode > (BREAST_PUMP_MODE_MAX + 1))
			{
				pump_user_defined_step_data_temp.data[i].mode = BREAST_PUMP_NORMAL_MODE_DEFAULT;
			}
			if (pump_user_defined_step_data_temp.data[i].level > BREAST_PUMP_LEVEL_MAX)
			{
				pump_user_defined_step_data_temp.data[i].level = BREAST_PUMP_LEVEL_15;
			}

			pump_user_defined_step_data_temp.data[i].freq = cab[n + 3] - 1;
			pump_user_defined_step_data_temp.data[i].time = cab[n + 4] << 8 | cab[n + 5];
			if (pump_user_defined_step_data_temp.data[i].freq > SUCTION_SPEED_MAX)
			{
				pump_user_defined_step_data_temp.data[i].freq = SUCTION_SPEED_HIGH;
			}
			//PumpUserUdTemp[i].heat_switch = data[n + 6];
			//PumpUserUdTemp[i].heat_level = data[n + 7];
			//PumpUserUdTemp[i].shake_switch = data[n + 8];
			//PumpUserUdTemp[i].shake_level = data[n + 9];
			pump_user_defined_step_data_temp.custom_total_time += pump_user_defined_step_data_temp.data[i].time;
		}

		LOG_SRV("ud-mode[%d]=0x%02x", i, pump_user_defined_step_data_temp.data[i].mode);
		LOG_SRV("ud-level[%d]=0x%02x", i, pump_user_defined_step_data_temp.data[i].level);
		LOG_SRV("ud-time[%d]=0x%04x", i, pump_user_defined_step_data_temp.data[i].time);
	}
    //pump_user_defined_step_data_temp.custom_total_time = 1000*pump_user_defined_step_data_temp.custom_total_time; //转换为ms
	n = 5 + 10 * i;
	LOG_SRV("data n = [%d] data len = [%d]", n, cal);
    //存入flash
    app_old_comm_set_custom_pump_step_parameter_infor(&pump_user_defined_step_data_temp);
    //先暂停pump
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_VOID(pump_ctx);
    if (app_old_comm_get_current_pump_mode() == BREAST_PUMP_CUSTOM_MODE)
        pump_ctx->set_breast_pump_status(BREAST_PUMP_PAUSE);
    //将storage的custom 目标pump time更新
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_VOID(storage_ctx);
    storage_ctx->put_store_ram_val(STORE_ID_BREAST_PUMP_TIME, BREAST_PUMP_CUSTOM_MODE,
                                   pump_user_defined_step_data_temp.custom_total_time, NULL);
    LOG_SRV("pump_user_defined_step_data_temp.custom_total_time= 0x%x", pump_user_defined_step_data_temp.custom_total_time);
    //将storage的custom 的setting更新
    app_old_comm_update_storage_custom_setting(&pump_user_defined_step_data_temp);
    //将默认的custom 策略更新
    //int custom_mode_parameter_update(motor_policy_cfg_t *cfg, uint8_t cfg_num)
    app_old_comm_pump_custom_step_para_to_local_motor_policy(&pump_user_defined_step_data_temp);
    //重新启动pump
    if (app_old_comm_get_current_pump_mode() == BREAST_PUMP_CUSTOM_MODE)
        pump_ctx->set_breast_pump_status(BREAST_PUMP_START);

    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_USER_DEFINE, 0, NULL);
}

//0xc5
void app_old_comm_handle_set_user_define_wr_flash_req(uint8_t cal, uint8_t *cab)
{
    //不需要做啥c4 已经写入
    app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_PARA_DOWNLOAD, 0, NULL);
}

//0xf0
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab)
{
    LOG_SRV(" Handling get device info request\n");
    if (cal != 0x04) {
        LOG_SRV(" Invalid data length for GET_DEV_INFO (expected 4, got %d)\n", cal);
        return;
    }
    storage_manager_context_t *ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    factory_dev_info_t factory_dev_info;

    EXCEPTION_CHECK_VOID(ctx);
    EXCEPTION_CHECK_VOID(cab);
    uint8_t ack_data[64] = {0};
    uint16_t data_len = 0;
    app_old_protocol_handler_context_t *handler_ctx = app_old_protocol_handler_ctx_get();

    ack_data[data_len++] = ctx->priv.ver_info.sys_local_protocol_id;
    ack_data[data_len++] = ctx->priv.ver_info.soc_version;
    ack_data[data_len++] = ctx->priv.ver_info.hw_version;
    ack_data[data_len++] = ctx->priv.ver_info.sw_version / 10 % 10;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[0];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[1];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[2];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[3];//custom_mode_id;
    ack_data[data_len++] = 0x00;//milk_full_ver;
    ack_data[data_len++] = 0x00;//milk_full_switch;
    ctx->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &factory_dev_info);
    memcpy(ack_data + data_len, factory_dev_info.sn, factory_dev_info.sn_len);
    data_len += factory_dev_info.sn_len;
#if BLE_PAYLOAD_CRYPTION
    ack_data[data_len++] = handler_ctx->encrypt_status;//encrypt_status;
    ack_data[data_len++] = handler_ctx->encrypt_key;//encrypt_key;
#endif
    ack_data[data_len++] = ctx->priv.ver_info.sw_version % 10;
    LOG_SRV(" Sending device info ACK (data len: %d)\n", data_len);
   app_old_protocol_hander_frame_builder_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_DEV_INFO, data_len, ack_data);
}
