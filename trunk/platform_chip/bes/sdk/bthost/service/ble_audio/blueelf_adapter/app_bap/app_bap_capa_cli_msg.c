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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "bthost_logger.h"
#if BLE_AUDIO_ENABLED
#ifdef AOB_MOBILE_ENABLED
#include "app_bap.h"

#include "app_gaf.h"
#include "app_gaf_custom_api.h"
#include "app_bap_capa_cli_msg.h"

#include "bap_unicast_client.h"

/// Published Audio Capabilities Client environment structure
typedef struct app_bap_capa_cli_env
{
    /// Pointer to Direction information structure
    app_bap_capa_cli_supp_t capa_info[APP_GAF_DIRECTION_MAX];
} app_bap_capa_cli_env_t;

/// PAC information structure
typedef struct app_bap_capa_pac_cfg
{
    /// List of records (@see app_bap_capa_record_cfg_t for elements)
    struct list_node list_cfg;
} app_bap_capa_pac_cfg_t;

/// PACC Direction Configuration structure
typedef struct app_bap_capa_dir_cfg
{
    /// Audio Locations for the direction
    uint32_t location_bf;
    /// Available Audio Contexts for the direction
    uint16_t supp_context_bf;
    /// Available Audio Contexts for the direction
    uint16_t ava_context_bf;
    /// Number of PACs for the direction
    uint8_t nb_pacs;
} app_bap_capa_dir_cfg_t;

typedef struct app_bap_capa_cfg
{
    /// Pointer to Direction information structure
    app_bap_capa_dir_cfg_t dir_cfg[APP_GAF_DIRECTION_MAX];
    /// PAC informations (nb_pacs elements)
    app_bap_capa_pac_cfg_t *p_pac_cfg[APP_BAP_DFT_CAPA_C_SINK_SUPP_NUN + APP_BAP_DFT_CAPA_C_SRC_SUPP_NUN];
} app_bap_capa_cfg_t;

/// Published Audio Capabilities Client Configuration structure
typedef struct app_bap_capa_cfg_env
{
    app_bap_capa_cfg_t capa_cfg[BLE_AUDIO_CONNECTION_CNT];
} app_bap_capa_cfg_env_t;

/* Internal environment */
app_bap_capa_cli_env_t *p_app_capa_cli_env = NULL;
app_bap_capa_cfg_env_t *p_app_capa_cfg_env = NULL;

