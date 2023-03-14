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

#include "stream_base.h"
#include "buffer_adapter.h"
#include "buffer_manager.h"
#include "watchdog.h"

namespace OHOS::Camera {
std::map<StreamIntent, std::string> IStream::g_availableStreamType = {
    {PREVIEW, STREAM_INTENT_TO_STRING(PREVIEW)},
    {VIDEO, STREAM_INTENT_TO_STRING(VIDEO)},
    {STILL_CAPTURE, STREAM_INTENT_TO_STRING(STILL_CAPTURE)},
    {POST_VIEW, STREAM_INTENT_TO_STRING(POST_VIEW)},
    {ANALYZE, STREAM_INTENT_TO_STRING(ANALYZE)},
    {CUSTOM, STREAM_INTENT_TO_STRING(CUSTOM)},
};

StreamBase::StreamBase(const int32_t id,
                       const StreamIntent type,
                       std::shared_ptr<IPipelineCore>& p,
                       std::shared_ptr<CaptureMessageOperator>& m)
{
    streamId_ = id;
    streamType_ = static_cast<int32_t>(type);
    pipelineCore_ = p;
    messenger_ = m;
}

StreamBase::~StreamBase()
{
    if (state_ == STREAM_STATE_BUSY) {
        StopStream();
    }

    if (hostStreamMgr_ != nullptr) {
        hostStreamMgr_->DestroyHostStream({streamId_});
    }

    if (pipeline_ != nullptr) {
        pipeline_->DestroyPipeline({streamId_});
    }
}

RetCode StreamBase::ConfigStream(StreamConfiguration& config)
{
    if (state_ != STREAM_STATE_IDLE) {
        return RC_ERROR;
    }

    streamConfig_ = config;
    streamConfig_.usage = GetUsage();
    if (tunnel_ != nullptr) {
        streamConfig_.tunnelMode = true;
    }
    streamConfig_.bufferCount = GetBufferCount();
    streamConfig_.maxBatchCaptureCount = 1;
    streamConfig_.maxCaptureCount = 1;
    // get device cappability to overide configuration
    return RC_OK;
}

RetCode StreamBase::CommitStream()
{
    if (state_ != STREAM_STATE_IDLE) {
        return RC_ERROR;
    }

    CHECK_IF_PTR_NULL_RETURN_VALUE(pipelineCore_, RC_ERROR);

    pipeline_ = pipelineCore_->GetStreamPipelineCore();
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);

    hostStreamMgr_ = pipelineCore_->GetHostStreamMgr();
    CHECK_IF_PTR_NULL_RETURN_VALUE(hostStreamMgr_, RC_ERROR);

    HostStreamInfo info;
    info.type_ = static_cast<StreamIntent>(streamType_);
    info.streamId_ = streamId_;
    info.width_ = streamConfig_.width;
    info.height_ = streamConfig_.height;
    info.format_ = streamConfig_.format;
    info.usage_ = streamConfig_.usage;
    info.encodeType_ = streamConfig_.encodeType;

