/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utest_v4l2_device_manager.h"
#include <iostream>
#include <gtest/gtest.h>
#include "v4l2_device_manager.h"

namespace OHOS::Camera {
std::shared_ptr<IDeviceManager> DM = nullptr;
void UtestV4l2DeviceManager::SetUpTestCase(void)
{
    std::cout << "Camera::V4l2DeviceManager SetUpTestCase" << std::endl;
}

void UtestV4l2DeviceManager::TearDownTestCase(void)
{
    std::cout << "Camera::V4l2DeviceManager TearDownTestCase" << std::endl;
}

void UtestV4l2DeviceManager::SetUp(void)
{
    std::cout << "Camera::V4l2DeviceManager SetUp" << std::endl;
}

void UtestV4l2DeviceManager::TearDown(void)
{
    std::cout << "Camera::V4l2DeviceManager TearDown.." << std::endl;
}

HWTEST_F(UtestV4l2DeviceManager, Init_Success, TestSize.Level0)
{
    DM = IDeviceManager::GetInstance();
    RetCode rc;
    if(DM != nullptr) {
        rc = DM->Init();
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestV4l2DeviceManager, PowerUp_Success, TestSize.Level0)
{
    RetCode rc;
    if(DM != nullptr) {
        rc = DM->PowerUp(CAMERA_FIRST);
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestV4l2DeviceManager, PowerUp_Failed, TestSize.Level0)
{
    RetCode rc;
    if(DM != nullptr) {
        rc = DM->PowerUp(CAMERA_THIRD);
    }
    EXPECT_EQ(RC_ERROR,rc);
}

HWTEST_F(UtestV4l2DeviceManager, PowerDown_Success, TestSize.Level0)
{
    RetCode rc;
    if(DM != nullptr) {
        rc = DM->PowerDown(CAMERA_FIRST);
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestV4l2DeviceManager, GetSensor_Success, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_FIRST);
    }
    CameraStandard::CameraMetadata meta(100,2000);
    sensor->Init(meta);
    EXPECT_EQ(true,sensor != nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetSensor_Success2, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_SECOND);
    }
    CameraStandard::CameraMetadata meta(100,2000);;
    sensor->Init(meta);
    EXPECT_EQ(true,sensor != nullptr);
}


HWTEST_F(UtestV4l2DeviceManager, GetSensor_Failed, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_THIRD);
    }
    EXPECT_EQ(true,sensor == nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetManager_Success, TestSize.Level0)
{
    std::shared_ptr<IManager> manager;
    if(DM != nullptr) {
        manager = DM->GetManager(DM_M_SENSOR);
    }
    EXPECT_EQ(true,manager != nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetManager_Failed, TestSize.Level0)
{
    std::shared_ptr<IManager> manager;
    if(DM != nullptr) {
        manager = DM->GetManager(DM_M_VO);
    }
    EXPECT_EQ(true,manager == nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetController_Success, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(CAMERA_FIRST,DM_M_SENSOR,DM_C_SENSOR);
    }
    EXPECT_EQ(true,controller != nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetController_Failed1, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(CAMERA_FIRST,DM_M_VI,DM_C_SENSOR);
    }
    EXPECT_EQ(true,controller == nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetController_Failed2, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(CAMERA_FIRST,DM_M_VPSS,DM_C_VO);
    }
    EXPECT_EQ(true,controller == nullptr);
}

HWTEST_F(UtestV4l2DeviceManager, GetCameraId_Success, TestSize.Level0)
{
    std::vector<CameraId> cameraId;
    if(DM != nullptr) {
        cameraId = DM->GetCameraId();
    }
    for (auto iter = cameraId.cbegin(); iter != cameraId.cend(); iter++) {
        EXPECT_EQ(true,(*iter) != CAMERA_MAX);
    }
}
}// namespace OHOS::Camera
