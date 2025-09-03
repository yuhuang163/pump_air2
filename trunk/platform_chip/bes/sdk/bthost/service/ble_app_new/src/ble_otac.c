/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
 *
 ****************************************************************************/
#include "bthost_logger.h"

#ifdef CFG_APP_OTAC
#include "app_ble.h"
#include "gatt_service.h"
#include "ble_otac.h"
#include "app_ble_cmd_handler.h"

#define OTAC_PREF_MTU             (512)
#define USE_128BIT_UUID 1

#if USE_128BIT_UUID

#define otac_service_uuid_128_le 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66
#define otac_character_uuid_128_le 0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77

#else //USE_128BIT_UUID

#define otac_service_uuid_16      0xFEF8
#define otac_rx_character_uuid_16 0xFEF9
#define otac_tx_character_uuid_16 0xFEFA
#endif

static uint8_t otac_prf_id = 0;
static app_otac_event_cb_t *otac_client_cb = NULL;

typedef struct {
    gatt_prf_t head;
    bool peer_write_notified;
    gatt_peer_service_t *peer_service;
    gatt_peer_character_t *peer_char[OTAC_CHAR_MAX_NUM];
} otac_prf_t;

bt_status_t ble_otac_read_req(uint16_t connhdl, uint16_t code)
{
    otac_prf_t *prf = NULL;
    gatt_peer_character_t *c = NULL;
    bt_status_t status = BT_STS_FAILED;

    bool is_desc = (code & OTAC_DESC_MASK) ? true : false;
    uint16_t desc_uuid = 0;

    prf = (otac_prf_t *)gattc_get_profile(otac_prf_id, connhdl);
    if (prf == NULL)
    {
        return status;
    }

    switch(code)
    {
        case OTAC_NTF_AND_WRITE_CHAR_DATA: {
            c = prf->peer_char[OTAC_CHAR_DATA];
        } break;
        case OTAC_RD_WR_NTF_CCC_CFG: {
            c = prf->peer_char[OTAC_CHAR_DATA];
            desc_uuid = GATT_DESC_UUID_CHAR_CLIENT_CONFIG;
        } break;
        case OTAC_RD_NTF_AND_WR_CUD: {
            c = prf->peer_char[OTAC_CHAR_DATA];
            desc_uuid = GATT_DESC_UUID_CHAR_USER_DESC;
        } break;
        default: break;
    }
    if (c == NULL)
    {
        return status;
    }

    if (!is_desc) {
        status = gattc_read_character_value(&prf->head, c);
    } else {
        status = gattc_read_descriptor_value(&prf->head, c, desc_uuid);
    }

    return status;
}

bt_status_t ble_otac_write_req(uint16_t connhdl,
                                          uint16_t code,
                                          uint8_t write_cmd,
                                          const uint8_t *value, uint16_t len)
{
    otac_prf_t *prf = NULL;
    gatt_peer_character_t *c = NULL;
    bt_status_t status = BT_STS_FAILED;

    if (code >= OTAC_CHAR_MAX_NUM || value == NULL || len == 0)
    {
        return status;
    }

    prf = (otac_prf_t *)gattc_get_profile(otac_prf_id, connhdl);
    if (prf == NULL)
    {
        return status;
    }

    c = prf->peer_char[code];
    if (c == NULL)
    {
        return status;
    }

    if(code == OTAC_CHAR_DATA)
    {
        if (write_cmd)
        {
            status = gattc_write_character_command(&prf->head, c, value, len, false);
        }
        else
        {
            status = gattc_write_character_value(&prf->head, c, value, len);
        }
    }

    return status;
}