    if (streamConfig_.tunnelMode) {
        BufferManager* mgr = BufferManager::GetInstance();
        CHECK_IF_PTR_NULL_RETURN_VALUE(mgr, RC_ERROR);

        if (bufferPool_ == nullptr) {
            poolId_ = mgr->GenerateBufferPoolId();
            CHECK_IF_EQUAL_RETURN_VALUE(poolId_, 0, RC_ERROR);
            bufferPool_ = mgr->GetBufferPool(poolId_);
            if (bufferPool_ == nullptr) {
                CAMERA_LOGE("stream [id:%{public}d] get buffer pool failed.", streamId_);
                return RC_ERROR;
            }
        }

        info.bufferPoolId_ = poolId_;
        info.bufferCount_ = GetBufferCount();
        RetCode rc = bufferPool_->Init(streamConfig_.width, streamConfig_.height, streamConfig_.usage,
                                       streamConfig_.format, GetBufferCount(), CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
        if (rc != RC_OK) {
            CAMERA_LOGE("stream [id:%{public}d] initialize buffer pool failed.", streamId_);
            return RC_ERROR;
        }
    }

    RetCode rc = hostStreamMgr_->CreateHostStream(info, [this](std::shared_ptr<IBuffer> buffer) {
        HandleResult(buffer);
        return;
    });
    if (rc != RC_OK) {
        CAMERA_LOGE("commit stream [id:%{public}d] to pipeline failed.", streamId_);
        return RC_ERROR;
    }
    CAMERA_LOGI("commit a stream to pipeline id[%{public}d], w[%{public}d], h[%{public}d], poolId[%{public}llu], \
        encodeType = %{public}d", info.streamId_, info.width_, info.height_, info.bufferPoolId_, info.encodeType_);
    state_ = STREAM_STATE_ACTIVE;

    return RC_OK;
}

RetCode StreamBase::StartStream()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);

    if (state_ != STREAM_STATE_ACTIVE) {
        return RC_ERROR;
    }

    CAMERA_LOGI("start stream [id:%{public}d] begin", streamId_);
    tunnel_->NotifyStart();

    RetCode rc = pipeline_->Prepare({streamId_});
    if (rc != RC_OK) {
        CAMERA_LOGE("pipeline [id:%{public}d] prepare failed", streamId_);
        return rc;
    }

    state_ = STREAM_STATE_BUSY;
    std::string threadName =
        g_availableStreamType[static_cast<StreamIntent>(streamType_)] + "#" + std::to_string(streamId_);
    handler_ = std::make_unique<std::thread>([this, &threadName] {
        prctl(PR_SET_NAME, threadName.c_str());
        while (state_ == STREAM_STATE_BUSY) {
            HandleRequest();
        }
    });
    if (handler_ == nullptr) {
        state_ = STREAM_STATE_ACTIVE;
        return RC_ERROR;
    }

    rc = pipeline_->Start({streamId_});
    if (rc != RC_OK) {
        CAMERA_LOGE("pipeline [%{public}d] start failed", streamId_);
        return RC_ERROR;
    }
    CAMERA_LOGI("start stream [id:%{public}d] end", streamId_);

    return RC_OK;
}

RetCode StreamBase::StopStream()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);
    if (state_ == STREAM_STATE_IDLE) {
        CAMERA_LOGI("stream [id:%{public}d], no need to stop", streamId_);
        return RC_OK;
    }
    CAMERA_LOGI("stop stream [id:%{public}d] begin", streamId_);

    state_ = STREAM_STATE_IDLE;

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

    CAMERA_LOGI("stream [id:%{public}d] is waiting buffers returned", streamId_);
    tunnel_->WaitForAllBufferReturned();
    CAMERA_LOGI("stream [id:%{public}d], all buffers are returned.", streamId_);

    rc = pipeline_->Stop({streamId_});
    if (rc != RC_OK) {
        CAMERA_LOGE("stream [id:%{public}d], pipeline stop failed", streamId_);
        return RC_ERROR;
    }

    if (lastRequest_->IsContinous() && !inTransitList_.empty()) {
        std::shared_ptr<ICaptureMessage> endMessage =
            std::make_shared<CaptureEndedMessage>(streamId_, lastRequest_->GetCaptureId(),
            lastRequest_->GetEndTime(), lastRequest_->GetOwnerCount(), tunnel_->GetFrameCount());
        CAMERA_LOGV("end of stream [%{public}d], ready to send end message", streamId_);
        messenger_->SendMessage(endMessage);
    }

    CAMERA_LOGI("stop stream [id:%{public}d] end", streamId_);
    isFirstRequest = true;

    inTransitList_.clear();
    tunnel_->CleanBuffers();
    bufferPool_->ClearBuffers();
    return RC_OK;
}

