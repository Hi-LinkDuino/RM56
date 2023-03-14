#define _GNU_SOURCE
#include "usr_lib_define.h"
#include "pthread_impl.h"

_LIBC_TEXT_SECTION static int __pthread_timedjoin_np(pthread_t t, void **res, const struct timespec *at)
{
	int state, cs, r = 0;
	unsigned int tid;
	pthread_t self = __pthread_self();
	if (t == self) {
		r = EINVAL;
		goto out;
	}

	switch (t->detach_state) {
		case DT_JOINABLE: {
			r = __syscall(SYS_pthread_join, t->tid);
			break;
		}
		case DT_EXITING:
			break;
		case DT_DETACHED:
		default:
			r = EINVAL;
			break;
	}

out:
	if (r == ESRCH || r == EINVAL) return r;
	if (res) *res = t->result;
	tid = t->tid;
	t->tid = 0;
	return __syscall(SYS_pthread_deatch, tid);
}

_LIBC_TEXT_SECTION int __pthread_join(pthread_t t, void **res)
{
	return __pthread_timedjoin_np(t, res, 0);
}

weak_alias(__pthread_join, pthread_join);
