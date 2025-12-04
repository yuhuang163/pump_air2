/**************************************************************************//**
 * @file     best1605_bth.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           ARMCM33 Device Series
 * @version  V2.02
 * @date     10. September 2014
 *
 * @note     configured for CM33 with FPU
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2014 ARM LIMITED
   Copyright (c) 2021 BES

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef __BEST1605_BTH_H__
#define __BEST1605_BTH_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__
/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Cortex-M33 Processor Exceptions Numbers  ------------------ */
    NonMaskableInt_IRQn         = -14,      /*!<  2 Non Maskable Interrupt          */
    HardFault_IRQn              = -13,      /*!<  3 HardFault Interrupt             */
    MemoryManagement_IRQn       = -12,      /*!<  4 Memory Management Interrupt     */
    BusFault_IRQn               = -11,      /*!<  5 Bus Fault Interrupt             */
    UsageFault_IRQn             = -10,      /*!<  6 Usage Fault Interrupt           */
    SVCall_IRQn                 =  -5,      /*!< 11 SV Call Interrupt               */
    DebugMonitor_IRQn           =  -4,      /*!< 12 Debug Monitor Interrupt         */
    PendSV_IRQn                 =  -2,      /*!< 14 Pend SV Interrupt               */
    SysTick_IRQn                =  -1,      /*!< 15 System Tick Interrupt           */

