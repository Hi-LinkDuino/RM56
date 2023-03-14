/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file expected in compliance with the License.
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

#include "camera_3a_test.h"

void UtestCamera3ATest::SetUpTestCase(void)
{}
void UtestCamera3ATest::TearDownTestCase(void)
{}
void UtestCamera3ATest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestCamera3ATest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: updateSettings AE
  * @tc.desc: Preview，updateSettings OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, success.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0001)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "then UpdateSettings OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // Configure preview stream information
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log] UpdateSettings, exposure for 10s." << std::endl;
    sleep(10);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_AUTO, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0002)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_AUTO, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // Configure stream information
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_AUTO." << std::endl;
    display_->cameraDevice->UpdateSettings(meta);
    sleep(2);

    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_INCANDESCENT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0003)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_INCANDESCENT, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_INCANDESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings,";
    std::cout << "awb mode: OHOS_CAMERA_AWB_MODE_INCANDESCENT" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_FLUORESCENT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0004) // 3A mode white balance blue scene
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_FLUORESCENT, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // Configure stream information
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode =  AWB_MODE_WARM_FLUORESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_AUTO." << std::endl;
    display_->cameraDevice->UpdateSettings(meta);
    sleep(2);

    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0005)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT, ";
    std::cout << "success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, ";
    std::cout << "awb mode: OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_DAYLIGHT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0006)
{
    std::cout << "==========[test log] Preview, ";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_DAYLIGHT, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_DAYLIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_DAYLIGHT" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);
    sleep(5);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0007) // 3A mode reset
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT,";
    std::cout << "success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, ";
    std::cout << "awb mode: OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0008)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_TWILIGHT, ";
    std::cout << "success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // Configure stream information
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_TWILIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_TWILIGHT" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0009) // 3A mode white balance yellow scene
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_SHADE, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_SHADE;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_SHADE" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_OFF, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0010)
{
    std::cout << "==========[test log] Preview，";
    std::cout << "updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CAMERA_AWB_MODE_OFF, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_OFF;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log] UpdateSettings, awb mode: OHOS_CAMERA_AWB_MODE_OFF" << std::endl;
    display_->cameraDevice->UpdateSettings(meta);

    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings reset
  * @tc.desc: Preview set 3A, then close device, and preview, 3A is reset.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0030)
{
    std::cout << "==========[test log] Preview set 3A, then close device, and preview, 3A is reset." << std::endl;
    std::cout << "==========[test log] The 1st time set 3A." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log] UpdateSettings, exposure for 3s." << std::endl;
    sleep(3);
    // Post action of stream operation
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);

    // Turn off the device
    display_->Close();
    std::cout << "cameraDevice->Close" << std::endl;
    std::cout << "==========[test log] Close device, and preview, 3A is reset." << std::endl;
    display_->Init();
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings different 3A params
  * @tc.desc: Preview, updatesetting different 3A params together.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0031) // 3A mode white balance green scene
{
    std::cout << "==========[test log] Preview, updatesetting different 3A params together." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);
    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_TWILIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    display_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log] UpdateSettings, exposure for 10s." << std::endl;
    sleep(10);
    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: updateSettings AE
  * @tc.desc: UpdateSettings-OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0040)
{
    std::cout << "UpdateSettings-OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->cameraDevice->UpdateSettings(meta);
    sleep(5);
}

/**
  * @tc.name: updateSettings AWB without preview
  * @tc.desc: UpdateSettings-OHOS_CAMERA_AWB_MODE, without preview, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCamera3ATest, camera_3a_0041)
{
    std::cout << "UpdateSettings-OHOS_CAMERA_AWB_MODE, success." << std::endl;

    // Issue 3A parameters
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    display_->cameraDevice->UpdateSettings(meta);
}
