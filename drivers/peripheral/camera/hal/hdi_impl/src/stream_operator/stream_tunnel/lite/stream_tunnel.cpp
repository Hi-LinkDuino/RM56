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
#include "stream_tunnel.h"
#include "buffer_adapter.h"
#include "image_buffer.h"
#include "video_key_info.h"

namespace {
constexpr uint32_t STRIDE_ALIGNMENT = 8;
} // namespace

namespace OHOS::Camera {
StreamTunnel::~StreamTunnel()
{
    CAMERA_LOGV("enter");
    DetachBufferQueue();
}

RetCode StreamTunnel::AttachBufferQueue(std::shared_ptr<OHOS::Surface>& producer)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(producer, RC_ERROR);
    bufferQueue_ = producer;
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferQueue_, RC_ERROR);
    return RC_OK;
}

RetCode StreamTunnel::DetachBufferQueue()
{
    bufferQueue_ = nullptr;
    return RC_OK;
}

void StreamTunnel::CleanBuffers()
{
    if (stop_ == false) {
        return;
    }
    wakeup_ = true;
    waitCV_.notify_one();

    std::lock_guard<std::mutex> l(lock_);
    buffers.clear();
    index = -1;
}

std::shared_ptr<IBuffer> StreamTunnel::GetBuffer()
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferQueue_, nullptr);
    OHOS::SurfaceBuffer *sb = nullptr;
    do {
        sb = bufferQueue_->RequestBuffer();
        if (sb == nullptr) {
            std::unique_lock<std::mutex> l(waitLock_);
            waitCV_.wait(l, [this] { return wakeup_ == true; });
        }
    } while (!stop_ && sb == nullptr);
    wakeup_ = false;

    if (stop_) {
        if (sb != nullptr) {
            bufferQueue_->CancelBuffer(sb);
        }
        return nullptr;
    }

    std::shared_ptr<IBuffer> cb = nullptr;
    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto it = buffers.begin(); it != buffers.end(); it++) {
            if (it->second == sb) {
                cb = it->first;
            }
        }
    }
    if (cb == nullptr) {
        cb = std::make_shared<ImageBuffer>(CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
        RetCode rc = BufferAdapter::SurfaceBufferToCameraBuffer(sb, bufferQueue_, cb);
        if (rc != RC_OK || cb == nullptr) {
            CAMERA_LOGE("create tunnel buffer failed.");
            return nullptr;
        }

        cb->SetIndex(++index);
        {
            std::lock_guard<std::mutex> l(lock_);
            buffers[cb] = sb;
        }
    } else {
        cb->SetBufferStatus(CAMERA_BUFFER_STATUS_OK);
    }
    restBuffers++;
    return cb;
}

RetCode StreamTunnel::PutBuffer(const std::shared_ptr<IBuffer>& buffer)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(buffer, RC_ERROR);
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferQueue_, RC_ERROR);
    OHOS::SurfaceBuffer *sb = nullptr;
    {
        std::lock_guard<std::mutex> l(lock_);
        auto it = buffers.find(buffer);
        if (it == buffers.end()) {
            CAMERA_LOGE("buffer [%{public}d] doesn't belong to this tunnel.", buffer->GetIndex());
            return RC_ERROR;
        }
        sb = it->second;
    }

    if (buffer->GetBufferStatus() == CAMERA_BUFFER_STATUS_OK) {
        EsFrameInfo esInfo = buffer->GetEsFrameInfo();
        if (esInfo.size != -1 && esInfo.timestamp != -1) {
            sb->SetInt32(OHOS::Camera::VIDEO_KEY_INFO_DATA_SIZE, esInfo.size);
            sb->SetInt32(OHOS::Camera::VIDEO_KEY_INFO_IS_KEY_FRAME, esInfo.isKey);
            sb->SetInt64(OHOS::Camera::VIDEO_KEY_INFO_TIMESTAMP, esInfo.timestamp);
        }
        bufferQueue_->FlushBuffer(sb);
        frameCount_++;
    } else {
        bufferQueue_->CancelBuffer(sb);
    }

    {
        restBuffers--;
        std::unique_lock<std::mutex> l(finishLock_);
        finishCV_.notify_all();
    }
    {
        std::unique_lock<std::mutex> l(waitLock_);
        wakeup_ = true;
        waitCV_.notify_one();
    }
    return RC_OK;
}

RetCode StreamTunnel::SetBufferCount(const int32_t n)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(bufferQueue_, RC_ERROR);
    bufferQueue_->SetQueueSize(n);
    return RC_OK;
}

RetCode StreamTunnel::Config(const TunnelConfig& config)
{
    bufferQueue_->SetWidthAndHeight(config.width, config.height);
    bufferQueue_->SetFormat(BufferAdapter::CameraFormatToPixelFormat(config.format));
    bufferQueue_->SetStrideAlignment(STRIDE_ALIGNMENT);

    return RC_OK;
}

uint64_t StreamTunnel::GetFrameCount() const
{
    return frameCount_;
}

void StreamTunnel::NotifyStop()
{
    std::unique_lock<std::mutex> l(waitLock_);
    wakeup_ = true;
    stop_ = true;
    waitCV_.notify_one();
}

void StreamTunnel::NotifyStart()
{
    stop_ = false;
}

void StreamTunnel::WaitForAllBufferReturned()
{
    std::unique_lock<std::mutex> l(finishLock_);
    finishCV_.wait(l, [this] {
        return restBuffers == 0;
        });

    return;
}
} // namespace OHOS::Camera
