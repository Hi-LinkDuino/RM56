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
#include "capture_message.h"

namespace OHOS::Camera {
ICaptureMessage::ICaptureMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t count)
{
    peerMessageCount_ = count;
    timestamp_ = time;
    streamId_ = streamId;
    captureId_ = captureId;
}

uint32_t ICaptureMessage::GetPeerMessageCount() const
{
    return peerMessageCount_;
}

uint64_t ICaptureMessage::GetTimestamp() const
{
    return timestamp_;
}

int32_t ICaptureMessage::GetStreamId() const
{
    return streamId_;
}

int32_t ICaptureMessage::GetCaptureId() const
{
    return captureId_;
}

CaptureMessageType ICaptureMessage::GetMessageType() const
{
    return CAPTURE_MESSAGE_TYPE_INVALID;
}

CaptureMessageOperator::CaptureMessageOperator(MessageOperatorFunc f)
{
    messageOperator_ = f;
}

CaptureMessageOperator::~CaptureMessageOperator()
{
    running_ = false;
    cv_.notify_one();
    if (messageHandler_ != nullptr) {
        messageHandler_->join();
    }
    messageBox_.clear();
}

void CaptureMessageOperator::SendMessage(std::shared_ptr<ICaptureMessage>& message)
{
    if (message == nullptr) {
        return;
    }

    CAMERA_LOGV("%{public}s, %{public}d, enter", __FUNCTION__, __LINE__);
    std::unique_lock<std::mutex> l(lock_);
    CAMERA_LOGV("%{public}s, %{public}d, enter", __FUNCTION__, __LINE__);
    auto it = messageBox_.find(message->GetMessageType());
    if (it == messageBox_.end()) {
        messageBox_[message->GetMessageType()] = {{message}};
        if (message->GetPeerMessageCount() == 1) {
            wakeup_ = true;
            cv_.notify_one();
        }
    } else {
        bool isPeerMessage = false;
        for (auto& mit : it->second) {
            if (mit.empty()) {
                continue;
            }
            if (mit[0] == nullptr) {
                CAMERA_LOGE("unknown error, message is null");
                continue;
            }
            if (message->GetTimestamp() == mit[0]->GetTimestamp() &&
                message->GetMessageType() == mit[0]->GetMessageType()) {
                mit.emplace_back(message);
                if (mit.size() == mit[0]->GetPeerMessageCount()) {
                    wakeup_ = true;
                    cv_.notify_one();
                }
                isPeerMessage = true;
                break;
            }
        }
        if (!isPeerMessage) {
            MessageGroup mg = {message};
            it->second.emplace_back(mg);
            if (message->GetPeerMessageCount() == 1) {
                wakeup_ = true;
                cv_.notify_one();
            }
        }
    }
    CAMERA_LOGV("%{public}s, %{public}d, enter", __FUNCTION__, __LINE__);

    return;
}

RetCode CaptureMessageOperator::StartProcess()
{
    running_ = true;
    messageHandler_ = std::make_unique<std::thread>([this]() {
        prctl(PR_SET_NAME, "MessageOperator");
        while (running_) {
            HandleMessage();
        }
    });
    if (messageHandler_ == nullptr) {
        return RC_ERROR;
    }
    return RC_OK;
}

void CaptureMessageOperator::HandleMessage()
{
    {
        std::unique_lock<std::mutex> l(lock_);
        cv_.wait(l, [this] { return !running_ || wakeup_; });
        wakeup_ = false;
    }

    if (!running_) {
        return;
    }

    std::list<MessageGroup> messages = {};
    {
        CAMERA_LOGV("%{public}s, %{public}d, enter", __FUNCTION__, __LINE__);
        std::unique_lock<std::mutex> l(lock_);
        for (auto& lit : messageBox_) {
            for (auto& vit : lit.second) {
                if (vit.empty()) {
                    continue;
                }

                if (vit.size() == vit[0]->GetPeerMessageCount()) {
                    messages.emplace_back(vit);
                    vit.clear();
                }
            }
            for (auto it = lit.second.begin(); it != lit.second.end();) {
                if (it->empty()) {
                    it = lit.second.erase(it);
                    continue;
                }
                it++;
            }
        }
        CAMERA_LOGV("%{public}s, %{public}d, enter", __FUNCTION__, __LINE__);
    }
    for (auto it = messages.begin(); it != messages.end();) {
        messageOperator_(*it);
        it = messages.erase(it);
    }
    return;
}
}
// namespace OHOS::Camera
