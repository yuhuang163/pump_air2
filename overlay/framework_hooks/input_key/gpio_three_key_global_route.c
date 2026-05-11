/*
 * Central routing for three-key products: runs from input_key_hook before dispatch_key_event_wrapper.
 */
#include "gpio_three_key_global_route.h"
#include "project_support_overlay.h" // IWYU pragma: keep (IS_SUPPORT_GPIO_THREE_KEY_LAYOUT)

#if IS_SUPPORT_GPIO_THREE_KEY_LAYOUT

#include "airplane_mode_api.h"
#include "battery_manager_api.h"
#include "input_key_code.h"
#include "power_manager_api.h"
#include "root_common_api.h"
#include "tests_manager_api.h"
#include "windows_manager.h"
#include "windows_map_define.h"

extern void gpio_three_key_global_switch_pump_normal_mode(bool is_increase);

static void three_key_navigate_exit_charge(window_manager_context_t *wm)
{
    airplane_mode_context_t *airplane = (airplane_mode_context_t *)find_module(ID_MODULE_AIRPLANE_MODE);
    multi_func_msg_data_t msg = {0};
    if (airplane && airplane->is_on_airplane_mode()) {
        msg.union_body.extra.window_id = ID_WINDOW_AIRPLANE_MODE;
    } else if (wm->is_window_guidance_deployed() == 0) {
        msg.union_body.extra.window_id = ID_WINDOW_GUIDE;
    } else {
        msg.union_body.extra.window_id = ID_WINDOW_HOME;
    }
    wm->dispatch_new_window_args(wm, &msg);
}

static int three_key_dummy_power_click(window_manager_context_t *wm)
{
    power_manager_context_t *pm = (power_manager_context_t *)find_module(ID_MODULE_POWER);
    tests_manager_context_t *tests = (tests_manager_context_t *)find_module(ID_MODULE_TESTS);

    if (pm) {
        pm->pm_wakeup(PM_WAKEUP_REASON_KEY_POWER);
    }
    multi_func_msg_data_t msg = {0};
    if (tests && tests->tests_manager_get_current_mode() != TST_NONE_TYPE) {
        msg.union_body.extra.window_id = ID_WINDOW_PRODUCT_TEST_MODE;
        LOG_KEY("gpio3 dummy power -> product test");
    } else {
        msg.union_body.extra.window_id = ID_WINDOW_HOME;
        LOG_KEY("gpio3 dummy power -> home");
    }
    wm->dispatch_new_window_args(wm, &msg);
    return 1;
}

int gpio_three_key_global_input_try_route(uint16_t keycode, uint16_t keytype)
{
    if ((keytype & KEY_EVENT_CLICK) == 0) {
        return 0;
    }

    window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
    if (!wm) {
        return 0;
    }

    uint8_t wid = wm->get_current_window_id();

    /* Home keeps existing handlers (e.g. power -> enter pump flow). */
    if (wid == ID_WINDOW_HOME) {
        return 0;
    }

    /* Pump window keeps delayed single / double-click policy in window_screen_pump_handle. */
    if (wid == ID_WINDOW_PUMP || wm->get_overlay_window_id(wm) == ID_WINDOW_PUMP) {
        return 0;
    }

    if (wid == ID_WINDOW_CHARGE) {
        if ((keycode & (KEY_CODE_POWER | KEY_CODE_KNOB_LEFT | KEY_CODE_KNOB_RIGHT)) == 0) {
            return 0;
        }
        battery_manager_context_t *bat =
            (battery_manager_context_t *)find_module(ID_MODULE_BATTERY);
        if (bat && bat->is_adapter_mismatch()) {
            return 0;
        }
        if (keycode & KEY_CODE_POWER) {
            LOG_KEY("gpio3 charge: power click -> exit charge");
            three_key_navigate_exit_charge(wm);
        } else {
            bool inc = (keycode & KEY_CODE_KNOB_RIGHT) != 0;
            LOG_KEY("gpio3 charge: knob -> mode %s", inc ? "inc" : "dec");
            gpio_three_key_global_switch_pump_normal_mode(inc);
        }
        return 1;
    }

    if (wid == ID_WINDOW_DUMMY) {
        if (keycode & KEY_CODE_POWER) {
            return three_key_dummy_power_click(wm);
        }
        if ((keycode & KEY_CODE_KNOB_LEFT) || (keycode & KEY_CODE_KNOB_RIGHT)) {
            gpio_three_key_global_switch_pump_normal_mode((keycode & KEY_CODE_KNOB_RIGHT) != 0);
            LOG_KEY("gpio3 dummy: knob mode switch");
            return 1;
        }
    }

    return 0;
}

#else /* !IS_SUPPORT_GPIO_THREE_KEY_LAYOUT */

int gpio_three_key_global_input_try_route(uint16_t keycode, uint16_t keytype)
{
    (void)keycode;
    (void)keytype;
    return 0;
}

#endif /* IS_SUPPORT_GPIO_THREE_KEY_LAYOUT */
