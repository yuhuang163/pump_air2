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
#ifdef AOB_MOBILE_ENABLED
#include "app_gaf.h"
#include "app_vap.h"
#include "bthost_logger.h"
#include "app_gaf_custom_api.h"

#include "app_vap_vas_msg.h"

#include "vas.h"

/// Callback for vas bond data
static void app_vap_vas_cb_bond_data_evt(uint8_t va_lid, uint8_t con_lid, uint8_t char_type, uint16_t cli_cfg)
{
    LOG_I("%s va_lid = %d, con_lid = %d, char_type = %d, cli_cfg = %d",
          __func__, va_lid, con_lid, char_type, cli_cfg);
}

/// Callback function called to inform upper layer that a client device has requested an operation
static void app_vap_vas_cb_cp_operation_req(uint8_t va_lid, uint8_t con_lid, uint8_t opcode)
{
    uint16_t status = vas_vae_cp_operation_req_cfm(VAP_VAE_CP_RESP_OP_RSP_VAL_SUCCESS, va_lid);

    LOG_I("%s va_lid = %d, con_lid = %d, opcode = %d, cfm_status = %d",
          __func__, va_lid, con_lid, opcode, status);
}

static const vas_evt_cb_t vas_evt_cb =
{
    .cb_bond_data = app_vap_vas_cb_bond_data_evt,
    .cb_cp_operation_req = app_vap_vas_cb_cp_operation_req,
};

int app_vap_vas_init(void)
{
    LOG_I("app_vap vas init");

    vas_init_cfg_t vas_init_cfg =
    {
        .nb_vas_inst_supp = APP_VAP_VAS_DFT_NB_VA_SUPP,
    };

    vas_inst_cfg_t vas_inst_cfg =
    {
        /// Preferred MTU size
        .pref_mtu = GAF_PREFERRED_MTU,
        /// Content Control ID
        .ccid = APP_VAP_VAS_CCID,
        /// VAE Name length
        .vae_name_len = sizeof(APP_VAP_VAS_VAE_NAME),
        /// Installed loc length
        .installed_loc_len = sizeof(APP_VAP_VAS_INSTALLED_LOC),
        /// Suppoted languages
        .supp_languages_len = sizeof(APP_VAP_VAS_VAE_SUPP_LANGUAGES),
        /// VAE Name value
        .p_vae_name = (uint8_t *)APP_VAP_VAS_VAE_NAME,
        /// Installed loc value
        .p_installed_loc = (uint8_t *)APP_VAP_VAS_INSTALLED_LOC,
        /// Suppoted languages
        .p_supported_languages = (uint8_t *)APP_VAP_VAS_VAE_SUPP_LANGUAGES,
    };

    memcpy(vas_inst_cfg.vae_uuid, (uint8_t *)APP_VAP_VAS_VAE_UUID, sizeof(APP_VAP_VAS_VAE_UUID));

    uint16_t status = vas_init(&vas_init_cfg, &vas_evt_cb);

    if (status == BT_STS_SUCCESS)
    {
        status = vas_add_vas_instant(&vas_inst_cfg, NULL);
    }

    return status;
}

int app_vap_vas_deinit(void)
{
    LOG_I("app_vap vas deinit");

    return vas_deinit();
}

int app_vap_vas_session_update(uint8_t con_lid, uint8_t va_lid, uint8_t state, uint8_t flag)
{
    return vas_set_va_session_state_flag(con_lid, va_lid, state, flag);
}

#endif /* AOB_MOBILE_ENABLED */
#endif /* BLE_AUDIO_ENABLED */

/// @} APP
