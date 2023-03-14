/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_MANAGER_TEST_H
#define PLATFORM_MANAGER_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

enum PlatformManagerTestCmd {
    PLAT_MANAGER_TEST_ADD_DEVICE = 0,
    PLAT_MANAGER_TEST_GET_DEVICE  = 1,
    PLAT_MANAGER_TEST_RELIABILITY = 2,
    PLAT_MANAGER_TEST_CMD_MAX,
};

int PlatformManagerTestExecute(int cmd);
void PlatformManagerTestExecuteAll(void);

#ifdef __cplusplus
}
#endif
#endif /* PLATFORM_MANAGER_TEST_H */
