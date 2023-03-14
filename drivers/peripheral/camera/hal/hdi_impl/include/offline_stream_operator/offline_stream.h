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

#ifndef HOS_CAMERA_OFFLINE_STREAM_H
#define HOS_CAMERA_OFFLINE_STREAM_H

#include "camera.h"
#include "offline_stream_context.h"
#include "istream_operator_callback.h"
#include <condition_variable>
#include <mutex>
#include "capture_message.h"

namespace OHOS::Camera {
class OfflineStream {
public:
    OfflineStream() = default;
    virtual ~OfflineStream();
    OfflineStream(int32_t id, OHOS::sptr<IStreamOperatorCallback>& callback);

    RetCode Init(std::shared_ptr<OfflineStreamContext>& context);
    RetCode CancelCapture(int32_t captureId);
    RetCode Release();
    bool CheckCaptureIdExist(int32_t captureId);
    void ReceiveOfflineBuffer(std::shared_ptr<IBuffer>& buffer);
    int32_t GetStreamId() const;

private:
    RetCode ReturnBuffer(std::shared_ptr<IBuffer>& buffer);
    void HandleMessage(MessageGroup& message);
    void OnCaptureEnded(int32_t captureId, const std::vector<std::shared_ptr<CaptureEndedInfo>>& infos);
    void OnCaptureError(int32_t captureId, const std::vector<std::shared_ptr<CaptureErrorInfo>>& infos);

private:
    int32_t streamId_ = -1;
    std::shared_ptr<OfflineStreamContext> context_ = {};
    OHOS::sptr<IStreamOperatorCallback> callback_ = nullptr;
    std::mutex lock_;
    std::shared_ptr<CaptureMessageOperator> messenger_ = nullptr;
};
} // namespace OHOS::Camera
#endif
