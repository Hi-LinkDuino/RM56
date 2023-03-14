#include <sys/resource.h>
#include <errno.h>
#include "syscall.h"

#define FIX(x) do{ if ((x)>=SYSCALL_RLIM_INFINITY) (x)=RLIM_INFINITY; }while(0)

int getrlimit(int resource, struct rlimit *rlim)
{
	unsigned long long k_rlim[2];

	if (syscall(SYS_getrlimit, resource, k_rlim) < 0)
		return -1;

	rlim->rlim_cur = k_rlim[0];
	rlim->rlim_max = k_rlim[1];

	return 0;
}

weak_alias(getrlimit, getrlimit64);
