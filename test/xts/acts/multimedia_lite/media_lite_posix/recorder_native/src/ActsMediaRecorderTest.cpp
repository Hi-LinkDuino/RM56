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
#include "ActsMediaRecorderTest.h"

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

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0100
 * @tc.name      : Start recorder
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Start_test_001, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0200
 * @tc.name      : Start recorder without recorder sink prepare
 * @tc.desc      : [C- SOFTWARE -0200]
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
    sleep(1);
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0300
 * @tc.name      : Pasue recorder
 * @tc.desc      : [C- SOFTWARE -0200]
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0400
 * @tc.name      : Pause recorder - pause multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Pause_test_002, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0500
 * @tc.name      : Resume recorder
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Resume_test_001, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Start();
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Resume();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0600
 * @tc.name      : Resume recorder - resume multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Resume_test_002, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Start();
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Resume();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Resume();
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0700
 * @tc.name      : Stop recorder
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Stop_test_001, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Stop(true);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0800
 * @tc.name      : Stop recorder - stop multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Stop_test_002, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Stop(true);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Stop(true);
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_0900
 * @tc.name      : Reset recorder
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Reset_test_001, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Reset();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1000
 * @tc.name      : Reset recorder - Reset multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Reset_test_002, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Reset();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Reset();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1100
 * @tc.name      : Release recorder
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Release_test_001, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    EXPECT_EQ(RET_OK, retStatus);
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1200
 * @tc.name      : Reset recorder - Reset multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Release_test_002, Level1)
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    EXPECT_NE(RET_OK, retStatus);
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1300
 * @tc.name      : SetFileSplitDuration POST recording
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetFileSplitDuration_test_001, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    FileSplitType type = FILE_SPLIT_POST;
    int64_t timestamp = -1;
    uint32_t duration = 5;
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetFileSplitDuration(type, timestamp, duration);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1400
 * @tc.name      : SetFileSplitDuration PRE recording
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetFileSplitDuration_test_002, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    FileSplitType type = FILE_SPLIT_PRE;
    int64_t timestamp = -1;
    uint32_t duration = 5;
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetFileSplitDuration(type, timestamp, duration);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1500
 * @tc.name      : SetFileSplitDuration NORMAL recording
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetFileSplitDuration_test_003, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    FileSplitType type = FILE_SPLIT_NORMAL;
    int64_t timestamp = -1;
    uint32_t duration = 5;
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetFileSplitDuration(type, timestamp, duration);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1600
 * @tc.name      : SetFileSplitDuration NORMAL recording in PAUSE state
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetFileSplitDuration_test_004, Level1)
{
    int32_t retStatus = 0;
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    FileSplitType type = FILE_SPLIT_NORMAL;
    int64_t timestamp = -1;
    uint32_t duration = 5;
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
    sleep(1);
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->Pause();
    EXPECT_EQ(RET_OK, retStatus);
    retStatus = recInstance->SetFileSplitDuration(type, timestamp, duration);
    EXPECT_NE(RET_OK, retStatus);
    retStatus = recInstance->Release();
    delete recInstance;
    recInstance = nullptr;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1700
 * @tc.name      : Set Video Source ES
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSource_test_001, Level1)
{
    cout << "medialite_recorder_SetVideoSource01 starting..." << endl;
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    cout << "medialite_recorder_SetVideoSource01 ending..." << endl;
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1800
 * @tc.name      : Set Video Source YUV
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSource_test_002, Level1)
{
    cout << "medialite_recorder_SetVideoSource01 starting..." << endl;
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_YUV, sourceId);
    EXPECT_EQ(SUCCESS, ret);
    cout << "medialite_recorder_SetVideoSource01 ending..." << endl;
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_1900
 * @tc.name      : Set Video Source RGB
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSource_test_003, Level1)
{
    cout << "medialite_recorder_SetVideoSource03 starting..." << endl;
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_RGB, sourceId);
    EXPECT_EQ(RET_OK, ret);
    cout << "medialite_recorder_SetVideoSource03 ending..." << endl;
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2000
 * @tc.name      : Set Video Source invalid
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSource_test_004, Level1)
{
    cout << "medialite_recorder_SetVideoSource01 starting..." << endl;
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_BUTT, sourceId);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    cout << "medialite_recorder_SetVideoSource01 ending..." << endl;
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2100
 * @tc.name      : Set Video Source Max count 4
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSource_test_005, Level1)
{
    cout << "medialite_recorder_SetVideoSource01 starting..." << endl;
    Recorder *recorder = new Recorder();
    for (int32_t i = 0; i < g_recorderSourceMaxCount; i++) {
        int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, i);
        EXPECT_EQ(RET_OK, ret);
    }
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, g_recorderSourceMaxCount);
    EXPECT_EQ(ERR_NOFREE_CHANNEL, ret);
    cout << "medialite_recorder_SetVideoSource01 ending..." << endl;
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2200
 * @tc.name      : Set Video Encoder with video surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncoder_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoEncoder(sourceId, HEVC);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2300
 * @tc.name      : Set Video Encoder without video surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncoder_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    int32_t ret = recorder->SetVideoEncoder(sourceId, HEVC);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2400
 * @tc.name      : Set Video Encoder with VIDEO_DEFAULT
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncoder_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoEncoder(sourceId, VIDEO_DEFAULT);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2500
 * @tc.name      : Set Video Encoder with H264
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncoder_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoEncoder(sourceId, H264);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2600
 * @tc.name      : Set Video Encoder with invalid video source
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncoder_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_BUTT, sourceId);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    ret = recorder->SetVideoEncoder(sourceId, HEVC);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2700
 * @tc.name      : Set Video Size valid resolution
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSize_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t width = 1920;
    int32_t height = 1080;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoSize(sourceId, width, height);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2800
 * @tc.name      : Set Video Size invalid resolution
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSize_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t width = -1920;
    int32_t height = -1080;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoSize(sourceId, width, height);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_2900
 * @tc.name      : Set Video Size valid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSize_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    int32_t width = 1920;
    int32_t height = 1080;
    int32_t ret = recorder->SetVideoSize(sourceId, width, height);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3000
 * @tc.name      : Set Video Size unsupported resolution
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSize_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t width = 1280;
    int32_t height = 720;
    int32_t ret = recorder->SetVideoSize(sourceId, width, height);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3100
 * @tc.name      : Set Video Size invalid resolution
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoSize_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t ret = recorder->SetVideoSize(sourceId, width, height);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3200
 * @tc.name      : Set Video FrameRate with valid value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoFrameRate_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t frameRate = 30;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoFrameRate(sourceId, frameRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3300
 * @tc.name      : Set Video FrameRate with invalid value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoFrameRate_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t frameRate = -1;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoFrameRate(sourceId, frameRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3400
 * @tc.name      : Set Video FrameRate with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoFrameRate_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    int32_t frameRate = 30;
    int32_t ret = recorder->SetVideoFrameRate(sourceId, frameRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3500
 * @tc.name      : Set Video Encoding BitRate with valid info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncodingBitRate_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t rate = 4096;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoEncodingBitRate(sourceId, rate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3600
 * @tc.name      : Set Video Encoding BitRate with invalid rate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncodingBitRate_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t rate = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetVideoEncodingBitRate(sourceId, rate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3700
 * @tc.name      : Set Video Encoding BitRate with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetVideoEncodingBitRate_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    int32_t rate = 4096;
    int32_t ret = recorder->SetVideoEncodingBitRate(sourceId, rate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3800
 * @tc.name      : Set Capture Rate with valid info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetCaptureRate_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    double fps = 30.0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetCaptureRate(sourceId, fps);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_3900
 * @tc.name      : Set Capture Rate with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetCaptureRate_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    double fps = 30.0;
    int32_t ret = recorder->SetCaptureRate(sourceId, fps);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4000
 * @tc.name      : Set Capture Rate with invalid fps
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetCaptureRate_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    double fps = -30.0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetCaptureRate(sourceId, fps);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4100
 * @tc.name      : Set Audio Source with valid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSource_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4200
 * @tc.name      : Set Audio Source with invalid source
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSource_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_SOURCE_INVALID, audioSourceId);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4300
 * @tc.name      : Set Audio Source multiple times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSource_test_003, Level1)
{
    int32_t sourceId;
    int32_t ret;
    Recorder *recorder = new Recorder();
    for (int32_t i = 0; i < g_recorderSourceMaxCount; i++) {
        ret = recorder->SetAudioSource(AUDIO_MIC, sourceId);
        cout << i << endl;
        EXPECT_EQ(RET_OK, ret);
    }
    ret = recorder->SetAudioSource(AUDIO_MIC, sourceId);
    EXPECT_EQ(ERR_NOFREE_CHANNEL, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4400
 * @tc.name      : Set Audio Encoder with AAC_LC
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AAC_LC);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4500
 * @tc.name      : Set Audio Encoder with AUDIO_DEFAULT
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AUDIO_DEFAULT);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4600
 * @tc.name      : Set Audio Encoder with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 10000;
    int32_t ret = recorder->SetAudioEncoder(audioSourceId, AAC_LC);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4700
 * @tc.name      : Set Audio Encoder with valid data AAC_HE_V1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AAC_HE_V1);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4800
 * @tc.name      : Set Audio Encoder with valid data AAC_HE_V2
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AAC_HE_V2);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_4900
 * @tc.name      : Set Audio Encoder with valid data AAC_LD
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_006, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AAC_LD);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5000
 * @tc.name      : Set Audio Encoder with valid data AAC_ELD
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncoder_test_007, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncoder(audioSourceId, AAC_ELD);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5100
 * @tc.name      : Set Audio Sample Rate with valid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = 48000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5200
 * @tc.name      : Set Audio Sample Rate with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 10000;
    int32_t sampleRate = 48000;
    int32_t ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5300
 * @tc.name      : Set Audio Sample Rate with invalid sample rate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = -48000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5400
 * @tc.name      : Set Audio Sample Rate with 44.1Khz
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = 44100;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5500
 * @tc.name      : Set Audio Sample Rate with 8Khz
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = 8000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5600
 * @tc.name      : Set Audio Sample Rate with 16Khz
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_006, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = 16000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5700
 * @tc.name      : Set Audio Sample Rate with 32Khz
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioSampleRate_test_007, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t sampleRate = 32000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5800
 * @tc.name      : Set Audio Channels with valid one channel count
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 1;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_5900
 * @tc.name      : Set Audio Channels with valid two channel count
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 2;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6000
 * @tc.name      : Set Audio Channels with invalid channel count
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6100
 * @tc.name      : Set Audio Channels with valid one channel count and AUDIO_VOICE_CALL as source
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 2;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6200
 * @tc.name      : Set Audio Channels with valid two channel count and AUDIO_VOICE_CALL as source
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 1;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6300
 * @tc.name      : Set Audio Channels with invalid channel count and AUDIO_VOICE_CALL as source
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_006, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t channelCount = 0;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6400
 * @tc.name      : Set Audio Channels with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioChannels_test_007, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 100000;
    int32_t channelCount = 1;
    int32_t ret = recorder->SetAudioChannels(audioSourceId, channelCount);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6500
 * @tc.name      : Set Audio Encoding BitRate with valid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 48000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6600
 * @tc.name      : Set Audio Encoding BitRate with 44000
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 44000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6700
 * @tc.name      : Set Audio Encoding BitRate with 8000
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 8000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6800
 * @tc.name      : Set Audio Encoding BitRate with 16000
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 16000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_6900
 * @tc.name      : Set Audio Encoding BitRate with 32000
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_005, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 32000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7000
 * @tc.name      : Set Audio Encoding BitRate with invalid bitrate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_006, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = -48000;
    int32_t ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7100
 * @tc.name      : Set Audio Encoding BitRate with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_007, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 10000;
    int32_t audioEncodingBitRate = 48000;
    int32_t ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7200
 * @tc.name      : Set Audio Encoding BitRate with valid data and source AUDIO_VOICE_CALL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_008, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 48000;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7300
 * @tc.name      : Set Audio Encoding BitRate with 44000 and source AUDIO_VOICE_CALL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_009, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 44000;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7400
 * @tc.name      : Set Audio Encoding BitRate with 8000 and source AUDIO_VOICE_CALL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_010, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 8000;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7500
 * @tc.name      : Set Audio Encoding BitRate with 16000 and source AUDIO_VOICE_CALL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_011, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 16000;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7600
 * @tc.name      : Set Audio Encoding BitRate with 32000 and source AUDIO_VOICE_CALL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetAudioEncodingBitRate_test_012, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t audioSourceId = 0;
    int32_t audioEncodingBitRate = 32000;
    int32_t ret = recorder->SetAudioSource(AUDIO_VOICE_CALL, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7700
 * @tc.name      : Prepare recorder with valid info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_Prepare_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t audioSourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->SetAudioSource(AUDIO_MIC, audioSourceId);
    EXPECT_EQ(RET_OK, ret);
    ret = recorder->Prepare();
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7800
 * @tc.name      : GetSurface with valid info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_GetSurface_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 0;
    int32_t ret = recorder->SetVideoSource(VIDEO_SOURCE_SURFACE_ES, sourceId);
    EXPECT_EQ(RET_OK, ret);
    Surface *surface = recorder->GetSurface(sourceId).get();
    ret = (int32_t)(surface == nullptr);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_7900
 * @tc.name      : GetSurface with invalid source ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_GetSurface_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t sourceId = 10000;
    Surface *surface = recorder->GetSurface(sourceId).get();
    EXPECT_EQ(nullptr, surface);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8000
 * @tc.name      : Set Max Duration with valid value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxDuration_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxDuration(1000);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8100
 * @tc.name      : Set Max Duration with zero value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxDuration_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxDuration(0);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8200
 * @tc.name      : Set Max Duration with invalid value
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxDuration_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxDuration(-1);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8300
 * @tc.name      : Set Max Duration with invalid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxDuration_test_004, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxDuration(4294967295);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8400
 * @tc.name      : Set Max File Size with invalid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxFileSize_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxFileSize(0);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8500
 * @tc.name      : Set Max File Size with valid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetMaxFileSize_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetMaxFileSize(2000);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8600
 * @tc.name      : Set Output File with invalid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetOutputFile_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetOutputFile(-1);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8700
 * @tc.name      : Set Next Output File with invalid data
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetNextOutputFile_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetNextOutputFile(-1);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8800
 * @tc.name      : Set Output Path with empty path
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetOutputPath_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    const string path = "";
    int32_t ret = recorder->SetOutputPath(path);
    EXPECT_EQ(ERR_INVALID_PARAM, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_8900
 * @tc.name      : Set Output Path with FORMAT_DEFAULT
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetOutputFormat_test_001, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetOutputFormat(FORMAT_DEFAULT);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_9000
 * @tc.name      : Set Output Path with FORMAT_MPEG_4
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetOutputFormat_test_002, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetOutputFormat(FORMAT_MPEG_4);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}

/* *
 * @tc.number    : SUB_MEDIA_REC_TEST_9100
 * @tc.name      : Set Output Path with FORMAT_TS
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(RecoderLiteTest, medialite_recorder_SetOutputFormat_test_003, Level1)
{
    Recorder *recorder = new Recorder();
    int32_t ret = recorder->SetOutputFormat(FORMAT_TS);
    EXPECT_EQ(RET_OK, ret);
    delete recorder;
    recorder = NULL;
}
} // namespace OHOS
