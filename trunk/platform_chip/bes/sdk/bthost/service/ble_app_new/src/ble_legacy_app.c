/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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

#if (BLE_LEGACY_APP_ENABLED)

#include "bluetooth.h"

#include "app_ble.h"
#include "app_bt.h"
#include "ble_legacy_app.h"

#include "nvrecord_extension.h"
#include "nvrecord_ble.h"
#include "nvrecord_bt.h"

#include "bluetooth_nv_mgr.h"

#include "gap_service.h"
#include "gap_i.h"
#include "gaf_service.h"

#if defined(IBRT)
#include "bts_core_if.h"
#else
#error Only for TWS LEA with IBRT enabled
#endif

/* DEFINITIONS */
static const bt_bdaddr_t zero_addr = {{0}};

#define BLE_LEGACY_APP_ADV_REFRESH_PERIOD_MS (500)
#define BLE_LEGACY_APP_IS_ADDR_ZERO(p_addr)  (p_addr && (memcmp(p_addr, &zero_addr, sizeof(zero_addr)) == 0))

/* ENUMERATIONS */
enum ble_legacy_app_tws_sync_op
{
    BLE_LEGACY_APP_TWS_SYNC_OP_BT_RECORD = 0,
    BLE_LEGACY_APP_TWS_SYNC_OP_LE_RECORD = 1,
    BLE_LEGACY_APP_TWS_SYNC_OP_SYNC_LE = 2,
};

/* STRUCTURES */
typedef struct ble_legacy_app_info
{
    uint8_t addr[6];
    uint8_t ltk[16];
    uint8_t irk[16];
    uint8_t local_irk[16];
    uint8_t local_rand[8];
    uint16_t local_ediv;
    uint8_t bond_info_bf;
    uint8_t padding[3];
} ble_legacy_app_sync_t;

struct ble_legacy_app_information
{
    // Adv enable
    bool adv_enable;
    // IRK for app
    uint8_t irk[GAP_KEY_LEN];
    // Adv handle for demo app adv
    uint8_t adv_hdl;
    // Adv callback
    app_ble_adv_activity_func adv_activity_func;
    // Demo app profile lid
    uint8_t prf_lid;
    // Specific service databse hash
    uint8_t hash[GAP_KEY_LEN];
    // Refresh timer app
    osTimerId adv_refresh_timer;
    // Refresh idx
    uint8_t adv_refresh_idx;
    // Addr to gen rpa
    bt_bdaddr_t peer_addr_gen_rpa;
} *p_legacy_app_info = NULL;

/* DECLARATIONS */
static bool ble_legacy_app_adv_activity_prepare(ble_adv_activity_t *adv);
static void ble_legacy_app_conn_estb_thread_handler(uint8_t conidx);
static void ble_legacy_app_visible_svc_database_hash_gen_cmp(void *priv, int error_code, const uint8_t *hash);
static void ble_legacy_app_smp_requirements_modify(uint16_t connhdl, ble_smp_require_t *p_requirements);
static void ble_legacy_app_add_ble_record_handler(uint16_t connhdl, BleDevicePairingInfo *p_record);
static void ble_legacy_app_get_specific_irk_ia_handler(uint16_t connhdl, uint8_t **p_irk, bt_bdaddr_t **p_ia);
#if BLE_GATT_CLIENT_SUPPORT
static bool ble_legacy_app_gatt_client_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
#endif /* BLE_GATT_CLIENT_SUPPORT */
static void ble_legacy_app_get_specific_hash_handler(uint16_t connhdl, uint8_t **pp_hash);
static void ble_legacy_app_global_handler_ind(ble_event_t *event, void *output);
static void ble_legacy_app_refresh_timer_handler(void const *param);
static void ble_legacy_app_tws_sync_record_info(bool is_bt, uint8_t *p_buf, uint16_t len);

osTimerDef(BLE_LEGACY_APP_REFRESH_TIMER, ble_legacy_app_refresh_timer_handler);

