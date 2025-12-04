/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_lcd_internal.h"
#include "bsp_config.h"

/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_LCD_SPI_DRIVER      (0)
#define IS_SUPPORT_LCD_PSRAM_DRIVER    (1)
#define LCD_PIXEL_SIZE                 (LCD_H_RES * LCD_V_RES)
#define LCD_IMG_SIZE                   (LCD_PIXEL_SIZE * sizeof(uint16_t))
#define LCD_PARAM_NO_CACHE_ENABLE      (0)
#define LCD_REFRESH_BUFFER_NUM         (2)  /// 缓冲区数量：1=单缓冲(阻塞), 2=双缓冲(推荐), 3+=多缓冲
#define LCD_REFRESH_DETECT_INTERVAL_MS (10)  /// 刷新检测间隔时间(ms)

#if IS_SUPPORT_LCD_SPI_DRIVER
typedef struct {
    uint16_t refresh_status[LCD_REFRESH_BUFFER_NUM];  /// 刷新状态：0=空闲, 1=已填充, 2=刷新中
    uint16_t *buffer[LCD_REFRESH_BUFFER_NUM];       /// 双缓冲区数组
} lcd_refresh_buffer_mgr_t;
#endif

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static rt_device_node_t *node = NULL;
static bool lcd_device_is_ready = false;
static void (*s_lcd_ready_callback)(int type, int status);
#if IS_SUPPORT_LCD_SPI_DRIVER
/// LCD刷新线程
static thread_context_t s_lcd_thd = {0};
/// LCD刷新互斥锁
static os_mutex_id s_refresh_mutex = NULL;
/// LCD刷新管理结构
static lcd_refresh_buffer_mgr_t s_refresh_mgr = {0};
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void lcd_power_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)LCD_POWER_PIN, 
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_NOPULL,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(LCD_POWER_PIN, HAL_GPIO_DIR_OUT, 0);
}

static void lcd_power_control(bool enable)
{
    GPIO_PIN_CTRL(LCD_POWER_PIN, enable ? 1 : 0);
    LOG_PBUS("lcd_power_control: %d", enable);
}

static void lcd_reset_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)LCD_RST_PIN, 
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_NOPULL,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(LCD_RST_PIN, HAL_GPIO_DIR_OUT, 0);
}

static void lcd_device_reset(void)
{
    GPIO_PIN_CTRL(LCD_RST_PIN, 1);
    msleep(5);
    GPIO_PIN_CTRL(LCD_RST_PIN, 0);
    msleep(20);
    GPIO_PIN_CTRL(LCD_RST_PIN, 1);
    msleep(10);
}

static int bsp_lcd_refresh(uint16_t x1, uint16_t y1, 
        uint16_t x2, uint16_t y2, uint16_t *color)
{
#if IS_SUPPORT_LCD_SPI_DRIVER
    if (color == NULL) {
        LOGE("color is NULL");
        return BSP_ERR_POINTER_NULL;
    }

    if (!lcd_device_is_ready) {
        LOGE("lcd no ready");
        return BSP_NOT_READY;
    } 
    return lcd_draw_bitmap(x1, y1, x2, y2, color);
#else
    return 0;
#endif
}

#if IS_SUPPORT_LCD_SPI_DRIVER
static uint8_t bsp_lcd_refresh_idle_index_get(void)
{
    MUTEX_LOCK(s_refresh_mutex);
    for (int i = 0; i < LCD_REFRESH_BUFFER_NUM; i++) {
        if (s_refresh_mgr.refresh_status[i] == LCD_REFRESH_STATUS_IDLE) {
            s_refresh_mgr.refresh_status[i] = LCD_REFRESH_STATUS_WAITING_FOR_FILL;
            MUTEX_UNLOCK(s_refresh_mutex);
            LOGI("get idle index: %d", i);
            return i;
        }
    }
    MUTEX_UNLOCK(s_refresh_mutex);
    return LCD_REFRESH_BUFFER_NUM;
}

