/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_i3c_entry_test.h"
#include "hdf_log.h"
#include "i3c_test.h"

#define HDF_LOG_TAG hdf_adc_entry_test

int32_t HdfI3cTestEntry(HdfTestMsg *msg)
{
    HDF_LOGI("%s: enter", __func__);
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    msg->result = I3cTestExecute(msg->subCmd);

    return HDF_SUCCESS;
}