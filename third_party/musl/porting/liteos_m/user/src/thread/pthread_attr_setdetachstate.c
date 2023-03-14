#include "usr_lib_define.h"
#include "pthread_impl.h"

_LIBC_TEXT_SECTION int pthread_attr_setdetachstate(pthread_attr_t *a, int state)
{
	if (state > 1U) return EINVAL;
	a->_a_detach = state;
	return 0;
}
