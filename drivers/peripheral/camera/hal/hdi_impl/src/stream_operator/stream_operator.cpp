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

#include "watchdog.h"
#include "stream_operator.h"
#include "buffer_adapter.h"
#include "camera_device_impl.h"
#include <algorithm>
#include <iterator>
#include <set>

namespace OHOS::Camera {
StreamOperator::StreamOperator(const OHOS::sptr<IStreamOperatorCallback>& callback,
                               const std::weak_ptr<CameraDevice>& device)
{
    CAMERA_LOGV("enter");
    callback_ = callback;
    device_ = device;
}

StreamOperator::~StreamOperator()
{
    CAMERA_LOGV("enter");
}

RetCode StreamOperator::Init()
{
    auto dev = std::static_pointer_cast<CameraDeviceImpl>(device_.lock());
    CHECK_IF_PTR_NULL_RETURN_VALUE(dev, RC_ERROR);
    pipelineCore_ = dev->GetPipelineCore();
    if (pipelineCore_ == nullptr) {
        CAMERA_LOGE("get pipeline core failed.");
        return RC_ERROR;
    }

    requestTimeoutCB_ = std::bind(&CameraDeviceImpl::OnRequestTimeout, dev);
    streamPipeline_ = pipelineCore_->GetStreamPipelineCore();
    if (streamPipeline_ == nullptr) {
        CAMERA_LOGE("get stream pipeline core failed.");
        return RC_ERROR;
    }

    RetCode rc = streamPipeline_->Init();
    if (rc != RC_OK) {
        CAMERA_LOGE("stream pipeline core init failed.");
        return RC_ERROR;
    }

    auto cb = [this](MessageGroup& m) { HandleCallbackMessage(m); };
    messenger_ = std::make_shared<CaptureMessageOperator>(cb);
    CHECK_IF_PTR_NULL_RETURN_VALUE(messenger_, RC_ERROR);
    messenger_->StartProcess();

    return RC_OK;
}

CamRetCode StreamOperator::IsStreamsSupported(OperationMode mode,
                                              const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting,
                                              const std::vector<std::shared_ptr<StreamInfo>>& pInfo,
                                              StreamSupportType& type)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(streamPipeline_, DEVICE_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(modeSetting, INVALID_ARGUMENT);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    std::set<int32_t> inputIDSet = {};
    std::vector<int32_t> checkStreamIdVec = {};
    for (auto it : pInfo) {
        CHECK_IF_PTR_NULL_RETURN_VALUE(it, INVALID_ARGUMENT);
        CHECK_IF_NOT_EQUAL_RETURN_VALUE(CheckStreamInfo(it), true, INVALID_ARGUMENT);
        inputIDSet.emplace(it->streamId_);
        checkStreamIdVec.push_back(it->streamId_);
    }
    CHECK_IF_EQUAL_RETURN_VALUE(inputIDSet.empty(), true, INVALID_ARGUMENT);

    auto uniqueIt = std::unique(checkStreamIdVec.begin(), checkStreamIdVec.end());
    if (checkStreamIdVec.size() != (uint32_t)(std::distance(checkStreamIdVec.begin(), uniqueIt))) {
        CAMERA_LOGE("stream id must be unique");
        return INVALID_ARGUMENT;
    }

    DynamicStreamSwitchMode method = CheckStreamsSupported(mode, modeSetting, pInfo);
    if (method == DYNAMIC_STREAM_SWITCH_SUPPORT) {
        type = DYNAMIC_SUPPORTED;
        return NO_ERROR;
    }

    if (method == DYNAMIC_STREAM_SWITCH_NOT_SUPPORT) {
        type = NOT_SUPPORTED;
        return NO_ERROR;
    }

    // change mode need to update pipeline, and caller must restart streams
    if (mode != streamPipeline_->GetCurrentMode()) {
        if (method == DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART) {
            type = RE_CONFIGURED_REQUIRED;
            return NO_ERROR;
        }
        type = NOT_SUPPORTED;
        return NO_ERROR;
    }

    std::set<int32_t> currentIDSet = {};
    {
        std::lock_guard<std::mutex> l(streamLock_);
        for (auto& it : streamMap_) {
            currentIDSet.emplace(it.first);
        }
    }
    // no streams are running
    if (currentIDSet.empty()) {
        if (method == DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART) {
            type = DYNAMIC_SUPPORTED;
            return NO_ERROR;
        }
        type = NOT_SUPPORTED;
        return NO_ERROR;
    }

    // the difference of currentSet from inputIDSet
    std::set<int32_t> cfiSet = {};
    std::set_difference(inputIDSet.begin(), inputIDSet.end(), currentIDSet.begin(), currentIDSet.end(),
                        std::inserter(cfiSet, cfiSet.begin()));

    // the difference of cfiSet from inputIDSet
    std::set<int32_t> expectCurrentSet = {};
    std::set_difference(inputIDSet.begin(), inputIDSet.end(), cfiSet.begin(), cfiSet.end(),
                        std::inserter(expectCurrentSet, expectCurrentSet.begin()));

    bool isEqual =
        std::equal(expectCurrentSet.begin(), expectCurrentSet.end(), currentIDSet.begin(), currentIDSet.end());
    if (isEqual) {
        // currentIDSet is subset of inputIDSet
        if (method == DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART) {
            type = DYNAMIC_SUPPORTED;
            return NO_ERROR;
        }
    } else {
        if (method == DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART) {
            type = RE_CONFIGURED_REQUIRED;
            return NO_ERROR;
        }
    }

    type = NOT_SUPPORTED;
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

DynamicStreamSwitchMode StreamOperator::CheckStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>>& infos)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(streamPipeline_, DYNAMIC_STREAM_SWITCH_NOT_SUPPORT);
    std::vector<StreamConfiguration> configs = {};
    for (auto& it : infos) {
        StreamConfiguration config = {};
        config.type = it->intent_;
        config.width = it->width_;
        config.height = it->height_;
        PixelFormat pf = static_cast<PixelFormat>(it->format_);
        config.format = BufferAdapter::PixelFormatToCameraFormat(pf);
        config.dataspace = it->datasapce_; // fix spell error
        config.tunnelMode = it->tunneledMode_;
        config.minFrameDuration = it->minFrameDuration_;
        config.encodeType = it->encodeType_;
        configs.emplace_back(config);
    }
    // search device capability to check if this configuration is supported.
    return streamPipeline_->CheckStreamsSupported(mode, modeSetting, configs);
}

