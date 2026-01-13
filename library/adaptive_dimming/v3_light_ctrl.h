#ifndef _V3_LIGHT_CTRL_H_
#define _V3_LIGHT_CTRL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "algo_port.h"

/*******************************************************************************
 * Function Declarations
 ******************************************************************************/
void algo_light_handle(unsigned int lux_level_input, algo_port_light_result_t *output);

#ifdef __cplusplus
}
#endif

#endif
