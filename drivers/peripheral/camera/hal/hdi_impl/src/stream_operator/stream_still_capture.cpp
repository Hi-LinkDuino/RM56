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

#include "stream_still_capture.h"
#include "offline_stream.h"

namespace OHOS::Camera {
StreamStillCapture::StreamStillCapture(const int32_t id,
                                       const StreamIntent type,
                                       std::shared_ptr<IPipelineCore>& p,
                                       std::shared_ptr<CaptureMessageOperator>& m)
    : StreamBase(id, type, p, m)
{
    CAMERA_LOGV("enter");
}

StreamStillCapture::~StreamStillCapture()
{
    CAMERA_LOGV("enter");
}

void StreamStillCapture::HandleResult(std::shared_ptr<IBuffer>& buffer)
{
    if (state_ == STREAM_STATE_OFFLINE) {
        std::lock_guard<std::mutex> l(offlineLock_);
        auto stream = offlineStream.lock();
        if (stream == nullptr) {
            return;
        }
        stream->ReceiveOfflineBuffer(buffer);
    }

    StreamBase::HandleResult(buffer);
    return;
}

RetCode StreamStillCapture::Capture(const std::shared_ptr<CaptureRequest>& request)
{
    std::lock_guard<std::mutex> l(offlineLock_);
    if (state_ == STREAM_STATE_OFFLINE) {
        return RC_OK;
    }

    return StreamBase::Capture(request);
}

RetCode StreamStillCapture::ChangeToOfflineStream(std::shared_ptr<OfflineStream> offlineStream)
{
    auto context = std::make_shared<OfflineStreamContext>();
    CHECK_IF_PTR_NULL_RETURN_VALUE(context, RC_ERROR);
    context->streamInfo = streamConfig_;
    context->tunnel = tunnel_;
    CHECK_IF_PTR_NULL_RETURN_VALUE(context->tunnel, RC_ERROR);
    context->bufferPool = bufferPool_;
    CHECK_IF_PTR_NULL_RETURN_VALUE(context->bufferPool, RC_ERROR);
    context->pipeline = pipeline_;
    CHECK_IF_PTR_NULL_RETURN_VALUE(context->pipeline.lock(), RC_ERROR);

    {
        std::unique_lock<std::mutex> l(wtLock_);
        waitingList_.clear();
    }

    std::lock_guard<std::mutex> l(offlineLock_);
    {
        std::lock_guard<std::mutex> l(tsLock_);
        context->restRequests = inTransitList_;
        state_ = STREAM_STATE_OFFLINE;
        CAMERA_LOGI("there is/are %{public}u request(s) left in stream %{public}d.",
            context->restRequests.size(), streamId_);
    }

    RetCode rc = offlineStream->Init(context);
    if (rc != RC_OK) {
        CAMERA_LOGE("offline stream [id:%{public}d] init failed", streamId_);
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode StreamStillCapture::StopStream()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);
    if (state_ == STREAM_STATE_IDLE) {
        CAMERA_LOGI("stream [id:%{public}d], no need to stop", streamId_);
        return RC_OK;
    }
    CAMERA_LOGI("stop stream [id:%{public}d] begin", streamId_);

    if (state_ != STREAM_STATE_OFFLINE) {
        state_ = STREAM_STATE_IDLE;
    }

    tunnel_->NotifyStop();
    cv_.notify_one();
    if (handler_ != nullptr) {
        handler_->join();
    }

    if (!waitingList_.empty()) {
        auto request = waitingList_.front();
        if (request != nullptr && request->IsContinous()) {
            request->Cancel();
        }
    }
    {
        std::unique_lock<std::mutex> l(wtLock_);
        waitingList_.clear();
    }

    RetCode rc = pipeline_->Flush({streamId_});
    if (rc != RC_OK) {
        CAMERA_LOGE("stream [id:%{public}d], pipeline flush failed", streamId_);
        return RC_ERROR;
    }

    if (state_ != STREAM_STATE_OFFLINE) {
        CAMERA_LOGI("stream [id:%{public}d] is waiting buffers returned", streamId_);
        tunnel_->WaitForAllBufferReturned();
        CAMERA_LOGI("stream [id:%{public}d], all buffers are returned.", streamId_);
    }

    rc = pipeline_->Stop({streamId_});
    if (rc != RC_OK) {
        CAMERA_LOGE("stream [id:%{public}d], pipeline stop failed", streamId_);
        return RC_ERROR;
    }

    CAMERA_LOGI("stop stream [id:%{public}d] end", streamId_);
    isFirstRequest = true;

    if (state_ != STREAM_STATE_OFFLINE) {
        inTransitList_.clear();
        tunnel_->CleanBuffers();
        bufferPool_->ClearBuffers();
    }
    return RC_OK;
}

bool StreamStillCapture::IsRunning() const
{
    return state_ == STREAM_STATE_BUSY || state_ == STREAM_STATE_OFFLINE;
}

REGISTERSTREAM(StreamStillCapture, {"STILL_CAPTURE"});
} // namespace OHOS::Camera
