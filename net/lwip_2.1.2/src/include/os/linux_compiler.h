#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H

#define __user
#define __kernel
#define __safe
#define __force
#define __nocast
#define __iomem
#define __chk_user_ptr(x) (void)0
#define __chk_io_ptr(x) (void)0
#define __builtin_warning(x, y...) (1)
#define __must_hold(x)
#define __acquires(x)
#define __releases(x)
#define __acquire(x) (void)0
#define __release(x) (void)0
#define __cond_lock(x,c) (c)
#define __percpu
#define __rcu

#ifndef __attribute_const__
#define __attribute_const__	const
#endif

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#endif


