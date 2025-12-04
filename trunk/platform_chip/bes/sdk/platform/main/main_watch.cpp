/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#include "plat_addr_map.h"
#include "analog.h"
#include "apps.h"
#include "cmsis.h"
#include "hal_bootmode.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_wdt.h"
#include "mpu_cfg.h"
#include "norflash_api.h"
#include "tgt_hardware.h"
#include "app_utils.h"
#include "watchdog/watchdog.h"
#include "hal_evr.h"
#include "factory_section.h"
#include "main_entry.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "random.h"
#include "stdlib.h"
#include "main_api.h"

#ifdef RTOS
#include "cmsis_os.h"
#include "app_factory.h"
#endif

#ifdef RAM_DUMP_TO_FLASH
#include "ramdump_section.h"
#endif

#ifdef CORE_DUMP_TO_FLASH
#include "coredump_section.h"
#endif

extern "C" void log_dump_init(void);
extern "C" void crash_dump_init(void);

static osThreadId_t main_thread_tid = NULL;

int system_shutdown(void)
{
    TR_INFO(TR_MOD(MAIN), "system_shutdown!!");
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osThreadFlagsSet(main_thread_tid, 0x4);
    return 0;
}

int system_reset(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osThreadFlagsSet(main_thread_tid, 0x8);
    return 0;
}

int signal_send_to_main_thread(uint32_t signals)
{
    osThreadFlagsSet(main_thread_tid, signals);
    return 0;
}

static int tgt_hardware_setup(void)
{
#if CFG_HW_PWL_NUM > 0
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl,
                   sizeof(cfg_hw_pinmux_pwl) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
#endif
    return 0;
}

/**
 * @brief  User MAIN function entry
 * @note
 * @retval
 */
int MAIN_ENTRY(void)
{
    uint8_t sys_case = 0;
    int ret = 0;

    hw_revision_checker();

#if !defined(BLE_ONLY_ENABLED)
    app_wdt_open(15);
#else
    app_wdt_open(30);
#endif

    uint32_t bootmode = hal_sw_bootmode_get();

    tgt_hardware_setup();

#if defined(ROM_UTILS_ON)
    rom_utils_init();
#endif

    main_thread_tid = osThreadGetId();

    hwtimer_init();
    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();
    norflash_api_init();
#if defined(DUMP_LOG_ENABLE)
    log_dump_init();
#endif
#if (defined(DUMP_CRASH_LOG) || defined(TOTA_CRASH_DUMP_TOOL_ENABLE))
    crash_dump_init();
#endif

#if defined(RAM_DUMP_TO_FLASH)
    ramdump_to_flash_init();
#endif

#ifdef CORE_DUMP_TO_FLASH
    coredump_to_flash_init();
#endif

    app_reset_gpio_status();
    app_trace_port_config(bootmode);

    hal_iomux_ispi_access_init();

    system_power_on_callback();

#ifndef FPGA
    hal_norflash_show_id_state(HAL_FLASH_ID_0, true);
    // Software will load the factory data and user data from the bottom TWO sectors from the flash,
    // the FLASH_SIZE defined is the common.mk must be equal or greater than the actual chip flash size,
    // otherwise the ota will load the wrong information
    uint32_t actualFlashSize = hal_norflash_get_flash_total_size(HAL_FLASH_ID_0);
    if (FLASH_SIZE > actualFlashSize) {
        TR_INFO(TR_MOD(MAIN), "Wrong FLASH_SIZE defined in target.mk!");
        TR_INFO(TR_MOD(MAIN), "FLASH_SIZE is defined as 0x%x \
                        while the actual chip flash size is 0x%x!",
                FLASH_SIZE, actualFlashSize);
        TR_INFO(TR_MOD(MAIN), "Please change the FLASH_SIZE in common.mk \
                        to 0x%x to enable the OTA feature.",
                actualFlashSize);
        ASSERT(false, "FLASH_SIZE config error!!!");
    }
#endif

    mpu_cfg();

    random_open();

#ifdef EVR_TRACING
    hal_evr_init(0);
#endif

    factory_section_open();

#if defined(__FACTORY_MODE_SUPPORT__) && !defined(SLIM_BTC_ONLY)
    if (bootmode & HAL_SW_BOOTMODE_FACTORY) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_FACTORY);
        ret = app_factorymode_init(bootmode);
    } else if (bootmode & HAL_SW_BOOTMODE_CALIB) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CALIB);
        ret = app_factorymode_calib_only();
    }
    else
#endif
    {
#ifdef SLIM_BTC_ONLY
        ret = app_init_btc();
#else
        ret = app_init();
#endif
    }

    if (!ret) {
        while (1) {
            osThreadFlagsClear(0x0f);

            // wait any signal
            uint32_t flags;

            flags = osThreadFlagsWait(CMSIS_V1_SIGNAL_ANY, osFlagsWaitAny, osWaitForever);

            if ((flags > 0U) && (flags < 0x80000000U)) {
                if (flags & 0x04) {
                    sys_case = 1;
                    break;
                } else if (flags & 0x08) {
                    sys_case = 2;
                    break;
                }
            } else {
                sys_case = 1;
                break;
            }
        }
    }

    system_shutdown_wdt_config(10);
#ifdef SLIM_BTC_ONLY
    app_deinit_btc(ret);
#else
    app_deinit(ret);
#endif
    system_power_off_callback(sys_case);
    TR_INFO(TR_MOD(MAIN), "byebye~~~ %d\n", sys_case);
    if ((sys_case == 1) || (sys_case == 0)) {
        TR_INFO(TR_MOD(MAIN), "shutdown\n");
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pmu_shutdown();
    } else if (sys_case == 2) {
        TR_INFO(TR_MOD(MAIN), "reset\n");
        pmu_reboot();
    }

    return 0;
}

WEAK void hw_revision_checker(void)
{
}

WEAK void app_reset_gpio_status(void)
{
}

WEAK void system_power_on_callback(void)
{
}

WEAK void system_power_off_callback(uint8_t sys_case)
{
}
