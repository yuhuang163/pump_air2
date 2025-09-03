/**
 * @brief Bluetooth Service Low Energy Audio Unicast Client
 *
 * @copyright Copyright (c) 2015-20223 BES Technic.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 */
#ifndef __BT_SVC_LEA_UC_CLIENT_H__
#define __BT_SVC_LEA_UC_CLIENT_H__

#ifdef AOB_MOBILE_ENABLED
/*****************************header include********************************/
#include "bt_svc_lea_uc_client_conn_stm.h"

/******************************macro defination*****************************/
#define BTS_BT_UC_CLI_SET_BIT(data, value, bit) data = (value?(data|(1<<bit)):(data&(~(1<<bit))))
#define BTS_BT_UC_CLI_GET_BIT(data, bit)        (data&(1<<bit))

/******************************type defination******************************/
typedef enum
{
    BTS_LEA_UC_DIRECTION_TX = 0,
    BTS_LEA_UC_DIRECTION_RX,
    BTS_LEA_UC_DIRECTION_MAX,
} BTS_LEA_UC_STREAM_DIRECTION_E;

typedef struct
{
    uint16_t sample_rate;
    uint8_t  ch_num;
    uint8_t  bits_byte;
    uint16_t fream_size;
} bt_svc_lea_uc_cli_audio_param_t;

typedef struct
{
    bt_svc_lea_uc_cli_audio_param_t* playback;
    bt_svc_lea_uc_cli_audio_param_t* capture;
} bt_svc_lea_uc_cli_stream_start_t;

typedef struct
{
    int (*open)(uint8_t input_type);
    int (*close)(void);
    int (*stream_set)(bool operate);
} bt_svc_lea_uc_cli_input_func_t;

/****************************function declearation**************************/


void bt_svc_lea_uc_client_init();

void bt_svc_lea_uc_cli_event_report(bt_svc_lea_uc_cli_event_t* event);

void bts_lea_uc_client_pairing_init(uint8_t *param, int param_len);

int bts_lea_uc_cli_input_usb_func_get(bt_svc_lea_uc_cli_input_func_t * func);

void bt_svc_lea_uc_cli_stream_start(bt_svc_lea_uc_cli_stream_start_t*param);

void bt_svc_lea_uc_cli_stream_stop();

void bt_svc_lea_uc_cli_stream_check(uint8_t conn_lid);

void bt_svc_lea_uc_cli_stream_vol_set(uint8_t direction, uint32_t vol);

void bt_svc_lea_uc_cli_stream_data_tx_write(uint8_t* data, uint32_t len, uint8_t bits_byte);

void bt_svc_lea_uc_cli_stream_data_rx_read(uint8_t* data, uint32_t len);

void bt_svc_lea_uc_cli_stream_data_tx_read(uint8_t* data, uint32_t len);

void bt_svc_lea_uc_cli_stream_data_rx_write(uint8_t* data, uint32_t len);

void bt_svc_lea_offload_uc_cli_stream_data_tx_read(uint8_t* data, uint32_t len);

void bt_svc_lea_offload_uc_cli_stream_data_tx_write(uint8_t* data, uint32_t len, uint8_t bits_byte);

void bt_svc_lea_uc_cli_conn_send_event(ble_bdaddr_t* bdaddr, BLE_MOBILE_CONNECT_EVENT event,uint32_t param0,uint32_t param1);

#endif

#endif