RetCode StreamBase::AddRequest(std::shared_ptr<CaptureRequest>& request)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(request, RC_ERROR);
    request->AddOwner(shared_from_this());

    request->SetFirstRequest(false);
    if (isFirstRequest) {
        RetCode rc = StartStream();
        if (rc != RC_OK) {
            CAMERA_LOGE("start stream [id:%{public}d] failed", streamId_);
            return RC_ERROR;
        }
        request->SetFirstRequest(true);
        isFirstRequest = false;
    }

    {
        std::unique_lock<std::mutex> l(wtLock_);
        waitingList_.emplace_back(request);
        cv_.notify_one();
    }

    return RC_OK;
}

RetCode StreamBase::CancelRequest(const std::shared_ptr<CaptureRequest>& request)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(request, RC_ERROR);

    {
        // We don't care if this request is continious-capture or single-capture, just erase it.
        // And those requests in inTransitList_ removed in HandleResult.
        std::unique_lock<std::mutex> wl(wtLock_);
        auto it = std::find(waitingList_.begin(), waitingList_.end(), request);
        if (it != waitingList_.end()) {
            waitingList_.erase(it);
            CAMERA_LOGI("stream [id:%{public}d], cancel request(capture id:%{public}d) success",
                streamId_, request->GetCaptureId());
        }
    }

    if (request->IsContinous()) {
        // may be this is the last request
        std::unique_lock<std::mutex> tl(tsLock_);
        auto it = std::find(inTransitList_.begin(), inTransitList_.end(), request);
        if (it == inTransitList_.end()) {
            std::shared_ptr<ICaptureMessage> endMessage =
                std::make_shared<CaptureEndedMessage>(streamId_, request->GetCaptureId(), request->GetEndTime(),
                                                      request->GetOwnerCount(), tunnel_->GetFrameCount());
            CAMERA_LOGV("end of stream [%{public}d], ready to send end message", streamId_);
            messenger_->SendMessage(endMessage);
            pipeline_->CancelCapture({streamId_});
        }
    }
    return RC_OK;
}

void StreamBase::HandleRequest()
{
    if (waitingList_.empty()) {
        std::unique_lock<std::mutex> l(wtLock_);
        if (waitingList_.empty()) {
            cv_.wait(l, [this] { return !(state_ == STREAM_STATE_BUSY && waitingList_.empty()); });
        }
    }
    if (state_ != STREAM_STATE_BUSY) {
        return;
    }

    std::shared_ptr<CaptureRequest> request = nullptr;
    {
        // keep a copy of continious-capture in waitingList_, unless it's going to be canceled.
        std::unique_lock<std::mutex> l(wtLock_);
        if (waitingList_.empty()) {
            return;
        }
        request = waitingList_.front();
        CHECK_IF_PTR_NULL_RETURN_VOID(request);
        if (!request->IsContinous()) {
            waitingList_.pop_front();
        }
    }
    if (request == nullptr) {
        CAMERA_LOGE("fatal error, stream [%{public}d] request list is not empty, but can't get one", streamId_);
        return;
    }

    if (request->NeedCancel()) {
        return;
    }

    request->Process(streamId_);

    return;
}

RetCode StreamBase::Capture(const std::shared_ptr<CaptureRequest>& request)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(request, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);

    RetCode rc = RC_ERROR;
    if (request->IsFirstOne() && !request->IsContinous()) {
        uint32_t n = GetBufferCount();
        for (uint32_t i = 0; i < n; i++) {
            DeliverBuffer();
        }
    } else {
        do {
            rc = DeliverBuffer();
        } while (rc != RC_OK && state_ == STREAM_STATE_BUSY);
    }

    if (request->NeedCancel()) {
        CAMERA_LOGE("StreamBase::Capture stream [id:%{public}d] request->NeedCancel", streamId_);
        return RC_OK;
    }

    rc = pipeline_->Config({streamId_}, request->GetCaptureSetting());
    if (rc != RC_OK) {
        CAMERA_LOGE("stream [id:%{public}d] config pipeline failed.", streamId_);
        return RC_ERROR;
    }

    rc = pipeline_->Capture({streamId_}, request->GetCaptureId());
    if (rc != RC_OK) {
        CAMERA_LOGE("stream [id:%{public}d] take a capture failed.", streamId_);
        return RC_ERROR;
    }

    {
        std::unique_lock<std::mutex> l(tsLock_);
        inTransitList_.emplace_back(request);
    }

    if (request->IsFirstOne()) {
        if (messenger_ == nullptr) {
            CAMERA_LOGE("stream [id:%{public}d] can't send message, messenger_ is null", streamId_);
            return RC_ERROR;
        }
        std::shared_ptr<ICaptureMessage> startMessage = std::make_shared<CaptureStartedMessage>(
            streamId_, request->GetCaptureId(), request->GetBeginTime(), request->GetOwnerCount());
        messenger_->SendMessage(startMessage);
        request->SetFirstRequest(false);
    }

    return RC_OK;
}

