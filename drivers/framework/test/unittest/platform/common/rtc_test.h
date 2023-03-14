/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef RTC_TEST_H
#define RTC_TEST_H

#include "rtc_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t RtcTestExecute(int cmd);

struct RtcTestConfig {
    uint32_t time;
    uint32_t maxYear;
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t weekday;
    uint32_t millisecond;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t frequency;
    uint32_t userValue;
    uint32_t userMaxIndex;
    uint32_t waitTimeSecond;
    uint32_t writeWaitMillisecond;
};

struct RtcTester {
    struct RtcTestConfig config;
    struct RtcTime time;
    DevHandle handle;
};

enum RtcTestCmd {
    RTC_TEST_CMD_WR_TIME = 0,
    RTC_TEST_CMD_WR_MAX_TIME,
    RTC_TEST_CMD_WR_MIN_TIME,
    RTC_TEST_CMD_WR_ALARM_TIME,
    RTC_TEST_CMD_WR_ALARM_MAX_TIME,
    RTC_TEST_CMD_WR_ALARM_MIN_TIME,
    RTC_TEST_CMD_RTC_ALARM_ENABLE,
    RTC_TEST_CMD_RTC_ALARM_IRQ,
    RTC_TEST_CMD_RTC_REGISTER_CALLBACK,
    RTC_TEST_CMD_RTC_REGISTER_CALLBACK_NULL,
    RTC_TEST_CMD_RTC_WR_FREQ,
    RTC_TEST_CMD_RTC_WR_MAX_FREQ,
    RTC_TEST_CMD_RTC_WR_MIN_FREQ,
    RTC_TEST_CMD_RTC_WR_USER_REG,
    RTC_TEST_CMD_RTC_WR_USER_REG_MAX_INDEX,
    RTC_TEST_CMD_RTC_WR_RELIABILITY,
    RTC_TEST_CMD_RTC_FUNCTION_TEST,
    RTC_TEST_CMD_MAX,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTC_TEST_H */
