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
#ifndef __BLE_GAP_I_H__
#define __BLE_GAP_I_H__

#include "gap_service.h"
#include "hci_i.h"
#include "smp_i.h"

/* DEFINES */

/* TYPEDEFINES */
typedef struct
{
    uint32_t adv_interval_min;
    uint32_t adv_interval_max;
    uint16_t adv_event_props;
    uint8_t own_addr_type;
    uint8_t peer_type;
    uint8_t adv_channel_map;
    uint8_t adv_filter_policy;
    bt_bdaddr_t local_addr;
    bt_bdaddr_t peer_addr;
    int8_t adv_tx_power;
    uint8_t primary_adv_phy;
    uint8_t primary_coded_phy_prefer;
    uint8_t secondary_adv_phy;
    uint8_t secondary_coded_phy_prefer;
    uint8_t secondary_adv_max_skip;
    uint8_t scan_req_notify_enable;
} gap_adv_set_param_t;

typedef struct
{
    bool adv_params_set;
    bool adv_enabled;
    uint8_t adv_handle;
    bool legacy_advertising;
    int8_t selected_tx_power;
    bool fast_advertising;
    bool continue_advertising;
    bool start_bg_advertising;
    bool directed_advertising;
    uint8_t activity_priority;
    uint8_t adv_stop_timer;
    bool pa_is_enabled;
    uint32_t duration_ms;
    gap_dt_buf_t adv_data;
    gap_dt_buf_t scan_rsp_data;
    // Keep it until remove adv set
    gap_decision_t decision_data;
    gap_adv_callback_t adv_callback;
    gap_adv_set_param_t set_param;
    gap_adv_timing_t timing_cfg;
} gap_advertising_t;

typedef struct
{
    bool scan_enabled;
    bool legacy_scanning;
    bool background_scan;
    bool initiating_scan;
    bt_addr_type_t own_addr_type;
    uint8_t scan_handle;
    uint8_t scan_stop_timer;
    uint16_t period_1_28s;
    uint32_t duration_ms;
    uint16_t scan_interval_slots;
    uint16_t scan_interval_coded;
    uint16_t scan_window_slots;
    uint16_t scan_window_coded;
    gap_scan_param_t param;
    gap_scan_callback_t scan_callback;
} gap_scanning_t;

typedef struct
{
    bool legacy_initiating;
    bool is_conn_creating;
    bool is_direct_initiating;
    uint8_t peer_type;
    bt_addr_type_t own_addr_type;
    bt_bdaddr_t peer_addr;
    uint32_t init_proc_timeout_ms;
    bool timeout_and_restart_initiating;
    bool start_bg_initiating;
    bool peer_is_ble_audio;
    uint8_t init_handle;
    uint8_t init_stop_timer;
    uint8_t pa_adv_handle;
    uint8_t sd_subevent;
    uint16_t scan_interval_slots;
    uint16_t scan_interval_coded;
    uint16_t scan_window_slots;
    uint16_t scan_window_coded;
    uint16_t min_conn_interval_1_25ms;  // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t max_conn_interval_1_25ms;  // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t min_conn_interval_coded;   // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t max_conn_interval_coded;   // 0x06 to 0x0C80 * 1.25ms, 7.5ms to 4s
    uint16_t superv_timeout_ms;         // 100ms to 32s
    uint16_t min_ce_length_slots;       // min len of connection event, 0x00 to 0xFFFF * 0.625ms
    uint16_t max_ce_length_slots;       // max len of connection event, 0x00 to 0xFFFF * 0.625ms
    gap_init_callback_t init_callback;
    gap_init_param_t init_param;
} gap_initiating_t;

