/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_dac_entry_test.h"
#include "dac_test.h"

#define HDF_LOG_TAG hdf_dac_entry_test

int32_t HdfDacTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    msg->result = DacTestExecute(msg->subCmd);

    return HDF_SUCCESS;
}
