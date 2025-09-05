/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h" // IWYU pragma: keep
#include "root_common_api.h" // IWYU pragma: keep
#include "heap_api.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define U_HEAP_SIZE (32 * 1024)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static multi_heap_handle_t u_heap = NULL;
static uint8_t u_heap_buf[U_HEAP_SIZE];

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

void *u_malloz(uint32_t size)
{
    void *p = u_malloc(size);

    if (p) {
        memset(p, 0, size);
    }
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

int u_free(void *p)
{
    if (p) {
        heap_free(u_heap, p);
        return 0;
    }
    return -1;
}

uint32_t u_heap_free_size_get(void)
{
    return heap_free_size(u_heap);
}

void u_heap_init(void)
{
    u_heap = heap_register(u_heap_buf, U_HEAP_SIZE);
}