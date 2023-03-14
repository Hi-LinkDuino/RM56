#include "usr_lib_define.h"
#include "pthread_impl.h"

_LIBC_TEXT_SECTION int pthread_attr_init(pthread_attr_t *a)
{
	*a = (pthread_attr_t){0};
	a->_a_stacksize = 0x2000;
	a->_a_policy = SCHED_RR;
	a->_a_prio = 25;
	return 0;
}
