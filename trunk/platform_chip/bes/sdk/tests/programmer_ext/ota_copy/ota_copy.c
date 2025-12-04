/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_bootmode.h"
#include "hal_cache.h"
#include "export_fn_rom.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "hal_trace.h"
#include "hal_wdt.h"
#include "pmu.h"
#include "string.h"
#include "sys_api_programmer.h"
#include "crc16_c.h"
#include "crc32_c.h"
#include "tgt_hardware.h"
#include "plat_addr_map.h"
#ifdef OTA_BIN_COMPRESSED
#include "bes_lzma_api.h"
#endif

typedef void (*FLASH_ENTRY)(void);

#ifdef OTA_BOOT_TO_DUAL_CHIP
#define OTA_BOOT_LOC                    BOOT_TEXT_SRAM_LOC NOINLINE
#define OTA_BOOT_STR(s)                 CONST_STR_BOOT_SRAM(s)
#define FLASH_RESET_REMAP(id)           hal_norflash_reset_remap(id)
#define FLASH_CONFIG_REMAP(id, rid, addr, len, offset) \
                                        hal_norflash_config_remap(id, rid, addr, len, offset)
#define FLASH_ENABLE_REMAP(id, rid)     hal_norflash_enable_remap(id, rid)
#define DISABLE_ALL_IRQS()              NVIC_DisableAllIRQs_Ram()
#define SW_BOOTMODE_GET()               hal_sw_bootmode_get_ram();
#else
#define OTA_BOOT_LOC                    BOOT_TEXT_FLASH_LOC
#define OTA_BOOT_STR(s)                 (s)
#define FLASH_RESET_REMAP(id)           hal_norflash_boot_reset_remap(id)
#define FLASH_CONFIG_REMAP(id, rid, addr, len, offset) \
                                        hal_norflash_boot_config_remap(id, rid, addr, len, offset)
#define FLASH_ENABLE_REMAP(id, rid)     hal_norflash_boot_enable_remap(id, rid)
#define DISABLE_ALL_IRQS()              NVIC_DisableAllIRQs()
#define SW_BOOTMODE_GET()               hal_sw_bootmode_get();
#endif

#define OTA_IMAGE_COPY_ENABLED
#define BOOT_MAGIC_NUMBER               0xBE57EC1C
#define COPY_NEW_IMAGE	                0x5a5a5a5a
//#define NEW_IMAGE_FLASH_OFFSET	      0x200000
#define NORMAL_BOOT		                BOOT_MAGIC_NUMBER
#define FLASH_SECTOR_SIZE_IN_BYTES      4096
#define OTA_DATA_BUFFER_SIZE_FOR_BURNING    FLASH_SECTOR_SIZE_IN_BYTES
#define OTA_FLASH_LOGIC_ADDR            FLASH_NC_BASE

#ifdef USE_MULTI_FLASH
#define OTA_FLASH1_LOGIC_ADDR           (FLASH1_NC_BASE)
#define SRC_FLASH_ID                    HAL_FLASH_ID_1
#define DST_FLASH_ID                    HAL_FLASH_ID_0
#define SRC_FLASH_LOGIC_ADDR            OTA_FLASH1_LOGIC_ADDR
#define DST_FLASH_LOGIC_ADDR            OTA_FLASH_LOGIC_ADDR
#else
#define SRC_FLASH_ID                    HAL_FLASH_ID_0
#define DST_FLASH_ID                    HAL_FLASH_ID_0
#define SRC_FLASH_LOGIC_ADDR            OTA_FLASH_LOGIC_ADDR
#define DST_FLASH_LOGIC_ADDR            OTA_FLASH_LOGIC_ADDR
#endif

#define BOOT_WORD_A                     0xAAAAAAAA
#define BOOT_WORD_B                     0xBBBBBBBB

#define NORFLASH_REMAP_ID               HAL_NORFLASH_REMAP_ID_0
#define NORFLASH_REMAP_SWAP_ID          (NORFLASH_REMAP_ID + 1)

uint8_t dataBufferForBurning[OTA_DATA_BUFFER_SIZE_FOR_BURNING];

