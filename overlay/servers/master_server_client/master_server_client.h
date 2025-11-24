#ifndef __MASTER_SERVER_CLIENT_H__
#define __MASTER_SERVER_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "stdint.h" // IWYU pragma: keep

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
void master_server_client_init(void);
int on_master_app_old_protocol_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);

#ifdef __cplusplus
}
#endif

#endif // __PROTOCOL_SERVICE_H__

