/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "algorithm_manager.h"  // IWYU pragma: keep
#include "common_tlvs.h" // IWYU pragma: keep

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
int algo_tlvs_sample_onoff_set(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len)
{
#if IS_SUPPORT_ALGO_SAMPLING
    EXCEPTION_CHECK_INT(builder && service && data);
    algo_manager_context_t *algo_ctx = (algo_manager_context_t *)find_module(ID_MODULE_ALGO_HUB);
    EXCEPTION_CHECK_INT(algo_ctx && algo_ctx->hook && algo_ctx->hook->algo_sampling_start && algo_ctx->hook->algo_sampling_stop);
    if (data[0] == 1) {
        algo_ctx->hook->algo_sampling_start(algo_ctx);
    } else {
        algo_ctx->hook->algo_sampling_stop(algo_ctx);
    }
#endif
    LOGI("algo_tlvs_sample_onoff_set, data: %d", data[0]);
    return 0;
}

int algo_tlvs_sample_onoff_get(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len)
{
#if IS_SUPPORT_ALGO_SAMPLING
    EXCEPTION_CHECK_INT(builder && service && data);
    algo_manager_context_t *algo_ctx = (algo_manager_context_t *)find_module(ID_MODULE_ALGO_HUB);
    EXCEPTION_CHECK_INT(algo_ctx && algo_ctx->hook && algo_ctx->hook->algo_sampling_is_sampling);
    uint8_t onoff = algo_ctx->hook->algo_sampling_is_sampling(algo_ctx);
    comm_protocol_tlv_node_t tlv_node = {
        .type = ALGO_TLV_SAMPLE_ONOFF_GET,
        .length = 1,
        .value = &onoff,
    };
    return comm_protocol_builder_add_tlv(builder, service, &tlv_node);
#endif
    return 0;
}

int algo_tlvs_sample_interval_set(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service && data);
    LOGI("algo_tlvs_sample_interval_set, data: %d", data[0]);
    return 0;
}

int algo_tlvs_sample_interval_get(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service && data);
    return 0;
}