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

#include "stream_operator_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>

namespace OHOS::Camera {
void StreamOperatorCallbackProxy::OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteInt32(captureId)) {
        HDF_LOGE("%{public}s: write captureId failed.", __func__);
        return;
    }

    if (!data.WriteInt32Vector(streamId)) {
        HDF_LOGE("%{public}s: write streamIds failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_STARTED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void StreamOperatorCallbackProxy::OnCaptureEnded(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureEndedInfo>> &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteInt32(captureId)) {
        HDF_LOGE("%{public}s: write captureId failed.", __func__);
        return;
    }

    size_t size = info.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        HDF_LOGE("%{public}s: write info size failed.", __func__);
        return;
    }
    for (size_t i = 0; i < size; i++) {
        auto captureEndInfo = info.at(i);
        bool bRet = data.WriteBuffer((void*)captureEndInfo.get(), sizeof(CaptureEndedInfo));
        if (!bRet) {
            HDF_LOGE("%{public}s: write info index = %d failed.", __func__, i);
            return;
        }
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ENDED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void StreamOperatorCallbackProxy::OnCaptureError(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureErrorInfo>> &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteInt32(captureId)) {
        HDF_LOGE("%{public}s: write captureId failed.", __func__);
        return;
    }

    size_t size = info.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        HDF_LOGE("%{public}s: write info size failed.", __func__);
        return;
    }
    for (size_t i = 0; i < size; i++) {
        auto captureErrorInfo = info.at(i);
        bool bRet = data.WriteBuffer((void*)captureErrorInfo.get(), sizeof(CaptureErrorInfo));
        if (!bRet) {
            HDF_LOGE("%{public}s: write info index = %d failed.", __func__, i);
            return;
        }
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ERROR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}

void StreamOperatorCallbackProxy::OnFrameShutter(int32_t captureId,
    const std::vector<int32_t> &streamId, uint64_t timestamp)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    HDF_LOGV("%{public}s: enter OnFrameShutter Callback", __func__);

    if (!data.WriteInterfaceToken(StreamOperatorCallbackProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    if (!data.WriteInt32(captureId)) {
        HDF_LOGE("%{public}s: write captureId failed.", __func__);
        return;
    }

    if (!data.WriteInt32Vector(streamId)) {
        HDF_LOGE("%{public}s: write streamId failed.", __func__);
        return;
    }

    if (!data.WriteUint64(timestamp)) {
        HDF_LOGE("%{public}s: write streamId failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CALLBACK_ON_FRAME_SHUTTER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return;
    }
}
}