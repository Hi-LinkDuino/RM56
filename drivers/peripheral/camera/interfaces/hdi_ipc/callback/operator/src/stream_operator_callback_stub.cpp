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

#include "stream_operator_callback_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "stream_operator_callback.h"
#include "cmd_common.h"

namespace OHOS::Camera {
int32_t StreamOperatorCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HDF_LOGE("%s: StreamOperatorCallbackStub::OnRemoteRequest entry!", __func__);
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_STARTED: {
            ret = OnCaptureStartedStub(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ENDED: {
            ret = OnCaptureEndedStub(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ERROR: {
            ret = OnCaptureErrorStub(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CALLBACK_ON_FRAME_SHUTTER: {
            ret = OnFrameShutterStub(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t StreamOperatorCallbackStub::OnCaptureStartedStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorCallbackStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t captureId = data.ReadInt32();
    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%{public}s: OnCaptureStarted read streamIds failed.", __func__);
        return HDF_FAILURE;
    }
    OnCaptureStarted(captureId, streamIds);

    return HDF_SUCCESS;
}

int32_t StreamOperatorCallbackStub::OnCaptureEndedStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorCallbackStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t captureId = data.ReadInt32();
    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<CaptureEndedInfo>> info;
    for (int32_t i = 0; i < count; i++) {
        const CaptureEndedInfo *pInfo =
            reinterpret_cast<const CaptureEndedInfo*>(
            data.ReadBuffer(sizeof(CaptureEndedInfo)));
        if (pInfo == nullptr) {
            HDF_LOGE("%{public}s: read ended info failed.", __func__);
            return HDF_FAILURE;
        }
        std::shared_ptr<CaptureEndedInfo> captureEndedInfo = std::make_shared<CaptureEndedInfo>();
        captureEndedInfo->streamId_ = pInfo->streamId_;
        captureEndedInfo->frameCount_ = pInfo->frameCount_;
        info.push_back(captureEndedInfo);
    }
    OnCaptureEnded(captureId, info);

    return HDF_SUCCESS;
}

int32_t StreamOperatorCallbackStub::OnCaptureErrorStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorCallbackStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t captureId = data.ReadInt32();
    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<CaptureErrorInfo>> info;
    for (int32_t i = 0; i < count; i++) {
        const CaptureErrorInfo *pInfo =
            reinterpret_cast<const CaptureErrorInfo*>(
            data.ReadBuffer(sizeof(CaptureErrorInfo)));
        if (pInfo == nullptr) {
            HDF_LOGE("%{public}s: read error info failed.", __func__);
            return HDF_FAILURE;
        }
        std::shared_ptr<CaptureErrorInfo> captureErrorInfo = std::make_shared<CaptureErrorInfo>();
        captureErrorInfo->streamId_ = pInfo->streamId_;
        captureErrorInfo->error_ = pInfo->error_;
        info.push_back(captureErrorInfo);
    }

    OnCaptureError(captureId, info);
    return HDF_SUCCESS;
}

int32_t StreamOperatorCallbackStub::OnFrameShutterStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorCallbackStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t captureId = data.ReadInt32();
    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%{public}s: read streamIds failed.", __func__);
        return HDF_FAILURE;
    }
    uint64_t timestamp = data.ReadUint64();
    OnFrameShutter(captureId, streamIds, timestamp);

    return HDF_SUCCESS;
}
}
