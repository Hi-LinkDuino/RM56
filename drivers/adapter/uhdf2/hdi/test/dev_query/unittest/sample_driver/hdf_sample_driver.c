/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hdf_log.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"

#define HDF_LOG_TAG hdf_sample_driver

int32_t HdfSampleDriverBind(struct HdfDeviceObject *para)
{
    (void)para;

    HDF_LOGE("%{public}s", __func__);
    return 0;
}

int32_t HdfSampleDriverInit(struct HdfDeviceObject *para)
{
    (void)para;

    HDF_LOGE("%{public}s", __func__);
    return 0;
}

void HdfSampleDriverRelease(struct HdfDeviceObject *para)
{
    (void)para;

    HDF_LOGE("%{public}s", __func__);
}

struct HdfDriverEntry g_sampleDriverEntry = {
    .moduleVersion = 1,
    .Bind = HdfSampleDriverBind,
    .Init = HdfSampleDriverInit,
    .Release = HdfSampleDriverRelease,
    .moduleName = "sample_driver",
};
HDF_INIT(g_sampleDriverEntry);

