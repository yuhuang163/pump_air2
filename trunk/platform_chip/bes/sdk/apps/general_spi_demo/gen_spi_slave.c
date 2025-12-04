/***************************************************************************
 *
 * Copyright 2015-2021 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/*
*****************************************************************************
WARNING: This example is tested that the general SPI can be both SPI master and slave.
         Not a special SPI slave in the spi_debug_slave_test.c example.
*****************************************************************************
*/

#ifdef GENERAL_SPI_SLAVE
#include "cmsis.h"
#include "string.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_sysfreq.h"
#include "cmsis_os.h"
#include <stdlib.h>
#include "crc32_c.h"

#define     SPI_TEST_MODE       1/*0: master read,slave write
                                   1: master write,slave read,2021-07-16
                                 */
#define     SPI_DMA_ENABLE
#define     SPI_ID_SELECT       0   //0:spi0,1:spilcd,2021-06-28

#define SPI_DMA_LINK_ENABLE
#ifdef SPI_DMA_LINK_ENABLE
#define HEAD_SIZE   16
#define BES_SPI_DMA_MAX_LEN     (64 * 1024)
#define SPI_DMA_DESC_CNT ((BES_SPI_DMA_MAX_LEN + HAL_DMA_MAX_DESC_XFER_SIZE - 1) / HAL_DMA_MAX_DESC_XFER_SIZE)
struct HAL_DMA_DESC_T spi_master_dma_desc[SPI_DMA_DESC_CNT * 2];
#endif

#define CRC_SIZE    4
#define HEAD_SIZE   6
#define DATA_SIZE   (128 - (CRC_SIZE + HEAD_SIZE))
#define BUF_SIZE    BES_SPI_DMA_MAX_LEN
static uint8_t tx_buf[BUF_SIZE];
static uint8_t rx_buf[BUF_SIZE];

//master tx  slave rx
#define MASTER_TX_REQ_GPIO_PIN              (HAL_GPIO_PIN_P3_2)
#define SLAVE_RX_READY_GPIO_PIN             (HAL_GPIO_PIN_P3_0)

//slave tx master rx
#define SLAVE_TX_READY_GPIO_PIN             (HAL_GPIO_PIN_P3_3)

//master tx/rx done
#define MASTER_TXRX_DONE_GPIO_PIN           (HAL_GPIO_PIN_P3_1)
// #define MASTER_TXRX_DONE_GPIO_PIN           (HAL_GPIO_PIN_P7_1)

#define SPI_MASTER_ACTION_RX_INDICATE       0
#define SPI_MASTER_ACTION_TX_OK_CONFIRMED   1

extern uint8_t hal_gpio_pin_get_out_val(enum HAL_GPIO_PIN_T pin);

typedef enum {
    CMD_SPI_RCV_DONE = 0x01,
    CMD_SPI_SEND_REQ,
    CMD_SPI_RCV_REQ,
} SLV_CMD_PRO_T; 

typedef struct
{
    uint32_t crc32_c;
    uint16_t len;
} __attribute__((packed))buf_head_t;

const osThreadAttr_t slv_thread_attr = {
    .name = "slv_task",
    .stack_size = 5 * 1024,
    .priority = osPriorityNormal,
};
osThreadId_t slv_thread_id = NULL;

osSemaphoreDef(spi_io_sem);
osSemaphoreId spi_io_sem_id = NULL;

volatile static uint8_t spi_slv = 0;
volatile static uint8_t spi_master_rx_ok = 0;

static struct HAL_SPI_CFG_T spi_cfg;
static struct HAL_GPIO_IRQ_CFG_T gpiocfg;

static void bth_slv_thread_init(void);

static int spi_io_sem_init(void)
{
    if (spi_io_sem_id == NULL) {
        spi_io_sem_id = osSemaphoreCreate(osSemaphore(spi_io_sem), 0);
    }
    ASSERT(spi_io_sem_id, "create tdk42607_sem_id fail!");
    return 0;
}

int signal_send_to_slv_thread(uint32_t signals)
{
    TRACE(0,"%s : 0x%02x", __func__, signals);
    osSignalSet(slv_thread_id, signals);
    return 0;
}

