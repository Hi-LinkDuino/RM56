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

#include "istream_operator.h"
#include "stream_operator_c_if.h"
#include "stream_operator_stub.h"

namespace OHOS::Camera {
class StreamOperator : public StreamOperatorStub {
public:
    void Init(StreamOperatorCIF* op);
    virtual CamRetCode IsStreamsSupported(OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::shared_ptr<StreamInfo> &pInfo,
        StreamSupportType &pType) override;
    virtual CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) override;
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    virtual CamRetCode CommitStreams(OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting) override;
    virtual CamRetCode GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>> &attributes) override;
    virtual CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer> &producer) override;
    virtual CamRetCode DetachBufferQueue(int streamId) override;
    virtual CamRetCode Capture(int captureId, const std::shared_ptr<CaptureInfo> &pInfo,  bool isStreaming) override;
    virtual CamRetCode CancelCapture(int captureId) override;
    virtual CamRetCode ChangeToOfflineStream(const std::vector<int> &streamIds,
        OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IOfflineStreamOperator> &offlineOperator) override;

public:
    StreamOperator();
    virtual ~StreamOperator();
    StreamOperator(const StreamOperator &other) = delete;
    StreamOperator(StreamOperator &&other) = delete;
    StreamOperator& operator=(const StreamOperator &other) = delete;
    StreamOperator& operator=(StreamOperator &&other) = delete;

private:
    StreamOperatorCIF* operator_ = nullptr;
};
} // end namespace OHOS::Camera
#endif // STREAM_OPERATOR_STREAM_OPERATOR_H
