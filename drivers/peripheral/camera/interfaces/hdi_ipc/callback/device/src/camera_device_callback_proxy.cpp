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

#include "camera_device_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "metadata_utils.h"

namespace OHOS::Camera {
void CameraDeviceCallbackProxy::OnError(ErrorType type, int32_t errorMsg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteUint32(type)) {
        HDF_LOGE("%{public}s: write error type failed.", __func__);
        return;
    }

    if (!data.WriteInt32(errorMsg)) {
        HDF_LOGE("%{public}s: write error message failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_CALLBACK_ON_ERROR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void CameraDeviceCallbackProxy::OnResult(uint64_t timestamp,
    const std::shared_ptr<CameraStandard::CameraMetadata> &result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (result == nullptr) {
        return;
    }

    if (!data.WriteInterfaceToken(CameraDeviceCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteUint64(timestamp)) {
        HDF_LOGE("%{public}s: write timestamp failed.", __func__);
        return;
    }

    if (!CameraStandard::MetadataUtils::EncodeCameraMetadata(result, data)) {
        HDF_LOGE("%s: write metadata failed", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_CALLBACK_ON_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}
}