#include "usr_lib_define.h"
#include "pthread_impl.h"
#include <threads.h>

_LIBC_TEXT_SECTION pthread_t __pthread_self()
{
	uintptr_t p;
	p = __syscall(SYS_get_thread_area);
	return (pthread_t)(p - sizeof(struct pthread));
}

weak_alias(__pthread_self, pthread_self);