/* FUNCTIONS */
int ble_legacy_app_init(void)
{
    DEBUG_INFO(1, "%s", __func__);

    if (p_legacy_app_info != NULL)
    {
        return BT_STS_SUCCESS;
    }

    p_legacy_app_info = (struct ble_legacy_app_information *)bes_bt_buf_malloc(sizeof(struct ble_legacy_app_information));

    if (p_legacy_app_info == NULL)
    {
        return BT_STS_NO_RESOURCES;
    }

    memset(p_legacy_app_info, 0, sizeof(*p_legacy_app_info));
    // Demo app adv refresh timer
    p_legacy_app_info->adv_refresh_timer = osTimerCreate(osTimer(BLE_LEGACY_APP_REFRESH_TIMER), osTimerPeriodic, NULL);

    if (p_legacy_app_info->adv_refresh_timer == NULL)
    {
        DEBUG_VERBOSE(0, "%s adv refresh timer create failed", __func__);
        return BT_STS_NO_RESOURCES;
    }

#if BLE_GATT_CLIENT_SUPPORT
    // Demo app profile
    p_legacy_app_info->prf_lid = gattc_register_profile(ble_legacy_app_gatt_client_callback, NULL);
#endif /* BLE_GATT_CLIENT_SUPPORT */
    // SMP requirements modify
    app_ble_smp_require_modify_callback_register(ble_legacy_app_smp_requirements_modify);
    // IRK/IA speicifc for handle connection
    app_ble_smp_get_specifc_irk_ia_callback_register(ble_legacy_app_get_specific_irk_ia_handler);
    // Add ble record to nv before handler
    app_ble_gap_add_record_modify_callback_register(ble_legacy_app_add_ble_record_handler);
    // Hash value modify
    app_ble_gatt_get_specifc_hash_callback_register(ble_legacy_app_get_specific_hash_handler);
    // LE event
    app_ble_core_register_global_handler_ind(ble_legacy_app_global_handler_ind);

    return BT_STS_SUCCESS;
}

int ble_legacy_app_deinit(void)
{
    if (p_legacy_app_info == NULL)
    {
        return BT_STS_SUCCESS;
    }

#if BLE_GATT_CLIENT_SUPPORT
    // Demo app profile
    gattc_unregister_profile(p_legacy_app_info->prf_lid);
#endif /* BLE_GATT_CLIENT_SUPPORT */
    // SMP requirements modify
    app_ble_smp_require_modify_callback_register(NULL);
    // IRK/IA speicifc for handle connection
    app_ble_smp_get_specifc_irk_ia_callback_register(NULL);
    // Add ble record to nv before handler
    app_ble_gap_add_record_modify_callback_register(NULL);
    // Get nv record callback
    app_ble_gap_get_specifc_record_callback_register(NULL);
    // Hash value modify
    app_ble_gatt_get_specifc_hash_callback_register(NULL);
    // Demo app adv deregister
    app_ble_register_advertising(p_legacy_app_info->adv_hdl, NULL);
    // LE event deregister
    app_ble_core_unregister_global_handler_ind(ble_legacy_app_global_handler_ind);

    bes_bt_buf_free(p_legacy_app_info);
    p_legacy_app_info = NULL;

    return BT_STS_SUCCESS;
}

ble_adv_activity_t *ble_legacy_app_register_advertising(uint8_t adv_handle, app_ble_adv_activity_func adv_activity_func)
{
    if (p_legacy_app_info != NULL)
    {
        p_legacy_app_info->adv_hdl = adv_handle;
        p_legacy_app_info->adv_activity_func = adv_activity_func;
        return app_ble_register_advertising(adv_handle, ble_legacy_app_adv_activity_prepare);
    }
    else
    {
        return app_ble_register_advertising(adv_handle, adv_activity_func);
    }
}

static void ble_legacy_app_connection_established_handler(uint8_t conidx)
{
    if (p_legacy_app_info == NULL)
    {
        return;
    }

    bt_thread_call_func_1(ble_legacy_app_conn_estb_thread_handler,
                          bt_fixed_param(conidx));
}

