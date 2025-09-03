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
#undef MOUDLE
#define MOUDLE COBT
#include "bt_common_define.h"
#include "bthost_logger.h"

#include "heap_api.h"
#include "multi_heap.h"

#include "multi_heap_platform.h"
#include "multi_heap_internal.h"

#include "heap_porting.h"

/// COHEAP API for all module
void bt_heap_init(struct bt_heap_global_t *bt_heap)
{
    if (bt_heap->bt_heap_b_handle != NULL)
    {
        return;
    }

    if (bt_heap->bt_heap_max_sml_size)
    {
        bt_heap->bt_heap_s_handle = heap_register(bt_heap->bt_heap_s_pool, bt_heap->bt_heap_max_sml_size);
    }
    else
    {
        bt_heap->bt_heap_s_pool = NULL;
        bt_heap->bt_heap_s_handle = NULL;
    }

    bt_heap->bt_heap_b_handle = heap_register(bt_heap->bt_heap_b_pool, bt_heap->bt_heap_max_big_size);
}

void bt_heap_deinit(struct bt_heap_global_t *bt_heap)
{
    bt_heap->bt_heap_b_handle = NULL;
    bt_heap->bt_heap_s_handle = NULL;
}

void bt_heap_enable_debug(struct bt_heap_global_t *bt_heap, bool enable)
{
    bt_heap->bt_heap_enable_debug = enable;
}

struct bt_heap_buff_header_t *bt_heap_buff_header(struct bt_heap_block_t *block)
{
    /*
    * Header must calculate align
    * the calculation is according to fuction multi_heap_malloc_align_impl
    *  ptr = (void*)ALIGN(((uintptr_t)p + PTR_OFFSET_SZ), align);
    */
    void *ptr = multi_heap_get_block_address((multi_heap_block_handle_t)block);
    return (void *)ALIGN(((uintptr_t)ptr + sizeof(uint16_t)), sizeof(void *));
}

bool bt_heap_block_is_free(struct bt_heap_block_t *block)
{
    return (block->header & 0x01);
}

struct bt_heap_block_t *bt_heap_next_block(struct bt_heap_block_t *block)
{
    intptr_t next = (block->header & (~3));
    if (next == 0)
    {
        return NULL;
    }
    if (next <= (intptr_t)block)
    {
        DEBUG_ERROR(0, "INVALID_NEXT_BLOCK %p %p", (void *)next, block);
        return NULL;
    }
    return (struct bt_heap_block_t *)next;
}

inline uint32_t bt_heap_lock(struct bt_heap_header_t *heap)
{
    return MULTI_HEAP_LOCK(heap->lock);
}

inline void bt_heap_unlock(struct bt_heap_header_t *heap, uint32_t flags)
{
    MULTI_HEAP_UNLOCK(heap->lock, flags);
}

int hal_trace_output(const unsigned char *buf, unsigned int buf_len);

void bt_heap_trace(const char *buf, int len)
{
    if (buf && len > 0)
    {
        hal_trace_output((unsigned char *)buf, (unsigned int)len);
    }
}

#define COHEAP_TRACE_MAX_LEN 128

