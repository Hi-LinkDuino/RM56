/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_mipi_csi_entry_test.h"
#include "hdf_log.h"
#include "mipi_csi_test.h"

#define HDF_LOG_TAG hdf_mipi_csi_entry_test

int32_t HdfMipiCsiEntry(HdfTestMsg *msg)
{
    int32_t ret;
    struct MipiCsiTest *test = NULL;

    if (msg == NULL) {
        HDF_LOGE("%s: msg is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    test = MipiCsiTestServiceGet();
    if (test == NULL) {
        HDF_LOGE("%s: test is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HDF_LOGI("%s: call [doTest]", __func__);
    ret = test->doTest(test, msg->subCmd);
    msg->result = (int8_t)ret;

    return ret;
}
