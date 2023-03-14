#include <errno.h>
#include "stdarg.h"
#include "stdbool.h"
#include "pthread_impl.h"

struct user_param {
	unsigned long user_area;
	unsigned long user_sp;
	unsigned long map_base;
	unsigned int map_size;
};

int __thread_clone(int (*func)(void *), int flags, struct pthread *thread, unsigned char *sp)
{
	int ret;
	bool join_flag = false;
	struct user_param param;

	if (thread->detach_state == DT_JOINABLE) {
		join_flag = true;
	}

	param.user_area = TP_ADJ(thread);
	param.user_sp = sp;
	param.map_base = thread->map_base;
	param.map_size = thread->map_size;
	ret = __syscall(SYS_creat_user_thread , func, &param, join_flag);
	if (ret < 0) {
		return ret;
	}

	thread->tid = (unsigned long)ret;
	return 0;
}
