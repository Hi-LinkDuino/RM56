/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_timer_entry_test.h"
#include "hdf_log.h"
#include "timer_test.h"

#define HDF_LOG_TAG hdf_timer_entry_test

int32_t HdfTimerUnitTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    msg->result = TimerTestExecute(msg->subCmd);
    return msg->result;
}
