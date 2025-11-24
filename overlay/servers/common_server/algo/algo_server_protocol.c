/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "common_tlvs.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
const static protocol_service_tlvs_handle_t protocol_service_tlvs_list[] = {
    {ALGO_TLV_SAMPLE_ONOFF_SET, algo_tlvs_sample_onoff_set},
    {ALGO_TLV_SAMPLE_INTERVAL_SET, algo_tlvs_sample_interval_set},
    {ALGO_TLV_SAMPLE_ONOFF_GET, algo_tlvs_sample_onoff_get},
    {ALGO_TLV_SAMPLE_INTERVAL_GET, algo_tlvs_sample_interval_get},
};

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
int algo_subscribe_service_handle(comm_protocol_builder_t *builder, comm_protocol_service_node_t *srv_node)
{
    int ret = 0;
    EXCEPTION_CHECK_INT(builder && srv_node && srv_node->tlvs);
    if (srv_node->srv_length < COMM_PROTOCOL_TLV_HEADER_SIZE || srv_node->srv_length > COMM_PROTOCOL_MAX_PAYLOAD_SIZE) {
        LOGE("==> algo server protocol service length error: %d", srv_node->srv_length);
        return -1;
    }
    protocol_service_tlvs_handle_list_t ctx = {
        .list = protocol_service_tlvs_list,
        .list_len = ARRAY_SIZE(protocol_service_tlvs_list),
        .name = "algo",
    };
    
    protocol_service_tlv_func tlv_func =  NULL;
    comm_protocol_tlv_t *tlv = NULL;

    uint32_t tlv_offset = 0;

    LOGI("==> algo service tlv_offset: %d", srv_node->srv_length);
    LOG_HEX("==> algo service tlvs", srv_node->tlvs, srv_node->srv_length);
    /// 添加应答服务节点
    comm_protocol_service_t *service = comm_protocol_builder_init_service(builder, COMM_PROTOCOL_ALGO_SERVICE);
    if (service == NULL) {
        LOGE("==> algo service begin error");
        return -1;
    }
    /// 处理TLV
    for (;;) {
        /// 验证TLV空间是否足够
        if (tlv_offset + COMM_PROTOCOL_TLV_HEADER_SIZE > service->srv_length) {
            break;
        }
        /// 获取TLV
        tlv = (comm_protocol_tlv_t *)(service->tlvs + tlv_offset);
        if (tlv_offset + COMM_PROTOCOL_TLV_HEADER_SIZE + tlv->length > service->srv_length) {
            LOGE("==> algo service tlv length error: %d", tlv->type);
            ret = -1;
            break;
        }
        /// 获取TLV函数
        tlv_func = get_protocol_service_tlvs(&ctx, tlv->type);
        if (tlv_func == NULL) {
            LOGW("==> algo service tlv func not found: %d", tlv->type);
            tlv_offset += COMM_PROTOCOL_TLV_HEADER_SIZE + tlv->length;
            continue;
        }
        /// 调用TLV函数
        LOGI("==> algo service tlv_id: %d, tlv_func: %p", tlv->type, tlv_func);
        ret = tlv_func(builder, service, tlv->value, tlv->length);
        if (ret != 0) {
            LOGE("==> algo service tlv func error: %d, tlv_id: %d", ret, tlv->type);
            break;
        }
        /// 更新TLV偏移
        tlv_offset += COMM_PROTOCOL_TLV_HEADER_SIZE + tlv->length;
    }

    /// 添加应答TLV
    uint16_t err_code = (ret == 0) ? 0 : 1;
    comm_protocol_tlv_node_t rsp_tlv_node = {
        .type = PROTOCOL_SERVICE_ERROR_CODE_TLV_ID,
        .length = sizeof(err_code),
        .value = (uint8_t *)&err_code,
    };
    comm_protocol_builder_add_tlv(builder, service, &rsp_tlv_node);
    return ret;
}