bt_status_t ble_otac_write_desc_req(uint16_t connhdl,
                                          uint16_t code,
                                          uint8_t write_cmd,
                                          const uint8_t *value, uint16_t len)
{
    otac_prf_t *prf = NULL;
    gatt_peer_character_t *c = NULL;
    bool notify_enabled = false;
    bt_status_t status = BT_STS_FAILED;

    if (code >= OTAC_CHAR_MAX_NUM || value == NULL || len == 0)
    {
        return status;
    }

    prf = (otac_prf_t *)gattc_get_profile(otac_prf_id, connhdl);
    if (prf == NULL)
    {
        return status;
    }

    c = prf->peer_char[code];
    if (c == NULL)
    {
        return status;
    }

    if(code == OTAC_CHAR_DATA)
    {
        notify_enabled = value[0] ? true : false;
        status = gattc_write_cccd_descriptor(&prf->head, c, notify_enabled, false);
        prf->peer_write_notified = notify_enabled;
    }

    return status;
}

static void ble_otac_conn_handler(otac_prf_t *prf)
{
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);

    ble_otac_start_discover(prf->head.con_idx);

    if(otac_client_cb && otac_client_cb->otac_connected_done_cb)
    {
        otac_client_cb->otac_connected_done_cb(conidx);
    }
}

static void ble_otac_disconn_handler(otac_prf_t *prf)
{
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);

    if(otac_client_cb && otac_client_cb->otac_disconnected_done_cb)
    {
        otac_client_cb->otac_disconnected_done_cb(conidx);
    }
}

static void ble_otac_mtu_changed(otac_prf_t *prf, uint16_t mtu)
{
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);

    if(otac_client_cb && otac_client_cb->otac_mtu_exchanged_done_cb)
    {
        otac_client_cb->otac_mtu_exchanged_done_cb(conidx, mtu);
    }
}

static void ble_otac_discover_complete(otac_prf_t *prf)
{
    TRACE(0,"%s ",__func__);
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);
    uint8_t value[2] = {0x01, 00};

    ble_otac_write_desc_req(prf->head.connhdl, OTAC_CHAR_DATA, false, value, 2);

    if(otac_client_cb && otac_client_cb->otac_discover_done_cb)
    {
        otac_client_cb->otac_discover_done_cb(conidx);
    }
}

static void ble_otac_char_read_complete(otac_prf_t *prf,
                                                   uint8_t error_code,
                                                   const gatt_peer_character_t *c,
                                                   const uint8_t *value, uint16_t len)
{
    POSSIBLY_UNUSED otac_char_enum_t char_enum = OTAC_CHAR_MAX_NUM;

    if (error_code)
    {
        DEBUG_VERBOSE(0,"%s err code:%d", __func__, error_code);
    }

    if (c == prf->peer_char[OTAC_CHAR_DATA]) {
        char_enum = OTAC_CHAR_DATA;
    } else if (c == prf->peer_char[OTAC_CHAR_DATA]) {
        char_enum = OTAC_CHAR_DATA;
    }

    if(value)
    {
        DEBUG_VERBOSE(0,"%s char_enum %d read cmp:", __func__, char_enum);
        DUMP8_D(value, len);
    }
}

static void ble_otac_desc_read_complete(otac_prf_t *prf,
                                                   uint8_t error_code,
                                                   const gatt_peer_character_t *c,
                                                   uint16_t desc_uuid,
                                                   const uint8_t *value, uint16_t len)
{
    POSSIBLY_UNUSED otac_char_enum_t char_enum = OTAC_CHAR_MAX_NUM;
    POSSIBLY_UNUSED otac_desc_enum_t desc_enum = OTAC_DESC_MAX_NUM;

    if (error_code)
    {
        DEBUG_VERBOSE(0,"%s err code:%d", __func__, error_code);
    }

    if (c == prf->peer_char[OTAC_CHAR_DATA])
    {
        char_enum = OTAC_CHAR_DATA;
        if(desc_uuid == GATT_DESC_UUID_CHAR_CLIENT_CONFIG)
        {
            desc_enum = OTAC_DESC_RX_CFG;
        }
        else if(desc_uuid == GATT_DESC_UUID_CHAR_USER_DESC)
        {
            desc_enum = OTAC_DESC_CUD;
        }
    }
    else if (c == prf->peer_char[OTAC_CHAR_DATA])
    {
        char_enum = OTAC_CHAR_DATA;
        if(desc_uuid == GATT_DESC_UUID_CHAR_USER_DESC)
        {
            desc_enum = OTAC_DESC_CUD;
        }
    }

    if(value)
    {
        DEBUG_VERBOSE(0,"%s char_enum %d desc_enmu %d read cmp:", __func__, char_enum, desc_enum);
        DUMP8_D(value, len);
    }
}

