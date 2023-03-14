#ifdef HOOK_ENABLE
#include "musl_preinit_common.h"
#include "musl_malloc.h"
#include <stdatomic.h>
#include <malloc.h>
#include <stdlib.h>

struct musl_libc_globals __musl_libc_globals;

struct MallocDispatchType __libc_malloc_default_dispatch = {
	.malloc = MuslMalloc(malloc),
	.free = MuslMalloc(free),
};

volatile atomic_bool __hook_enable_hook_flag;

#endif
