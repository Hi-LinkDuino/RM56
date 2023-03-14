#ifndef _MUSL_PREINIT_COMMON_H
#define _MUSL_PREINIT_COMMON_H

#include <stdatomic.h>
#include "musl_malloc_dispatch_table.h"
#include "musl_malloc_dispatch.h"

extern struct musl_libc_globals __musl_libc_globals;

extern struct MallocDispatchType __libc_malloc_default_dispatch;

extern volatile atomic_bool __hook_enable_hook_flag;

enum EnumFunc {
	INITIALIZE_FUNCTION,
	FINALIZE_FUNCTION,
	GET_HOOK_FLAG_FUNCTION,
	SET_HOOK_FLAG_FUNCTION,
	ON_START_FUNCTION,
	ON_END_FUNCTION,
	LAST_FUNCTION,
};

enum EnumHookMode {
	STATRUP_HOOK_MODE,
	DIRECT_HOOK_MODE,
	STEP_HOOK_MODE,
};

#ifdef HOOK_ENABLE
extern void* function_of_shared_lib[];
extern volatile atomic_llong ohos_malloc_hook_shared_library;
#endif // HOOK_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
inline bool __get_global_hook_flag()
{
#ifdef HOOK_ENABLE
	volatile bool g_flag = atomic_load_explicit(&__hook_enable_hook_flag, memory_order_acquire);
	return g_flag;
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline bool __get_hook_flag()
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void *)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return false;
	}
	else if (impl_handle == (void *)-1) {
		return true;
	}
	else {
		GetHookFlagType get_hook_func_ptr = (GetHookFlagType)(function_of_shared_lib[GET_HOOK_FLAG_FUNCTION]);
		bool flag = get_hook_func_ptr();
		return flag;
	}
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline bool __set_hook_flag(bool flag)
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void *)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return false;
	}
	else if (impl_handle == (void *)-1) {
		return true;
	}
	else {
		SetHookFlagType set_hook_func_ptr = (SetHookFlagType)(function_of_shared_lib[SET_HOOK_FLAG_FUNCTION]);
		return set_hook_func_ptr(flag);
	}
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline volatile const struct MallocDispatchType* get_current_dispatch_table()
{
#ifdef HOOK_ENABLE
	volatile const struct MallocDispatchType* ret = (struct MallocDispatchType *)atomic_load_explicit(&__musl_libc_globals.current_dispatch_table, memory_order_acquire);
	if (ret != NULL) {
		if (!__get_global_hook_flag()) {
			ret = NULL;
		}
		else if (!__get_hook_flag()) {
			ret = NULL;
		}
	}
	return ret;
#else
	return NULL;
#endif // HOOK_ENABLE
}

#define MUSL_HOOK_PARAM_NAME "libc.hook_mode"
#define OHOS_PARAM_MAX_SIZE 96

#ifdef __cplusplus
}
#endif

#endif
