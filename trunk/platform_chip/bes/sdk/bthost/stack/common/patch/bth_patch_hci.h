
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
#ifndef __BTH_PATCH_HCI_H__
#define __BTH_PATCH_HCI_H__

#if defined (BUILD_BTH_ROM)
#include "a2dp_i.h"
#include "a2dp_common_define.h"
#include "hid_i.h"
#include "l2cap_service.h"
#include "me_api.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t g_patch_tbl_hci[];
#define BTH_PATCH_HCI_FUNC_ID(func)  FUNC_ID_##hci_##func

typedef enum
{
    BTH_PATCH_HCI_FUNC_ID(hci_handle_fc_on_disconnect),
    BTH_PATCH_HCI_FUNC_ID(hci_handle_conn_event),
    BTH_PATCH_HCI_FUNC_ID(hci_cmd_start_handle),
    BTH_PATCH_HCI_FUNC_ID(hci_rx_handle),
    BTH_PATCH_HCI_FUNC_ID(hci_send_all_pending_host_num_complete),
    BTH_PATCH_HCI_FUNC_ID(hci_rx_num_completed_packet_inc),
    BTH_PATCH_HCI_FUNC_ID(hci_create_cmd_packet_with_priv),
    BTH_PATCH_HCI_FUNC_ID(hci_cmd_wait_complete_timeout),
    BTH_PATCH_HCI_FUNC_ID(hci_free_cmd_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_cmd_end_handle),
    BTH_PATCH_HCI_FUNC_ID(hci_send_pending_cmd_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_impl_send_cmd_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_send_command_with_callback),
    BTH_PATCH_HCI_FUNC_ID(hci_simulate_event),
    BTH_PATCH_HCI_FUNC_ID(hci_send_bt_acl_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_clean_rx_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_clean_tx_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_clean_cmd_packet),
#ifdef ISO_BEARER_SUPPORT
    BTH_PATCH_HCI_FUNC_ID(hci_get_iso_rx_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_send_iso_packet),
    BTH_PATCH_HCI_FUNC_ID(hci_tx_iso_packet_handle),
    BTH_PATCH_HCI_FUNC_ID(hci_tx_iso_ind),
    BTH_PATCH_HCI_FUNC_ID(hci_handle_iso_tx_num_of_complete),
#endif
    BTH_PATCH_HCI_FUNC_ID(hci_fc_each_link_has_tx_chance),
    BTH_PATCH_HCI_FUNC_ID(hci_tx_buff_process),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_delete_free),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_add_new),
    BTH_PATCH_HCI_FUNC_ID(btm_chip_init_continue),
    BTH_PATCH_HCI_FUNC_ID(btm_chip_init_continue_noraml_test_mode_switch),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_acl_req),
    BTH_PATCH_HCI_FUNC_ID(btm_sco_connect_req),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_acl_close),
    BTH_PATCH_HCI_FUNC_ID(btm_set_audio_default_param),
    BTH_PATCH_HCI_FUNC_ID(btm_ibrt_report_snoop_acl_disconnected),
    BTH_PATCH_HCI_FUNC_ID(btm_save_ctx),
    BTH_PATCH_HCI_FUNC_ID(btm_restore_ctx),
    BTH_PATCH_HCI_FUNC_ID(btm_sco_tws_sync_connected),
    BTH_PATCH_HCI_FUNC_ID(btm_security_link_key_notify),
    BTH_PATCH_HCI_FUNC_ID(btm_security_link_key_req),
    BTH_PATCH_HCI_FUNC_ID(btm_security_authen_complete),
    BTH_PATCH_HCI_FUNC_ID(btm_security_encryption_change),
    BTH_PATCH_HCI_FUNC_ID(btm_security_askfor_authority),
    BTH_PATCH_HCI_FUNC_ID(btm_create_acl_connection_fail_process),
    BTH_PATCH_HCI_FUNC_ID(btm_sco_conn_accept),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_accept_extra_acl_req_check),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_handle_extra_acl_conn_req),
    BTH_PATCH_HCI_FUNC_ID(btm_conn_disconnect_process),
    BTH_PATCH_HCI_FUNC_ID(btm_process_init_state_cmd_cmpl),
    BTH_PATCH_HCI_FUNC_ID(btm_process_cmd_complete_evt),
    BTH_PATCH_HCI_FUNC_ID(btm_get_bdaddr_from_pending_hci_cmd),
    BTH_PATCH_HCI_FUNC_ID(btif_me_reconnect_next_device),
    BTH_PATCH_HCI_FUNC_ID(btif_me_wait_acl_complete),
    BTH_PATCH_HCI_FUNC_ID(btif_me_wait_acl_complete_callback),
    BTH_PATCH_HCI_FUNC_ID(btif_me_event_report),
    BTH_PATCH_HCI_FUNC_ID(btif_me_source_event_report),
    BTH_PATCH_HCI_FUNC_ID(btif_me_is_creating_source_link),
    BTH_PATCH_HCI_FUNC_ID(btif_me_is_accepting_source_link),
    BTH_PATCH_HCI_FUNC_ID(me_event_report_handler),
    BTH_PATCH_HCI_FUNC_ID(btif_me_set_accessible_mode),
    BTH_PATCH_HCI_FUNC_ID(btif_me_save_record_ctx),
    BTH_PATCH_HCI_FUNC_ID(btif_me_set_record_ctx),
} bt_patch_hci_func_enum_t;

