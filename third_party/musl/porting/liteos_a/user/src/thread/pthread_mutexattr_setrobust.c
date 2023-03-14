#include "pthread_impl.h"
#include "syscall.h"

int pthread_mutexattr_setrobust(pthread_mutexattr_t *a, int robust)
{
	if (robust > 1U) return EINVAL;
	if (robust) {
		a->__attr |= 4;
		return 0;
	}
	a->__attr &= ~4;
	return 0;
}
