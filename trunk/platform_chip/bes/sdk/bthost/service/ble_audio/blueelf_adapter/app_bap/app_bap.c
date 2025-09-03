/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#include "bthost_logger.h"

#if BLE_AUDIO_ENABLED
#include "app_bap.h"

#include "app_bap_uc_cli_msg.h"
#include "app_bap_uc_srv_msg.h"
#include "app_bap_capa_cli_msg.h"
#include "app_bap_capa_srv_msg.h"
#include "app_bap_bc_sink_msg.h"
#include "app_bap_bc_scan_msg.h"
#include "app_bap_bc_src_msg.h"
#include "app_bap_bc_deleg_msg.h"
#include "app_bap_bc_assist_msg.h"

#include "app_gaf_custom_api.h"

/*CAPA SRV*/
extern int app_bap_capa_srv_init(void);
extern int app_bap_capa_srv_deinit(void);
extern int app_bap_capa_srv_info_init(app_bap_capa_srv_dir_t *sink_capa_info, app_bap_capa_srv_dir_t *src_capa_info);
extern int app_bap_capa_srv_info_deinit(void);
/*UC SRV*/
extern int app_bap_uc_srv_init(void);
extern int app_bap_uc_srv_deinit(void);
extern int app_bap_uc_srv_info_init(void);
extern int app_bap_uc_srv_info_deinit(void);
/*BC SRC*/
extern int app_bap_bc_src_init(void);
extern int app_bap_bc_src_deinit(void);
extern int app_bap_bc_src_info_init(void);
extern int app_bap_bc_src_info_deinit(void);
/*BC SINK*/
extern int app_bap_bc_sink_init(void);
extern int app_bap_bc_sink_deinit(void);
/*BC DELEG*/
extern int app_bap_bc_deleg_init(void);
extern int app_bap_bc_deleg_deinit(void);
extern int app_bap_bc_deleg_info_init(void);
extern int app_bap_bc_deleg_info_deinit(void);
/*BC ASSISTANT*/
extern int app_bap_bc_assist_init(void);
extern int app_bap_bc_assist_deinit(void);
extern int app_bap_bc_assist_info_init(void);
extern int app_bap_bc_assist_info_deinit(void);
/*BC SCAN*/
extern int app_bap_bc_scan_init(void);
extern int app_bap_bc_scan_deinit(void);
extern int app_bap_bc_scan_info_init(void);
extern int app_bap_bc_scan_info_deinit(void);
#ifdef AOB_MOBILE_ENABLED
/*UC CLI*/
extern int app_bap_uc_cli_init(void);
extern int app_bap_uc_cli_deinit(void);
extern int app_bap_uc_cli_info_init(void);
extern int app_bap_uc_cli_info_deinit(void);
/*CAPA CLI*/
extern int app_bap_capa_cli_init(void);
extern int app_bap_capa_cli_deinit(void);
extern int app_bap_capa_cli_info_init(void);
extern int app_bap_capa_cli_info_deinit(void);
#endif /// AOB_MOBILE_ENABLED

/*LOCAL VARIABLE*/
const app_gaf_codec_id_t codec_id_lc3 = {{0x06}};
const app_gaf_codec_id_t codec_id_lc3plus = {{0xFF, 0x08, 0xA9, 0x00, 0x02,}};
const app_gaf_codec_id_t codec_id_lhdc = {{0xFF, 0x35, 0x4C, 0x05, 0x3A,}};

static uint32_t role_bf;
static uint8_t g_bap_activity_type = GAF_BAP_ACT_TYPE_MAX;
#ifdef AOB_MOBILE_ENABLED
static uint8_t g_bap_device_num_to_be_connect = 0;
#endif /// AOB_MOBILE_ENABLED

const char *ase_state_str[ASCS_ASE_STATE_MAX + 1] =
{
    "IDLE",
    "CODEC CONFIGURED",
    "QOS CONFIGURED",
    "ENABLING",
    "STREAMING",
    "DISABLING",
    "RELEASING",
    "INVALID"
};

