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

#ifndef HOS_CAMERA_IBUFFER_H
#define HOS_CAMERA_IBUFFER_H

#include "camera.h"

namespace OHOS {
namespace Camera {
enum CameraBufferStatus {
    CAMERA_BUFFER_STATUS_OK = 0,
    CAMERA_BUFFER_STATUS_DROP,
    CAMERA_BUFFER_STATUS_INVALID,
};

class IBuffer {
public:
    virtual ~IBuffer(){};

    virtual int32_t GetIndex() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetStride() const = 0;
    virtual int32_t GetFormat() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual uint64_t GetUsage() const = 0;
    virtual void* GetVirAddress() const = 0;
    virtual uint64_t GetPhyAddress() const = 0;
    virtual int32_t GetFileDescriptor() const = 0;
    virtual int32_t GetSourceType() const = 0;
    virtual uint64_t GetTimestamp() const = 0;
    virtual uint64_t GetFrameNumber() const = 0;
    virtual int64_t GetPoolId() const = 0;
    virtual int32_t GetCaptureId() const = 0;
    virtual CameraBufferStatus GetBufferStatus() const = 0;
    virtual int32_t GetSequenceId() const = 0;
    virtual int32_t GetFenceId() const = 0;
    virtual EsFrameInfo GetEsFrameInfo() const = 0;
    virtual int32_t GetEncodeType() const = 0;
    virtual int32_t GetStreamId() const = 0;

    virtual void SetIndex(const int32_t index) = 0;
    virtual void SetWidth(const uint32_t width) = 0;
    virtual void SetHeight(const uint32_t height) = 0;
    virtual void SetStride(const uint32_t stride) = 0;
    virtual void SetFormat(const int32_t format) = 0;
    virtual void SetSize(const uint32_t size) = 0;
    virtual void SetUsage(const uint64_t usage) = 0;
    virtual void SetVirAddress(const void* addr) = 0;
    virtual void SetPhyAddress(const uint64_t addr) = 0;
    virtual void SetFileDescriptor(const int32_t fd) = 0;
    virtual void SetTimestamp(const uint64_t timestamp) = 0;
    virtual void SetFrameNumber(const uint64_t frameNumber) = 0;
    virtual void SetPoolId(const int64_t id) = 0;
    virtual void SetCaptureId(const int32_t id) = 0;
    virtual void SetBufferStatus(const CameraBufferStatus flag) = 0;
    virtual void SetSequenceId(const int32_t sequence) = 0;
    virtual void SetFenceId(const int32_t fence) = 0;
    virtual void SetEncodeType(const int32_t type) = 0;
    virtual void SetEsFrameSize(const int32_t frameSize) = 0;
    virtual void SetEsTimestamp(const int64_t timeStamp) = 0;
    virtual void SetEsKeyFrame(const int32_t isKey) = 0;
    virtual void SetEsFrameNum(const int32_t frameNum) = 0;
    virtual void SetStreamId(const int32_t streamId) = 0;

    virtual void Free() = 0;

    virtual bool operator==(const IBuffer& u) = 0;
};
} // namespace Camera
} // namespace OHOS
#endif

