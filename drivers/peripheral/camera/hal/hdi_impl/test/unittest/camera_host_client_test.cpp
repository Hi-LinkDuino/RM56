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

#include <thread>
#include <unistd.h>
#include <vector>
#include <gtest/gtest.h>
#include <map>
#include <hdf_log.h>
#include <osal_mem.h>
#include "securec.h"

#include "camera_metadata_info.h"
#include "camera.h"
#include "ibuffer.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ioffline_stream_operator.h"
#include <surface.h>
#include <display_type.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "icamera_host.h"
#include "camera_host_proxy.h"
#include "camera_device_callback.h"
#include "camera_host_callback.h"
#include "icamera_device.h"
#include "stream_operator_callback.h"
#include "istream_operator_callback.h"

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::Camera;

#define HDF_LOG_TAG camera_service_test

constexpr const char *TEST_SERVICE_NAME = "camera_service";

class TestStreamConsumerListener: public IBufferConsumerListener {
    public:
        TestStreamConsumerListener()
        {
        }

        ~TestStreamConsumerListener()
        {
        }

        void OnBufferAvailable()
        {
        }
};

class StreamConsumer {
    public:
        OHOS::sptr<OHOS::IBufferProducer> CreateProducer(std::function<void(void*, uint32_t)> callback)
        {
            consumer_ = OHOS::Surface::CreateSurfaceAsConsumer();
            if (consumer_ == nullptr) {
                return nullptr;
            }
            sptr<IBufferConsumerListener> listener = new TestStreamConsumerListener();
            consumer_->RegisterConsumerListener(listener);

            auto producer = consumer_->GetProducer();
            std::cout << "create a buffer queue producer: " << producer.GetRefPtr() << std::endl;

            if (producer == nullptr) {
                return nullptr;
            }
            callback_ = callback;

            consumerThread_ = new std::thread([this] {
                int64_t timestamp = 0;
                int32_t flushFence = 0;
                OHOS::Rect damage;
                OHOS::BufferRequestConfig config;
                while (running_ == true) {
                    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
                    consumer_->AcquireBuffer(buffer, flushFence, timestamp, damage);
                    if (buffer != nullptr) {
                        uint32_t size = buffer->GetSize();
                        void* addr = buffer->GetVirAddr();
                        if (callback_ != nullptr) {
                            callback_(addr, size);
                        }
                        consumer_->ReleaseBuffer(buffer, -1);
                        shotCount_--;
                        if (shotCount_ == 0) {
                            std::unique_lock<std::mutex> l(l_);
                            cv_.notify_one();
                        }
                    }
                }
            });

            return producer;
        }

        void WaitSnapshotEnd()
        {
            std::unique_lock<std::mutex> l(l_);
            cv_.wait(l, [this]() { return shotCount_ == 0; });
        }

        void TakeSnapshot()
        {
            shotCount_++;
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
        OHOS::sptr<OHOS::Surface> consumer_ = nullptr;
        std::thread* consumerThread_ = nullptr;
        std::function<void(void*, uint32_t)> callback_ = nullptr;
};

class CameraRemoteTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(CameraRemoteTest, HostSetCallback, TestSize.Level0)
{
    sptr<ICameraHost> sampleObj = ICameraHost::Get(TEST_SERVICE_NAME);
    if (sampleObj == nullptr) {
        std::cout << "ICameraHost get failed." << std::endl;
    }
    ASSERT_TRUE(sampleObj != nullptr);

    OHOS::sptr<CameraHostCallback> callback = new CameraHostCallback();
    sampleObj->SetCallback(callback);

    std::vector<std::string> cameraIds;
    Camera::CamRetCode ret = sampleObj->GetCameraIds(cameraIds);
    std::cout << "GetCameraIds ret = " << ret << std::endl;

    std::shared_ptr<CameraAbility> ability = nullptr;
    std::string cameraId = cameraIds.front();

    ret = sampleObj->GetCameraAbility(cameraId, ability);

    common_metadata_header_t *meta = ability->get();
    size_t tagCount = get_camera_metadata_data_count(meta);
    std::cout << "CameraRemoteTest tagcount = " << tagCount << std::endl;

    OHOS::sptr<CameraDeviceCallback> deviceCallback = new CameraDeviceCallback();
    OHOS::sptr<ICameraDevice> cameraDevice = nullptr;
    ret = sampleObj->OpenCamera(cameraId, deviceCallback, cameraDevice);
    if (cameraDevice == nullptr) {
        return;
    }
    std::cout << "sampleObj->OpenCamera ret = " << ret << std::endl;

    ret = cameraDevice->UpdateSettings(ability);
    std::cout << "UpdateSettings camRetCode = " << ret << std::endl;