void bt_heap_dump_multi_heap(struct bt_heap_global_t *bt_heap, heap_handle_t heap_handle)
{
    struct bt_heap_header_t *heap = NULL;
    uint8_t *pool_start = NULL;
    uint8_t *pool_end = NULL;
    struct bt_heap_block_t *block = NULL;
    struct bt_heap_block_t *next = NULL;
    struct bt_heap_buff_header_t *buff_header = NULL;
    uint32_t bt_heap_block_count = 0;
    uintptr_t block_size = 0;
    int trace_size = 0;
    char trace_buffer[COHEAP_TRACE_MAX_LEN];

    heap = (struct bt_heap_header_t *)heap_handle;
    if (heap_handle == bt_heap->bt_heap_s_handle)
    {
        pool_start =  bt_heap->bt_heap_s_pool;
        pool_end = pool_start + bt_heap->bt_heap_max_sml_size;
    }
    else if (heap_handle == bt_heap->bt_heap_b_handle)
    {
        pool_start = bt_heap->bt_heap_b_pool;
        pool_end = pool_start + bt_heap->bt_heap_max_big_size;
    }
    else
    {
        DEBUG_ERROR(0, "COBUF_INVALID_HANDLE %p", heap_handle);
        return;
    }

    uint32_t used_block_total_size = 0;
    uint32_t free_block_total_size = 0;
    uint32_t flags = bt_heap_lock(heap);

    block = &heap->first_block;
    next = bt_heap_next_block(block);

    trace_size = snprintf(trace_buffer, COHEAP_TRACE_MAX_LEN,
                          "bt_heap_dump: start %p heap %p #0000 %p #0001 %p #000n %p end %p\n",
                          pool_start, heap, block, next, heap->last_block, pool_end);
    bt_heap_trace(trace_buffer, trace_size);

    for (; block != NULL; block = next)
    {
        next = bt_heap_next_block(block);
        if (next == NULL)
        {
            // curr block is the last block
            trace_size = snprintf(trace_buffer, COHEAP_TRACE_MAX_LEN,
                                  "bt_heap_dump: #000n FREE %p %04d next free NULL\n", block, 8);
            bt_heap_trace(trace_buffer, trace_size);
            break;
        }
        block_size = (uintptr_t)next - (uintptr_t)block;
        if (bt_heap_block_is_free(block))
        {
            trace_size = snprintf(trace_buffer, COHEAP_TRACE_MAX_LEN,
                                  "bt_heap_dump: #%04d FREE %p %04d next free %p next %p\n",
                                  bt_heap_block_count, block, block_size, block->next_free, next);
            free_block_total_size += block_size;
        }
        else
        {
            buff_header = bt_heap_buff_header(block);
            trace_size = snprintf(trace_buffer, COHEAP_TRACE_MAX_LEN,
                                  "bt_heap_dump: #%04d USED %p %04d buff %p #%08d ca=%x ln %04d next %p\n",
                                  bt_heap_block_count, block, block_size, buff_header + 1,
                                  buff_header->buf_seqn, buff_header->alloc_ca,
                                  buff_header->alloc_ln, next);
            used_block_total_size += block_size;
        }
        bt_heap_trace(trace_buffer, trace_size);
        bt_heap_block_count += 1;
    }

    DEBUG_INFO(1, "used/free block total size %d/%d", used_block_total_size, free_block_total_size);

    bt_heap_unlock(heap, flags);
}

void bt_heap_dump_if_broken(struct bt_heap_global_t *bt_heap, heap_handle_t heap_handle)
{
    struct bt_heap_header_t *heap = NULL;
    struct bt_heap_block_t *block = NULL;
    struct bt_heap_block_t *next = NULL;
    bool chain_broken = false;

    heap = (struct bt_heap_header_t *)heap_handle;

    uint32_t flags = bt_heap_lock(heap);

    block = &heap->first_block;
    next = bt_heap_next_block(block);

    for (; block != NULL; block = next)
    {
        next = bt_heap_next_block(block);
        if (next == NULL)
        {
            break;
        }
        if (next <= block)
        {
            chain_broken = true;
            break;
        }
    }

    bt_heap_unlock(heap, flags);

    if (chain_broken)
    {
        bt_heap_dump_multi_heap(bt_heap, heap_handle);
    }
}

uint32 bt_heap_get_buff_seqn(void)
{
    static uint32 bt_heap_buff_seed = 0;
    return bt_heap_buff_seed += 1;
}

void bt_heap_dump_statistics(struct bt_heap_global_t *bt_heap)
{
    if (bt_heap->bt_heap_s_handle)
    {
        DEBUG_INFO(0, "\r\n\r\n============BT COHEAP (S) DUMP STATISTICS:==========");
        bt_heap_dump_multi_heap(bt_heap, bt_heap->bt_heap_s_handle);
    }

    DEBUG_INFO(0, "\r\n\r\n============BT COHEAP (B) DUMP STATISTICS:==========");
    bt_heap_dump_multi_heap(bt_heap, bt_heap->bt_heap_b_handle);

    DEBUG_INFO(0, "\r\n\r\n");
}