static void ble_legacy_app_smp_pairing_completed_handler(uint8_t conidx)
{
    ble_adv_activity_t *lea_adv_actv = NULL;
    gap_conn_item_t *conn = gap_get_conn_item(gap_zero_based_ble_conidx_as_hdl(conidx));

    if (conn == NULL)
    {
        DEBUG_VERBOSE(0, "%s err conidx = %d", __func__, conidx);
        return;
    }

    lea_adv_actv = app_ble_get_advertising_by_user(USER_BLE_AUDIO);

    if (lea_adv_actv == NULL)
    {
        DEBUG_VERBOSE(0, "%s err no lea adv", __func__);
        return;
    }

    if (app_ibrt_if_is_nv_master() == false &&
            bt_callback_cfg_le_audio_enabled() == true &&
            (conn->adv_handle == lea_adv_actv->adv_handle))
    {
        DEBUG_VERBOSE(1, "nv slave with lea adv, do not sync smp info");
        return;
    }

    BleDevicePairingInfo pair_info = {0};
    BLE_ADDR_INFO_T peer_le_addr = {.addr_type = (conn->sec.peer_type & 0x01)};
    memcpy(peer_le_addr.addr, &conn->sec.peer_addr, sizeof(bt_bdaddr_t));
    if (nv_record_blerec_get_paired_dev_from_addr(&pair_info, &peer_le_addr))
    {
        if (bts_tws_if_is_tws_link_connected() == false)
        {
            pair_info.bond_info_bf |= CO_BIT_MASK(BONDED_SMP_NOT_SYNCED);
            bluetooth_nv_mgr_ble_record_add(BLE_NV_REC_ADD_CTKD_OVER_BREDR, &pair_info);
            return;
        }

        ble_legacy_app_tws_sync_record_info(false, (uint8_t *)&pair_info, sizeof(pair_info));
    }
    else
    {
        DEBUG_VERBOSE(1, "%s err can not find ble record", __func__);
    }
}

static void ble_legacy_app_smp_key_ctkd_derived_handler(bool is_linkkey, const bt_bdaddr_t *ia_addr)
{
    if (is_linkkey == true)
    {
        btif_device_record_t record = {{{0}}, 0};

        if (nv_record_ddbrec_find(ia_addr, &record) != BT_STS_SUCCESS)
        {
            DEBUG_VERBOSE(0, "%s invalid bt record", __func__);
            return;
        }

        ble_legacy_app_tws_sync_record_info(true, (uint8_t *)&record, sizeof(record));
    }
    else
    {
        BleDevicePairingInfo pair_info = {0};
        BLE_ADDR_INFO_T addr = {{0}, 0};

        memcpy(addr.addr, ia_addr, sizeof(*ia_addr));

        if (nv_record_blerec_get_paired_dev_from_addr(&pair_info, (BLE_ADDR_INFO_T *)&addr) == false)
        {
            DEBUG_VERBOSE(0, "%s invalid le record", __func__);
            return;
        }

        if (bts_tws_if_is_tws_link_connected() == false)
        {
            pair_info.bond_info_bf |= CO_BIT_MASK(BONDED_SMP_NOT_SYNCED);
            bluetooth_nv_mgr_ble_record_add(BLE_NV_REC_ADD_CTKD_OVER_BREDR, &pair_info);
            return;
        }

        ble_legacy_app_tws_sync_record_info(false, (uint8_t *)&pair_info, sizeof(pair_info));
    }
}

