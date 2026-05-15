#ifndef __BSP_IMU_INTERNAL_H__
#define __BSP_IMU_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h"
#include "hal_imu.h"

/**@brief Firmware information define*/

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_imu_init(void);
int bsp_imu_deinit(void);
int bsp_imu_read_six_axis(void *buf, uint32_t n_bytes);
int bsp_imu_get_chip_id(void *args);
int bsp_imu_enable_irq(void);
int bsp_imu_disable_irq(void);
int bsp_imu_set_irq_callback(void (*cb)(uint8_t *data));
int bsp_imu_get_fifo_data(void *args);

#ifdef __cplusplus
}
#endif

#endif 
