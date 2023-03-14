/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_pwm_entry_test.h"
#include "hdf_log.h"
#include "pwm_test.h"

#define HDF_LOG_TAG hdf_pwm_entry_test

int32_t HdfPwmUnitTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        HDF_LOGE("%s: msg is NULL!", __func__);
        return HDF_FAILURE;
    }

    msg->result = PwmTestExecute(msg->subCmd);
    return HDF_SUCCESS;
}