static void ble_otac_char_write_complete(otac_prf_t *prf,
                                                    uint8_t error_code,
                                                    const gatt_peer_character_t *c)
{
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);

    if (error_code)
    {
        DEBUG_VERBOSE(0, "%s err_code %d", __func__, error_code);
    }

    if(otac_client_cb && otac_client_cb->otac_tx_done_cb)
    {
        otac_client_cb->otac_tx_done_cb(conidx, error_code);
    }
}

static void ble_otac_desc_write_complete(otac_prf_t *prf,
                                                    uint8_t error_code,
                                                    const gatt_peer_character_t *c,
                                                    uint16_t desc_uuid)
{
    DEBUG_VERBOSE(0, "%s err_code %d", __func__, error_code);
}

static void ble_otac_rx_data_received(otac_prf_t *prf,
                                                 const gatt_peer_character_t *c,
                                                 const uint8_t *value, uint16_t len)
{
    uint8_t conidx = gap_zero_based_conidx(prf->head.con_idx);

    // DEBUG_VERBOSE(0, "%s len %d", __func__, len);
    // DUMP8_D(value, len);
    if(otac_client_cb && otac_client_cb->otac_data_received_cb)
    {
        otac_client_cb->otac_data_received_cb(conidx, value, len);
    }
}

void ble_otac_start_discover(uint8_t conidx)
{
    TRACE(0,"%s ",__func__);
    uint16_t connhdl = app_ble_get_conhdl_from_conidx(conidx);

    otac_prf_t *prf = (otac_prf_t *)gattc_get_profile(otac_prf_id, connhdl);
    if (prf == NULL)
    {
        return;
    }

#if USE_128BIT_UUID
    uint8_t dp_service_uuid_le[16] = {otac_service_uuid_128_le};
    gattc_discover_service(&prf->head, 0, dp_service_uuid_le);
#else
    gattc_discover_service(&prf->head, otac_service_uuid_16, NULL);
#endif

    return;
}

