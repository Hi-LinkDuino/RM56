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

#include "buffer_loop_tracking.h"
#include "buffer_manager.h"

namespace OHOS::Camera {
TrackingNode::TrackingNode(std::string name)
{
    nodeName_ = name;
}

TrackingNode::~TrackingNode()
{
    {
        std::lock_guard<std::mutex> l(lock_);
        trackingBufferList_.clear();
    }
}

std::string TrackingNode::GetNodeName() const
{
    return nodeName_;
}

std::shared_ptr<TrackingBuffer> TrackingNode::FindTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer)
{
    if (buffer == nullptr) {
        return nullptr;
    }

    std::lock_guard<std::mutex> l(lock_);
    auto it = std::find_if(trackingBufferList_.begin(), trackingBufferList_.end(),
        [&buffer](const std::shared_ptr<TrackingBuffer>& b) {
            return b->GetFrameNumber() == buffer->GetFrameNumber();
        });
    if (it == trackingBufferList_.end()) {
        return nullptr;
    }
    return *it;
}

void TrackingNode::AttachTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer)
{
    {
        std::lock_guard<std::mutex> l(lock_);
        trackingBufferList_.emplace_back(buffer);
    }

    return;
}

void TrackingNode::DetachTrackingBuffer(const std::shared_ptr<TrackingBuffer>& buffer)
{
    {
        std::lock_guard<std::mutex> l(lock_);
        auto it = std::find_if(trackingBufferList_.begin(), trackingBufferList_.end(),
            [&buffer](const std::shared_ptr<TrackingBuffer>& b) {
                return b->GetFrameNumber() == buffer->GetFrameNumber();
            });
        if (it != trackingBufferList_.end()) {
            trackingBufferList_.erase(it);
        }
    }

    return;
}

bool TrackingNode::IsEmpty()
{
    std::lock_guard<std::mutex> l(lock_);
    return trackingBufferList_.empty();
}

TrackingStream::TrackingStream(const int32_t id)
{
    trackingId_ = id;
}

TrackingStream::~TrackingStream()
{
    {
        std::lock_guard<std::mutex> l(lock_);
        trackingNodeList_.clear();
        CAMERA_LOGD("dtor TrackingStream %{public}p", this);
    }
}

int32_t TrackingStream::GetTrackingStreamId() const
{
    return trackingId_;
}

std::shared_ptr<TrackingNode> TrackingStream::FindTrackingNode(const std::string node)
{
    std::lock_guard<std::mutex> l(lock_);
    auto it = std::find_if(trackingNodeList_.begin(), trackingNodeList_.end(),
                           [&node](const std::shared_ptr<TrackingNode>& n) { return n->GetNodeName() == node; });
    if (it != trackingNodeList_.end()) {
        return *it;
    }

    return nullptr;
}

void TrackingStream::AttachTrackingNode(const std::shared_ptr<TrackingNode>& node)
{
    if (FindTrackingNode(node->GetNodeName()) != nullptr) {
        return;
    }

    {
        std::lock_guard<std::mutex> l(lock_);
        trackingNodeList_.emplace_back(node);
    }

    return;
}

void TrackingStream::DetachTrackingNode(const std::shared_ptr<TrackingNode>& node)
{
    auto n = FindTrackingNode(node->GetNodeName());
    if (n == nullptr) {
        return;
    }

    {
        std::lock_guard<std::mutex> l(lock_);
        auto it = std::find(trackingNodeList_.begin(), trackingNodeList_.end(), node);
        if (it != trackingNodeList_.end()) {
            trackingNodeList_.erase(it);
        }
    }

    return;
}

std::shared_ptr<TrackingNode> TrackingStream::LocateBuffer(const std::shared_ptr<TrackingBuffer>& buffer)
{
    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto& n : trackingNodeList_) {
            if (n == nullptr) {
                CAMERA_LOGE("fatal error, tracking node list is corrupt.");
                return nullptr;
            }
            if (n->IsEmpty()) {
                continue;
            }
            if (n->FindTrackingBuffer(buffer) != nullptr) {
                return n;
            }
        }
        return nullptr;
    }
}

void TrackingStream::MoveBuffer(std::shared_ptr<TrackingBuffer>& buffer, std::shared_ptr<TrackingNode>& node)
{
    if (node == nullptr) {
        CAMERA_LOGE("node is null, cannot track");
        return;
    }
    auto dest = FindTrackingNode(node->GetNodeName());
    if (dest == nullptr) {
        CAMERA_LOGE("node %{public}s doesn't being tracked", node->GetNodeName().c_str());
        return;
    }

    {
        std::lock_guard<std::mutex> tl(traceLock_);
        auto src = LocateBuffer(buffer);
        if (src != nullptr) {
            src->DetachTrackingBuffer(buffer);
        }

        dest->AttachTrackingBuffer(buffer);
    }
    return;
}

