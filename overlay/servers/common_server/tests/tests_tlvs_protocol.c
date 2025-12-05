/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h"      // IWYU pragma: keep
#include "common_tlvs.h"          // IWYU pragma: keep
#include "storage_manager_api.h"  // IWYU pragma: keep
#include "protocol_manager_api.h" // IWYU pragma: keep
#include "ble_manager_api.h"      // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define TESTS_SN_LENGTH 35          /// SN数据长度
#define TESTS_PRODUCT_ID_LENGTH 6   /// 产品ID长度
#define TESTS_DEVICE_ID_LENGTH 16   /// 设备ID长度
#define TESTS_SECRET_KEY_LENGTH 16  /// 密钥长度
#define TESTS_TRIPLE_DATA_LENGTH 38 /// 三元组数据长度 (6+16+16)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static storage_manager_context_t *get_storage_ctx(void)
{
    return (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
}

static protocol_manager_context_t *get_protocol_ctx(void)
{
    return (protocol_manager_context_t *)find_module(ID_MODULE_PROTOCOL);
}

static ble_manager_context_t *get_ble_ctx(void)
{
    return (ble_manager_context_t *)find_module(ID_MODULE_BLE);
}

static void tests_tlvs_suction_test_mode_handler(uint8_t on_off)
{
    breast_pump_manager_context_t *pump_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    EXCEPTION_CHECK_VOID(pump_ctx);
    motor_manager_context_t *motor_ctx = pump_ctx->priv.motor;
    EXCEPTION_CHECK_VOID(motor_ctx);
    if (on_off) {
        pump_ctx->set_breast_pump_status(BREAST_PUMP_PAUSE);
        pump_ctx->set_breast_pump_mode(BREAST_PUMP_PUMP_MODE);
        pump_ctx->set_breast_pump_status(BREAST_PUMP_START);
        msleep(100);
        uint8_t time_index = motor_ctx->get_runtime_policy_index(MOTOR_TIME_POLICY);
        if (time_index >= ROOT_BREAST_PUMP_STEP_MAX) {
            time_index = 0;
        }
        pump_setting_t setting = {0};
        pump_ctx->get_breast_pump_setting(&setting, time_index);
        setting.level[0] = BREAST_PUMP_LEVEL_15;
        setting.level[1] = BREAST_PUMP_LEVEL_15;
        pump_ctx->set_breast_pump_setting(&setting, time_index);

        LOG_SRV("tests_tlvs_suction_test_mode_handler: level[0] = %d, level[1] = %d", setting.level[0],
                setting.level[1]);
    } else {
        pump_ctx->set_breast_pump_status(BREAST_PUMP_PAUSE);
    }
}
/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/// ======================== 模式控制 ========================

int tests_tlvs_factory_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                 uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    uint8_t mode = (data && data_len >= 1) ? data[0] : 0;

    LOG_SRV("tests_tlvs_factory_test_mode: %s", mode ? "ENTER" : "EXIT");
    tests_manager_context_t *tests_ctx = (tests_manager_context_t *)find_module(ID_MODULE_TESTS);
    EXCEPTION_CHECK_INT(tests_ctx);
    // TODO: 实现产测模式进入/退出逻辑
    if (mode == 1) {
        /* 进入产测模式 */
        tests_ctx->tests_manager_set_current_mode(TST_FACTORY_TYPE);
        /* 打开测试窗口 */
        window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
        EXCEPTION_CHECK_INT(wm);
        if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE) == 0) {
                multi_func_msg_data_t msg = {0};
                msg.union_body.extra.window_id = ID_WINDOW_PRODUCT_TEST_MODE;
                wm->dispatch_new_window_args(wm, &msg);
            }
        } else {
            /* 退出产测模式 */
            tests_ctx->tests_manager_set_current_mode(TST_NONE_TYPE);
            /* 关闭测试窗口 */
            /*ndow_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
            EXCEPTION_CHECK_INT(wm);
            if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE)) {   // 如果当前窗口是测试窗口，则关闭测试窗口
                multi_func_msg_data_t msg = {0};
                msg.union_body.extra.window_id = ID_WINDOW_LOGO;
                wm->dispatch_new_window_args(wm, &msg);
            }*/
        }
    // 应答格式: Length=1, Value=0x00失败/0x01成功
    uint8_t result = 0x01; // 默认成功

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_FACTORY_TEST_MODE,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_aging_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                               uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    uint8_t mode = (data && data_len >= 1) ? data[0] : 0;

    LOG_SRV("tests_tlvs_aging_test_mode: %s", mode ? "ENTER" : "EXIT");
    tests_manager_context_t *tests_ctx = (tests_manager_context_t *)find_module(ID_MODULE_TESTS);
    EXCEPTION_CHECK_INT(tests_ctx);

    // TODO: 实现老化模式进入/退出逻辑
    if (mode == 1) {
        /* 进入老化模式 */
        tests_ctx->tests_manager_set_current_mode(TST_AGING_TEST_TYPE);
        //启动产测模块 老化测试

        /* 打开测试窗口 */
        window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
        EXCEPTION_CHECK_INT(wm);
        if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE) == 0) {
            multi_func_msg_data_t msg = {0};
            msg.union_body.extra.window_id = ID_WINDOW_PRODUCT_TEST_MODE;
            wm->dispatch_new_window_args(wm, &msg);
        }
    } else {
        /* 退出老化模式 */
        tests_ctx->tests_manager_set_current_mode(TST_NONE_TYPE);
        /* 关闭测试窗口 */
        /*ndow_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
        EXCEPTION_CHECK_INT(wm);
        if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE)) {   // 如果当前窗口是测试窗口，则关闭测试窗口
            multi_func_msg_data_t msg = {0};
            msg.union_body.extra.window_id = ID_WINDOW_LOGO;
            wm->dispatch_new_window_args(wm, &msg);
        }*/
    }

    // 应答格式: Length=1, Value=0x00失败/0x01成功
    uint8_t result = 0x01; // 默认成功

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_AGING_TEST_MODE,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_suction_test_mode(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                 uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    uint8_t mode = (data && data_len >= 1) ? data[0] : 0;

    LOG_SRV("tests_tlvs_suction_test_mode: %s", mode ? "ENTER" : "EXIT");

    tests_manager_context_t *tests_ctx = (tests_manager_context_t *)find_module(ID_MODULE_TESTS);
    EXCEPTION_CHECK_INT(tests_ctx);
    // TODO: 实现吸力测试模式进入/退出逻辑
    if (mode == 1) {
        /* 进入吸力测试模式 */
        tests_ctx->tests_manager_set_current_mode(TST_SUCTION_TEST_TYPE);
        //启动产测模块 吸力测试 吸乳模式→L15档→双边工作
        
        /* 打开测试窗口 */
        window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
        EXCEPTION_CHECK_INT(wm);
        if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE) == 0) {
            multi_func_msg_data_t msg = {0};
            msg.union_body.extra.window_id = ID_WINDOW_PRODUCT_TEST_MODE;
            wm->dispatch_new_window_args(wm, &msg);
        }
        tests_tlvs_suction_test_mode_handler(1);
    } else {
        /* 退出吸力测试模式 */
        tests_ctx->tests_manager_set_current_mode(TST_NONE_TYPE);
        /* 关闭测试窗口 */
        /*ndow_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
        EXCEPTION_CHECK_INT(wm);
        if (wm->is_current_window(wm, ID_WINDOW_PRODUCT_TEST_MODE)) {   // 如果当前窗口是测试窗口，则关闭测试窗口
            multi_func_msg_data_t msg = {0};
            msg.union_body.extra.window_id = ID_WINDOW_LOGO;
            wm->dispatch_new_window_args(wm, &msg);
        }*/
        tests_tlvs_suction_test_mode_handler(0);
    }

    // 应答格式: Length=1, Value=0x00失败/0x01成功
    uint8_t result = 0x01; // 默认成功

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_SUCTION_TEST_MODE,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== 老化状态查询 ========================