void *bt_heap_get_heap_from_buffer(struct bt_heap_global_t *bt_heap, struct bt_heap_buff_t *buffer)
{
    heap_handle_t heap = NULL;
    uint8_t *p = (uint8_t *)buffer;

    if (bt_heap->bt_heap_s_pool && p > bt_heap->bt_heap_s_pool && p < bt_heap->bt_heap_s_pool + bt_heap->bt_heap_max_sml_size)
    {
        heap = bt_heap->bt_heap_s_handle;
    }
    else if (p > bt_heap->bt_heap_b_pool && p < bt_heap->bt_heap_b_pool + bt_heap->bt_heap_max_big_size)
    {
        heap = bt_heap->bt_heap_b_handle;
    }

    return heap;
}

static struct bt_heap_buff_header_t *bt_heap_get_header_from_buffer(struct bt_heap_buff_t *buffer)
{
    return (struct bt_heap_buff_header_t *)(((uint8_t *)buffer) - sizeof(struct bt_heap_buff_header_t));
}

static struct bt_heap_buff_t *bt_heap_impl_malloc_with_ca(struct bt_heap_global_t *bt_heap, heap_handle_t heap_handle, uint16 size, uint32 ca,
                                                          uint32 line)
{
    struct bt_heap_header_t *heap = NULL;
    struct bt_heap_buff_header_t *header = NULL;
    struct bt_heap_buff_t *alloced_buffer = NULL;
    POSSIBLY_UNUSED struct bt_heap_block_t *block = NULL;
    uint16_t peak_size = 0;
    bool print_peak = false;

    if (size == 0 || size > bt_heap->bt_heap_big_high_limit)
    {
        DEBUG_ERROR(0, "INVALID_ALLOC_SIZE %d %p %d", size, (void *)(uintptr_t)ca, line);
        return NULL;
    }

    header = heap_malloc_align(heap_handle, sizeof(struct bt_heap_buff_header_t) + size, sizeof(void *));
    if (header == NULL)
    {
        return NULL;
    }

    heap = (struct bt_heap_header_t *)heap_handle;
    if (heap->free_bytes == heap->minimum_free_bytes && heap->free_bytes < heap->total_bytes / 2)
    {
        peak_size = heap->total_bytes - heap->free_bytes;
        if (heap_handle == bt_heap->bt_heap_s_handle)
        {
            if (peak_size >= bt_heap->peak_size_s_pool + 1024)
            {
                bt_heap->peak_size_s_pool = peak_size;
                print_peak = true;
            }
        }
        else
        {
            if (peak_size >= bt_heap->peak_size_b_pool + 1024)
            {
                bt_heap->peak_size_b_pool = peak_size;
                print_peak = true;
            }
        }
        if (print_peak)
        {
            DEBUG_INFO(0, "ALLOC_PEAK_SIZE %x %p %d",
                       ((heap->total_bytes & 0xffff) << 16) | peak_size,
                       (void *)ca, line);
        }
    }

    header->alloc_ca = ca;
    header->alloc_ln = (uint16_t)line;
    header->buf_seqn = bt_heap_get_buff_seqn();

    alloced_buffer = (struct bt_heap_buff_t *)(((uint8_t *)header) + sizeof(struct bt_heap_buff_header_t));

    if (bt_heap->bt_heap_enable_debug)
    {
        block = (struct bt_heap_block_t *)(((uintptr_t)header) - sizeof(intptr_t));
        DEBUG_INFO(0, "bt_heap_malloc : %p size %04d #%08d ca=%x ln %04d %04x %08x", alloced_buffer, size,
                   header->buf_seqn, ca, line, ((uintptr_t)block) & 0xFFFF, (uintptr_t)bt_heap_next_block(block));
        bt_heap_dump_if_broken(bt_heap, heap_handle);
    }

    return alloced_buffer;
}

struct bt_heap_buff_t *bt_heap_bt_malloc_with_ca(struct bt_heap_global_t *bt_heap, uint16 size, uint32 ca, uint32 line, bool allow_fail_alloc)
{
    struct bt_heap_buff_t *buffer = NULL;

