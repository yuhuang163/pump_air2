/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "heap_api.h" // IWYU pragma: keep
#include "hal_psram.h" // IWYU pragma: keep
#include "hal_location.h" // IWYU pragma: keep
#include "root_common_api.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define U_HEAP_SIZE (80 * 1024)
#if PSRAM_ENABLE
#define U_HEAP_SIZE_PSRAM_NC (120 * 1024)
#define U_HEAP_SIZE_PSRAM    (120 * 1024)
#endif

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static multi_heap_handle_t u_heap = NULL;
static uint8_t u_heap_sram_buff[U_HEAP_SIZE];

#if PSRAM_ENABLE
static multi_heap_handle_t u_heap_psram_nc = NULL;
static multi_heap_handle_t u_heap_psram = NULL;
static uint8_t u_heap_psram_nc_buff[U_HEAP_SIZE_PSRAM_NC] PSRAM_NC_BSS_LOC;
static uint8_t u_heap_psram_buff[U_HEAP_SIZE_PSRAM] PSRAM_BSS_LOC;
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL FUNCTION DEFINITIONSz
*****************************************************************************************
*/
void *u_malloc(uint32_t size)
{
    void *p = NULL;
    p = heap_malloc(u_heap, size);
    if (!p) {
        LOGE("Oops! memory alloc(%ld) fail.\n", size);
        return NULL;
    }
    return p;
}

void *u_malloc_psram(uint32_t size, int is_psram_nc)
{
    void *p = NULL;
#if PSRAM_ENABLE
    if (is_psram_nc) {
        p = heap_malloc(u_heap_psram_nc, size);
    } else {
        p = heap_malloc(u_heap_psram, size);
    }
#endif
    return p;
}

void *u_malloz(uint32_t size)
{
    void *p = u_malloc(size);
    if (p) {
        memset(p, 0, size);
    }
    return p;
}

void *u_malloz_psram(uint32_t size, int is_psram_nc)
{
    void *p = u_malloc_psram(size, is_psram_nc) ;
    if (p) {
        memset(p, 0, size);
    }
    return p;
}

void *u_malloz_align(uint32_t size, uint32_t align)
{
    void *p = heap_malloc_align(u_heap, size, align);
    if (p) {
        memset(p, 0, size);
    } else {
        LOGE("Oops! memory alloc(%ld) align(%ld) fail.\n", size, align);
    }
    return p;
}

void *u_malloz_psram_align(uint32_t size, uint32_t align, int is_psram_nc)
{
    void *p = NULL;
#if PSRAM_ENABLE
    if (is_psram_nc) {
        p = heap_malloc_align(u_heap_psram_nc, size, align);
    } else {
        p = heap_malloc_align(u_heap_psram, size, align);
    }
    if (p) {
        memset(p, 0, size);
    } else {
        LOGE("Oops! memory alloc(%ld) align(%ld) fail.\n", size, align);
    }
#endif
    return p;
}

void *u_realloc(void *p1, uint32_t size)
{
    void *p = NULL;

    p = heap_realloc(u_heap, p1, size);
    if (!p) {
        LOGE("Oops! memory alloc(%ld) fail.\n", size);
        return NULL;
    }
    return p;
}

void *u_realloc_psram(void *p1, uint32_t size, int is_psram_nc)
{
    void *p = NULL;
#if PSRAM_ENABLE
    if (is_psram_nc) {
        p = heap_realloc(u_heap_psram_nc, p1, size);
    } else {
        p = heap_realloc(u_heap_psram, p1, size);
    }
#endif
    return p;
}

int u_free(void *p)
{
    if (p) {
        heap_free(u_heap, p);
        return 0;
    }
    return -1;
}

int u_free_psram(void *p, int is_psram_nc)
{
#if PSRAM_ENABLE
    if (p) {
        if (is_psram_nc) {
            heap_free(u_heap_psram_nc, p);
        } else {
            heap_free(u_heap_psram, p);
        }
    }
#endif
    return -1;
}

int u_free_align(void *p)
{
    if (p) {
        heap_free_align(u_heap, p);
        return 0;
    }
    return -1;
}

int u_free_align_psram(void *p, int is_psram_nc)
{
#if PSRAM_ENABLE
    if (p) {
        if (is_psram_nc) {
            heap_free_align(u_heap_psram_nc, p);
        } else {
            heap_free_align(u_heap_psram, p);
        }
        return 0;
    }
#endif
    return -1;
}

uint32_t u_heap_free_size_get(void)
{
    return heap_free_size(u_heap);
}

uint32_t u_heap_free_size_psram_get(int is_psram_nc)
{
    uint32_t ret = 0;
#if PSRAM_ENABLE
    if (is_psram_nc) {
        ret = heap_free_size(u_heap_psram_nc);
    } else {
        ret = heap_free_size(u_heap_psram);
    }
#endif
    return ret;
}

void u_heap_info_output(void)
{
    multi_heap_info_t info;
    heap_get_info(u_heap, &info);
    PRINT_LOG("total:bytes=%ld, blocks=%ld; alloc:bytes=%ld, blocks=%ld; free:bytes=%ld, blocks=%ld, largest=%ld, mini=%ld\r\n", 
        info.total_allocated_bytes, info.allocated_blocks, info.total_bytes, info.total_blocks,
        info.total_free_bytes, info.free_blocks, info.largest_free_block, info.minimum_free_bytes);
#if PSRAM_ENABLE
    multi_heap_info_t info_psram_nc;
    heap_get_info(u_heap_psram_nc, &info_psram_nc);
    PRINT_LOG("total:bytes=%ld, blocks=%ld; alloc:bytes=%ld, blocks=%ld; free:bytes=%ld, blocks=%ld, largest=%ld, mini=%ld\r\n", 
        info_psram_nc.total_allocated_bytes, info_psram_nc.allocated_blocks, info_psram_nc.total_bytes, info_psram_nc.total_blocks,
        info_psram_nc.total_free_bytes, info_psram_nc.free_blocks, info_psram_nc.largest_free_block, info_psram_nc.minimum_free_bytes);
    multi_heap_info_t info_psram;
    heap_get_info(u_heap_psram, &info_psram);
    PRINT_LOG("total:bytes=%ld, blocks=%ld; alloc:bytes=%ld, blocks=%ld; free:bytes=%ld, blocks=%ld, largest=%ld, mini=%ld\r\n", 
        info_psram.total_allocated_bytes, info_psram.allocated_blocks, info_psram.total_bytes, info_psram.total_blocks,
        info_psram.total_free_bytes, info_psram.free_blocks, info_psram.largest_free_block, info_psram.minimum_free_bytes);
#endif
}

void u_heap_init(void)
{
    u_heap = heap_register(u_heap_sram_buff, U_HEAP_SIZE);
#if PSRAM_ENABLE
    memset(u_heap_psram_nc_buff, 0, U_HEAP_SIZE_PSRAM_NC);
    memset(u_heap_psram_buff, 0, U_HEAP_SIZE_PSRAM);
    u_heap_psram_nc = heap_register(u_heap_psram_nc_buff, U_HEAP_SIZE_PSRAM_NC);
    u_heap_psram = heap_register(u_heap_psram_buff, U_HEAP_SIZE_PSRAM);
    LOGI("u_heap_psram_nc_init: %p, u_heap_psram_init: %p", u_heap_psram_nc, u_heap_psram);
#endif
    LOGI("u_heap_init: %p", u_heap);
}

