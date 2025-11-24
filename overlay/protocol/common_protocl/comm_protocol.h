#ifndef __COMM_PROTOCOL_H__
#define __COMM_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "comm_protocol_builder.h" // IWYU pragma: keep
#include "comm_protocol_parser.h" // IWYU pragma: keep
#include "comm_protocol_defs.h" // IWYU pragma: keep



/// 描述发布数据回调函数.
typedef int (*comm_protocol_publish_data_cb_t)(uint8_t msg_channel, uint8_t *data, uint16_t len);
/// 描述发布数据上下文.
typedef struct {
    uint16_t msg_channel;                /// 消息通道.
    uint16_t tlv_num;                    /// TLV数量.
    comm_protocol_tlv_node_t *tlvs;      /// TLV列表.
    comm_protocol_publish_data_cb_t cb;  /// 发布数据回调函数.
} comm_protocol_publish_context_t;

/// 发布数据处理接口
typedef int (*comm_protocol_publish_func)(comm_protocol_publish_context_t ctx);

/// 描述发布数据处理结构.
typedef struct {
    uint16_t service_id;                             /// 服务id
    comm_protocol_publish_func publish_func;         /// 发布数据功能函数.
} comm_protocol_publish_handle_t;

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif // __COMM_PROTOCOL_H__

