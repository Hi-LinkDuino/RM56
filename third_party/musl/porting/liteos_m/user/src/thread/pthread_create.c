#define _GNU_SOURCE
#include "usr_lib_define.h"
#include "pthread_impl.h"
#include "libc.h"
#include "lock.h"
#include "string.h"
#include "stdbool.h"

_LIBC_TEXT_SECTION _Noreturn void __pthread_exit(void *result)
{
	pthread_t self = pthread_self();

	self->canceldisable = 1;
	self->cancelasync = 0;
	self->result = result;

	while (self->cancelbuf) {
		void (*f)(void *) = self->cancelbuf->__f;
		void *x = self->cancelbuf->__x;
		self->cancelbuf = self->cancelbuf->__next;
		f(x);
	}

	/* This atomic potentially competes with a concurrent pthread_detach
	 * call; the loser is responsible for freeing thread resources. */
	int state = a_cas(&self->detach_state, DT_JOINABLE, DT_EXITING);

	/* After the kernel thread exits, its tid may be reused. Clear it
	 * to prevent inadvertent use and inform functions that would use
	 * it that it's no longer available. */
	if (self->detach_state == DT_DETACHED) {
		/* Detached threads must block even implementation-internal
		 * signals, since they will not have a stack in their last
		 * moments of existence. */
		self->tid = 0;
	}

	UNLOCK(self->killlock);

	for (;;) __syscall(SYS_exit, 0);
}

struct start_args {
	void *(*start_func)(void *);
	void *start_arg;
};

_LIBC_TEXT_SECTION static int start(void *p)
{
	struct start_args *args = (struct start_args *)p;
	__pthread_exit(args->start_func(args->start_arg));
	return 0;
}

_LIBC_TEXT_SECTION static int start_c11(void *p)
{
	struct start_args *args = (struct start_args *)p;
	int (*start)(void*) = (int(*)(void*)) args->start_func;
	__pthread_exit((void *)(uintptr_t)start(args->start_arg));
	return 0;
}

_LIBC_TEXT_SECTION static int __pthread_init_and_check_attr(const pthread_attr_t *restrict attrp, pthread_attr_t *attr)
{
	int policy = 0;
	struct sched_param param = { 0 };
	int c11 = (attrp == __ATTRP_C11_THREAD);
	int ret;

	if (attrp && !c11) memcpy(attr, attrp, sizeof(pthread_attr_t));

	if (!attrp || c11) {
		pthread_attr_init(attr);
	}

	if (!attr->_a_sched) {
		ret = pthread_getschedparam(pthread_self(), &policy, &param);
		if (ret) return ret;
		attr->_a_policy = policy;
		attr->_a_prio = param.sched_priority;
	}

	if (attr->_a_policy != SCHED_RR && attr->_a_policy != SCHED_FIFO) {
		return EINVAL;
	}

	if (attr->_a_prio < 0 || attr->_a_prio > PTHREAD_PRIORITY_LOWEST) {
		return EINVAL;
	}

	return 0;
}

_LIBC_TEXT_SECTION static int __thread_clone(int (*func)(void *), int flags, struct pthread *thread, unsigned char *sp)
{
	int ret;
	bool join_flag = false;
	unsigned long user_area, user_sp;

	if (thread->detach_state == DT_JOINABLE) {
		join_flag = true;
	}

	user_area = (unsigned long)TP_ADJ(thread);
	user_sp = (unsigned long)sp;
	ret = __syscall(SYS_creat_user_thread, func, user_area, user_sp, join_flag);
	if (ret < 0) {
		return ret;
	}

	thread->tid = (unsigned long)ret;
	return 0;
}

_LIBC_TEXT_SECTION int __pthread_create(pthread_t *restrict res, const pthread_attr_t *restrict attrp, void *(*entry)(void *), void *restrict arg)
{
	int ret, c11 = (attrp == __ATTRP_C11_THREAD);
	size_t size;
	struct pthread *self, *new;
	unsigned char *stack = 0, *stack_limit, *map_base = 0;
	pthread_attr_t attr = { 0 };

	if (!entry) return EINVAL;
	self = pthread_self();

	ret = __pthread_init_and_check_attr(attrp, &attr);
	if (ret) {
		return ret;
	}

	size = attr._a_stacksize;
	if (attr._a_stackaddr) {
		stack = (void *)(attr._a_stackaddr & -16);
		stack_limit = (void *)(attr._a_stackaddr - size);
	} else {
		map_base = malloc(size);
		if (map_base == NULL) goto fail;
		stack_limit = map_base;
		stack = map_base + size;
	}
	new = (struct pthread *)(((uintptr_t)stack - sizeof(struct pthread)) & -16);
	new->map_base = stack_limit;
	new->map_size = size;
	new->stack = new;
	new->stack_size = (uintptr_t)new->stack - (uintptr_t)stack_limit;
	new->guard_size = 0;
	new->self = new;
	new->tsd = (void *)NULL;
	new->locale = NULL;
	if (attr._a_detach) {
		new->detach_state = DT_DETACHED;
	} else {
		new->detach_state = DT_JOINABLE;
	}
	new->robust_list.head = &new->robust_list.head;
	new->CANARY = self->CANARY;
	new->sysinfo = self->sysinfo;

	/* Setup argument structure for the new thread on its stack.
	 * It's safe to access from the caller only until the thread
	 * list is unlocked. */
	stack -= (uintptr_t)new->stack % sizeof(uintptr_t);
	stack -= sizeof(struct start_args);
	struct start_args *args = (void *)stack;
	args->start_func = entry;
	args->start_arg = arg;

	ret = __thread_clone((c11 ? start_c11 : start), 0, new, stack);

	/* All clone failures translate to EAGAIN. If explicit scheduling
	 * was requested, attempt it before unlocking the thread list so
	 * that the failed thread is never exposed and so that we can
	 * clean up all transient resource usage before returning. */
	if (ret < 0) {
		ret = -EAGAIN;
	} else {
		*res = new;
		ret = __syscall(SYS_sched_setscheduler,
			new->tid, attr._a_policy, attr._a_prio);
	}

	if (ret < 0) {
		if (map_base) free(map_base);
		return -ret;
	}

	return 0;
fail:
	return EAGAIN;
}

weak_alias(__pthread_exit, pthread_exit);
weak_alias(__pthread_create, pthread_create);
