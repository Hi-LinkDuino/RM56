/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "recorder_lite_test.h"

#include "media_info.h"
#include "media_errors.h"
#include "recorder.h"

#include <iostream>

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
const int32_t RET_OK = 0;
const int32_t RET_NOK = -1;
static int32_t g_recorderSourceMaxCount = 4; // max recorder source setting

void RecoderLiteTest::SetUpTestCase(void) {}

void RecoderLiteTest::TearDownTestCase(void) {}

void RecoderLiteTest::SetUp() {}

void RecoderLiteTest::TearDown() {}

void RecoderLiteTest::OnError(const int32_t errorType, const int32_t errorCode)
{
    cout << "RecoderLiteTest::OnError ..." << endl;
}

void RecoderLiteTest::OnInfo(const int32_t type, const int32_t extra)
{
    cout << "RecoderLiteTest::OnInfo ..." << endl;
}

namespace Media {
void TestVideoRecorderCallback::OnError(int32_t errorType, int32_t errorCode)
{
    cout << "TestVideoRecorderCallback::OnError ..." << endl;
}

void TestVideoRecorderCallback::OnInfo(int32_t type, int32_t extra)
{
    cout << "TestVideoRecorderCallback::OnInfo ..." << endl;
}
}

/*
 * Feature: Recorder
 * Function: Start Recording
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Start recorder without recorder sink prepare
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Start_test_002, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    Recorder *recInstance = new Recorder();
    shared_ptr<TestVideoRecorderCallback> cb = std::make_shared<TestVideoRecorderCallback> ();

    retStatus = recInstance->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetRecorderCallback(cb);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Start();
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/*
 * Feature: Recorder
 * Function: Pause Recording
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Pasue recorder
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Pause_test_001, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    Recorder *recInstance = new Recorder();
    shared_ptr<TestVideoRecorderCallback> cb = std::make_shared<TestVideoRecorderCallback> ();

    retStatus = recInstance->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Prepare();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetRecorderCallback(cb);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Start();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}
} // namespace OHOS
