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

#include "devmgr_service.h"
#include "devmgr_service_full.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hdf_device_manager

int main()
{
    HDF_LOGI("start hdf device manager");
    int status = HDF_FAILURE;
    struct IDevmgrService* instance = DevmgrServiceGetInstance();
    if (instance == NULL) {
        HDF_LOGE("Getting DevmgrService instance failed");
        return status;
    }

    if (instance->StartService != NULL) {
        status = instance->StartService(instance);
    }

    if (status == HDF_SUCCESS) {
        struct DevmgrServiceFull *fullService = (struct DevmgrServiceFull *)instance;
        struct HdfMessageLooper *looper = &fullService->looper;
        if ((looper != NULL) && (looper->Start != NULL)) {
            looper->Start(looper);
        }
    }

    HDF_LOGE("starting device manager service failed, status is %{public}d", status);
    return status;
}