static int bsp_lcd_color_set(uint16_t color)
{
    if (s_lcd_thd.msg_id == NULL || s_refresh_mgr.buffer[0] == NULL) {
        LOGE("msg_id%p, buffer%p", s_lcd_thd.msg_id, s_refresh_mgr.buffer[0]);
        return BSP_ERROR;
    }

    uint8_t idle_index = bsp_lcd_refresh_idle_index_get();
    if (idle_index >= LCD_REFRESH_BUFFER_NUM) {
        LOGE("no idle index");
        return BSP_ERROR;
    }

    for (uint32_t i = 0; i < LCD_PIXEL_SIZE; i++) {
        s_refresh_mgr.buffer[idle_index][i] = color;
    }
    MUTEX_LOCK(s_refresh_mutex);
    s_refresh_mgr.refresh_status[idle_index] = LCD_REFRESH_STATUS_FILLED;
    MUTEX_UNLOCK(s_refresh_mutex);
    lcd_refresh_param_t param = {
        .x1 = 0,
        .y1 = 0,
        .x2 = LCD_H_RES - 1,
        .y2 = LCD_V_RES - 1,
        .img = s_refresh_mgr.buffer[idle_index],
        .refresh_status = &s_refresh_mgr.refresh_status[idle_index],
    };
    MSG_QUEUE_SEND(s_lcd_thd.msg_id, &param, 0);

    return BSP_OK;
}
#endif

static void bsp_add_lcd_ready_callback(void (*callback)(int type, int status))
{
    s_lcd_ready_callback = callback;
}

#if IS_SUPPORT_LCD_SPI_DRIVER
static void lcd_refresh_thread(const void *args)
{
    lcd_refresh_param_t param = {0};
    while (1) {
        MSG_QUEUE_RECV(s_lcd_thd.msg_id, &param, OS_WAIT_FOREVER);
        if (param.img != NULL) {
            MUTEX_LOCK(s_refresh_mutex);
            if (*param.refresh_status != LCD_REFRESH_STATUS_FILLED) {
                LOGE("refresh status is not filled");
                MUTEX_UNLOCK(s_refresh_mutex);
                continue;
            }
            *param.refresh_status = LCD_REFRESH_STATUS_REFRESHING;
            MUTEX_UNLOCK(s_refresh_mutex);
            bsp_lcd_refresh(param.x1, param.y1, param.x2, param.y2, param.img);
            MUTEX_LOCK(s_refresh_mutex);
            *param.refresh_status = LCD_REFRESH_STATUS_IDLE;
            MUTEX_UNLOCK(s_refresh_mutex);
            LOGI("lcd refresh done: x1=%d, y1=%d, x2=%d, y2=%d, img:%p", param.x1, param.y1, param.x2, param.y2, param.img);
        } else {
            LOGE("lcd refresh param is NULL");
        }
    }
}
#endif

#if IS_SUPPORT_LCD_SPI_DRIVER
static int lcd_refresh_img_init(void)
{
    /// 初始化缓冲区管理结构
    memset(&s_refresh_mgr, 0, sizeof(lcd_refresh_buffer_mgr_t));
    for (int i = 0; i < LCD_REFRESH_BUFFER_NUM; i++) {
    #if IS_SUPPORT_LCD_PSRAM_DRIVER
        s_refresh_mgr.buffer[i] = u_malloc_psram(LCD_IMG_SIZE, LCD_PARAM_NO_CACHE_ENABLE ? true : false);
    #else
        s_refresh_mgr.buffer[i] = u_malloc(LCD_IMG_SIZE);
    #endif
    }
    LOGI("lcd refresh buffer init success, buffer_num=%d", LCD_REFRESH_BUFFER_NUM);

    return BSP_OK;
}

