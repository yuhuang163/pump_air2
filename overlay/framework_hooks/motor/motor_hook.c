/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "persist_storage.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "breast_pump_mode.h"

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
static int breast_pump_policy_register(struct motor_manager_context *thiz, add_breast_pump_policy_fptr add_policy_cb)
{
    list_head_t *head = &thiz->priv.breast_pump_policy_list;

    add_policy_cb(head, "stimulation", BREAST_PUMP_MODE_STIMULATION, NULL,
        stimulation_mode_parameter_init, stimulation_mode_policy_get);
    add_policy_cb(head, "breastfeeding",  BREAST_PUMP_MODE_BREASTFEEDING,  NULL,
        breastfeeding_mode_parameter_init,  breastfeeding_mode_policy_get);
    add_policy_cb(head, "hybrid", BREAST_PUMP_MODE_HYBRID, NULL,
        hybrid_mode_parameter_init, hybrid_mode_policy_get);
    add_policy_cb(head, "personalized", BREAST_PUMP_MODE_PERSONALIZED, NULL,
        personalized_mode_parameter_init, personalized_mode_policy_get);
    add_policy_cb(head, "pp_catch_up",  BREAST_PUMP_MODE_PP,  NULL,
        pp_catch_up_mode_parameter_init,  pp_catch_up_mode_policy_get);
    add_policy_cb(head, "2+8_upgrade", BREAST_PUMP_MODE_UPGRADE, NULL,
        upgrade_mode_parameter_init, upgrade_mode_policy_get);
    add_policy_cb(head, "milk_initiation", BREAST_PUMP_MODE_MILK_INIT, NULL,
        milk_initiation_mode_parameter_init, milk_initiation_mode_policy_get);

    return 0;
}

static int motor_level_convert_table_index(int level)
{
    return 0;
}

static int normal_mode_config_pasrse(void)
{
    return 0;
}

static int get_normal_mode_default_para(int mode, int mode_idx, int side, motor_policy_para_t *motor) 
{
    LOGE("side config no found");
    return -3;
}

static int on_motor_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data)
{
    return 0;
}

static int generate_motor_para_personalized(void *user_data, int data_size)
{
    // todo
    return 0;
}

static int generate_motor_para(int mode, void *user_data, int data_size)
{
    switch (mode) {
        case BREAST_PUMP_MODE_PERSONALIZED:
            return generate_motor_para_personalized(user_data, data_size);
        default:
            return -1;
    }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_motor_hook(motor_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __motor_manager_hook));
    ctx->hook->breast_pump_policy_register = breast_pump_policy_register;
    ctx->hook->motor_level_convert_table_index = motor_level_convert_table_index;
    ctx->hook->normal_mode_config_pasrse = normal_mode_config_pasrse;
    ctx->hook->get_normal_mode_default_para = get_normal_mode_default_para;
    ctx->hook->on_motor_receiver_route = on_motor_receiver_route;
    ctx->hook->generate_motor_para = generate_motor_para;
}
