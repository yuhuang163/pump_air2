/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "root_common.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "motor_policy.h" // IWYU pragma: keep
#include "breast_pump_policy.h" // IWYU pragma: keep

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
static const motor_total_policy_t *breast_pump_mode_policy_get(struct motor_manager_context *thiz)
{
    int breast_pump_mode = thiz->priv.pump_ctx->get_breast_pump_mode();
    list_head_t *head = &thiz->priv.breast_pump_policy_list;
    if (head->next != NULL) {
        LIST_FOR_EACH(pos, head) {
            breast_pump_policy_node_t *node = CONTAINER_OF(pos, breast_pump_policy_node_t, list);
            if (node->breast_pump_mode == breast_pump_mode) {
                LOG_MOTOR("==> find name: %s, breast_pump_mode: %d", node->name, breast_pump_mode);
                return node->get_policy_cb(node->user_data);
            }
        }
    }
    LOGE("Oops! ==> not found! breast_pump_mode: %d", breast_pump_mode);
    msleep(500);

    return NULL;
}

static void update_para_to_motor_device(struct motor_manager_context *thiz, uint8_t id, negative_pressure_info_t *info)
{
    uint32_t args[3] = {0};

    switch (info->action) {
        case MOTOR_PUMP_ON:
            args[0] = id;
            args[1] = BREAST_PUMP_PUMP_FREQ;
            args[2] = info->n_pressure.pump_level;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
            info->n_pressure_flag = 1;
            info->start_time = get_timestamp_ms(0);
            break;

        case MOTOR_PUMP_OFF:
            args[0] = id;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
            info->start_time = get_timestamp_ms(0);
            break;

        case MOTOR_VALVE_ON:
            args[0] = id;
            args[1] = BREAST_PUMP_VALVE_FREQ;
            args[2] = BREAST_PUMP_VALVE_DUTY;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
            info->start_time = get_timestamp_ms(0);
            break;

        case MOTOR_VALVE_OFF:
            args[0] = id;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
            info->n_pressure_flag = 0;
            info->start_time = get_timestamp_ms(0);
            break;

        default:
            break;

    }
}

static void negative_pressure_release(struct motor_manager_context *thiz)
{
    uint32_t args[3] = {0, BREAST_PUMP_VALVE_FREQ, BREAST_PUMP_VALVE_DUTY};

    for (uint8_t i = 0; i < MOTOR_DEV_NUM; i++) {
        args[0] = i;
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
        msleep(5);
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
    }

    msleep(100);
    
    for (uint8_t i = 0; i < MOTOR_DEV_NUM; i++) {
        args[0] = i;
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
    }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void breast_pump_policy_start(struct motor_manager_context *thiz, uint8_t id, uint8_t action)
{
    const motor_total_policy_t *policy_obj = breast_pump_mode_policy_get(thiz);
    EXCEPTION_CHECK_VOID(policy_obj);
    memset(&thiz->priv.m_info[id], 0, sizeof(motor_policy_info_t));
    thiz->priv.m_info[id].total_obj = (motor_total_policy_t *)policy_obj;
}

int breast_pump_mode_policy_handler(struct motor_manager_context *thiz, motor_task_para_t *play_para)
{
    int exit_code = EXIT_REASON_PLAY_COMPLETED;
    motor_policy_info_t *m_info = thiz->priv.m_info;

    LOG_MOTOR("===enter===");
    negative_pressure_release(thiz);
    while (1) {
        if (!thiz->is_can_play_now()) {
            exit_code = EXIT_REASON_INTERRUPT;
            break;
        }

        for (uint8_t i = 0; i < MOTOR_DEV_NUM; i++) {
            pump_time_policy_update(&m_info[i]);  ///  时间策略
            pump_policy_update(&m_info[i].current_info);       /// 子策略
            pump_compensation_policy_update(thiz, &m_info[i]); /// 补偿策略
            update_para_to_motor_device(thiz, i, &m_info[i].current_info);
        }
        msleep(10);
    }
    negative_pressure_release(thiz);
    LOG_MOTOR("exit:%d", exit_code);

    return exit_code;
}
 