/*Functions*/
int app_bap_server_init(app_bap_capa_srv_dir_t *sink_capa_info,
                        app_bap_capa_srv_dir_t *src_capa_info, uint32_t curr_role_bf)
{
    LOG_D("app_bap srv init");

    role_bf = curr_role_bf;

    if ((role_bf & APP_BAP_ROLE_SUPP_CAPA_SRV_BIT))
    {
        app_bap_capa_srv_info_init(sink_capa_info, src_capa_info);
        app_bap_capa_srv_init();
    }

    if ((role_bf & APP_BAP_ROLE_SUPP_UC_SRV_BIT))
    {
        app_bap_uc_srv_info_init();
        app_bap_uc_srv_init();
    }

#ifdef APP_BLE_BIS_SRC_ENABLE
    app_bap_bc_src_info_init();
    app_bap_bc_src_init();
#endif ///APP_BLE_BIS_SRC_ENABLE

#if defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_scan_info_init();
    app_bap_bc_scan_init();
#endif /// defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)

#if defined (APP_BLE_BIS_SINK_ENABLE) || defined (APP_BLE_BIS_DELEG_ENABLE)
    app_bap_bc_sink_init();
#endif /// defined (APP_BLE_BIS_SINK_ENABLE) || defined (APP_BLE_BIS_DELEG_ENABLE)

#ifdef APP_BLE_BIS_DELEG_ENABLE
    app_bap_bc_deleg_info_init();
    app_bap_bc_deleg_init();
#endif /// APP_BLE_BIS_DELEG_ENABLE

    return BT_STS_SUCCESS;
}

int app_bap_server_deinit(void)
{
    LOG_D("app_bap srv deinit");

    if ((role_bf & APP_BAP_ROLE_SUPP_CAPA_SRV_BIT))
    {
        app_bap_capa_srv_deinit();
        app_bap_capa_srv_info_deinit();
    }

    if ((role_bf & APP_BAP_ROLE_SUPP_UC_SRV_BIT))
    {
        app_bap_uc_srv_deinit();
        app_bap_uc_srv_info_deinit();
    }

#ifdef APP_BLE_BIS_SRC_ENABLE
    app_bap_bc_src_deinit();
    app_bap_bc_src_info_deinit();
#endif ///APP_BLE_BIS_SRC_ENABLE

#if defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_scan_deinit();
    app_bap_bc_scan_info_deinit();
#endif /// defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)

#if defined (APP_BLE_BIS_SINK_ENABLE) || defined (APP_BLE_BIS_DELEG_ENABLE)
    app_bap_bc_sink_deinit();
#endif /// defined (APP_BLE_BIS_SINK_ENABLE) || defined (APP_BLE_BIS_DELEG_ENABLE)

#ifdef APP_BLE_BIS_DELEG_ENABLE
    app_bap_bc_deleg_deinit();
    app_bap_bc_deleg_info_deinit();
#endif /// APP_BLE_BIS_DELEG_ENABLE

    return BT_STS_SUCCESS;
}

#ifdef AOB_MOBILE_ENABLED
int app_bap_client_init(void)
{
    LOG_I("app_bap cli init");

    role_bf = 0;
    role_bf |= APP_BAP_ROLE_SUPP_CAPA_CLI_BIT;
    role_bf |= APP_BAP_ROLE_SUPP_UC_CLI_BIT;
    role_bf |= APP_BAP_ROLE_SUPP_BC_SRC_BIT;
    role_bf |= APP_BAP_ROLE_SUPP_BC_SCAN_BIT;
    role_bf |= APP_BAP_ROLE_SUPP_BC_SINK_BIT;
    role_bf |= APP_BAP_ROLE_SUPP_BC_ASSIST_BIT;

    if ((role_bf & APP_BAP_ROLE_SUPP_UC_CLI_BIT))
    {
        app_bap_uc_cli_info_init();
        app_bap_uc_cli_init();
    }

    if ((role_bf & APP_BAP_ROLE_SUPP_CAPA_CLI_BIT))
    {
        app_bap_capa_cli_info_init();
        app_bap_capa_cli_init();
    }

#ifdef APP_BLE_BIS_SRC_ENABLE
    app_bap_bc_src_info_init();
    app_bap_bc_src_init();
#endif ///APP_BLE_BIS_SRC_ENABLE

#if defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_scan_info_init();
    app_bap_bc_scan_init();
#endif /// defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)

#if defined (APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_sink_init();
#endif /// defined (APP_BLE_BIS_SINK_ENABLE)

#ifdef APP_BLE_BIS_ASSIST_ENABLE
    app_bap_bc_assist_info_init();
    app_bap_bc_assist_init();
#endif /// APP_BLE_BIS_ASSIST_ENABLE

    return BT_STS_SUCCESS;
}

