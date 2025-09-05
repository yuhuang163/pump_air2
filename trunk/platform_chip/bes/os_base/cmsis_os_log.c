#include "hal_trace.h"
#include "cmsis_os_api.h"
#include <stdarg.h>

void platform_logf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    hal_trace_printf_internal(TR_ATTR_LEVEL(TR_LEVEL_INFO), fmt, ap);
    va_end(ap);
}


