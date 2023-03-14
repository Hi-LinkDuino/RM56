/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_EVENT_TEST_H
#define PLATFORM_EVENT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

enum PlatformEventTestCmd {
    PLAT_EVENT_TEST_INIT_AND_UNINIT = 0,
    PLAT_EVENT_TEST_POST_AND_WAIT = 1,
    PLAT_EVENT_TEST_LISTEN_AND_UNLISTEN = 2,
    PLAT_EVENT_TEST_RELIABILITY = 3,
    PLAT_EVENT_TEST_CMD_MAX,
};

int PlatformEventTestExecute(int cmd);
void PlatformEventTestExecuteAll(void);

#ifdef __cplusplus
}
#endif
#endif /* PLATFORM_EVENT_TEST_H */
