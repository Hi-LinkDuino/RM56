/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_QUEUE_TEST_H
#define PLATFORM_QUEUE_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

enum PlatformQueueTestCmd {
    PLAT_QUEUE_TEST_ADD_AND_WAIT = 0,
    PLAT_QUEUE_TEST_SUSPEND_AND_RESUME = 1,
    PLAT_QUEUE_TEST_RELIABILITY = 2,
    PLAT_QUEUE_TEST_CMD_MAX,
};

int PlatformQueueTestExecute(int cmd);
void PlatformQueueTestExecuteAll(void);

#ifdef __cplusplus
}
#endif
#endif /* PLATFORM_QUEUE_TEST_H */
