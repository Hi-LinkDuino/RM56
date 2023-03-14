#include <signal.h>
#include "syscall.h"

int sigsuspend(const sigset_t *mask)
{
	int ret,retval;
	sigset_t oldset;

	retval = sigprocmask(SIG_BLOCK, 0, &oldset);
	if (retval != 0){
		return retval;
	}

	ret = syscall_cp(SYS_rt_sigsuspend, mask, _NSIG/8);

	if (ret == -1){
		retval = sigprocmask(SIG_SETMASK, &oldset, 0);
		if (retval != 0){
			return retval;
		}
	}

	return ret;
}