/* Functions */
static int app_bap_capa_cli_vendor_pac_parse(app_gaf_ltv_t *vendor_capa_in, app_gaf_bap_cfg_t *p_cfg)
{
#if defined(LC3PLUS_SUPPORT)
    app_gaf_bap_lc3p_frame_dur_ltv_t *p_add_capa = (app_gaf_bap_lc3p_frame_dur_ltv_t *)p_cfg->add_cfg.data;

    app_gaf_bap_lc3p_supp_fr_dur_bf_ltv_t *p_frame_dur_bf =
        (app_gaf_bap_lc3p_supp_fr_dur_bf_ltv_t *)app_bap_get_ltv_value_by_type(vendor_capa_in,
                                                                               APP_GAF_LC3P_CAPA_TYPE_FRAME_DURATION);
    app_gaf_bap_lc3p_frame_octets_ltv_t *p_frame_octets_10ms =
        (app_gaf_bap_lc3p_frame_octets_ltv_t *)app_bap_get_ltv_value_by_type(vendor_capa_in,
                                                                             APP_GAF_LC3P_CAPA_TYPE_FRAME_OCT_10MS);
    app_gaf_bap_lc3p_frame_octets_ltv_t *p_frame_octets_7_5ms =
        (app_gaf_bap_lc3p_frame_octets_ltv_t *)app_bap_get_ltv_value_by_type(vendor_capa_in,
                                                                             APP_GAF_LC3P_CAPA_TYPE_FRAME_OCT_7_5MS);

    p_cfg->add_cfg.len = 0;

    if (p_frame_dur_bf != NULL)
    {
        uint16_t frame_dur_bf = p_frame_dur_bf->frame_dur_bf;
        LOG_I("LC3Plus Frame duration bf:0x%x", frame_dur_bf);
        // Not allow using bap fram dur only allow using vendor
        p_cfg->param.frame_dur = 0;
        p_add_capa->len = sizeof(app_gaf_bap_lc3p_frame_dur_ltv_t) - 1;
        p_add_capa->type = APP_GAF_LC3P_CFG_TYPE_FRAME_DURATION;

        if (frame_dur_bf & APP_GAF_LC3P_CAPA_FRAME_DUR_BIT_10MS)
        {
            p_add_capa->frame_dur = APP_GAF_LC3P_CFG_FRAME_DURATION_10MS;
        }
        else if (frame_dur_bf & APP_GAF_LC3P_CAPA_FRAME_DUR_BIT_7_5MS)
        {
            p_add_capa->frame_dur = APP_GAF_LC3P_CFG_FRAME_DURATION_7_5MS;
        }
        else if (frame_dur_bf & APP_GAF_LC3P_CAPA_FRAME_DUR_BIT_5MS)
        {
            p_add_capa->frame_dur = APP_GAF_LC3P_CFG_FRAME_DURATION_5MS;
        }
        else if (frame_dur_bf & APP_GAF_LC3P_CAPA_FRAME_DUR_BIT_2_5MS)
        {
            p_add_capa->frame_dur = APP_GAF_LC3P_CFG_FRAME_DURATION_2_5MS;
        }
        else
        {
            p_add_capa->frame_dur = APP_GAF_LC3P_CFG_FRAME_DURATION_10MS;
        }

        p_cfg->add_cfg.len += sizeof(app_gaf_bap_lc3p_frame_dur_ltv_t);
    }

    if (p_frame_octets_10ms != NULL)
    {
        LOG_I("LC3Plus Frame octets 10ms:%d %d",
              p_frame_octets_10ms->frame_octet_min, p_frame_octets_10ms->frame_octet_max);

        if ((p_frame_octets_10ms->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN)
                && (p_frame_octets_10ms->frame_octet_max <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
        {
            p_cfg->param.frame_octet = p_frame_octets_10ms->frame_octet_max;
        }
        else if ((p_frame_octets_10ms->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX)
                 && (p_frame_octets_10ms->frame_octet_min <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
        {
            p_cfg->param.frame_octet = APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX;
        }
    }

    if (p_frame_octets_7_5ms != NULL)
    {
        LOG_I("LC3Plus Frame octets 7.5ms:%d %d",
              p_frame_octets_7_5ms->frame_octet_min, p_frame_octets_7_5ms->frame_octet_max);

        if ((p_frame_octets_7_5ms->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN)
                && (p_frame_octets_7_5ms->frame_octet_max <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
        {
            p_cfg->param.frame_octet = co_max(p_frame_octets_7_5ms->frame_octet_max, p_cfg->param.frame_octet);
        }
        else if ((p_frame_octets_7_5ms->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX)
                 && (p_frame_octets_7_5ms->frame_octet_min <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
        {
            p_cfg->param.frame_octet = APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX;
        }
    }
#endif

    return BT_STS_SUCCESS;
}

static int app_bap_capa_cli_basic_pac_parse(app_gaf_bap_capa_param_t *param_in, app_gaf_bap_cfg_param_t *param_cfg)
{
    if ((NULL == param_in) || (NULL == param_cfg))
    {
        return BT_STS_INVALID_PARM;
    }

    param_cfg->location_bf = param_in->chan_cnt_bf;

    if ((param_in->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN)
            && (param_in->frame_octet_max <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
    {
        param_cfg->frame_octet = param_in->frame_octet_max;
    }
    else if ((param_in->frame_octet_max >= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX)
             && (param_in->frame_octet_min <= APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX))
    {
        param_cfg->frame_octet = APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX;
    }

    uint16_t sampling_freq_bf = param_in->sampling_freq_bf & APP_BAP_DFT_CAPA_C_SAMPLING_FREQ_BF;

    if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_48000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_48000HZ;
    }
    else if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_32000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_32000HZ;
    }
    else if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_24000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_24000HZ;
    }
    else if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_16000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_16000HZ;
    }
    else if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_8000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_8000HZ;
    }
    else
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_MIN;
    }

    uint16_t frame_dur_bf = param_in->frame_dur_bf & APP_BAP_DFT_CAPA_C_FRAME_DURATION_BF;

    if (frame_dur_bf & APP_GAF_BAP_FRAME_DUR_10MS_BIT)
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_10MS;
    }
    else if (frame_dur_bf & APP_GAF_BAP_FRAME_DUR_7_5MS_BIT)
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_7_5MS;
    }
    else
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_MIN;
    }

    //lc3plus nb_lc3plus_frames set
    param_cfg->frames_sdu = param_in->max_frames_sdu;

    return BT_STS_SUCCESS;
}

#ifdef LEA_LHDC
static int app_bap_capa_cli_cfg_lhdc_param_judge(app_gaf_bap_capa_param_t *param_in,
                                                    app_gaf_bap_capa_param_t *param_supp,
                                                    app_gaf_bap_cfg_param_t *param_cfg)
{
    if ((NULL == param_in) || (NULL == param_supp) || (NULL == param_cfg))
    {
        return BT_STS_INVALID_PARM;
    }

    // lhdc channel cnt bf
    /// TODO: In here , it means channel_cnt_bf
    param_cfg->location_bf = param_in->chan_cnt_bf;

    //lhdc frame_octet set
    if ((param_in->frame_octet_max >= param_supp->frame_octet_min)
            && (param_in->frame_octet_max <= param_supp->frame_octet_max))
    {
        param_cfg->frame_octet = param_in->frame_octet_max;
    }
    else if ((param_in->frame_octet_max >= param_supp->frame_octet_max)
             && (param_in->frame_octet_min <= param_supp->frame_octet_max))
    {
        param_cfg->frame_octet = param_supp->frame_octet_max;
    }

    //lhdc sampling_frequency set
    uint16_t sampling_freq_bf = param_in->sampling_freq_bf & param_supp->sampling_freq_bf;

    if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_96000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_96000HZ;
    }
    else if (sampling_freq_bf & APP_GAF_BAP_SAMPLING_FREQ_48000HZ_BIT)
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_48000HZ;
    }
    else
    {
        param_cfg->sampling_freq = APP_GAF_BAP_SAMPLING_FREQ_MIN;
    }

    //lhdc frame_duration set
    uint16_t frame_dur_bf = param_in->frame_dur_bf & param_supp->frame_dur_bf;
    if (frame_dur_bf & APP_GAF_BAP_FRAME_DUR_10MS_BIT)
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_10MS;
    }
    else if (frame_dur_bf & APP_GAF_BAP_FRAME_DUR_5MS_BIT)
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_5MS;
    }
    else if (frame_dur_bf & APP_GAF_BAP_FRAME_DUR_2_5MS_BIT)
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_2_5MS;
    }
    else
    {
        param_cfg->frame_dur = APP_GAF_BAP_FRAME_DUR_MIN;
    }

    //lhdc nb_lhdc_frames set
    param_cfg->frames_sdu = param_in->max_frames_sdu;

    return BT_STS_SUCCESS;
}
#endif

