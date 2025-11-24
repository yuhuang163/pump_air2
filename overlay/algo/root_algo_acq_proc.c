/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ble_manager_api.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "algorithm_manager.h"  // IWYU pragma: keep
#include "common_tlvs.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define ALGO_SAMPLING_DATA_POINT_NUM      10 /// 一包数据包含采集点个数

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static uint16_t sampling_count = 0; /// 算法采集数据次数
static algo_sampling_data_t *algo_sampling_data = NULL;
static motor_manager_context_t *motor_ctx = NULL;
static protocol_manager_context_t *protocol_ctx = NULL;
static ble_manager_context_t *ble_ctx = NULL;

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
void algo_sampling_data_cb(algo_manager_context_t *ctx, uint8_t *data, int data_len)
{
    int ret = 0;
    EXCEPTION_CHECK_VOID(ctx && data && algo_sampling_data);
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
    algo_sampling_data_t *sampling_data = &algo_sampling_data[sampling_count];
    motor_policy_info_t *info = motor_ctx->priv.runtime.info;
    sampling_data->timestamp = get_timestamp_ms(0);
    /// 气压数据
    for (int i = 0; i < AIR_PRESSURE_NUM; i++) {
        sampling_data->air_pressure[i] = sensor_data->air_pressure_data[i].pressure;
    }
    /// 泵奶数据
    for (int i = 0; i < BREAST_PUMP_NUM; i++) {
        sampling_data->pump_state[i].state = info[i].state.status;
        sampling_data->pump_state[i].pump_duty = info[i].state.pump_duty;
        sampling_data->pump_state[i].valve_duty = info[i].state.valve_duty;
        sampling_data->pump_state[i].target_pressure = info[i].state.target_pressure;
        #if IS_SUPPORT_RELATIVE_RUN_TIME
            sampling_data->pump_state[i].run_time = info[i].state.run_time;
        #else
            sampling_data->pump_state[i].run_time = get_timestamp_ms(info[i].state.start_time);
        #endif
    }
    sampling_count++;
    if (sampling_count >= ALGO_SAMPLING_DATA_POINT_NUM) {
        sampling_count = 0;
        /// 构建协议数据结构
        comm_protocol_builder_t builder = {0};
        ret = comm_protocol_builder_init(&builder, 1024);
        if (ret != 0) {
            LOG_ALGO("comm_protocol_builder_init failed");
            return;
        }
        /// 初始化服务节点
        comm_protocol_service_t *service = comm_protocol_builder_init_service(&builder, COMM_PROTOCOL_ALGO_SERVICE);
        if (service == NULL) {
            LOGE("==> algo service begin error");
            comm_protocol_builder_free(&builder);
            return;
        }
        /// 添加TLV
        comm_protocol_tlv_node_t rsp_tlv_node = {0};
        uint16_t data_num = ALGO_SAMPLING_DATA_POINT_NUM;
        rsp_tlv_node.type = ALGO_TLV_SAMPLE_DATA_NUM;
        rsp_tlv_node.length = 2;
        rsp_tlv_node.value = (uint8_t *)&data_num;
        ret = comm_protocol_builder_add_tlv(&builder, service, &rsp_tlv_node);
        if (ret != 0) {
            LOG_ALGO("comm_protocol_builder_add_tlv failed");
            comm_protocol_builder_free(&builder);
            return;
        }
        rsp_tlv_node.type = ALGO_TLV_SAMPLE_AIR_PRESSURE_DATA;
        rsp_tlv_node.length = sizeof(algo_sampling_data_t) * ALGO_SAMPLING_DATA_POINT_NUM;
        rsp_tlv_node.value = (uint8_t *)algo_sampling_data;
        ret = comm_protocol_builder_add_tlv(&builder, service, &rsp_tlv_node);
        if (ret != 0) {
            LOG_ALGO("comm_protocol_builder_add_tlv failed");
            comm_protocol_builder_free(&builder);
            return;
        }
        uint16_t out_size = 0;
        uint8_t *data = comm_protocol_builder_finalize(&builder, &out_size);
        if (data == NULL) {
            LOG_ALGO("comm_protocol_builder_finalize failed");
            comm_protocol_builder_free(&builder);   
            return;
        }
        // LOG_ALGO("algo sampling data size %d", out_size);
        ble_ctx->dispatch_ble_event_wrapper(ble_ctx, BLE_MSG_FAC_SEND_DATA, (uint8_t *)data, out_size);
        comm_protocol_builder_free(&builder);
    }

#if 0
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        PRINT_LOG("%d pressure:%f raw:%d state:%d pump:%d valve:%d target:%d time:%d; ", 
            i, sensor_data->air_pressure_data[i].pressure, sensor_data->air_pressure_data[i].raw_data,
            info[i].state.status, 
            info[i].state.pump_duty, info[i].state.valve_duty,
            info[i].state.target_pressure, info[i].state.run_time);
    }
    PRINT_LOG("\r\n");
#endif

}

void algo_sampling_start(algo_manager_context_t *ctx)
{
    EXCEPTION_CHECK_VOID(ctx);
    motor_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    protocol_ctx = (protocol_manager_context_t *)find_module(ID_MODULE_PROTOCOL);
    ble_ctx = (ble_manager_context_t *)find_module(ID_MODULE_BLE);
    if (protocol_ctx == NULL || motor_ctx == NULL || ble_ctx == NULL) {
        LOG_ALGO("protocol_ctx or motor_ctx not found");
        return;
    }
    EXCEPTION_CHECK_VOID(motor_ctx && protocol_ctx);
    sampling_count = 0;
    ctx->priv.is_sampling = 1;
    algo_sampling_data = u_malloz(sizeof(algo_sampling_data_t) * ALGO_SAMPLING_DATA_POINT_NUM);
    LOG_ALGO("algo sampling start");
}

void algo_sampling_stop(algo_manager_context_t *ctx)
{
    EXCEPTION_CHECK_VOID(ctx);
    ctx->priv.is_sampling = 0;
    u_free(algo_sampling_data);
    algo_sampling_data = NULL;
    LOG_ALGO("algo sampling stop");
}

bool algo_sampling_is_sampling(algo_manager_context_t *ctx)
{
    EXCEPTION_CHECK_BOOL(ctx);
    return ctx->priv.is_sampling;
}