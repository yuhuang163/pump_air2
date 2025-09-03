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
#include "app_vap.h"
#include "app_vap_vac_msg.h"

/*EXTERNAL FUNCTIONS*/
/*VAC*/
extern int app_vap_vac_init(void);
extern int app_vap_vac_deinit(void);
#ifdef AOB_MOBILE_ENABLED
extern int app_vap_vas_init(void);
extern int app_vap_vas_deinit(void);
#endif /* AOB_MOBILE_ENABLED */

/*FUNCTIONS*/
int app_vap_client_init(void)
{
    return app_vap_vac_init();
}

int app_vap_client_deinit(void)
{
    return app_vap_vac_deinit();
}

#ifdef AOB_MOBILE_ENABLED
int app_vap_server_init(void)
{
    return app_vap_vas_init();
}

int app_vap_server_deinit(void)
{
    return app_vap_vas_deinit();
}
#endif /* AOB_MOBILE_ENABLED */

#endif

/// @} APP
