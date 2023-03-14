#include <sched.h>
#include <errno.h>
#include "syscall.h"
#include <string.h>

int sched_getparam(pid_t pid, struct sched_param *param)
{
	int r;
	if (!param) {
		r = -EINVAL;
		goto exit;
	}
	memset(param, 0, sizeof(struct sched_param));
	r = __syscall(SYS_sched_getparam, pid , param);
	if (r >= 0) {
		r = 0;
	}
exit:
	return __syscall_ret(r);
}
