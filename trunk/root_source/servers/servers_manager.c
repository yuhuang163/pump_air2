/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"
#include "windows_manager.h" // IWYU pragma: keep
#include "servers_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static servers_manager_context_t *thiz = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int on_post_servers_init(void *ptr)
{
    servers_manager_context_t *ctx = ptr;
    if (ctx->hook && ctx->hook->on_post_init_route) {
        ctx->hook->on_post_init_route(ctx);
    }
    LOGI("on_post_servers_init");
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_module_for_servers_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, servers_manager_context_t);
    thiz = ptr;
    return ADD_MODULE("servers", ID_MODULE_SERVERS, NULL, 
        on_post_servers_init, ptr, list_head, sizeof(servers_manager_context_t));
}

