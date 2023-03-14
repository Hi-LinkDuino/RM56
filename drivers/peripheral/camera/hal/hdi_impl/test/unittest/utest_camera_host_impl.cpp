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

#include "utest_camera_host_impl.h"

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::Camera;

HWTEST_F(CameraHostImplTest, UTestCreateCameraHost, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);
}

HWTEST_F(CameraHostImplTest, UTestGetCameraIds, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    ret = GetCameraIds();
    EXPECT_EQ(true, ret);
}

HWTEST_F(CameraHostImplTest, UTestGetCameraAbility, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    ret = GetCameraIds();
    EXPECT_EQ(true, ret);

    std::shared_ptr<CameraAbility> ability = nullptr;
    std::string cameraId = cameraIds_.front();
    OHOS::Camera::CamRetCode rc =
        cameraHost_->GetCameraAbility(cameraId, ability);
    EXPECT_EQ(OHOS::Camera::CamRetCode::NO_ERROR, rc);
}

#ifndef CAMERA_BUILT_ON_OHOS_LITE
HWTEST_F(CameraHostImplTest, UTestSetFlashlight, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    ret = GetCameraIds();
    EXPECT_EQ(true, ret);

    std::shared_ptr<CameraAbility> ability = nullptr;
    std::string cameraId = cameraIds_.front();
    bool isEnable = true;
    OHOS::Camera::CamRetCode rc =
        cameraHost_->SetFlashlight(cameraId, isEnable);
    EXPECT_EQ(OHOS::Camera::CamRetCode::NO_ERROR, rc);

    sleep(5);

    isEnable = false;
    rc = cameraHost_->SetFlashlight(cameraId, isEnable);
    EXPECT_EQ(OHOS::Camera::CamRetCode::NO_ERROR, rc);
}
#endif

HWTEST_F(CameraHostImplTest, UTestOpenCamera, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    ret = GetCameraIds();
    EXPECT_EQ(true, ret);

    std::string cameraId = cameraIds_.front();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraDeviceCallback> deviceCallback = std::make_shared<CameraDeviceCallback>();
#else
    OHOS::sptr<CameraDeviceCallback> deviceCallback = new CameraDeviceCallback();
#endif
    OHOS::Camera::CamRetCode rc = cameraHost_->OpenCamera(
        cameraId, deviceCallback, cameraDevice_);
    EXPECT_EQ(OHOS::Camera::CamRetCode::NO_ERROR, rc);

    if (cameraDevice_ != nullptr) {
        cameraDevice_->Close();
        cameraDevice_ = nullptr;
    }
}

HWTEST_F(CameraHostImplTest, OpenCamera_CallbackIsNull, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    ret = GetCameraIds();
    EXPECT_EQ(true, ret);

    std::string cameraId = cameraIds_.front();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraDeviceCallback> deviceCallback = nullptr;
#else
    OHOS::sptr<CameraDeviceCallback> deviceCallback = nullptr;
#endif
    OHOS::Camera::CamRetCode rc = cameraHost_->OpenCamera(
        cameraId, deviceCallback, cameraDevice_);
    EXPECT_EQ(OHOS::Camera::CamRetCode::INVALID_ARGUMENT, rc);

    if (cameraDevice_ != nullptr) {
        cameraDevice_->Close();
        cameraDevice_ = nullptr;
    }
}

HWTEST_F(CameraHostImplTest, OpenCamera_CameraIdIsEmpty, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    std::string cameraId;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraDeviceCallback> deviceCallback = std::make_shared<CameraDeviceCallback>();
#else
    OHOS::sptr<CameraDeviceCallback> deviceCallback = new CameraDeviceCallback();
#endif
    OHOS::Camera::CamRetCode rc = cameraHost_->OpenCamera(
        cameraId, deviceCallback, cameraDevice_);
    EXPECT_EQ(OHOS::Camera::CamRetCode::INVALID_ARGUMENT, rc);

    if (cameraDevice_ != nullptr) {
        cameraDevice_->Close();
        cameraDevice_ = nullptr;
    }
}

HWTEST_F(CameraHostImplTest, OpenCamera_CameraIdIsNotFound, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);

    std::string cameraId = "qwerty";
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraDeviceCallback> deviceCallback = std::make_shared<CameraDeviceCallback>();
#else
    OHOS::sptr<CameraDeviceCallback> deviceCallback = new CameraDeviceCallback();
#endif
    OHOS::Camera::CamRetCode rc = cameraHost_->OpenCamera(
        cameraId, deviceCallback, cameraDevice_);
    EXPECT_EQ(OHOS::Camera::CamRetCode::INVALID_ARGUMENT, rc);

    if (cameraDevice_ != nullptr) {
        cameraDevice_->Close();
        cameraDevice_ = nullptr;
    }
}

HWTEST_F(CameraHostImplTest, UtestSetCallback, TestSize.Level0)
{
    bool ret = InitCameraHost();
    EXPECT_EQ(true, ret);
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraHostCallback> callback = std::make_shared<CameraHostCallback>();
#else
    OHOS::sptr<CameraHostCallback> callback = new CameraHostCallback();
#endif
    OHOS::Camera::CamRetCode rc = cameraHost_->SetCallback(callback);
    EXPECT_EQ(OHOS::Camera::CamRetCode::NO_ERROR, rc);
}
