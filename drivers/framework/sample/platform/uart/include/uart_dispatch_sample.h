/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef UART_DISPATCH_SAMPLE_H
#define UART_DISPATCH_SAMPLE_H

#include "uart_pl011_sample.h"

enum {
    UART_WRITE = 1
};

int32_t SampleDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);

#endif // UART_DISPATCH_SAMPLE_H
