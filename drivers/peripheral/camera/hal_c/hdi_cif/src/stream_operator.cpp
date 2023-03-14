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

#include "stream_operator.h"
#include "buffer_producer_wrapper.h"
#include "offline_stream_operator.h"
#include "stream_operator_callback_wrapper.h"

namespace OHOS::Camera {
StreamOperator::StreamOperator() {}

StreamOperator::~StreamOperator() {}

void StreamOperator::Init(StreamOperatorCIF* op)
{
    operator_ = op;
}

CamRetCode StreamOperator::IsStreamsSupported(OperationMode mode,
                                              const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting,
                                              const std::shared_ptr<StreamInfo>& pInfo,
                                              StreamSupportType& pType)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    StreamInfoCIF si;
    si.streamId = pInfo->streamId_;
    si.width = pInfo->width_;
    si.height = pInfo->height_;
    si.format = pInfo->format_;
    si.datasapce = pInfo->datasapce_;
    si.intent = pInfo->intent_;
    si.tunneledMode = pInfo->tunneledMode_;
    si.minFrameDuration = pInfo->minFrameDuration_;

    BufferProducerCIF producer;
    BindBufferProducer(pInfo->streamId_, pInfo->bufferQueue_);
    producer.RequestBuffer = BufferProducerRequestBuffer;
    producer.CancelBuffer = BufferProducerCancelBuffer;
    producer.FlushBuffer = BufferProducerFlushBuffer;
    producer.GetQueueSize = BufferProducerGetQueueSize;
    producer.SetQueueSize = BufferProducerSetQueueSize;
    producer.DetachBufferProducer = BufferProducerDetachBufferProducer;
    si.producer = producer;

    CameraMetadataCIF* meta = modeSetting->get();

    int type = -1;
    int ret = operator_->IsStreamSupported(static_cast<int>(mode), meta, si, &type);
    pType = static_cast<StreamSupportType>(ret);

    return static_cast<CamRetCode>(pType);
}

CamRetCode StreamOperator::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>>& streamInfos)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = streamInfos.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    StreamInfoCIF* sis = new StreamInfoCIF[count];
    if (sis == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    for (int i = 0; i < count; i++) {
        sis[i].streamId = streamInfos[i]->streamId_;
        sis[i].width = streamInfos[i]->width_;
        sis[i].height = streamInfos[i]->height_;
        sis[i].format = streamInfos[i]->format_;
        sis[i].datasapce = streamInfos[i]->datasapce_;
        sis[i].intent = streamInfos[i]->intent_;
        sis[i].tunneledMode = streamInfos[i]->tunneledMode_;
        sis[i].minFrameDuration = streamInfos[i]->minFrameDuration_;

        BufferProducerCIF producer;
        BindBufferProducer(streamInfos[i]->streamId_, streamInfos[i]->bufferQueue_);
        producer.RequestBuffer = BufferProducerRequestBuffer;
        producer.CancelBuffer = BufferProducerCancelBuffer;
        producer.FlushBuffer = BufferProducerFlushBuffer;
        producer.GetQueueSize = BufferProducerGetQueueSize;
        producer.SetQueueSize = BufferProducerSetQueueSize;
        producer.DetachBufferProducer = BufferProducerDetachBufferProducer;
        sis[i].producer = producer;
    }

    int ret = operator_->CreateStreams(sis, count);
    if (sis != nullptr) {
        delete[] sis;
    }
    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = streamIds.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    int* ids = new int[count];
    if (ids == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    for (int i = 0; i < count; i++) {
        ids[i] = streamIds[i];
    }

    int ret = operator_->ReleaseStreams(ids, count);

    if (ids != nullptr) {
        delete[] ids;
    }
    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::CommitStreams(OperationMode mode, const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    CameraMetadataCIF* meta = modeSetting->get();
    int ret = operator_->CommitStreams(static_cast<int>(mode), meta);

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = 0;
    StreamAttributeCIF* sa = nullptr;
    int ret = operator_->GetStreamAttributes(&sa, &count);
    if (ret != NO_ERROR || sa == nullptr || count == 0) {
        return DEVICE_ERROR;
    }

    for (int i = 0; i < count; i++) {
        std::shared_ptr<StreamAttribute> it = std::make_shared<StreamAttribute>();
        if (it == nullptr) {
            return INSUFFICIENT_RESOURCES;
        }
        it->streamId_ = sa[i].streamId;
        it->width_ = sa[i].width;
        it->height_ = sa[i].height;
        it->overrideFormat_ = sa[i].overrideFormat;
        it->overrideDatasapce_ = sa[i].overrideDatasapce;
        it->producerUsage_ = sa[i].producerUsage;
        it->producerBufferCount_ = sa[i].producerBufferCount;
        it->maxBatchCaptureCount_ = sa[i].maxBatchCaptureCount;
        it->maxCaptureCount_ = sa[i].maxCaptureCount;
        attributes.push_back(it);
    }

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    BufferProducerCIF bufferQueue;
    BindBufferProducer(streamId, const_cast<OHOS::sptr<OHOS::IBufferProducer>&>(producer));
    bufferQueue.RequestBuffer = BufferProducerRequestBuffer;
    bufferQueue.CancelBuffer = BufferProducerCancelBuffer;
    bufferQueue.FlushBuffer = BufferProducerFlushBuffer;
    bufferQueue.GetQueueSize = BufferProducerGetQueueSize;
    bufferQueue.SetQueueSize = BufferProducerSetQueueSize;
    bufferQueue.DetachBufferProducer = BufferProducerDetachBufferProducer;

    int ret = operator_->AttachBufferQueue(streamId, bufferQueue);

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::DetachBufferQueue(int streamId)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int ret = operator_->DetachBufferQueue(streamId);

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::Capture(int captureId, const std::shared_ptr<CaptureInfo>& pInfo, bool isStreaming)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    if (pInfo == nullptr) {
        return NO_ERROR;
    }

    int count = pInfo->streamIds_.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    int* ids = new int[count];
    if (ids == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    CaptureInfoCIF info;
    info.streamIds = ids;
    info.count = count;
    // info.captureSetting
    info.enableShutterCallback = pInfo->enableShutterCallback_;

    int ret = operator_->Capture(captureId, info, static_cast<int>(isStreaming));

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::CancelCapture(int captureId)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int ret = operator_->CancelCapture(captureId);

    return static_cast<CamRetCode>(ret);
}

CamRetCode StreamOperator::ChangeToOfflineStream(const std::vector<int>& streamIds,
                                                 OHOS::sptr<IStreamOperatorCallback>& callback,
                                                 OHOS::sptr<IOfflineStreamOperator>& offlineOperator)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    BindStreamOperatorCallback(callback);

    StreamOperatorCallbackCIF cb;
    cb.OnCaptureStarted = StreamCBOnCaptureStarted;
    cb.OnCaptureEnded = StreamCBOnCaptureEnded;
    cb.OnCaptureError = StreamCBOnCaptureError;
    cb.OnFrameShutter = StreamCBOnFrameShutter;

    int count = streamIds.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    int* ids = new int[count];
    if (ids == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    for (int i = 0; i < count; i++) {
        ids[i] = streamIds[i];
    }

    OfflineStreamOperatorCIF* op = nullptr;
    int ret = operator_->ChangeToOfflineStream(ids, count, cb, op);
    auto os = new OfflineStreamOperator();
    os->Init(op);

    if (ids != nullptr) {
        delete[] ids;
    }

    return static_cast<CamRetCode>(ret);
}

} // end namespace OHOS::Camera
