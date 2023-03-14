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
#include "hdf_load_hdi.h"

#define HDF_LOG_TAG hdf_sample_service_a
#define SAMPLEA_RET 100
#define SAMPLEB_RET 101

int SampleAServiceA()
{
    HDF_LOGE("%{public}s", __func__);
    return SAMPLEA_RET;
}

int SampleAServiceB(int para)
{
    HDF_LOGE("%{public}s %{public}d", __func__, para);
    return SAMPLEB_RET;
}

int SampleAOpen()
{
    HDF_LOGE("%{public}s", __func__);
    return 0;
}

struct HdiSampleA {
    struct HdiBase base;
    int (*ServiceA)();
    int (*ServiceB)(int para);
};

struct HdiSampleA g_sampleA = {
    .base = {
        .moduleVersion = 1,
        .moduleName = "SampleServiceA",
        .OpenHdi = SampleAOpen,
    },
    .ServiceA = SampleAServiceA,
    .ServiceB = SampleAServiceB,
};

HDF_HDI_INIT(g_sampleA);

