#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <debug.h>
#include <signal.h>
#include <atomic.h>
#include <pthread.h>
#include "libc.h"

extern bool g_enable_check;
extern void mem_check_deinit(void);
extern void clean_recycle_list(bool clean_all);
pthread_mutex_t __exit_mutex = PTHREAD_MUTEX_INITIALIZER; 

static void dummy()
{
}

/* atexit.c and __stdio_exit.c override these. the latter is linked
 * as a consequence of linking either __toread.c or __towrite.c. */
weak_alias(dummy, __funcs_on_exit);
weak_alias(dummy, __stdio_exit);
weak_alias(dummy, _fini);

extern weak hidden void (*const __fini_array_start)(void), (*const __fini_array_end)(void);

static void libc_exit_fini(void)
{
	uintptr_t a = (uintptr_t)&__fini_array_end;
	for (; a>(uintptr_t)&__fini_array_start; a-=sizeof(void(*)()))
		(*(void (**)())(a-sizeof(void(*)())))();
	_fini();
}

weak_alias(libc_exit_fini, __libc_exit_fini);

_Noreturn void exit(int code)
{
	sigset_t set;

	pthread_mutex_lock(&__exit_mutex);

	__block_app_sigs(&set);
	if (g_enable_check) {
		check_leak();
		check_heap_integrity();
		mem_check_deinit();
		clean_recycle_list(true);
	}
	__funcs_on_exit();
	__libc_exit_fini();
	__stdio_exit();
	_Exit(code);
}
