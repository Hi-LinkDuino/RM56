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

#include "allocator_service_stub.h"
#include "buffer_handle_parcel.h"
#include "buffer_handle_utils.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf_ipc.h"
#include "parcel_utils.h"

#define HDF_LOG_TAG HDI_DISP_STUB

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
int32_t AllocatorServiceStub::AllocaltorStubAllocMem(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != AllocatorServiceStub::GetDescriptor()) {
        HDF_LOGE("AllocMem: failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    AllocInfo info;
    if (ParcelUtils::UnpackAllocInfo(data, &info) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: UnpackAllocInfo failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    BufferHandle *buffer = nullptr;
    int32_t errCode = AllocMem(info, buffer);

    if (!reply.WriteInt32(errCode)) {
        HDF_LOGE("AllocMem: write reply failed!");
        return HDF_FAILURE;
    }

    if (errCode != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:  call failed", __func__);
        return errCode;
    }

    if (WriteBufferHandle(reply, *buffer) != true) {
        HDF_LOGE("%{public}s: WriteBufferHandle failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    FreeBufferHandle(buffer);
    return HDF_SUCCESS;
}

int32_t AllocatorServiceStub::OnRemoteRequest(
    uint32_t cmdId, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (cmdId) {
        case CMD_ALLOCATOR_ALLOCMEM:
            return AllocaltorStubAllocMem(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: not support cmd", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS
