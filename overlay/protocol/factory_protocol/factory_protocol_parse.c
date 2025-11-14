/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
//#include "factory_protocol_handler.h"
#include "factory_protocol_parse.h"
#include "protocol_manager_api.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static protocol_manager_context_t *thiz = NULL;

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

uint8_t factory_tests_protocol_parse_route(uint8_t *data, uint16_t len)
{
    // 这里添加工厂测试协议的解析逻辑
    LOGI("Factory protocol parse data len: %d", len);
    // 示例：简单返回成功标志
    return 1;
}

int on_factory_protocol_br_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data)
{
    protocol_manager_context_t *ctx = context;

    switch (id.code) {
        case ID_BROADCAST_PM_STATE_CHANGE:
            {

            }
            break;
        case ID_BROADCAST_UPDATE_BATTERY_INFO:
            {

            }
            break;
        default:
            break;
    }
}

/// 初始化工厂测试模块，获取工厂测试单独需要相关设备句柄.
void on_perpare_factory_protocol_init(protocol_manager_context_t *ctx)
{
    thiz = ctx;
}

/// 初始化工厂测试模块，获取设备存储的产测信息等操作.
void on_post_factory_protocol_init(protocol_manager_context_t *ctx)
{
    thiz = ctx;
}