int tests_tlvs_aging_status_query(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                  uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_aging_status_query");

    //: byte0=老化结果(0x00-没有老化, 0x01-老化成功, 0x02-老化失败), byte1-4=老化时长(小端)
    uint8_t response[5] = {0};

    // TODO: 从存储中读取老化状态
    storage_manager_context_t *st = get_storage_ctx();
    if (st) {
        factory_flag_info_t flag_info = {0};
        st->get_store_val(STORE_ID_FACTORY_FLAG_INFO, 0, 0, &flag_info);
        response[0] = flag_info.age_flag;
        //单位s
        response[1] = (uint8_t)(flag_info.age_time >> 24);//小端序
        response[2] = (uint8_t)(flag_info.age_time >> 16);//小端序
        response[3] = (uint8_t)(flag_info.age_time >> 8);//小端序
        response[4] = (uint8_t)(flag_info.age_time);//小端序
        //打印flag_info.age_flag和flag_info.age_time
        LOG_SRV("flag_info.age_flag: %d, flag_info.age_time: %d", flag_info.age_flag, flag_info.age_time);
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_AGING_STATUS_QUERY,
        .length = sizeof(response),
        .value = response,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== 三元组写入 ========================

int tests_tlvs_write_sn(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                        uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_write_sn, data_len: %d", data_len);
    LOG_HEX("write_sn", data, data_len);

    uint8_t result = 0x00; // 默认失败

    if (data && data_len == TESTS_SN_LENGTH) {
        storage_manager_context_t *st = get_storage_ctx();
        if (st) {
            factory_dev_info_t dev_info = {0};
            st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
            memcpy(dev_info.sn, data, TESTS_SN_LENGTH);
            dev_info.sn_len = TESTS_SN_LENGTH;
            if (st->put_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info) == 0) {
                result = 0x01; // 成功
            }
        }
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_WRITE_SN,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_write_product_id(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_write_product_id, data_len: %d", data_len);
    LOG_HEX("write_product_id", data, data_len);

    uint8_t result = 0x00; // 默认失败

    if (data && data_len == TESTS_PRODUCT_ID_LENGTH) {
        storage_manager_context_t *st = get_storage_ctx();
        if (st) {
            factory_dev_info_t dev_info = {0};
            st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
            memcpy(dev_info.product_id, data, TESTS_PRODUCT_ID_LENGTH);
            dev_info.product_id_len = TESTS_PRODUCT_ID_LENGTH;
            if (st->put_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info) == 0) {
                result = 0x01; // 成功
            }
        }
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_WRITE_PRODUCT_ID,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_write_device_id(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                               uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_write_device_id, data_len: %d", data_len);
    LOG_HEX("write_device_id", data, data_len);

    uint8_t result = 0x00; // 默认失败

    if (data && data_len == TESTS_DEVICE_ID_LENGTH) {
        storage_manager_context_t *st = get_storage_ctx();
        if (st) {
            factory_dev_info_t dev_info = {0};
            st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
            memcpy(dev_info.device_id, data, TESTS_DEVICE_ID_LENGTH);
            dev_info.device_id_len = TESTS_DEVICE_ID_LENGTH;
            if (st->put_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info) == 0) {
                result = 0x01; // 成功
            }
        }
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_WRITE_DEVICE_ID,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_write_secret_key(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_write_secret_key, data_len: %d", data_len);
    LOG_HEX("write_secret_key", data, data_len);

    uint8_t result = 0x00; // 默认失败

    if (data && data_len == TESTS_SECRET_KEY_LENGTH) {
        storage_manager_context_t *st = get_storage_ctx();
        if (st) {
            factory_dev_info_t dev_info = {0};
            st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
            memcpy(dev_info.enctyption_key, data, TESTS_SECRET_KEY_LENGTH);
            dev_info.enctyption_key_len = TESTS_SECRET_KEY_LENGTH;
            if (st->put_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info) == 0) {
                result = 0x01; // 成功
            }
        }
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_WRITE_SECRET_KEY,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== 三元组读取 ========================

int tests_tlvs_read_triple_data(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_read_triple_data");

    // 应答格式: byte0-5=产品ID(6字节), byte6-21=设备ID(16字节), byte22-37=密钥(16字节)
    uint8_t response[TESTS_TRIPLE_DATA_LENGTH] = {0};

    storage_manager_context_t *st = get_storage_ctx();
    if (st) {
        factory_dev_info_t dev_info = {0};
        st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
        memcpy(response, dev_info.product_id, TESTS_PRODUCT_ID_LENGTH);
        memcpy(response + TESTS_PRODUCT_ID_LENGTH, dev_info.device_id, TESTS_DEVICE_ID_LENGTH);
        memcpy(response + TESTS_PRODUCT_ID_LENGTH + TESTS_DEVICE_ID_LENGTH, dev_info.enctyption_key,
               TESTS_SECRET_KEY_LENGTH);
        //打印dev_info.product_id, dev_info.device_id, dev_info.enctyption_key
        LOG_HEX("dev_info.product_id", dev_info.product_id, TESTS_PRODUCT_ID_LENGTH);
        LOG_HEX("dev_info.device_id", dev_info.device_id, TESTS_DEVICE_ID_LENGTH);
        LOG_HEX("dev_info.enctyption_key", dev_info.enctyption_key, TESTS_SECRET_KEY_LENGTH);
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_READ_TRIPLE_DATA,
        .length = TESTS_TRIPLE_DATA_LENGTH,
        .value = response,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_read_sn(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                       uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_read_sn");

    uint8_t response[TESTS_SN_LENGTH] = {0};

    storage_manager_context_t *st = get_storage_ctx();
    if (st) {
        factory_dev_info_t dev_info = {0};
        st->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &dev_info);
        memcpy(response, dev_info.sn, TESTS_SN_LENGTH);
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_READ_SN,
        .length = TESTS_SN_LENGTH,
        .value = response,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== 设备状态 ========================

int tests_tlvs_device_error_status(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                   uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_device_error_status");

    // TODO: 获取设备异常状态
    uint8_t error_status = TESTS_DEVICE_ERROR_NONE;

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_DEVICE_ERROR_STATUS,
        .length = 1,
        .value = &error_status,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

int tests_tlvs_suction_compensation(comm_protocol_builder_t *builder, comm_protocol_service_t *service, uint8_t *data,
                                    uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    uint8_t onoff = (data && data_len >= 1) ? data[0] : 0;

    LOG_SRV("tests_tlvs_suction_compensation: %s", onoff ? "ON" : "OFF");

    // TODO: 实现吸力补偿开关逻辑

    // 应答格式: Length=1, Value=0x00失败/0x01成功
    uint8_t result = 0x01; // 默认成功

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_SUCTION_COMPENSATION,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== 产测完成标识写入 ========================

int tests_tlvs_write_factory_complete_flag(comm_protocol_builder_t *builder, comm_protocol_service_t *service,
                                           uint8_t *data, uint16_t data_len)
{
    EXCEPTION_CHECK_INT(builder && service);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);

    LOG_SRV("tests_tlvs_write_factory_complete_flag, data_len: %d", data_len);

    uint8_t result = 0x00; // 默认失败

    if (data && data_len >= 1) {
        uint8_t complete_flag = data[0]; // 0x00=产测未完成, 0x01=产测完成

        storage_manager_context_t *st = get_storage_ctx();
        if (st) {
            factory_flag_info_t flag_info = {0};
            st->get_store_val(STORE_ID_FACTORY_FLAG_INFO, 0, 0, &flag_info);
            // 使用 deploy_flag 字段存储产测完成标识
            flag_info.deploy_flag = complete_flag;
            if (st->put_store_val(STORE_ID_FACTORY_FLAG_INFO, 0, 0, &flag_info) == 0) {
                result = 0x01; // 成功
                LOG_SRV("tests_tlvs_write_factory_complete_flag: %s", complete_flag ? "COMPLETE" : "NOT_COMPLETE");
            }
        }
    }

    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_WRITE_FACTORY_COMPLETE_FLAG,
        .length = 1,
        .value = &result,
    };
    return protocol_ctx->comm_protocol_builder_add_tlv(builder, service, &tlv_node);
}

/// ======================== NOTIFY上报 ========================

int tests_key_status_publish(servers_publish_data_args_t *args)
{
    int ret = COMM_PROTOCOL_ERROR_NONE;
    EXCEPTION_CHECK_INT(args);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);
    ble_manager_context_t *ble_ctx = get_ble_ctx();
    EXCEPTION_CHECK_INT(ble_ctx);

    uint8_t key_status = (uint8_t)args->arg1; // 0x01=短按, 0x02=长按
    LOG_SRV("tests_key_status_publish: %s", key_status == TESTS_KEY_STATUS_SHORT_PRESS ? "SHORT" : "LONG");

    comm_protocol_builder_t builder = {0};
    /// 初始化构建器
    ret = protocol_ctx->comm_protocol_builder_init(&builder, 256, protocol_ctx->get_packet_seq(),
                                                   COMM_PROTOCOL_FRAME_TYPE_NOTIFY);
    if (ret != 0) {
        LOGE("==> comm_protocol_builder_init failed");
        return COMM_PROTOCOL_ERROR_INVALID_BUILDER;
    }

    /// 初始化服务节点
    comm_protocol_service_t *service =
        protocol_ctx->comm_protocol_builder_init_service(&builder, COMM_PROTOCOL_TESTS_SERVICE);
    if (service == NULL) {
        LOGE("==> tests service begin error");
        goto ERROR_EXIT;
    }

    /// 添加按键状态TLV
    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_KEY_STATUS_REPORT,
        .length = 1,
        .value = &key_status,
    };
    ret = protocol_ctx->comm_protocol_builder_add_tlv(&builder, service, &tlv_node);
    if (ret != 0) {
        LOGE("==> comm_protocol_builder_add_tlv failed");
        goto ERROR_EXIT;
    }

    /// 最终化构建器
    uint16_t out_size = 0;
    uint8_t *data = protocol_ctx->comm_protocol_builder_finalize(&builder, &out_size);
    if (data == NULL) {
        LOGE("==> comm_protocol_builder_finalize failed");
        goto ERROR_EXIT;
    }

    /// 发送数据
    ble_ctx->dispatch_ble_event_wrapper(ble_ctx, BLE_MSG_FAC_SEND_DATA, data, out_size);

ERROR_EXIT:
    protocol_ctx->comm_protocol_builder_free(&builder);
    return ret;
}

int tests_encoder_status_publish(servers_publish_data_args_t *args)
{
    int ret = COMM_PROTOCOL_ERROR_NONE;
    EXCEPTION_CHECK_INT(args);
    protocol_manager_context_t *protocol_ctx = get_protocol_ctx();
    EXCEPTION_CHECK_INT(protocol_ctx);
    ble_manager_context_t *ble_ctx = get_ble_ctx();
    EXCEPTION_CHECK_INT(ble_ctx);

    uint8_t encoder_status = (uint8_t)args->arg1; // 0x01=左旋, 0x02=右旋
    LOG_SRV("tests_encoder_status_publish: %s", encoder_status == TESTS_ENCODER_STATUS_LEFT ? "LEFT" : "RIGHT");

    comm_protocol_builder_t builder = {0};
    /// 初始化构建器
    ret = protocol_ctx->comm_protocol_builder_init(&builder, 256, protocol_ctx->get_packet_seq(),
                                                   COMM_PROTOCOL_FRAME_TYPE_NOTIFY);
    if (ret != 0) {
        LOGE("==> comm_protocol_builder_init failed");
        return COMM_PROTOCOL_ERROR_INVALID_BUILDER;
    }

    /// 初始化服务节点
    comm_protocol_service_t *service =
        protocol_ctx->comm_protocol_builder_init_service(&builder, COMM_PROTOCOL_TESTS_SERVICE);
    if (service == NULL) {
        LOGE("==> tests service begin error");
        goto ERROR_EXIT;
    }

    /// 添加编码器状态TLV
    comm_protocol_tlv_node_t tlv_node = {
        .type = TESTS_TLV_ENCODER_STATUS_REPORT,
        .length = 1,
        .value = &encoder_status,
    };
    ret = protocol_ctx->comm_protocol_builder_add_tlv(&builder, service, &tlv_node);
    if (ret != 0) {
        LOGE("==> comm_protocol_builder_add_tlv failed");
        goto ERROR_EXIT;
    }

    /// 最终化构建器
    uint16_t out_size = 0;
    uint8_t *data = protocol_ctx->comm_protocol_builder_finalize(&builder, &out_size);
    if (data == NULL) {
        LOGE("==> comm_protocol_builder_finalize failed");
        goto ERROR_EXIT;
    }

    /// 发送数据
    ble_ctx->dispatch_ble_event_wrapper(ble_ctx, BLE_MSG_FAC_SEND_DATA, data, out_size);

ERROR_EXIT:
    protocol_ctx->comm_protocol_builder_free(&builder);
    return ret;
}