static void ble_legacy_app_add_ble_record_handler(uint16_t connhdl, BleDevicePairingInfo *p_record)
{
    gap_conn_item_t *conn = gap_get_conn_item(connhdl);

    uint8_t local_irk[GAP_KEY_LEN] = {0};

    if (conn == NULL)
    {
        btif_device_record_t record = {{{0}}, 0};
        if (nv_record_ddbrec_find(
                    (const bt_bdaddr_t *)p_record->peer_addr.addr, &record) != BT_STS_SUCCESS)
        {
            DEBUG_VERBOSE(1, "Error can not store local irk!!!");
            DUMP8_D(p_record->peer_addr.addr, 6);
            memcpy(p_record->LOCAL_IRK, gap_get_local_irk(), GAP_KEY_LEN);
        }
        else
        {
            memcpy(local_irk, record.linkKey, GAP_KEY_LEN);
            memcpy(p_record->LOCAL_IRK, local_irk, GAP_KEY_LEN);
            DEBUG_VERBOSE(1, "BT CTKD Local IRK:");
            DUMP8_D(local_irk, GAP_KEY_LEN);
        }
    }
    else
    {
        memcpy(local_irk, p_record->LTK, GAP_KEY_LEN);
        memcpy(p_record->LOCAL_IRK, local_irk, GAP_KEY_LEN);
        DEBUG_VERBOSE(1, "BLE SMP Local IRK:");
        DUMP8_D(local_irk, GAP_KEY_LEN);
    }
}

void ble_legacy_app_tws_sync_info(void)
{
    uint8_t sync_info_buf[sizeof(ble_legacy_app_sync_t) * BLE_RECORD_NUM + 1] = {BLE_LEGACY_APP_TWS_SYNC_OP_SYNC_LE};
    ble_legacy_app_sync_t *p_sync_info = (ble_legacy_app_sync_t *)(sync_info_buf + 1);
    memset(p_sync_info, 0, sizeof(sync_info_buf) - 1);
    uint8_t i = 0;
    uint8_t j = 0;
    NV_RECORD_PAIRED_BLE_DEV_INFO_T *dev_info = nv_record_blerec_get_ptr();
    BleDeviceinfo *record = NULL;
    uint32_t count = dev_info->saved_list_num;
    for (i = 0; i < count; i++)
    {
        record = dev_info->ble_nv + i;

        if ((record->pairingInfo.bond_info_bf & CO_BIT_MASK(BONDED_SMP_NOT_SYNCED)) &&
                record->pairingInfo.peer_addr.addr_type == 0 &&
                record->pairingInfo.LTK[0] != 0 &&
                record->pairingInfo.IRK[0] != 0)
        {
            memcpy(p_sync_info[j].addr, record->pairingInfo.peer_addr.addr, 6);
            if (record->pairingInfo.bond_info_bf & CO_BIT_MASK(BONDED_SECURE_CONNECTION))
            {
                memcpy(p_sync_info[j].ltk, record->pairingInfo.LTK, GAP_KEY_LEN);
            }
            else
            {
                memcpy(p_sync_info[j].ltk, record->pairingInfo.LOCAL_LTK, GAP_KEY_LEN);
            }
            memcpy(p_sync_info[j].irk, record->pairingInfo.IRK, GAP_KEY_LEN);
            memcpy(p_sync_info[j].local_irk, record->pairingInfo.LOCAL_IRK, GAP_KEY_LEN);
            p_sync_info[j].bond_info_bf = record->pairingInfo.bond_info_bf;
            p_sync_info[j].local_ediv = record->pairingInfo.LOCAL_EDIV;
            memcpy(p_sync_info[j].local_rand, record->pairingInfo.LOCAL_RANDOM, 8);
            j++;
        }
    }

    tws_ctrl_send_cmd(APP_TWS_CMD_SEND_DEMO_APP_INFO, (uint8_t *)sync_info_buf, j * sizeof(ble_legacy_app_sync_t) + 1);
}

static void ble_legacy_app_tws_sync_record_info(bool is_bt, uint8_t *p_buf, uint16_t len)
{
    uint8_t *p_info = bes_bt_buf_malloc(len + 1);

    if (p_info == NULL)
    {
        return;
    }

    p_info[0] = is_bt ? BLE_LEGACY_APP_TWS_SYNC_OP_BT_RECORD : BLE_LEGACY_APP_TWS_SYNC_OP_LE_RECORD;
    memcpy(p_info + 1, p_buf, len);

    tws_ctrl_send_cmd(APP_TWS_CMD_SEND_DEMO_APP_INFO, p_info, len + 1);

    bes_bt_buf_free(p_info);
}