typedef struct
{
    uint32_t magicNumber; // NORMAL_BOOT or COPY_NEW_IMAGE
    uint32_t imageSize;
    uint32_t imageCrc;
    uint32_t newImageFlashOffset;
    uint32_t boot_word;
} FLASH_OTA_BOOT_INFO_T;

#ifdef OTA_BOOT_DEBUG
static const
#else
__attribute((section(".ota_boot_info")))
#endif
FLASH_OTA_BOOT_INFO_T otaBootInfoInFlash =
#ifdef FORCE_COPY_NEW_IMAGE
    {COPY_NEW_IMAGE, FLASH_SIZE / 2 - OTA_CODE_OFFSET, 0, FLASH_SIZE / 2, BOOT_WORD_A};
#else
    {NORMAL_BOOT, 0, 0, OTA_CODE_OFFSET, BOOT_WORD_A};
#endif

uint32_t OTA_BOOT_LOC programmer_get_magic_number(void)
{
    uint32_t magic;

    magic = (uint32_t)&otaBootInfoInFlash;

    // First read (and also flush the controller prefetch buffer)
    *(volatile uint32_t *)(magic + 0x400);

    return *(volatile uint32_t *)magic;
}

uint32_t OTA_BOOT_LOC programmer_get_new_image_offset(void)
{
    return otaBootInfoInFlash.newImageFlashOffset;
}

bool OTA_BOOT_LOC programmer_need_run_bflash(void)
{
    bool ret = false;

    if(otaBootInfoInFlash.magicNumber == COPY_NEW_IMAGE && otaBootInfoInFlash.boot_word == BOOT_WORD_B)
    {
        ret = true;
    }

    return ret;
}

static int OTA_BOOT_LOC programmer_inflash_is_single_line_download_mode(void)
{
#ifdef SINGLE_WIRE_DOWNLOAD
    uint32_t bootmode = SW_BOOTMODE_GET();

    return (bootmode & HAL_SW_BOOTMODE_SINGLE_LINE_DOWNLOAD);
#else
    return false;
#endif
}

void OTA_BOOT_LOC remap_b_image(void)
{
    uint32_t magicNumber = programmer_get_magic_number();
    uint32_t lock = 0;

    // CAUTION:
    // Trace has NOT been initialized, so no trace can be printed here

    if (COPY_NEW_IMAGE == magicNumber)
    {
        uint32_t ota_remap_offset = programmer_get_new_image_offset();
        uint32_t dstLogicAddr = FLASH_BASE+OTA_CODE_OFFSET;
        uint32_t len = ota_remap_offset;
        int32_t offsetToRmap = ota_remap_offset;
        enum HAL_NORFLASH_RET_T ret;

        // Remap boot
        lock = int_lock_global();
        ret = FLASH_CONFIG_REMAP(HAL_FLASH_ID_0, NORFLASH_REMAP_ID, dstLogicAddr, len, offsetToRmap);
        if (ret != HAL_NORFLASH_OK) {
            goto _err;
        }
        ret = FLASH_ENABLE_REMAP(HAL_FLASH_ID_0, NORFLASH_REMAP_ID);
        if (ret != HAL_NORFLASH_OK) {
            goto _err;
        }
#ifdef OTA_BOOT_REMAP_SWAP
        ret = FLASH_CONFIG_REMAP(HAL_FLASH_ID_0, NORFLASH_REMAP_SWAP_ID, dstLogicAddr + offsetToRmap, len, -offsetToRmap);
        if (ret != HAL_NORFLASH_OK) {
            goto _err;
        }
        ret = FLASH_ENABLE_REMAP(HAL_FLASH_ID_0, NORFLASH_REMAP_SWAP_ID);
        if (ret != HAL_NORFLASH_OK) {
            goto _err;
        }
#endif
        // CAUTION:
        // If cache has been enabled, the remap destination address had better be invalidted here.
        // Since the cache invalidation functions are located in .boot_sram_text,
        // the whole boot sram should be initialized before the invalidation.
_err:
        int_unlock_global(lock);
    }
}