static void _spi_master_gpio_irq(enum HAL_GPIO_PIN_T pin)
{
    if (pin == MASTER_TX_REQ_GPIO_PIN) {
        TRACE(0, "%s enter line %d", __func__, __LINE__);
        signal_send_to_slv_thread(CMD_SPI_RCV_REQ);
    } else if (pin == MASTER_TXRX_DONE_GPIO_PIN) {
        if (hal_gpio_pin_get_val(MASTER_TX_REQ_GPIO_PIN) == 0) {
            TRACE(0, "%s enter line %d", __func__, __LINE__);
            //master tx/slave rx
            osSemaphoreRelease(spi_io_sem_id);
        } else {
            //master rx/slave tx
            osSemaphoreRelease(spi_io_sem_id);
        }
    }
}

static void set_slave_iomux(void)
{
    // gpio
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_trigger_master_pinmux[] = {
        {MASTER_TX_REQ_GPIO_PIN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},//HAL_IOMUX_PIN_PULLUP_ENABLE
        {SLAVE_RX_READY_GPIO_PIN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {SLAVE_TX_READY_GPIO_PIN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {MASTER_TXRX_DONE_GPIO_PIN, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLDOWN_ENABLE},
    };
    hal_iomux_init(gpio_trigger_master_pinmux, ARRAY_SIZE(gpio_trigger_master_pinmux));

    // output pin
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)SLAVE_RX_READY_GPIO_PIN, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)SLAVE_TX_READY_GPIO_PIN, HAL_GPIO_DIR_OUT, 1);

    // input pin
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)MASTER_TXRX_DONE_GPIO_PIN, HAL_GPIO_DIR_IN, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)MASTER_TX_REQ_GPIO_PIN, HAL_GPIO_DIR_IN, 1);

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = false;
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    gpiocfg.irq_handler = _spi_master_gpio_irq;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    hal_gpio_setup_irq(MASTER_TX_REQ_GPIO_PIN, &gpiocfg);
    hal_gpio_setup_irq(MASTER_TXRX_DONE_GPIO_PIN, &gpiocfg);

    // gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    // hal_gpio_setup_irq(MASTER_TXRX_DONE_GPIO_PIN, &gpiocfg);
}

void spi_slave_tx_ready(char set)
{
    TRACE(0,"%s enter, set %d", __func__, set);
    if (set) {
        hal_gpio_pin_set(SLAVE_TX_READY_GPIO_PIN);
    } else {
        hal_gpio_pin_clr(SLAVE_TX_READY_GPIO_PIN);
    }
}

void spi_slave_rx_ready(char set)
{
    TRACE(0,"%s enter, set %d", __func__, set);
    if (set) {
        hal_gpio_pin_set(SLAVE_RX_READY_GPIO_PIN);
    } else {
        hal_gpio_pin_clr(SLAVE_RX_READY_GPIO_PIN);
    }
}


void spi_dma_send_recv_callback(int error)
{
    TRACE(0,"enter %s error %d", __func__, error);
    if(error!=0)
    {
        TRACE(3,"spi send error:%d",error);
    }
    // osSemaphoreRelease(spi_io_sem_id);
}

static void spi_dma_stop_recv(void)
{

#if (SPI_ID_SELECT == 1)
    while(hal_spilcd_busy());
#else
    while (hal_spi_busy());
#endif

#if (SPI_ID_SELECT == 0)
    hal_spi_stop_dma_recv();
#elif
    hal_spilcd_stop_dma_recv();
#endif
}

POSSIBLY_UNUSED static int32_t spi_slave_device_write(uint8_t *pbuf, uint32_t len, uint8_t enable_dma)
{
    int32_t ret = 0;

#if (SPI_ID_SELECT == 0)
    if (enable_dma) {
#ifdef SPI_DMA_LINK_ENABLE
        ret = hal_spi_dma_send_desc(pbuf, len, spi_master_dma_desc, SPI_DMA_DESC_CNT, spi_dma_send_recv_callback);
#else
        ret = hal_spi_dma_send(pbuf, len, spi_dma_send_recv_callback);
#endif
    } else {
        ret = hal_spi_send(pbuf, len);
    }
#elif (SPI_ID_SELECT == 1)
    if (enable_dma) {
        ret = hal_spilcd_dma_send(pbuf, len, spi_dma_send_recv_callback);
    } else {
        ret = hal_spilcd_send(pbuf, len);
    }
#endif
    TRACE(3 , "hal spi send ret = %d",ret);
    return ret;
}

