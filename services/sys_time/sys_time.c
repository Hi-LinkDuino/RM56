/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <string.h>
#include "plat_addr_map.h"
#include "hal_timer.h"
#define IGNORE_HAL_TIMER_RAW_API_CHECK
#include "hal_timer_raw.h"
#include "reg_timer.h"
#include "hal_location.h"
#include "hal_cmu.h"
#include "cmsis_nvic.h"
#include "sys_time.h"

static u32_t base_ntp_sec = 0;
static u32_t base_ntp_ms = 0;
static int ntp_delta = DIFF_SEC_GMT8;
static u32_t base_sys_ticks = 0;
static bool  ntp_updated = false;

/* ntp time with GMT/NTC need different delta */
void hal_sys_time_set_ntp_delta(int delta)
{
	ntp_delta = delta;
}

void set_ntp_update(void)
{
	ntp_updated = false;
}

int get_ntp_ready(void)
{
	return (ntp_updated == false) ? 0 : 1;
}

void hal_sys_time_init(u32_t sec, u32_t us)
{
	base_ntp_sec = sec;
	base_ntp_ms = us / 1000;
	base_sys_ticks = hal_sys_timer_get();
	ntp_updated = true;
}

/* return UTC 0 (1970-1-1) or NTP time */
time_t hal_sys_time_get(void)
{
	return (time_t)
		((base_ntp_sec == 0) ? 0
		: base_ntp_sec + ((TICKS_TO_MS(hal_sys_timer_get() - base_sys_ticks) + base_ntp_ms) / 1000) + ntp_delta);
}

char *hal_sys_get_cur_time(void)
{
	time_t rawtime;
	struct tm *timeinfo;
	rawtime = hal_sys_time_get();
	if (rawtime == 0)
		return NULL;
	timeinfo = localtime(&rawtime);
	return asctime(timeinfo);
}

time_t hal_sys_get_cur_rawtime(void)
{
	return hal_sys_time_get();
}

char *hal_sys_format_time(time_t t)
{
	char *str = NULL;

	if (t == 0)
		return NULL;
	str = ctime(&t);
	if (str != NULL)
		str[strlen(str)-1] = 0;

	return str;
}
#ifdef SYS_TIME_IF
/* Get the current time of day and timezone information,
   putting it into *TV and *TZ.  If TZ is NULL, *TZ is not filled.
   Returns 0 on success, -1 on errors.  */
int _gettimeofday(struct timeval *__restrict __p, void *__restrict __tz)
{
	if (__p == NULL)
		return -1;

	__p->tv_sec = (long int) hal_sys_time_get();
#if 0
	/* Better to call hal_sys_time_init only in sntp hook, in case that
	 * gettimeofday from userspace during sntp breaks the procedure.
	 */
	if (__p->tv_sec == 0) {
		hal_sys_time_init(10000000, 0);
		__p->tv_sec = (long int) hal_sys_time_get();
	}
#endif
	__p->tv_usec = (long int)(((TICKS_TO_MS(hal_sys_timer_get() - base_sys_ticks) + base_ntp_ms) % 1000) * 1000);

	return 0;
}

#ifndef ALIOS_THINGS_RELEASE
int gettimeofday(struct timeval *__restrict __p, void *__restrict __tz)
{
	return _gettimeofday(__p, __tz);
}
#endif

int gettimeofms(void)
{
	return TICKS_TO_MS(hal_sys_timer_get());
}

int gettimeready(void)
{
	return (hal_sys_time_get() == 0) ? 0 : 1;
}
#endif

