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

#ifndef HOS_BUFFER_LOOP_TRACKING_H
#define HOS_BUFFER_LOOP_TRACKING_H

#include "buffer_trace.h"
#include "camera.h"
#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <algorithm>

namespace OHOS::Camera {
class TrackingNode {
public:
    explicit TrackingNode(std::string name);
    ~TrackingNode();

    std::string GetNodeName() const;
    std::shared_ptr<TrackingBuffer> FindTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer);
    void AttachTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer);
    void DetachTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer);
    bool IsEmpty();
    std::list<std::shared_ptr<TrackingBuffer>> GetTrackingBuffer() const
    {
        return trackingBufferList_;
    }

    bool operator==(const TrackingNode& n)
    {
        return this->nodeName_ == n.nodeName_;
    }

private:
    std::mutex lock_;
    std::string nodeName_ = "";
    std::list<std::shared_ptr<TrackingBuffer>> trackingBufferList_ = {};

private:
    TrackingNode() = default;
};

class TrackingStream {
public:
    explicit TrackingStream(const int32_t id);
    ~TrackingStream();

    int32_t GetTrackingStreamId() const;
    void AttachTrackingNode(const std::shared_ptr<TrackingNode>& node);
    void DetachTrackingNode(const std::shared_ptr<TrackingNode>& node);
    void MoveBuffer(std::shared_ptr<TrackingBuffer>& buffer, std::shared_ptr<TrackingNode>& node);
    void RemoveBuffer(std::shared_ptr<TrackingBuffer>& buffer);
    void DumpTrace(BufferTraceGraph& graph);
    bool operator==(const TrackingStream& n)
    {
        return this->trackingId_ == n.trackingId_;
    }
    std::shared_ptr<TrackingNode> FindTrackingNode(const std::string node);
    std::shared_ptr<TrackingNode> LocateBuffer(const std::shared_ptr<TrackingBuffer>& buffer);
    std::list<std::shared_ptr<TrackingNode>> FindTrackingNodePath(const std::string beginNode,
                                                                  const std::string endNode);
    void NodeAddComplete()
    {
        addNodeComplete_ = true;
    }

    bool IsNodeComplete() const
    {
        return addNodeComplete_;
    }

private:
    int32_t trackingId_ = -1;
    bool addNodeComplete_ = false;
    std::list<std::shared_ptr<TrackingNode>> trackingNodeList_ = {};
    std::mutex lock_;
    std::mutex traceLock_;

private:
    TrackingStream() = default;
};

class BufferLoopTracking {
public:
    static BufferLoopTracking& GetInstance();
    void AddTrackingStreamBegin(const int32_t trackingId, const int64_t poolId);
    void AddTrackingStreamEnd(const int32_t trackingId);
    void DeleteTrackingStream(const int32_t trackingId);
    void AddTrackingNode(const int32_t trackingId, const std::string node);
    void SendBufferMovementMessage(const std::shared_ptr<BufferTrackingMessage>& message);
    void StartTracking();
    void StopTracking();
    int32_t IsEmpty(const int32_t id, const std::string node);
    int32_t IsEmpty(const int32_t id, const std::string beginNode, const std::string endNode);
    void DumpTrace(const int32_t trackingId, BufferTraceGraph& graph);

private:
    BufferLoopTracking() = default;
    BufferLoopTracking(const BufferLoopTracking&);
    BufferLoopTracking& operator=(const BufferLoopTracking&);
    BufferLoopTracking(BufferLoopTracking&&);
    BufferLoopTracking& operator=(BufferLoopTracking&&);
    ~BufferLoopTracking();

    std::shared_ptr<TrackingStream> FindTrackingStream(const int32_t id);
    std::shared_ptr<BufferTrackingMessage> ReceiveMessage();
    void HandleMessage();

private:
    std::mutex lock_;
    std::mutex messageLock_;
    std::condition_variable cv_;
    std::atomic<bool> running_ = false;

    std::unique_ptr<std::thread> handler_ = nullptr;
    std::list<std::shared_ptr<TrackingStream>> trackingStreamList_ = {};
    std::list<std::shared_ptr<BufferTrackingMessage>> messageQueue_ = {};
};
} // namespace OHOS::Camera
#endif
