#define _GNU_SOURCE
#include "usr_lib_define.h"
#include <string.h>
#include "pthread_impl.h"
#include "syscall.h"

_LIBC_TEXT_SECTION int pthread_cancel(pthread_t t)
{
	return ENOSYS;
}
