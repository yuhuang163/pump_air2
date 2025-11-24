#ifndef __COMM_PROTOCOL_DEFS_H__
#define __COMM_PROTOCOL_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "stdint.h" // IWYU pragma: keep

/// 协议常量定义
#define COMM_PROTOCOL_SOF                 0x5CC5             ///< Start Of Frame 标识
#define COMM_PROTOCOL_HEADER_SIZE         4                  ///< Header大小: SOF(2) + Length(2)
#define COMM_PROTOCOL_FOOTER_SIZE         2                  ///< Footer大小: Checksum(2)
#define COMM_PROTOCOL_SERVICE_HEADER_SIZE 4                  ///< Service Header: SvrID(2) + Length(2)
#define COMM_PROTOCOL_TLV_HEADER_SIZE     4                  ///< TLV Header: Type(2) + Length(2)
#define COMM_PROTOCOL_SERVICE_SIZE_MIN    (COMM_PROTOCOL_SERVICE_HEADER_SIZE + \
                        COMM_PROTOCOL_TLV_HEADER_SIZE)       ///< 单个Service最小长度
#define COMM_PROTOCOL_PAYLOAD_SIZE_MIN    (COMM_PROTOCOL_HEADER_SIZE + \
                        COMM_PROTOCOL_FOOTER_SIZE)           ///< Payload最小长度
#define COMM_PROTOCOL_MAX_PAYLOAD_SIZE    2048               ///< 最大Payload长度
#define COMM_PROTOCOL_FRAME_SIZE_MAX      (COMM_PROTOCOL_MAX_PAYLOAD_SIZE + \
                        COMM_PROTOCOL_HEADER_SIZE +\
                        COMM_PROTOCOL_FOOTER_SIZE)           ///< 帧最大长度
#define COMM_PROTOCOL_FRAME_SIZE_MIN    (COMM_PROTOCOL_HEADER_SIZE + \
                        COMM_PROTOCOL_SERVICE_SIZE_MIN + \
                        COMM_PROTOCOL_FOOTER_SIZE)           ///< 帧最小长度

/// 协议最大长度限制
#define COMM_PROTOCOL_MAX_SERVICE_NUM     16                 ///< 单帧最大Service数量
#define COMM_PROTOCOL_MAX_TLV_NUM         32                 ///< 单Service最大TLV数量
#define COMM_PROTOCOL_MAX_TLV_VALUE_SIZE  256                ///< 单个TLV最大Value长度

/// TLV结构定义
typedef struct {
    uint16_t type;          ///< Type ID
    uint16_t length;        ///< Value数据长度
    uint8_t  value[];       ///< 柔性数组成员，实际数据（动态长度）
} comm_protocol_tlv_t;

/// TLV节点（用于链表或数组管理）
typedef struct {
    uint16_t type;          ///< Type ID
    uint16_t length;        ///< Value数据长度
    uint8_t  *value;        ///< 指向Value数据的指针
} comm_protocol_tlv_node_t;

/// Service结构定义（带柔性数组）
typedef struct {
    uint16_t svr_id;        ///< Service ID
    uint16_t srv_length;    ///< Service数据长度（包含所有TLV的总长度）
    uint8_t  tlvs[];        ///< 柔性数组成员，存储多个TLV包的数据
} comm_protocol_service_t;

/// Service节点（用于动态管理多个TLV）
typedef struct {
    uint16_t svr_id;                                           ///< Service ID
    uint16_t srv_length;                                       ///< Service数据长度
    uint8_t *tlvs;                                             ///< TLV数组
} comm_protocol_service_node_t;

/// Payload节点（用于动态管理多个Service）
typedef struct {
    uint16_t service_count;                                         ///< Service数量
    comm_protocol_service_node_t services[COMM_PROTOCOL_MAX_SERVICE_NUM]; ///< Service数组
} comm_protocol_payload_node_t;

/// 完整的通讯帧结构（用于数据流解析）
typedef struct {
    uint16_t sof;             ///< Start Of Frame: 0x5C5C
    uint16_t payload_length; ///< Payload字段的长度（不包含Header和Footer）
    uint8_t  payload[];      ///< 柔性数组成员，Payload数据
    // uint16_t checksum;      ///< CRC16校验和，覆盖Header和Payload域
} comm_protocol_frame_t;

/// 完整的通讯帧节点（用于构建和解析）
typedef struct {
    uint16_t sof;           ///< Start Of Frame: 0x5C5C
    uint16_t payload_length;        ///< Payload字段的长度（不包含Header和Footer）
    comm_protocol_payload_node_t  payload;    ///< Payload节点
    uint16_t checksum;      ///< CRC16校验和，覆盖Header和Payload域
} comm_protocol_frame_node_t;

/// 协议帧构建器
typedef struct {
    uint16_t  payload_size;      /// 帧总大小
    comm_protocol_frame_t *frame;
} comm_protocol_builder_t;

/// 服务节点包装器
typedef struct {
    comm_protocol_builder_t *builder;      /// 协议帧构建器
    comm_protocol_service_t *service;        /// 服务帧
} comm_protocol_service_node_wrapper_t;

/// 服务ID
typedef enum {
    COMM_PROTOCOL_SERVICE_ID_NONE = 0,
    COMM_PROTOCOL_ALGO_SERVICE = 0,
    COMM_PROTOCOL_SENSORS_SERVICE = 1,
    COMM_PROTOCOL_BREAST_PUMP_SERVICE = 2,
    COMM_PROTOCOL_IOT_SERVICE = 3,
    COMM_PROTOCOL_SYSTEM_CONFIG_SERVICE = 4,
    COMM_PROTOCOL_SYSTEM_STATE_SERVICE = 5,
    COMM_PROTOCOL_TESTS_SERVICE = 6,
    COMM_PROTOCOL_TRACKING_DATA_SERVICE = 7,

    COMM_PROTOCOL_SERVICE_ID_RSP = 0xFF00, // 应答服务ID
} comm_protocol_service_id_e;

#ifdef __cplusplus
}
#endif
#endif // __COMM_PROTOCOL_DEFS_H__ 