void ble_legacy_app_tws_state_changed(bool connected)
{
    DEBUG_VERBOSE(0, "%s nv %d connected %d", __func__, app_ibrt_if_is_nv_master(), connected);

    if (connected)
    {
        p_legacy_app_info->adv_enable = app_ibrt_if_is_nv_master();
        ble_legacy_app_tws_sync_info();
    }
    else
    {
        p_legacy_app_info->adv_enable = true;
    }

    app_ble_refresh_adv_state_generic();
}

void ble_legacy_app_tws_role_switch_start(uint8_t curr_ui_role)
{
}

void ble_legacy_app_tws_role_switch_cmp(uint8_t curr_ui_role)
{
    if (bts_tws_if_is_tws_link_connected() == false)
    {
        p_legacy_app_info->adv_enable = true;
    }

    app_ble_refresh_adv_state_generic();
}

void ble_legacy_app_tws_role_update(uint8_t curr_ui_role)
{
}

static void ble_legacy_app_tws_sync_bt_record(uint8_t *p_info, uint16_t len)
{
    DEBUG_VERBOSE(1, "%s len = %d", __func__, len);

    if (len >= sizeof(btif_device_record_t))
    {
        btif_device_record_t *record = (btif_device_record_t *)(p_info);
        bluetooth_nv_mgr_bt_record_add(BT_NV_REC_ADD_CTKD_OVER_LE, record);
    }
}

static void ble_legacy_app_tws_sync_le_record(uint8_t *p_info, uint16_t len)
{
    DEBUG_VERBOSE(1, "%s len = %d", __func__, len);

    if (len >= sizeof(BleDevicePairingInfo))
    {
        BleDevicePairingInfo *record = (BleDevicePairingInfo *)(p_info);

        bluetooth_nv_mgr_ble_record_add(BLE_NV_REC_ADD_CTKD_OVER_BREDR, record);

        app_ble_add_devices_info_to_resolving();
    }
}

static void ble_legacy_app_tws_sync_le_info(uint8_t *p_info, uint16_t len)
{
    BleDevicePairingInfo record;

    uint8_t size = len / sizeof(ble_legacy_app_sync_t);
    uint8_t i = 0;

    ble_legacy_app_sync_t *p_info_list = (ble_legacy_app_sync_t *)p_info;

    while (i < size)
    {
        memset(&record, 0, sizeof(record));

        record.peer_addr.addr_type = 0;
        record.enc_key_size = GAP_KEY_LEN;
        record.bond_info_bf |= (p_info_list[i].bond_info_bf & (~CO_BIT_MASK(BONDED_SMP_NOT_SYNCED)));
        memcpy(record.peer_addr.addr, p_info_list[i].addr, sizeof(bt_bdaddr_t));
        memcpy(record.IRK, p_info_list[i].irk, GAP_KEY_LEN);
        memcpy(record.LOCAL_IRK, p_info_list[i].local_irk, GAP_KEY_LEN);
        if (record.bond_info_bf & CO_BIT_MASK(BONDED_SECURE_CONNECTION))
        {
            memcpy(record.LTK, p_info_list[i].ltk, GAP_KEY_LEN);
        }
        else
        {
            memcpy(record.LOCAL_LTK, p_info_list[i].ltk, GAP_KEY_LEN);
        }
        record.LOCAL_EDIV = p_info_list[i].local_ediv;
        memcpy(record.LOCAL_RANDOM, p_info_list[i].local_rand, 8);
        bluetooth_nv_mgr_ble_record_add(BLE_NV_REC_ADD_CTKD_OVER_BREDR, &record);
        i++;
    }

    if (size > 0)
    {
        app_ble_add_devices_info_to_resolving();
    }
}

void ble_legacy_app_tws_sync_info_receive_handler(uint8_t *p_info, uint16_t len)
{
    uint8_t op = *p_info;

    switch (op)
    {
        case BLE_LEGACY_APP_TWS_SYNC_OP_BT_RECORD:
        {
            ble_legacy_app_tws_sync_bt_record(p_info + 1, len - 1);
        }
        break;
        case BLE_LEGACY_APP_TWS_SYNC_OP_LE_RECORD:
        {
            ble_legacy_app_tws_sync_le_record(p_info + 1, len - 1);
        }
        break;
        case BLE_LEGACY_APP_TWS_SYNC_OP_SYNC_LE:
        {
            ble_legacy_app_tws_sync_le_info(p_info + 1, len - 1);
        }
        break;
        default:
            break;
    }
}