CamRetCode StreamOperator::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>>& streamInfos)
{
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;
    for (auto it : streamInfos) {
        CHECK_IF_NOT_EQUAL_RETURN_VALUE(CheckStreamInfo(it), true, INVALID_ARGUMENT);
        if (streamMap_.count(it->streamId_) > 0) {
            CAMERA_LOGE("stream [id = %{public}d] has already been created.", it->streamId_);
            return INVALID_ARGUMENT;
        }
        std::shared_ptr<IStream> stream = StreamFactory::Instance().CreateShared(
            IStream::g_availableStreamType[it->intent_], it->streamId_, it->intent_, pipelineCore_, messenger_);
        if (stream == nullptr) {
            CAMERA_LOGE("create stream [id = %{public}d] failed.", it->streamId_);
            return INSUFFICIENT_RESOURCES;
        }
        StreamConfiguration scg;
        scg.id = it->streamId_;
        scg.type = it->intent_;
        scg.width = it->width_;
        scg.height = it->height_;
        PixelFormat pf = static_cast<PixelFormat>(it->format_);
        scg.format = BufferAdapter::PixelFormatToCameraFormat(pf);
        scg.dataspace = it->datasapce_; // fix misspell
        scg.tunnelMode = it->tunneledMode_;
        scg.minFrameDuration = it->minFrameDuration_;
        scg.encodeType = it->encodeType_;

        RetCode rc = stream->ConfigStream(scg);
        if (rc != RC_OK) {
            CAMERA_LOGE("configure stream %{public}d failed", it->streamId_);
            return INVALID_ARGUMENT;
        }
        if (!scg.tunnelMode && it->bufferQueue_ != nullptr) {
            CAMERA_LOGE("stream [id:%{public}d] is not tunnel mode, can't bind a buffer producer", it->streamId_);
            return INVALID_ARGUMENT;
        }
        if (it->bufferQueue_ != nullptr) {
            auto tunnel = std::make_shared<StreamTunnel>();
            CHECK_IF_PTR_NULL_RETURN_VALUE(tunnel, INSUFFICIENT_RESOURCES);
            RetCode rc = tunnel->AttachBufferQueue(it->bufferQueue_);
            CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, INVALID_ARGUMENT);
            if (stream->AttachStreamTunnel(tunnel) != RC_OK) {
                CAMERA_LOGE("attach buffer queue to stream [id = %{public}d] failed", it->streamId_);
                return INVALID_ARGUMENT;
            }
        }
        {
            std::lock_guard<std::mutex> l(streamLock_);
            streamMap_[stream->GetStreamId()] = stream;
        }
        CAMERA_LOGI("create stream success [id:%{public}d] [type:%{public}s]", stream->GetStreamId(),
                    IStream::g_availableStreamType[it->intent_].c_str());
    }
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;
    for (auto id : streamIds) {
        std::lock_guard<std::mutex> l(streamLock_);
        auto it = streamMap_.find(id);
        if (it == streamMap_.end()) {
            continue;
        }
        if (it->second->IsRunning()) {
            it->second->StopStream();
        }
        streamMap_.erase(it);
    }

    for (auto id : streamIds) {
        CHECK_IF_EQUAL_RETURN_VALUE(id < 0, true, INVALID_ARGUMENT);
    }

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}


