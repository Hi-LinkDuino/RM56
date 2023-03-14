/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_RTC_BASE_H
#define HDF_RTC_BASE_H

#include "rtc_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define IS_LEAP_YEAR(year)     (((((year) % 4) == 0) && (((year) % 100) != 0)) || (((year) % 400) == 0))
#define RTC_YEAR_DAYS(year)    (IS_LEAP_YEAR(year) ? 366 : 365)

#define RTC_DAY_SECONDS        86400ULL /* 24 hours * 60 minutes * 60 seconds */
#define RTC_DAY_HOURS          24       /* 24 hours */
#define RTC_HOUR_SECONDS       3600UL   /* 60 minutes * 60 seconds */
#define RTC_MAX_MONTH          12
#define RTC_MAX_WEEKDAY        7
#define RTC_GREAT_MONTH_DAY    31
#define RTC_SMALL_MONTH_DAY    30
#define RTC_TWO_MONTH_DAY      28
#define RTC_MAX_HOUR           24
#define RTC_MAX_MINUTE         60UL
#define RTC_MAX_SECOND         60UL
#define RTC_MAX_MS             1000
#define RTC_UNIT_DIFF          1
#define RTC_ODD_MONTH_MASK     0x1
#define RTC_TIME_UNIT          60
#define RTC_FALSE              0
#define RTC_TRUE               1

#define RTC_BEGIN_YEAR         1970 /* UTC started at 00:00:00 January 1, 1970 */
#define RTC_BEGIN_WEEKDAY      0x04 /* Thursday January 1, 1970 */

#define IS_INVALID_YEAR(year)    ((year) < RTC_BEGIN_YEAR)
#define IS_INVALID_MONTH(m)      (((m) < RTC_JANUARY) || ((m) > RTC_MAX_MONTH))
#define IS_INVALID_WEEKDAY(wd)   (((wd) < 1) || ((wd) > RTC_MAX_WEEKDAY))
#define IS_INVALID_HOUR(hour)    ((hour) >= RTC_MAX_HOUR)
#define IS_INVALID_MIN(min)      ((min) >= RTC_MAX_MINUTE)
#define IS_INVALID_SECOND(s)     ((s) >= RTC_MAX_SECOND)
#define IS_INVALID_MS(ms)        ((ms) >= RTC_MAX_MS)

enum RtcMonth {
    RTC_JANUARY = 1,
    RTC_FEBRUARY,
    RTC_MARCH,
    RTC_APRIL,
    RTC_MAY,
    RTC_JUNE,
    RTC_JULY,
    RTC_AUGUST,
    RTC_SEPTEMBER,
    RTC_OCTOBER,
    RTC_NOVEMBER,
    RTC_DECEMBER,
};

uint8_t RtcGetMonthDays(const uint8_t isLeapYear, const uint8_t month);
uint8_t RtcIsInvalidDay(const uint8_t day, const uint8_t month, const uint16_t year);
uint8_t RtcIsInvalid(const struct RtcTime *time);
uint8_t RtcGetWeekDay(const struct RtcTime *time);
#ifndef __KERNEL__
uint64_t RtcTimeToTimestamp(const struct RtcTime *time);
void TimestampToRtcTime(struct RtcTime *time, const uint64_t seconds);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDF_RTC_BASE_H */
