#include <sched.h>
#include <errno.h>
#include "syscall.h"

int sched_setscheduler(pid_t pid, int sched, const struct sched_param *param)
{
	int r;
	if (!param) {
		r = -EINVAL;
		goto exit;
	}
	r = __syscall(SYS_sched_setscheduler, pid , sched , param);
exit:
	return __syscall_ret(r);
}