static void lcd_refresh_img_free(void)
{
    /// 释放所有缓冲区
    for (int i = 0; i < LCD_REFRESH_BUFFER_NUM; i++) {
        if (s_refresh_mgr.buffer[i] != NULL) {
        #if IS_SUPPORT_LCD_PSRAM_DRIVER
            u_free_psram(s_refresh_mgr.buffer[i], LCD_PARAM_NO_CACHE_ENABLE ? true : false);
        #else
            u_free(s_refresh_mgr.buffer[i]);
        #endif
            s_refresh_mgr.buffer[i] = NULL;
        }
    }
    LOGI("lcd refresh buffer freed");
}
#endif

static int lcd_dev_open(struct rt_device_node *dev)
{
    int ret = 0;
    lcd_power_init();
    lcd_reset_init();
    lcd_power_control(true);
#if IS_SUPPORT_LCD_SPI_DRIVER
    lcd_device_reset();
    ret = LCD_INTERFACE_INIT();
    if (ret != BSP_OK) {
        LOGE("lcd interface init failed");
        return ret;
    }
    msleep(10);
    ret = lcd_chip_init();
    if (ret != BSP_OK) {
        LOGE("lcd chip init failed");
        return ret;
    }
    ret = lcd_refresh_img_init();
    if (ret != BSP_OK) {
        LOGE("lcd refresh init failed");
        return ret;
    }
    CREATE_MSG_TYPE_QUEUE(&s_lcd_thd.msg_id, LCD_REFRESH_BUFFER_NUM, sizeof(lcd_refresh_param_t));
    RUN_NEW_THREAD(&s_lcd_thd, "lcd_drv", lcd_refresh_thread, RT_PRIO_LCD_THREAD, (LCD_THREAD_STACKSIZE));
    lcd_device_is_ready = true;
    bsp_lcd_color_set(0x0000); /// 清屏
#else
    msleep(10);
    ret = bsp_lcd_init();
    if (ret != BSP_OK) {
        LOGE("lcd init failed");
        return ret;
    }
#endif
    if (s_lcd_ready_callback) {
        s_lcd_ready_callback(0, true);
    }
    return BSP_OK;
}

static int lcd_dev_close(struct rt_device_node *dev)
{
    return 0;
}

static int lcd_dev_write(struct rt_device_node *dev, const void *buf, uint32_t n_bytes)
{
#if IS_SUPPORT_LCD_SPI_DRIVER
    if (!buf || n_bytes < (sizeof(uint32_t) * 6)) {
        LOG_PBUS("invalid args: buf:%p, n_bytes:%d", buf, n_bytes);
        return -1;
    }
    uint32_t *tmp = (uint32_t *)buf;
    uint16_t x1 = (uint16_t)tmp[0];
    uint16_t y1 = (uint16_t)tmp[1];
    uint16_t x2 = (uint16_t)tmp[2];
    uint16_t y2 = (uint16_t)tmp[3];
    uint16_t *color = (uint16_t *)tmp[4];
    return bsp_lcd_refresh(x1, y1, x2, y2, color);
#else
    return 0;
#endif
}

static int lcd_dev_ioctl(struct rt_device_node *dev, uint32_t op_code, void *args)
{
    uint32_t *tmp = NULL;
    switch (op_code) {
        case OP_CODE_PANEL_CLEAR:
            tmp = (uint32_t *)args;
        #if IS_SUPPORT_LCD_SPI_DRIVER
            return bsp_lcd_color_set((uint16_t)tmp[0]);
        #else
            return 0;
        #endif
        case OP_CODE_PANEL_SET_BL:
            tmp = (uint32_t *)args;
            lcd_backlight_control(tmp[0]);
            return 0;
        case OP_CODE_PANEL_ADD_READY_CB:
            bsp_add_lcd_ready_callback((void (*)(int, int))args);
            return 0;
        case OP_CODE_PANEL_DATA_SEND:
            tmp = (uint32_t *)args;
            EXCEPTION_CHECK_INT(tmp);
            return bsp_lcd_data_send((uint8_t)tmp[0], (lcd_tlvs_t *)tmp[1], tmp[2]);
        default:
            LOGE("lcd err op_code: %lx", op_code);
            return -1;
    }
}

