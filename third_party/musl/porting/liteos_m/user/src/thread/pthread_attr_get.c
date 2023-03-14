#include "usr_lib_define.h"
#include "pthread_impl.h"

_LIBC_TEXT_SECTION int pthread_attr_getdetachstate(const pthread_attr_t *a, int *state)
{
	*state = a->_a_detach;
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getguardsize(const pthread_attr_t *restrict a, size_t *restrict size)
{
	return ENOSYS;
}

_LIBC_TEXT_SECTION int pthread_attr_getinheritsched(const pthread_attr_t *restrict a, int *restrict inherit)
{
	*inherit = a->_a_sched;
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getschedparam(const pthread_attr_t *restrict a, struct sched_param *restrict param)
{
	param->sched_priority = a->_a_prio;
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getschedpolicy(const pthread_attr_t *restrict a, int *restrict policy)
{
	*policy = a->_a_policy;
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getscope(const pthread_attr_t *restrict a, int *restrict scope)
{
	*scope = PTHREAD_SCOPE_SYSTEM;
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getstack(const pthread_attr_t *restrict a, void **restrict addr, size_t *restrict size)
{
	if (!a->_a_stackaddr)
		return EINVAL;
	*size = a->_a_stacksize;
	*addr = (void *)(a->_a_stackaddr - *size);
	return 0;
}

_LIBC_TEXT_SECTION int pthread_attr_getstacksize(const pthread_attr_t *restrict a, size_t *restrict size)
{
	*size = a->_a_stacksize;
	return 0;
}
