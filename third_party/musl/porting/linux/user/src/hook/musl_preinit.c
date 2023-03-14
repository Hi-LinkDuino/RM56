/*
The hook mode has 3 kinds which can be set by command "param set":
(1) param set libc.hook_mode "startup:\"prog1 \""
(2) param set libc.hook_mode startup:program1
(3) param set libc.hook_mode step
(4) param set libc.hook_mode direct

Case 1 represents "startup" mode, and the hooked process is "prog1 ",
which loads hooking shared library when the program starts up.
The path is added with two quotation marks because a quotation mark is a special charcter,
which need be escaped.
(2) is similar with (1), but no escaped character, so quotation marks doesn't need.
(3) represents "step" mode, which loads hooking shared library by some steps.
(4) represetnt  "direct" mode, which loads hooking shared library by a step.
*/

#ifdef HOOK_ENABLE
#include <unistd.h>
#include <signal.h>
#include "musl_malloc_dispatch_table.h"
#include "musl_malloc.h"
#include "musl_preinit_common.h"
#include <pthread.h>
#include <stdlib.h>
#include <limits.h>
#include <dlfcn.h>
#include <stdatomic.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

void* ohos_malloc_hook_init_function(size_t bytes);

static struct MallocDispatchType __ohos_malloc_hook_init_dispatch = {
	.malloc = ohos_malloc_hook_init_function,
	.free = MuslMalloc(free),
};
#define MAX_SYM_NAME_SIZE 1000
static char *__malloc_hook_shared_lib = "libnative_hook.z.so";
static char *__malloc_hook_function_prefix = "ohos_malloc_hook";
static char *__get_param_shared_Lib = "libparam_client.z.so";
volatile atomic_llong ohos_malloc_hook_shared_library;
void* function_of_shared_lib[LAST_FUNCTION];
static enum EnumHookMode __hook_mode = STEP_HOOK_MODE;
static char __hook_process_path[PATH_MAX+ 1] = {0};
static char __progname[PATH_MAX + 1] = {0};


static char* get_native_hook_param()
{
	int (*getFunction)(const char *name, char *value, unsigned int *len);

	void* shared_library_handle = dlopen(__get_param_shared_Lib, RTLD_NOW | RTLD_LOCAL);
	if (!shared_library_handle) {
		return NULL;
	}

	getFunction = (int (*)(const char *name, char *value, unsigned int *len))dlsym((void*)shared_library_handle, "SystemGetParameter");
	if (getFunction == NULL) {
		dlclose(shared_library_handle);
		return NULL;
	}
	const char *key =  MUSL_HOOK_PARAM_NAME;
	char *value = (char *)malloc(OHOS_PARAM_MAX_SIZE);
	if (value == NULL) {
		dlclose(shared_library_handle);
		return NULL;
	}
	memset(value, 0, OHOS_PARAM_MAX_SIZE);
	unsigned int len = OHOS_PARAM_MAX_SIZE;
	getFunction(key, value, &len);
	dlclose(shared_library_handle);
	return value;
}

static int parse_hook_variable(enum EnumHookMode* mode, char* path, int size)
{
	if (!mode || !path || size <= 0) {
		return -1;
	}

	bool flag = __set_hook_flag(false);
	char* hook_param_value = get_native_hook_param();
	if (!hook_param_value) {
		*mode = STEP_HOOK_MODE;
		path[0] = '\0';
	} else {
		char* ptr = hook_param_value;
		char* mode_str = hook_param_value;

		while (*ptr && *ptr != ':') {
			++ptr;
		}

		if (*ptr == ':') {
			*ptr = '\0';
			++ptr;
		}

		if (strcmp(mode_str, "startup") == 0) {
			*mode = STATRUP_HOOK_MODE;
		} else if (strcmp(mode_str, "direct") == 0) {
			*mode = DIRECT_HOOK_MODE;
		} else if (strcmp(mode_str, "step") == 0) {
			*mode = STEP_HOOK_MODE;
		} else {
			*mode = STEP_HOOK_MODE;
		}
		if (*mode == STATRUP_HOOK_MODE) {
			if (*ptr == '\"') {
				++ptr;
				int idx = 0;
				while (idx < size - 1 && *ptr && *ptr != '\"') {
					path[idx++] = *ptr++;
				}
				path[idx] = '\0';
			} else {
				int idx = 0;
				while (idx < size - 1 && *ptr) {
					path[idx++] = *ptr++;
				}
				path[idx] = '\0';
			}
		}

		free(hook_param_value);
	}
	__set_hook_flag(flag);
	return 0;
}

static bool init_malloc_function(void* malloc_shared_library_handler, MallocMallocType* func, const char* prefix)
{
	char symbol[MAX_SYM_NAME_SIZE];
	snprintf(symbol, sizeof(symbol), "%s_%s", prefix, "malloc");
	*func = (MallocMallocType)(dlsym(malloc_shared_library_handler, symbol));
	if (*func == NULL) {
		return false;
	}
	return true;
}

