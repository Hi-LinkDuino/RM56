/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef UART_IF_H
#define UART_IF_H

#include <stdint.h>

#define UART_DEV_SERVICE_NAME_PREFIX "HDF_PLATFORM_UART_%d"
#define MAX_DEV_NAME_SIZE 32

enum {
    UART_WRITE = 1
};

struct DevHandle {
    void *object;
};

struct DevHandle *UartOpen(uint32_t port);
int32_t UartWrite(struct DevHandle *handle, uint8_t *data, uint32_t size);
void UartClose(struct DevHandle *handle);

#endif // UART_IF_H