typedef struct
{
    bt_bdaddr_t factory_bt_addr;
    bt_bdaddr_t factory_le_addr;
    bt_bdaddr_t le_static_addr;
    const bt_bdaddr_t *hci_bt_addr;
    const bt_bdaddr_t *hci_le_addr;
    const bt_bdaddr_t *hci_random_addr;
    // Used for speed up smp pairing
    // when using DHkey generation
    const uint8_t *secret_key_256;
    const uint8_t *public_key_256;
    uint32_t sign_counter;
    uint8_t irk[16];
    uint8_t csrk[16];
    uint8_t local_database_hash[16];
    uint8_t max_filter_list_size;
    uint8_t max_resolving_list_size;
    uint8_t max_pa_list_size;
    uint8_t max_adv_monitored_list_size;
    uint8_t max_num_adv_sets;
    uint8_t max_size_att_prep_q;
    uint16_t max_adv_data_len;
    uint32_t le_feat_page_0[2];
    uint32_t le_feat_page_1[1];
    uint16_t data_sign_support: 1;
    uint16_t use_random_identity_address: 1;
    uint16_t reso_address_by_host: 1;
    uint16_t address_reso_support: 1;
    uint16_t aes_encrypt_by_host: 1;
    uint16_t gatt_collect_cache: 1;
} gap_local_info_t;

typedef void (*gap_user_confirm_func_t)(void *priv, bool user_confirmed);
typedef void (*gap_data_func_t)(void *priv, const uint8_t *data);
typedef void (*gap_passkey_func_t)(void *priv, uint32_t passkey);
typedef void (*gap_oob_auth_data_callback_t)(void *priv, const gap_smp_oob_auth_data_t *data);

typedef struct gap_enc_ase_128
{
    uint8_t key[GAP_KEY_LEN];
    uint8_t data[GAP_KEY_LEN];
    hci_cmd_evt_func_t cmd_cb;
    void *priv;
    void *cont;
    void *cmpl;
} gap_enc_aes_128_t;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Get gap connection item by connhdl
 *
 * @param[in] connhdl  Connection handle
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_item(uint16_t connhdl);

/**
 * @brief Get gap connection item by bt address
 *
 * @param[in] remote   BDAddress
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_by_bt_address(const bt_bdaddr_t *remote);

/**
 * @brief Get gap connection item by ble address
 *
 * @param[in] peer_type
 *                     Peer address type
 * @param[in] peer_addr
 *                     Peer BDAddress
 *
 * @return gap_conn_item_t
 *                     Gap connection stored info
 */
gap_conn_item_t *gap_get_conn_by_le_address(bt_addr_type_t peer_type, const bt_bdaddr_t *peer_addr);

/**
 * @brief Get connection used indentity address by connection item
 *
 * @param[in] conn     Connetion item
 *
 * @return ble_bdaddr_t
 *                     ble address
 */
ble_bdaddr_t gap_conn_own_identity_address(const gap_conn_item_t *conn);

/**
 * @brief Get local le indentity address that will used for CTKD (Random or public)
 *
 * @param[in] conn     Connection item run CTKD (SMP IA Exchange)
 *
 * @return ble_bdaddr_t
 *                     ble address
 */
ble_bdaddr_t gap_ctkd_get_le_identity_address(const gap_conn_item_t *conn);

/**
 * @brief Get resolving list item by index
 *
 * @param[in] index    Resolving list item index in list
 *
 * @return const gap_resolv_item_t*
 *                     Return index's resolving list item
 */
const gap_resolv_item_t *gap_resolving_list_get_item(int index);

/**
 * @brief Get items specified by index
 *
 * @param[in] index    Item index
 *
 * @return const gap_filter_item_t*
 *                     Return item in raw device structures format
 */
const gap_filter_item_t *gap_filter_list_get_item(int index);

/**
 * @brief Remove device from resolving list status by index
 *
 * @param[in] index    Device index in resolving list
 * @param[in] check_addr_reso_switch
 *                     Check address resolution state and auto disable and enable control
 *                     Now leave it as false, we can stop all related adv no matter addr reso
 *
 * @return bt_status_t Remove device from resolving list by index status
 */
bt_status_t gap_resolving_list_remove_by_index(int index, bool check_addr_reso_switch);

