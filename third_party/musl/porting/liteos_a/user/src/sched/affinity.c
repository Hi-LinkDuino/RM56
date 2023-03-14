#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include "pthread_impl.h"
#include "syscall.h"

int sched_setaffinity(pid_t tid, size_t size, const cpu_set_t *set)
{
    if (size != sizeof(cpu_set_t)) {
        errno = EINVAL;
        return -1;
    }
    return syscall(SYS_sched_setaffinity, tid, (unsigned short)(set->__bits[0]), MUSL_TYPE_PROCESS);
}

int pthread_setaffinity_np(pthread_t td, size_t size, const cpu_set_t *set)
{
    if (size != sizeof(cpu_set_t)) {
        return EINVAL;
    }
    return -__syscall(SYS_sched_setaffinity, td->tid, (unsigned short)(set->__bits[0]), MUSL_TYPE_THREAD);
}

static int do_getaffinity(pid_t tid, size_t size, cpu_set_t *set, int flag)
{
    unsigned int cpuset;
    if (size != sizeof(cpu_set_t)) {
        return -EINVAL;
    }
    int ret = __syscall(SYS_sched_getaffinity, tid, &cpuset, flag);
    if (ret < 0) {
        return ret;
    }
    set->__bits[0] = (long)cpuset;
    return 0;
}

int sched_getaffinity(pid_t tid, size_t size, cpu_set_t *set)
{
    return __syscall_ret(do_getaffinity(tid, size, set, MUSL_TYPE_PROCESS));
}

int pthread_getaffinity_np(pthread_t td, size_t size, cpu_set_t *set)
{
    return -do_getaffinity(td->tid, size, set, MUSL_TYPE_THREAD);
}
