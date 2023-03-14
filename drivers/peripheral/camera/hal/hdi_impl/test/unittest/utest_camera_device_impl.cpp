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

#include "utest_camera_device_impl.h"

void CameraDeviceImplTest::SetUpTestCase(void)
{
}

void CameraDeviceImplTest::TearDownTestCase(void)
{
    std::cout << "Camera::CameraDeviceImpl TearDownTestCase" << std::endl;
}

void CameraDeviceImplTest::SetUp(void)
{
    bool ret = InitCameraHost();
    if (!ret) {
        std::cout << "CameraDeviceImplTest init camerahost failed" << std::endl;
        return;
    }

    ret = GetCameraIds();
    if (!ret) {
        std::cout << "CameraDeviceImplTest init GetCameraIds failed" << std::endl;
        return;
    }

    ret = GetCameraDevice();
    if (!ret) {
        std::cout << "CameraDeviceImplTest init GetCameraDevice failed" << std::endl;
        return;
    }
}

void CameraDeviceImplTest::TearDown(void)
{
}

HWTEST_F(CameraDeviceImplTest, UTestGetStreamOperator, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    bool ret = GetStreamOperator();
    EXPECT_EQ(true, ret);
}

HWTEST_F(CameraDeviceImplTest, UTestUpdateSettings, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    std::vector<std::string> cameraIds;
    Camera::CamRetCode ret = cameraHost_->GetCameraIds(cameraIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::shared_ptr<CameraAbility> ability = nullptr;
    std::string cameraId = cameraIds.front();
    ret = cameraHost_->GetCameraAbility(cameraId, ability);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    ret = cameraDevice_->UpdateSettings(ability);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
    std::cout << "UTestUpdateSettings ret = " << ret << std::endl;
}

HWTEST_F(CameraDeviceImplTest, UTestSetResultMode, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    ResultCallbackMode mode = PER_FRAME;
    CamRetCode rc = cameraDevice_->SetResultMode(mode);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);
}

HWTEST_F(CameraDeviceImplTest, UTestGetEnabledResults, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    std::vector<MetaType> results;
    CamRetCode rc = cameraDevice_->GetEnabledResults(results);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);

    for (auto &type : results) {
        std::cout << "MetaType = " << type << std::endl;
    }
}

HWTEST_F(CameraDeviceImplTest, UTestEnableResult, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    std::vector<MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    CamRetCode rc = cameraDevice_->EnableResult(results);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);
}

HWTEST_F(CameraDeviceImplTest, UTestDisableResult, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    std::vector<MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    CamRetCode rc = cameraDevice_->EnableResult(results);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);
    std::vector<Camera::MetaType> disable_tag;
    rc = cameraDevice_->GetEnabledResults(disable_tag);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);

    rc = cameraDevice_->DisableResult(disable_tag);
    EXPECT_EQ(true, rc == OHOS::Camera::NO_ERROR);
}

HWTEST_F(CameraDeviceImplTest, UTestClose, TestSize.Level0)
{
    EXPECT_EQ(false, cameraHost_ == nullptr);
    EXPECT_EQ(false, cameraDevice_ == nullptr);

    if (cameraDevice_ != nullptr) {
        cameraDevice_->Close();
        cameraDevice_ = nullptr;
    }
}
