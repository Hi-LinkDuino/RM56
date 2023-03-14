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

#ifndef HDI_STREAM_STILL_CAPTURE_H
#define HDI_STREAM_STILL_CAPTURE_H

#include "stream_base.h"

namespace OHOS::Camera {
class StreamStillCapture : public StreamBase {
public:
    StreamStillCapture(const int32_t id,
                       const StreamIntent type,
                       std::shared_ptr<IPipelineCore>& p,
                       std::shared_ptr<CaptureMessageOperator>& m);
    virtual ~StreamStillCapture();

public:
    virtual void HandleResult(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode Capture(const std::shared_ptr<CaptureRequest>& request) override;
    virtual RetCode ChangeToOfflineStream(std::shared_ptr<OfflineStream> offlineStream) override;
    virtual RetCode StopStream() override;
    virtual bool IsRunning() const override;

private:
    std::weak_ptr<OfflineStream> offlineStream;
    std::mutex offlineLock_ = {};
};
} // end namespace OHOS::Camera
#endif // HDI_STREAM_STILL_CAPTURE_H