    heap_handle_t heap_handle = bt_heap->bt_heap_b_handle;

    if (bt_heap->bt_heap_s_handle && size < bt_heap->bt_heap_big_low_limit)
    {
        heap_handle = bt_heap->bt_heap_s_handle;
    }

    buffer = bt_heap_impl_malloc_with_ca(bt_heap, heap_handle, size, ca, line);
    if (buffer == NULL && !allow_fail_alloc)
    {
        DEBUG_ERROR(0, "COBUF_ALLOC_FAILED %d %d %p %d", size, bt_heap->bt_heap_big_low_limit, (void *)(uintptr_t)ca, line);
        bt_heap_dump_statistics(bt_heap);
        CO_ASSERT_3(0, (size << 16) | bt_heap->bt_heap_big_low_limit, (void *)(uintptr_t)ca, line);
    }

    return buffer;
}

bool bt_heap_check_malloc_available(struct bt_heap_global_t *bt_heap, uint16_t size)
{
    heap_handle_t heap_handle = bt_heap->bt_heap_b_handle;

    if (size == 0 || size > bt_heap->bt_heap_big_high_limit)
    {
        DEBUG_ERROR(0, "INVALID_ALLOC_SIZE %d", size);
        return false;
    }

    if (bt_heap->bt_heap_s_handle && size < bt_heap->bt_heap_big_low_limit)
    {
        heap_handle = bt_heap->bt_heap_s_handle;
    }

    return heap_check_size_malloc_available(heap_handle, sizeof(struct bt_heap_buff_header_t) + size);
}

void bt_heap_free_with_ca(struct bt_heap_global_t *bt_heap, struct bt_heap_buff_t *p, uint32 ca, uint32 line)
{
    struct bt_heap_buff_header_t *header = NULL;
    POSSIBLY_UNUSED struct bt_heap_buff_header_t backup;
    POSSIBLY_UNUSED struct bt_heap_block_t *block = NULL;

    heap_handle_t heap_handle = bt_heap_get_heap_from_buffer(bt_heap, p);

    if (p == NULL)
    {
        DEBUG_ERROR(0, "NULL_BUFF_PTR %p %d", (void *)(uintptr_t)ca, line);
        return;
    }

    if (heap_handle == NULL)
    {
        DEBUG_ERROR(0, "COBUF_INVALID_HANDLE %p %d", (void *)(uintptr_t)ca, line);
        return;
    }

    header = bt_heap_get_header_from_buffer(p);

    backup = *header;

    if (bt_heap->bt_heap_enable_debug)
    {
        block = (struct bt_heap_block_t *)(((uintptr_t)header) - sizeof(intptr_t));
        DEBUG_INFO(0, "bt_heap_free   : %p #%08d called by ca=%x ln %04d %p", p,
                   backup.buf_seqn, ca, line, block);
    }

    heap_free_align(heap_handle, header);

    if (bt_heap->bt_heap_enable_debug)
    {
        DEBUG_INFO(0, "bt_heap_free   : %p #%08d ca=%x ln %04d %04x %08x", p,
                   backup.buf_seqn, backup.alloc_ca, backup.alloc_ln,
                   ((uintptr_t)block) & 0xFFFF, (uintptr_t)bt_heap_next_block(block));
        bt_heap_dump_if_broken(bt_heap, heap_handle);
    }
}

int bt_heap_count_available_buffers(struct bt_heap_global_t *bt_heap, int size)
{
    multi_heap_info_t heap_info = {0};

    heap_handle_t heap_handle = bt_heap->bt_heap_b_handle;

    if (!bt_heap_check_malloc_available(bt_heap, size))
    {
        return 0;
    }

    if (bt_heap->bt_heap_s_handle && size < bt_heap->bt_heap_big_low_limit)
    {
        heap_handle = bt_heap->bt_heap_s_handle;
    }

    multi_heap_get_info(heap_handle, &heap_info);

    return heap_info.largest_free_block / (size + sizeof(struct bt_heap_buff_header_t));
}