void TrackingStream::RemoveBuffer(std::shared_ptr<TrackingBuffer>& buffer)
{
    std::lock_guard<std::mutex> tl(traceLock_);
    auto src = LocateBuffer(buffer);
    if (src != nullptr) {
        src->DetachTrackingBuffer(buffer);
    }

    return;
}

void TrackingStream::DumpTrace(BufferTraceGraph& graph)
{
    graph.clear();
    std::lock_guard<std::mutex> tl(traceLock_);
    std::lock_guard<std::mutex> l(lock_);
    for (auto& n : trackingNodeList_) {
        if (n == nullptr) {
            graph.clear();
            return;
        }
        std::list<TrackingBuffer> buffers = {};
        auto bufferList = n->GetTrackingBuffer();
        for (auto& b : bufferList) {
            if (b == nullptr) {
                graph.clear();
                return;
            }
            buffers.emplace_back(*b);
        }
        graph.emplace_back(std::make_pair(n->GetNodeName(), buffers));
    }

    return;
}

std::list<std::shared_ptr<TrackingNode>> TrackingStream::FindTrackingNodePath(const std::string beginNode,
                                                                              const std::string endNode)
{
    std::lock_guard<std::mutex> l(lock_);
    std::list<std::shared_ptr<TrackingNode>> nodePath = {};
    bool isInPath = false;
    for (auto& n : trackingNodeList_) {
        if (n == nullptr) {
            return {};
        }
        if (n->GetNodeName() == beginNode || n->GetNodeName() == endNode) {
            if (nodePath.empty()) {
                isInPath = true;
            } else {
                isInPath = false;
            }
            nodePath.emplace_back(n);
            continue;
        }
        if (isInPath) {
            nodePath.emplace_back(n);
        }
    }
    return nodePath;
}

BufferLoopTracking& BufferLoopTracking::GetInstance()
{
    static BufferLoopTracking stalker;
    return stalker;
}

BufferLoopTracking::~BufferLoopTracking()
{
    {
        std::lock_guard<std::mutex> l(lock_);
        trackingStreamList_.clear();
        handler_ = nullptr;
    }
}

std::shared_ptr<TrackingStream> BufferLoopTracking::FindTrackingStream(const int32_t id)
{
    std::lock_guard<std::mutex> l(lock_);
    auto it = std::find_if(
        trackingStreamList_.begin(), trackingStreamList_.end(),
        [&id](const std::shared_ptr<TrackingStream>& stream) { return stream->GetTrackingStreamId() == id; });
    if (it != trackingStreamList_.end()) {
        return *it;
    }

    return nullptr;
}

void BufferLoopTracking::AddTrackingStreamBegin(const int32_t trackingId, const int64_t poolId)
{
    auto stream = FindTrackingStream(trackingId);
    if (stream != nullptr) {
        return;
    }

    {
        std::lock_guard<std::mutex> l(lock_);
        stream = std::make_shared<TrackingStream>(trackingId);
        trackingStreamList_.emplace_back(stream);
        CAMERA_LOGD("add tracking stream %{public}d begin", trackingId);
    }

    auto manager = BufferManager::GetInstance();
    if (manager == nullptr) {
        CAMERA_LOGE("can't get buffer manager");
        return;
    }

    auto pool = manager->GetBufferPool(poolId);
    if (pool == nullptr) {
        CAMERA_LOGE("can't get buffer pool, id = %{public}lld", poolId);
        return;
    }
    pool->EnableTracking(trackingId);

    return;
}

void BufferLoopTracking::AddTrackingStreamEnd(const int32_t trackingId)
{
    auto stream = FindTrackingStream(trackingId);
    if (stream == nullptr) {
        return;
    }
    stream->NodeAddComplete();
    CAMERA_LOGD("add tracking stream %{public}d end", trackingId);

    return;
}

void BufferLoopTracking::DeleteTrackingStream(const int32_t trackingId)
{
    std::lock_guard<std::mutex> l(lock_);
    auto it = std::find_if(trackingStreamList_.begin(), trackingStreamList_.end(),
        [&trackingId](const std::shared_ptr<TrackingStream>& stream) {
            return stream->GetTrackingStreamId() == trackingId;
        });
    if (it == trackingStreamList_.end()) {
        CAMERA_LOGE("stream %{public}d is not being tracked.", trackingId);
        return;
    }
    trackingStreamList_.erase(it);

    return;
}

