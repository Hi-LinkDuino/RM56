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

#include "ipc_stream_operator_fuzzer.h"
#include "fuzz_base.h"

#include <cstddef>
#include <cstdint>

class IPCStreamOperatorFuzzer : public StreamOperatorStub {
public:
    CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::vector<std::shared_ptr<StreamInfo>> &info,
        StreamSupportType &type) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::shared_ptr<StreamInfo> &info,
        StreamSupportType &type) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode ReleaseStreams(const std::vector<int>& streamIds) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode CommitStreams(OperationMode mode,
                                     const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode DetachBufferQueue(int streamId) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode Capture(int captureId,
        const std::shared_ptr<CaptureInfo>& captureInfo, bool isStreaming) override {}
        {
            return OHOS::Camera::NO_ERROR;
        }
    CamRetCode CancelCapture(int captureId) override
    {
        return OHOS::Camera::NO_ERROR;
    }
    CamRetCode ChangeToOfflineStream(const std::vector<int>& streamIds,
        OHOS::sptr<IStreamOperatorCallback>& callback,
        OHOS::sptr<IOfflineStreamOperator>& offlineOperator) override
        {
            return OHOS::Camera::NO_ERROR;
        }
};

static uint32_t U32_AT(const uint8_t *ptr)
{
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

static int32_t onRemoteRequest(uint32_t code, MessageParcel &data)
{
    MessageParcel reply;
    MessageOption option;
    IPCStreamOperatorFuzzer IPCStreamSer;
    auto ret = IPCStreamSer.OnRemoteRequest(code, data, reply, option);
    return ret;
}

static void IpcFuzzService(const uint8_t *data, size_t size)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel dataMessageParcel;
    if (size > sizeof(uint32_t)) {
        uint32_t code = U32_AT(data);
        uint8_t *number = data;
        number = number + sizeof(uint32_t);
        if (code == 7) { // 7:code size
            return;
        }
        size_t length = size;
        length = length - sizeof(uint32_t);
        dataMessageParcel.WriteInterfaceToken(StreamOperatorStub::GetDescriptor());
        dataMessageParcel.WriteBuffer(number, length);
        dataMessageParcel.RewindRead(0);
        onRemoteRequest(code, dataMessageParcel);
    }
}

static void OnRemoteRequestFunc(const uint8_t *data, size_t size)
{
    IpcFuzzService(data, size);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OnRemoteRequestFunc(data, size);
    return 0;
}