/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __BT_PATCH_GAF_H__
#define __BT_PATCH_GAF_H__

#if defined (BUILD_BTH_ROM)

#include "gaf_cfg.h"
#include "gatt_i.h"
#include "bap_unicast_client.h"
#include "bap_broadcast_common.h"

#define BTH_PATCH_GAF_FUNC_ID(func)  FUNC_ID_##gaf_##func

/// Function table
extern uint32_t g_patch_tbl_gaf[];

/// Function enumeration
typedef enum
{
#if (ACC_MCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(mcs_prepare_service_attr_list),
#endif
#if (ACC_TBS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(tbs_prepare_service_attr_list),
#endif
#if (ACC_OTS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ots_prepare_service_attr_list),
#endif
    BTH_PATCH_GAF_FUNC_ID(csis_prepare_service_attr_list),
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ascs_send_notify),
#endif
    BTH_PATCH_GAF_FUNC_ID(asc_common_ase_state_changes_check),
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ascc_write_ase_cp),
#endif
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(bap_uc_srv_before_set_ase_state),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_srv_queued_disconnect_cis),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_srv_iso_cb_cis_conn_request),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_before_set_ase_state),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_prepare_cis_hdl_to_create_cis),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_queued_create_cis),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_queued_disconnect_cis),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_loop_prepare_ase_cp_wr_buf),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_before_cfg_uc_stream_grp),
#endif
#if (BAP_SCAN_DELEGATOR)
    BTH_PATCH_GAF_FUNC_ID(bap_bc_scan_deleg_is_bc_rx_src_proc_busy),
    BTH_PATCH_GAF_FUNC_ID(bap_scan_deleg_get_total_sub_grp_bis_sync_req_bf),
    BTH_PATCH_GAF_FUNC_ID(bap_scan_deleg_before_send_bc_rx_state_ntf),
#endif
#if (ACC_MCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(mcs_gatt_server_callback),
#endif
#if (ACC_OTS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ots_gatt_server_callback),
#endif
#if (ACC_TBS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(tbs_gatt_server_callback),
#endif
#if (ATC_CSISM_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(csis_gatt_server_callback),
#endif
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ascs_gatt_server_callback),
#endif
#if (BAP_BASS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(bass_gatt_server_callback),
#endif
#if (BAP_PACS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(pacs_gatt_server_callback),
#endif
#if (ARC_AICS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(aics_gatt_server_callback),
#endif
#if (ARC_VOCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(vocs_gatt_server_callback),
#endif
#if (ARC_VCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(vcs_gatt_server_callback),
#endif
#if (ARC_MICS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(mics_gatt_server_callback),
#endif
#if (CAP_CAS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(cas_gatt_server_callback),
#endif
#if (GMAP_GMAS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(gmas_gatt_server_callback),
#endif
#if (HAP_HAS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(has_gatt_server_callback),
#endif
#if (TMAP_TMAS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(tmas_gatt_server_callback),
#endif
#if (ACC_MCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(mcc_gatt_callback),
#endif
#if (ACC_OTC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(otc_gatt_callback),
#endif
#if (ACC_TBC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(tbc_gatt_callback),
#endif
#if (ARC_AICC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(aicc_gatt_callback),
#endif
#if (ARC_MICC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(micc_gatt_callback),
#endif
#if (ARC_VCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(vcc_gatt_callback),
#endif
#if (ARC_VOCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(vocc_gatt_callback),
#endif
    BTH_PATCH_GAF_FUNC_ID(csisc_gatt_callback),
#if (BAP_BROADCAST_ASSIST)
    BTH_PATCH_GAF_FUNC_ID(bap_bc_assist_gatt_callback),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(ascc_gatt_callback),
#endif
#if (BAP_PACC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(pacc_gatt_callback),
#endif
#if (CAP_CAC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(cac_gatt_callback),
#endif
#if (GMAP_GMAC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(gmac_gatt_callback),
#endif
#if (HAP_HAC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(hac_gatt_callback),
#endif
#if (TMAP_TMAC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(tmac_gatt_callback),
#endif
    BTH_PATCH_GAF_FUNC_ID(gen_aud_codec_cfg_check_param),
    BTH_PATCH_GAF_FUNC_ID(gen_aud_codec_capa_check_param),
    BTH_PATCH_GAF_FUNC_ID(gaf_log_func_internal),
    BTH_PATCH_GAF_FUNC_ID(gaf_log_dump_internal),
    BTH_PATCH_GAF_FUNC_ID(gaf_prf_alloc_con_lid_for_btgatt),
    BTH_PATCH_GAF_FUNC_ID(gen_aud_codec_is_id_valid),
