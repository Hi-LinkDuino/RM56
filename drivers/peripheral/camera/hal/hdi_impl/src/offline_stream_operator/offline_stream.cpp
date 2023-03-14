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

#include "offline_stream.h"
#include "offline_pipeline_manager.h"

namespace OHOS::Camera {
OfflineStream::OfflineStream(int32_t id, OHOS::sptr<IStreamOperatorCallback>& callback)
{
    streamId_ = id;
    callback_ = callback;
}

OfflineStream::~OfflineStream()
{
    context_->restRequests.clear();
    context_->tunnel->CleanBuffers();
    context_->bufferPool->ClearBuffers();
}

int32_t OfflineStream::GetStreamId() const
{
    return streamId_;
}

RetCode OfflineStream::Init(std::shared_ptr<OfflineStreamContext>& context)
{
    context_ = context;
    CHECK_IF_PTR_NULL_RETURN_VALUE(context_, RC_ERROR);

    auto callback = [this](MessageGroup& m) { HandleMessage(m); };
    messenger_ = std::make_shared<CaptureMessageOperator>(callback);
    CHECK_IF_PTR_NULL_RETURN_VALUE(messenger_, RC_ERROR);
    messenger_->StartProcess();

    for (auto it : context_->restRequests) {
        it->DisableSync();
    }

    OfflinePipelineManager& manager = OfflinePipelineManager::GetInstance();
    std::shared_ptr<IStreamPipelineCore> pipeline = context_->pipeline.lock();
    auto cb = [this](std::shared_ptr<IBuffer>& buffer) { ReceiveOfflineBuffer(buffer); };
    RetCode ret = manager.SwitchToOfflinePipeline(streamId_, context_->streamInfo.type, pipeline, cb);
    if (ret != RC_OK) {
        CAMERA_LOGE("switch to offline stream failed.");
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode OfflineStream::CancelCapture(int32_t captureId)
{
    OfflinePipelineManager& manager = OfflinePipelineManager::GetInstance();
    RetCode ret = manager.CancelCapture(streamId_, captureId);
    if (ret != RC_OK) {
        CAMERA_LOGE("cancel capture %{public}d failed", captureId);
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode OfflineStream::Release()
{
    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto it : context_->restRequests) {
            CancelCapture(it->GetCaptureId());
        }
    }

    context_->tunnel->WaitForAllBufferReturned();
    OfflinePipelineManager& manager = OfflinePipelineManager::GetInstance();
    RetCode ret = manager.DestoryOfflinePipeline(streamId_);
    if (ret != RC_OK) {
        CAMERA_LOGE("release offline pipeline %{public}d failed", streamId_);
        return RC_ERROR;
    }

    return RC_OK;
}

void OfflineStream::ReceiveOfflineBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (context_ == nullptr) {
        return;
    }

    if (buffer == nullptr) {
        CAMERA_LOGE("fatal error, buffer should not be null");
        return;
    }

    ReturnBuffer(buffer);
    return;
}

RetCode OfflineStream::ReturnBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto tunnel = context_->tunnel;
    auto bufferPool = context_->bufferPool;

    CHECK_IF_PTR_NULL_RETURN_VALUE(buffer, RC_ERROR);
    if (buffer->GetBufferStatus() == CAMERA_BUFFER_STATUS_INVALID) {
        CAMERA_LOGI("offline stream [id:%{public}d], this buffer(index:%{public}d) has nothing to do with request.",
            streamId_, buffer->GetIndex());
        tunnel->PutBuffer(buffer);
        bufferPool->ReturnBuffer(buffer);
        return RC_OK;
    }

    int32_t captureId = buffer->GetCaptureId();

    std::shared_ptr<CaptureRequest> request = nullptr;
    {
        std::lock_guard<std::mutex> l(lock_);
        auto it = std::find_if(
            context_->restRequests.begin(), context_->restRequests.end(),
            [captureId](const std::shared_ptr<CaptureRequest>& r) { return r->GetCaptureId() == captureId; });
        if (it == context_->restRequests.end()) {
            CAMERA_LOGE("fatal error, offline stream [%{public}d] reveived a wrong buffer, index:%{public}d.",
                streamId_, buffer->GetIndex());
            return RC_ERROR;
        }
        request = *it;
        context_->restRequests.erase(it);
    }

    CameraBufferStatus status = buffer->GetBufferStatus();
    if (status != CAMERA_BUFFER_STATUS_OK) {
        if (status != CAMERA_BUFFER_STATUS_DROP) {
            std::shared_ptr<ICaptureMessage> errorMessage =
                std::make_shared<CaptureErrorMessage>(streamId_, request->GetCaptureId(), request->GetEndTime(),
                                                      request->GetOwnerCount(), static_cast<StreamError>(status));
            messenger_->SendMessage(errorMessage);
        }
    }

    if (request->NeedShutterCallback()) {
        std::shared_ptr<ICaptureMessage> shutterMessage = std::make_shared<FrameShutterMessage>(
            streamId_, request->GetCaptureId(), request->GetEndTime(), request->GetOwnerCount());
        messenger_->SendMessage(shutterMessage);
    }

    std::shared_ptr<ICaptureMessage> endMessage = std::make_shared<CaptureEndedMessage>(
        streamId_, request->GetCaptureId(), request->GetEndTime(), request->GetOwnerCount(), tunnel->GetFrameCount());
    messenger_->SendMessage(endMessage);

    CAMERA_LOGI("offline stream [id:%{public}d] dequeue buffer index:%{public}d, status:%{public}d",
        streamId_, buffer->GetIndex(),
                buffer->GetBufferStatus());
    bufferPool->ReturnBuffer(buffer);
    tunnel->PutBuffer(buffer);

    return RC_OK;
}

bool OfflineStream::CheckCaptureIdExist(int32_t captureId)
{
    std::lock_guard<std::mutex> l(lock_);
    auto it =
        std::find_if(context_->restRequests.begin(), context_->restRequests.end(),
                     [captureId](const std::shared_ptr<CaptureRequest>& r) { return r->GetCaptureId() == captureId; });
    if (it != context_->restRequests.end()) {
        return true;
    }
    return false;
}

void OfflineStream::HandleMessage(MessageGroup& message)
{
    if (message.empty()) {
        return;
    }
    CHECK_IF_PTR_NULL_RETURN_VOID(message[0]);
    CaptureMessageType type = message[0]->GetMessageType();
    switch (type) {
        case CAPTURE_MESSAGE_TYPE_ON_ERROR: {
            std::vector<std::shared_ptr<CaptureErrorInfo>> info = {};
            for (auto cm : message) {
                auto m = std::static_pointer_cast<CaptureErrorMessage>(cm);
                CHECK_IF_PTR_NULL_RETURN_VOID(m);
                auto edi = std::make_shared<CaptureErrorInfo>();
                edi->streamId_ = m->GetStreamId();
                edi->error_ = m->GetStreamError();
                info.push_back(edi);
            }
            OnCaptureError(message[0]->GetCaptureId(), info);
            break;
        }
        case CAPTURE_MESSAGE_TYPE_ON_ENDED: {
            std::vector<std::shared_ptr<CaptureEndedInfo>> info = {};
            for (auto cm : message) {
                auto m = std::static_pointer_cast<CaptureEndedMessage>(cm);
                CHECK_IF_PTR_NULL_RETURN_VOID(m);
                auto edi = std::make_shared<CaptureEndedInfo>();
                edi->streamId_ = m->GetStreamId();
                edi->frameCount_ = (int32_t)(m->GetFrameCount());
                info.push_back(edi);
            }
            OnCaptureEnded(message[0]->GetCaptureId(), info);
            break;
        }
        default:
            break;
    }
    return;
}

void OfflineStream::OnCaptureEnded(int32_t captureId, const std::vector<std::shared_ptr<CaptureEndedInfo>>& infos)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnCaptureEnded(captureId, infos);
}

void OfflineStream::OnCaptureError(int32_t captureId, const std::vector<std::shared_ptr<CaptureErrorInfo>>& infos)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnCaptureError(captureId, infos);
}
} // namespace OHOS::Camera
