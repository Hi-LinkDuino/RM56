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

#ifndef STREAM_OPERATOR_STREAM_TUNNEL_H
#define STREAM_OPERATOR_STREAM_TUNNEL_H

#include <mutex>
#include <unordered_map>
#include "display_type.h"
#include "ibuffer.h"
#include "surface.h"
#include "surface_type.h"

namespace OHOS::Camera {
struct TunnelConfig {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t usage;
};

class StreamTunnel {
public:
    virtual RetCode AttachBufferQueue(std::shared_ptr<OHOS::Surface>& producer);
    virtual RetCode DetachBufferQueue();
    virtual std::shared_ptr<IBuffer> GetBuffer();
    virtual RetCode PutBuffer(const std::shared_ptr<IBuffer>& buffer);
    virtual RetCode SetBufferCount(const int32_t n);
    virtual RetCode Config(const TunnelConfig& config);
    virtual uint64_t GetFrameCount() const;
    virtual void WaitForAllBufferReturned();
    virtual void NotifyStart();
    virtual void NotifyStop();
    virtual void CleanBuffers();

    StreamTunnel() = default;
    virtual ~StreamTunnel();
    StreamTunnel(const StreamTunnel& other) = delete;
    StreamTunnel(StreamTunnel&& other) = delete;
    StreamTunnel& operator=(const StreamTunnel& other) = delete;
    StreamTunnel& operator=(StreamTunnel&& other) = delete;

protected:
    int32_t index = -1;
    uint64_t frameCount_ = 0;
    std::shared_ptr<OHOS::Surface> bufferQueue_ = nullptr;
    std::unordered_map<std::shared_ptr<IBuffer>, OHOS::SurfaceBuffer*> buffers = {};
    std::mutex lock_ = {};
    std::mutex waitLock_ = {};
    std::condition_variable waitCV_ = {};
    std::atomic<bool> wakeup_ = false;
    std::atomic<bool> stop_ = false;
    std::atomic<uint32_t> restBuffers = 0;
    std::mutex finishLock_ = {};
    std::condition_variable finishCV_ = {};
};
} // namespace OHOS::Camera
#endif
