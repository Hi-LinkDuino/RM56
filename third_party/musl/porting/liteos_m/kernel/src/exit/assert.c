#include "assert.h"
#include "los_context.h"
#include "los_debug.h"
#include "los_task.h"

void __assert_fail(const char* expr, const char* file, int line, const char* func) {
  PRINT_ERR("%s:%d: %s: assertion \"%s\" failed\n", file, line, func, expr);
  LOS_Panic("__assert error:\n");
  while (1);
}