int app_bap_client_deinit(void)
{
    LOG_I("app_bap cli deinit");

    if ((role_bf & APP_BAP_ROLE_SUPP_UC_CLI_BIT))
    {
        app_bap_uc_cli_info_deinit();
        app_bap_uc_cli_deinit();
    }

    if ((role_bf & APP_BAP_ROLE_SUPP_CAPA_CLI_BIT))
    {
        app_bap_capa_cli_info_deinit();
        app_bap_capa_cli_deinit();
    }

#ifdef APP_BLE_BIS_SRC_ENABLE
    app_bap_bc_src_deinit();
    app_bap_bc_src_info_deinit();
#endif ///APP_BLE_BIS_SRC_ENABLE

#if defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_scan_deinit();
    app_bap_bc_scan_info_deinit();
#endif /// defined(APP_BLE_BIS_ASSIST_ENABLE) || defined(APP_BLE_BIS_DELEG_ENABLE) || defined(APP_BLE_BIS_SINK_ENABLE)

#if defined (APP_BLE_BIS_SINK_ENABLE)
    app_bap_bc_sink_deinit();
#endif /// defined (APP_BLE_BIS_SINK_ENABLE)

#ifdef APP_BLE_BIS_ASSIST_ENABLE
    app_bap_bc_assist_deinit();
    app_bap_bc_assist_info_deinit();
#endif /// APP_BLE_BIS_ASSIST_ENABLE

    return BT_STS_SUCCESS;
}
#endif /// AOB_MOBILE_ENABLED

void app_bap_start(uint8_t con_lid)
{
#ifdef AOB_MOBILE_ENABLED
    app_bap_capa_cli_start(con_lid);
#endif /// AOB_MOBILE_ENABLED
#ifdef APP_BLE_BIS_ASSIST_ENABLE
    app_bap_bc_assist_start(con_lid);
#endif /// APP_BLE_BIS_ASSIST_ENABLE
}

/*******************************************************************************/
bool app_bap_codec_is_lc3(const void *p_codec_id)
{
    return gen_aud_codec_is_lc3((uint8_t *)p_codec_id);
}

bool app_bap_codec_is_lc3plus(const void *p_codec_id)
{
    uint8_t *p_codec_id_lc3plus = (uint8_t *)p_codec_id;

    return (p_codec_id_lc3plus[3] == 0x00) && (p_codec_id_lc3plus[4] == 0x02);
}

#ifdef LEA_LHDC
bool app_bap_codec_is_lhdc(const void *p_codec_id)
{
    uint8_t *p_codec_id_lhdc = (uint8_t *)p_codec_id;

    return (p_codec_id_lhdc[3] == 0x05) && (p_codec_id_lhdc[4] == 0x3A);
}
#endif

uint8_t app_bap_frame_dur_us_to_frame_dur_enum(uint32_t frame_dur_us)
{
    LOG_D("%s get duration %d us", __func__, frame_dur_us);
    uint8_t frame_dur_e = APP_GAF_BAP_FRAME_DURATION_10MS;

    if (frame_dur_us == 10000)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_10MS;
    }
    else if (frame_dur_us == 7500)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_7_5MS;
    }
    else if (frame_dur_us == 5000)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_5MS;
    }
    else if (frame_dur_us == 2500)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_2_5MS;
    }
    else if (frame_dur_us == 8163)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_8_163MS;
    }
    else if (frame_dur_us == 10884)
    {
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_10_884MS;
    }
    else
    {
        LOG_I("%s unsupported duration %d us", __func__, frame_dur_us);
        frame_dur_e = APP_GAF_BAP_FRAME_DURATION_MAX;
    }

    return frame_dur_e;
}

