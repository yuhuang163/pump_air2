#ifndef __BLE_FACTORY_MESSAGE_H__
#define __BLE_FACTORY_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep

#define BLE_FACTORY_CONFIG_HEAD          (0xC0)

int ble_factory_srv_recv_data(void *context, uint8_t *p_data, uint16_t size);
int ble_factory_srv_send_data(void *context, uint8_t *p_data, uint16_t size);
#ifdef __cplusplus
}
#endif

#endif 


