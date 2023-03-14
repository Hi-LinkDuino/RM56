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
#ifndef CAMERA_TEST_COMMON_H
#define CAMERA_TEST_COMMON_H

#include <stdlib.h>
#include <limits.h>
#include <gtest/gtest.h>
#include <iostream>
#include <climits>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <thread>
#include <stdio.h>
#include <sys/time.h>
#include <vector>
#include <map>
#include "utils.h"
#include "camera.h"
#include "camera_host.h"
#include "types.h"
#include <surface.h>
#include "idevice_manager.h"
#include "camera_metadata_info.h"
#include "ibuffer.h"
#include <display_type.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "securec.h"
#include "icamera_host.h"
#include "icamera_device.h"
#include "istream_operator.h"
#include "ioffline_stream_operator.h"
#include "camera_host_callback.h"
#include "camera_device_callback.h"
#include "stream_operator_callback.h"
#include "video_key_info.h"
#include "type_common.h"

namespace OHOS::Camera {
class Test {
public:
    void Init();
    void Open();
    void Close();
    void GetCameraAbility();
    uint64_t GetCurrentLocalTimeStamp();
    int32_t SaveYUV(const char* type, const void* buffer, int32_t size);
    int32_t SaveVideoFile(const char* type, const void* buffer, int32_t size, int32_t operationMode);
    void StartStream(std::vector<Camera::StreamIntent> intents);
    void StartCapture(int streamId, int captureId, bool shutterCallback, bool isStreaming);
    void StopStream(std::vector<int>& captureIds, std::vector<int>& streamIds);
    void StopOfflineStream(int captureId);
    void GetCameraMetadata();
    void StopConsumer(std::vector<Camera::StreamIntent> intents);
    void CreateStreamOperatorCallback();
    void CreateDeviceCallback();
    void CreateOfflineStreamOperatorCallback();
    void StreamInfoFormat();

    OHOS::sptr<StreamOperatorCallback> streamOperatorCallback = nullptr;
    OHOS::sptr<CameraHostCallback> hostCallback = nullptr;
    OHOS::sptr<CameraDeviceCallback> deviceCallback = nullptr;
    OHOS::sptr<IStreamOperator> streamOperator = nullptr;
    OHOS::sptr<Camera::IOfflineStreamOperator> offlineStreamOperator = nullptr;
    OHOS::sptr<IStreamOperatorCallback> offlineStreamOperatorCallback = nullptr;

    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = nullptr;
    std::vector<std::shared_ptr<OHOS::Camera::StreamInfo>> streamInfos;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo2 = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo_pre = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo_video = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo_capture = nullptr;
    std::vector<std::string> cameraIds;
    int streamId_preview = 1000;
    int streamId_preview_double = 1001;
    int streamId_capture = 1010;
    int streamId_video = 1020;
    int captureId_preview = 2000;
    int captureId_preview_double = 2001;
    int captureId_capture = 3000;
    int captureId_video = 4000;
    std::vector<int> captureIds;
    std::vector<int> streamIds;
    std::vector<Camera::StreamIntent> intents;
    OHOS::Camera::CamRetCode rc;

#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Camera::CameraHost> service = nullptr;
    std::shared_ptr<ICameraDevice> cameraDevice = nullptr;
#else
    OHOS::sptr<OHOS::Camera::ICameraHost> service = nullptr;
    OHOS::sptr<ICameraDevice> cameraDevice = nullptr;
#endif
    std::shared_ptr<CameraAbility> ability = nullptr;

    bool status;
    bool onCameraStatusFlag;
    bool onFlashlightStatusFlag;
    bool onErrorFlag;
    bool onResultFlag;
    bool captureStartFlag;
    bool captureEndFlag;
    bool captureErrorFlag;
    bool frameShutterFlag;
    int previewBufCnt = 0;
    int32_t videoFd = -1;
    class StreamConsumer;
    std::map<OHOS::Camera::StreamIntent, std::shared_ptr<StreamConsumer>> consumerMap_ = {};

    class TestBufferConsumerListener : public IBufferConsumerListener {
    public:
        TestBufferConsumerListener() {}
        ~TestBufferConsumerListener() {}
        void OnBufferAvailable() {}
    };

    class StreamConsumer {
    public:
        void StopConsumer();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        std::shared_ptr<OHOS::Surface> CreateProducer(std::function<void(OHOS::SurfaceBuffer* buffer)> callback);
#else
        OHOS::sptr<OHOS::IBufferProducer> CreateProducer(std::function<void(void*, uint32_t)> callback);
#endif
        void TakeSnapshot()
        {
            shotCount_++;
        }
        void WaitSnapshotEnd()
        {
            std::cout << "ready to wait" << std::endl;
            std::unique_lock<std::mutex> l(l_);
            cv_.wait(l, [this]() { return shotCount_ == 0; });
        }
        ~StreamConsumer()
        {
            running_ = false;
            if (consumerThread_ != nullptr) {
                consumerThread_->join();
                delete consumerThread_;
            }
        }
    public:
        std::atomic<uint64_t> shotCount_ = 0;
        std::mutex l_;
        std::condition_variable cv_;
        bool running_ = true;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        std::shared_ptr<OHOS::Surface> consumer_ = nullptr;
        std::function<void(OHOS::SurfaceBuffer*)> callback_ = nullptr;
#else
        OHOS::sptr<OHOS::Surface> consumer_ = nullptr;
        std::function<void(void*, uint32_t)> callback_ = nullptr;
#endif
        std::thread* consumerThread_ = nullptr;
    };
};

class HdiHostCallback : public CameraHostCallback {
public:
    Test *test_;
    virtual ~HdiHostCallback() = default;
    HdiHostCallback(Test *test)
    {
        test_ = test;
    }
    virtual void OnCameraStatus(const std::string &cameraId, CameraStatus status) override
    {
        test_->onCameraStatusFlag = true;
    }
    virtual void OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status) override
    {
        test_->onFlashlightStatusFlag = true;
    }
};
class HdiDeviceCallback : public CameraDeviceCallback {
public:
    Test *test_;
    virtual ~HdiDeviceCallback() = default;
    HdiDeviceCallback(Test *test)
    {
        test_ = test;
    }
    virtual void OnError(ErrorType type, int32_t errorMsg) override
    {
        test_->onErrorFlag = true;
    }
    virtual void OnResult(uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata> &result) override
    {
        test_->onResultFlag = true;
    }
};
class HdiOperatorCallback : public StreamOperatorCallback {
public:
    Test *test_;
    virtual ~HdiOperatorCallback() = default;
    HdiOperatorCallback(Test *test)
    {
        test_ = test;
    }
    virtual void OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamId) override
    {
        test_->captureStartFlag = true;
    }
    virtual void OnCaptureEnded(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureEndedInfo>> &info) override
    {
        test_->captureEndFlag = true;
    }
    virtual void OnCaptureError(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureErrorInfo>> &info) override
    {
        test_->captureErrorFlag = true;
    }
    virtual void OnFrameShutter(int32_t captureId,
        const std::vector<int32_t> &streamId, uint64_t timestamp) override
    {
        test_->frameShutterFlag = true;
    }
};
}
#endif