static void app_bap_pacc_cb_bond_data_evt(uint8_t con_lid, const bap_uc_cli_pacc_svc_info_t *param)
{
    LOG_D("app_bap capa cli capa bond con_lid = %d, shdl = %d, ehdl = %d, uuid = %x",
          con_lid, param->svc_range.shdl, param->svc_range.ehdl, param->uuid);
}

static void app_bap_pacc_cb_discovery_cmp_evt(uint8_t con_lid, uint16_t err_code)
{
    LOG_D("app_bap capa cli found capa service, con_lid = %d, status = %d", con_lid, err_code);
}

static void app_bap_pacc_cb_set_cfg_cmp_evt(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid,
                                            uint16_t err_code)
{
    LOG_D("app_bap capa cli set cfg cmp, con_lid = %d, char_type = %d, pac_lid = %d, status = %d",
          con_lid, char_type, pac_lid, err_code);
}

static void app_bap_pacc_cb_cfg_evt(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid,
                                    bool enabled, uint16_t err_code)
{
    LOG_D("app_bap capa cli get cfg cmp, con_lid = %d, char_type = %d, pac_lid = %d, enable_ntf = %d, status = %d",
          con_lid, char_type, pac_lid, enabled, err_code);
}

static void app_bap_pacc_cb_ava_context_value_evt(uint8_t con_lid, uint16_t sink_ava_context,
                                                  uint16_t src_ava_context, uint16_t err_code)
{
    LOG_D("app_bap capa cli ava context, status = %d, context_bf_sink = %04x, context_bf_src = %04x",
          err_code, sink_ava_context, src_ava_context);

    app_bap_capa_dir_cfg_t *dir_cfg_sink =
        &p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[APP_GAF_DIRECTION_SINK];
    app_bap_capa_dir_cfg_t *dir_cfg_src =
        &p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[APP_GAF_DIRECTION_SRC];

    dir_cfg_sink->ava_context_bf = sink_ava_context;
    dir_cfg_src->ava_context_bf = sink_ava_context;

    app_gaf_capa_cli_context_ind_t context_ind;
    context_ind.con_lid = con_lid;
    context_ind.context_type = APP_GAF_BAP_CAPA_CONTEXT_TYPE_AVA;
    context_ind.context_bf_sink = sink_ava_context;
    context_ind.context_bf_src = sink_ava_context;
    app_gaf_mobile_evt_report(APP_GAF_PACC_CONTEXT_IND, (void *)&context_ind,
                              sizeof(app_gaf_capa_cli_context_ind_t));
}

static void app_bap_pacc_cb_supp_context_value_evt(uint8_t con_lid, uint16_t sink_supp_context,
                                                   uint16_t src_supp_context, uint16_t err_code)
{
    LOG_D("app_bap capa cli supp context, status = %d, context_bf_sink = %04x, context_bf_src = %04x",
          err_code, sink_supp_context, src_supp_context);

    app_bap_capa_dir_cfg_t *dir_cfg_sink =
        &p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[APP_GAF_DIRECTION_SINK];
    POSSIBLY_UNUSED app_bap_capa_cli_supp_t *capa_info_sink = &p_app_capa_cli_env->capa_info[APP_GAF_DIRECTION_SINK];

    app_bap_capa_dir_cfg_t *dir_cfg_src =
        &p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[APP_GAF_DIRECTION_SRC];
    POSSIBLY_UNUSED app_bap_capa_cli_supp_t *capa_info_src = &p_app_capa_cli_env->capa_info[APP_GAF_DIRECTION_SRC];

    dir_cfg_sink->supp_context_bf = sink_supp_context;
    dir_cfg_src->supp_context_bf = src_supp_context;

    LOG_D("app_bap capa cli local supp context, context_bf_sink = %04x, context_bf_src = %04x",
          capa_info_sink->context_bf_supp, capa_info_src->context_bf_supp);

    app_gaf_capa_cli_context_ind_t context_ind;
    context_ind.con_lid = con_lid;
    context_ind.context_type = APP_GAF_BAP_CAPA_CONTEXT_TYPE_SUPP;
    context_ind.context_bf_sink = sink_supp_context;
    context_ind.context_bf_src = src_supp_context;
    app_gaf_mobile_evt_report(APP_GAF_PACC_CONTEXT_IND, (void *)&context_ind,
                              sizeof(app_gaf_capa_cli_context_ind_t));
}

