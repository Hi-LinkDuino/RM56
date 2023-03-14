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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio-related APIs, including custom data types and functions for capture drivers function.
 * accessing a driver adapter, and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hdi_common.h
 *
 * @brief Declares APIs for operations related to the capturing audio adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"
#include "audio_hdicapture_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_HDMI = "hdmi";
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";
const int BUFFER_SIZE = 16384;
const int BUFFER_SIZE_LITTLE = 0;
const uint64_t FILESIZE = 1024;

class AudioHdiCaptureTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static TestAudioManager *(*GetAudioManager)();
    static void *handleSo;
#ifdef AUDIO_MPI_SO
    static int32_t (*SdkInit)();
    static void (*SdkExit)();
    static void *sdkSo;
#endif

    static int32_t GetLoadAdapterAudioPara(struct PrepareAudioPara& audiopara);
};

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiCaptureTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureTest::SetUpTestCase(void)
{
#ifdef AUDIO_MPI_SO
    char sdkResolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_render");
    sdkSo = dlopen(sdkResolvedPath, RTLD_LAZY);
    if (sdkSo == nullptr) {
        return;
    }
    SdkInit = (int32_t (*)())(dlsym(sdkSo, "MpiSdkInit"));
    if (SdkInit == nullptr) {
        return;
    }
    SdkExit = (void (*)())(dlsym(sdkSo, "MpiSdkExit"));
    if (SdkExit == nullptr) {
        return;
    }
    SdkInit();
#endif
    char absPath[PATH_MAX] = {0};
    if (realpath(RESOLVED_PATH.c_str(), absPath) == nullptr) {
        return;
    }
    handleSo = dlopen(absPath, RTLD_LAZY);
    if (handleSo == nullptr) {
        return;
    }
    GetAudioManager = (TestAudioManager *(*)())(dlsym(handleSo, FUNCTION_NAME.c_str()));
    if (GetAudioManager == nullptr) {
        return;
    }
}

void AudioHdiCaptureTest::TearDownTestCase(void)
{
#ifdef AUDIO_MPI_SO
    SdkExit();
    if (sdkSo != nullptr) {
        dlclose(sdkSo);
        sdkSo = nullptr;
    }
    if (SdkInit != nullptr) {
        SdkInit = nullptr;
    }
    if (SdkExit != nullptr) {
        SdkExit = nullptr;
    }
#endif
    if (handleSo != nullptr) {
        dlclose(handleSo);
        handleSo = nullptr;
    }
    if (GetAudioManager != nullptr) {
        GetAudioManager = nullptr;
    }
}


void AudioHdiCaptureTest::SetUp(void) {}

void AudioHdiCaptureTest::TearDown(void) {}


