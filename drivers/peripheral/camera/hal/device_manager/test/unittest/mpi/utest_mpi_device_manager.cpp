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

#include "utest_mpi_device_manager.h"
#include <iostream>
#include <gtest/gtest.h>
#include "mpi_device_manager.h"
#include "venc_controller.h"
#include "vo_controller.h"
#include "vi_controller.h"
#include "vpss_controller.h"

using namespace testing::ext;
namespace OHOS::Camera {
class TestNode
{
public:
    ~TestNode() {};
    TestNode() {};
public:
    void CallBack(std::shared_ptr<FrameSpec> buffer) {};
    void Init()
    {
        std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
        deviceManager->SetNodeCallBack([&](std::shared_ptr<FrameSpec> frameBuffer) {
            CallBack(frameBuffer);
        });
    };
};

class TestDevice
{
public:
    TestDevice() {};
    ~TestDevice() {};
public:
    void Init()
    {
        std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
        deviceManager->SetMetaDataCallBack([&](std::shared_ptr<CameraStandard::CameraMetadata> meta) {
            CallBack(meta);
        });
    };
    void CallBack(std::shared_ptr<CameraStandard::CameraMetadata> meta) {};
};

std::shared_ptr<IDeviceManager> DM = nullptr;
void UtestMpiDeviceManager::SetUpTestCase(void)
{
    std::cout << "Camera::DeviceManager SetUpTestCase" << std::endl;
}

void UtestMpiDeviceManager::TearDownTestCase(void)
{
    std::cout << "Camera::DeviceManager TearDownTestCase" << std::endl;
}

void UtestMpiDeviceManager::SetUp(void)
{
    std::cout << "Camera::DeviceManager SetUp" << std::endl;
}

void UtestMpiDeviceManager::TearDown(void)
{
    std::cout << "Camera::DeviceManager TearDown.." << std::endl;
}

HWTEST_F(UtestMpiDeviceManager, Init_Success, TestSize.Level0)
{
    DM = IDeviceManager::GetInstance();
    RetCode rc = RC_OK;
    if(DM != nullptr) {
        rc = DM->Init();
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestMpiDeviceManager, PowerUp_Failed, TestSize.Level0)
{
    RetCode rc = RC_OK;
    if(DM != nullptr) {
        rc = DM->PowerUp(CAMERA_THIRD);
    }
    EXPECT_EQ(RC_ERROR,rc);
}

HWTEST_F(UtestMpiDeviceManager, PowerUp_Success, TestSize.Level0)
{
    RetCode rc = RC_OK;
    if(DM != nullptr) {
        rc = DM->PowerUp(CAMERA_FIRST);
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestMpiDeviceManager, GetSensor_Success, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_FIRST);
    }
    CameraStandard::CameraMetadata meta(100,2000);
    sensor->Init(meta);
    EXPECT_EQ(true,sensor != nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetSensor_Success2, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_SECOND);
    }
    CameraStandard::CameraMetadata meta(100,2000);
    sensor->Init(meta);
    EXPECT_EQ(true,sensor != nullptr);
}


