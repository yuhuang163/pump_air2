/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_lcd_uart.h"
#include "uart_extension_lcd.h" // IWYU pragma: keep

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

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
__WEAK void knob_button_event_handler(int arg1, int arg2, void *param)
{

}

int bsp_lcd_init(void) 
{
    bsp_lcd_uart_register_event_cb(LCD_UART_KNOB_BUTTON_EVENT, knob_button_event_handler);
    return bsp_lcd_uart_init();
}