static void ble_legacy_app_smp_requirements_modify(uint16_t connhdl, ble_smp_require_t *p_requirements)
{
    gap_conn_item_t *conn = gap_get_conn_item(connhdl);
    ble_adv_activity_t *lea_adv_actv = app_ble_get_advertising_by_user(USER_BLE_AUDIO);

    if (lea_adv_actv == NULL)
    {
        DEBUG_VERBOSE(0, "%s err no lea adv", __func__);
        return;
    }

    if (conn == NULL || conn->conn_type == HCI_CONN_TYPE_BT_ACL)
    {
        return;
    }

    if (app_ibrt_if_is_nv_master() == false)
    {
        if (bt_callback_cfg_le_audio_enabled() == true &&
                (conn->adv_handle == lea_adv_actv->adv_handle))
        {
            DEBUG_VERBOSE(1, "nv slave with lea adv, can not do ctkd");
            p_requirements->init_key_dist &= ~GAP_KDIST_LINKKEY;
            p_requirements->resp_key_dist &= ~GAP_KDIST_LINKKEY;
        }
        else if (bts_tws_if_is_tws_link_connected() == false)
        {
            p_requirements->init_key_dist |= GAP_KDIST_LINKKEY;
            p_requirements->resp_key_dist |= GAP_KDIST_LINKKEY;
        }
    }
    else
    {
        p_requirements->init_key_dist |= GAP_KDIST_LINKKEY;
        p_requirements->resp_key_dist |= GAP_KDIST_LINKKEY;
    }

    return;
}

static void ble_legacy_app_get_specific_irk_ia_handler(uint16_t connhdl, uint8_t **p_irk, bt_bdaddr_t **p_ia)
{
    gap_conn_item_t *conn = gap_get_conn_item(connhdl);
    ble_adv_activity_t *lea_adv_actv = app_ble_get_advertising_by_user(USER_BLE_AUDIO);

    if (lea_adv_actv == NULL)
    {
        DEBUG_VERBOSE(0, "%s err no lea adv", __func__);
        return;
    }

    if (conn == NULL)
    {
        return;
    }

    DEBUG_VERBOSE(1, "%s %p %d", __func__, conn, conn->conn_type);

    if (p_irk != NULL)
    {
        if (conn != NULL)
        {
            if (conn->conn_type == HCI_CONN_TYPE_BT_ACL)
            {
                btif_device_record_t record = {{{0}}, 0};
                bt_bdaddr_t *bt_addr = (bt_bdaddr_t *)app_bt_get_addr_byhandle(conn->connhdl);

                if (nv_record_ddbrec_find(bt_addr, &record) != BT_STS_SUCCESS)
                {
                    DEBUG_VERBOSE(1, "Error can not send local irk!!!");
                }
                else
                {
                    memcpy(p_legacy_app_info->irk, record.linkKey, GAP_KEY_LEN);
                    *p_irk = p_legacy_app_info->irk;
                }
            }
            else
            {
                memcpy(p_legacy_app_info->irk, conn->sec.ltk, GAP_KEY_LEN);
                *p_irk = p_legacy_app_info->irk;
            }
        }
        else
        {
            DEBUG_VERBOSE(1, "Error conn null can not send local irk!!!");
        }
    }

    if (p_ia != NULL)
    {
        if (conn != NULL)
        {
            if (bt_callback_cfg_le_audio_enabled() == true &&
                    (conn->adv_handle == lea_adv_actv->adv_handle))
            {
                *p_ia = (bt_bdaddr_t *)gap_hci_le_address();
            }
            else
            {
                *p_ia = (bt_bdaddr_t *)gap_hci_bt_address();
            }
        }
        else
        {
            DEBUG_VERBOSE(1, "Error conn null can not send local ia!!!");
        }
    }
}