/* --------------------  BEST1605 Specific Interrupt Numbers  --------------------- */
    FPU_IRQn                    =   0,      /*!< FPU Interrupt                 */
    SPI_WAKEUP_IRQn             =   1,      /*!< SPI Slave Wakeup Interrupt         */
    SYS_USB_IRQn                =   2,      /*!< SYS USB Interrupt                  */
    SYS_I2C0_IRQn               =   3,      /*!< SYS I2C0 Interrupt                 */
    AUDMA_IRQn                  =   4,      /*!< Audio DMA Interrupt                */
    AON_WDT_IRQn                =   5,      /*!< AON Watchdog Timer Interrupt       */
    SYS_SPI0_IRQn               =   6,      /*!< SYS SPI0 Interrupt                 */
    SYS_I2S0_IRQn               =   7,      /*!< SYS I2S0 Interrupt                 */
    WAKEUP_IRQn                 =   8,      /*!< Wakeup Interrupt                   */
    GPDMA_IRQn                  =   9,      /*!< General Purpose DMA Interrupt      */
    MCU_WDT_IRQn                =  10,      /*!< Watchdog Timer Interrupt           */
    SYS_SPDIF0_IRQn             =  11,      /*!< SYS SPDIF0 Interrupt               */
    MCU_TIMER00_IRQn            =  12,      /*!< Timer00 Interrupt                  */
    MCU_TIMER01_IRQn            =  13,      /*!< Timer01 Interrupt                  */
    SYS_USB_PIN_IRQn            =  14,      /*!< SYS_USB_PIN Interrupt              */
    SYS_AUDMA_IRQn              =  15,      /*!< SYS AUDMA Interrupt                */
    SYS_GPDMA_IRQn              =  16,      /*!< SYS GPDMA Interrupt                */
    UART0_IRQn                  =  17,      /*!< UART0 Interrupt                    */
    UART1_IRQn                  =  18,      /*!< UART1 Interrupt                    */
    CODEC_IRQn                  =  19,      /*!< CODEC Interrupt                    */
    BTPCM_IRQn                  =  20,      /*!< BTPCM Interrupt                    */
    SYS_I2S1_IRQn               =  21,      /*!< SYS I2S1 Interrupt                 */
    SYS_SPILCD_IRQn             =  22,      /*!< SYS SPILCD Interrupt               */
    SYS_I2C1_IRQn               =  23,      /*!< SYS I2C1 Interrupt                 */
    BT_IRQn                     =  24,      /*!< BT to MCU Interrupt                */
    CODEC_TX_PEAK_IRQn          =  25,      /*!< CODEC TX PEAK Interrupt            */
    SYS_IRQn                    =  26,      /*!< SYS ALL Interrupt                  */
    MCU_TIMER10_IRQn            =  27,      /*!< Timer10 Interrupt                  */
    MCU_TIMER11_IRQn            =  28,      /*!< Timer11 Interrupt                  */
    SYS_SEDMA_IRQn              =  29,      /*!< SYS SEDMA Interrupt                */
    DUMP_IRQn                   =  30,      /*!< DUMP Interrupt                     */
    BT_TIME_IRQn                =  31,      /*!< SPIPHY Interrupt                   */
    ISDONE_IRQn                 =  32,      /*!< Intersys MCU2BT Data Done Interrupt */
    ISDONE1_IRQn                =  33,      /*!< Intersys MCU2BT Data1 Done Interrupt */
    ISDATA_IRQn                 =  34,      /*!< Intersys BT2MCU Data Indication Interrupt */
    ISDATA1_IRQn                =  35,      /*!< Intersys BT2MCU Data1 Indication Interrupt */
    BTH2SYS_DONE_IRQn           =  36,      /*!< Intersys BTH2SYS Data Done Interrupt */
    BTH2SYS_DONE1_IRQn          =  37,      /*!< Intersys BTH2SYS Data1 Done Interrupt */
    SYS2BTH_DATA_IRQn           =  38,      /*!< Intersys SYS2BTH Data Indication Interrupt */
    SYS2BTH_DATA1_IRQn          =  39,      /*!< Intersys SYS2BTH Data1 Indication Interrupt */
    PMU1_IRQn                   =  40,      /*!< PMU1 Interrupt                     */
    SYS_SEC_ENG_IRQn            =  41,      /*!< SYS SEC ENG Interrupt              */
    PMU_IRQn                    =  42,      /*!< PMU Interrupt                      */
    SYS_SDMMC_IRQn              =  43,      /*!< SYS SDMMC or EMMC Interrupt        */
    AON_IRQn                    =  44,      /*!< AON Interrupt                      */
    AON_TIMER00_IRQn            =  45,      /*!< AON Timer00 Interrupt              */
    AON_TIMER01_IRQn            =  46,      /*!< AON Timer01 Interrupt              */
    AON_TIMER10_IRQn            =  47,      /*!< AON Timer00 Interrupt              */
    AON_TIMER11_IRQn            =  48,      /*!< AON Timer01 Interrupt              */
    SYS_IMGDMA_IRQn             =  49,      /*!< SYS IMGDMA or GPU Interrupt        */
    SYS_DISPLAY_IRQn            =  50,      /*!< SYS DISPLAY Interrupt              */
    SYS_DSI_IRQn                =  51,      /*!< SYS DSI Interrupt                  */
    UART2_IRQn                  =  52,      /*!< UART2 Interrupt                    */
    I2C0_IRQn                   =  53,      /*!< I2C0 Interrupt                     */
    I2C1_IRQn                   =  54,      /*!< I2C1 Interrupt                     */
    I2C2_IRQn                   =  55,      /*!< I2C2 Interrupt                     */
    I2C3_IRQn                   =  56,      /*!< I2C3 Interrupt                     */
    I2C4_IRQn                   =  57,      /*!< I2C4 Interrupt                     */
    SPI_IRQn                    =  58,      /*!< SPI Interrupt                      */
    SPILCD_IRQn                 =  59,      /*!< SPILCD Interrupt                   */
    SPI2_IRQn                   =  60,      /*!< SPI2 Interrupt                     */
    BT_STAMP_IRQn               =  61,      /*!< BT Playtime Stamp Interrupt        */
    SYS_WDT_IRQn                =  62,      /*!< SYS WDT Interrupt                  */
    BTC_WDT_IRQn                =  63,      /*!< BTC WDT Interrupt                  */
    SYS_SDIO_IRQn               =  64,      /*!< SYS SDIO Interrupt                 */
    SYS_GPU_IRQn                =  65,      /*!< SYS GPU Interrupt                  */
    FPU_CP_IRQn                 =  66,      /*!< FPU CP Interrupt                   */
    MCU_TIMER20_IRQn            =  67,      /*!< Timer20 Interrupt                  */
    MCU_TIMER21_IRQn            =  68,      /*!< Timer21 Interrupt                  */
    MCU_TIMER30_IRQn            =  69,      /*!< Timer30 Interrupt                  */
    MCU_TIMER31_IRQn            =  70,      /*!< Timer31 Interrupt                  */
    I2S0_IRQn                   =  71,      /*!< I2S0 Interrupt                     */
    I2S1_IRQn                   =  72,      /*!< I2S1 Interrupt                     */
    SENSOR_ENG0_IRQn            =  73,      /*!< Sensor Engine0 Interrupt           */
    SENSOR_ENG1_IRQn            =  74,      /*!< Sensor Engine1 Interrupt           */
    SENSOR_ENG2_IRQn            =  75,      /*!< Sensor Engine2 Interrupt           */
    SENSOR_ENG3_IRQn            =  76,      /*!< Sensor Engine3 Interrupt           */
    AON_SPY_IRQn                =  77,      /*!< AON SPY Interrupt                  */
    ISDONE2_IRQn                =  78,      /*!< Intersys MCU2BT Data2 Done Interrupt */
    ISDONE3_IRQn                =  79,      /*!< Intersys MCU2BT Data3 Done Interrupt */
    ISDATA2_IRQn                =  80,      /*!< Intersys BT2MCU Data2 Indication Interrupt */
    ISDATA3_IRQn                =  81,      /*!< Intersys BT2MCU Data3 Indication Interrupt */
    BTH2SYS_DONE2_IRQn          =  82,      /*!< Intersys BTH2SYS Data2 Done Interrupt */
    BTH2SYS_DONE3_IRQn          =  83,      /*!< Intersys BTH2SYS Data3 Done Interrupt */
    SYS2BTH_DATA2_IRQn          =  84,      /*!< Intersys SYS2BTH Data2 Indication Interrupt */
    SYS2BTH_DATA3_IRQn          =  85,      /*!< Intersys SYS2BTH Data3 Indication Interrupt */
    CP2MCU_DATA_IRQn            =  86,      /*!< Intersys CP2MCU Data Indication Interrupt */
    CP2MCU_DATA1_IRQn           =  87,      /*!< Intersys CP2MCU Data1 Indication Interrupt */
    CP2MCU_DATA2_IRQn           =  88,      /*!< Intersys CP2MCU Data2 Indication Interrupt */
    CP2MCU_DATA3_IRQn           =  89,      /*!< Intersys CP2MCU Data3 Indication Interrupt */
    MCU2CP_DONE_IRQn            =  90,      /*!< Intersys MCU2CP Data Done Interrupt  */
    MCU2CP_DONE1_IRQn           =  91,      /*!< Intersys MCU2CP Data1 Done Interrupt */
    MCU2CP_DONE2_IRQn           =  92,      /*!< Intersys MCU2CP Data2 Done Interrupt */
    MCU2CP_DONE3_IRQn           =  93,      /*!< Intersys MCU2CP Data3 Done Interrupt */
    AON_TIMER20_IRQn            =  94,      /*!< AON Timer20 Interrupt              */
    AON_TIMER21_IRQn            =  95,      /*!< AON Timer21 Interrupt              */
    BTC2BTH_SLP_IRQn            =  96,      /*!< BT2MCU Slp Interrupt               */
    ISO_DMA_IRQn                =  97,      /*!< ISO DMA Interrupt                  */

    USER_IRQn_QTY,
    INVALID_IRQn                = USER_IRQn_QTY,
} IRQn_Type;