bool app_bap_get_specifc_ltv_data(app_gaf_ltv_t *add_data, uint8_t specific_ltv_type,
                                  void *p_cfg_out)
{
    uint8_t length = 0;
    uint8_t type = 0;
    uint8_t rem_length = 0;
    uint8_t section_length = 0;
    uint8_t *p_data = NULL;
    // Init remain length
    rem_length = add_data->len;
    p_data = &add_data->data[0];
    while (rem_length != 0)
    {
        // Length
        length = *(p_data + GEN_AUD_LTV_LENGTH_POS);
        // Type
        type = *(p_data + GEN_AUD_LTV_TYPE_POS);

        section_length += (GEN_AUD_LTV_TYPE_POS + length);

        if (type == specific_ltv_type)
        {
            memcpy(p_cfg_out, p_data, section_length);
            // Directly return
            return true;
        }
        p_data += section_length;
        rem_length -= section_length;
    }

    return false;
}

void app_bap_capa_param_print(app_gaf_bap_capa_param_t *capa_param)
{
    if (!capa_param)
    {
        return ;
    }

    LOG_D("sampling_freq_bf = 0x%04x, frame_dur_bf = 0x%02x", capa_param->sampling_freq_bf,
          capa_param->frame_dur_bf);

    LOG_D("frame_octet_min = %d, frame_octet_max = %d", capa_param->frame_octet_min,
          capa_param->frame_octet_max);

    LOG_D("chan_cnt_bf = 0x%02x, max_codec_frame_sdu = %d", capa_param->chan_cnt_bf,
          capa_param->max_frames_sdu);
}

void app_bap_capa_print(app_gaf_bap_capa_t *p_capa)
{
    if (!p_capa)
    {
        return ;
    }

    LOG_D("sampling_freq_bf = 0x%04x, frame_dur_bf = 0x%02x", p_capa->param.sampling_freq_bf,
          p_capa->param.frame_dur_bf);

    LOG_D("frame_octet_min = %d, frame_octet_max = %d", p_capa->param.frame_octet_min,
          p_capa->param.frame_octet_max);

    LOG_D("chan_cnt_bf = 0x%02x, max_codec_frame_sdu = %d", p_capa->param.chan_cnt_bf,
          p_capa->param.max_frames_sdu);

    LOG_D("add_capa_len = %d, value:", p_capa->add_capa.len);

    if (p_capa->add_capa.len > 0)
    {
        DUMP8_D(p_capa->add_capa.data, p_capa->add_capa.len);
    }
}

void app_bap_capa_metadata_print(app_gaf_bap_capa_metadata_t *p_capa_metadata)
{
    if (!p_capa_metadata)
    {
        return ;
    }

    LOG_D("context_type = 0x%04x, add_metadata_len = %d, value:",
          p_capa_metadata->param.context_bf, p_capa_metadata->add_metadata.len);

    if (p_capa_metadata->add_metadata.len > 0)
    {
        DUMP8_D(p_capa_metadata->add_metadata.data, p_capa_metadata->add_metadata.len);
    }
}

void app_bap_cfg_print(app_gaf_bap_cfg_t *p_cfg)
{
    if (!p_cfg)
    {
        return ;
    }

    LOG_I("location_bf = 0x%08x, frame_octet = %d, sampling_freq = %d",
          p_cfg->param.location_bf, p_cfg->param.frame_octet, p_cfg->param.sampling_freq);

    LOG_I("frame_dur = %d, nb_codec_frames = %d, add_codec_cfg_len = %d, value:",
          p_cfg->param.frame_dur, p_cfg->param.frames_sdu, p_cfg->add_cfg.len);

    if (p_cfg->add_cfg.len > 0)
    {
        DUMP8_I(p_cfg->add_cfg.data, p_cfg->add_cfg.len);
    }
}

