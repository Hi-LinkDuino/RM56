/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_usb_entry_test.h"
#include "hdf_log.h"
#include "usb_test.h"
#include "usb_raw_test.h"

#define HDF_LOG_TAG hdf_usb_entry_test

int32_t HdfUsbUnitTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        return HDF_FAILURE;
    }
    dprintf("---%s::command[%u], subCommand[%u]\n", __func__, msg->cmd, msg->subCmd);
    if (msg->cmd == TEST_USB_HOST_TYPE) {
        msg->result = UsbTestEntry(msg->subCmd);
    } else if (msg->cmd == TEST_USB_HOST_RAW_TYPE) {
        msg->result = UsbRawTestEntry(msg->subCmd);
    }
    dprintf("%s::command[%u], subCommand[%u], result:%d\n", __func__, msg->cmd, msg->subCmd, msg->result);
    return msg->result;
}
