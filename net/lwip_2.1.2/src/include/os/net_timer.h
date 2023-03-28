#ifndef __NET_TIMER_H__
#define __NET_TIMER_H__

#include "cmsis_os.h"
#include "linux_list.h"
#include "hal_timer.h"
#include <time.h>

#if !defined(NUTTX_BUILD)
#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define FSEC_PER_SEC	1000000000000000LL
#endif

#include "jiffies.h"
#if 0
struct timespec {
	unsigned long	tv_sec; 		/* seconds */
	unsigned long	tv_nsec;		/* nanoseconds */
};
#endif
struct timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	unsigned long expires;
	osTimerId timer_id;
	osTimerDef_t timer_def;
	unsigned char pending;
	unsigned long data;
	void (*function)(unsigned long);
#ifdef CMSIS_OS_NUTTX
	osTimerAttr_t time_attr;
#endif
#ifdef CMSIS_OS_RTX
	uint32_t timer_data[5];
#endif
};

extern void ktime_get_ts(struct timespec *ts);
#define do_posix_clock_monotonic_gettime(ts) ktime_get_ts(ts)

static inline void msleep(int ms)
{
	osDelay(ms);
}

static inline void net_mdelay(int ms)
{
	osDelay(ms);
}

static inline int timer_pending(const struct timer_list *timer)
{
	return timer->pending == N_TRUE;
}

extern void setup_timer(struct timer_list *timer,
			void (*function)(unsigned long),
			unsigned long data);

extern int mod_timer(struct timer_list *timer, uint64_t expires);
extern int add_timer(struct timer_list *timer);
extern int cancel_timer(struct timer_list *timer);
extern int del_timer(struct timer_list *timer);
extern int del_timer_sync(struct timer_list *timer);

#endif

