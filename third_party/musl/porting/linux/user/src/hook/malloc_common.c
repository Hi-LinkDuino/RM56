#ifdef HOOK_ENABLE
#include <unistd.h>
#include <sys/types.h>
#include "musl_malloc.h"
#include <malloc.h>
#include "musl_malloc_dispatch_table.h"
#include "common_def.h"
#include "musl_preinit_common.h"

void* malloc(size_t bytes)
{
	volatile const struct MallocDispatchType* dispatch_table = get_current_dispatch_table();
	if (__predict_false(dispatch_table != NULL)) {
		void*ret = dispatch_table->malloc(bytes);
		return ret;
	}
	void* result = MuslMalloc(malloc)(bytes);
	if (__predict_false(result == NULL)) {
		//__musl_log(__MUSL_LOG_WARN, "malloc(%zu) failed: returning null pointer\n", bytes);
	}
	return result;
}

void free(void* mem)
{
	volatile const struct MallocDispatchType* dispatch_table = get_current_dispatch_table();
	if (__predict_false(dispatch_table != NULL)) {
		dispatch_table->free(mem);
	} else {
		MuslMalloc(free)(mem);
	}
}
#endif
