/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
#include "cmsis_os.h"

#include "heap_porting.h"

/// BT APP MEMERY
#define BT_APP_HEAP_BIG_BUFFER_LOW_LIMIT      (128)
#define BT_APP_HEAP_BIG_BUFFER_HIGH_LIMIT     (1024*1)

#if defined(BLE_ONLY_ENABLED) && !(BLE_AUDIO_ENABLED)  //BLE only without LEA
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_SML   (0)
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_BIG   (1024*5)
#elif defined(BLE_TOTA_ENABLED)                        //Support OTA over LE
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_SML   (1024*2)
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_BIG   (1024*10)
#else                                                  //Default app cobuffer size
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_SML   (1024*2)
#define BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_BIG   (1024*3)
#endif

static uint8_t bt_app_heap_b_pool[BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_BIG] __attribute__((aligned(32)));
static uint8_t bt_app_heap_s_pool[BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_SML] __attribute__((aligned(32)));

struct bt_heap_global_t bt_app_heap;

unsigned char *bt_app_buf_malloc(uint16_t size, uint32_t ca, uint32_t line)
{
    return (unsigned char *)bt_heap_bt_malloc_with_ca(&bt_app_heap, size, ca, line, false);
}

void bt_app_buf_free(unsigned char *buf, uint32_t ca, uint32_t line)
{
    bt_heap_free_with_ca(&bt_app_heap, (struct bt_heap_buff_t *)buf, ca, line);
}

bool bt_app_check_buf_available(uint16_t size)
{
    return bt_heap_check_malloc_available(&bt_app_heap, size);
}

void bt_app_buf_init(void)
{
    bt_app_heap.bt_heap_max_sml_size = BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_SML;
    bt_app_heap.bt_heap_max_big_size = BT_APP_HEAP_MAX_BUFFER_SIZE_FOR_BIG;
    bt_app_heap.bt_heap_big_low_limit = BT_APP_HEAP_BIG_BUFFER_LOW_LIMIT;
    bt_app_heap.bt_heap_big_high_limit = BT_APP_HEAP_BIG_BUFFER_HIGH_LIMIT;
    bt_app_heap.bt_heap_b_pool = bt_app_heap_b_pool;
    bt_app_heap.bt_heap_s_pool = bt_app_heap_s_pool;

    bt_heap_init(&bt_app_heap);
}

void bt_app_buf_deinit(void)
{
    bt_heap_deinit(&bt_app_heap);
}
