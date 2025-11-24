/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "factory_server_client.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static int on_post_servers_init_route(struct servers_manager_context *ctx)
{
    factory_server_client_init();
    LOGI("factory_server_client_init");
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_servers_manager_hook(servers_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __servers_manager_hook));
    ctx->hook->on_post_init_route = on_post_servers_init_route;
}