int OTA_BOOT_LOC ProgrammerInflashCheckBootToApp(void)
{
    bool isEnterSecondaryBootloader = false;
    if (is_ota_remap_enabled())
    {
        if (programmer_inflash_is_single_line_download_mode())
        {
            isEnterSecondaryBootloader = true;
        } else {
            if (programmer_need_run_bflash())
            {
                remap_b_image();
            }
        }
    }
    else
    {
        uint32_t magicNumber = programmer_get_magic_number();
        if (programmer_inflash_is_single_line_download_mode() || (0x5a5a5a5a == magicNumber))
        {
            isEnterSecondaryBootloader = true;
        }
    }

    if (isEnterSecondaryBootloader)
    {
        return 0;
    }

    return 1;
}

#ifdef OTA_IMAGE_COPY_ENABLED

static void programmer_update_magic_number(uint32_t newMagicNumber)
{
    uint32_t lock;

    uint32_t addr = (uint32_t)&otaBootInfoInFlash;

    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_erase(HAL_FLASH_ID_0, addr, FLASH_SECTOR_SIZE_IN_BYTES);
    hal_norflash_write(HAL_FLASH_ID_0, addr, (uint8_t*)(&newMagicNumber), 4);
    pmu_flash_read_config();
    int_unlock(lock);
}

void programmer_copy_new_image(uint32_t srcFlashOffset, uint32_t dstFlashOffset)
{
    FLASH_OTA_BOOT_INFO_T otaBootInfo;
    memcpy((uint8_t *)&otaBootInfo, (uint8_t *)&otaBootInfoInFlash, sizeof(FLASH_OTA_BOOT_INFO_T));

    TRACE(0, "ota boot info is:");
    DUMP8("0x%02x ", (uint8_t *)&otaBootInfoInFlash, sizeof(FLASH_OTA_BOOT_INFO_T));

    uint32_t copiedDataSize = 0;
#ifdef __CRC16__
    uint32_t crcValue = 0xFFFF;
#else
    uint32_t crcValue = 0;
#endif
    uint32_t bytesToCopy = 0;
    uint32_t lock;

#ifdef PROGRAMMER_WATCHDOG
    uint32_t copiedDataSizeSinceLastWdPing = 0;
    //uint32_t erasedDataSizeSinceLastWdPing = 0;
#endif

#ifdef OTA_BOOT_TO_DUAL_CHIP
    int ret;

    ret = hal_norflash_set_dual_chip_mode(HAL_FLASH_ID_0, true, false);
    if (ret != HAL_NORFLASH_OK) {
        SAFE_PROGRAM_STOP();
    }
#endif

    while (copiedDataSize < otaBootInfo.imageSize)
    {
        if (otaBootInfo.imageSize - copiedDataSize > OTA_DATA_BUFFER_SIZE_FOR_BURNING)
        {
            bytesToCopy = OTA_DATA_BUFFER_SIZE_FOR_BURNING;
        }
        else
        {
            bytesToCopy = otaBootInfo.imageSize - copiedDataSize;
        }
        lock = int_lock();

        memcpy((uint8_t *)dataBufferForBurning, (uint8_t *)(SRC_FLASH_LOGIC_ADDR + srcFlashOffset + copiedDataSize), bytesToCopy);

        hal_norflash_erase(DST_FLASH_ID, dstFlashOffset + copiedDataSize,
                           OTA_DATA_BUFFER_SIZE_FOR_BURNING);

        hal_norflash_write(DST_FLASH_ID, dstFlashOffset + copiedDataSize,
                           dataBufferForBurning, bytesToCopy);
		
		//dataBufferForBurning's buf is not changed, It's redundant;
        //memcpy((uint8_t *)dataBufferForBurning, (uint8_t *)(OTA_FLASH_LOGIC_ADDR + dstFlashOffset + copiedDataSize), bytesToCopy);

        int_unlock(lock);

        if (0 == copiedDataSize)
        {
            *(uint32_t *)dataBufferForBurning = 0xFFFFFFFF;
        }

        copiedDataSize += bytesToCopy;

#ifdef __CRC16__
        crcValue = crc16ccitt_c(crcValue, (uint8_t *)dataBufferForBurning, 0,
                              bytesToCopy);
#else
        crcValue = crc32_c(crcValue, (uint8_t *)dataBufferForBurning,
                         bytesToCopy);
#endif


#ifdef PROGRAMMER_WATCHDOG
        copiedDataSizeSinceLastWdPing += bytesToCopy;
        if (copiedDataSizeSinceLastWdPing > 20 * OTA_DATA_BUFFER_SIZE_FOR_BURNING)
        {
            programmer_watchdog_ping();
            copiedDataSizeSinceLastWdPing = 0;
        }
#endif
    }

#ifdef OTA_BOOT_TO_DUAL_CHIP
    ret = hal_norflash_set_dual_chip_mode(HAL_FLASH_ID_0, false, false);
    if (ret != HAL_NORFLASH_OK) {
        SAFE_PROGRAM_STOP();
    }
#endif

    TRACE(2, "Original CRC32 is 0x%x Confirmed CRC32 is 0x%x.", otaBootInfo.imageCrc, crcValue);

#ifdef FORCE_COPY_NEW_IMAGE
    crcValue = otaBootInfo.imageCrc;
#endif

    if (crcValue == otaBootInfo.imageCrc)
    {
        programmer_update_magic_number(NORMAL_BOOT);
    }

    /*     uint32_t erasedDataSize = 0;

    while (erasedDataSize < otaBootInfo.imageSize)
    {
        lock = int_lock();
        hal_norflash_erase(HAL_FLASH_ID_0, (OTA_FLASH_LOGIC_ADDR + srcFlashOffset + erasedDataSize),
                OTA_DATA_BUFFER_SIZE_FOR_BURNING);
        int_unlock(lock);
        erasedDataSize += OTA_DATA_BUFFER_SIZE_FOR_BURNING;

#ifdef PROGRAMMER_WATCHDOG
        erasedDataSizeSinceLastWdPing += erasedDataSize;
        if (erasedDataSizeSinceLastWdPing > 20*OTA_DATA_BUFFER_SIZE_FOR_BURNING)
        {
            hal_wdt_ping(HAL_WDT_ID_0);
            erasedDataSizeSinceLastWdPing = 0;
        }
#endif
    } */

    if (crcValue == otaBootInfo.imageCrc)
    {
        TRACE_FLUSH();
        hal_cmu_sys_reboot();
        while (1)
        {
        }
    }
}

