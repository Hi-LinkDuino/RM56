/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "allocator_service_impl.h"
#include "allocator_service_stub.h"
#include "buffer_handle_parcel.h"
#include "buffer_handle_utils.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf_ipc.h"

#define HDF_LOG_TAG HDI_DISP_GALLOC

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
AllocatorService::AllocatorService() : grallocFuncs_(nullptr)
{
    if (GrallocInitialize(&grallocFuncs_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: gralloc init failed", __func__);
    }
}

AllocatorService::~AllocatorService()
{
    if (GrallocUninitialize(grallocFuncs_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: gralloc uninit failed", __func__);
    }
}

int32_t AllocatorService::AllocMem(const AllocInfo &info, BufferHandle *&handle)
{
    if (grallocFuncs_ == nullptr) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return grallocFuncs_->AllocMem(&info, &handle);
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

using namespace OHOS::HDI::Display::V1_0;

void *AllocatorServiceInstance(void)
{
    return reinterpret_cast<void *>(new AllocatorService());
}

void AllocatorServiceRelease(void *servObj)
{
    delete reinterpret_cast<AllocatorService *>(servObj);
}

int32_t AllocatorServiceOnRemoteRequest(void *service, int cmdId, struct HdfSBuf &data, struct HdfSBuf &reply)
{
    if (service == nullptr) {
        HDF_LOGE("%{public}s: stub is nullptr", __func__);
        return HDF_FAILURE;
    }

    AllocatorService *allocatorService = reinterpret_cast<AllocatorService *>(service);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    if (SbufToParcel(&reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(&data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return allocatorService->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}