RetCode StreamBase::DeliverBuffer()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(tunnel_, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferPool_, RC_ERROR);

    std::shared_ptr<IBuffer> buffer = tunnel_->GetBuffer();
    CHECK_IF_PTR_NULL_RETURN_VALUE(buffer, RC_ERROR);

    buffer->SetEncodeType(streamConfig_.encodeType);
    buffer->SetStreamId(streamId_);
    bufferPool_->AddBuffer(buffer);
    CAMERA_LOGI("stream [id:%{public}d] enqueue buffer index:%{public}d", streamId_, buffer->GetIndex());
    return RC_OK;
}

void StreamBase::HandleResult(std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_PTR_NULL_RETURN_VOID(buffer);
    if (buffer->GetBufferStatus() == CAMERA_BUFFER_STATUS_INVALID) {
        CAMERA_LOGI("stream [id:%{public}d], this buffer(index:%{public}d) has nothing to do with request.", streamId_,
                    buffer->GetIndex());
        ReceiveBuffer(buffer);
        return;
    }

    if (buffer->GetStreamId() != streamId_) {
        CAMERA_LOGE("fatal error, stream [%{public}d] reveived a wrong buffer, index:%{public}d. \
            this buffer belongs to stream:%{public}d", streamId_, buffer->GetIndex(), buffer->GetStreamId());
        return;
    }

    int32_t captureId = buffer->GetCaptureId();
    std::shared_ptr<CaptureRequest> request = nullptr;
    {
        std::unique_lock<std::mutex> l(tsLock_);
        for (auto& r : inTransitList_) {
            if (r == nullptr) {
                continue;
            }
            if (r->GetCaptureId() == captureId) {
                request = r;
            }
        }
    }
    if (request == nullptr) {
        CAMERA_LOGI("stream [id:%{public}d], this buffer(index:%{public}d) has nothing to do with request.",
            streamId_, buffer->GetIndex());
        buffer->SetBufferStatus(CAMERA_BUFFER_STATUS_INVALID);
        ReceiveBuffer(buffer);
        return;
    }
    request->AttachBuffer(buffer);
    // To synchronize multiple stream, bottom-layer device stream need be synchronized first.
    request->OnResult(streamId_);
    lastRequest_ = request;
    return;
}

