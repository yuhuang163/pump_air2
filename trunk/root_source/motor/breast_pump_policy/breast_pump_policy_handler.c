/*
* INCLUDE FILES
*****************************************************************************************
*/
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
static const motor_policy_manager_t *breast_pump_mode_policy_get(struct motor_manager_context *thiz, int breast_pump_mode)
{
    list_head_t *head = &thiz->priv.policy_list;
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

static void update_para_to_motor_device(struct motor_manager_context *thiz, uint8_t id, motor_policy_info_t *info)
{
    motor_para_t motor = {0};
    uint32_t args[2] = {0};

    switch (info->state.action) {
        case MOTOR_ACTION_PUMP_ON:
            motor.un.pwm.duty = info->parameters.pump_level;
            motor.un.pwm.freq = BREAST_PUMP_PUMP_FREQ;
            args[0] =  MOTOR_PWM_PUMP_1 + id;
            args[1] = (uint32_t)&motor;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
            info->state.n_pressure_flag = 1;
            break;

        case MOTOR_ACTION_PUMP_OFF:
            args[0] = MOTOR_PWM_PUMP_1 + id;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
            break;

        case MOTOR_ACTION_VALVE_ON:
            motor.un.pwm.duty = BREAST_PUMP_VALVE_DUTY;
            motor.un.pwm.freq = BREAST_PUMP_VALVE_FREQ;
            args[0] = MOTOR_PWM_VALVE_1 + id;
            args[1] = (uint32_t)&motor;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
            break;

        case MOTOR_ACTION_VALVE_OFF:
            args[0] = MOTOR_PWM_VALVE_1 + id;
            DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
            info->state.n_pressure_flag = 0;
            break;

        default:
            break;

    }
    info->state.action = MOTOR_ACTION_IDLE;
}


/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void negative_pressure_release(struct motor_manager_context *thiz)
{
    motor_para_t motor = {
        .un.pwm.duty = BREAST_PUMP_VALVE_DUTY,
        .un.pwm.freq = BREAST_PUMP_VALVE_FREQ
    };
    uint32_t args[2] = {0, (uint32_t)&motor};
    LOG_MOTOR("-->step 1");
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        args[0] = i;
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
    }
    msleep(5);
    LOG_MOTOR("-->step 2");
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        args[0] = i;
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY, args);
    }
    msleep(100);
    LOG_MOTOR("-->step 3");
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        args[0] = i;
        DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY, args);
    }
}

void breast_pump_policy_prepare(struct motor_manager_context *thiz, uint8_t action)
{
    motor_policy_runtime_t *body = &thiz->priv.runtime;
    body->breast_pump_mode = thiz->priv.pump_ctx->get_breast_pump_mode();
    const motor_policy_manager_t *policy_obj = breast_pump_mode_policy_get(thiz,
        body->breast_pump_mode);
        
    EXCEPTION_CHECK_VOID(policy_obj);
    body->timed_policy_start_time = get_timestamp();
    body->manager_start_time = body->timed_policy_start_time;
    body->manager = (motor_policy_manager_t *)policy_obj;
    body->timed_policy = &body->manager->head;
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        memcpy(&body->policy[i], &body->manager->head.policy, sizeof(motor_policy_node_t));
        pump_policy_info_reset(thiz, &body->policy[i], &body->info[i]);
    }
}

int breast_pump_mode_policy_handler(struct motor_manager_context *thiz, motor_task_para_t *play_para)
{
    int exit_code = EXIT_REASON_PLAY_COMPLETED;
    motor_policy_runtime_t *runtime = &thiz->priv.runtime;

    LOG_MOTOR("===enter===");
    EXCEPTION_CHECK_INT(runtime->manager);
    negative_pressure_release(thiz);
    while (1) {
        if (!thiz->is_can_play_now()) {
            exit_code = EXIT_REASON_INTERRUPT;
            break;
        }
        
        pump_time_policy_update(thiz, runtime);  ///  时间策略
        for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {   
            if (runtime->policy[i].core.mapping.mode_type == SUCTION_MODE_IDLE) {
                continue;
            } 
            pump_policy_update(&runtime->info[i]);    
            update_para_to_motor_device(thiz, i, &runtime->info[i]);   
        }    
        msleep(10);
    }
    negative_pressure_release(thiz);
    LOG_MOTOR("exit:%d", exit_code);

    return exit_code;
}
 

