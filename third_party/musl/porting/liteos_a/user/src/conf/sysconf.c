#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include "syscall.h"
#include "libc.h"

#define JT(x) (-256|(x))
#define VER JT(1)
#define JT_ARG_MAX JT(2)
#define JT_MQ_PRIO_MAX JT(3)
#define JT_PAGE_SIZE JT(4)
#define JT_SEM_VALUE_MAX JT(5)
#define JT_NPROCESSORS_CONF JT(6)
#define JT_NPROCESSORS_ONLN JT(7)
#define JT_PHYS_PAGES JT(8)
#define JT_AVPHYS_PAGES JT(9)
#define JT_ZERO JT(10)
#define JT_DELAYTIMER_MAX JT(11)

#define RLIM(x) (-32768|(RLIMIT_ ## x))

long sysconf(int name)
{
	return syscall(SYS_sysconf, name);
}
