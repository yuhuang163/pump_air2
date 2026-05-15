/*
 * InvenSense driver port: delay and time base (hal_sys_timer).
 */
#include "hal_timer.h"
#include "imu/inv_imu_extfunc.h"

void inv_imu_sleep_us(uint32_t us)
{
    hal_sys_timer_delay_us(us);
}

uint64_t inv_imu_get_time_us(void)
{
    return (uint64_t)TICKS_TO_US(hal_sys_timer_get());
}
