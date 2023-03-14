/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_i2c_entry_test.h"
#include "i2c_test.h"

#define HDF_LOG_TAG hdf_i2c_entry_test

int32_t HdfI2cTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    msg->result = I2cTestExecute(msg->subCmd);

    return HDF_SUCCESS;
}