void BufferLoopTracking::AddTrackingNode(const int32_t trackingId, const std::string node)
{
    auto stream = FindTrackingStream(trackingId);
    if (stream == nullptr) {
        CAMERA_LOGI("can't add node %{public}s to stream %{public}d", node.c_str(), trackingId);
        return;
    }
    auto n = std::make_shared<TrackingNode>(node);
    return stream->AttachTrackingNode(n);
}

std::shared_ptr<BufferTrackingMessage> BufferLoopTracking::ReceiveMessage()
{
    if (messageQueue_.empty()) {
        std::unique_lock<std::mutex> l(messageLock_);
        if (messageQueue_.empty()) {
            cv_.wait(l, [this] { return !(running_.load() && messageQueue_.empty()); });
        }
    }

    if (!running_.load()) {
        return nullptr;
    }

    std::shared_ptr<BufferTrackingMessage> message = nullptr;
    if (!messageQueue_.empty()) {
        std::unique_lock<std::mutex> l(messageLock_);
        if (!messageQueue_.empty()) {
            message = messageQueue_.front();
            messageQueue_.pop_front();
            return message;
        }
    }

    return message;
}

void BufferLoopTracking::SendBufferMovementMessage(const std::shared_ptr<BufferTrackingMessage>& message)
{
    std::unique_lock<std::mutex> l(messageLock_);
    messageQueue_.emplace_back(message);
    cv_.notify_one();

    return;
}

void BufferLoopTracking::HandleMessage()
{
    auto message = ReceiveMessage();
    if (message == nullptr) {
        return;
    }

    auto stream = FindTrackingStream(message->trackingId);
    if (stream == nullptr) {
        CAMERA_LOGE("cannot handle stream %{public}d", message->trackingId);
        return;
    }

    if (!stream->IsNodeComplete()) {
        CAMERA_LOGW("tracking node in stream %{public}d is incomplete", message->trackingId);
        return;
    }

    auto buffer = std::make_shared<TrackingBuffer>(message->frameNumber);
    if (message->isReturnBack) {
        stream->RemoveBuffer(buffer);
        CAMERA_LOGI("buffer %{public}llu return back to pool.", message->frameNumber);
        return;
    }

    auto node = std::make_shared<TrackingNode>(message->nodeName);
    stream->MoveBuffer(buffer, node);

    return;
}

void BufferLoopTracking::StartTracking()
{
    handler_ = std::make_unique<std::thread>([this] {
        prctl(PR_SET_NAME, "buffertracking");
        do {
            HandleMessage();
        } while (running_.load() == true);
    });
    if (handler_ == nullptr) {
        return;
    }
    running_ = true;
    return;
}

void BufferLoopTracking::StopTracking()
{
    running_ = false;
    cv_.notify_one();
    handler_->join();
    return;
}

int32_t BufferLoopTracking::IsEmpty(const int32_t id, const std::string node)
{
    auto stream = FindTrackingStream(id);
    if (stream == nullptr) {
        CAMERA_LOGI("stream %{public}d doesn't exist", id);
        return INVALID_TRACKING_ID;
    }
    auto n = stream->FindTrackingNode(node);
    return n->IsEmpty() ? NODE_IS_EMPTY : NODE_IS_NOT_EMPTY;
}

int32_t BufferLoopTracking::IsEmpty(const int32_t id, const std::string beginNode, const std::string endNode)
{
    auto stream = FindTrackingStream(id);
    if (stream == nullptr) {
        CAMERA_LOGI("stream %{public}d doesn't exist", id);
        return INVALID_TRACKING_ID;
    }
    auto nodePath = stream->FindTrackingNodePath(beginNode, endNode);
    for (auto& n : nodePath) {
        if (n == nullptr) {
            continue;
        }
        if (n->IsEmpty() != true) {
            return NODE_IS_NOT_EMPTY;
        } else {
            return NODE_IS_EMPTY;
        }
    }

    return true;
}

void BufferLoopTracking::DumpTrace(const int32_t id, BufferTraceGraph& graph)
{
    auto stream = FindTrackingStream(id);
    if (stream == nullptr) {
        CAMERA_LOGI("stream %{public}d doesn't exist, can't dump trace", id);
        return;
    }

    stream->DumpTrace(graph);
    return;
}
} // namespace OHOS::Camera
