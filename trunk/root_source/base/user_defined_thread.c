/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "peripheral_bus_api.h"
#include "root_common_api.h" // IWYU pragma: keep

/*
 * DEFINES
 *****************************************************************************************
 */

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
#if IS_SUPPORT_PRESSURE_TEST  
static int run_led_test_cb(void)
{
    static uint8_t led_status = LED_STATUS_OFF;
    LOG_PBUS("===> run led test!!!!! status: %d", led_status);
    led_manager_context_t *led = (led_manager_context_t *)find_module(ID_MODULE_LED);
    led_status = (led_status == LED_STATUS_OFF) ? LED_STATUS_ON : LED_STATUS_OFF;
    return 1000;
}

static void run_pump_test_cb(void)
{
    LOG_PBUS("===> run pump test!!!!!");
    breast_pump_manager_context_t *breast_ctx = (breast_pump_manager_context_t *)find_module(ID_MODULE_BREAST_PUMP);
    // breast_ctx->set_breast_pump_mode(BREAST_PUMP_PP_MILK_TRACK_MODE, 0);
    breast_ctx->set_breast_pump_status(BREAST_PUMP_CONTINUE);
}

static void reboot_system_test_cb(void)
{
    static int reboot_count = 0;
    reboot_count++;
    if (reboot_count < 10) {
        return;
    } 
    reboot_count = 0;
    LOG_PBUS("===> reboot system after 100ms");
    msleep(100);
    platform_system_reboot();
}

static int lcd_data_send_test_cb(void)
{
#if (V3_HARDWARE_VERSION == V3_2710IMP_HARDWARE_V1)
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    rt_device_node_t *lcd_dev = pbus->find_device(ID_DEVICE_DISPLAY_PANEL);
    EXCEPTION_CHECK_INT(lcd_dev);
    uint8_t img_id = LCD_DISPLAY_PUMPING_COMPLETED;
    lcd_tlvs_msg_t msg = {
        .srv_id = LCD_DISPLAY_EVENT_SRV,
        .tlvs = u_malloz(sizeof(lcd_tlvs_t)),
        .tlvs_num = 1,
    };
    EXCEPTION_CHECK_INT(msg.tlvs);
    msg.tlvs[0].type = LCD_IMG_ID;
    msg.tlvs[0].data_size = 1;
    msg.tlvs[0].data[0] = img_id;
    lcd_dev->ioctl(lcd_dev, OP_CODE_PANEL_DATA_SEND, (void *)&msg);
    u_free(msg.tlvs);
#endif
    return 1000;
}

static void motor_debug_policy_test_cb(void)
{
    motor_manager_context_t *motor_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    EXCEPTION_CHECK_VOID(motor_ctx);
    EXCEPTION_CHECK_VOID(motor_ctx->hook);
    EXCEPTION_CHECK_VOID(motor_ctx->hook->motor_debug_with_user_para);
    motor_policy_original_para_t para = {
        .pump_level = 75,
        .pumping_time = 1000,
        .valve_on_time = 500,
        .sleep_time = 200,
        .hold_time = 100,
    };
    LOG_PBUS("===> motor debug test!!!!! pump_level: %d, pumping_time: %d, valve_on_time: %d, sleep_time: %d, hold_time: %d",
        para.pump_level, para.pumping_time, para.valve_on_time, para.sleep_time, para.hold_time);
    motor_ctx->hook->motor_debug_with_user_para(motor_ctx, MOTOR_USER_DEBUG_POLICY, &para);
}

static int motor_debug_test_cb(void)
{
    static uint8_t state = MOTOR_USER_DEBUG_PUMP_ON;
    motor_manager_context_t *motor_ctx = (motor_manager_context_t *)find_module(ID_MODULE_MOTOR);
    motor_debug_para_t para = {
        .duty = 75,
        .hold_time = 300,
    };
    if(state > MOTOR_USER_DEBUG_VALVE_OFF) {
        state = MOTOR_USER_DEBUG_PUMP_ON;
    }
    LOG_PBUS("===> motor debug test!!!!! state: %d, duty: %d, hold_time: %d", state, para.duty, para.hold_time);
    motor_ctx->hook->motor_debug_with_user_para(motor_ctx, state, &para);
    state++;
    return 1000;
}

