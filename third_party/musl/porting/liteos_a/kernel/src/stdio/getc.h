#include "stdio_impl.h"
#include "pthread_impl.h"

#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static int locking_getc(FILE *f)
{
	FLOCK(f);
	int c = getc_unlocked(f);
	FUNLOCK(f);
	return c;
}

static inline int do_getc(FILE *f)
{
	return locking_getc(f);
}
