#ifndef __RCUPDATE_H__
#define __RCUPDATE_H__

#include "net_defs.h"

#define RCU_INITIALIZER(v) (v)

#define rcu_dereference(p) (p)

#define RCU_INIT_POINTER(p, v) \
	do { \
		p = RCU_INITIALIZER(v); \
	} while (0)

#define rcu_assign_pointer(p, v)  (p = v)
#define rcu_dereference_raw(p)    (p)
#define rcu_dereference_raw_notrace(p) (p)
#define rcu_dereference_bh(p)      (p)
#define rcu_dereference_protected(p, c) (p)
#define lockdep_is_held(lock) (lock)
#define rcu_access_pointer(p)  (p)
#define rcu_dereference_check(p, c) (p)

static __INLINE void call_rcu(struct rcu_head *head, void (*func)(struct rcu_head *rcu))
{
	if (func)
		func(head);
}

static inline void kfree_call_rcu(struct rcu_head *head,
				  void (*func)(struct rcu_head *rcu))
{
	call_rcu(head, func);
}

static __INLINE
void __kfree_rcu(struct rcu_head *head, unsigned long offset)
{
	typedef void (*rcu_callback)(struct rcu_head *);

	kfree_call_rcu(head, (rcu_callback)offset);
}


/**
 * kfree_rcu() - kfree an object after a grace period.
 * @ptr:	pointer to kfree
 * @rcu_head:	the name of the struct rcu_head within the type of @ptr.
 *
 * Many rcu callbacks functions just call kfree() on the base structure.
 * These functions are trivial, but their size adds up, and furthermore
 * when they are used in a kernel module, that module must invoke the
 * high-latency rcu_barrier() function at module-unload time.
 *
 * The kfree_rcu() function handles this issue.  Rather than encoding a
 * function address in the embedded rcu_head structure, kfree_rcu() instead
 * encodes the offset of the rcu_head structure within the base structure.
 * Because the functions are not allowed in the low-order 4096 bytes of
 * kernel virtual memory, offsets up to 4095 bytes can be accommodated.
 * If the offset is larger than 4095 bytes, a compile-time error will
 * be generated in __kfree_rcu().  If this error is triggered, you can
 * either fall back to use of call_rcu() or rearrange the structure to
 * position the rcu_head structure into the first 4096 bytes.
 *
 * Note that the allowable offset might decrease in the future, for example,
 * to allow something like kmem_cache_free_rcu().
 */
#define kfree_rcu(ptr, rcu_head)					\
	__kfree_rcu(&((ptr)->rcu_head), offsetof(typeof(*(ptr)), rcu_head))

#endif