RetCode StreamOperator::ReleaseStreams()
{
    std::vector<int32_t> ids = {};
    for (auto it : streamMap_) {
        ids.push_back(it.first);
    }
    ReleaseStreams(ids);
    return RC_OK;
}

CamRetCode StreamOperator::CommitStreams(OperationMode mode,
                                         const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting)
{
    CAMERA_LOGV("enter");
    CHECK_IF_PTR_NULL_RETURN_VALUE(streamPipeline_, DEVICE_ERROR);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    std::vector<StreamConfiguration> configs = {};
    {
        std::lock_guard<std::mutex> l(streamLock_);
        for (auto it : streamMap_) {
            configs.emplace_back(it.second->GetStreamAttribute());
        }
    }

    DynamicStreamSwitchMode method = streamPipeline_->CheckStreamsSupported(mode, modeSetting, configs);
    if (method == DYNAMIC_STREAM_SWITCH_NOT_SUPPORT) {
        return INVALID_ARGUMENT;
    }
    if (method == DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART) {
        std::lock_guard<std::mutex> l(streamLock_);
        for (auto it : streamMap_) {
            it.second->StopStream();
        }
    }
    {
        std::lock_guard<std::mutex> l(streamLock_);
        for (auto it : streamMap_) {
            if (it.second->CommitStream() != RC_OK) {
                CAMERA_LOGE("commit stream [id = %{public}d] failed.", it.first);
                return DEVICE_ERROR;
            }
        }
    }
    RetCode rc = streamPipeline_->PreConfig(modeSetting);
    if (rc != RC_OK) {
        CAMERA_LOGE("prepare mode settings failed");
        return DEVICE_ERROR;
    }
    rc = streamPipeline_->CreatePipeline(mode);
    if (rc != RC_OK) {
        CAMERA_LOGE("create pipeline failed.");
        return INVALID_ARGUMENT;
    }

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes)
{
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    attributes.clear();
    for (auto it : streamMap_) {
        auto configuration = it.second->GetStreamAttribute();
        auto attribute = std::make_shared<StreamAttribute>();
        attribute->streamId_ = it.first;
        attribute->width_ = configuration.width;
        attribute->height_ = configuration.height;
        attribute->overrideFormat_ = (int32_t)BufferAdapter::CameraFormatToPixelFormat(configuration.format);
        attribute->overrideDatasapce_ = configuration.dataspace;
        attribute->producerUsage_ = BufferAdapter::CameraUsageToGrallocUsage(configuration.usage);
        attribute->producerBufferCount_ = configuration.bufferCount;
        attribute->maxBatchCaptureCount_ = configuration.maxCaptureCount;
        attribute->maxCaptureCount_ = configuration.maxCaptureCount;
        attributes.emplace_back(attribute);
    }
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer)
{
    CHECK_IF_EQUAL_RETURN_VALUE(streamId < 0, true, INVALID_ARGUMENT);
    CHECK_IF_PTR_NULL_RETURN_VALUE(producer, INVALID_ARGUMENT);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    std::shared_ptr<IStream> stream = nullptr;
    {
        std::lock_guard<std::mutex> l(streamLock_);
        auto it = streamMap_.find(streamId);
        if (it == streamMap_.end()) {
            return INVALID_ARGUMENT;
        }
        stream = it->second;
    }

    CHECK_IF_PTR_NULL_RETURN_VALUE(stream, INVALID_ARGUMENT);
    CHECK_IF_EQUAL_RETURN_VALUE(stream->GetTunnelMode(), false, METHOD_NOT_SUPPORTED);

    auto tunnel = std::make_shared<StreamTunnel>();
    CHECK_IF_EQUAL_RETURN_VALUE(tunnel, nullptr, INSUFFICIENT_RESOURCES);
    auto bufferQueue = const_cast<OHOS::sptr<OHOS::IBufferProducer>&>(producer);
    RetCode rc = tunnel->AttachBufferQueue(bufferQueue);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, INVALID_ARGUMENT);

    rc = stream->AttachStreamTunnel(tunnel);
    if (rc != RC_OK) {
        CAMERA_LOGE("attach buffer queue to stream [id = %{public}d] failed", streamId);
        return CAMERA_BUSY;
    }
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::DetachBufferQueue(int streamId)
{
    CHECK_IF_EQUAL_RETURN_VALUE(streamId < 0, true, INVALID_ARGUMENT);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    std::shared_ptr<IStream> stream = nullptr;
    {
        std::lock_guard<std::mutex> l(streamLock_);
        auto it = streamMap_.find(streamId);
        if (it == streamMap_.end()) {
            return INVALID_ARGUMENT;
        }
        stream = it->second;
    }

    CHECK_IF_PTR_NULL_RETURN_VALUE(stream, INVALID_ARGUMENT);
    CHECK_IF_EQUAL_RETURN_VALUE(stream->GetTunnelMode(), false, METHOD_NOT_SUPPORTED);
    RetCode rc = RC_ERROR;
    if (stream->IsRunning()) {
        rc = stream->StopStream();
        CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, DEVICE_ERROR);
    }

    rc = stream->DetachStreamTunnel();
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, DEVICE_ERROR);

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::Capture(int captureId, const std::shared_ptr<CaptureInfo>& captureInfo, bool isStreaming)
{
    CHECK_IF_EQUAL_RETURN_VALUE(captureId < 0, true, INVALID_ARGUMENT);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    for (auto id : captureInfo->streamIds_) {
        std::lock_guard<std::mutex> l(streamLock_);
        auto it = streamMap_.find(id);
        if (it == streamMap_.end()) {
            return INVALID_ARGUMENT;
        }
    }

    {
        std::lock_guard<std::mutex> l(requestLock_);
        auto itr = requestMap_.find(captureId);
        if (itr != requestMap_.end()) {
            return INVALID_ARGUMENT;
        }
    }

    CaptureSetting setting = captureInfo->captureSetting_;
    auto request =
        std::make_shared<CaptureRequest>(captureId, captureInfo->streamIds_.size(), setting,
                                         captureInfo->enableShutterCallback_, isStreaming);
    for (auto id : captureInfo->streamIds_) {
        RetCode rc = streamMap_[id]->AddRequest(request);
        if (rc != RC_OK) {
            return DEVICE_ERROR;
        }
    }

    {
        std::lock_guard<std::mutex> l(requestLock_);
        requestMap_[captureId] = request;
    }
    return NO_ERROR;
}

