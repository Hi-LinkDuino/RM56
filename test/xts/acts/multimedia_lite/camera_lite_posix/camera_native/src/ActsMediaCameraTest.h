/*
 * Copyright (C) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef CAMERA_LITE_TEST_H
#define CAMERA_LITE_TEST_H

#include "gtest/gtest.h"
#include "recorder.h"

namespace OHOS {
const int32_t RET_OK = 0;
const int32_t RET_ERR = -1;
const int32_t FRAME_RATE_DEFAULT = 30;
const int32_t OK = 0;
const int32_t FAIL = -1;
const int32_t QUEUE_SIZE = 3;
const int32_t BUFFER_SIZE = 1024;
static std::string g_filePath = "/test_root/multimedia/";
std::string g_testPath;

bool g_onGetCameraAbilityFlag;
bool g_onConfigureFlag;
bool g_onGetSupportedSizesFlag;
// CameraDeviceCallBack Flag
bool g_onCameraAvailableFlag;
bool g_onCameraUnavailableFlag;
// CameraStateCallback
bool g_onCreatedFlag;
bool g_onCreateFailedFlag;
bool g_onReleasedFlag;
bool g_onConfiguredFlag;
bool g_onConfigureFailedFlag;
// FrameStateCallback
bool g_onCaptureTriggerAbortedFlag;
bool g_onCaptureTriggerCompletedFlag;
bool g_onCaptureTriggerStartedFlag;
bool g_onGetFrameConfigureType;
bool g_onFrameFinishedFlag;
bool g_onFrameErrorFlag;
bool g_onFrameProgressedFlag;
bool g_onFrameStartedFlag;
bool g_onRecorderFlag;
bool g_onPreviewFlag;

// VideoSize
enum TestVideoSize {
    WIDTH = 1920,
    HEIGHT = 1080,
};

class CameraLiteTest : public testing::Test {
public:
    // SetUpTestCase: before all testcasee
    static void SetUpTestCase(void);
    // TearDownTestCase: after all testcase
    static void TearDownTestCase(void);
    // SetUp
    void SetUp(void);
    // TearDown
    void TearDown(void);
};
} // namespace OHOS
#endif // CAMERA_LITE_TEST_H