static void ble_legacy_app_global_handler_ind(ble_event_t *event, void *output)
{
    const ble_event_handled_t *p = &(event->p);

    switch (event->evt_type)
    {
        case BLE_LINK_CONNECTED_EVENT:
        {
            ble_legacy_app_connection_established_handler(p->connect_handled.conidx);
        }
        break;
        case BLE_SMP_PAIRING_CMP_EVENT:
        {
            ble_legacy_app_smp_pairing_completed_handler(p->pair_cmp_handled.conidx);
        }
        break;
        case BLE_SMP_CTKD_KEY_DERIVED_EVENT:
        {
            ble_legacy_app_smp_key_ctkd_derived_handler(
                p->ctkd_key_derived.is_linkkey, &p->ctkd_key_derived.peer_addr);
        }
        break;
        default:
            break;
    }
}

static void ble_legacy_app_get_specific_hash_handler(uint16_t connhdl, uint8_t **pp_hash)
{
    const uint8_t empty_hash[GAP_KEY_LEN] = {0};
    gap_conn_item_t *conn = gap_get_conn_item(connhdl);

    if (conn != NULL && bt_callback_cfg_le_audio_enabled() == false &&
            (memcmp(p_legacy_app_info->hash, empty_hash, GAP_KEY_LEN) != 0))
    {
        *pp_hash = p_legacy_app_info->hash;
    }
}

static void ble_legacy_app_conn_estb_thread_handler(uint8_t conidx)
{
    const uint8_t empty_hash[GAP_KEY_LEN] = {0};
    gap_conn_item_t *conn = gap_get_conn_item(app_ble_get_conhdl_from_conidx(conidx));

    if (conn == NULL)
    {
        return;
    }

    if (bt_callback_cfg_le_audio_enabled() == false)
    {
        // Control LEA service no visible
        gaf_prf_control_all_service(conn->connhdl, false);
        // Generate database hash when control service done
        if (memcmp(p_legacy_app_info->hash, empty_hash, GAP_KEY_LEN) == 0)
        {
            gatts_gen_visible_svc_database_hash(conn->connhdl,
                                                ble_legacy_app_visible_svc_database_hash_gen_cmp);
        }
    }
}

static void ble_legacy_app_visible_svc_database_hash_gen_cmp(void *priv, int error_code, const uint8_t *hash)
{
    struct pp_buff *ppb = (struct pp_buff *)priv;
    uint8_t *raw_data = ppb_get_data(ppb);
    gap_conn_item_t *conn = NULL;

    if (error_code || hash == NULL)
    {
        DEBUG_VERBOSE(1, "%s %d", __func__, error_code);
        ppb_free(ppb);
        return;
    }

    conn = gap_get_conn_item(CO_COMBINE_UINT16_LE(&raw_data[ppb->len - 2]));

    if (conn != NULL)
    {
        DUMP8_D(hash, 16);
        memcpy(p_legacy_app_info->hash, hash, 16);
    }

    ppb_free(ppb);
}

static uint8_t ble_legacy_app_rpa_peer_addr_fill(uint8_t record_index)
{
    NV_RECORD_PAIRED_BLE_DEV_INFO_T *dev_info = nv_record_blerec_get_ptr();
    BleDeviceinfo *record = NULL;

    if (dev_info->saved_list_num == 0)
    {
        DEBUG_VERBOSE(1, "%s no nv record", __func__);
        p_legacy_app_info->peer_addr_gen_rpa = *(bt_bdaddr_t *)gap_hci_bt_address();
    }
    else
    {
        record = dev_info->ble_nv + record_index % dev_info->saved_list_num;
        p_legacy_app_info->peer_addr_gen_rpa = *(bt_bdaddr_t *)record->pairingInfo.peer_addr.addr;
    }

    return dev_info->saved_list_num;
}

