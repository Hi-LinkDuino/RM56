#include "stdio_impl.h"
#include "pthread_impl.h"

#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static int locking_putc(int c, FILE *f)
{
    FLOCK(f);
    c = putc_unlocked(c, f);
    FUNLOCK(f);
    return c;
}

static inline int do_putc(int c, FILE *f)
{
	return locking_putc(c, f);
}
