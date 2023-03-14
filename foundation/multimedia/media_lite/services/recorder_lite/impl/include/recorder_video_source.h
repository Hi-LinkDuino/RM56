/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_VIDEO_SOURCE_H
#define RECORDER_VIDEO_SOURCE_H

#include "recorder_source.h"
#include <memory>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "format.h"
#include "media_errors.h"
#include "media_info.h"
#include "surface.h"

namespace OHOS {
namespace Media {
using OHOS::Surface;
using OHOS::SurfaceBuffer;
using OHOS::IBufferConsumerListener;

class RecorderVideoSource : public RecorderSource, public IBufferConsumerListener {
public:

    RecorderVideoSource();
    ~RecorderVideoSource() override;

    std::shared_ptr<OHOS::Surface> GetSurface();

    int32_t SetSurfaceSize(int32_t width, int32_t height);

    int32_t Start() override;

    int32_t AcquireBuffer(RecorderSourceBuffer &buffer, bool isBlocking) override;

    int32_t ReleaseBuffer(RecorderSourceBuffer &buffer) override;

    int32_t Stop() override;

    int32_t Resume() override;

    int32_t Pause() override;

    int32_t Release() override;

    void OnBufferAvailable() override;

private:
    std::shared_ptr<Surface> surface_;
    std::mutex lock_;
    std::condition_variable frameAvailableCondition_;
    int32_t frameAvailableCount_;
    std::vector<SurfaceBuffer*> acquiredBuffers;
    SurfaceBuffer *acquireBuffer_;
    bool started_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // RECORDER_VIDEO_SOURCE_H