#define BTH2SYS_DATA_IRQn       SYS2BTH_DATA_IRQn
#define BTH2SYS_DATA1_IRQn      SYS2BTH_DATA1_IRQn
#define BTH2SYS_DATA2_IRQn      SYS2BTH_DATA3_IRQn
#define BTH2SYS_DATA3_IRQn      SYS2BTH_DATA3_IRQn
#define SYS2BTH_DONE_IRQn       BTH2SYS_DONE_IRQn
#define SYS2BTH_DONE1_IRQn      BTH2SYS_DONE1_IRQn
#define SYS2BTH_DONE2_IRQn      BTH2SYS_DONE2_IRQn
#define SYS2BTH_DONE3_IRQn      BTH2SYS_DONE3_IRQn

#define GPIO_IRQn               AON_IRQn
#define GPADC_IRQn              INVALID_IRQn
#define PWRKEY_IRQn             INVALID_IRQn
#define TIMER00_IRQn            INVALID_IRQn
#define TIMER01_IRQn            AON_TIMER01_IRQn
#define TIMER10_IRQn            MCU_TIMER00_IRQn
#define TIMER11_IRQn            MCU_TIMER01_IRQn
#define TIMER20_IRQn            MCU_TIMER10_IRQn
#define TIMER21_IRQn            MCU_TIMER11_IRQn
#define WDT_IRQn                AON_WDT_IRQn
#define DSP_WDT_IRQn            SYS_WDT_IRQn

#define USB_PIN_IRQn            SYS_USB_PIN_IRQn

#ifdef BTH_USE_SYS_PERIPH
#define SDIO_IRQn               SYS_SDIO_IRQn
#define USB_IRQn                SYS_USB_IRQn
#define SPDIF0_IRQn             SYS_SPDIF0_IRQn
#define SEDMA_IRQn              SYS_SEDMA_IRQn
#define SEC_ENG_IRQn            SYS_SEC_ENG_IRQn
#define SDMMC0_IRQn             SYS_SDMMC_IRQn
// TODO: Will add TRNG IRQ in next ECO
#define TRNG_IRQn               INVALID_IRQn
#endif

#endif

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM33_REV                0x0000U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       0U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __DSP_PRESENT             1U        /* DSP extension present */

#include "core_cm33.h"                      /* Processor and core peripherals */

#ifndef __ASSEMBLER__

#include "system_ARMCM.h"                  /* System Header                                     */

#endif

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/* -------------------  Start of section using anonymous unions  ------------------ */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/* --------------------  End of section using anonymous unions  ------------------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

#ifdef __cplusplus
}
#endif

#endif