static bool ble_otac_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param)
{
    otac_prf_t *otac = (otac_prf_t *)prf;

    switch (event)
    {
        case GATT_PROF_EVENT_OPENED:
        {
            ble_otac_conn_handler(otac);
            break;
        }
        case GATT_PROF_EVENT_CLOSED:
        {
            ble_otac_disconn_handler(otac);
            break;
        }
        case GATT_PROF_EVENT_MTU_CHANGED:
        {
            ble_otac_mtu_changed(otac, param.mtu_changed->mtu);
            break;
        }
        case GATT_PROF_EVENT_SERVICE_CHANGED:
        {
            // ble_otac_conn_handler(otac);
            // ble_otac_start_discover(otac->head.con_idx);
            break;
        }
        case GATT_PROF_EVENT_SERVICE:
        {
            gatt_profile_service_t *p = param.service;
            gatt_peer_service_t *s = p->service;
            if (p->error_code != ATT_ERROR_NO_ERROR)
            {
                break;
            }
            otac->peer_service = s;
#if USE_128BIT_UUID
            gatt_char_uuid_t gap_chars[] = {
                    {{otac_character_uuid_128_le}, true},
                    {{otac_character_uuid_128_le}, true},
                };

            gattc_discover_multi_128_characters(prf, s, gap_chars, ARRAY_SIZE(gap_chars));
#else
            uint16_t gap_chars[] = {
                    otac_rx_character_uuid_16,
                    otac_tx_character_uuid_16,
                };

            gattc_discover_multi_characters(prf, s, gap_chars, sizeof(gap_chars)/sizeof(uint16_t));
#endif
            break;
        }
        case GATT_PROF_EVENT_CHARACTER:
        {
            gatt_profile_character_t *p = param.character;
            gatt_peer_character_t *c = p->character;
            if (p->discover_idx < OTAC_CHAR_MAX_NUM)
            {
                if (p->error_code == ATT_ERROR_NO_ERROR)
                {
                    otac->peer_char[p->discover_idx] = c;
                }
                else
                {
                    otac->peer_char[p->discover_idx] = NULL;
                }
            }
            if (p->discover_cmpl)
            {
                ble_otac_discover_complete(otac);
            }
            break;
        }
        case GATT_PROF_EVENT_CHAR_READ_RSP:
        {
            gatt_profile_char_read_rsp_t *p = param.char_read_rsp;
            ble_otac_char_read_complete(otac, p->error_code, p->character, p->value, p->value_len);
            break;
        }
        case GATT_PROF_EVENT_DESC_READ_RSP:
        {
            gatt_profile_desc_read_rsp_t *p = param.desc_read_rsp;
            ble_otac_desc_read_complete(otac, p->error_code, p->character, p->desc_uuid, p->value, p->value_len);
            break;
        }
        case GATT_PROF_EVENT_CHAR_WRITE_RSP:
        {
            gatt_profile_char_write_rsp_t *p = param.char_write_rsp;
            ble_otac_char_write_complete(otac,p->error_code, p->character);
            break;
        }
        case GATT_PROF_EVENT_DESC_WRITE_RSP:
        {
            gatt_profile_desc_write_rsp_t *p = param.desc_write_rsp;
            ble_otac_desc_write_complete(otac, p->error_code, p->character, p->desc_uuid);
            break;
        }
        case GATT_PROF_EVENT_NOTIFY:
        {
            gatt_profile_recv_notify_t *ntf = param.notify;
            if (ntf->character == otac->peer_char[OTAC_CHAR_DATA])
            {
                ble_otac_rx_data_received(otac, ntf->character, ntf->value, ntf->value_len);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

void ble_otac_init(void)
{
    gattc_cfg_t prf_cfg = {0};
    prf_cfg.prf_size = sizeof(otac_prf_t);
    prf_cfg.preferred_mtu = OTAC_PREF_MTU;
    otac_prf_id = gattc_register_profile(ble_otac_callback, &prf_cfg);
}

void app_otac_register_callback(app_otac_event_cb_t *callback)
{
    otac_client_cb = callback;
}

void app_otac_control_notification(uint8_t conidx, bool isEnable)
{
    uint16_t connhdl = app_ble_get_conhdl_from_conidx(conidx);
    uint8_t value[2] = {0};
    otac_prf_t *prf = (otac_prf_t *)gattc_get_profile(otac_prf_id, connhdl);
    if (prf == NULL)
    {
        return;
    }

    if (isEnable != prf->peer_write_notified)
    {
        value[0] = isEnable;
        ble_otac_write_desc_req(connhdl, OTAC_CHAR_DATA, false, value, 2);
    }
}

bool app_otac_send_data_via_write_command(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    uint16_t connhdl = app_ble_get_conhdl_from_conidx(conidx);
    int ret = ble_otac_write_req(connhdl, OTAC_CHAR_DATA, true, ptrData, length);
    if(ret == BT_STS_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_otac_send_data_via_write_request(uint8_t conidx, uint8_t* ptrData, uint32_t length)
{
    uint16_t connhdl = app_ble_get_conhdl_from_conidx(conidx);
    int ret = ble_otac_write_req(connhdl, OTAC_CHAR_DATA, false, ptrData, length);
    if(ret == BT_STS_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}


#endif //CFG_APP_OTAC
