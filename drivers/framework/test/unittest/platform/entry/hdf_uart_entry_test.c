/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_uart_entry_test.h"
#include "uart_test.h"

#define HDF_LOG_TAG hdf_uart_entry_test

int32_t HdfUartUnitTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    msg->result = UartTestExecute(msg->subCmd);

    return msg->result;
}
