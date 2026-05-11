#ifndef GPIO_THREE_KEY_GLOBAL_ROUTE_H
#define GPIO_THREE_KEY_GLOBAL_ROUTE_H

#include <stdint.h>

/**
 * Try to handle keys for GPIO three-key / no-screen layout before WM dispatch.
 * @return non-zero if the event was consumed (WM will not receive MSG_TYPE_INPUT_KEY_EVENT).
 */
int gpio_three_key_global_input_try_route(uint16_t keycode, uint16_t keytype);

#endif /* GPIO_THREE_KEY_GLOBAL_ROUTE_H */
