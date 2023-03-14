/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "source_node.h"
#include <unistd.h>

namespace OHOS::Camera {
SourceNode::SourceNode(const std::string& name, const std::string& type) : NodeBase(name, type)
{
    name_ = name;
    type_ = type;
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

SourceNode::~SourceNode()
{
    CAMERA_LOGV("%{public}s, source node dtor.", __FUNCTION__);
}

RetCode SourceNode::Init(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode SourceNode::Start(const int32_t streamId)
{
    std::shared_ptr<IPort> port = nullptr;
    auto outPorts = GetOutPorts();
    for (auto& p : outPorts) {
        PortFormat format = {};
        p->GetFormat(format);
        if (streamId == format.streamId_) {
            port = p;
            break;
        }
    }
    if (port == nullptr) {
        return RC_ERROR;
    }

    {
        std::lock_guard<std::mutex> l(hndl_);
        if (handler_.count(streamId) > 0) {
            CAMERA_LOGI("stream [%{public}d] start again, skip", streamId);
            return RC_OK;
        }
    }

    SetBufferCallback();
    std::shared_ptr<PortHandler> ph = std::make_shared<PortHandler>(port);
    CHECK_IF_PTR_NULL_RETURN_VALUE(ph, RC_ERROR);
    {
        std::lock_guard<std::mutex> l(hndl_);
        handler_[streamId] = ph;
    }
    RetCode rc = handler_[streamId]->StartCollectBuffers();
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);

    rc = handler_[streamId]->StartDistributeBuffers();
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);

    return RC_OK;
}

RetCode SourceNode::Flush(const int32_t streamId)
{
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(handler_.count(streamId) > 0, true, RC_ERROR);
    handler_[streamId]->StopCollectBuffers();
    return RC_OK;
}

RetCode SourceNode::Stop(const int32_t streamId)
{
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(handler_.count(streamId) > 0, true, RC_ERROR);
    handler_[streamId]->StopDistributeBuffers();

    {
        std::lock_guard<std::mutex> l(hndl_);
        auto it = handler_.find(streamId);
        if (it != handler_.end()) {
            handler_.erase(it);
        }
    }
    return RC_OK;
}

RetCode SourceNode::Config(const int32_t streamId, const CaptureMeta& meta)
{
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(handler_.count(streamId) > 0, true, RC_ERROR);
    (void)meta;
    return RC_OK;
}

void SourceNode::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_PTR_NULL_RETURN_VOID(buffer);
    int32_t id = buffer->GetStreamId();
    {
        std::lock_guard<std::mutex> l(requestLock_);
        CAMERA_LOGV("deliver a buffer to stream id:%{public}d, queue size:%{public}u",
            id, captureRequests_[id].size());
        if (captureRequests_.count(id) == 0) {
            buffer->SetBufferStatus(CAMERA_BUFFER_STATUS_INVALID);
        } else if (captureRequests_[id].empty()) {
            buffer->SetBufferStatus(CAMERA_BUFFER_STATUS_INVALID);
        } else {
            buffer->SetCaptureId(captureRequests_[id].front());
            captureRequests_[id].pop_front();
        }
    }
    NodeBase::DeliverBuffer(buffer);
}

void SourceNode::OnPackBuffer(std::shared_ptr<FrameSpec> frameSpec)
{
    CAMERA_LOGI("SourceNode::OnPackBuffer enter");

    CHECK_IF_PTR_NULL_RETURN_VOID(frameSpec);
    auto buffer = frameSpec->buffer_;
    CHECK_IF_PTR_NULL_RETURN_VOID(buffer);
    handler_[buffer->GetStreamId()]->OnBuffer(buffer);

    CAMERA_LOGI("SourceNode::OnPackBuffer exit");
    return;
}

void SourceNode::SetBufferCallback()
{
    return;
}

RetCode SourceNode::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    (void)frameSpec;
    return RC_OK;
}

RetCode SourceNode::Capture(const int32_t streamId, const int32_t captureId)
{
    std::lock_guard<std::mutex> l(requestLock_);
    if (captureRequests_.count(streamId) == 0) {
        captureRequests_[streamId] = {captureId};
    } else {
        captureRequests_[streamId].emplace_back(captureId);
    }
    CAMERA_LOGV("received a request from stream [id:%{public}d], queue size:%{public}u",
        streamId, captureRequests_[streamId].size());
    return RC_OK;
}