int32_t AudioHdiCaptureTest::GetLoadAdapterAudioPara(struct PrepareAudioPara& audiopara)
{
    int32_t ret = -1;
    int size = 0;
    auto *inst = (AudioHdiCaptureTest *)audiopara.self;
    if (inst != nullptr && inst->GetAudioManager != nullptr) {
        audiopara.manager = inst->GetAudioManager();
    }
    if (audiopara.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = audiopara.manager->GetAllAdapters(audiopara.manager, &audiopara.descs, &size);
    if (ret < 0) {
        return ret;
    }
    if (audiopara.descs == nullptr || size == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int index = SwitchAdapter(audiopara.descs, audiopara.adapterName,
        audiopara.portType, audiopara.audioPort, size);
    if (index < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    } else {
        audiopara.desc = &audiopara.descs[index];
    }
    if (audiopara.desc == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    } else {
        ret = audiopara.manager->LoadAdapter(audiopara.manager, audiopara.desc, &audiopara.adapter);
    }
    if (ret < 0) {
        return ret;
    }
    if (audiopara.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}


/**
* @tc.name  Test AudioCaptureCaptureFrame API via legal input
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0001
* @tc.desc  test AudioCaptureCaptureFrame interface,Returns 0 if the input data is read successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t replyBytes = 0;
    uint64_t requestBytes = BUFFER_SIZE;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    char *frame = (char *)calloc(1, BUFFER_SIZE);
    EXPECT_NE(nullptr, frame);
    ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
}
/**
* @tc.name  Test AudioCaptureCaptureFrame API via setting the incoming parameter frame is nullptr
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0002
* @tc.desc  test AudioCaptureCaptureFrame interface,Returns -1 if the incoming parameter frame is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0002, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t replyBytes = 0;
    uint64_t requestBytes = BUFFER_SIZE;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    char *frame = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureCaptureFrame API via setting the incoming parameter replyBytes is nullptr
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0003
* @tc.desc  test AudioCaptureCaptureFrame interface,Returns -1 if the incoming parameter replyBytes is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0003, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t requestBytes = BUFFER_SIZE;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    uint64_t *replyBytes = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    char *frame = (char *)calloc(1, BUFFER_SIZE);
    EXPECT_NE(nullptr, frame);
    ret = capture->CaptureFrame(capture, frame, requestBytes, replyBytes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
}
/**
* @tc.name  Test AudioCaptureCaptureFrame API via setting the incoming parameter capture is nullptr
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0004
* @tc.desc  test AudioCaptureCaptureFrame interface,Returns -1 if the incoming parameter capture is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0004, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t requestBytes = BUFFER_SIZE;
    uint64_t replyBytes = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    char *frame = (char *)calloc(1, BUFFER_SIZE);
    EXPECT_NE(nullptr, frame);
    ret = capture->CaptureFrame(captureNull, frame, requestBytes, &replyBytes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
}
/**
* @tc.name  Test AudioCaptureFrame API without calling interface capturestart
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0005
* @tc.desc  Test AudioCaptureFrame interface,Returns -1 if without calling interface capturestart
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0005, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t requestBytes = BUFFER_SIZE;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    uint64_t replyBytes = 0;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    char *frame = (char *)calloc(1, BUFFER_SIZE);
    EXPECT_NE(nullptr, frame);
    ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
}
/**
* @tc.name  Test AudioCaptureCaptureFrame API via setting the incoming parameter requestBytes
less than interface requirements
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_0006
* @tc.desc  test AudioCaptureCaptureFrame interface,Returns -1 if the incoming parameter
requestBytes less than interface requirements
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureFrame_0006, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t requestBytes = BUFFER_SIZE_LITTLE;
    uint64_t replyBytes = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    char *frame = (char *)calloc(1, BUFFER_SIZE);
    EXPECT_NE(nullptr, frame);
    ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
}
/**
* @tc.name  Test AudioCaptureGetCapturePosition API via legal input
* @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0001
* @tc.desc  Test AudioCaptureGetCapturePosition interface,Returns 0 if get CapturePosition during playing.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->GetCapturePosition(audiopara.capture, &frames, &time);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
        EXPECT_GT(frames, INITIAL_VALUE);
    }

    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Test GetCapturePosition API via get CapturePosition after the audiois Paused and resumed
* @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0002
* @tc.desc   Test GetCapturePosition interface,Returns 0 if get Position after Pause and resume during playing
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0002, TestSize.Level1)
{
    int32_t ret = -1;
    int64_t timeExp = 0;
    uint64_t frames = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->control.Pause((AudioHandle)(audiopara.capture));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.capture->GetCapturePosition(audiopara.capture, &frames, &time);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
        EXPECT_GT(frames, INITIAL_VALUE);
        ret = audiopara.capture->control.Resume((AudioHandle)(audiopara.capture));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.capture->GetCapturePosition(audiopara.capture, &frames, &time);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
        EXPECT_GT(frames, INITIAL_VALUE);
    }

    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Test GetCapturePosition API via get CapturePosition after the audio file is stopped
* @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0003
* @tc.desc  Test GetCapturePosition interface,Returns 0 if get CapturePosition after stop during playing
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    uint64_t frames = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    int64_t timeExp = 0;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via get CapturePosition after the object is created
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0004
    * @tc.desc  Test GetCapturePosition interface, return 0 if get CapturePosition after the object is created
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0004, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    uint64_t frames = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    int64_t timeExp = 0;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via setting the parameter Capture is nullptr
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0005
    * @tc.desc  Test GetCapturePosition interface, return -1 if setting the parameter Capture is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0005, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    uint64_t frames = 0;
    struct AudioTimeStamp time = {};

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(captureNull, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via setting the parameter frames is nullptr
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0006
    * @tc.desc  Test GetCapturePosition interface, return -1 if setting the parameter frames is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0006, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    uint64_t *framesNull = nullptr;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, framesNull, &time);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via setting the parameter time is nullptr
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0007
    * @tc.desc  Test GetCapturePosition interface, return -1 if setting the parameter time is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0007, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    uint64_t frames = 0;
    struct AudioTimeStamp *timeNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, timeNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via get CapturePosition continuously
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0008
    * @tc.desc  Test GetCapturePosition interface, return 0 if the GetCapturePosition was called twice
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0008, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = {};
    struct AudioCapture *capture = nullptr;
    uint64_t frames = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    struct AudioTimeStamp timeSec = {.tvSec = 0, .tvNSec = 0};
    int64_t timeExp = 0;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);
    ret = capture->GetCapturePosition(capture, &frames, &timeSec);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via define format to AUDIO_FORMAT_PCM_16_BIT
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0009
    * @tc.desc  Test GetCapturePosition interface,return 0 if get framesize define format to AUDIO_FORMAT_PCM_16_BIT
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0009, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t channelCountExp = 2;
    uint32_t sampleRateExp = 48000;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioSampleAttributes attrsValue = {};
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    attrs.type = AUDIO_IN_MEDIA;
    attrs.interleaved = false;
    attrs.format = AUDIO_FORMAT_PCM_16_BIT;
    attrs.sampleRate = 48000;
    attrs.channelCount = 2;
    ret = capture->attr.SetSampleAttributes(capture, &attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->attr.GetSampleAttributes(capture, &attrsValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(sampleRateExp, attrsValue.sampleRate);
    EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, attrsValue.format);
    EXPECT_EQ(channelCountExp, attrsValue.channelCount);

    ret = AudioCaptureStartAndOneFrame(capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via define format to AUDIO_FORMAT_PCM_24_BIT
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0010
    * @tc.desc  Test GetCapturePosition interface,return 0 if get framesize define format to AUDIO_FORMAT_PCM_24_BIT
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0010, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioSampleAttributes attrsValue = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    uint64_t channelCountExp = 2;
    uint32_t sampleRateExp = 48000;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    attrs.type = AUDIO_IN_MEDIA;
    attrs.interleaved = false;
    attrs.format = AUDIO_FORMAT_PCM_24_BIT;
    attrs.sampleRate = 48000;
    attrs.channelCount = 2;
    ret = capture->attr.SetSampleAttributes(capture, &attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->attr.GetSampleAttributes(capture, &attrsValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(AUDIO_FORMAT_PCM_24_BIT, attrsValue.format);
    EXPECT_EQ(channelCountExp, attrsValue.channelCount);
    EXPECT_EQ(sampleRateExp, attrsValue.sampleRate);

    ret = AudioCaptureStartAndOneFrame(capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via define sampleRate and channelCount to different value
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0011
    * @tc.desc  Test GetCapturePosition interface,return 0 if get framesize define channelCount  as different values
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0011, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioSampleAttributes attrsValue = {};
    uint64_t channelCountExp = 1;
    uint32_t sampleRateExp = 48000;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    attrs.type = AUDIO_IN_MEDIA;
    attrs.interleaved = false;
    attrs.format = AUDIO_FORMAT_PCM_16_BIT;
    attrs.sampleRate = 48000;
    attrs.channelCount = 1;
    ret = capture->attr.SetSampleAttributes(capture, &attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->attr.GetSampleAttributes(capture, &attrsValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, attrsValue.format);
    EXPECT_EQ(sampleRateExp, attrsValue.sampleRate);
    EXPECT_EQ(channelCountExp, attrsValue.channelCount);

    ret = AudioCaptureStartAndOneFrame(capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test GetCapturePosition API via define sampleRate and channelCount to 1
    * @tc.number  SUB_Audio_HDI_AudioCaptureGetCapturePosition_0012
    * @tc.desc  Test GetCapturePosition interface,return 0 if get framesize define channelCount to 1
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_AudioCaptureGetCapturePosition_0012, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t channelCountExp = 1;
    uint32_t sampleRateExp = 48000;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioSampleAttributes attrsValue = {};
    struct AudioTimeStamp time = {.tvSec = 0, .tvNSec = 0};
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    attrs.type = AUDIO_IN_MEDIA;
    attrs.interleaved = false;
    attrs.format = AUDIO_FORMAT_PCM_24_BIT;
    attrs.sampleRate = 48000;
    attrs.channelCount = 1;
    ret = capture->attr.SetSampleAttributes(capture, &attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->attr.GetSampleAttributes(capture, &attrsValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(channelCountExp, attrsValue.channelCount);
    EXPECT_EQ(AUDIO_FORMAT_PCM_24_BIT, attrsValue.format);
    EXPECT_EQ(sampleRateExp, attrsValue.sampleRate);

    ret = AudioCaptureStartAndOneFrame(capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->GetCapturePosition(capture, &frames, &time);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((time.tvSec) * SECTONSEC + (time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via legal input
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0001
* @tc.desc  Test ReqMmapBuffer interface,return 0 if call ReqMmapBuffer interface successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    fclose(fp);
    if (ret == 0) {
        munmap(desc.memoryAddress, reqSize);
    }
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via setting the incoming parameter reqSize is bigger than
            the size of actual audio file
* @tc.number  SUB_Audio_HDI_RenderReqMmapBuffer_0002
* @tc.desc  Test ReqMmapBuffer interface,return -3 if call ReqMmapBuffer interface unsuccessful when setting the
            incoming parameter reqSize is bigger than the size of actual audio file
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    reqSize = reqSize + BUFFER_LENTH;
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    fclose(fp);
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via setting the incoming parameter reqSize is smaller than
            the size of actual audio file
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0003
* @tc.desc  Test ReqMmapBuffer interface,return 0 if call ReqMmapBuffer interface successfully when setting the
            incoming parameter reqSize is smaller than the size of actual audio file
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0003, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    reqSize = reqSize / 2;
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    fclose(fp);
    if (ret == 0) {
        munmap(desc.memoryAddress, reqSize);
    }
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via setting the incoming parameter reqSize is zero
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0004
* @tc.desc  Test ReqMmapBuffer interface,return -1 if call ReqMmapBuffer interface unsuccessful when setting the
            incoming parameter reqSize is zero
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0004, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    reqSize = 0;
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    fclose(fp);
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via setting the incoming parameter memoryFd  of desc is illegal
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0005
* @tc.desc  Test ReqMmapBuffer interface,return -3 if call ReqMmapBuffer interface unsuccessful when setting the
            incoming parameter memoryFd  of desc is illegal
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0005, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager *manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    desc.memoryFd = 1;
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    fclose(fp);
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0006
* @tc.desc  Test ReqMmapBuffer interface,return -3 if call ReqMmapBuffer interface unsuccessful when setting the
            incoming parameter handle is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0006, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = false;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    FILE *fp = fopen(AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str(), "wb+");
    ASSERT_NE(nullptr, fp);
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)captureNull, reqSize, &desc);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    fclose(fp);
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via the incoming parameter desc is nullptr
* @tc.number  SUB_Audio_HDI_CaptureReqMmapBuffer_0007
* @tc.desc  Test ReqMmapBuffer interface,return -3 if call ReqMmapBuffer interface unsuccessful when setting the
            incoming parameter desc is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureReqMmapBuffer_0007, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t reqSize = 0;
    struct AudioMmapBufferDescripter *descNull = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_USB, &adapter, &capture);
    if (ret < 0 || capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, capture);
    }
    reqSize = FILE_CAPTURE_SIZE;
    ret =  capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret =  capture->attr.ReqMmapBuffer((AudioHandle)capture, reqSize, descNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test GetMmapPosition API via Getting position is normal in Before recording , recording and after recording
* @tc.number  SUB_Audio_HDI_CaptureGetMmapPosition_0001
* @tc.desc  Test GetMmapPosition interface,return 0 if Getting position successfully.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureGetMmapPosition_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    uint64_t framesCapturing = 0;
    uint64_t framesExpCapture = 0;
    int64_t timeExp = 0;
    int64_t timeExpCaptureing = 0;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str()
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0 || audiopara.capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, audiopara.capture);
    }
    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, &frames, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ((audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec), timeExp);
    EXPECT_EQ(frames, INITIAL_VALUE);
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordMapAudio, &audiopara);
    if (ret != 0) {
        audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    sleep(1);
    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, &framesCapturing, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec), timeExp);
    EXPECT_GT(framesCapturing, INITIAL_VALUE);
    timeExpCaptureing = (audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec);
    void *result = nullptr;
    pthread_join(audiopara.tids, &result);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, &framesExpCapture, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec), timeExpCaptureing);
    EXPECT_GT(framesExpCapture, framesCapturing);

    audiopara.capture->control.Stop((AudioHandle)audiopara.capture);
    audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
}
/**
* @tc.name  Test GetMmapPosition API via SetSampleAttributes and Getting position is normal.
* @tc.number  SUB_Audio_HDI_CaptureGetMmapPosition_0002
* @tc.desc  Test GetMmapPosition interface,return 0 if Getting position successfully.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureGetMmapPosition_0002, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str()
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0 || audiopara.capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, audiopara.capture);
    }
    InitAttrs(audiopara.attrs);
    audiopara.attrs.format = AUDIO_FORMAT_PCM_24_BIT;
    audiopara.attrs.channelCount = 1;
    ret = audiopara.capture->attr.SetSampleAttributes(audiopara.capture, &(audiopara.attrs));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordMapAudio, &audiopara);
    if (ret != 0) {
        audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    void *result = nullptr;
    pthread_join(audiopara.tids, &result);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);

    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, &frames, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_GT((audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec), timeExp);
    EXPECT_GT(frames, INITIAL_VALUE);

    audiopara.capture->control.Stop((AudioHandle)audiopara.capture);
    audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Test ReqMmapBuffer API via inputtint frames is nullptr.
* @tc.number  SUB_Audio_HDI_CaptureGetMmapPosition_0003
* @tc.desc  Test GetMmapPosition interface,return -3 if Error in incoming parameter.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureGetMmapPosition_0003, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t *frames = nullptr;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str()
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0 || audiopara.capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, audiopara.capture);
    }

    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, frames, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via inputtint time is nullptr.
* @tc.number  SUB_Audio_HDI_CaptureGetMmapPosition_0004
* @tc.desc  Test GetMmapPosition interface,return -3 if Error in incoming parameter.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureGetMmapPosition_0004, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    struct AudioTimeStamp *time = nullptr;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str()
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0 || audiopara.capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, audiopara.capture);
    }

    ret = audiopara.capture->attr.GetMmapPosition(audiopara.capture, &frames, time);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
}
/**
* @tc.name  Test ReqMmapBuffer API via inputtint capture is nullptr.
* @tc.number  SUB_Audio_HDI_RenderReqMmapBuffer_0005
* @tc.desc  Test GetMmapPosition interface,return -3 if Error in incoming parameter.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureTest, SUB_Audio_HDI_CaptureGetMmapPosition_0005, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    struct AudioCapture *captureNull = nullptr;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_LOW_LATENCY_CAPTURE_FILE.c_str()
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0 || audiopara.capture == nullptr) {
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ASSERT_EQ(nullptr, audiopara.capture);
    }

    ret = audiopara.capture->attr.GetMmapPosition(captureNull, &frames, &(audiopara.time));
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
}
}