void app_bap_cfg_metadata_print(app_gaf_bap_cfg_metadata_t *p_cfg_metadata)
{
    if (!p_cfg_metadata)
    {
        return ;
    }

    LOG_D("context_type = 0x%04x, add_metadata_len = %d, value:",
          p_cfg_metadata->param.context_bf, p_cfg_metadata->add_metadata.len);

    if (p_cfg_metadata->add_metadata.len > 0)
    {
        DUMP8_D(p_cfg_metadata->add_metadata.data, p_cfg_metadata->add_metadata.len);
    }
}

void app_bap_add_data_set(uint8_t *data, uint8_t data_len)
{
    if ((NULL != data) && (data_len > 0))
    {
        data[0] = data_len - 1;
        for (uint8_t i = 1; i < data_len; i++)
        {
            data[i] = i;
        }
    }
}

static CIS_TIMING_CONFIGURATION_T cis_timing_config =
{
    .m2s_ft = 4,
    .s2m_ft = 4,

    .m2s_bn = 1,
    .m2s_nse = 2,
    .s2m_bn = 1,
    .s2m_nse = 2,

    .frame_cnt_per_sdu = 1,
    .iso_interval = 8,  // 10ms = 8 * 1.25ms
};

void app_bap_update_cis_timing(CIS_TIMING_CONFIGURATION_T *pTiming)
{
    DEBUG_INFO(0, "Update CIS timing:");
    DEBUG_INFO(0, "m2s_bn %d", pTiming->m2s_bn);
    DEBUG_INFO(0, "m2s_nse %d", pTiming->m2s_nse);
    DEBUG_INFO(0, "m2s_ft %d", pTiming->m2s_ft);
    DEBUG_INFO(0, "s2m_bn %d", pTiming->s2m_bn);
    DEBUG_INFO(0, "s2m_nse %d", pTiming->s2m_nse);
    DEBUG_INFO(0, "s2m_ft %d", pTiming->s2m_ft);
    DEBUG_INFO(0, "frame_cnt_per_sdu %d", pTiming->frame_cnt_per_sdu);
    DEBUG_INFO(0, "iso_interval %d", pTiming->iso_interval);

    cis_timing_config = *pTiming;
}

CIS_TIMING_CONFIGURATION_T *app_bap_get_cis_timing_config(void)
{
    return &cis_timing_config;
}

gaf_bap_activity_type_e app_bap_get_activity_type(void)
{
    return g_bap_activity_type;
}

void app_bap_set_activity_type(gaf_bap_activity_type_e type)
{
    g_bap_activity_type = type;
}

#ifdef AOB_MOBILE_ENABLED
void app_bap_set_device_num_to_be_connected(uint8_t dev_num)
{
    if (dev_num > BLE_CONNECTION_MAX)
    {
        DEBUG_VERBOSE(1, "%s dev num err %d", __func__, dev_num);
        return;
    }
    DEBUG_INFO(1, "%s dev num %d", __func__, dev_num);
    g_bap_device_num_to_be_connect = dev_num;
}

uint8_t app_bap_get_device_num_to_be_connected(void)
{
    return g_bap_device_num_to_be_connect;
}
#endif

uint32_t app_bap_get_role_bit_filed(void)
{
    return role_bf;
}

uint32_t app_bap_frame_dur_enum_to_frame_dur_us(uint8_t frame_duration_e)
{
    LOG_I("%s duration %d", __func__, frame_duration_e);

    switch (frame_duration_e)
    {
        case APP_GAF_BAP_FRAME_DURATION_2_5MS:
            return 2500;
            break;
        case APP_GAF_BAP_FRAME_DURATION_5MS:
            return 5000;
            break;
        case APP_GAF_BAP_FRAME_DURATION_7_5MS:
            return 7500;
            break;
        case APP_GAF_BAP_FRAME_DURATION_10MS:
            return 10000;
            break;
        default:
            LOG_D("%s unsupported duration enum %d", __func__, frame_duration_e);
            break;
    }

    return 0;
}