static void app_bap_pacc_cb_location_value_evt(uint8_t con_lid, uint8_t direction,
                                               uint32_t location_bf, uint16_t err_code)
{
    LOG_D("app_bap capa cli capa location: con_lid = %d, status = %d, direction = %d, loc_bf = %08x",
          con_lid, err_code, direction, location_bf);

    app_bap_capa_dir_cfg_t *dir_cfg = &p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction];
    POSSIBLY_UNUSED app_bap_capa_cli_supp_t *capa_info = &p_app_capa_cli_env->capa_info[direction];

    dir_cfg->location_bf = location_bf;

    LOG_D("app_bap capa cli local supp location: con_lid = %d, direction = %d, loc_bf = %08x", con_lid,
          direction, capa_info->location_bf);

    app_gaf_capa_cli_location_ind_t loc_ind;
    loc_ind.con_lid = con_lid;
    loc_ind.direction = direction;
    loc_ind.location_bf = location_bf;
    app_gaf_mobile_evt_report(APP_GAF_PACC_LOCATION_IND, (void *)&loc_ind,
                              sizeof(app_gaf_capa_cli_location_ind_t));
}

static void app_bap_pacc_cb_chan_capa_record_value_evt(uint8_t con_lid, uint8_t num_record, uint8_t record_lid,
                                                       uint32_t channel_type, const pacs_chan_capa_desc_u *p_desc, uint16_t err_code)
{
    LOG_I("app_bap capa cli chan capa: con_lid = %d, num_record = %d rec_lid = %d, chan_type = 0x%x",
          con_lid, num_record, record_lid, channel_type);

    if (err_code == BT_STS_SUCCESS && p_desc != NULL)
    {
        DUMP8_D(p_desc, sizeof(*p_desc));
    }
}

static void app_bap_pacc_cb_pref_aud_cfg_record_value_evt(uint8_t con_lid, uint8_t num_record, uint8_t record_lid,
                                                          uint16_t use_case_id, const pacs_pref_aud_cfg_data_set_ptr_t *p_pref_aud_cfg, uint16_t err_code)
{
    LOG_I("app_bap capa cli pref aud cfg: con_lid = %d, num_record = %d rec_lid = %d, use_case_id = 0x%x",
          con_lid, num_record, record_lid, use_case_id);

    if (err_code == BT_STS_SUCCESS && p_pref_aud_cfg != NULL)
    {
        LOG_I("pref data present bf = 0x%02x ", p_pref_aud_cfg->data_present_bits);
    }
}

