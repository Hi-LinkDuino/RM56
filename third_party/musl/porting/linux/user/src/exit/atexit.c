#include <stdlib.h>
#include <stdint.h>
#include "libc.h"
#include "lock.h"

/* Ensure that at least 32 atexit handlers can be registered without malloc */
#define COUNT 32

static struct fl
{
	struct fl *next;
	void (*f[COUNT])(void *);
	void *a[COUNT];
	void *dso[COUNT];
} builtin, *head;

static int slot;
static volatile int lock[1];

void __funcs_on_exit()
{
	void (*func)(void *), *arg;
	LOCK(lock);
	for (; head; head=head->next, slot=COUNT) while(slot-->0) {
		if (head->dso[slot] != NULL) {
			func = head->f[slot];
			arg = head->a[slot];
			UNLOCK(lock);
			func(arg);
			LOCK(lock);
		}
	}
	UNLOCK(lock);
}

void __cxa_finalize(void *dso)
{
	void (*func)(void *), *arg;
	LOCK(lock);
	for (; head; head=head->next, slot=COUNT) while(slot-->0) {
		if (dso == head->dso[slot]) {
			func = head->f[slot];
			arg = head->a[slot];
			UNLOCK(lock);
			func(arg);
			LOCK(lock);

			head->dso[slot] = NULL;
		}
	}
	UNLOCK(lock);
}

int __cxa_atexit(void (*func)(void *), void *arg, void *dso)
{
	LOCK(lock);

	/* Defer initialization of head so it can be in BSS */
	if (!head) head = &builtin;

	/* If the current function list is full, add a new one */
	if (slot==COUNT) {
		struct fl *new_fl = calloc(sizeof(struct fl), 1);
		if (!new_fl) {
			UNLOCK(lock);
			return -1;
		}
		new_fl->next = head;
		head = new_fl;
		slot = 0;
	}

	/* Append function to the list. */
	head->f[slot] = func;
	head->a[slot] = arg;
	head->dso[slot] = dso;

	slot++;

	UNLOCK(lock);
	return 0;
}

static void call(void *p)
{
	((void (*)(void))(uintptr_t)p)();
}

int atexit(void (*func)(void))
{
	return __cxa_atexit(call, (void *)(uintptr_t)func, 0);
}