static int lcd_dev_suspend(struct rt_device_node *dev)
{
    lcd_device_is_ready = false;
    if (s_lcd_ready_callback) {
        s_lcd_ready_callback(0, false);
    }
    return 0;
}

static int lcd_dev_resume(struct rt_device_node *dev)
{
    lcd_device_is_ready = true;
    if (s_lcd_ready_callback) {
        s_lcd_ready_callback(0, true);
    }
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void *register_device_for_lcd(list_head_t *list_head)
{
    node = u_malloz(sizeof(rt_device_node_t));

    node->name = "lcd";
    node->id = ID_DEVICE_DISPLAY_PANEL;
    node->open = lcd_dev_open;
    node->close = lcd_dev_close;
    node->suspend = lcd_dev_suspend;
    node->resume = lcd_dev_resume;
    node->write = lcd_dev_write;
    node->ioctl = lcd_dev_ioctl;
    __list_add_tail(&node->list, list_head);
    LOG_PBUS("dev: %s registered", node->name);

    return node;
}

#if IS_SUPPORT_LCD_SPI_DRIVER
void bsp_lcd_init_test(void)
{
    int ret = 0;
    lcd_power_init();
    lcd_reset_init();
    lcd_power_control(true);
    msleep(10);
    lcd_device_reset();
    ret = LCD_INTERFACE_INIT();
    if (ret != BSP_OK) {
        LOGE("lcd interface init failed");
        return;
    }
    msleep(10);
    // ret = lcd_chip_init();
    // if (ret != BSP_OK) {
    //     LOGE("lcd chip init failed");
    //     return;
    // }
    ret = lcd_refresh_img_init();
    if (ret != BSP_OK) {
        LOGE("lcd refresh init failed");
        return;
    }
    CREATE_MSG_TYPE_QUEUE(&s_lcd_thd.msg_id, LCD_REFRESH_BUFFER_NUM, sizeof(lcd_refresh_param_t));
    RUN_NEW_THREAD(&s_lcd_thd, "lcd_drv", lcd_refresh_thread, RT_PRIO_LCD_THREAD, (LCD_THREAD_STACKSIZE));
    lcd_device_is_ready = true;
    LOGI("lcd init test success");
}

void bsp_lcd_chip_init_test(void)
{
    lcd_chip_init();
    LOGI("lcd chip init test success");
}

void bsp_lcd_draw_bitmap_test(uint16_t color)
{
    bsp_lcd_color_set(color);
}


void bsp_lcd_img_refresh_test(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *color)
{
    if (color == NULL || s_refresh_mgr.buffer[0] == NULL || s_lcd_thd.msg_id == NULL) {
        LOGE("color:%p, buffer:%p, msg_id:%p", color, s_refresh_mgr.buffer[0], s_lcd_thd.msg_id);
        return;
    }
    
    if (x2 >= LCD_H_RES || y2 >= LCD_V_RES || x1 > x2 || y1 > y2) {
        LOGE("invalid parameters: x1=%d, y1=%d, x2=%d, y2=%d", x1, y1, x2, y2);
        return;
    }

    uint8_t idle_index = bsp_lcd_refresh_idle_index_get();
    if (idle_index >= LCD_REFRESH_BUFFER_NUM) {
        LOGE("no idle index");
        return;
    }
    
    uint32_t copy_size = (x2 - x1 + 1) * (y2 - y1 + 1) * sizeof(uint16_t);
    memcpy(s_refresh_mgr.buffer[idle_index], color, copy_size);
    MUTEX_LOCK(s_refresh_mutex);
    s_refresh_mgr.refresh_status[idle_index] = LCD_REFRESH_STATUS_FILLED;
    MUTEX_UNLOCK(s_refresh_mutex);
    lcd_refresh_param_t param = {
        .x1 = x1,
        .y1 = y1,
        .x2 = x2,
        .y2 = y2,
        .img = s_refresh_mgr.buffer[idle_index],
        .refresh_status = &s_refresh_mgr.refresh_status[idle_index],
    };
    MSG_QUEUE_SEND(s_lcd_thd.msg_id, &param, 0);
}
#endif