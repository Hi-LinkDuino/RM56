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

#ifndef HOS_CAMERA_BUFFER_TRACE_H
#define HOS_CAMERA_BUFFER_TRACE_H

#include "camera.h"
#include <list>
#include <string>

namespace OHOS::Camera {
enum {
    INVALID_TRACKING_ID = -1,
    NODE_IS_EMPTY,
    NODE_IS_NOT_EMPTY,
};

struct BufferTrackingMessage {
    int32_t trackingId = -1;
    uint64_t frameNumber = 0;
    std::string nodeName = "";

    // isReturnBack = true indicate a buffer is recycled.
    bool isReturnBack = false;
};

class TrackingBuffer {
public:
    explicit TrackingBuffer(const uint64_t frameNumber)
    {
        frameNumber_ = frameNumber;
    }
    ~TrackingBuffer() = default;

    bool operator==(const TrackingBuffer& id)
    {
        return this->frameNumber_ == id.frameNumber_;
    }

    uint64_t GetFrameNumber() const
    {
        return frameNumber_;
    }

private:
    TrackingBuffer() = default;

private:
    uint64_t frameNumber_ = 0;
};

using BufferTraceGraph = std::list<std::pair<std::string, std::list<TrackingBuffer>>>;
} // namespace OHOS::Camera
#endif