static void app_bap_pacc_cb_pac_record_value_evt(uint8_t con_lid, uint8_t direction, uint8_t pac_lid, uint8_t num_record,
                                                 uint8_t rec_lid, const pacc_pac_set_record_t *pac_record, uint16_t err_code)
{
    app_bap_capa_record_cfg_t *p_record_cfg = NULL;
    const uint8_t *p_codec_id = NULL;
    const gen_aud_capa_ptr_t *p_capa_codec = NULL;
    const gen_aud_metadata_ptr_t *p_metadata = NULL;

    LOG_I("app_bap capa cli pac ind: con_lid = %d, pac_lid = %d, num_record = %d rec_lid = %d", con_lid,
          pac_lid, num_record, rec_lid);

    if (err_code != BT_STS_SUCCESS)
    {
        LOG_D("app_bap capa cli capa record ind, err_code = %d", err_code);
        return;
    }

    p_codec_id = pac_record->p_codec_id;
    p_capa_codec = pac_record->p_capa_codec;
    p_metadata = pac_record->p_metadata;

    uint16_t status = BT_STS_SUCCESS;

    do
    {
        app_bap_capa_cfg_t *capa_cfg = &p_app_capa_cfg_env->capa_cfg[con_lid];

        uint8_t pac_record_prev = capa_cfg->dir_cfg[APP_GAF_DIRECTION_SINK].nb_pacs +
                                  capa_cfg->dir_cfg[APP_GAF_DIRECTION_SRC].nb_pacs;

        p_record_cfg = (app_bap_capa_record_cfg_t *)app_gaf_malloc_buff(sizeof(app_bap_capa_record_cfg_t));

        if (NULL == p_record_cfg)
        {
            status = BT_STS_NO_RESOURCES;
            break;
        }

        memset((void *)p_record_cfg, 0, sizeof(app_bap_capa_record_cfg_t));
        INIT_LIST_HEAD(&p_record_cfg->hdr);

        p_record_cfg->cfg_id = rec_lid;

        app_gaf_capa_cli_record_ind_t pac_record_evt =
        {
            .con_lid = con_lid,
            .pac_lid = pac_lid,
            .nb_records = 1,
            .record_lid = rec_lid,
        };

        //****************************CODEC ID********************************
        memcpy(p_record_cfg->codec_id.codec_id, p_codec_id, GEN_AUD_CODEC_ID_LEN);

        pac_record_evt.codec_id = p_record_cfg->codec_id;

        //****************************CODEC CFG(CAPA TRANS)*******************

        uint8_t add_param_len = p_capa_codec->add_capa_param_ptr.data != NULL ?
                                p_capa_codec->add_capa_param_ptr.len : 0;

        p_record_cfg->p_cfg = (app_gaf_bap_cfg_t *)app_gaf_malloc_buff(sizeof(app_gaf_bap_cfg_t) + add_param_len);

        if (p_record_cfg->p_cfg == NULL)
        {
            status = BT_STS_NO_RESOURCES;
            break;
        }

        pac_record_evt.add_capa_len = add_param_len;

        memset(p_record_cfg->p_cfg, 0, sizeof(app_gaf_bap_cfg_t) + add_param_len);

        LOG_D("app_bap capa cli remote supported Codec Info:");
        app_bap_capa_param_print((app_gaf_bap_capa_param_t *)&p_capa_codec->basic_capa_param);

        if (add_param_len != 0)
        {
            DUMP8_D(p_capa_codec->add_capa_param_ptr.data, p_capa_codec->add_capa_param_ptr.len);
            p_record_cfg->p_cfg->add_cfg.len = p_capa_codec->add_capa_param_ptr.len;
            memcpy(p_record_cfg->p_cfg->add_cfg.data, p_capa_codec->add_capa_param_ptr.data,
                   p_capa_codec->add_capa_param_ptr.len);
        }

        // Basic capa parse first
        app_bap_capa_cli_basic_pac_parse((app_gaf_bap_capa_param_t *)
                                         &p_capa_codec->basic_capa_param, &p_record_cfg->p_cfg->param);
        // Vendor capa is set in additional capa
        app_bap_capa_cli_vendor_pac_parse(&p_record_cfg->p_cfg->add_cfg, p_record_cfg->p_cfg);

        p_record_cfg->cfg_len = sizeof(app_gaf_bap_cfg_t) + p_record_cfg->p_cfg->add_cfg.len;

        //****************************METADATA********************************
        add_param_len = gen_aud_codec_calc_metadata_ltv_list_value_len(&p_metadata->parsed_metadata_ptr);

        p_record_cfg->p_metadata = (app_gaf_bap_cfg_metadata_t *)
                                   app_gaf_malloc_buff(sizeof(app_gaf_bap_cfg_metadata_t) + add_param_len);

        if (p_record_cfg->p_metadata == NULL)
        {
            status = BT_STS_NO_RESOURCES;
            break;
        }

        memset(p_record_cfg->p_metadata, 0, sizeof(app_gaf_bap_cfg_metadata_t) + add_param_len);

        LOG_D("app_bap capa cli remote metadata Info: prefer_audio_context_bf = 0x%04x",
              p_metadata->basic_metadata.preferred_audio_context);

        p_record_cfg->p_metadata->param.context_bf = p_metadata->basic_metadata.preferred_audio_context;

        p_record_cfg->metadata_len = sizeof(app_gaf_bap_cfg_metadata_t) +
                                     gen_aud_codec_memcpy_s_metadata_ltv_list_value(&p_metadata->parsed_metadata_ptr,
                                                                                    &p_record_cfg->p_metadata->add_metadata.data[0],
                                                                                    add_param_len);
        DUMP8_D(p_record_cfg->p_metadata->add_metadata.data, add_param_len);

        LOG_D("app_bap capa cli metadata cfg:");
        app_bap_cfg_metadata_print(p_record_cfg->p_metadata);

        if (capa_cfg->p_pac_cfg[pac_lid] == NULL)
        {
            LOG_I("app_bap capa cli new pac, direction = %d, pac_lid = %d", direction, pac_lid);

            capa_cfg->p_pac_cfg[pac_lid] = (app_bap_capa_pac_cfg_t *)
                                           app_gaf_malloc_buff(sizeof(app_bap_capa_pac_cfg_t));

            if (capa_cfg->p_pac_cfg[pac_lid] == NULL)
            {
                status = BT_STS_NO_RESOURCES;
                break;
            }

            memset(capa_cfg->p_pac_cfg[pac_lid], 0, sizeof(app_bap_capa_pac_cfg_t));
            INIT_LIST_HEAD(&capa_cfg->p_pac_cfg[pac_lid]->list_cfg);
            // Increase PAC number specified by direction
            capa_cfg->dir_cfg[direction].nb_pacs++;
        }

        colist_addto_tail(&p_record_cfg->hdr, &capa_cfg->p_pac_cfg[pac_lid]->list_cfg);

        pac_record_evt.add_metadata_len = p_record_cfg->metadata_len;

        app_gaf_mobile_evt_report(APP_GAF_PACC_PAC_RECORD_IND, (void *)&pac_record_evt,
                                  sizeof(pac_record_evt));

        // If previous pac record number is 0, means just discovery cmp
        if (pac_record_prev == 0)
        {
            app_gaf_capa_operation_cmd_ind_t op_cmp_ind;
            op_cmp_ind.con_lid = con_lid;

            app_gaf_mobile_evt_report(APP_GAF_PACC_DISCOVERY_CMP_IND,
                                      (void *)&op_cmp_ind, sizeof(app_gaf_capa_operation_cmd_ind_t));
        }
    } while (0);

    if (status != BT_STS_SUCCESS)
    {
        if (p_record_cfg != NULL)
        {
            if (p_record_cfg->p_cfg != NULL)
            {
                app_gaf_free_buff(p_record_cfg->p_cfg);
            }

            if (p_record_cfg->p_metadata != NULL)
            {
                app_gaf_free_buff(p_record_cfg->p_metadata);
            }

            app_gaf_free_buff(p_record_cfg);
        }
    }

    return;
}

