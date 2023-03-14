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

#include "capture_request.h"
#include <sys/time.h>

namespace OHOS::Camera {
CaptureRequest::CaptureRequest(const int32_t id,
                               const int32_t n,
                               CaptureSetting& setting,
                               bool needReport,
                               bool isContinuous)
{
    semp_ = std::make_unique<RequestSemaphore>(n);
    semr_ = std::make_unique<RequestSemaphore>(n);
    captureId_ = id;
    settings_ = setting;
    needShutterCallback_ = needReport;
    isContinuous_ = isContinuous;
}

CaptureRequest::~CaptureRequest()
{
    CAMERA_LOGV("enter");
    owners_.clear();
}

RetCode CaptureRequest::AddOwner(const std::weak_ptr<IStream>& owner)
{
    auto stream = owner.lock();
    CHECK_IF_PTR_NULL_RETURN_VALUE(stream, RC_ERROR);

    std::lock_guard<std::mutex> l(lock_);
    owners_[stream->GetStreamId()] = stream;
    ownerCount_++;

    return RC_OK;
}

RetCode CaptureRequest::Process(const int32_t id)
{
    auto stream = owners_[id].lock();
    CHECK_IF_PTR_NULL_RETURN_VALUE(stream, RC_ERROR);

    semp_->Sync();
    return stream->Capture(shared_from_this());
}

RetCode CaptureRequest::OnResult(const int32_t id)
{
    auto stream = owners_[id].lock();
    CHECK_IF_PTR_NULL_RETURN_VALUE(stream, RC_ERROR);

    semr_->Sync();
    return stream->OnFrame(shared_from_this());
}

void CaptureRequest::DisableSync()
{
    std::lock_guard<std::mutex> l(lock_);
    ownerCount_ = 1;
    semp_->needSync = false;
    semp_->cv_.notify_all();
    semr_->needSync = false;
    semr_->cv_.notify_all();
}

uint32_t CaptureRequest::GetOwnerCount()
{
    std::lock_guard<std::mutex> l(lock_);
    return ownerCount_;
}

RetCode CaptureRequest::Cancel()
{
    for (auto it : owners_) {
        auto stream = it.second.lock();
        if (stream == nullptr) {
            continue;
        }
        needCancel_ = true;
        stream->CancelRequest(shared_from_this());
    }
    return RC_OK;
}

uint64_t CaptureRequest::GetBeginTime() const
{
    return semp_->timestamp_;
}

uint64_t CaptureRequest::GetEndTime() const
{
    return semr_->timestamp_;
}

bool CaptureRequest::NeedShutterCallback() const
{
    return needShutterCallback_;
}

bool CaptureRequest::IsContinous() const
{
    return isContinuous_;
}

int32_t CaptureRequest::GetCaptureId() const
{
    return captureId_;
}

void CaptureRequest::AttachBuffer(std::shared_ptr<IBuffer>& b)
{
    buffer_ = b;
}

std::shared_ptr<IBuffer> CaptureRequest::GetAttachedBuffer() const
{
    return buffer_;
}

bool CaptureRequest::NeedCancel() const
{
    return needCancel_.load();
}

void CaptureRequest::SetFirstRequest(bool b)
{
    isFirstRequest_ = b;
}

bool CaptureRequest::IsFirstOne() const
{
    return isFirstRequest_;
}

CaptureMeta CaptureRequest::GetCaptureSetting() const
{
    return settings_.lock();
}

CaptureRequest::RequestSemaphore::RequestSemaphore(const int32_t n)
{
    ownerCount_ = n;
}

CaptureRequest::RequestSemaphore::~RequestSemaphore()
{
    cv_.notify_all();
}

void CaptureRequest::RequestSemaphore::Sync()
{
    if (!needSync) {
        return;
    }

    syncCount_++;
    if (syncCount_ < ownerCount_) {
        std::unique_lock<std::mutex> l(sem_);
        cv_.wait(l);
    } else {
        std::unique_lock<std::mutex> l(sem_);
        syncCount_ = 0;
        timestamp_ = GenerateTimeStamp();
        cv_.notify_all();
    }
}

uint64_t CaptureRequest::RequestSemaphore::GenerateTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t time = static_cast<uint64_t>(tv.tv_sec) * 1000 * 1000 + tv.tv_usec; // 1000:microsecond
    return time;
}
} // namespace OHOS::Camera
