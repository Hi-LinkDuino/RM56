/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef GPIO_SERVICE_H
#define GPIO_SERVICE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum GpioIoCmd {
    GPIO_IO_READ = 0,
    GPIO_IO_WRITE = 1,
    GPIO_IO_GETDIR = 2,
    GPIO_IO_SETDIR = 3,
    GPIO_IO_SETIRQ = 4,
    GPIO_IO_UNSETIRQ = 5,
    GPIO_IO_ENABLEIRQ = 6,
    GPIO_IO_DISABLEIRQ = 7,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* GPIO_SERVICE_H */
