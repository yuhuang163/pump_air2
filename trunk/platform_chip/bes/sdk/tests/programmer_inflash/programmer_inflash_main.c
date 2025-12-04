/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_location.h"
#if defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT) || defined(NO_DOWNLOAD_FEATURE)
#include "hal_norflash.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "mpu_cfg.h"
#endif
#include "tool_msg.h"
#include "tgt_hardware.h"
#ifdef USER_SECURE_BOOT
#include "user_secure_boot.h"
#endif

#if defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT)
#define OTA_BOOT_LOC                    BOOT_TEXT_SRAM_LOC NOINLINE
#define OTA_BOOT_STR(s)                 CONST_STR_BOOT_SRAM(s)
#define DISABLE_ALL_IRQS()              NVIC_DisableAllIRQs_Ram()
#else
#define OTA_BOOT_LOC                    BOOT_TEXT_FLASH_LOC
#define OTA_BOOT_STR(s)                 (s)
#define DISABLE_ALL_IRQS()              NVIC_DisableAllIRQs()
#endif

typedef void (*FLASH_ENTRY)(void);

extern void programmer_start(void);

extern uint32_t programmer_get_magic_number(void);
extern bool programmer_need_run_bflash(void);

#ifdef APP_BOOT_GPIO_CONTROL
#include "hal_iomux.h"
void programmer_set_gpio_up(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_up = {
        HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
    };

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&gpio_up, 1);

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)gpio_up.pin, HAL_GPIO_DIR_OUT, 1);
}
#endif

uint32_t get_download_uart_baud_rate(void)
{
    return DOWNLOAD_UART_BAUD_RATE;
}

int OTA_BOOT_LOC ProgrammerInflashEnterApp(void)
{
    struct boot_struct_t * boot_struct;

    boot_struct = (struct boot_struct_t *)(FLASH_NC_BASE + OTA_CODE_OFFSET);

    if (BOOT_MAGIC_NUMBER == boot_struct->hdr.magic)
    {
        FLASH_ENTRY entry;
        // Disable all IRQs
        DISABLE_ALL_IRQS();
        // Ensure in thumb state
        entry = (FLASH_ENTRY)(&boot_struct->hdr + 1);
        entry = (FLASH_ENTRY)((uint32_t)entry | 1);
        entry();
        //can't run to here
        return 0;
    }

    return -1;
}

int OTA_BOOT_LOC WEAK ProgrammerInflashCheckBootToApp(void)
{
    return 0;
}

void OTA_BOOT_LOC programmer_app_boot(void)
{
#if defined(USER_SECURE_BOOT)
    if (user_secure_boot_check() == 0) {
        user_secure_boot_jump_to_next_entry();
    }
#else
    if (ProgrammerInflashCheckBootToApp()) {
        ProgrammerInflashEnterApp();
    }
#endif

    return;
}

void BOOT_TEXT_FLASH_LOC ProgrammerInflashBootInit(void)
{
#ifndef APP_BOOT_GPIO_CONTROL
#if !(defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT))
    programmer_app_boot();
#endif
#endif
    // Enable icache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE);
    // Enable write buffer
    hal_cache_writebuffer_enable(HAL_CACHE_ID_D_CACHE);
    // Enable dcache
    hal_cache_enable(HAL_CACHE_ID_D_CACHE);

    // Init boot sections
    boot_init_boot_sections();
}
void boot_loader_pre_init_hook(void) __attribute__((alias("ProgrammerInflashBootInit")));

#if defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT) || defined(NO_DOWNLOAD_FEATURE)
void OTA_BOOT_LOC programmer_pre_boot_setup(void)
{
    int ret;

    NVIC_InitVectors();
#ifdef UNALIGNED_ACCESS
    SystemInit();
#endif
#ifdef MPU_INIT_TABLE
    // Init memory map
    mpu_boot_cfg();
#endif

    // Ensure flash is idle
    hal_sys_timer_delay(MS_TO_TICKS(2));

    // Init sys/flash freq
    hal_cmu_programmer_setup();

#ifdef DEBUG
    hal_dma_open();
#if (DEBUG_PORT == 3)
    hal_iomux_set_analog_i2c();
    hal_iomux_set_uart2();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART2);
#elif (DEBUG_PORT == 2)
    hal_iomux_set_analog_i2c();
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#elif (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
#elif (DEBUG_PORT == 0)
    hal_trace_open(HAL_TRACE_TRANSPORT_NULL);
#endif
#endif

    // Init flash
#ifdef OTA_BOOT_TO_DUAL_CHIP
    ret = hal_norflash_set_dual_chip_mode(HAL_FLASH_ID_0, true, false);
#else
    ret = hal_norflash_init(HAL_FLASH_ID_0);
#endif
    if (ret != HAL_NORFLASH_OK) {
        SAFE_PROGRAM_STOP();
    }

#ifdef OTA_BOOT_TO_DUAL_CHIP
    // From now on all the codes should be in ram, until dual chip mode is closed
#endif
}
#endif

#ifdef OTA_BOOT_TO_DUAL_CHIP
void OTA_BOOT_LOC programmer_post_boot_setup(void)
{
    hal_norflash_set_dual_chip_mode(HAL_FLASH_ID_0, false, false);
    // From now on flash XIP is allowed
}
#endif

int BOOT_TEXT_FLASH_LOC programmer_inflash_main(int argc, char *argv[])
{
#if defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT) || defined(NO_DOWNLOAD_FEATURE)
    programmer_pre_boot_setup();

#if defined(OTA_BOOT_TO_DUAL_CHIP) || defined(USER_SECURE_BOOT)
    programmer_app_boot();

#ifdef OTA_BOOT_TO_DUAL_CHIP
    programmer_post_boot_setup();
#endif
#endif
#endif

#ifndef NO_DOWNLOAD_FEATURE
    programmer_start();
#endif

#ifdef UNCONDITIONAL_ENTER_SINGLE_WIRE_DOWNLOAD
    if (ProgrammerInflashEnterApp() < 0) {
        //enter app failed,try reboot.
        hal_cmu_sys_reboot();
    }
#endif

    return 0;
}
int _start(int,  char **) __attribute__((weak,alias("programmer_inflash_main")));