CamRetCode StreamOperator::CancelCapture(int captureId)
{
    CHECK_IF_EQUAL_RETURN_VALUE(captureId < 0, true, INVALID_ARGUMENT);
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;

    std::lock_guard<std::mutex> l(requestLock_);
    auto itr = requestMap_.find(captureId);
    if (itr == requestMap_.end()) {
        CAMERA_LOGE("can't cancel capture [id = %{public}d], this capture doesn't exist", captureId);
        return INVALID_ARGUMENT;
    }

    RetCode rc = itr->second->Cancel();
    if (rc != RC_OK) {
        return DEVICE_ERROR;
    }
    requestMap_.erase(itr);

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode StreamOperator::ChangeToOfflineStream(const std::vector<int>& streamIds,
                                                 OHOS::sptr<IStreamOperatorCallback>& callback,
                                                 OHOS::sptr<IOfflineStreamOperator>& offlineOperator)
{
    PLACE_A_NOKILL_WATCHDOG(requestTimeoutCB_);
    DFX_LOCAL_HITRACE_BEGIN;
    CHECK_IF_PTR_NULL_RETURN_VALUE(callback, INVALID_ARGUMENT);
    // offlineOperator should not be null
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(offlineOperator, nullptr, INVALID_ARGUMENT);
    CHECK_IF_EQUAL_RETURN_VALUE(streamIds.empty(), true, INVALID_ARGUMENT);

#ifdef CAMERA_BUILT_ON_OHOS_LITE
    oflstor_ = std::make_shared<OfflineStreamOperator>();
#else
    oflstor_ = new (std::nothrow) OfflineStreamOperator();
#endif
    CHECK_IF_PTR_NULL_RETURN_VALUE(oflstor_, INSUFFICIENT_RESOURCES);

    for (auto it : streamIds) {
        CHECK_IF_EQUAL_RETURN_VALUE(it < 0, true, INVALID_ARGUMENT);
        std::lock_guard<std::mutex> l(streamLock_);
        auto streamIt = streamMap_.find(it);
        if (streamIt == streamMap_.end()) {
            CAMERA_LOGE("stream id %{public}d doesn't exist, change to offline mode failed.", it);
            return INVALID_ARGUMENT;
        }
        // only still-capture stream can be changed to offline mode
        if (streamMap_[it]->GetStreamAttribute().type != STILL_CAPTURE) {
            return METHOD_NOT_SUPPORTED;
        }

        auto offlineStream = std::make_shared<OfflineStream>(it, callback);
        CHECK_IF_PTR_NULL_RETURN_VALUE(offlineStream, INSUFFICIENT_RESOURCES);

        RetCode rc = streamMap_[it]->ChangeToOfflineStream(offlineStream);
        if (rc != RC_OK) {
            CAMERA_LOGE("stream id %{public}d change to offline mode failed.", it);
            return DEVICE_ERROR;
        }
        rc = oflstor_->CommitOfflineStream(offlineStream);
        if (rc != RC_OK) {
            CAMERA_LOGE("stream id %{public}d, commit offline stream failed.", it);
            return DEVICE_ERROR;
        }
        CAMERA_LOGI("stream %{public}d switch to offline success.", it);
    }

    offlineOperator = oflstor_;
    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

bool StreamOperator::CheckStreamInfo(const std::shared_ptr<StreamInfo>& streamInfo)
{
    if (streamInfo->streamId_ < 0 || streamInfo->width_ < 0 || streamInfo->height_ < 0 || streamInfo->format_ < 0 ||
        streamInfo->datasapce_ < 0 || streamInfo->intent_ > CUSTOM || streamInfo->intent_ < PREVIEW ||
        streamInfo->minFrameDuration_ < 0) {
        return false;
    }
    return true;
}

void StreamOperator::HandleCallbackMessage(MessageGroup& message)
{
    if (message.empty()) {
        return;
    }
    CHECK_IF_PTR_NULL_RETURN_VOID(message[0]);
    CaptureMessageType type = message[0]->GetMessageType();
    switch (type) {
        case CAPTURE_MESSAGE_TYPE_ON_STARTED: {
            std::vector<int32_t> ids = {};
            for (auto cm : message) {
                auto m = std::static_pointer_cast<CaptureStartedMessage>(cm);
                CHECK_IF_PTR_NULL_RETURN_VOID(m);
                ids.push_back(m->GetStreamId());
            }
            OnCaptureStarted(message[0]->GetCaptureId(), ids);
            break;
        }
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
                edi->frameCount_ = m->GetFrameCount();
                info.push_back(edi);
            }
            OnCaptureEnded(message[0]->GetCaptureId(), info);
            break;
        }
        case CAPTURE_MESSAGE_TYPE_ON_SHUTTER: {
            std::vector<int32_t> ids = {};
            for (auto cm : message) {
                auto m = std::static_pointer_cast<FrameShutterMessage>(cm);
                CHECK_IF_PTR_NULL_RETURN_VOID(m);
                ids.push_back(m->GetStreamId());
            }
            OnFrameShutter(message[0]->GetCaptureId(), ids, message[0]->GetTimestamp());
            break;
        }
        default:
            break;
    }
    return;
}

void StreamOperator::OnCaptureStarted(int32_t captureId, const std::vector<int32_t>& streamIds)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnCaptureStarted(captureId, streamIds);
}

void StreamOperator::OnCaptureEnded(int32_t captureId, const std::vector<std::shared_ptr<CaptureEndedInfo>>& infos)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnCaptureEnded(captureId, infos);

    std::lock_guard<std::mutex> l(requestLock_);
    auto itr = requestMap_.find(captureId);
    if (itr == requestMap_.end()) {
        return;
    }
    requestMap_.erase(itr);
}

void StreamOperator::OnCaptureError(int32_t captureId, const std::vector<std::shared_ptr<CaptureErrorInfo>>& infos)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnCaptureError(captureId, infos);
}

void StreamOperator::OnFrameShutter(int32_t captureId, const std::vector<int32_t>& streamIds, uint64_t timestamp)
{
    CHECK_IF_EQUAL_RETURN_VOID(callback_, nullptr);
    callback_->OnFrameShutter(captureId, streamIds, timestamp);
}
} // end namespace OHOS::Camera
