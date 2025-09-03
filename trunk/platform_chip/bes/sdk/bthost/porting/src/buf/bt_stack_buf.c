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
#include "cmsis.h"
#include "cmsis_os.h"

#include "heap_api.h"
#include "multi_heap_platform.h"
#include "multi_heap_internal.h"
#include "heap_porting.h"

/// STACK BUF API for internal use
#define BT_STACK_HEAP_BIG_BUFFER_LOW_LIMIT (128)
#define BT_STACK_HEAP_BIG_BUFFER_HIGH_LIMIT (1024*6)

#if defined(BLE_ONLY_ENABLED) && !(BLE_AUDIO_ENABLED)
#define BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_SML (0)
#define BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_BIG (1024*15)
#else
#define BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_SML (0)
#define BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_BIG (1024*27)
#endif

static uint8_t bt_stack_heap_b_pool[BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_BIG] __attribute__((aligned(32)));
static uint8_t bt_stack_heap_s_pool[BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_SML] __attribute__((aligned(32)));

static struct bt_heap_global_t bt_stack_heap;

int bt_stack_buf_count_available_buffers(int size)
{
    multi_heap_info_t heap_info = {0};
    heap_handle_t heap_handle = bt_stack_heap.bt_heap_b_handle;

    if (!bt_heap_check_malloc_available(&bt_stack_heap, size))
    {
        return 0;
    }

    if (bt_stack_heap.bt_heap_s_handle && size < bt_stack_heap.bt_heap_big_low_limit)
    {
        heap_handle = bt_stack_heap.bt_heap_s_handle;
    }

    multi_heap_get_info(heap_handle, &heap_info);

    return heap_info.largest_free_block / (size + sizeof(struct bt_heap_buff_header_t));
}

bool bt_stack_buf_check_buffer_available(int size)
{
    return bt_heap_check_malloc_available(&bt_stack_heap, size);
}

unsigned char *bt_stack_buf_malloc_with_ca(uint16_t size, uint32_t ca, uint32_t line)
{
    return (unsigned char *)bt_heap_bt_malloc_with_ca(&bt_stack_heap, size, ca, line, false);
}

void bt_stack_buf_free_with_ca(unsigned char *buf, uint32_t ca, uint32_t line)
{
    bt_heap_free_with_ca(&bt_stack_heap, (struct bt_heap_buff_t *)buf, ca, line);
}

void bt_stack_buf_print_statistic(void)
{
    bt_heap_dump_statistics(&bt_stack_heap);
}

void bt_stack_buf_enable_debug(bool enable)
{
    bt_heap_enable_debug(&bt_stack_heap, enable);
}

void bt_stack_buf_init(void)
{
    bt_stack_heap.bt_heap_max_sml_size = BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_SML;
    bt_stack_heap.bt_heap_max_big_size = BT_STACK_HEAP_MAX_BUFFER_SIZE_FOR_BIG;
    bt_stack_heap.bt_heap_big_low_limit = BT_STACK_HEAP_BIG_BUFFER_LOW_LIMIT;
    bt_stack_heap.bt_heap_big_high_limit = BT_STACK_HEAP_BIG_BUFFER_HIGH_LIMIT;
    bt_stack_heap.bt_heap_b_pool = bt_stack_heap_b_pool;
    bt_stack_heap.bt_heap_s_pool = bt_stack_heap_s_pool;

    bt_heap_init(&bt_stack_heap);
}

void bt_stack_buf_deinit(void)
{
    bt_heap_deinit(&bt_stack_heap);
}
