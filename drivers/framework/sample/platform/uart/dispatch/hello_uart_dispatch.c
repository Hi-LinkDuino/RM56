/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <string.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "uart_if.h"

#define HDF_LOG_TAG hello_uart_dispatch
#define UART_PORT 5

int main(void)
{
    const char *info = " HELLO UART! ";

    struct DevHandle *handle = UartOpen(UART_PORT);
    if (handle == NULL) {
        HDF_LOGE("Failed to open uart %d", UART_PORT);
        return HDF_FAILURE;
    }

    int ret = UartWrite(handle, (uint8_t *)info, strlen(info));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Failed to send data to uart");
    }

    UartClose(handle);
    return ret;
}