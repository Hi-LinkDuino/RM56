/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "core/components/camera/camera_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string CALLBACK_SUCESS = "success";
const std::string CALLBACK_FAIL = "fail";
const std::string CALLBACK_COMPLETE = "complete";
const std::string DEFAULT_RECORD_ID = "0";

}

class CameraComponentTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "CameraComponentTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "CameraComponentTest TearDownTestCase";
    }

    void SetUp() override
    {
        camera_ = AceType::MakeRefPtr<CameraComponent>();
    }

    void TearDown() override
    {
        camera_ = nullptr;
    }

protected:
    RefPtr<CameraComponent> camera_;
};

/**
 * @tc.name: CameraComponentTest001
 * @tc.desc: Checking the default value of cameracomponent
 * @tc.type: FUNC
 */
HWTEST_F(CameraComponentTest, CameraComponentTest001, TestSize.Level1)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<CameraComponent>();
    RefPtr<CameraController> controller = cameraComponent->GetCameraController();
    ASSERT_TRUE(controller != nullptr);
}

/**
 * @tc.name: CameraComponentTest002
 * @tc.desc: Checking the default value of cameracomponent
 * @tc.type: FUNC
 */
HWTEST_F(CameraComponentTest, CameraComponentTest002, TestSize.Level1)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<CameraComponent>();
    ASSERT_TRUE(cameraComponent->GetFlash() == FlashType::AUTO);
    ASSERT_TRUE(cameraComponent->GetDevicePosition() == DevicePosition::CAMERA_FACING_BACK);
}

/**
 * @tc.name: CameraComponentTest003
 * @tc.desc: Verify the takePhoto works fine when triggering camera takephoto.
 * @tc.type: FUNC
 */
HWTEST_F(CameraComponentTest, CameraComponentTest003, TestSize.Level1)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<CameraComponent>();
    RefPtr<CameraController> controller = cameraComponent->GetCameraController();
    controller->SetTakePhotoImpl([](const TakePhotoParams& params) {
        ASSERT_TRUE(params.success == CALLBACK_SUCESS);
        ASSERT_TRUE(params.fail == CALLBACK_FAIL);
        ASSERT_TRUE(params.complete == CALLBACK_COMPLETE);
    });
    TakePhotoParams takePhotoParams;
    takePhotoParams.success = CALLBACK_SUCESS;
    takePhotoParams.fail = CALLBACK_FAIL;
    takePhotoParams.complete = CALLBACK_COMPLETE;
    controller->TakePhoto(takePhotoParams);
}

/**
 * @tc.name: CameraComponentTest004
 * @tc.desc: Verify the StartRecord interface works fine when triggering camera StartRecord.
 * @tc.type: FUNC
 */
HWTEST_F(CameraComponentTest, CameraComponentTest004, TestSize.Level1)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<CameraComponent>();
    RefPtr<CameraController> controller = cameraComponent->GetCameraController();
    bool isStartRecord = false;
    controller->SetStartRecordImpl([&isStartRecord]() {
        isStartRecord = true;
    });
    controller->StartRecord();

    ASSERT_TRUE(isStartRecord);
}

/**
 * @tc.name: CameraComponentTest005
 * @tc.desc: Verify the CloseRecorder interface works fine when triggering camera StartRecord.
 * @tc.type: FUNC
 */
HWTEST_F(CameraComponentTest, CameraComponentTest005, TestSize.Level1)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<CameraComponent>();
    RefPtr<CameraController> controller = cameraComponent->GetCameraController();
    bool isCloseRecord = false;
    controller->SetCloseRecorderImpl([&isCloseRecord](const std::string& params) {
        isCloseRecord = true;
    });
    controller->CloseRecorder(DEFAULT_RECORD_ID);

    ASSERT_TRUE(isCloseRecord);
}

} // namespace OHOS::Ace
