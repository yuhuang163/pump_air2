#ifndef __APP_PROTOCOL_PARSE_H__
#define __APP_PROTOCOL_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

void on_perpare_app_commication_protocol_init(protocol_manager_context_t *ctx);
void on_post_app_commication_protocol_init(protocol_manager_context_t *ctx);
uint8_t app_commication_protocol_parse_route(uint8_t *data, uint16_t len);
int on_app_commication_protocol_br_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);
void app_commication_send_data_via_ble(uint8_t *data, uint16_t data_len);


#ifdef __cplusplus
}
#endif

#endif 