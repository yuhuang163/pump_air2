/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "window_screen_product_test.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
static ui_proc_product_test_t *proc_thiz = NULL;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/


/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void product_tests_data_recv_parse(void *ctx, uint8_t *p_data, uint16_t ulLen)
{
    product_tests_send_data_via_ble_uart(p_data, ulLen);
}

static void product_tests_uart_data_recv_cb(void *ctx, uint8_t *p_data, uint16_t ulLen)
{
    EXCEPTION_CHECK_VOID(ctx);
    EXCEPTION_CHECK_VOID(p_data);
    if (!p_data || ulLen== 0) {
        LOGE("recv data is NULL or size is 0");
        return;
    }
    LOG_WM("uart_factory_recv_data[%d]", ulLen);
    LOG_HEX("recv_data hex", p_data, ulLen);
    LOG_WM("recv_data str %s", p_data);
    // 这里可以添加对接收到的数据的处理逻辑
    product_tests_data_recv_parse(ctx, p_data, ulLen);
}

static uint16_t product_tests_ble_recv_data_cb(void *ctx, uint8_t *p_data, uint16_t ulLen)
{
    EXCEPTION_CHECK_INT(ctx);
    EXCEPTION_CHECK_INT(p_data);
    if (!p_data || ulLen== 0) {
        LOGE("recv data is NULL or size is 0");
        return -1;
    }
    LOG_WM("ble_factory_recv_data[%d]", ulLen);
    LOG_HEX("recv_data hex", p_data, ulLen);
    LOG_WM("recv_data str %s", p_data);
    // 这里可以添加对接收到的数据的处理逻辑
    product_tests_data_recv_parse(ctx, p_data, ulLen);

    return 1;
}

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

void set_product_tests_ble_uart_register(ui_proc_product_test_t *proc)
{

    EXCEPTION_CHECK_VOID(proc);
    if (proc) {
        window_manager_context_t *wm = proc->wm;
        wm->priv.ble_ctx->set_factory_ble_register_cb(product_tests_ble_recv_data_cb); /// 注册工厂模式蓝牙数据接收回调函数.
        LOG_WM("register factory ble cb");
        if (wm->priv.product_tst->priv.comm_ctx.fcuartdev) { /// 注册工厂模式串口数据接收回调函数.
            LOG_WM("register factory uart cb");
            DEVICE_IOCTL_ARGS_NO_RET(wm->priv.product_tst->priv.comm_ctx.fcuartdev, OP_CODE_UART_ADD_EVENT_CB, product_tests_uart_data_recv_cb);
        }
    }
    proc_thiz = proc;
}

void set_product_tests_ble_uart_unregister(ui_proc_product_test_t *proc)
{
    if (proc) {
        window_manager_context_t *wm = proc->wm;
        EXCEPTION_CHECK_VOID(wm->priv.ble_ctx);
        wm->priv.ble_ctx->set_factory_ble_register_cb(NULL); /// 清除工厂模式蓝牙数据接收回调函数.
        LOG_WM("unregister factory ble cb");
        if (wm->priv.product_tst->priv.comm_ctx.fcuartdev) { /// 清除工厂模式串口数据接收回调函数.
            LOG_WM("unregister factory uart cb");
            DEVICE_IOCTL_ARGS_NO_RET(wm->priv.product_tst->priv.comm_ctx.fcuartdev, OP_CODE_UART_ADD_EVENT_CB, NULL);
        }
    }
    proc_thiz = NULL;
}

void product_tests_send_data_via_ble_uart(uint8_t *data, uint16_t data_len)
{
    const uint16_t MAX_SEND_LEN = 256;

    if (!proc_thiz) {
        LOG_WM("no proc_thiz, drop send");
        return;
    }
    if (!data || data_len == 0) {
        LOG_WM("invalid data or length");
        return;
    }

    window_manager_context_t *wm = proc_thiz->wm;
    if (!wm) {
        LOG_WM("no wm, drop send");
        return;
    }

    if (data_len > MAX_SEND_LEN) {
        LOG_WM("data_len(%d) > MAX_SEND_LEN(%d), truncate", data_len, MAX_SEND_LEN);
        data_len = MAX_SEND_LEN;
    }

    /* 通过 BLE 发送（有连接且回调存在时） */
    if (wm->priv.ble_ctx && wm->priv.ble_ctx->ble_is_connected &&
        wm->priv.ble_ctx->ble_is_connected()) {
        if (wm->priv.ble_ctx->dispatch_ble_event_wrapper) {
            wm->priv.ble_ctx->dispatch_ble_event_wrapper(
                wm->priv.ble_ctx, BLE_MSG_FAC_SEND_DATA, data, data_len);
            LOG_WM("send data via BLE[%d]", data_len);
        } else {
            LOG_WM("ble dispatch wrapper missing");
        }
    }

    /* 通过 UART 发送（优先调用 device->write） */
    if (wm->priv.product_tst && wm->priv.product_tst->priv.comm_ctx.fcuartdev) {
        rt_device_node_t *uart = (rt_device_node_t *)wm->priv.product_tst->priv.comm_ctx.fcuartdev;
        if (uart->write) {
            int ret = uart->write(uart, data, data_len);
            LOG_WM("send data via UART[%d], ret=%d", data_len, ret);
        } else {
            LOG_WM("uart write cb missing, cannot send via uart");
        }
    }
}