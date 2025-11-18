#ifndef __APP_PROTOCOL_PARSE_H__
#define __APP_PROTOCOL_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

// Protocol core definitions
#define APP_OLD_COMM_SOP_FCF          0xAA55  // Frame header
#define APP_OLD_COMM_MAX_PACKET_LEN   128     // Maximum packet length (MTU=128)
#define APP_OLD_COMM_HEARTBEAT_INTERVAL 30000 // Heartbeat interval (30 seconds)
#define APP_OLD_COMM_RX_QUEUE_SIZE    512     // Receive queue size
#define APP_OLD_COMM_FRAME_TIMEOUT_MS 2000     // Frame timeout (200ms)

// Command type definitions
typedef enum {
    APP_OLD_COMM_CT_REQ    = 0x00,  // Request
    APP_OLD_COMM_CT_ACK    = 0x01,  // Acknowledge
    APP_OLD_COMM_CT_NACK   = 0x02,  // Negative acknowledge
    APP_OLD_COMM_CT_NOTIFY = 0x03,  // Active notification
    APP_OLD_COMM_CT_DEVICE = 0x04   // Hardware control report
} AppOldCommCmdType;

// Core command ID definitions
typedef enum {
    APP_OLD_COMM_CID_HEARTBEAT     = 0x00,  // Heartbeat packet
    APP_OLD_COMM_CID_GET_DEV_INFO  = 0xF0,  // Get device information
    APP_OLD_COMM_CID_GET_SN        = 0xF1,  // Get SN code
    APP_OLD_COMM_CID_SET_RTC       = 0xF2,  // Set RTC time
    APP_OLD_COMM_CID_SYS_CTRL      = 0xFC,  // System control
    APP_OLD_COMM_CID_GET_ENCRYPT   = 0xFD,  // Get encryption information
    APP_OLD_COMM_CID_GET_STATUS    = 0xB0,  // Get device status
    APP_OLD_COMM_CID_CTRL_DEV      = 0xC0,  // Device control
    APP_OLD_COMM_CID_SET_BIND      = 0xC1,  // Set binding status
    APP_OLD_COMM_CID_SET_HEAT      = 0xCA,  // Heating control
    APP_OLD_COMM_CID_SET_TRAVEL_LOCK = 0xCF,// Travel lock control
    APP_OLD_COMM_CID_NOTIFY_STATUS = 0xD0,  // Active status report
    APP_OLD_COMM_CID_NOTIFY_BIND   = 0xD1,  // Binding status report
    APP_OLD_COMM_CID_NOTIFY_DISCON = 0xD2   // Forced disconnection report
} AppOldCommCmdId;

// Protocol parsing result enumeration
typedef enum {
    APP_OLD_COMM_PARSE_SUCCESS = 0,       // Parse successful
    APP_OLD_COMM_PARSE_ERR_HEADER,        // Header error
    APP_OLD_COMM_PARSE_ERR_CHECKSUM,      // Checksum error
    APP_OLD_COMM_PARSE_ERR_LENGTH,        // Length error
    APP_OLD_COMM_PARSE_ERR_CMD_UNKNOWN,   // Unknown command
    APP_OLD_COMM_PARSE_ERR_INCOMPLETE,    // Incomplete data
    APP_OLD_COMM_PARSE_ERR_TIMEOUT        // Frame timeout
} AppOldCommParseResult;

// Ring queue structure
typedef struct {
    uint8_t buffer[APP_OLD_COMM_RX_QUEUE_SIZE];  // Queue buffer
    uint16_t head;                           // Head index
    uint16_t tail;                           // Tail index
    uint16_t len;                            // Current queue length
    uint32_t last_recv_time;                 // Last received data timestamp (ms)
} AppOldCommRingQueue;

void on_perpare_app_commication_protocol_init(protocol_manager_context_t *ctx);
void on_post_app_commication_protocol_init(protocol_manager_context_t *ctx);
uint8_t app_commication_protocol_parse_route(uint8_t *data, uint16_t len);
int on_app_commication_protocol_br_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);
void app_commication_send_data_via_ble(uint8_t *data, uint16_t data_len);
void app_old_comm_send_ack(AppOldCommCmdId cid, uint8_t data_len, uint8_t *data);
void app_old_comm_send_nack(AppOldCommCmdId cid, uint8_t err_code);
void app_old_comm_send_notify(AppOldCommCmdId cid, uint8_t data_len, uint8_t *data);
const char* app_old_comm_get_cmd_id_str(AppOldCommCmdId cid);

#ifdef __cplusplus
}
#endif

#endif 