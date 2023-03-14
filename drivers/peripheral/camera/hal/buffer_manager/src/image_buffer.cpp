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

#include "image_buffer.h"

namespace OHOS::Camera {
ImageBuffer::ImageBuffer() {}

ImageBuffer::ImageBuffer(const int32_t source)
{
    sourceType_ = source;
}

ImageBuffer::ImageBuffer(const int32_t source,
                         const uint32_t width,
                         const uint32_t height,
                         const uint64_t usage,
                         const uint32_t format)
{
    sourceType_ = source;
    width_ = width;
    height_ = height;
    usage_ = usage;
    format_ = format;
}

ImageBuffer::~ImageBuffer()
{
    Free();
}

int32_t ImageBuffer::GetIndex() const
{
    return index_;
}

uint32_t ImageBuffer::GetWidth() const
{
    return width_;
}

uint32_t ImageBuffer::GetHeight() const
{
    return height_;
}

uint32_t ImageBuffer::GetStride() const
{
    return stride_;
}

int32_t ImageBuffer::GetFormat() const
{
    return format_;
}

uint32_t ImageBuffer::GetSize() const
{
    return size_;
}

uint64_t ImageBuffer::GetUsage() const
{
    return usage_;
}

void* ImageBuffer::GetVirAddress() const
{
    return virAddr_;
}

uint64_t ImageBuffer::GetPhyAddress() const
{
    return phyAddr_;
}

int32_t ImageBuffer::GetFileDescriptor() const
{
    return fd_;
}

int32_t ImageBuffer::GetSourceType() const
{
    return sourceType_;
}

uint64_t ImageBuffer::GetTimestamp() const
{
    return timeStamp_;
}

uint64_t ImageBuffer::GetFrameNumber() const
{
    return frameNumber_;
}

int64_t ImageBuffer::GetPoolId() const
{
    return poolId_;
}

int32_t ImageBuffer::GetCaptureId() const
{
    return captureId_;
}

CameraBufferStatus ImageBuffer::GetBufferStatus() const
{
    return status_;
}

int32_t ImageBuffer::GetSequenceId() const
{
    return sequenceId_;
}

int32_t ImageBuffer::GetFenceId() const
{
    return fenceId_;
}

EsFrameInfo ImageBuffer::GetEsFrameInfo() const
{
    return esInfo_;
}

int32_t ImageBuffer::GetEncodeType() const
{
    return encodeType_;
}

int32_t ImageBuffer::GetStreamId() const
{
    return streamId_;
}

void ImageBuffer::SetIndex(const int32_t index)
{
    std::lock_guard<std::mutex> l(l_);
    index_ = index;
    return;
}

void ImageBuffer::SetWidth(const uint32_t width)
{
    std::lock_guard<std::mutex> l(l_);
    width_ = width;
    return;
}

void ImageBuffer::SetHeight(const uint32_t height)
{
    std::lock_guard<std::mutex> l(l_);
    height_ = height;
    return;
}

void ImageBuffer::SetStride(const uint32_t stride)
{
    std::lock_guard<std::mutex> l(l_);
    stride_ = stride;
    return;
}

void ImageBuffer::SetFormat(const int32_t format)
{
    std::lock_guard<std::mutex> l(l_);
    format_ = format;
    return;
}

void ImageBuffer::SetSize(const uint32_t size)
{
    std::lock_guard<std::mutex> l(l_);
    size_ = size;
    return;
}

void ImageBuffer::SetUsage(const uint64_t usage)
{
    std::lock_guard<std::mutex> l(l_);
    usage_ = usage;
    return;
}

void ImageBuffer::SetVirAddress(const void* addr)
{
    std::lock_guard<std::mutex> l(l_);
    virAddr_ = const_cast<void*>(addr);
    return;
}

void ImageBuffer::SetPhyAddress(const uint64_t addr)
{
    std::lock_guard<std::mutex> l(l_);
    phyAddr_ = addr;
    return;
}

void ImageBuffer::SetFileDescriptor(const int32_t fd)
{
    std::lock_guard<std::mutex> l(l_);
    fd_ = fd;
    return;
}

void ImageBuffer::SetTimestamp(const uint64_t timeStamp)
{
    std::lock_guard<std::mutex> l(l_);
    timeStamp_ = timeStamp;
    return;
}

void ImageBuffer::SetFrameNumber(const uint64_t frameNumber)
{
    std::lock_guard<std::mutex> l(l_);
    frameNumber_ = frameNumber;
    return;
}

void ImageBuffer::SetPoolId(const int64_t id)
{
    std::lock_guard<std::mutex> l(l_);
    poolId_ = id;
    return;
}

void ImageBuffer::SetCaptureId(const int32_t id)
{
    std::lock_guard<std::mutex> l(l_);
    captureId_ = id;
    return;
}

void ImageBuffer::SetBufferStatus(const CameraBufferStatus flag)
{
    std::lock_guard<std::mutex> l(l_);
    status_ = flag;
    return;
}

void ImageBuffer::SetSequenceId(const int32_t sequence)
{
    std::lock_guard<std::mutex> l(l_);
    sequenceId_ = sequence;
    return;
}

void ImageBuffer::SetFenceId(const int32_t fence)
{
    std::lock_guard<std::mutex> l(l_);
    fenceId_ = fence;
    return;
}

void ImageBuffer::SetEsFrameSize(const int32_t frameSize)
{
    std::lock_guard<std::mutex> l(l_);
    esInfo_.size = frameSize;
    return;
}

void ImageBuffer::SetEsTimestamp(const int64_t timeStamp)
{
    std::lock_guard<std::mutex> l(l_);
    esInfo_.timestamp = timeStamp;
    return;
}

void ImageBuffer::SetEsKeyFrame(const int32_t isKey)
{
    std::lock_guard<std::mutex> l(l_);
    esInfo_.isKey = isKey;
    return;
}

void ImageBuffer::SetEsFrameNum(const int32_t frameNum)
{
    std::lock_guard<std::mutex> l(l_);
    esInfo_.frameNum = frameNum;
    return;
}

void ImageBuffer::SetEncodeType(const int32_t type)
{
    std::lock_guard<std::mutex> l(l_);
    encodeType_ = type;
    return;
}

void ImageBuffer::SetStreamId(const int32_t streamId)
{
    std::lock_guard<std::mutex> l(l_);
    streamId_ = streamId;
    return;
}

void ImageBuffer::Free()
{
    index_ = -1;
    width_ = 0;
    height_ = 0;
    stride_ = 0;
    format_ = CAMERA_FORMAT_INVALID;
    size_ = 0;
    usage_ = 0;
    virAddr_ = nullptr;
    phyAddr_ = 0;
    fd_ = -1;

    return;
}

bool ImageBuffer::operator==(const IBuffer& u)
{
    if (u.GetSourceType() != sourceType_) {
        return false;
    }

    if (u.GetPhyAddress() == 0 || phyAddr_ == 0) {
        return u.GetVirAddress() == virAddr_;
    }

    return u.GetPhyAddress() == phyAddr_;
}
} // namespace OHOS::Camera
