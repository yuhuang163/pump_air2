/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "root_common_api.h"  // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
typedef struct queue_module_t {
    os_message_id id;
    osMessageQueueAttr_t *message_def;
    list_head_t list;
} queue_module_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
osMutexDef(queue_module_mutex);
static os_mutex_id queue_module_mutex_id = NULL;
static list_head_t list_message_module = {0};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void *list_del_condition_callback(void *data, list_head_t *pos)
{
    os_message_id id = (os_message_id)data;
    queue_module_t *node = CONTAINER_OF(pos, queue_module_t, list);
    if (id == node->id) {
        u_free(node->message_def);
        return node;
    }
    return NULL;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
os_status_e os_message_put_wrapper (os_message_id queue_id, void *msg, uint32_t millisec)
{
    return osMessageQueuePut(queue_id, msg, 0, millisec);
}

os_status_e os_message_get_wrapper (os_message_id queue_id, void *msg, uint32_t millisec)
{
    return osMessageQueueGet(queue_id, msg, NULL, millisec);
}
 
os_status_e os_message_delete_wrapper (os_message_id queue_id)
{
    os_message_id id = queue_id;
    os_status_e status = osMessageQueueDelete(queue_id);
    if (status != osOK) {
        LOGE("Oops! osMessageQueueDelete: fail %p", queue_id);
        return status;
    }
    os_mutex_wait_wrapper(queue_module_mutex_id, OS_WAIT_FOREVER);
    __list_del(&list_message_module, id, list_del_condition_callback);
    os_mutex_release_wrapper(queue_module_mutex_id);
    return status;
}
 
os_status_e os_message_reset_wrapper (os_message_id queue_id)
{
    return osMessageQueueReset(queue_id);
}

int create_msg_queue_wrapper(os_message_id *msg_ptr, int queue_num, int item_size)
{
    osMessageQueueAttr_t *attr = (osMessageQueueAttr_t *)u_malloz(sizeof(osMessageQueueAttr_t));
    attr->name = NULL;
    attr->attr_bits = 0;
    attr->cb_mem = NULL;
    attr->cb_size = 0;
    attr->mq_mem = u_malloz(queue_num * item_size);
    attr->mq_size = queue_num * item_size;
    if(attr->mq_mem == NULL) {
        LOGE("u_malloz failed");
        u_free(attr);
        return -1;
    }

    *msg_ptr = osMessageQueueNew(queue_num, item_size, attr);
    if (*msg_ptr == NULL) {
        LOGE("osMessageQueueNew failed");
        u_free(attr->mq_mem);
        u_free(attr);
        return -1;
    } 

    os_mutex_wait_wrapper(queue_module_mutex_id, OS_WAIT_FOREVER);
    queue_module_t *module = (queue_module_t *)u_malloz(sizeof(queue_module_t));
    module->id = *msg_ptr;
    module->message_def = attr;
    __list_add(&(module->list), &list_message_module);
    os_mutex_release_wrapper(queue_module_mutex_id);

    return 0;
}

int queue_module_init(void)
{
    queue_module_mutex_id = osMutexCreate(osMutex(queue_module_mutex));
    if (queue_module_mutex_id == NULL) {
        LOGE("Oops! osMutexCreate: fail %s", "queue_module_mutex");
        return -1;
    }
    return 0;
}
