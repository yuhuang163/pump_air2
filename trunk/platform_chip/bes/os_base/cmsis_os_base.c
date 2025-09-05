/*
* INCLUDE FILES
*****************************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os_api.h" 
#include "rtx_lib.h" 

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
bool is_interrupt_handler_wrapper(void)
{
    if (IsIrqMode() || IsIrqMasked()) {
        return 1;
    } else {
        return 0;
    }
}

uint32_t os_kernel_systick_wrapper(void)
{
    return osKernelGetSysTimerCount();
}

