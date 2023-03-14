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

#include "fork_node.h"
#include "securec.h"

namespace OHOS::Camera {
ForkNode::ForkNode(const std::string& name, const std::string& type) : NodeBase(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

ForkNode::~ForkNode()
{
    streamRunning_ = false;
    cv_.notify_all();
    if (forkThread_ != nullptr) {
        CAMERA_LOGI("forkThread need join");
        forkThread_->join();
        forkThread_ = nullptr;
    }
    CAMERA_LOGI("fork Node exit.");
}

RetCode ForkNode::Start(const int32_t streamId)
{
    CAMERA_LOGI("ForkNode::Start streamId = %{public}d\n", streamId);

    if (streamRunning_ == true)
        return RC_OK;

    inPutPorts_ = GetInPorts();
    outPutPorts_ = GetOutPorts();
    if (streamRunning_ == false) {
        CAMERA_LOGI("ForkNode::Start::streamrunning = false");
        streamRunning_ = true;
        ForkBuffers();
    }
    return RC_OK;
}

RetCode ForkNode::Stop(const int32_t streamId)
{
    CAMERA_LOGI("ForkNode::Stop streamId = %{public}d\n", streamId);

    streamRunning_ = false;
    cv_.notify_all();
    if (forkThread_ != nullptr) {
        CAMERA_LOGI("ForkNode::Stop need join");
        forkThread_->join();
        forkThread_ = nullptr;
    }
    return RC_OK;
}

RetCode ForkNode::Flush(const int32_t streamId)
{
    CAMERA_LOGI("ForkNode::Flush streamId = %{public}d\n", streamId);
    return RC_OK;
}

void ForkNode::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer == nullptr) {
        CAMERA_LOGE("frameSpec is null");
        return;
    }
    int32_t id = buffer->GetStreamId();
    {
        std::unique_lock <std::mutex> lck(mtx_);
        tmpBuffer_ = buffer;
        cv_.notify_one();
    }
    for (auto& it : outPutPorts_) {
        if (it->format_.streamId_ == id) {
            it->DeliverBuffer(buffer);
            CAMERA_LOGI("fork node deliver buffer streamid = %{public}d", it->format_.streamId_);
            return;
        }
    }
}

RetCode ForkNode::Capture(const int32_t streamId, const int32_t captureId)
{
    CAMERA_LOGV("ForkNode::received a request from stream [id:%{public}d], queue size:%{public}u",
        streamId, captureRequests_[streamId].size());

    for (auto& in : inPutPorts_) {
        CAMERA_LOGI("ForkNode::Capture in->format_.streamId_ = %{public}d", in->format_.streamId_);
        if (streamId == in->format_.streamId_) {
            return RC_OK;
        }
    }

    std::lock_guard<std::mutex> l(requestLock_);
    if (captureRequests_.count(streamId) == 0) {
        captureRequests_[streamId] = {captureId};
    } else {
        captureRequests_[streamId].emplace_back(captureId);
    }

    return RC_OK;
}

RetCode ForkNode::CancelCapture(const int32_t streamId)
{
    CAMERA_LOGI("ForkNode::CancelCapture streamid = %{public}d", streamId);
    cv_.notify_all();

    return RC_OK;
}

void ForkNode::ForkBuffers()
{
    int32_t id = 0;
    uint64_t bufferPoolId = 0;
    for (auto& in : inPutPorts_) {
        for (auto& out : outPutPorts_) {
            if (out->format_.streamId_ != in->format_.streamId_) {
                id = out->format_.streamId_;
                bufferPoolId = out->format_.bufferPoolId_;
                CAMERA_LOGI("fork buffer get buffer streamId = %{public}d", out->format_.streamId_);
            }
        }
    }
    forkThread_ = std::make_shared<std::thread>([this, id, bufferPoolId] {
        prctl(PR_SET_NAME, "fork_buffers");
        std::shared_ptr<FrameSpec> frameSpec = std::make_shared<FrameSpec>();
        std::shared_ptr<IBuffer> buffer = nullptr;
        while (streamRunning_ == true) {
            if (CopyBuffer(bufferPoolId, buffer) != RC_OK) {
                continue;
            }
            for (auto& it : outPutPorts_) {
                if (it->format_.streamId_ == id) {
                    CAMERA_LOGI("fork node deliver buffer streamid = %{public}d", it->format_.streamId_);

                    int32_t id = buffer->GetStreamId();
                    {
                        std::lock_guard<std::mutex> l(requestLock_);
                        CAMERA_LOGV("ForkNode::deliver a buffer to stream id:%{public}d, queue size:%{public}u",
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
                    it->DeliverBuffer(buffer);
                    break;
                }
            }
        }
        CAMERA_LOGI("fork thread closed");
        return RC_OK;
    });
    return;
}

RetCode ForkNode::CopyBuffer(uint64_t poolId, std::shared_ptr<IBuffer>& buffer)
{
    BufferManager* bufferManager = Camera::BufferManager::GetInstance();
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferManager, RC_ERROR);
    std::unique_lock <std::mutex> lck(mtx_);
    cv_.wait(lck);
    std::shared_ptr<IBufferPool> bufferPool = bufferManager->GetBufferPool(poolId);
    if (bufferPool == nullptr) {
        CAMERA_LOGE("get bufferpool failed");
        return RC_ERROR;
    }
    CAMERA_LOGI("fork node acquirebuffer enter");
    buffer = bufferPool->AcquireBuffer();
    CAMERA_LOGI("fork node acquirebuffer exit");
    if (buffer == nullptr) {
        CAMERA_LOGE("acquire buffer failed.");
        return RC_ERROR;
    }
    if (memcpy_s(buffer->GetVirAddress(), buffer->GetSize(),
        tmpBuffer_->GetVirAddress(), tmpBuffer_->GetSize()) != 0) {
            CAMERA_LOGE("memcpy_s failed.");
    }
    return RC_OK;
}

REGISTERNODE(ForkNode, {"fork"})
} // namespace OHOS::Camera