POSSIBLY_UNUSED static int32_t spi_slave_device_read(uint8_t *cmd, uint8_t *pbuf, uint32_t len, uint8_t enable_dma)
{
    int32_t ret = 0;

#if (SPI_ID_SELECT == 0)
    if (enable_dma) {
#ifdef SPI_DMA_LINK_ENABLE
        ret = hal_spi_dma_recv_desc(cmd, pbuf, BES_SPI_DMA_MAX_LEN, spi_master_dma_desc, SPI_DMA_DESC_CNT * 2, spi_dma_send_recv_callback);
#else
        ret = hal_spi_dma_recv(cmd, pbuf, len, spi_dma_send_recv_callback);
#endif
    } else {
        ret = hal_spi_recv(cmd, pbuf, len);
    }
#elif (SPI_ID_SELECT == 1)
    if (enable_dma) {
        ret = hal_spilcd_dma_recv(cmd, pbuf, len, spi_dma_send_recv_callback);
    } else {
        ret = hal_spilcd_recv(cmd, pbuf, len);
    }
#endif

    return ret;
}

static const unsigned long crc_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

unsigned long pal_crc32(unsigned long crc, const unsigned char *buf, unsigned int len)
{
    unsigned long c;

    c = crc ^ 0xffffffffL;

    while (len) {
        c = crc_table[(c ^ (*buf++)) & 0xff] ^ (c >> 8);
        len--;
    }

    return c ^ 0xffffffffL;
}

POSSIBLY_UNUSED static void rand_generator(uint8_t *dest, uint32_t size)
{
    while (size--) {
        *dest++ = (uint8_t)rand();
    }
}

void spi_slv_test(void)
{

    POSSIBLY_UNUSED uint32_t spi_tx_data = 0;
    POSSIBLY_UNUSED uint32_t spi_rx_data = 0;
    POSSIBLY_UNUSED int32_t ret_tx;
    POSSIBLY_UNUSED int32_t ret_rx;
    POSSIBLY_UNUSED uint32_t val;
    uint32_t error = 0;

    TRACE(0, "   ");
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_208M);
    TRACE(1, "CPU frequency changed to: %uM", hal_sys_timer_calc_cpu_freq(5, 0) / 1000000);

    TRACE(0, "-------SPI slave test started------");
    TRACE(1, "CPU freq: %uM", hal_sys_timer_calc_cpu_freq(5, 0) / 1000000);

    memset(&spi_cfg, 0, sizeof(spi_cfg));
    spi_cfg.clk_delay_half = true;
    spi_cfg.clk_polarity = true;

    spi_cfg.slave = true;

#ifdef SPI_DMA_ENABLE
    spi_cfg.dma_rx = true;
    spi_cfg.dma_tx = true;
#else
    spi_cfg.dma_rx = false;
    spi_cfg.dma_tx = false;
#endif
    spi_cfg.rx_sep_line = false;
    spi_cfg.cs = 0;
    spi_cfg.rate = 6 * 1000 * 1000;//PERIPH_PLL_FREQ frequency is twice this frequency
    spi_cfg.rx_bits = 32;
    spi_cfg.tx_bits = 32;
    spi_cfg.rx_frame_bits = 0;

    TRACE(0, "  ");
    TRACE(1, "spi_cfg.clk_delay_half:%d", spi_cfg.clk_delay_half);
    TRACE(1, "spi_cfg.clk_polarity  :%d", spi_cfg.clk_polarity);
    TRACE(1, "spi_cfg.slave         :%d", spi_cfg.slave);
    TRACE(1, "spi_cfg.dma_rx        :%d", spi_cfg.dma_rx);
    TRACE(1, "spi_cfg.dma_tx        :%d", spi_cfg.dma_tx);
    TRACE(1, "spi_cfg.rx_sep_line   :%d", spi_cfg.rx_sep_line);
    TRACE(1, "spi_cfg.cs            :%d", spi_cfg.cs);
    TRACE(1, "spi_cfg.rate          :%d", spi_cfg.rate);
    TRACE(1, "spi_cfg.rx_bits       :%d", spi_cfg.rx_bits);
    TRACE(1, "spi_cfg.tx_bits       :%d", spi_cfg.tx_bits);
    TRACE(1, "spi_cfg.rx_frame_bits :%d", spi_cfg.rx_frame_bits);
    TRACE(0, "  ");

