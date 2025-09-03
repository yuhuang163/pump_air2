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
/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if BLE_AUDIO_ENABLED
#include "app_gaf.h"
#include "app_vap.h"
#include "bthost_logger.h"
#include "app_gaf_custom_api.h"

#include "app_vap_vac_msg.h"

#include "vac.h"

/// Callback for vac bond data
static void app_vap_vac_cb_bond_data_evt(uint8_t con_lid, uint8_t va_lid, const vac_prf_svc_t *param)
{
    LOG_I("app_vap vac service found con_lid = %d, va_lid = %d, shdl = %04x, ehdl = %04x",
          con_lid, va_lid, param->svc_range.shdl, param->svc_range.ehdl);

    uint8_t char_type = VAS_CHAR_TYPE_MIN;

    for (char_type = VAS_CHAR_TYPE_MIN; char_type < VAS_CHAR_TYPE_MAX; char_type++)
    {
        vac_character_value_read(con_lid, va_lid, char_type);
        vac_character_cccd_write(con_lid, va_lid, char_type, true);
    }
}

/// Callback for vac discovery done
static void app_vap_vac_cb_discovery_cmp_evt(uint8_t con_lid, uint16_t err_code)
{
    LOG_I("app_vap vac discovery cmp, con_lid = %d, status = %d", con_lid, err_code);
}

/// Callback for vac gatt set cfg complete
static void app_vap_vac_cb_set_cfg_cmp_evt(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, uint16_t err_code)
{
    LOG_I("app_vap vac set cfg cmp, con_lid = %d, va_lid = %d, char_type = %d, status = %d",
          con_lid, va_lid, char_type, err_code);
}

/// Callback for vac cccd cfg value received or read
static void app_vap_vac_cb_cfg_val_evt(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, bool enabled, uint16_t err_code)
{
    LOG_I("app_vap vac get cfg cmp, con_lid = %d, va_lid = %d, char_type = %d, status = %d, enable_ntf = %d",
          con_lid, va_lid, char_type, err_code, enabled);
}

/// Callback for vac op control complete
static void app_vap_vac_cb_vae_cp_cmp_evt(uint8_t con_lid, uint8_t va_lid, uint8_t op_code, const vac_resp_t *p_resp, uint16_t err_code)
{
    uint8_t rsp_code = 0xFF;
    uint8_t rsp_val = 0xFF;

    if (p_resp != NULL)
    {
        rsp_code = p_resp->resp_op;
        rsp_val = p_resp->val.rsp_val;
    }

    LOG_I("app_vap vac control cmp, con_lid = %d, va_lid = %d, op = %d, rsp_code = %d, val = %d, status = %d",
          con_lid, va_lid, op_code, rsp_code, rsp_val, err_code);
}

/// Callback for vac session state ntf or read
static void app_vap_vac_cb_session_state_evt(uint8_t con_lid, uint8_t va_lid, uint8_t session_state, uint16_t err_code)
{
    LOG_I("app_vap vac_session_state_ind con_lid = %d, va_lid = %d, state= %d, status = %d",
          con_lid, va_lid, session_state, err_code);
}

/// Callback for vac session flag or read
static void app_vap_vac_cb_session_flag_evt(uint8_t con_lid, uint8_t va_lid, uint8_t session_flag, uint16_t err_code)
{
    LOG_I("app_vap vac_session_flags_ind con_lid = %d, va_lid = %d, flags = %d, status = %d",
          con_lid, va_lid, session_flag, err_code);
}

/// Callback function called when a character value is received with status
static void app_vap_vac_cb_char_value_evt(uint8_t con_lid, uint8_t va_lid, uint8_t char_type, const uint8_t *val, uint16_t val_len, uint16_t err_code)
{
    LOG_I("app_vap vac_char_val_ind con_lid = %d, va_lid = %d, char_type = %d, val_len = %d, status = %d",
          con_lid, va_lid, char_type, val_len, err_code);

    if (val != NULL && val_len != 0)
    {
        DUMP8("%c", val, val_len);
    }
}

static void app_vap_vac_cb_prf_status_event(uint8_t con_lid, bool is_central, gatt_prf_status_event_e event)
{
    if (event == GATT_PRF_STATUS_EVENT_OPENED && is_central == true)
    {
        //vac_service_discovery(con_lid);
    }
}

static const vac_evt_cb_t vac_evt_cb =
{
    .cb_bond_data = app_vap_vac_cb_bond_data_evt,
    .cb_discovery_cmp = app_vap_vac_cb_discovery_cmp_evt,
    .cb_cfg_value = app_vap_vac_cb_cfg_val_evt,
    .cb_set_cfg_cmp = app_vap_vac_cb_set_cfg_cmp_evt,
    .cb_vae_ctrl_cmp = app_vap_vac_cb_vae_cp_cmp_evt,
    .cb_session_flag = app_vap_vac_cb_session_flag_evt,
    .cb_session_state = app_vap_vac_cb_session_state_evt,
    .cb_char_val = app_vap_vac_cb_char_value_evt,
    .cb_prf_status_event = app_vap_vac_cb_prf_status_event,
};

int app_vap_vac_init(void)
{
    LOG_I("app_vap vac init");

    vac_cfg_t vac_init_cfg =
    {
        .max_supp_vas_inst = APP_VAP_VAC_DFT_NB_VA_SUPP,
        .pref_mtu = GAF_PREFERRED_MTU,
    };

    return vac_init(&vac_init_cfg, &vac_evt_cb);
}

int app_vap_vac_deinit(void)
{
    LOG_I("app_vap vac deinit");

    return vac_deinit();
}

int app_vap_vac_start(uint8_t con_lid)
{
    LOG_I("app_vap vac start, con_lid = %d", con_lid);
    return vac_service_discovery(con_lid);
}

int app_vap_vac_control(uint8_t con_lid, uint8_t va_lid, uint8_t opcode)
{
    return vac_vae_control(con_lid, va_lid, opcode);
}

#endif /* BLE_AUDIO_ENABLED */

/// @} APP
