/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "breath_pump_policy.h" // IWYU pragma: keep

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
static motor_total_policy_t *breath_pump_mode_policy_get(motor_manager_context_t *thiz)
{
    int breath_pump_mode = thiz->priv.pump_ctx->get_breath_pump_mode();

    list_head_t *head = &thiz->priv.breath_pump_policy_list;
    if (head->next != NULL) {
        LIST_FOR_EACH(pos, head) {
            breath_pump_policy_node_t *node = CONTAINER_OF(pos, breath_pump_policy_node_t, list);
            if (node->breath_pump_mode == breath_pump_mode) {
                LOG_MOTOR("==> find name: %s, breath_pump_mode: %d", node->name, breath_pump_mode);
                return node->get_policy_cb(node->user_data);
            }
        }
    }
    LOGE("Oops! ==> not found! breath_pump_mode: %d", breath_pump_mode);
    msleep(500);

    return NULL;
}

static void breath_pump_policy_status_init(motor_manager_context_t *thiz, motor_total_policy_t *policy_obj, uint8_t action)
{

}

static motor_total_policy_t *breath_pump_policy_perpare(void *node, motor_manager_context_t *thiz, uint8_t action)
{
    motor_total_policy_t *policy_obj = breath_pump_mode_policy_get(thiz);
    motor_policy_info_t *policy = &thiz->priv.policy_info;

    if (policy_obj == NULL) {
        LOGE("policy obj NULL");
        return NULL;
    }

    if (thiz->priv.motor_dev == NULL) {
        LOGE("motor devive NULL");
        return NULL;
    }

    breath_pump_policy_status_init(thiz, policy_obj, MOTOR_POLICY_START);
    if (thiz->priv.is_user_stoped) {
        LOGE("motor climb stop before 3s");
        return NULL;
    }

    ///  初始化启动时间
    policy->policy_start_time = get_timestamp();
    policy->para_start_time = policy->policy_start_time;

    ///  策略导入
    // motor_area_time_policy_update(thiz);        ///  时间策略

    ///  切换为指令模式
    DEVICE_IOCTL(thiz->priv.motor_dev, OP_CODE_MOTOR_START_PLAY);
    if (thiz->priv.motor_dev) {
        // uint32_t args[] = {PUMP_DATA_CONTROL, (uint32_t)&thiz->policy.motor};
        // DEVICE_IOCTL_ARGS(thiz->priv.motor_dev, OP_CODE_MOTOR_SET_PARAMETER, args);
    }
    // memcpy(&thiz->policy.last_motor_para, &thiz->policy.motor, sizeof(motor_para_t));  

    return policy_obj;
}

 
static int breath_pump_mode_policy_handler(void *node, motor_manager_context_t *thiz, uint8_t action)
{
    if (!thiz->is_can_play_now()) {
        LOGE("breath_pump stop");
        return EXIT_REASON_EXCEPTION;
    }

    motor_total_policy_t *policy_obj = breath_pump_policy_perpare(node, thiz, action);
    int exit_code = EXIT_REASON_PLAY_COMPLETED;

    LOG_MOTOR("===enter===");
    if (!policy_obj) {
        LOGE("policy_obj NULL");
        msleep(20);
        return EXIT_REASON_EXCEPTION;
    }
    msleep(20);
    while (1) {
        if (!thiz->is_can_play_now()) {
            exit_code = EXIT_REASON_INTERRUPT;
            break;
        }
        // motor_area_time_policy_update(thiz);        ///  时间策略
        // motor_user_policy_update(thiz);     ///  用户自定义策略 
        // update_breath_pump_parameter(thiz);
        msleep(20);
    }
    DEVICE_IOCTL(thiz->priv.motor_dev, OP_CODE_MOTOR_STOP_PLAY);
    LOG_MOTOR("exit:%d", exit_code);

    return exit_code;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int breath_pump_mode_config_init(motor_manager_context_t *thiz)
{
    return 0;
}
 
void update_breath_pump_parameter(motor_manager_context_t *thiz)
{
    // if (memcmp(&thiz->policy.last_motor_para, &thiz->policy.motor, sizeof(motor_para_t)) != 0) {
    //     memcpy(&thiz->policy.last_motor_para, &thiz->policy.motor, sizeof(motor_para_t));           
    //     uint32_t args[] = {PUMP_DATA_CONTROL, (uint32_t)&thiz->policy.motor };
    //     DEVICE_IOCTL_ARGS(thiz->motor_dev, OP_CODE_MOTOR_SET_PARAMETER, args);
    // }    
}