bt_status_t gap_start_aes_128_encrypt(const uint8_t *key, const uint8_t *data, hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
bt_status_t gap_send_hci_le_encrypt(const uint8_t *key, const uint8_t *data, hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
bt_status_t gap_send_hci_le_rand(hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_raw_hci_cmd(uint16_t cmd_opcode, uint16_t cmd_len, const uint8_t *data_little_endian, hci_cmd_evt_func_t cmd_cb, void *priv);
bt_status_t gap_send_raw_hci_cmd_extra(uint16_t cmd_opcode, uint16_t cmd_len, const uint8_t *data_little_endian,
                                       hci_cmd_evt_func_t cmd_cb, void *priv, void *cont, void *cmpl);
gap_local_info_t *gap_local_info(void);
gap_config_t gap_get_config(void);
const uint8_t *gap_get_local_irk(void);
void gap_set_local_irk(const uint8_t *p_irk);
const uint8_t *gap_get_local_csrk(void);
ble_bdaddr_t gap_local_identity_address(bt_addr_type_t own_addr_type);
uint32_t gap_conn_bf(uint8_t conidx);
uint8_t gap_count_ble_connections(void);
uint32_t gap_local_le_features(void);
uint32_t gap_local_le_high_features(void);
bt_status_t gap_gen_local_rpa(void (*cmpl)(void *priv, const bt_bdaddr_t *local_rpa), void *priv);
bt_status_t gap_resolve_rpa(const bt_bdaddr_t *rpa, void (*cmpl)(void *priv, const gap_bond_sec_t *out), void *priv);
bt_status_t gap_generate_local_host_rpa(void *priv);
bt_status_t gap_use_aes_encrypt_by_host_enable(bool enable);
void gap_set_send_sec_error_rsp_directly(uint8_t send_sec_error_rsp);
void gap_set_report_sec_error_directly(uint8_t report_sec_error);
bool gap_dt_start_encrypt_data(const gap_key_material_t *key, const uint8_t *data, uint16_t len, gap_dt_encrypt_callback_t func, void *priv);
bool gap_dt_start_decrypt_data(const gap_key_material_t *key, const gap_enc_data_t *p, gap_key_callback_t func, void *priv);
bool gap_address_reso_is_enabled(void);
bool gap_is_refresh_adv_after_addr_reso_enable(void);
uint8_t gap_resolving_list_curr_size(void);
uint8_t gap_resolving_list_max_size(void);
bool gap_filter_list_user_item_exist(uint8_t user);
uint8_t gap_filter_list_curr_size(void);
uint8_t gap_filter_list_max_size(void);
uint8_t gap_pa_list_max_size(void);
uint8_t gap_adv_monitored_list_curr_size(void);
uint8_t gap_adv_monitored_list_max_size(void);
uint8_t gap_adv_get_adv_set_id(uint8_t adv_hdl);
gap_scan_timing_t gap_default_scan_timing(void);
gap_init_timing_t gap_default_init_timing(void);
bt_status_t gap_register_past_received_callback(const gap_pa_sync_callback_t past_recv_cb);
bool gap_stack_is_ready(void);
bool gap_dt_add_data_type(gap_dt_buf_t *buf, uint8_t type, const uint8_t *data, uint16_t len);
bool gap_dt_add_raw_data(gap_dt_buf_t *buf, const uint8_t *data, uint16_t len);
bool gap_dt_buf_is_empty(const gap_dt_buf_t *buf);
const uint8_t *gap_dt_buf_data(const gap_dt_buf_t *buf);
uint16_t gap_dt_buf_len(const gap_dt_buf_t *buf);
void gap_dt_buf_clear(gap_dt_buf_t *buf);
bool gap_dt_add_flags(gap_dt_buf_t *buf, uint8_t discoverable, bool simu_bredr_support);
const gap_dt_head_t *gap_dt_buf_find_type(const gap_dt_buf_t *buf, uint8_t ad_type, uint8_t ad_type_2);
bool gap_dt_add_local_le_name(gap_dt_buf_t *buf, bool use_legacy_pdu, const char *cust_le_name);
bool gap_set_le_service_16_uuid(gap_dt_buf_t *buf, const uint16_t *uuid, uint16_t count);
bool gap_set_le_service_128_uuid(gap_dt_buf_t *buf, const gap_uuid_128_t *uuid_le, uint16_t count);
bool gap_dt_add_service_data(gap_dt_buf_t *buf, uint16_t service_uuid, const uint8_t *data, uint8_t len);
bool gap_dt_add_tx_power(gap_dt_buf_t *buf, uint8_t tx_power_level);
bool gap_dt_add_peripheral_conn_interval(gap_dt_buf_t *buf, uint16_t min_interval_1_25ms, uint16_t max_interval_1_25ms);
bool gap_dt_add_manufacturer_data(gap_dt_buf_t *buf, uint16_t company_id, const uint8_t *data_ptr, uint8_t data_len);
void gap_dump_ble_status(void);
const char *gap_local_le_name(uint8_t *len);
const bt_bdaddr_t *gap_factory_bt_address(void);
const bt_bdaddr_t *gap_factory_le_address(void);
const bt_bdaddr_t *gap_le_static_address(void);
const bt_bdaddr_t *gap_hci_bt_address(void);
const bt_bdaddr_t *gap_hci_le_address(void);
const bt_bdaddr_t *gap_hci_random_address(void);
const bt_bdaddr_t *gap_public_identity_address(void);
const bt_bdaddr_t *gap_random_identity_address(void);
ble_bdaddr_t gap_local_identity_address(bt_addr_type_t own_addr_type);
bt_status_t gap_get_peer_oob_auth_data(gap_conn_item_t *conn);
bt_status_t gap_start_tx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_start_rx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_tx_rx_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
bt_status_t gap_start_eatt_block_authentication(gap_conn_item_t *conn, gap_who_started_auth_t who);
void gap_report_le_conn_disconnected(gap_conn_item_t *conn, uint8_t reason);
void gap_report_le_conn_cache_data(gap_conn_item_t *conn, const gatt_client_cache_t *cli_cache,
                                   const gatt_server_cache_t *srv_cache);
void gap_report_le_conn_restore_done(gap_conn_item_t *conn);
void gap_report_mtu_is_exchanged(gap_conn_item_t *conn, uint16_t mtu);
bool gap_is_directly_send_sec_error_rsp(void);
bool gap_is_directly_report_sec_error(void);
bt_status_t gap_send_hci_le_read_local_p256_public_key(hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_hci_le_gen_dhkey(const uint8_t *pkx, const uint8_t *pky, bool use_debug_private_key,
                                      hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont);
bt_status_t gap_send_hci_le_enable_encryption(gap_conn_item_t *conn, const uint8_t *rand, const uint8_t *ediv,
                                              const uint8_t *ltk, hci_cmd_evt_func_t cmd_cb, void *priv, void *cont);
bt_status_t gap_send_le_ltk_request_reply(gap_conn_item_t *conn, const uint8_t *ltk, bool positive_reply);
bt_status_t gap_ask_user_numeric_comparison(gap_conn_item_t *conn, uint32_t user_confirm_value, void *priv);
bt_status_t gap_ask_user_ltk_req_reply(gap_conn_item_t *conn, const gap_ltk_enc_info_t *p_req_enc);
bt_status_t gap_ask_input_6_digit_passkey(gap_conn_item_t *conn, gap_passkey_func_t passkey_cb, void *priv);
bt_status_t gap_ask_display_6_digit_passkey(gap_conn_item_t *conn, uint32_t passkey);
bt_status_t gap_get_tk_from_oob_data(gap_conn_item_t *conn, gap_data_func_t tk_cb, void *priv);
bt_status_t gap_get_peer_oob_auth_data(gap_conn_item_t *conn);
bt_status_t gap_get_local_oob_auth_data(gap_conn_item_t *conn);
void gap_get_smp_own_specific_ia(gap_conn_item_t *conn, ble_bdaddr_t *ia);
void gap_prepare_ble_stack(void);
void gap_gen_local_random(uint8_t *random, uint8_t bytes);
bool gap_is_pairing_method_accept(const gap_bond_sec_t *sec, uint8_t accept_method);
void gap_get_smp_own_specific_irk(gap_conn_item_t *conn, uint8_t *irk);
int gatt_conn_event_handler(uintptr_t connhdl, gap_conn_event_t event, gap_conn_callback_param_t param);
bt_status_t gatt_conn_ready_handler(gap_conn_item_t *conn);
void gatt_continue_rx_packet(gap_conn_item_t *item);
void gatt_continue_tx_packet(gap_conn_item_t *item);
bool gap_is_gatt_data_allowed_send(gap_conn_item_t *conn);
void gap_report_mtu_is_exchanged(gap_conn_item_t *conn, uint16_t mtu);
void gap_report_mtu_exchange_req(gap_conn_item_t *conn, uint16_t mtu_request);
void gap_report_smp_pairing_complete(gap_conn_item_t *conn, uint8_t err_code);
void gap_report_gatt_over_bredr_state(gap_conn_item_t *bredr_conn, bool is_connected, uint8_t reason);
void gap_get_local_secret_public_key_pair(const uint8_t **pp_sec_key_256, const uint8_t **pp_pub_key_256);
void gap_ctkd_notify_ltk_derived(gap_conn_item_t *bredr_conn, bt_addr_type_t peer_type, const bt_bdaddr_t *peer_addr, bool wait_peer_kdist);
void gap_ctkd_notify_link_key_derived(gap_conn_item_t *conn, const bt_bdaddr_t *bt_addr, const uint8_t *link_key, bool wait_peer_ia);
void gap_ltk_from_lk_generated(gap_conn_item_t *conn, const uint8_t *ltk, bool waiting_peer_kdist);
void gap_lk_from_ltk_generated(gap_conn_item_t *conn, const uint8_t *link_key, bool waiting_peer_ia);
void gap_recv_smp_encrypted(gap_conn_item_t *conn, bool new_pair, uint8_t error_code);
bool gap_recv_smp_security_request(gap_conn_item_t *conn, uint8_t auth_req);
bool gap_recv_smp_pairing_requirements(gap_conn_item_t *conn, bool is_pairing_request, const smp_requirements_t *p_smp_req);
void gap_legacy_ltk_ediv_rand_generated(gap_conn_item_t *conn);
void gap_recv_peer_legacy_ltk_ediv_rand(gap_conn_item_t *conn);
void gap_recv_peer_irk_ia(gap_conn_item_t *conn);
void gap_recv_peer_csrk(gap_conn_item_t *conn);
bool gap_is_directly_send_sec_error_rsp(void);
bool gap_is_directly_report_sec_error(void);
void gap_set_default_key_size(uint8_t key_size);
void gap_set_default_auth_req(uint8_t mitm_auth);
void gap_pts_set_ble_l2cap_test(bool test);
bool gap_is_pts_ble_l2cap_test(void);
void gap_pts_set_use_passkey_entry(void);
void gap_pts_set_use_oob_method(void);
void gap_pts_set_no_mitm_auth(void);
void gap_pts_set_display_only(void);
void gap_pts_set_keyboard_only(void);
void gap_pts_set_no_bonding(void);
void gap_pts_set_dont_start_smp(bool dont_auto_start_smp);
bool gap_pts_get_dont_start_smp(void);
void gap_pts_gen_linkkey_from_ltk(void);
void gap_pts_set_dist_irk_only(void);
void gap_pts_set_dist_csrk(void);

/**
 * bes tws fast pair host support apis
 * usage: set enable before le adv/scan
*/

void gap_vendor_enable_besfp(uint8_t *tws_bt_addr, uint8_t *peer_le_addr);
void gap_vendor_disable_besfp();
bool gap_vendor_is_besfp_enabled();
uint8_t *gap_vender_besfp_get_tws_bt_addr();
uint8_t *gap_vender_besfp_get_peer_le_addr();

#if defined(__cplusplus)
}
#endif
#endif /* __BLE_GAP_I_H__ */
