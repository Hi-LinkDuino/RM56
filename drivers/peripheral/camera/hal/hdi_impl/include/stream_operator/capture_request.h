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

#ifndef STREAM_OPERATOR_CAPTURE_REQUEST_H
#define STREAM_OPERATOR_CAPTURE_REQUEST_H

#include "camera_metadata_info.h"
#include "istream.h"
#include "stream.h"
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>

namespace OHOS::Camera {
class IStream;
using CaptureSetting = std::weak_ptr<CameraStandard::CameraMetadata>;

class CaptureRequest final : public std::enable_shared_from_this<CaptureRequest> {
public:
    CaptureRequest(const int32_t id, const int32_t n, CaptureSetting& setting, bool needReport, bool isContinuous);
    ~CaptureRequest();
    RetCode AddOwner(const std::weak_ptr<IStream>& owner);
    RetCode Process(const int32_t id);
    RetCode OnResult(const int32_t id);
    RetCode Cancel();
    uint32_t GetOwnerCount();
    void DisableSync();
    uint64_t GetBeginTime() const;
    uint64_t GetEndTime() const;
    bool NeedShutterCallback() const;
    bool IsContinous() const;
    int32_t GetCaptureId() const;
    void AttachBuffer(std::shared_ptr<IBuffer>& b);
    std::shared_ptr<IBuffer> GetAttachedBuffer() const;
    bool NeedCancel() const;
    void SetFirstRequest(bool b);
    bool IsFirstOne() const;
    CaptureMeta GetCaptureSetting() const;

private:
    class RequestSemaphore final {
    public:
        RequestSemaphore() = default;
        RequestSemaphore(const int32_t n);
        ~RequestSemaphore();
        void Sync();

    private:
        uint64_t GenerateTimeStamp();

    public:
        std::mutex sem_ = {};
        uint64_t timestamp_ = 0;
        std::atomic<uint32_t> ownerCount_ = 0;
        std::atomic<uint32_t> syncCount_ = 0;
        std::condition_variable cv_ = {};
        std::atomic<bool> needSync = true;
    };

    int32_t captureId_ = -1;
    std::mutex lock_ = {};
    std::map<int32_t, std::weak_ptr<IStream>> owners_ = {};
    std::unique_ptr<RequestSemaphore> semp_ = nullptr;
    std::unique_ptr<RequestSemaphore> semr_ = nullptr;
    CaptureSetting settings_;
    bool needShutterCallback_ = false;
    bool isContinuous_ = false;
    std::shared_ptr<IBuffer> buffer_ = nullptr;
    std::atomic<bool> needCancel_ = false;
    uint32_t ownerCount_ = 0;
    bool isFirstRequest_ = false;
};
} // namespace OHOS::Camera
#endif