#if (BAP_ASCS_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(bap_uc_srv_bap_stream_event_callback),
#endif
#if (BAP_ASCC_ENABLE)
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_stream_event_callback),
    BTH_PATCH_GAF_FUNC_ID(bap_uc_cli_is_cig_meet_new_requirements),
#endif
} bt_patch_gaf_func_enum_t;

/// Function extern
extern uint8_t gaf_prf_alloc_con_lid_for_btgatt(uint8_t conidx, uint16_t event);
#if (ACC_MCS_ENABLE)
extern uint16_t mcs_prepare_service_attr_list(bool is_gmcs, gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len, uint8_t transfer_lid);
#endif /// (ACC_MCS_ENABLE)
#if (ACC_TBS_ENABLE)
extern uint16_t tbs_prepare_service_attr_list(bool is_gtbs, gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#endif /// (ACC_TBS_ENABLE)
#if (ACC_OTS_ENABLE)
extern uint16_t ots_prepare_service_attr_list(gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#endif /// (ACC_OTS_ENABLE)
extern uint16_t csis_prepare_service_attr_list(gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#if (BAP_ASCS_ENABLE)
extern uint16_t ascs_send_notify(uint8_t con_lid, uint8_t ase_inst_id, const uint8_t *character, const uint8_t *ntf_buf, uint16_t buf_len);
#endif /// (BAP_ASCS_ENABLE)
#if (BAP_ASCS_ENABLE) || (BAP_ASCC_ENABLE)
extern bool asc_common_ase_state_changes_check(uint8 opcode, uint8_t direction, uint8_t old_state, uint8_t new_state);
#endif /// (BAP_ASCS_ENABLE) || (BAP_ASCC_ENABLE)
#if (BAP_ASCC_ENABLE)
extern uint16_t ascc_write_ase_cp(uint8_t con_lid, uint8_t *val, uint8_t val_len, uint16_t ase_op_bf);
#endif /// (BAP_ASCC_ENABLE)
#if (BAP_ASCS_ENABLE)
extern uint16_t bap_uc_srv_before_set_ase_state(void *p_ase, uint8_t ase_state, bool *continue_handle);
extern uint16_t bap_uc_srv_queued_disconnect_cis(uint16_t cis_hdl);
extern void bap_uc_srv_iso_cb_cis_conn_request(uint8_t con_lid, uint8_t cig_id, uint8_t cis_id, uint16_t cis_hdl);
#endif /// (BAP_ASCS_ENABLE)
#if (BAP_ASCC_ENABLE)
extern uint16_t bap_uc_cli_before_set_ase_state(void *p_ase, uint8_t ase_state, const uint8_t *ase_state_param, uint8_t param_len, bool *continue_handle);
extern uint16_t bap_uc_cli_prepare_cis_hdl_to_create_cis(uint8_t con_lid, uint16_t *cis_hdl_list, uint8_t *cis_count);
extern uint16_t bap_uc_cli_queued_create_cis(uint8_t con_lid, uint8_t cis_count, uint16_t *p_cis_hdl);
extern uint16_t bap_uc_cli_queued_disconnect_cis(uint16_t cis_hdl);
extern uint16_t bap_uc_cli_loop_prepare_ase_cp_wr_buf(uint8_t op_code, uint8_t ase_index, const bap_uc_cli_ase_op_param_t *p_ase_param,
                                                             uint8_t *wr_cp_buf_len, uint8_t **p_wr_cp_buf);
extern uint16_t bap_uc_cli_before_cfg_uc_stream_grp(void *p_grp, uint8_t ase_num, const bap_uc_cli_ase_op_param_t *p_ase_param, bool *continue_handle);
#endif /// (BAP_ASCC_ENABLE)
#if (BAP_SCAN_DELEGATOR)
extern bool bap_bc_scan_deleg_is_bc_rx_src_proc_busy(void);
extern uint32_t bap_scan_deleg_get_total_sub_grp_bis_sync_req_bf(uint8_t num_sub_grp, bap_bc_sub_grp_t *p_sub_grp);
extern uint16_t bap_scan_deleg_before_send_bc_rx_state_ntf(void *p_rx_src, bool *continue_handle);
#endif /// (BAP_SCAN_DELEGATOR)
extern bool mcs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool ots_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool tbs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool csis_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool ascs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool bass_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool pacs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool aics_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool vocs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool vcs_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool mics_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool cas_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool gmas_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool has_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool tmas_gatt_server_callback(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
extern bool mcc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool otc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool tbc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool aicc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool micc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool vcc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool vocc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool csisc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool bap_bc_assist_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool ascc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool pacc_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool cac_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool gmac_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool hac_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern bool tmac_gatt_callback(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
extern uint16_t gen_aud_codec_cfg_check_param(const uint8_t *p_codec_id, const gen_aud_codec_cfg_param_t *p_param, uint16_t *p_ltv_len);
extern uint16_t gen_aud_codec_capa_check_param(const uint8_t *p_codec_id, const gen_aud_codec_capa_param_t *p_param, uint16_t *p_ltv_len);
extern int gaf_log_func_internal(uint8_t log_lvl, const char *format, va_list var_args);
extern int gaf_log_dump_internal(const char *format, unsigned int size, const void *buffer, unsigned int count);
extern bool gen_aud_codec_is_id_valid(const uint8_t *p_codec_id);
#if (BAP_ASCS_ENABLE)
extern int bap_uc_srv_bap_stream_event_callback(uintptr_t group_id, bap_event_t event, bap_event_param_t param);
#endif /// (BAP_ASCS_ENABLE)
#if (BAP_ASCC_ENABLE)
extern int bap_uc_cli_stream_event_callback(uintptr_t group_id, bap_event_t event, bap_event_param_t param);
extern bool bap_uc_cli_is_cig_meet_new_requirements(const bap_cis_param_t *old_cis_param, const bap_cis_param_t *new_cis_param);
#endif /// (BAP_ASCC_ENABLE)

/// Function pointer typedef
typedef uint8_t (*gaf_prf_alloc_con_lid_for_btgatt_func_t)(uint8_t conidx, uint16_t event);
#if (ACC_MCS_ENABLE)
typedef uint16_t (*mcs_prepare_service_attr_list_func_t)(bool is_gmcs, gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len, uint8_t transfer_lid);
#endif /// (ACC_MCS_ENABLE)
#if (ACC_TBS_ENABLE)
typedef uint16_t (*tbs_prepare_service_attr_list_func_t)(bool is_gtbs, gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#endif /// (ACC_TBS_ENABLE)
#if (ACC_OTS_ENABLE)
typedef uint16_t (*ots_prepare_service_attr_list_func_t)(gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#endif /// (ACC_OTS_ENABLE)
typedef uint16_t (*csis_prepare_service_attr_list_func_t)(gatt_attribute_t **pp_attr_list, uint8_t *p_attr_list_len);
#if (BAP_ASCS_ENABLE)
typedef uint16_t (*ascs_send_notify_func_t)(uint8_t con_lid, uint8_t ase_inst_id, const uint8_t *character, const uint8_t *ntf_buf, uint16_t buf_len);
#endif /// (BAP_ASCS_ENABLE)
typedef bool (*asc_common_ase_state_changes_check_func_t)(uint8 opcode, uint8_t direction, uint8_t old_state, uint8_t new_state);
#if (BAP_ASCC_ENABLE)
typedef uint16_t (*ascc_write_ase_cp_func_t)(uint8_t con_lid, uint8_t *val, uint8_t val_len, uint16_t ase_op_bf);
#endif /// (BAP_ASCC_ENABLE)
#if (BAP_ASCS_ENABLE)
typedef uint16_t (*bap_uc_srv_before_set_ase_state_func_t)(void *p_ase, uint8_t ase_state, bool *continue_handle);
typedef uint16_t (*bap_uc_srv_queued_disconnect_cis_func_t)(uint16_t cis_hdl);
typedef void (*bap_uc_srv_iso_cb_cis_conn_request_func_t)(uint8_t con_lid, uint8_t cig_id, uint8_t cis_id, uint16_t cis_hdl);
#endif /// (BAP_ASCS_ENABLE)
#if (BAP_ASCC_ENABLE)
typedef uint16_t (*bap_uc_cli_before_set_ase_state_func_t)(void *p_ase, uint8_t ase_state, const uint8_t *ase_state_param, uint8_t param_len, bool *continue_handle);
typedef uint16_t (*bap_uc_cli_prepare_cis_hdl_to_create_cis_func_t)(uint8_t con_lid, uint16_t *cis_hdl_list, uint8_t *cis_count);
typedef uint16_t (*bap_uc_cli_queued_create_cis_func_t)(uint8_t con_lid, uint8_t cis_count, uint16_t *p_cis_hdl);
typedef uint16_t (*bap_uc_cli_queued_disconnect_cis_func_t)(uint16_t cis_hdl);
typedef uint16_t (*bap_uc_cli_loop_prepare_ase_cp_wr_buf_func_t)(uint8_t op_code, uint8_t ase_index, const bap_uc_cli_ase_op_param_t *p_ase_param,
                                                                 uint8_t *wr_cp_buf_len, uint8_t **p_wr_cp_buf);
typedef uint16_t (*bap_uc_cli_before_cfg_uc_stream_grp_func_t)(void *p_grp, uint8_t ase_num, const bap_uc_cli_ase_op_param_t *p_ase_param, bool *continue_handle);
#endif /// (BAP_ASCC_ENABLE)
#if (BAP_SCAN_DELEGATOR)
typedef bool (*bap_bc_scan_deleg_is_bc_rx_src_proc_busy_func_t)(void);
typedef uint32_t (*bap_scan_deleg_get_total_sub_grp_bis_sync_req_bf_func_t)(uint8_t num_sub_grp, bap_bc_sub_grp_t *p_sub_grp);
typedef uint16_t (*bap_scan_deleg_before_send_bc_rx_state_ntf_func_t)(void *p_rx_src, bool *continue_handle);
#endif /// (BAP_SCAN_DELEGATOR)
typedef bool (*mcs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*ots_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*tbs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*csis_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*ascs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*bass_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*pacs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*aics_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*vocs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*vcs_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*mics_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*cas_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*gmas_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*has_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*tmas_gatt_server_callback_func_t)(gatt_svc_t *p_svc, gatt_server_event_t event, gatt_server_callback_param_t param);
typedef bool (*mcc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*otc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*tbc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*aicc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*micc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*vcc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*vocc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*csisc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*bap_bc_assist_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*ascc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*pacc_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*cac_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*gmac_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*hac_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef bool (*tmac_gatt_callback_func_t)(gatt_prf_t *prf, gatt_profile_event_t event, gatt_profile_callback_param_t param);
typedef uint16_t (*gen_aud_codec_cfg_check_param_func_t)(const uint8_t *p_codec_id, const gen_aud_codec_cfg_param_t *p_param, uint16_t *p_ltv_len);
typedef uint16_t (*gen_aud_codec_capa_check_param_func_t)(const uint8_t *p_codec_id, const gen_aud_codec_capa_param_t *p_param, uint16_t *p_ltv_len);
typedef int (*gaf_log_func_internal_func_t)(uint8_t log_lvl, const char *format, va_list var_args);
typedef int (*gaf_log_dump_internal_func_t)(const char *format, unsigned int size, const void *buffer, unsigned int count);
typedef bool (*gen_aud_codec_is_id_valid_func_t)(const uint8_t *p_codec_id);
#if (BAP_ASCS_ENABLE)
typedef int (*bap_uc_srv_bap_stream_event_callback_func_t)(uintptr_t group_id, bap_event_t event, bap_event_param_t param);
#endif /// (BAP_ASCS_ENABLE)
#if (BAP_ASCC_ENABLE)
typedef int (*bap_uc_cli_stream_event_callback_func_t)(uintptr_t group_id, bap_event_t event, bap_event_param_t param);
typedef bool (*bap_uc_cli_is_cig_meet_new_requirements_func_t)(const bap_cis_param_t *old_cis_param, const bap_cis_param_t *new_cis_param);
#endif /// (BAP_ASCC_ENABLE)

#endif /* BUILD_BTH_ROM */

#endif /* __BT_PATCH_GAF_H__ */