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

#include "merge_node.h"
#include <unistd.h>
namespace OHOS::Camera{
MergeNode::MergeNode(const std::string& name, const std::string& type) : NodeBase(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

MergeNode::~MergeNode()
{
    streamRunning_ = false;
    cv_.notify_all();
    if (mergeThread_ != nullptr) {
        CAMERA_LOGI("mergeThread need join");
        mergeThread_->join();
        mergeThread_ = nullptr;
    }
}

RetCode MergeNode::Start(const int32_t streamId)
{
    (void)streamId;
    GetOutPorts();
    if (streamRunning_ == false) {
        CAMERA_LOGI("streamrunning = false");
        streamRunning_ = true;
    }
    MergeBuffers();
    return RC_OK;
}

RetCode MergeNode::Stop(const int32_t streamId)
{
    streamRunning_ = false;
    (void)streamId;
    cv_.notify_all();
    if (mergeThread_ != nullptr) {
        CAMERA_LOGI("mergeThread need join");
        mergeThread_->join();
        mergeThread_ = nullptr;
    }
    return RC_OK;
}

void MergeNode::DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    CAMERA_LOGI("merge node get frame");
    if (frameSpec == nullptr) {
        CAMERA_LOGE("frameSpec is null");
        return;
    }
    {
        std::unique_lock<std::mutex> lck(mtx_);
        mergeVec_.push_back(frameSpec);
        bufferNum_++;
        cv_.notify_all();
    }
    return;
}

void MergeNode::MergeBuffers()
{
    mergeThread_ = std::make_shared<std::thread>([this] {
        prctl(PR_SET_NAME, "merge_buffers");
        tmpVec_.clear();
        while (streamRunning_ == true) {
            if (bufferNum_ > 0) {
                auto outPorts = GetOutPorts();
                for (auto& it : outPorts) {
                    if (tmpVec_.size() == 0) {
                    {
                        std::unique_lock<std::mutex> lck(mtx_);
                        cv_.wait(lck, [this] { return !mergeVec_.empty(); });
                        auto tmpFrame = std::find_if(mergeVec_.begin(), mergeVec_.end(),
                            [it](std::shared_ptr<FrameSpec> fs) {
                            return it->format_.bufferPoolId_ == fs->bufferPoolId_;
                        });
                        if (tmpFrame != mergeVec_.end()) {
                            tmpVec_.push_back((*tmpFrame));
                            mergeVec_.erase(tmpFrame);
                            bufferNum_--;
                            break;
                        }
                    }
                    } else if (tmpVec_.size() == 1) {
                    {
                        std::unique_lock<std::mutex> lck(mtx_);
                        auto tmpFrame_2 = std::find_if(mergeVec_.begin(), mergeVec_.end(),
                            [it](std::shared_ptr<FrameSpec> fs) {
                            return it->format_.bufferPoolId_ != fs->bufferPoolId_;
                        });
                        if (tmpFrame_2 != mergeVec_.end()) {
                            tmpVec_.push_back((*tmpFrame_2));
                            mergeVec_.erase(tmpFrame_2);
                            bufferNum_--;
                        }
                    }
                    } else if (tmpVec_.size() == 2) {
                        for (auto it : outPorts) {
                            it->DeliverBuffers(tmpVec_);
                            tmpVec_.clear();
                        }
                    }
                    usleep(20);
                }
            }
        }
        CAMERA_LOGI("merge thread closed");
        return;
    });
    return;
}
REGISTERNODE(MergeNode, {"merge"})
}// namespace OHOS::Camera