void btif_me_reconnect_next_device(void);
bool btif_me_wait_acl_complete(void* remote, void (*cb)(uint8_t device_id, void* remote, bool succ, uint8_t errcode));
void btif_me_wait_acl_complete_callback(uint8_t device_id, struct bdaddr_t *remote, uint8 event, uint8 errcode);
void btif_me_event_report(me_event_t *event);
void btif_me_source_event_report(me_event_t *event);
bool btif_me_is_creating_source_link(const void *remote_addr);
bool btif_me_is_accepting_source_link(const bt_bdaddr_t *remote);
void me_event_report_handler(uint16 evt_id, void* pdata);
uint32_t btif_me_save_record_ctx(uint8_t *ctx_buffer, uint8_t *addr);
uint32_t btif_me_set_record_ctx(uint8_t *ctx_buffer,  uint8_t *addr);
void btm_conn_delete_free(struct btm_conn_item_t *conn);
void btm_chip_init_continue(void);
void btm_chip_init_continue_noraml_test_mode_switch(void);
int8 btm_conn_acl_req(const struct bdaddr_t *remote_bdaddr);
int8 btm_sco_connect_req(struct bdaddr_t *remote_bdaddr);
struct btm_conn_item_t *btm_conn_add_new(const struct bdaddr_t *bdaddr);
int8 btm_conn_acl_close(struct bdaddr_t *bdaddr, uint8 reason);
void btm_set_audio_default_param(uint8 param);
gaf_media_data_t *hci_get_iso_rx_packet(uint16_t connhdl);
void btm_ibrt_report_snoop_acl_disconnected(uint8_t device_id, bt_bdaddr_t* remote);
uint32 btm_save_ctx(struct bdaddr_t *remote, uint8_t *buf, uint32_t buf_len);
uint32 btm_restore_ctx(struct btm_ctx_input *input, struct btm_ctx_output *output);
int8 btm_sco_tws_sync_connected(struct bdaddr_t *bdAddr, uint8 codec, uint16 conn_handle);
void btm_security_link_key_notify(struct bdaddr_t *remote, uint8 *linkkey, uint8 key_type);
void btm_security_link_key_req(struct bdaddr_t *remote);
bool btm_security_authen_complete(uint8 status, uint16 conn_handle);
void btm_security_encryption_change(uint8 status, uint16 conn_handle, uint8 encry_en);
int8 btm_security_askfor_authority(uint8_t device_id, uint16 conn_handle, uint16 psm, struct l2cap_channel *channel);
int8 btm_create_acl_connection_fail_process(struct btm_conn_item_t *conn, uint8 status, struct bdaddr_t *bdaddr);
void btm_sco_conn_accept(struct btm_conn_item_t *conn, struct hci_ev_conn_request *param);
bool btm_conn_accept_extra_acl_req_check(struct bdaddr_t *remote, bool connected);
void btm_conn_handle_extra_acl_conn_req(struct hci_ev_conn_request *param);
void btm_conn_disconnect_process(uint16 handle, uint8 status, uint8 reason);
void btm_process_init_state_cmd_cmpl(struct hci_evt_packet_t *pkt);
void btm_process_cmd_complete_evt(struct hci_evt_packet_t *pkt);
void btm_get_bdaddr_from_pending_hci_cmd(uint16 opcode,struct bdaddr_t *bdaddr);
void hci_handle_fc_on_disconnect(hci_conn_type_t conn_type, struct hci_conn_item_t *item, uint16_t connhdl);
void hci_handle_conn_event(struct hci_evt_packet_t *pkt);
void hci_cmd_start_handle(struct pp_buff *ppb);
void hci_rx_handle(struct hci_rx_desc_t *rx_desc);
struct pp_buff *hci_fc_each_link_has_tx_chance(hci_conn_type_t conn_type);
void hci_send_all_pending_host_num_complete(hci_conn_type_t conn_type);
void hci_rx_num_completed_packet_inc(struct hci_rx_desc_t *rx_desc, uint8_t n);
struct pp_buff *hci_create_cmd_packet_with_priv(uint16_t cmd_opcode, uint8_t cmd_data_len, hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont, void *cmpl);
void hci_cmd_wait_complete_timeout(void *arg);
void hci_free_cmd_packet(bool is_outstanding_cmd, struct pp_buff *ppb);
void hci_cmd_end_handle(struct hci_evt_packet_t *pkt);
void hci_send_pending_cmd_packet(void);
bt_status_t hci_impl_send_cmd_packet(struct pp_buff *ppb);
bt_status_t hci_send_command_with_callback(uint16_t cmd_opcode, const uint8_t *cmd_data, uint8_t data_len, hci_cmd_evt_func_t cmpl_status_cb);
bt_status_t hci_simulate_event(const uint8_t *buff, uint16_t buff_len);
bt_status_t hci_send_bt_acl_packet(hci_conn_type_t conn_type, uint16_t connhdl_flags, struct pp_buff *ppb);
void hci_clean_rx_packet(hci_conn_type_t conn_type, uint16_t connhdl);
void hci_clean_tx_packet(hci_conn_type_t conn_type, uint16_t connhdl);
void hci_clean_cmd_packet(uint16_t connhdl);
#ifdef ISO_BEARER_SUPPORT
bt_status_t hci_send_iso_packet(struct hci_tx_iso_desc_t *tx_iso_desc);
bool hci_tx_iso_packet_handle(void);
bool hci_tx_iso_ind(struct hci_tx_iso_desc_t *tx_iso_desc);
bool hci_handle_iso_tx_num_of_complete(struct hci_evt_header_t *header);
#endif
bool hci_tx_buff_process(void);