RetCode StreamBase::OnFrame(const std::shared_ptr<CaptureRequest>& request)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(request, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);
    auto buffer = request->GetAttachedBuffer();
    CameraBufferStatus status = buffer->GetBufferStatus();
    if (status != CAMERA_BUFFER_STATUS_OK) {
        if (status != CAMERA_BUFFER_STATUS_DROP) {
            std::shared_ptr<ICaptureMessage> errorMessage =
                std::make_shared<CaptureErrorMessage>(streamId_, request->GetCaptureId(), request->GetEndTime(),
                                                      request->GetOwnerCount(), static_cast<StreamError>(status));
            messenger_->SendMessage(errorMessage);
        }
    }
    if (request->NeedShutterCallback() && messenger_ != nullptr) {
        std::shared_ptr<ICaptureMessage> shutterMessage = std::make_shared<FrameShutterMessage>(
            streamId_, request->GetCaptureId(), request->GetEndTime(), request->GetOwnerCount());
        messenger_->SendMessage(shutterMessage);
    }

    bool isEnded = false;
    if (!request->IsContinous()) {
        isEnded = true;
    } else if (request->NeedCancel()) {
        isEnded = true;
    }

    {
        // inTransitList_ may has multiple copies of continious-capture request, we just need erase one of them.
        std::unique_lock<std::mutex> l(tsLock_);
        for (auto it = inTransitList_.begin(); it != inTransitList_.end(); it++) {
            if ((*it) == request) {
                inTransitList_.erase(it);
                break;
            }
        }

        if (isEnded) {
            // if this is the last request of capture, send CaptureEndedMessage.
            auto it = std::find(inTransitList_.begin(), inTransitList_.end(), request);
            if (it == inTransitList_.end()) {
                std::shared_ptr<ICaptureMessage> endMessage =
                    std::make_shared<CaptureEndedMessage>(streamId_, request->GetCaptureId(), request->GetEndTime(),
                                                          request->GetOwnerCount(), tunnel_->GetFrameCount());
                CAMERA_LOGV("end of stream [%d], ready to send end message, capture id = %d",
                    streamId_, request->GetCaptureId());
                messenger_->SendMessage(endMessage);
                pipeline_->CancelCapture({streamId_});
            }
        }
    }

    ReceiveBuffer(buffer);
    return RC_OK;
}

RetCode StreamBase::ReceiveBuffer(std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(buffer, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(tunnel_, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferPool_, RC_ERROR);

    CAMERA_LOGI("stream [id:%{public}d] dequeue buffer index:%{public}d, status:%{public}d",
        streamId_, buffer->GetIndex(), buffer->GetBufferStatus());
    bufferPool_->ReturnBuffer(buffer);
    tunnel_->PutBuffer(buffer);
    return RC_OK;
}

uint64_t StreamBase::GetFrameCount() const
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(tunnel_, 0);
    return tunnel_->GetFrameCount();
}

RetCode StreamBase::AttachStreamTunnel(std::shared_ptr<StreamTunnel>& tunnel)
{
    if (state_ == STREAM_STATE_BUSY || state_ == STREAM_STATE_OFFLINE) {
        return RC_ERROR;
    }

    tunnel_ = tunnel;
    CHECK_IF_PTR_NULL_RETURN_VALUE(tunnel_, RC_ERROR);
    tunnel_->SetBufferCount(GetBufferCount());
    TunnelConfig config = {(uint32_t)streamConfig_.width, (uint32_t)streamConfig_.height,
        (uint32_t)streamConfig_.format, streamConfig_.usage};
    tunnel_->Config(config);

    streamConfig_.tunnelMode = true;
    return RC_OK;
}

RetCode StreamBase::DetachStreamTunnel()
{
    if (state_ == STREAM_STATE_BUSY || state_ == STREAM_STATE_OFFLINE) {
        return RC_ERROR;
    }

    tunnel_.reset();
    streamConfig_.tunnelMode = false;

    state_ = STREAM_STATE_IDLE;
    return RC_OK;
}

RetCode StreamBase::ChangeToOfflineStream(std::shared_ptr<OfflineStream> offlineStream)
{
    (void)offlineStream;
    return RC_OK;
}

uint64_t StreamBase::GetUsage()
{
    return CAMERA_USAGE_SW_WRITE_OFTEN | CAMERA_USAGE_SW_READ_OFTEN | CAMERA_USAGE_MEM_DMA;
}

uint32_t StreamBase::GetBufferCount()
{
    return 3; // 3: buffer count
}

StreamConfiguration StreamBase::GetStreamAttribute() const
{
    return streamConfig_;
}

int32_t StreamBase::GetStreamId() const
{
    return streamId_;
}

bool StreamBase::IsRunning() const
{
    return state_ == STREAM_STATE_BUSY;
}

bool StreamBase::GetTunnelMode() const
{
    return streamConfig_.tunnelMode;
}
} // namespace OHOS::Camera
