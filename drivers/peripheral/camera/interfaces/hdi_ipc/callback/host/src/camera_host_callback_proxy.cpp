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

#include "camera_host_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>

namespace OHOS::Camera {
void CameraHostCallbackProxy::OnCameraStatus(const std::string &cameraId, CameraStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed.", __func__);
        return;
    }

    if (!data.WriteInt32(status)) {
        HDF_LOGE("%{public}s: write status failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_STATUS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void CameraHostCallbackProxy::OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed.", __func__);
        return;
    }

    if (!data.WriteInt32(status)) {
        HDF_LOGE("%{public}s: write status failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_FLASHLIGHT_STATUS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void CameraHostCallbackProxy::OnCameraEvent(const std::string &cameraId, CameraEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed.", __func__);
        return;
    }

    if (!data.WriteUint32(event)) {
        HDF_LOGE("%{public}s: write event failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_CAMERA_EVENT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}
}