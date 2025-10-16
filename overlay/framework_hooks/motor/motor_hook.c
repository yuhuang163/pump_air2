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

    add_policy_cb(head, "stimulation", BREAST_PUMP_STIMULATION_MODE, NULL,
        stimulation_mode_parameter_init, stimulation_mode_policy_get);
    add_policy_cb(head, "pump",  BREAST_PUMP_PUMP_MODE,  NULL,
        pump_mode_parameter_init,  pump_mode_policy_get);
    add_policy_cb(head, "mix", BREAST_PUMP_MIX_MODE, NULL,
        mix_mode_parameter_init, mix_mode_policy_get);
    add_policy_cb(head, "custom", BREAST_PUMP_CUSTOM_MODE, NULL,
        custom_mode_parameter_init, custom_mode_policy_get);
    add_policy_cb(head, "pp_milk_track",  BREAST_PUMP_PP_MILK_TRACK_MODE,  NULL,
        pp_milk_track_mode_parameter_init,  pp_milk_track_mode_policy_get);
    add_policy_cb(head, "2+8_milk_boost", BREAST_PUMP_MILK_BOOST_MODE, NULL,
        milk_boost_mode_parameter_init, milk_boost_mode_policy_get);
    add_policy_cb(head, "lactation_start", BREAST_PUMP_LACTATION_START_MODE, NULL,
        lactation_start_mode_parameter_init, lactation_start_mode_policy_get);

    return 0;
}

static void add_new_breast_pump_policy(list_head_t *head, const char *name, int breast_pump_mode, 
    void *user_data, breast_pump_policy_init_func init_cb, get_policy_obj_func policy_cb)
{
    breast_pump_policy_node_t *node = u_malloz(sizeof(breast_pump_policy_node_t));

    node->name = (char *)name;
    node->user_data = user_data;
    node->breast_pump_mode = breast_pump_mode;
    node->init_cb = init_cb;
    node->get_policy_cb = policy_cb;

    __list_add_tail(&node->list, head);
}

static int breast_pump_policy_init(struct motor_manager_context *ctx)
{
    int ret = 0;
    breast_pump_policy_register(ctx, add_new_breast_pump_policy);
    list_head_t *head = &ctx->priv.breast_pump_policy_list;
    if (head->next != NULL) {
        LIST_FOR_EACH(pos, head) {
            breast_pump_policy_node_t *node = CONTAINER_OF(pos, breast_pump_policy_node_t, list);
            if (node->init_cb) {
                ret = node->init_cb(ctx);
            }
        }
    }
    return ret;
}

static int breast_pump_mode_config_pasrse(void)
{
    return 0;
}

static int on_motor_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data)
{
    return 0;
}

static int generate_motor_para_custom(void *user_data, int data_size)
{
    return 0;
}

static int generate_motor_para(int mode, void *user_data, int data_size)
{
    switch (mode) {
        case BREAST_PUMP_CUSTOM_MODE:
            return generate_motor_para_custom(user_data, data_size);
        default:
            return -1;
    }
}

static int on_post_motor_init_route(struct motor_manager_context *ctx)
{
    /// 将模式策略参数挂载到列表中
    breast_pump_policy_init(ctx);

    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_motor_hook(motor_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __motor_manager_hook));
    ctx->hook->on_post_init_route = on_post_motor_init_route;
    ctx->hook->on_motor_receiver_route = on_motor_receiver_route;
    ctx->hook->generate_motor_para = generate_motor_para;
}