static void app_bap_pacc_cb_pac_v2_record_value_evt(uint8_t con_lid, uint8_t direction, uint8_t pac_lid,
                                                    uint8_t pac_set_id, uint8_t num_record, uint8_t rec_lid,
                                                    const pacc_pac_set_record_t *pac_record, uint16_t err_code)
{
    const uint8_t *p_codec_id = NULL;
    const gen_aud_capa_ptr_t *p_capa_codec = NULL;
    const gen_aud_metadata_ptr_t *p_metadata = NULL;

    LOG_I("app_bap capa cli pac v2: con_lid = %d, pac_lid = %d, pac_set_id = %d, num_record = %d rec_lid = %d",
          con_lid, pac_lid, pac_set_id, num_record, rec_lid);

    if (err_code != BT_STS_SUCCESS)
    {
        LOG_D("app_bap capa cli capa v2 record ind, err_code = %d", err_code);
        return;
    }

    p_codec_id = pac_record->p_codec_id;
    p_capa_codec = pac_record->p_capa_codec;
    p_metadata = pac_record->p_metadata;

    gen_aud_codec_capa_ptr_print(p_codec_id, p_capa_codec, p_metadata);
}

static void app_bap_pacc_cb_set_location_cmp_evt(uint8_t con_lid, uint8_t direction,
                                                 uint16_t err_code)
{
    LOG_D("app_bap capa cli set location cmp, con_lid = %d, direction = %d, status = %d", con_lid,
          direction, err_code);
}

static void app_bap_pacc_cleanup_pac_record(uint8_t con_lid)
{
    app_bap_capa_cfg_t *capa_cfg = &p_app_capa_cfg_env->capa_cfg[con_lid];

    app_bap_capa_pac_cfg_t *p_pac_cfg = NULL;
    app_bap_capa_record_cfg_t *record_cfg = NULL;
    uint8_t pac_lid = 0;

    for (; pac_lid < ARRAY_SIZE(capa_cfg->p_pac_cfg); pac_lid++)
    {
        p_pac_cfg = capa_cfg->p_pac_cfg[pac_lid];

        if (p_pac_cfg == NULL)
        {
            continue;
        }

        struct list_node *p, *n;
        struct list_node *pac_rec_list = &p_pac_cfg->list_cfg;

        // Check all pac record in this list
        colist_iterate_safe(p, n, pac_rec_list)
        {
            record_cfg = colist_structure(p, app_bap_capa_record_cfg_t, hdr);

            if (record_cfg->p_cfg != NULL)
            {
                app_gaf_free_buff(record_cfg->p_cfg);
            }

            if (record_cfg->p_metadata != NULL)
            {
                app_gaf_free_buff(record_cfg->p_metadata);
            }

            colist_delete(p);
            app_gaf_free_buff(record_cfg);
        }

        capa_cfg->p_pac_cfg[pac_lid] = NULL;

        app_gaf_free_buff(p_pac_cfg);
    }

    memset(capa_cfg, 0, sizeof(*capa_cfg));
}

static void app_bap_pacc_cb_prf_status_event(uint8_t con_lid, bool is_central, gatt_prf_status_event_e event)
{
    if (event == GATT_PRF_STATUS_EVENT_CLOSED && is_central == true)
    {
        app_bap_pacc_cleanup_pac_record(con_lid);
    }
    else if (event == GATT_PRF_STATUS_EVENT_OPENED && is_central == true)
    {
        //pacc_service_discovery(con_lid);
    }
}

static const bap_uc_cli_pacc_evt_cb_t pacc_evt_cb =
{
    .cb_bond_data = app_bap_pacc_cb_bond_data_evt,
    .cb_discovery_cmp = app_bap_pacc_cb_discovery_cmp_evt,
    .cb_set_cfg_cmp = app_bap_pacc_cb_set_cfg_cmp_evt,
    .cb_cfg_value = app_bap_pacc_cb_cfg_evt,
    .cb_ava_context_value = app_bap_pacc_cb_ava_context_value_evt,
    .cb_supp_context_value = app_bap_pacc_cb_supp_context_value_evt,
    .cb_location_bf_value = app_bap_pacc_cb_location_value_evt,
    .cb_chan_capa_record_value = app_bap_pacc_cb_chan_capa_record_value_evt,
    .cb_pref_aud_cfg_record_value = app_bap_pacc_cb_pref_aud_cfg_record_value_evt,
    .cb_pac_record_value = app_bap_pacc_cb_pac_record_value_evt,
    .cb_pac_v2_record_value = app_bap_pacc_cb_pac_v2_record_value_evt,
    .cb_set_location_cmp = app_bap_pacc_cb_set_location_cmp_evt,
    .cb_prf_status_event = app_bap_pacc_cb_prf_status_event,
};

int app_bap_capa_cli_init(void)
{
    LOG_I("%s", __func__);

    pacc_init_cfg_t init_cfg =
    {
        .max_supp_sink_pac = APP_BAP_DFT_CAPA_C_SINK_SUPP_NUN,
        .max_supp_src_pac = APP_BAP_DFT_CAPA_C_SRC_SUPP_NUN,
#if (BLE_SHP_CLIENT_SUPPORT)
        .max_supp_sink_pac_v2 = 1,
        .max_supp_src_pac_v2 = 1,
#endif /* BLE_SHP_CLIENT_SUPPORT */
    };

    return bap_uc_cli_pacc_init(&init_cfg, &pacc_evt_cb);
}