uint32_t app_shp_ht_intv_enum_to_ht_intv_us(uint8_t ht_intv_e)
{
    LOG_I("%s interval %d", __func__, ht_intv_e);

    switch (ht_intv_e)
    {
        case APP_GAF_BAP_HT_FRAME_INTV_7_5MS:
            return 7500;
            break;
        case APP_GAF_BAP_HT_FRAME_INTV_10MS:
            return 10000;
            break;
        case APP_GAF_BAP_HT_FRAME_INTV_20MS:
            return 20000;
            break;
        default:
            LOG_D("%s unsupported intv enum %d", __func__, ht_intv_e);
            break;
    }

    return 0;
}

uint8_t app_bap_get_audio_location_l_r_cnt(uint32_t audio_location_bf)
{
    uint8_t location_l_r_cnt = 0;
    if (audio_location_bf & (APP_GAF_BAP_AUDIO_LOCATION_SIDE_LEFT | APP_GAF_BAP_AUDIO_LOCATION_FRONT_LEFT))
    {
        location_l_r_cnt++;
    }
    if (audio_location_bf & (APP_GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT | APP_GAF_BAP_AUDIO_LOCATION_FRONT_RIGHT))
    {
        location_l_r_cnt++;
    }

    return location_l_r_cnt;
}

uint8_t app_bap_get_max_audio_channel_supp_cnt(uint32_t audio_channel_cnt_bf)
{
    uint8_t channel_cnt;
    if (audio_channel_cnt_bf == 0)
    {
        return 1;
    }
    for (channel_cnt = 31; channel_cnt >= 0; channel_cnt--)
    {
        if (CO_BIT(channel_cnt) & audio_channel_cnt_bf)
        {
            break;
        }
    }
    return channel_cnt + 1;
}

uint8_t *app_bap_get_ltv_value_by_type(app_gaf_ltv_t *p_ltv_data, uint8_t ltv_type)
{
    return gen_aud_get_ltv_value_by_type((gen_aud_var_info_t *)p_ltv_data, ltv_type);
}

uint8_t app_bap_get_sample_type(uint32_t sample_value)
{
    switch (sample_value)
    {
        case 8000:
            return APP_GAF_BAP_SAMPLE_FREQ_8000;
        break;
        case 16000:
            return APP_GAF_BAP_SAMPLE_FREQ_16000;
        break;
        case 22050:
            return APP_GAF_BAP_SAMPLE_FREQ_22050;
        break;
        case 24000:
            return APP_GAF_BAP_SAMPLE_FREQ_24000;
        break;
        case 32000:
            return APP_GAF_BAP_SAMPLE_FREQ_32000;
        break;
        case 44100:
            return APP_GAF_BAP_SAMPLE_FREQ_44100;
        break;
        case 48000:
            return APP_GAF_BAP_SAMPLE_FREQ_48000;
        break;
        case 88200:
            return APP_GAF_BAP_SAMPLE_FREQ_88200;
        break;
        case 96000:
            return APP_GAF_BAP_SAMPLE_FREQ_96000;
        break;
        case 176400:
            return APP_GAF_BAP_SAMPLE_FREQ_176400;
        break;
        case 192000:
            return APP_GAF_BAP_SAMPLE_FREQ_192000;
        break;
        case 384000:
            return APP_GAF_BAP_SAMPLE_FREQ_384000;
        break;
        default:
            LOG_D("%s unsupported freq %d", __func__, sample_value);
        break;
    }

    return 0;
}

#ifdef AOB_MOBILE_ENABLED
/**
 ****************************************************************************************
 * @brief update sdu intv before uc cli start stream
 *
 * @param sdu_intv_m2s_us
 * @param sdu_intv_s2m_us
 ****************************************************************************************
*/
void app_bap_update_sdu_intv(uint32_t sdu_intv_m2s_us, uint32_t sdu_intv_s2m_us)
{
    app_bap_uc_cli_set_sdu_intv(sdu_intv_m2s_us, sdu_intv_s2m_us);
}
#endif /// AOB_MOBILE_ENABLED
#endif /// BLE_AUDIO_ENABLED
/// @} APP
