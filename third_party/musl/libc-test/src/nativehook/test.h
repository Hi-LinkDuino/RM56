#include <stdint.h>
#include <unistd.h>

#define T_LOC2(l) __FILE__ ":" #l
#define T_LOC1(l) T_LOC2(l)
#define t_error(...) t_printf(T_LOC1(__LINE__) ": " __VA_ARGS__)

extern volatile int t_status;

int t_printf(const char *s, ...);
