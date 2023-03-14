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

#ifndef STREAM_OPERATOR_STREAM_OPERATOR_H
#define STREAM_OPERATOR_STREAM_OPERATOR_H

#include "camera_device.h"
#include "capture_message.h"
#include "istream.h"
#include "istream_operator.h"
#include "offline_stream_operator.h"
#include "offline_stream.h"
#include "stream_operator_stub.h"
#include "surface.h"
#include "types.h"
namespace OHOS::Camera {
class StreamOperator : public StreamOperatorStub {
public:
    virtual CamRetCode IsStreamsSupported(OperationMode mode,
                                          const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting,
                                          const std::vector<std::shared_ptr<StreamInfo>>& pInfo,
                                          StreamSupportType& type) override;
    virtual CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>>& streamInfos) override;
    virtual CamRetCode ReleaseStreams(const std::vector<int>& streamIds) override;
    virtual CamRetCode CommitStreams(OperationMode mode,
                                     const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting) override;
    virtual CamRetCode GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes) override;
    virtual CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer) override;
    virtual CamRetCode DetachBufferQueue(int streamId) override;
    virtual CamRetCode
        Capture(int captureId, const std::shared_ptr<CaptureInfo>& captureInfo, bool isStreaming) override;
    virtual CamRetCode CancelCapture(int captureId) override;
    virtual CamRetCode ChangeToOfflineStream(const std::vector<int>& streamIds,
                                             OHOS::sptr<IStreamOperatorCallback>& callback,
                                             OHOS::sptr<IOfflineStreamOperator>& offlineOperator) override;

public:
    StreamOperator() = default;
    StreamOperator(const OHOS::sptr<IStreamOperatorCallback>& callback, const std::weak_ptr<CameraDevice>& device);
    virtual ~StreamOperator();
    StreamOperator(const StreamOperator& other) = delete;
    StreamOperator(StreamOperator&& other) = delete;
    StreamOperator& operator=(const StreamOperator& other) = delete;
    StreamOperator& operator=(StreamOperator&& other) = delete;

    RetCode Init();
    RetCode ReleaseStreams();

private:
    void HandleCallbackMessage(MessageGroup& message);
    void OnCaptureStarted(int32_t captureId, const std::vector<int32_t>& streamIds);
    void OnCaptureEnded(int32_t captureId, const std::vector<std::shared_ptr<CaptureEndedInfo>>& infos);
    void OnCaptureError(int32_t captureId, const std::vector<std::shared_ptr<CaptureErrorInfo>>& infos);
    void OnFrameShutter(int32_t captureId, const std::vector<int32_t>& streamIds, uint64_t timestamp);
    bool CheckStreamInfo(const std::shared_ptr<StreamInfo>& streamInfo);
    DynamicStreamSwitchMode CheckStreamsSupported(OperationMode mode,
                                                  const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting,
                                                  const std::vector<std::shared_ptr<StreamInfo>>& infos);

private:
    OHOS::sptr<IStreamOperatorCallback> callback_ = nullptr;
    std::weak_ptr<CameraDevice> device_;
    std::shared_ptr<IPipelineCore> pipelineCore_ = nullptr;
    std::shared_ptr<IStreamPipelineCore> streamPipeline_ = nullptr;
    std::shared_ptr<CaptureMessageOperator> messenger_ = nullptr;

    std::mutex streamLock_ = {};
    std::unordered_map<int32_t, std::shared_ptr<IStream>> streamMap_ = {};

    std::mutex requestLock_ = {};
    std::unordered_map<int32_t, std::shared_ptr<CaptureRequest>> requestMap_ = {};
    OHOS::sptr<OfflineStreamOperator> oflstor_ = nullptr;
    std::function<void()> requestTimeoutCB_ = nullptr;
};
} // end namespace OHOS::Camera
#endif // STREAM_OPERATOR_STREAM_OPERATOR_H
