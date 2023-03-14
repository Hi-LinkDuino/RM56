/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_DEVICE_TEST_H
#define PLATFORM_DEVICE_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

enum PlatformDeviceTestCmd {
    PLAT_DEVICE_TEST_SET_NAME = 0,
    PLAT_DEVICE_TEST_GET_DEVICE = 1,
    PLAT_DEVICE_TEST_WAIT_EVENT = 2,
    PLAT_DEVICE_TEST_ADD_DEVICE  = 3,
    PLAT_DEVICE_TEST_CREATE_SERVICE = 4,
    PLAT_DEVICE_TEST_BIND_DEVICE = 5,
    PLAT_DEVICE_TEST_RELIABILITY = 6,
    PLAT_DEVICE_TEST_CMD_MAX,
};

int PlatformDeviceTestExecute(int cmd);
void PlatformDeviceTestExecuteAll(void);

#ifdef __cplusplus
}
#endif
#endif /* PLATFORM_DEVICE_TEST_H */