static bool init_free_function(void* malloc_shared_library_handler, MallocFreeType* func, const char* prefix)
{
	char symbol[MAX_SYM_NAME_SIZE];
	snprintf(symbol, sizeof(symbol), "%s_%s", prefix, "free");
	*func = (MallocFreeType)(dlsym(malloc_shared_library_handler, symbol));
	if (*func == NULL) {
		return false;
	}
	return true;
}

static bool init_hook_functions(void* shared_library_handler, struct MallocDispatchType* table, const char* prefix)
{
	if (!init_malloc_function(shared_library_handler, &table->malloc, prefix)) {
		return false;
	}
	if (!init_free_function(shared_library_handler, &table->free, prefix)) {
		return false;
	}
	return true;
}

static void clear_function_table()
{
	for (size_t i = 0; i < LAST_FUNCTION; i++) {
		function_of_shared_lib[i] = NULL;
	}
}

bool init_malloc_hook_shared_library(void* shared_library_handle, const char* shared_lib, const char* prefix, struct MallocDispatchType* dispatch_table)
{
	static const char* names[] = {
		"initialize",
		"finalize",
		"get_hook_flag",
		"set_hook_flag",
		"on_start",
		"on_end",
	};

	for (int i = 0; i < LAST_FUNCTION; i++) {
		char symbol[MAX_SYM_NAME_SIZE];
		snprintf(symbol, sizeof(symbol), "%s_%s", prefix, names[i]);
		function_of_shared_lib[i] = dlsym(shared_library_handle, symbol);
		if (function_of_shared_lib[i] == NULL) {
			// __musl_log(__MUSL_LOG_ERROR, "%s: %s routine not found in %s\n", getprogname(), symbol, shared_lib);
			clear_function_table();
			return false;
		}
	}

	if (!init_hook_functions(shared_library_handle, dispatch_table, prefix)) {
		clear_function_table();
		return false;
	}

	return true;
}

void* load_malloc_hook_shared_library(const char* shared_lib, const char* prefix, struct MallocDispatchType* dispatch_table)
{
	void* shared_library_handle = NULL;

	shared_library_handle = dlopen(shared_lib, RTLD_NOW | RTLD_LOCAL);

	if (shared_library_handle == NULL) {
		printf("Unable to open shared library %s: %s\n", shared_lib, dlerror());
		return NULL;
	}

	if (!init_malloc_hook_shared_library(shared_library_handle, shared_lib, prefix, dispatch_table)) {
		dlclose(shared_library_handle);
		shared_library_handle = NULL;
	}
	// printf("load_malloc_hook_shared_library success new version test\n");
	return shared_library_handle;
}

typedef void (*finalize_func_t)();
typedef bool (*init_func_t)(const struct MallocDispatchType*, bool*, const char*);
typedef bool (*on_start_func_t)();
typedef bool (*on_end_func_t)();

static void malloc_finalize()
{
	__set_hook_flag(false);
	((finalize_func_t)function_of_shared_lib[FINALIZE_FUNCTION])();

	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
}

bool finish_install_ohos_malloc_hooks(struct musl_libc_globals* globals, const char* options, const char* prefix)
{
	init_func_t init_func = (init_func_t)(function_of_shared_lib[INITIALIZE_FUNCTION]);
	if (!init_func(&__libc_malloc_default_dispatch, NULL, options)) {
		// __musl_log(__MUSL_LOG_ERROR, "%s: failed to enable malloc %s\n", getprogname(), prefix);
		clear_function_table();
		return false;
	}
	on_start_func_t start_func = (on_start_func_t)(function_of_shared_lib[ON_START_FUNCTION]);
	if (!start_func()) {
		// __musl_log(__MUSL_LOG_ERROR, "%s: failed to start %s\n", getprogname(), prefix);
		clear_function_table();
		return false;
	}
	atomic_store_explicit(&__musl_libc_globals.so_dispatch_table, (volatile long long)&globals->malloc_dispatch_table, memory_order_seq_cst);
	atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile long long)&globals->malloc_dispatch_table, memory_order_seq_cst);

	int ret_value = atexit(malloc_finalize);
	if (ret_value != 0) {
		// __musl_log(__MUSL_LOG_ERROR, "failed to set atexit cleanup function: %d\n", ret_value);
	}
	return true;
}

static bool is_empty_string(const char* str)
{
	while (*str) {
		if (!isspace((unsigned char)(*str))) {
			return false;
		}
	}
	return true;
}