static void ble_legacy_app_refresh_timer_handler(void const *param)
{
    DEBUG_VERBOSE(1, "%s %d", __func__, p_legacy_app_info->adv_refresh_idx);

    if (p_legacy_app_info->adv_refresh_idx >=
            ble_legacy_app_rpa_peer_addr_fill(p_legacy_app_info->adv_refresh_idx))
    {
        p_legacy_app_info->adv_refresh_idx = 0;
    }

    app_ble_refresh_adv_state_generic();
}

static bool ble_legacy_app_adv_activity_prepare(ble_adv_activity_t *adv)
{
    bool custom_adv_en = true;
    gap_adv_param_t *adv_param = &adv->adv_param;

    if (!ble_adv_is_allowed() || p_legacy_app_info->adv_enable == false)
    {
        if (osTimerIsRunning(p_legacy_app_info->adv_refresh_timer))
        {
            osTimerStop(p_legacy_app_info->adv_refresh_timer);
        }

        return false;
    }

    if (app_ibrt_if_is_nv_master() == false)
    {
        adv_param->peer_type = BT_ADDR_TYPE_PUBLIC;
        adv_param->peer_addr = p_legacy_app_info->peer_addr_gen_rpa;
        // First turn with no addr filled should fill it with default addr
        if (BLE_LEGACY_APP_IS_ADDR_ZERO(&adv_param->peer_addr) == true)
        {
            ble_legacy_app_rpa_peer_addr_fill(0);
        }
    }

    if (p_legacy_app_info->adv_activity_func != NULL)
    {
        custom_adv_en = p_legacy_app_info->adv_activity_func(adv);
    }
    else
    {
        adv->user = USER_BLE_DEMO0;
        adv_param->connectable = true;
        adv_param->scannable = true;
        adv_param->use_legacy_pdu = true;
        adv_param->include_tx_power_data = true;

        app_ble_dt_set_local_name(adv_param, NULL);
    }

    if (custom_adv_en == false || gap_dt_buf_is_empty(&adv_param->adv_data) == true)
    {
        DEBUG_VERBOSE(0, "%s custom not en or data empty", __func__);
        return false;
    }

    adv_param->own_addr_use_rpa = false;
    adv_param->use_fake_btc_rpa_when_no_irk_exist = false;

    if (app_ibrt_if_is_nv_master() == false)
    {
        adv_param->own_addr_use_rpa = true;
        adv_param->use_fake_btc_rpa_when_no_irk_exist = true;
        adv_param->peer_type = BT_ADDR_TYPE_PUBLIC;
        adv_param->peer_addr = p_legacy_app_info->peer_addr_gen_rpa;
    }

    if (adv_param->own_addr_use_rpa == false &&
            osTimerIsRunning(p_legacy_app_info->adv_refresh_timer))
    {
        osTimerStop(p_legacy_app_info->adv_refresh_timer);
    }
    else if (adv_param->own_addr_use_rpa == true &&
             osTimerIsRunning(p_legacy_app_info->adv_refresh_timer) == false)
    {
        osTimerStart(p_legacy_app_info->adv_refresh_timer, BLE_LEGACY_APP_ADV_REFRESH_PERIOD_MS);
    }

    return true;
}

#if BLE_GATT_CLIENT_SUPPORT
static bool ble_legacy_app_gatt_client_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param)
{
    if (p_legacy_app_info == NULL)
    {
        return false;
    }

    switch (event)
    {
        case GATT_PROF_EVENT_OPENED:
        case GATT_PROF_EVENT_CLOSED:
        case GATT_PROF_EVENT_SERVICE:
        case GATT_PROF_EVENT_CHARACTER:
        case GATT_PROF_EVENT_CHAR_READ_RSP:
        case GATT_PROF_EVENT_NOTIFY:
        case GATT_PROF_EVENT_INCLUDE:
        case GATT_PROF_EVENT_DESC_READ_RSP:
        case GATT_PROF_EVENT_CHAR_WRITE_RSP:
        case GATT_PROF_EVENT_DESC_WRITE_RSP:
        default:
        {
            break;
        }
    }

    return 0;
}
#endif /* BLE_GATT_CLIENT_SUPPORT */

#endif
