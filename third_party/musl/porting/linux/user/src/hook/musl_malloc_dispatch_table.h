#ifndef _MUSL_MALLOC_DISPATCH_TABLE_H
#define _MUSL_MALLOC_DISPATCH_TABLE_H

#include "musl_malloc_dispatch.h"
#include <stdatomic.h>

struct musl_libc_globals {
	volatile atomic_llong current_dispatch_table;
	volatile atomic_llong so_dispatch_table;
	struct MallocDispatchType malloc_dispatch_table;
};

#endif