HWTEST_F(UtestMpiDeviceManager, GetSensor_Failed, TestSize.Level0)
{
    std::shared_ptr<ISensor> sensor;
    if(DM != nullptr) {
        sensor = DM->GetSensor(CAMERA_THIRD);
    }
    EXPECT_EQ(true,sensor == nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetManager_Success, TestSize.Level0)
{
    std::shared_ptr<IManager> manager;
    if(DM != nullptr) {
        manager = DM->GetManager(DM_M_VI);
    }
    EXPECT_EQ(true,manager != nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetManager_Failed, TestSize.Level0)
{
    std::shared_ptr<IManager> manager;
    if(DM != nullptr) {
        manager = DM->GetManager(DM_M_FLASH);
    }
    EXPECT_EQ(true,manager == nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetController_Success, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(DM_M_VI,DM_C_VI);
    }
    EXPECT_EQ(true,controller != nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetController_Failed1, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(DM_M_VI,DM_C_SENSOR);
    }
    EXPECT_EQ(true,controller == nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetController_Failed2, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    if(DM != nullptr) {
        controller = DM->GetController(DM_M_VPSS,DM_C_VO);
    }
    EXPECT_EQ(true,controller == nullptr);
}

HWTEST_F(UtestMpiDeviceManager, GetCameraId_Success, TestSize.Level0)
{
    std::vector<CameraId> cameraId;
    if(DM != nullptr) {
        cameraId = DM->GetCameraId();
    }
    for (auto iter = cameraId.cbegin(); iter != cameraId.cend(); iter++) {
        EXPECT_EQ(true,(*iter) != CAMERA_MAX);
    }
}

HWTEST_F(UtestMpiDeviceManager, Connect_Success, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    controller = DM->GetController(DM_M_VI,DM_C_VI);
    std::shared_ptr<ViController> vi;
    vi = std::static_pointer_cast<ViController>(controller);
    vi->ConfigVi();
    vi->StartVi();
    RetCode rc = DM->Connect("vi#0","out#0","vpss#0","in#0");
    EXPECT_EQ(RC_OK,rc);

    controller = DM->GetController(DM_M_VPSS,DM_C_VPSS);
    std::shared_ptr<VpssController> vpss;
    vpss = std::static_pointer_cast<VpssController>(controller);
    vpss->ConfigVpss();
    vpss->StartVpss();
    rc = DM->Connect("vpss#0","out#0","vo#0","in#0");
    EXPECT_EQ(RC_OK,rc);

    controller = DM->GetController(DM_M_VO,DM_C_VO);
    std::shared_ptr<VoController> vo;
    vo = std::static_pointer_cast<VoController>(controller);
    rc = vo->ConfigVo();
    EXPECT_EQ(RC_OK,rc);
    rc = vo->StartVo();
    EXPECT_EQ(RC_OK,rc);
    sleep(1);
}

HWTEST_F(UtestMpiDeviceManager, SendFrameBuffer_Success, TestSize.Level0)
{
    std::shared_ptr<FrameSpec> buffer = std::make_shared<FrameSpec>();
    TestNode node;
    node.Init();
    TestDevice device;
    device.Init();
    DM->SendFrameBuffer(buffer);
}

HWTEST_F(UtestMpiDeviceManager, Configure_Success, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    controller = DM->GetController(DM_M_VI,DM_C_VI);
    EXPECT_EQ(true,controller != nullptr);
    std::shared_ptr<CameraStandard::CameraMetadata> meta = std::make_shared<CameraStandard::CameraMetadata>(100,2000);
    int32_t expo = 0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo ,1);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    RetCode rc = controller->Configure(meta);
    EXPECT_EQ(RC_OK,rc);
    sleep(1);
}

HWTEST_F(UtestMpiDeviceManager, SetAbilityMetaData_Success, TestSize.Level0)
{
    std::vector<int32_t> abilityMetaData;
    abilityMetaData.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    abilityMetaData.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    std::shared_ptr<IController> iController;
    iController = DM->GetController(DM_M_VI, DM_C_VI);
    EXPECT_EQ(true, iController != nullptr);
    DM->SetAbilityMetaDataTag(abilityMetaData);
}

HWTEST_F(UtestMpiDeviceManager, UnConnect_Success, TestSize.Level0)
{
    RetCode rc = RC_ERROR;
    std::shared_ptr<IController> controller;
    controller = DM->GetController(DM_M_VPSS,DM_C_VPSS);
    std::shared_ptr<VpssController> vpss;
    vpss = std::static_pointer_cast<VpssController>(controller);
    rc = DM->UnConnect("vpss#0","out#0","vo#0","in#0");
    EXPECT_EQ(RC_OK,rc);
    vpss->StopVpss();

    controller = DM->GetController(DM_M_VO,DM_C_VO);
    std::shared_ptr<VoController> vo;
    vo = std::static_pointer_cast<VoController>(controller);
    rc = vo->StopVo();
    EXPECT_EQ(RC_OK,rc);

    controller = DM->GetController(DM_M_VI,DM_C_VI);
    std::shared_ptr<ViController> vi;
    vi = std::static_pointer_cast<ViController>(controller);
    rc = DM->UnConnect("vi#0","out#0","vpss#0","in#0");
    vi->StopVi();
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestMpiDeviceManager, PowerDown_Success, TestSize.Level0)
{
    RetCode rc = RC_OK;
    if(DM != nullptr) {
        rc = DM->PowerDown(CAMERA_FIRST);
    }
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestMpiDeviceManager, Connect_Failed1, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    controller = DM->GetController(DM_M_VI,DM_C_VI);
    std::shared_ptr<ViController> vi;
    vi = std::static_pointer_cast<ViController>(controller);
    RetCode rc = DM->Connect("vi#0","out#1","venc#0","in#0");
    EXPECT_EQ(RC_OK,rc);
}

HWTEST_F(UtestMpiDeviceManager, Connect_Failed2, TestSize.Level0)
{
    std::shared_ptr<IController> controller;
    controller = DM->GetController(DM_M_VO,DM_C_VO);
    std::shared_ptr<VoController> vo;
    vo = std::static_pointer_cast<VoController>(controller);
    RetCode rc = DM->Connect("vo#0","out#4","vi#0","in#0");
    EXPECT_EQ(RC_ERROR,rc);
}

HWTEST_F(UtestMpiDeviceManager, UnConnect_Failed1, TestSize.Level0)
{
    RetCode rc = DM->UnConnect("vpss#0","out#0","vpss#0","in#0");
    EXPECT_EQ(RC_ERROR,rc);
}

HWTEST_F(UtestMpiDeviceManager, UnConnect_Failed2, TestSize.Level0)
{
    RetCode rc = DM->UnConnect("test#0","out#0","vpss#0","in#0");
    EXPECT_EQ(RC_OK,rc);
}
}// namespace OHOS::Camera