int app_bap_capa_cli_deinit(void)
{
    LOG_I("%s", __func__);

    return bap_uc_cli_pacc_deinit();
}

int app_bap_capa_cli_info_init(void)
{
    if (p_app_capa_cli_env != NULL)
    {
        return BT_STS_SUCCESS;
    }

    uint16_t status = BT_STS_SUCCESS;

    do
    {
        // Environment size
        uint16_t env_size = sizeof(app_bap_capa_cli_env_t);
        app_bap_capa_cli_supp_t *capa_info = NULL;
        // Allocate environment
        p_app_capa_cli_env = (app_bap_capa_cli_env_t *)app_gaf_malloc_buff(env_size);
        if (NULL == p_app_capa_cli_env)
        {
            LOG_D("app_bap capa cli init malloc error");
            status = BT_STS_NO_RESOURCES;
            break;
        }

        // Initialize environment content
        memset(p_app_capa_cli_env, 0, env_size);

        capa_info = &p_app_capa_cli_env->capa_info[APP_GAF_DIRECTION_SINK];
        capa_info->location_bf = APP_BAP_DFT_CAPA_C_SINK_LOCATION_BF;
        capa_info->context_bf_supp = APP_BAP_DFT_CAPA_C_SINK_CONTEXT_BF;

        capa_info = &p_app_capa_cli_env->capa_info[APP_GAF_DIRECTION_SRC];
        capa_info->location_bf = APP_BAP_DFT_CAPA_C_SRC_LOCATION_BF;
        capa_info->context_bf_supp = APP_BAP_DFT_CAPA_C_SRC_CONTEXT_BF;

        p_app_capa_cfg_env = (app_bap_capa_cfg_env_t *)app_gaf_malloc_buff(sizeof(app_bap_capa_cfg_env_t));

        if (NULL == p_app_capa_cfg_env)
        {
            LOG_E("app_bap capa cli capa_cfg malloc error");
            status = BT_STS_NO_RESOURCES;
            break;
        }

        memset(p_app_capa_cfg_env, 0, sizeof(*p_app_capa_cfg_env));
    } while (0);

    if (status != BT_STS_SUCCESS)
    {
        if (p_app_capa_cli_env != NULL)
        {
            // Free the environment
            app_gaf_free_buff(p_app_capa_cli_env);
        }

        p_app_capa_cli_env = NULL;
    }

    return status;
}

int app_bap_capa_cli_info_deinit(void)
{
    if (p_app_capa_cli_env == NULL)
    {
        return BT_STS_SUCCESS;
    }

    uint8_t con_lid = 0;

    for (con_lid = 0; con_lid < sizeof(p_app_capa_cfg_env->capa_cfg[con_lid]); con_lid++)
    {
        app_bap_pacc_cleanup_pac_record(con_lid);
    }

    app_gaf_free_buff(p_app_capa_cfg_env);

    p_app_capa_cfg_env = NULL;

    app_gaf_free_buff(p_app_capa_cli_env);

    p_app_capa_cli_env = NULL;

    return BT_STS_SUCCESS;
}

int app_bap_capa_cli_start(uint8_t con_lid)
{
    //Once ble connection established, discovery bap PACS services.
    LOG_D("app_bap capa cli start discovery PACS, con_lid = %d", con_lid);
    return bap_uc_cli_pacs_discovery(con_lid);
}

int app_bap_capa_cli_get_cfg(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid)
{
    LOG_D("%s does not support", __func__);
    return 0;
}

int app_bap_capa_cli_set_cfg(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid, uint8_t enable)
{
    return bap_uc_cli_write_pacs_cccd(con_lid, char_type, pac_lid, enable);
}

int app_bap_capa_cli_get(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid)
{
    uint16_t status = BT_STS_SUCCESS;

    if (char_type == PACS_CHAR_TYPE_LOC_SINK)
    {
        status = bap_uc_cli_read_audio_location(con_lid, BAP_DIRECTION_SINK);
    }
    else if (char_type == PACS_CHAR_TYPE_LOC_SRC)
    {
        status = bap_uc_cli_read_audio_location(con_lid, BAP_DIRECTION_SRC);
    }
    else if (char_type == PACS_CHAR_TYPE_CONTEXT_AVA)
    {
        status = bap_uc_cli_read_ava_audio_context(con_lid);
    }
    else if (char_type == PACS_CHAR_TYPE_CONTEXT_SUPP)
    {
        status = bap_uc_cli_read_supp_audio_context(con_lid);
    }
    else if (char_type == PACS_CHAR_TYPE_SINK_PAC)
    {
        status = bap_uc_cli_read_pac_value(con_lid, BAP_DIRECTION_SINK, pac_lid);
    }
    else if (char_type == PACS_CHAR_TYPE_SRC_PAC)
    {
        status = bap_uc_cli_read_pac_value(con_lid, BAP_DIRECTION_SRC, pac_lid);
    }
    else
    {
        status = BT_STS_INVALID_PARM;
    }

    return status;
}

