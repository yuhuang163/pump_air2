/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#if defined(__GATT_OVER_BR_EDR__)

#include "plat_types.h"
#include "app_btgatt.h"
#include "bt_if.h"
#include "app_bt.h"

void (*app_btgatt_state_cb)(const bt_bdaddr_t *remote, bool is_connected, uint8_t reason) = NULL;

bool app_btgatt_over_br_edr_enabled(void)
{
    return btif_is_gatt_over_br_edr_enabled();
}

bool app_btgatt_is_connected(uint8_t device_id)
{
    return gatt_over_bredr_is_profile_connected(gap_get_conn_hdl(device_id));
}

bool app_btgatt_is_connected_by_conidx(uint8_t con_idx)
{
    return gatt_over_bredr_is_profile_connected(gap_get_conn_hdl(con_idx));
}

void app_btgatt_client_create(const bt_bdaddr_t *remote)
{
    if (remote)
    {
        gatt_create_att_bearer_over_bredr(btif_me_get_acl_conn_handle(remote));
    }
}

void app_btgatt_disconnect(uint8_t device_id)
{
    gatt_disconnect_att_bearers_over_bredr(gap_get_conn_hdl(device_id), true, 0);
}

void app_btgatt_report_prf_state(uint8_t device_id, const bt_bdaddr_t *remote, bool is_connected, uint8_t reason)
{
#if defined(IBRT)
    if (reason != BT_ECODE_IBRT_SLAVE_CLEANUP)
    {
        uint16_t event = is_connected ? GATT_OVER_BREDR_EVENT_CONTROL_CONNECTED :
                         GATT_OVER_BREDR_EVENT_CONTROL_DISCONNECTED;
        app_bt_notify_profile_evt_callback(device_id, BTIF_APP_BTGATT_PROFILE_ID, (void *)remote, (void *)&event, NULL);
    }
#endif

    if (app_btgatt_state_cb != NULL)
    {
        app_btgatt_state_cb(remote, is_connected, reason);
    }
}

void app_btgatt_state_callback_register(void (*p_cb)(const bt_bdaddr_t *remote, bool is_connected, uint8_t reason))
{
    app_btgatt_state_cb = p_cb;
}

#endif
