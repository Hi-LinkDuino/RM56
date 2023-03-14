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

#include "utest_camera_hdi_base.h"

#define HDF_LOG_TAG camera_service_test

constexpr const char *TEST_SERVICE_NAME = "camera_service";

void CameraHdiBaseTest::SetUpTestCase(void)
{
}

void CameraHdiBaseTest::TearDownTestCase(void)
{
}

void CameraHdiBaseTest::SetUp(void)
{
}

void CameraHdiBaseTest::TearDown(void)
{
}

bool CameraHdiBaseTest::InitCameraHost()
{
    if (cameraHost_ != nullptr) {
        return true;
    }
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    cameraHost_ = OHOS::Camera::CameraHost::CreateCameraHost();
#else
    cameraHost_ = ICameraHost::Get(TEST_SERVICE_NAME);
#endif
    if (cameraHost_ == nullptr) {
        return false;
    }
    return true;
}

bool CameraHdiBaseTest::GetCameraDevice()
{
    if (cameraDevice_ != nullptr) {
        return true;
    }

    if (cameraIds_.empty()) {
        return false;
    }

    std::string cameraId = cameraIds_.front();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraDeviceCallback> deviceCallback = std::make_shared<CameraDeviceCallback>();
#else
    OHOS::sptr<CameraDeviceCallback> deviceCallback = new CameraDeviceCallback();
#endif
    CamRetCode rc = cameraHost_->OpenCamera(cameraId, deviceCallback, cameraDevice_);
    if (cameraDevice_ == nullptr) {
        return false;
    }
    return true;
}

bool CameraHdiBaseTest::GetStreamOperator()
{
    if (streamOperator_ != nullptr) {
        return true;
    }

    if (cameraDevice_ == nullptr) {
        return false;
    }

#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<StreamOperatorCallback> streamOperatorCallback = std::make_shared<StreamOperatorCallback>();
#else
    OHOS::sptr<StreamOperatorCallback> streamOperatorCallback = new StreamOperatorCallback();
#endif
    (void)cameraDevice_->GetStreamOperator(streamOperatorCallback, streamOperator_);
    if (streamOperator_ == nullptr) {
        return false;
    }
    return true;
}

bool CameraHdiBaseTest::GetCameraIds()
{
    if (InitCameraHost()) {
        (void)cameraHost_->GetCameraIds(cameraIds_);
    }
    if (cameraIds_.empty()) {
        return false;
    }
    return true;
}

int32_t CameraHdiBaseTest::SaveToFile(const std::string path, const void* buffer, int32_t size)
{
    char checkPath[PATH_MAX] = {0};
    if (::realpath(path.c_str(), checkPath) == nullptr) {
        return -1;
    }
    int imgFd = open(path.c_str(), O_RDWR | O_CREAT, 00766);
    if (imgFd == -1) {
        std::cout << "open file failed." << std::endl;
        return -1;
    }

    int ret = write(imgFd, buffer, size);
    if (ret == -1) {
        std::cout << "write failed." << std::endl;
        close(imgFd);
        return -1;
    }
    close(imgFd);
    return 0;
}

uint64_t CameraHdiBaseTest::GetCurrentLocalTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
}
