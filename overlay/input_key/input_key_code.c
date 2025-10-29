/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "input_key_code.h"  // IWYU pragma: keep

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
const char *get_input_key_code_name(uint8_t code)
{
    switch (code) {
        default:
            return "@unknow";
        case KEY_CODE_POWER:
            return "@power";
        case KEY_CODE_START:
            return "@start";
        case KEY_CODE_MODE:
            return "@mode";
        case KEY_CODE_FREQUENCY:
            return "@frequency";
        case KEY_CODE_BREASTFEEDING:
            return "@breastfeeding";
        case KEY_CODE_LEFT_CONTROL:
            return "@left_control";
        case KEY_CODE_RIGHT_CONTROL:
            return "@right_control";
    }
}