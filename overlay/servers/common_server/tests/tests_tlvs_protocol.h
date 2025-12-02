#ifndef __TESTS_TLVS_PROTOCOL_H__
#define __TESTS_TLVS_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "comm_protocol_builder.h" // IWYU pragma: keep
#include "servers_manager_api.h" // IWYU pragma: keep

/// TLV ID定义 (产测服务 Service ID = 0x0007)
typedef enum {
    TESTS_TLV_FACTORY_TEST_MODE = 0x0001,      /// 产测模式进入/退出 (0x01:进入, 0x00:退出)
    TESTS_TLV_AGING_TEST_MODE = 0x0002,        /// 老化模式进入/退出 (0x01:进入, 0x00:退出)
    TESTS_TLV_SUCTION_TEST_MODE = 0x0003,      /// 吸力测试模式进入/退出 (0x01:进入, 0x00:退出)
    TESTS_TLV_AGING_STATUS_QUERY = 0x0004,     /// 老化状态查询
    TESTS_TLV_KEY_STATUS_REPORT = 0x0005,      /// 按键状态上报 (0x01:短按, 0x02:长按) NOTIFY
    TESTS_TLV_ENCODER_STATUS_REPORT = 0x0006, /// 编码器状态上报 (0x01:左旋, 0x02:右旋) NOTIFY
    TESTS_TLV_WRITE_SN = 0x0007,               /// 写入整机序列号SN (35字节)
    TESTS_TLV_WRITE_PRODUCT_ID = 0x0008,       /// 写入整机产品ID (6字节)
    TESTS_TLV_WRITE_DEVICE_ID = 0x0009,        /// 写入整机设备ID (16字节)
    TESTS_TLV_WRITE_SECRET_KEY = 0x000A,       /// 写入整机三元组密钥 (16字节)
    TESTS_TLV_READ_TRIPLE_DATA = 0x000B,       /// 读取整机三元组所有数据
    TESTS_TLV_DEVICE_ERROR_STATUS = 0x000C,    /// 设备异常状态查询
    TESTS_TLV_SUCTION_COMPENSATION = 0x000D,   /// 吸力补偿开关 (0x01:开, 0x00:关)
    TESTS_TLV_READ_SN = 0x000E,                /// 读取整机SN数据
    TESTS_TLV_ID_MAX,
} tests_tlvs_id_e;

/// 按键状态值
typedef enum {
    TESTS_KEY_STATUS_SHORT_PRESS = 0x01,       /// 短按
    TESTS_KEY_STATUS_LONG_PRESS = 0x02,        /// 长按
} tests_key_status_e;

/// 编码器状态值
typedef enum {
    TESTS_ENCODER_STATUS_LEFT = 0x01,          /// 左旋
    TESTS_ENCODER_STATUS_RIGHT = 0x02,         /// 右旋
} tests_encoder_status_e;

/// 设备异常状态值
typedef enum {
    TESTS_DEVICE_ERROR_NONE = 0x00,            /// 正常
    TESTS_DEVICE_ERROR_LOW_BATTERY = 0x01,     /// 电量低
    TESTS_DEVICE_ERROR_AIR_LEAK = 0x02,        /// 漏气
    TESTS_DEVICE_ERROR_MOTOR_STALL = 0x03,     /// 堵转
} tests_device_error_status_e;

/// 老化状态结果
typedef enum {
    TESTS_AGING_STATUS_FAIL = 0x00,            /// 老化失败
    TESTS_AGING_STATUS_SUCCESS = 0x01,         /// 老化成功
} tests_aging_status_e;

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
/// 模式控制
int tests_tlvs_factory_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_aging_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_suction_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);

/// 老化状态查询
int tests_tlvs_aging_status_query(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);

/// 三元组写入
int tests_tlvs_write_sn(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_write_product_id(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_write_device_id(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_write_secret_key(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);

/// 三元组读取
int tests_tlvs_read_triple_data(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_read_sn(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);

/// 设备状态
int tests_tlvs_device_error_status(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);
int tests_tlvs_suction_compensation(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                    uint8_t *data, uint16_t data_len);

/// NOTIFY上报 (设备主动上报)
int tests_key_status_publish(servers_publish_data_args_t *args);
int tests_encoder_status_publish(servers_publish_data_args_t *args);

/// 服务处理入口
int tests_subscribe_service_handle(comm_protocol_builder_t *builder, comm_protocol_service_node_t *srv_node);

#ifdef __cplusplus
}
#endif

#endif 