typedef void (*btif_me_reconnect_next_device_func_t)(void);
typedef bool (*btif_me_wait_acl_complete_func_t)(void* remote, void (*cb)(uint8_t device_id, void* remote, bool succ, uint8_t errcode));
typedef void (*btif_me_wait_acl_complete_callback_func_t)(uint8_t device_id, struct bdaddr_t *remote, uint8 event, uint8 errcode);
typedef void (*btif_me_event_report_func_t)(me_event_t *event);
typedef struct btm_conn_item_t* (*btm_conn_add_new_func_t)(const struct bdaddr_t *bdaddr);
typedef void (*btif_me_source_event_report_func_t)(me_event_t *event);
typedef bool (*btif_me_is_creating_source_link_func_t)(const void *remote_addr);
typedef bool (*btif_me_is_accepting_source_link_func_t)(const bt_bdaddr_t *remote);
typedef void (*me_event_report_handler_func_t)(uint16 evt_id, void* pdata);
typedef uint32_t (*btif_me_save_record_ctx_func_t)(uint8_t *ctx_buffer, uint8_t *addr);
typedef uint32_t (*btif_me_set_record_ctx_func_t)(uint8_t *ctx_buffer,  uint8_t *addr);
typedef void (*btm_conn_delete_free_func_t)(struct btm_conn_item_t *conn);
typedef void (*btm_chip_init_continue_func_t)(void);
typedef void (*btm_chip_init_continue_noraml_test_mode_switch_func_t)(void);
typedef int8 (*btm_conn_acl_req_func_t)(struct bdaddr_t *remote_bdaddr);
typedef int8 (*btm_sco_connect_req_func_t)(struct bdaddr_t *remote_bdaddr);
typedef int8 (*btm_conn_acl_close_func_t)(struct bdaddr_t *bdaddr, uint8 reason);
typedef void (*btm_set_audio_default_param_func_t)(uint8 param);
typedef gaf_media_data_t* (*hci_get_iso_rx_packet_func_t)(uint16_t connhdl);
typedef void (*btm_ibrt_report_snoop_acl_disconnected_func_t)(uint8_t device_id, bt_bdaddr_t* remote);
typedef uint32 (*btm_save_ctx_func_t)(struct bdaddr_t *remote, uint8_t *buf, uint32_t buf_len);
typedef uint32 (*btm_restore_ctx_func_t)(struct btm_ctx_input *input, struct btm_ctx_output *output);
typedef int8 (*btm_sco_tws_sync_connected_func_t)(struct bdaddr_t *bdAddr, uint8 codec, uint16 conn_handle);
typedef int8 (*btm_sync_conn_audio_connected_func_t)(struct bdaddr_t *bdAddr, uint8 codec, uint16 conn_handle);
typedef void (*btm_security_link_key_notify_func_t)(struct bdaddr_t *remote, uint8 *linkkey, uint8 key_type);
typedef void (*btm_security_link_key_req_func_t)(struct bdaddr_t *remote);
typedef struct pp_buff* (*hci_fc_each_link_has_tx_chance_func_t)(hci_conn_type_t conn_type);
typedef bool (*btm_security_authen_complete_func_t)(uint8 status, uint16 conn_handle);
typedef void (*btm_security_encryption_change_func_t)(uint8 status, uint16 conn_handle, uint8 encry_en);
typedef int8 (*btm_security_askfor_authority_func_t)(uint8_t device_id, uint16 conn_handle, uint16 psm, struct l2cap_channel *channel);
typedef int8 (*btm_create_acl_connection_fail_process_func_t)(struct btm_conn_item_t *conn, uint8 status, struct bdaddr_t *bdaddr);
typedef void (*btm_sco_conn_accept_func_t)(struct btm_conn_item_t *conn, struct hci_ev_conn_request *param);
typedef void (*btm_accept_sync_conn_internal_func_t)(struct btm_conn_item_t *conn, struct hci_ev_conn_request *param);
typedef bool (*btm_conn_accept_extra_acl_req_check_func_t)(struct bdaddr_t *remote, bool connected);
typedef void (*btm_conn_handle_extra_acl_conn_req_func_t)(struct hci_ev_conn_request *param);
typedef void (*btm_conn_disconnect_process_func_t)(uint16 handle, uint8 status, uint8 reason);
typedef void (*btm_process_init_state_cmd_cmpl_func_t)(struct hci_evt_packet_t *pkt);
typedef void (*btm_process_cmd_complete_evt_func_t)(struct hci_evt_packet_t *pkt);
typedef void (*btm_get_bdaddr_from_pending_hci_cmd_func_t)(uint16 opcode,struct bdaddr_t *bdaddr);
typedef void (*hci_handle_fc_on_disconnect_func_t)(hci_conn_type_t conn_type, struct hci_conn_item_t *item, uint16_t connhdl);
typedef void (*hci_handle_conn_event_func_t)(struct hci_evt_packet_t *pkt);
typedef void (*hci_cmd_start_handle_func_t)(struct pp_buff *ppb);
typedef void (*hci_rx_handle_func_t)(struct hci_rx_desc_t *rx_desc);
typedef void (*hci_send_all_pending_host_num_complete_func_t)(hci_conn_type_t conn_type);
typedef struct pp_buff* (*hci_create_cmd_packet_with_priv_func_t)(uint16_t cmd_opcode, uint8_t cmd_data_len, hci_cmd_evt_func_t cmpl_status_cb, void *priv, void *cont, void *cmpl);
typedef void (*hci_rx_num_completed_packet_inc_func_t)(struct hci_rx_desc_t *rx_desc, uint8_t n);
typedef void (*hci_cmd_wait_complete_timeout_func_t)(void *arg);
typedef void (*hci_free_cmd_packet_func_t)(bool is_outstanding_cmd, struct pp_buff *ppb);
typedef void (*hci_cmd_end_handle_func_t)(struct hci_evt_packet_t *pkt);
typedef void (*hci_send_pending_cmd_packet_func_t)(void);
typedef bt_status_t (*hci_impl_send_cmd_packet_func_t)(struct pp_buff *ppb);
typedef bt_status_t (*hci_send_command_with_callback_func_t)(uint16_t cmd_opcode, const uint8_t *cmd_data, uint8_t data_len, hci_cmd_evt_func_t cmpl_status_cb);
typedef bt_status_t (*hci_simulate_event_func_t)(const uint8_t *buff, uint16_t buff_len);
typedef bt_status_t (*hci_send_bt_acl_packet_func_t)(hci_conn_type_t conn_type, uint16_t connhdl_flags, struct pp_buff *ppb);
typedef void (*hci_clean_rx_packet_func_t)(hci_conn_type_t conn_type, uint16_t connhdl);
typedef void (*hci_clean_tx_packet_func_t)(hci_conn_type_t conn_type, uint16_t connhdl);
typedef void (*hci_clean_cmd_packet_func_t)(uint16_t connhdl);
#ifdef ISO_BEARER_SUPPORT
typedef bt_status_t (*hci_send_iso_packet_func_t)(struct hci_tx_iso_desc_t *tx_iso_desc);
typedef bool (*hci_tx_iso_packet_handle_func_t)(void);
typedef bool (*hci_tx_iso_ind_func_t)(struct hci_tx_iso_desc_t *tx_iso_desc);
typedef bool (*hci_handle_iso_tx_num_of_complete_func_t)(struct hci_evt_header_t *header);
#endif
typedef bool (*hci_tx_buff_process_func_t)(void);

#ifdef __cplusplus
}
#endif
#endif /* BUILD_BTH_ROM */
#endif /* __BTH_PATCH_HCI_H__ */