void copy_new_image(void)
{
    uint32_t magicNumber = programmer_get_magic_number();
    uint32_t new_image_copy_flash_offset;
    uint32_t ota_flash_offset_of_image;

    TRACE(1, "magic num 0x%x", magicNumber);

    new_image_copy_flash_offset = programmer_get_new_image_offset();

    if (COPY_NEW_IMAGE == magicNumber)
    {
        hal_norflash_disable_protection(HAL_FLASH_ID_0);
        ota_flash_offset_of_image = OTA_CODE_OFFSET;
        TRACE(2, "Start copying new image from 0x%x to 0x%x...", new_image_copy_flash_offset, ota_flash_offset_of_image);
        //we will reboot once copy success
#ifdef OTA_BIN_COMPRESSED
        ota_copy_compressed_image(new_image_copy_flash_offset, ota_flash_offset_of_image);
#else
        programmer_copy_new_image(new_image_copy_flash_offset, ota_flash_offset_of_image);
#endif
        //TRACE(0,"Copying new image failed, enter OTA mode.");
        TRACE(0, "Copying new image failed, enter Single Download mode.");
    }
}
#endif

static void ota_copy_init(void)
{
#if defined(OTA_IMAGE_COPY_ENABLED)
    TRACE(0,"begin copy_new_image");
    // flash has been initialized in init_download_context()
    //init_flash();       //add for hal_norflash_open failed when copy image we need operate falash
    copy_new_image();
    TRACE(0,"end copy_new_image");
#endif
}

static const CUST_CMD_INIT_T CUST_CMD_INIT_TBL_LOC mod_init[] = {
    ota_copy_init,
};