    OHOS::Camera::ResultCallbackMode resultCallbackMode = OHOS::Camera::ON_CHANGED;
    ret = cameraDevice->SetResultMode(resultCallbackMode);
    std::cout << "cameraDevice->SetResultMode = " << ret << std::endl;

    std::vector<OHOS::Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    ret = cameraDevice->EnableResult(results);
    std::cout << "cameraDevice->EnableResult = " << ret << std::endl;

    std::vector<Camera::MetaType> disable_tag;
    ret = cameraDevice->GetEnabledResults(disable_tag);
    std::cout << "cameraDevice->GetEnabledResults = " << ret << std::endl;

    ret = cameraDevice->DisableResult(disable_tag);
    std::cout << "cameraDevice->DisableResult = " << ret << std::endl;

    OHOS::sptr<StreamOperatorCallback> streamOperatorCallback = new StreamOperatorCallback();
    OHOS::sptr<IStreamOperator> streamOperator = nullptr;
    ret = cameraDevice->GetStreamOperator(streamOperatorCallback, streamOperator);
    std::cout << "GetStreamOperator camRetCode = " << ret << std::endl;

    OHOS::Camera::OperationMode operationMode = NORMAL;
    StreamSupportType supportType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> streamInfos;
    std::shared_ptr<Camera::StreamInfo> streamInfo = std::make_shared<Camera::StreamInfo>();
    streamInfo->streamId_ = 1001;
    streamInfo->width_ = 720;
    streamInfo->height_ = 480;
    streamInfo->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfo->datasapce_ = 8;
    streamInfo->intent_ = Camera::PREVIEW;
    StreamConsumer previewConsumer;
    streamInfo->bufferQueue_ = previewConsumer.CreateProducer(
            [](void* addr, uint32_t size) {
               CAMERA_LOGI("received a preview buffer ..."); });
    streamInfo->tunneledMode_ = 5;
    streamInfos.push_back(streamInfo);
    ret = streamOperator->IsStreamsSupported(NORMAL, ability, streamInfo, supportType);
    std::cout << "streamOperator->IsStreamsSupported = " << ret << std::endl;

    std::shared_ptr<Camera::StreamInfo> streamInfoSnapshot = std::make_shared<Camera::StreamInfo>();
    streamInfoSnapshot->streamId_ = 1002;
    streamInfoSnapshot->width_ = 1920;
    streamInfoSnapshot->height_ = 1080;
    streamInfoSnapshot->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfoSnapshot->datasapce_ = 8;
    streamInfoSnapshot->intent_ = Camera::STILL_CAPTURE;
    StreamConsumer snapshotConsumer;
    streamInfoSnapshot->bufferQueue_ = snapshotConsumer.CreateProducer(
            [](void* addr, uint32_t size) {
               std::cout << "received a snapshot buffer ..." << std::endl; });
    streamInfoSnapshot->tunneledMode_ = 5;
    streamInfos.push_back(streamInfoSnapshot);

    ret = streamOperator->CreateStreams(streamInfos);
    std::cout << "streamOperator->CreateStreams = " << ret << std::endl;

    ret = streamOperator->CommitStreams(OHOS::Camera::NORMAL, ability);
    std::cout << "streamOperator->CommitStreams = " << ret << std::endl;

    int captureId = 2001;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {streamInfo->streamId_, streamInfoSnapshot->streamId_};
    captureInfo->captureSetting_ = ability;
    captureInfo->enableShutterCallback_ = false;
    ret = streamOperator->Capture(captureId, captureInfo, true);
    std::cout << "streamOperator->Capture = " << ret << std::endl;
    sleep(10);

    std::vector<std::shared_ptr<StreamAttribute>> attributes;
    ret = streamOperator->GetStreamAttributes(attributes);
    std::cout << "streamOperator->GetStreamAttributes = " << ret << std::endl;

    OHOS::sptr<IOfflineStreamOperator> offlineOperator = nullptr;
    OHOS::sptr<IStreamOperatorCallback> offlineOperatorCallback = streamOperatorCallback;
    ret = streamOperator->ChangeToOfflineStream({streamInfoSnapshot->streamId_},
            offlineOperatorCallback, offlineOperator);
    std::cout << "4streamOperator->ChangeToOfflineStream ret = " << ret << std::endl;
    EXPECT_EQ(Camera::NO_ERROR, ret);
    EXPECT_EQ(false, offlineOperator == nullptr);
    cameraDevice->Close();
    sleep(10);
    ret = offlineOperator->Release();
    EXPECT_EQ(false, ret != Camera::NO_ERROR);
}
