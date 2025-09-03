#include "bthost_logger.h"

#include <stdarg.h>

#include "hal_trace.h"

const char *bthost_module_strs[] =
{
    [TEST_MODULE] = "TEST",
    [STACK_MODULE] = "STACK",
    [SERVICE_MODULE] = "SERV",

    [L2CAP_MODULE] = "L2CAP",
    [HCI_MODULE] = "HCI",

    [APP_BLE_MODULE] = "BLE",
    [CS_MODULE] = "CS",
    [GAP_MODULE] = "GAP",
    [GATT_MODULE] = "GATT",
    [BAP_MODULE] = "BAP",
    [SMP_MODULE] = "SMP",

    [NAPP_MODULE] = "NAPP",
    [NBTT_MODULE] = "NBTT",
    [NDIP_MODULE] = "NDIP",
    [HIOD_MODULE] = "HIOD",
    [HIOH_MODULE] = "HIOH",
    [NSPP_MODULE] = "NSPP",
    [HTST_MODULE] = "HTST",
    [DTCP_MODULE] = "DTCP",
    [NHCI_MODULE] = "NHCI",
    [SSAP_MODULE] = "SSAP",
};

static uint8_t module_level[] =
{
    [TEST_MODULE] = TEST_LEVEL,
    [STACK_MODULE] = STACK_LEVEL,
    [SERVICE_MODULE] = SERVICE_LEVEL,

    [L2CAP_MODULE] = L2CAP_LEVEL,
    [HCI_MODULE] = HCI_LEVEL,

    [APP_BLE_MODULE] = APP_BLE_LEVEL,
    [CS_MODULE] = CS_LEVEL,
    [GAP_MODULE] = GAP_LEVEL,
    [GATT_MODULE] = GATT_LEVEL,
    [BAP_MODULE] = BAP_LEVEL,
    [SMP_MODULE] = SMP_LEVEL,

    [NAPP_MODULE] = NAPP_LEVEL,
    [NBTT_MODULE] = NBTT_LEVEL,
    [NDIP_MODULE] = NDIP_LEVEL,
    [HIOD_MODULE] = HIOD_LEVEL,
    [HIOH_MODULE] = HIOH_LEVEL,
    [HTST_MODULE] = HTST_LEVEL,
    [DTCP_MODULE] = DTCP_LEVEL,
    [NHCI_MODULE] = NHCI_LEVEL,
    [SSAP_MODULE] = SSAP_LEVEL,
};

static inline void check_level(uint8_t level)
{
    ASSERT(level <= LOGGER_LEVEL_FATAL, "%s illegal level %d", __func__, level);
}

void bthost_set_test_level(uint8_t level)
{
    check_level(level);
    module_level[TEST_MODULE] = level;
}

void bthost_set_stack_level(uint8_t level)
{
    check_level(level);

    module_level[STACK_MODULE] = level;

    module_level[L2CAP_LEVEL] = level;
    module_level[HCI_LEVEL] = level;

    module_level[CS_LEVEL] = level;
    module_level[GAP_LEVEL] = level;
    module_level[GATT_LEVEL] = level;
    module_level[BAP_LEVEL] = level;
    module_level[SMP_LEVEL] = level;

    module_level[DTCP_LEVEL] = level;
    module_level[NHCI_LEVEL] = level;
    module_level[SSAP_LEVEL] = level;
}

void bthost_set_service_level(uint8_t level)
{
    check_level(level);
    module_level[SERVICE_LEVEL] = level;

    module_level[APP_BLE_LEVEL] = level;

    module_level[NAPP_LEVEL] = level;
    module_level[NBTT_LEVEL] = level;
    module_level[NDIP_LEVEL] = level;
    module_level[HIOD_LEVEL] = level;
    module_level[HIOH_LEVEL] = level;
    module_level[NSPP_LEVEL] = level;
    module_level[HTST_LEVEL] = level;
}

static inline bool check_module_level(uint32_t attr)
{
    uint8_t module = ATTR2MODULE(attr);
    uint8_t level = ATTR2LEVEL(attr);

    return module_level[module] <= level;
}

void bthost_dump(uint32_t attr, uint8_t size, const void *data, size_t count)
{
    if (!check_module_level(attr))
    {
        return;
    }

    const char *format = NULL;
    switch (size)
    {
    case sizeof(uint8_t):
        format = "%02x ";
    break;
    case sizeof(uint16_t):
        format = "%04x ";
    break;
    case sizeof(uint32_t):
        format = "%08x ";
    break;
    }

    hal_trace_dump(format, size, count, data);
}

static void vlog(uint32_t attr, const char *format, va_list args)
{
    if (!check_module_level(attr))
    {
        return;
    }

    uint8_t level = ATTR2LEVEL(attr);

    uint32_t level_map[] =
    {
        [LOGGER_LEVEL_TRACE] = TR_ATTR_LEVEL(TR_LEVEL_VERBOSE),
        [LOGGER_LEVEL_DEBUG] = TR_ATTR_LEVEL(TR_LEVEL_DEBUG),
        [LOGGER_LEVEL_INFO]  = TR_ATTR_LEVEL(TR_LEVEL_INFO),
        [LOGGER_LEVEL_WARN]  = TR_ATTR_LEVEL(TR_LEVEL_WARN),
        [LOGGER_LEVEL_ERROR] = TR_ATTR_LEVEL(TR_LEVEL_ERROR),
        [LOGGER_LEVEL_FATAL] = TR_ATTR_LEVEL(TR_LEVEL_CRITICAL),
    };

    uint32_t raw_attr = level_map[level];

    if (attr & FLUSH_BIT)
    {
        raw_attr |= TR_ATTR_IMM;
    }

    hal_trace_printf_internal(raw_attr, format, args);
}

void bthost_log(uint32_t attr, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vlog(attr, format, args);

    va_end(args);
}

void bthost_crash(uint32_t attr, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vlog(attr, format, args);
    va_end(args);

    uint8_t module = ATTR2MODULE(attr);
    uint16_t line = ATTR2LINE(attr);
    ASSERT(0, "%s %s :%d %p", bthost_module_strs[module], __func__, line, __builtin_return_address(0));
}



const char bthost_strict_format_0[] = "%s %04x #%04d %s";
const char bthost_strict_format_1[] = "%s %04x #%04d %s %x";
const char bthost_strict_format_2[] = "%s %04x #%04d %s %x %x";
const char bthost_strict_format_3[] = "%s %04x #%04d %s %x %x %x";
const char bthost_strict_format_4[] = "%s %04x #%04d %s %x %x %x %x";
const char bthost_tag_placeholder[] = "NONE";