#include "assert.h"
#include "los_printf.h"
#include "los_exc.h"

_Noreturn void __assert_fail(const char *expr, const char *file, int line, const char *func)
{
	PRINT_ERR("%s:%d: %s: assertion \"%s\" failed", file, line, func, expr);
	LOS_Panic("__assert error:\n");
	while (1);
}
