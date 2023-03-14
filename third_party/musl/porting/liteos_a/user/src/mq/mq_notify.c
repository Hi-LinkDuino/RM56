#include <mqueue.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "syscall.h"

int mq_notify(mqd_t mqd, const struct sigevent *sev)
{
	if (!sev || sev->sigev_notify != SIGEV_THREAD)
		return syscall(SYS_mq_notify, mqd, sev);

	errno = ENOTSUP;
	return -1;
}