#if (SPI_ID_SELECT == 0)
    hal_iomux_set_spi();
#ifndef SPI_AS_MASTER
    hal_iomux_set_spi_slave();
#endif
    error = hal_spi_open(&spi_cfg);
    ASSERT(!error, "%s:%d,spi open error:%d", __func__, __LINE__, error);
#elif (SPI_ID_SELECT == 1)
    hal_iomux_set_spilcd();
#ifndef SPI_AS_MASTER
    hal_iomux_set_spilcd_slave();
    set_slave_iomux();
#endif
    error = hal_spilcd_open(&spi_cfg);
    ASSERT(!error, "%s:%d,spilcd open error:%d", __func__, __LINE__, error);
#endif

    set_slave_iomux();
    bth_slv_thread_init();
}

static int32_t spi_slave_rx_data(uint8_t *pbuf, uint32_t len)
{
    uint32_t rx_crc;
    uint32_t tx_crc;
    int32_t ret_rx;

    ret_rx = spi_slave_device_read(NULL, pbuf, len, spi_cfg.dma_rx);
    TRACE(2, "spi slave rx ret:%d rx buf len %d", ret_rx, len);
    //slave rx ready
    spi_slave_rx_ready(0);
    //master tx done
    osSemaphoreAcquire(spi_io_sem_id, osWaitForever);
    spi_dma_stop_recv();
    DUMP8("%02X ", pbuf, 24);
    buf_head_t buf_head = {0,};
    memcpy(&buf_head, pbuf, sizeof(buf_head));
    if (buf_head.len == 0) {
        TRACE(6, "***s rx len error, check out bus data");
        goto out;
    }
    tx_crc = buf_head.crc32_c;
    rx_crc = pal_crc32(0, (pbuf + sizeof(buf_head)), buf_head.len);

    if (rx_crc == tx_crc) {
        TRACE(5, "---[s rx success]crc32:0x%08X,rx size:%d", rx_crc, buf_head.len);
    } else {
        TRACE(6, "***[s rx fail]tx crc32:0x%08X,rx crc32:0x%08X,rx size:%d", tx_crc, rx_crc, buf_head.len);
    }
out:
    //resum idle
    spi_slave_rx_ready(1);
    return ret_rx;
}

static int32_t spi_slave_tx_data(uint8_t *pbuf, uint32_t len)
{
    uint32_t tx_crc;
    int32_t ret_tx;
    osStatus status;

    ret_tx = spi_slave_device_write(pbuf, len, spi_cfg.dma_tx);
    // TRACE(2, "spi slave tx, ret:%d, head txlen %d buf size %d, crc 0x%08X", ret_tx, buf_head.len, BUF_SIZE, tx_crc);

    // slave tx req
    spi_slave_tx_ready(0);

    // wait for master rx done
    status = osSemaphoreAcquire(spi_io_sem_id, osWaitForever);
    //resum idle
    spi_slave_tx_ready(1);

    return ret_tx;
}

void slv_thread_entry(void *param)
{
    while (1) {
        osEvent evt;
        //wait any signal
        evt = osSignalWait(0x0, osWaitForever);

        //get role from signal value
        if (evt.status == osEventSignal) {
            if (evt.value.signals == CMD_SPI_RCV_REQ) {
                spi_slave_rx_data(rx_buf, sizeof(rx_buf));
                signal_send_to_slv_thread(CMD_SPI_SEND_REQ);
            } else if (evt.value.signals == CMD_SPI_SEND_REQ) {
                memcpy(tx_buf, rx_buf, sizeof(rx_buf));
                spi_slave_tx_data(tx_buf, sizeof(tx_buf));
            }
        }
    }
}

POSSIBLY_UNUSED
static void bth_slv_thread_init(void)
{
    spi_io_sem_init();
    slv_thread_id = osThreadNew(slv_thread_entry, NULL, &slv_thread_attr);
    ASSERT(slv_thread_id != NULL, "gsensor thread init failed!");
}

#endif
