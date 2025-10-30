#ifndef __BLE_REMOT_MESSAGE_H__
#define __BLE_REMOT_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep

int ble_remot_srv_recv_data(void *context, uint8_t *p_data, uint16_t size);
int ble_remot_srv_send_data(void *context, uint8_t *p_data, uint16_t size);
#ifdef __cplusplus
}
#endif

#endif 


