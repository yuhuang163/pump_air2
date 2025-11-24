/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "common_service.h" // IWYU pragma: keep
#include "common_service_base.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
const static comm_protocol_subscribe_handle_t comm_protocol_subscribe_list[] = {
    {COMM_PROTOCOL_ALGO_SERVICE, algo_subscribe_service_handle},
    {COMM_PROTOCOL_BREAST_PUMP_SERVICE, pump_subscribe_service_handle},
    {COMM_PROTOCOL_SYSTEM_CONFIG_SERVICE, sys_config_subscribe_service_handle},
    {COMM_PROTOCOL_SYSTEM_STATE_SERVICE, sys_state_subscribe_service_handle},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static comm_protocol_subscribe_func get_comm_protocol_subscribe_func(uint16_t service_id)
{
    const comm_protocol_subscribe_handle_t *list = comm_protocol_subscribe_list;
    uint16_t list_len = LENGTH_OF_ARRAY(comm_protocol_subscribe_list);
    for (uint16_t i = 0; i < list_len; i++) {
        if (list[i].service_id == service_id) {
            return list[i].subscribe_func;
        }
    }
    LOGE("==> get comm protocol service func failed: %d", service_id);
    return NULL;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int comm_protocol_subscribe_handler(comm_protocol_subscribe_context_t *ctx)
{
    uint8_t *rsp_buffer = NULL;
    uint16_t rsp_buffer_size = 0;
    int err_code = COMM_SERVICE_ERROR_NONE;
    EXCEPTION_CHECK_INT(ctx);
    /// 解析协议帧
    comm_protocol_frame_node_t *frame = u_malloc(sizeof(comm_protocol_frame_node_t));
    EXCEPTION_CHECK_INT(frame);
    int ret = comm_protocol_frame_deserialize(ctx->buffer, ctx->size, frame);
    if (ret != 0){
        err_code = COMM_SERVICE_ERROR_INVALID_FRAME;
        LOGE("==> comm protocol subscribe deserialize error: %d", ret);
        goto ERROR_LABEL;
    }
    /// 解析Payload
    comm_protocol_service_node_t *service = NULL;
    if (frame->payload.service_count > COMM_PROTOCOL_MAX_SERVICE_NUM){
        err_code = COMM_SERVICE_ERROR_INVALID_PAYLOAD;
        goto ERROR_LABEL;
    }
    /// 构建应答帧
    comm_protocol_builder_t builder = {0};
    ret = comm_protocol_builder_init(&builder, COMM_PROTOCOL_MAX_SERVICE_NUM);
    if (ret != 0){
        err_code = COMM_SERVICE_ERROR_INVALID_BUILDER;
        goto ERROR_LABEL;
    }
    LOGI("==> comm protocol subscribe service count: %d", frame->payload.service_count);
    /// 查找服务处理接口
    for (uint8_t i = 0; i < frame->payload.service_count; i++){
        service = &frame->payload.services[i];
        LOGI("==> comm protocol subscribe service: %d", service->svr_id);
        comm_protocol_subscribe_func func = get_comm_protocol_subscribe_func(service->svr_id);
        if (func == NULL){
            LOGE("==> get comm protocol subscribe func failed: %d", service->svr_id);
            continue;
        }
        LOGI("==> comm protocol subscribe func: %p", func);
        ret = func(&builder, service);
        if (ret != 0){
            LOGE("==> comm protocol subscribe func error: %d, service_id: %d", ret, service->svr_id);
            continue;
        }
    }
    /// 结束服务
    LOGI("==> comm protocol subscribe finalize");
    rsp_buffer = comm_protocol_builder_finalize(&builder, &rsp_buffer_size);
    if (ctx->cb && rsp_buffer) {
        ctx->cb(err_code, rsp_buffer, rsp_buffer_size); /// 回调函数执行应答操作
    }
    LOGI("==> comm protocol subscribe builder free");
    comm_protocol_builder_free(&builder);
    u_free(frame);
    return 0;

ERROR_LABEL:
    LOGE("==> comm protocol subscribe error: %d", err_code);
    if (ctx->cb){
        ctx->cb(err_code, NULL, 0);
    }
    u_free(frame);
    return -1;
}