int app_bap_capa_cli_set_remote_location_bf(uint8_t con_lid, uint8_t direction,
                                            uint32_t location_bf)
{
    return bap_uc_cli_write_audio_location(con_lid, direction, location_bf);
}

app_bap_capa_record_cfg_t *app_bap_capa_cli_get_pac_record(uint8_t con_lid, uint8_t direction,
                                                           const void *codec_id, uint8_t sampleRate)
{
    if (con_lid > BLE_CONNECTION_MAX)
    {
        return NULL;
    }

    app_bap_capa_pac_cfg_t *p_pac_cfg = NULL;
    app_bap_capa_record_cfg_t *record_cfg = NULL;
    uint8_t pacs_idx = 0;
    uint8_t pacs_idx_shift = (direction == APP_GAF_DIRECTION_SINK) ? 0 : APP_BAP_DFT_CAPA_C_SINK_SUPP_NUN;

    LOG_D("app bap pick codc capa direction: %s, exist: %d",
          (direction == APP_GAF_DIRECTION_SINK) ? "SINK" : "SRC",
          p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].nb_pacs);

    for (; pacs_idx < (p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].nb_pacs); pacs_idx++)
    {
        p_pac_cfg = p_app_capa_cfg_env->capa_cfg[con_lid].p_pac_cfg[pacs_idx + pacs_idx_shift];

        if (p_pac_cfg == NULL)
        {
            continue;
        }

        struct list_node *p, *n;
        struct list_node *pac_rec_list = &p_pac_cfg->list_cfg;

        // Check all pac record in this list
        colist_iterate_safe(p, n, pac_rec_list)
        {
            record_cfg = colist_structure(p, app_bap_capa_record_cfg_t, hdr);

#if defined (AOB_SPLIT_PAC_RECORD_INTO_FOUR_RECORDS)
            if (record_cfg->p_cfg->param.sampling_freq == sampleRate)
#endif
            {
                if (app_bap_codec_is_lc3(&record_cfg->codec_id) && app_bap_codec_is_lc3(codec_id))
                {
                    LOG_I("app_bap find the LC3 pac record, sampling freq = %d", sampleRate);
                    break;
                }
#ifdef LC3PLUS_SUPPORT
                else if (app_bap_codec_is_lc3plus(&record_cfg->codec_id) && app_bap_codec_is_lc3plus(codec_id))
                {
                    LOG_I("app_bap find the LC3Plus pac record, sampling freq = %d", sampleRate);
                    break;
                }
#endif
#ifdef LEA_LHDC
                else if (app_bap_codec_is_lhdc(&record_cfg->codec_id) && app_bap_codec_is_lhdc(codec_id))
                {
                    LOG_I("app_bap find the lhdc pac record, sampling freq = %d", sampleRate);
                    break;
                }
#endif
            }
        }

        if (NULL != record_cfg)
        {
            break;
        }
    }

    return record_cfg;
}

int app_bap_capa_get_peer_audio_location_bf(uint8_t con_lid, uint8_t direction)
{
    return p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].location_bf;
}

bool app_bap_capa_cli_is_peer_support_stereo_channel(uint8_t con_lid, uint8_t direction)
{
    LOG_D("%s audio location bf: 0x%x", __func__,
          p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].location_bf);
    return (app_bap_get_audio_location_l_r_cnt(
                p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].location_bf) == 2);
}

uint16_t app_bap_capa_cli_get_ava_context_bf(uint8_t con_lid, uint8_t direction)
{
    if (NULL == p_app_capa_cfg_env)
    {
        LOG_D("%s err not init", __func__);
        return 0;
    }

    uint16_t ava_context_bf = p_app_capa_cfg_env->capa_cfg[con_lid].dir_cfg[direction].ava_context_bf;
    return ava_context_bf;
}

bool app_bap_capa_cli_is_codec_capa_support_stereo_channel(uint8_t con_lid, uint8_t direction,
                                                           const void *codec_id,
                                                           uint8_t sampleRate)
{
    // Get specific Pac Record
    app_bap_capa_record_cfg_t *record_cfg = app_bap_capa_cli_get_pac_record(con_lid, direction,
                                                                            codec_id, sampleRate);
    if (NULL == record_cfg)
    {
        LOG_D("app_bap uc cli pick codec cfg failed");
        return false;
    }

    /// Get audio location count
    /// TODO: In here, record_cfg->p_cfg->param.location_bf means channel_cnt_bf
    return (app_bap_get_max_audio_channel_supp_cnt(record_cfg->p_cfg->param.location_bf) == 2);
}

app_bap_capa_cli_supp_t *app_bap_capa_cli_get_capa_info(enum app_gaf_direction direction)
{
    return &p_app_capa_cli_env->capa_info[direction];
}

int app_bap_capa_cli_set_supp_location_bf(enum app_gaf_direction direction, uint32_t location_bf)
{
    p_app_capa_cli_env->capa_info[direction].location_bf = location_bf;
    return BT_STS_SUCCESS;
}

int app_bap_capa_cli_set_supp_context_bf(enum app_gaf_direction direction, uint16_t context_bf_supp)
{
    p_app_capa_cli_env->capa_info[direction].context_bf_supp = context_bf_supp;
    return BT_STS_SUCCESS;
}

#endif
#endif

/// @} APP