RetCode SourceNode::CancelCapture(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

SourceNode::PortHandler::PortHandler(std::shared_ptr<IPort>& p) : port(p)
{
}

RetCode SourceNode::PortHandler::StartCollectBuffers()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(port, RC_ERROR);
    PortFormat format = {};
    port->GetFormat(format);
    uint32_t streamId = format.streamId_;

    pool = BufferManager::GetInstance()->GetBufferPool(format.bufferPoolId_);
    CHECK_IF_PTR_NULL_RETURN_VALUE(pool, RC_ERROR);
    pool->NotifyStart();
    CAMERA_LOGI("SourceNode::PortHandler::StartCollectBuffers");

    cltRun = true;
    collector = std::make_unique<std::thread>([this, &streamId] {
        std::string name = "collect#" + std::to_string(streamId);
        prctl(PR_SET_NAME, name.c_str());
        while (cltRun) {
            CollectBuffers();
        }
    });

    return RC_OK;
}

RetCode SourceNode::PortHandler::StopCollectBuffers()
{
    CAMERA_LOGI("SourceNode::PortHandler::StopCollectBuffers enter");
    cltRun = false;
    pool->NotifyStop();
    if (collector != nullptr) {
        collector->join();
    }

    CHECK_IF_PTR_NULL_RETURN_VALUE(pool, RC_ERROR);
    auto node = port->GetNode();
    if (node != nullptr) {
        uint32_t n = pool->GetIdleBufferCount();
        for (uint32_t i = 0; i < n; i++) {
            auto buffer = pool->AcquireBuffer(-1);
            node->DeliverBuffer(buffer);
        }
    }
    CAMERA_LOGI("SourceNode::PortHandler::StopCollectBuffers exit");
    return RC_OK;
}

void SourceNode::PortHandler::CollectBuffers()
{
    CAMERA_LOGV("SourceNode::PortHandler::CollectBuffers");
    CHECK_IF_PTR_NULL_RETURN_VOID(pool);
    std::shared_ptr<IBuffer> buffer = pool->AcquireBuffer(-1);
    CHECK_IF_PTR_NULL_RETURN_VOID(buffer);

    PortFormat format = {};
    port->GetFormat(format);
    std::shared_ptr<FrameSpec> frameSpec = std::make_shared<FrameSpec>();
    frameSpec->bufferPoolId_ = format.bufferPoolId_;
    frameSpec->bufferCount_ = format.bufferCount_;
    frameSpec->buffer_ = buffer;

    auto node = port->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VOID(node);
    RetCode rc = node->ProvideBuffers(frameSpec);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("provide buffer failed.");
    }
}

RetCode SourceNode::PortHandler::StartDistributeBuffers()
{
    dbtRun = true;
    distributor = std::make_unique<std::thread>([this] {
        PortFormat format = {};
        port->GetFormat(format);
        int id = format.streamId_;
        std::string name = "collect#" + std::to_string(id);
        prctl(PR_SET_NAME, name.c_str());
        while (dbtRun) {
            DistributeBuffers();
        }
    });

    return RC_OK;
}

RetCode SourceNode::PortHandler::StopDistributeBuffers()
{
    CAMERA_LOGV("SourceNode::PortHandler::StopDistributeBuffers enter");
    FlushBuffers();
    dbtRun = false;
    rbcv.notify_one();
    if (distributor != nullptr) {
        distributor->join();
    }
    CAMERA_LOGV("SourceNode::PortHandler::StopDistributeBuffers exit");
    return RC_OK;
}

void SourceNode::PortHandler::DistributeBuffers()
{
    std::unique_lock<std::mutex> l(rblock);
    rbcv.wait(l, [this] { return dbtRun == false || !respondBufferList.empty(); });
    if (!dbtRun) {
        return;
    }

    auto node = port->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VOID(node);
    auto buffer = respondBufferList.front();
    node->DeliverBuffer(buffer);

    respondBufferList.pop_front();

    return;
}

void SourceNode::PortHandler::OnBuffer(std::shared_ptr<IBuffer>& buffer)
{
    CAMERA_LOGV("SourceNode::PortHandler::OnBuffer enter");
    {
        std::unique_lock<std::mutex> l(rblock);
        respondBufferList.emplace_back(buffer);
    }
    rbcv.notify_one();
    CAMERA_LOGV("SourceNode::PortHandler::OnBuffer exit");

    return;
}

void SourceNode::PortHandler::FlushBuffers()
{
    CAMERA_LOGV("SourceNode::PortHandler::FlushBuffers enter");
    if (respondBufferList.empty()) {
        CAMERA_LOGV("SourceNode::PortHandler::FlushBuffers respondBufferList is empty");
        return;
    }

    auto node = port->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VOID(node);
    std::unique_lock<std::mutex> l(rblock);
    while (!respondBufferList.empty()) {
        auto buffer = respondBufferList.front();
        node->DeliverBuffer(buffer);
        respondBufferList.pop_front();
    }
    CAMERA_LOGV("SourceNode::PortHandler::FlushBuffers exit");

    return;
}

REGISTERNODE(SourceNode, {"source"})
} // namespace OHOS::Camera