static void install_ohos_malloc_hook(struct musl_libc_globals* globals)
{
	volatile void* shared_library_handle = (volatile void *)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	assert(shared_library_handle == NULL || shared_library_handle == (volatile void*)-1);
	shared_library_handle = (volatile void*)load_malloc_hook_shared_library(__malloc_hook_shared_lib, __malloc_hook_function_prefix, &globals->malloc_dispatch_table);
	if (shared_library_handle == NULL) {
		// __musl_log(__MUSL_LOG_ERROR, "Can't load shared library '%s'\n", __malloc_hook_shared_lib);
		return;
	}

	if (finish_install_ohos_malloc_hooks(globals, NULL, __malloc_hook_function_prefix)) {
		atomic_store_explicit(&ohos_malloc_hook_shared_library, (volatile long long)shared_library_handle, memory_order_seq_cst);
	} else {
		// __musl_log(__MUSL_LOG_ERROR, "finish_install_ohos_malloc_hooks failed\n");
		dlclose((void *)shared_library_handle);
		atomic_store_explicit(&ohos_malloc_hook_shared_library, (volatile long long)0, memory_order_seq_cst);
	}
}

static void* init_ohos_malloc_hook()
{
	bool flag = __set_hook_flag(false);
	install_ohos_malloc_hook(&__musl_libc_globals);
	__set_hook_flag(flag);
	return NULL;
}

void* ohos_malloc_hook_init_function(size_t bytes)
{
	if (atomic_exchange(&__musl_libc_globals.current_dispatch_table, (volatile const long long)NULL)) {
		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, init_ohos_malloc_hook, NULL) != 0) {
			// __musl_log(__MUSL_LOG_ERROR, "%s: ohos_malloc_hook: failed to pthread_create\n", getprogname());
		} else if (pthread_detach(thread_id) != 0) {
			// __musl_log(__MUSL_LOG_ERROR, "%s: ohos_malloc_hook: failed to pthread_detach\n", getprogname());
		}
	}
	void*ptr = MuslMalloc(malloc)(bytes);
	return ptr;

}

static void __set_default_malloc()
{
	atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile const long long)NULL, memory_order_seq_cst);
}

static void __install_malloc_hook()
{
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)true, memory_order_seq_cst);

	volatile void* shared_library_handle = (volatile void* )atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (shared_library_handle == NULL) {
		if (__hook_mode == STEP_HOOK_MODE) {
			atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile const long long)&__ohos_malloc_hook_init_dispatch, memory_order_seq_cst);
			atomic_store_explicit(&ohos_malloc_hook_shared_library, (volatile long long)-1, memory_order_seq_cst);
		} else {
			init_ohos_malloc_hook();
		}
	} else if (shared_library_handle != (void*)-1) {
		bool flag = __set_hook_flag(false);
		on_start_func_t start_func = (on_start_func_t)(function_of_shared_lib[ON_START_FUNCTION]);
		if (start_func && !start_func()) {
			// __musl_log(__MUSL_LOG_ERROR, "%s: failed to enable malloc\n", getprogname());
		}
		__set_hook_flag(flag);
		volatile const struct MallocDispatchType* so_dispatch_value = (volatile const struct MallocDispatchType* )atomic_load_explicit(&__musl_libc_globals.so_dispatch_table, memory_order_acquire);
		atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile long long)so_dispatch_value, memory_order_seq_cst);
	}
}

static void __uninstal_malloc_hook()
{
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)false, memory_order_seq_cst);

	bool flag = __set_hook_flag(false);
	__set_default_malloc();
	on_end_func_t end_func = (on_end_func_t)(function_of_shared_lib[ON_END_FUNCTION]);
	if (end_func) {
		end_func();
	}
	__set_hook_flag(flag);
}

static void __install_malloc_hook_signal_handler()
{
	struct sigaction actionInstallHook = {};
	actionInstallHook.sa_handler = __install_malloc_hook;
	sigemptyset(&actionInstallHook.sa_mask);
	sigaddset(&actionInstallHook.sa_mask, SIGUNHOOK);
	sigaction(SIGHOOK, &actionInstallHook, NULL);

	struct sigaction actionDef = {};
	actionDef.sa_handler = __uninstal_malloc_hook;
	sigemptyset(&actionDef.sa_mask);
	sigaddset(&actionDef.sa_mask, SIGHOOK);
	sigaction(SIGUNHOOK, &actionDef, NULL);
}

static void __initialize_malloc()
{
	__install_malloc_hook_signal_handler();
}


__attribute__((constructor(1))) static void __musl_initialize()
{
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)false, memory_order_seq_cst);
	__set_default_malloc();
	//__init_musl_log();
	parse_hook_variable(&__hook_mode, __hook_process_path, sizeof(__hook_process_path));
	if (__hook_mode == STATRUP_HOOK_MODE) {
		readlink("/proc/self/exe", __progname, sizeof(__progname) - 1);
		const char *pos = strrchr(__progname, '/');
		const char* filename;
		if (pos) {
			filename = pos + 1;
		} else {
			filename = __progname;
		}
		if (strcmp(filename, __hook_process_path) == 0) {
			atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)true, memory_order_seq_cst);
			init_ohos_malloc_hook();
		} else {
			__hook_mode = STEP_HOOK_MODE;
		}
	}
	__initialize_malloc();
}
#endif