static void sys_dump_wdg_test_cb(void)
{
    window_manager_context_t *wm = NULL;
    LOG_PBUS("===> sys dump wdg test!!!!! wm: %p", wm);
    msleep(100);
    wm->window_scheduling_start();
    while(1) {
    }
}

static uint8_t buf[256];
static uint8_t buf1[1024 * 50];
static int external_flash_test_cb(void)
{
    static uint8_t mode = 5;
    int ret = 0;
    uint32_t addr = 0;
    uint32_t size = 0;
    uint32_t start_time = 0;
    switch (mode) {
        case 0:
            ret = platform_flash_erase(PARTITION_IMG_EXT_DATA_NAME, addr, 4096);
            if (ret != 0) {
                LOGE("===> step 0 erase failed, ret: %d", ret);
                break;
            }
            for (uint8_t i = 0; i < 2; i++) {
                ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
                if (ret != 0) {
                    LOGE("===> step 0 read failed, ret: %d", ret);
                    break;
                }
                addr += sizeof(buf);
                LOG_HEX("data1:", buf, sizeof(buf));
            }
            LOGI("===> step 0 erase size: %d, addr: %d", 4096, addr);
            mode = 1;
            break;

        case 1:
            addr = 0;
            size = 4096;
            for (int j = 0; j < sizeof(buf); j++) {
                buf[j] = j;
            }
            for (int i = 0; i < size;) {
                uint16_t write_size = ((size - i) > sizeof(buf)) ? sizeof(buf) : (size - i);
                ret = platform_flash_write(PARTITION_IMG_EXT_DATA_NAME, addr, buf + i, write_size);
                if (ret != 0) {
                    LOGE("===> step 1 write failed, ret: %d", ret);
                    break;
                }
                LOGE("===> external flash test!!!!! write size: %d, addr: %d, i: %d", write_size, addr, i);
                i += write_size;
                addr += write_size;
            }
            LOGI("===> step 1 write size: %d, addr: %d", size, addr);
            mode = 2;
            break;
        case 2:
            addr = 0;
            ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 2 read failed, ret: %d", ret);
                break;
            }
            LOG_HEX("data2:", buf, sizeof(buf));
            LOGI("===> step 2 read size: %d, addr: %d", sizeof(buf), addr);
            mode = 3;
            break;
        case 3:
            addr = 0x80000;
            ret = platform_flash_erase(PARTITION_IMG_EXT_DATA_NAME, addr, 0x10000);
            if (ret != 0) {
                LOGE("===> step 3 erase failed, ret: %d", ret);
                break;
            }
            ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
            if (ret != 0) {
                LOG_PBUS("===> step 3 read failed, ret: %d", ret);
                break;
            }
            LOG_HEX("data3:", buf, sizeof(buf));
            LOGI("===> step 3 read size: %d, addr: %d", sizeof(buf), addr);
            mode = 4;
            break;
        case 4:
            addr = 0x80000;
            memset(buf, 0x39, sizeof(buf));
            ret = platform_flash_write(PARTITION_IMG_EXT_DATA_NAME, addr + 0x1000, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 4 write failed, ret: %d", ret);
                break;
            }
            LOGI("===> step 4 write size: %d, addr: %d", sizeof(buf), addr + 0x1000);
            mode = 5;
            break;
        case 5:
            addr = 0x80000;
            ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr + 0x1000, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 5 read failed, ret: %d", ret);
                break;
            }
            LOG_HEX("data4:", buf, sizeof(buf));
            LOGI("===> step 5 read size: %d, addr: %d", sizeof(buf), addr + 0x1000);
            mode = 6;
            break;
        case 6:
            addr = 0x90000;
            ret = platform_flash_erase(PARTITION_IMG_EXT_DATA_NAME, addr, 4096 * 2);
            if (ret != 0) {
                LOGE("===> step 0 erase failed, ret: %d", ret);
                break;
            }
            LOGI("===> step 6 erase size: %d, addr: %d", 4096 * 2, addr);
            mode = 7;
            break;
        case 7:
            addr = 0x90000;
            ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 7 read failed, ret: %d", ret);
                break;
            }
            LOG_HEX("data5:", buf, sizeof(buf));
            LOGI("===> step 7 read size: %d, addr: %d", sizeof(buf), addr);
            mode = 8;
            break;
        case 8:
            addr = 0x90000;
            memset(buf, 0x55, sizeof(buf));
            ret = platform_flash_write(PARTITION_IMG_EXT_DATA_NAME, addr + 4096 + 15, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 8 write failed, ret: %d", ret);
                break;
            }
            LOGI("===> step 8 write size: %d, addr: %d", sizeof(buf), addr + 4096 + 15);
            mode = 9;
            break;
        case 9:
            addr = 0x90000;
            memset(buf, 0, sizeof(buf));
            ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr + 4096 + 15, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 9 read failed, ret: %d", ret);
                break;
            }
            LOG_HEX("data6:", buf, sizeof(buf));
            LOGI("===> step 9 read size: %d, addr: %d", sizeof(buf), addr + 4096 + 15);
            mode = 10;
            break;
        case 10:
            addr = 0x80000;
            size = 0x10000;
            start_time = get_timestamp();
            ret = platform_flash_erase(PARTITION_IMG_EXT_DATA_NAME, addr, size);
            if (ret != 0) {
                LOGE("===> step 10 erase failed, ret: %d", ret);
                break;
            }
            LOGI("===> step 10 erase size: %d, addr: %d, time: %dms", size, addr, get_timestamp_ms(start_time));
            mode = 11;
            break;
        case 11:
            addr = 0x80000;
            memset(buf, 0xAA, sizeof(buf));
            ret = platform_flash_write(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 11 write failed, ret: %d", ret);
                break;
            }
            addr = 0x80000 + sizeof(buf1) - sizeof(buf);
            memset(buf, 0xBB, sizeof(buf));
            ret = platform_flash_write(PARTITION_IMG_EXT_DATA_NAME, addr, buf, sizeof(buf));
            if (ret != 0) {
                LOGE("===> step 11 write failed, ret: %d", ret);
                break;
            }
            LOGI("===> step 11 write size: %d, addr: %d", sizeof(buf), addr);
            mode = 12;
            break;
        case 12:
            addr = 0x80000;
            start_time = get_timestamp();
            for (int i = 0; i < 1; i++) {
                ret = platform_flash_read(PARTITION_IMG_EXT_DATA_NAME, addr, buf1, sizeof(buf1));
                if (ret != 0) {
                    LOGE("===> step 13 read failed, ret: %d", ret);
                    break;
                }
            }
            LOGI("===> step 13 read size: %d, addr: %d, time: %dms, data: %02X/%02x", 
                sizeof(buf1), addr, get_timestamp_ms(start_time), (uint8_t)buf1[0], (uint8_t)buf1[sizeof(buf1) - 1]);
            mode = 0;
            break;
        default:
            break;
    }
    return 1000;
}

static void hdr_run_pressure_test_cb(hdr_post_args_t *args)
{
    // sys_dump_wdg_test_cb();
    // reboot_system_test_cb();
    // run_pump_test_cb();
    // POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, lcd_data_send_test_cb(), "pressure_test:continue");
    // POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, run_led_test_cb(), "pressure_test:continue"); 
    POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, external_flash_test_cb(), "pressure_test:continue");
    // motor_debug_policy_test_cb();
    // POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, motor_debug_test_cb(), "pressure_test:continue");
    // POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, 1000, "pressure_test:continue");
}
#endif

void get_night_light_info_test(void)
{

}


/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
int run_user_thread_dispatch(void)
{
#if IS_SUPPORT_PRESSURE_TEST  
    // POST_TASK_HANDLER_DELAYED_ARG0(hdr_run_pressure_test_cb, 2000, "pressure_test");
    get_night_light_info_test(); //开机默认关闭adaptive dimming状态
#endif
    window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
    if (wm) {
        wm->window_scheduling_start();
    }
    return 0;
}
