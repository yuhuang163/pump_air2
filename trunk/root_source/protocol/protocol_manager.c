/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"
#include "protocol_manager_api.h"

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static protocol_manager_context_t *thiz = NULL;

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int on_post_protocol_manager_init(void *self)
{
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_module_for_protocol_manager_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, protocol_manager_context_t);
    ptr->comm_protocol_builder_init = comm_protocol_builder_init;
    ptr->comm_protocol_builder_init_service = comm_protocol_builder_init_service;
    ptr->comm_protocol_builder_add_tlv = comm_protocol_builder_add_tlv;
    ptr->comm_protocol_builder_finalize = comm_protocol_builder_finalize;
    ptr->comm_protocol_builder_free = comm_protocol_builder_free;
    ptr->comm_protocol_assemble_packet = comm_protocol_assemble_packet;
    ptr->comm_protocol_frame_deserialize = comm_protocol_frame_deserialize;

    thiz = ptr;
    return ADD_MODULE("protocol", ID_MODULE_PROTOCOL, NULL, on_post_protocol_manager_init,
                      ptr, list_head, sizeof(protocol_manager_context_t));
}


