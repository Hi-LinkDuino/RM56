#ifndef __JIFFIES_H__
#define __JIFFIES_H__
#include "net_timer.h"
#define NET_HZ    1000 //OS_TICK //1600//CONFIG_SYSTICK_HZ
typedef unsigned int       U32;

#define jiffies GET_CURRENT_MS()


/*
 *	These inlines deal with timer wrapping correctly. You are
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b) ((long)((b) - (a)) < 0)
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	((long)((a) - (b)) >= 0)
#define time_before_eq(a,b)	time_after_eq(b,a)

/* time_is_before_jiffies(a) return true if a is before jiffies */
#define time_is_before_jiffies(a) time_after(jiffies, a)

/* time_is_after_jiffies(a) return true if a is after jiffies */
#define time_is_after_jiffies(a) time_before(jiffies, a)

/* time_is_before_eq_jiffies(a) return true if a is before or equal to jiffies*/
#define time_is_before_eq_jiffies(a) time_after_eq(jiffies, a)

/* time_is_after_eq_jiffies(a) return true if a is after or equal to jiffies*/
#define time_is_after_eq_jiffies(a) time_before_eq(jiffies, a)

/**
 * round_jiffies - function to round jiffies to a full second
 * @j: the time in (absolute) jiffies that should be rounded
 *
 * round_jiffies() rounds an absolute time in the future (in jiffies)
 * up or down to (approximately) full seconds. This is useful for timers
 * for which the exact time they fire does not matter too much, as long as
 * they fire approximately every X seconds.
 *
 * By rounding these timers to whole seconds, all such timers will fire
 * at the same time, rather than at various times spread out. The goal
 * of this is to have the CPU wake up less, which saves power.
 *
 * The return value is the rounded version of the @j parameter.
 */
static __INLINE unsigned long round_jiffies(unsigned long j)
{
    return j;
}

static __INLINE unsigned long round_jiffies_up(unsigned long j)
{
    return j;
}

static __INLINE unsigned long round_jiffies_relative(unsigned long j)
{
    return j;
}

static inline unsigned int jiffies_to_msecs(const unsigned long j)
{
    return (MSEC_PER_SEC / NET_HZ) * j;
}

extern unsigned long msecs_to_jiffies(const unsigned long m);
extern unsigned long usecs_to_jiffies(const unsigned long u);
#endif

