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
#include "audio_hdicapture_volume_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_HDMI = "hdmi";
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioHdiCaptureVolumeTest : public testing::Test {
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
    int32_t AudioCaptureStart(const string path, struct AudioCapture *capture) const;
};

TestAudioManager *(*AudioHdiCaptureVolumeTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureVolumeTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureVolumeTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureVolumeTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureVolumeTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureVolumeTest::SetUpTestCase(void)
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

void AudioHdiCaptureVolumeTest::TearDownTestCase(void)
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

void AudioHdiCaptureVolumeTest::SetUp(void) {}
void AudioHdiCaptureVolumeTest::TearDown(void) {}

int32_t AudioHdiCaptureVolumeTest::AudioCaptureStart(const string path, struct AudioCapture *capture) const
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    if (capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    InitAttrs(attrs);
    FILE *file = fopen(path.c_str(), "wb+");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    ret = FrameStartCapture(capture, file, attrs);
    fclose(file);
    return ret;
}

/**
* @tc.name  Test AudioCaptureSetMute API via legal input.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetMute_0001
* @tc.desc  Test AudioCaptureSetMute interface , return 0 if the audiocapture object sets mute successfully.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetMute_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteTrue = true;
    bool muteFalse = false;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetMute(capture, muteTrue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetMute(capture, &muteTrue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_TRUE(muteTrue);

    ret = capture->volume.SetMute(capture, muteFalse);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetMute(capture, &muteFalse);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_FALSE(muteFalse);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetMute API via setting the capture is empty .
* @tc.number  SUB_Audio_HDI_AudioCaptureSetMute_0002
* @tc.desc  Test AudioCaptureSetMute interface, return -1 if the capture is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetMute_0002, TestSize.Level1)
{
    bool muteTrue = true;
    bool muteFalse = false;
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.SetMute(captureNull, muteTrue);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->volume.SetMute(captureNull, muteFalse);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetMute API,when the parameter mutevalue equals 2.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetMute_0003
* @tc.desc  Test AudioCaptureSetMute interface and set the parameter mutevalue with 2.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetMute_0003, TestSize.Level1)
{
    bool muteValue = 2;
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetMute(capture, muteValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetMute(capture, &muteValue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_TRUE(muteValue);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetMute API via legal input.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetMute_0001
* @tc.desc  Test AudioCaptureGetMute interface , return 0 if the audiocapture gets mute successfully.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureGetMute_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteTrue = true;
    bool muteFalse = false;
    bool defaultmute = true;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetMute(capture, &muteTrue);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(muteTrue, defaultmute);

    ret = capture->volume.SetMute(capture, muteFalse);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetMute(capture, &muteFalse);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_FALSE(muteFalse);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test interface AudioCaptureGetMute when capture is empty.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetMute_0002
* @tc.desc  Test AudioCreateCapture interface, return -1 if the capture is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureGetMute_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteTrue = true;
    bool muteFalse = false;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetMute(captureNull, &muteTrue);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->volume.GetMute(captureNull, &muteFalse);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->volume.GetMute(capture, nullptr);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetVolume API via legal input.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetVolume_0001
* @tc.desc  Test AudioCaptureSetVolume interface , return 0 if the audiocapture sets volume successfully.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetVolume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeInit = 0.30;
    float volumeInitExpc = 0.30;
    float volumeLow = 0.10;
    float volumeLowExpc = 0.10;
    float volumeMid = 0.40;
    float volumeMidExpc = 0.40;
    float volumeHigh = 0.70;
    float volumeHighExpc = 0.70;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.SetVolume(capture, volumeInit);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeInit);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeInitExpc, volumeInit);
    ret = capture->volume.SetVolume(capture, volumeLow);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeLow);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeLowExpc, volumeLow);
    ret = capture->volume.SetVolume(capture, volumeMid);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeMid);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeMidExpc, volumeMid);
    ret = capture->volume.SetVolume(capture, volumeHigh);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeHigh);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeHighExpc, volumeHigh);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetVolume,when volume is set maximum value or minimum value.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetVolume_0002
* @tc.desc  Test AudioCaptureSetVolume,return 0 if volume is set maximum value or minimum value.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetVolume_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeMin = 0;
    float volumeMinExpc = 0;
    float volumeMax = 1.0;
    float volumeMaxExpc = 1.0;
    float volumeMinBoundary = -1;
    float volumeMaxBoundary = 1.1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetVolume(capture, volumeMin);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeMin);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeMinExpc, volumeMin);

    ret = capture->volume.SetVolume(capture, volumeMax);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volumeMax);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(volumeMaxExpc, volumeMax);

    ret = capture->volume.SetVolume(capture, volumeMinBoundary);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->volume.SetVolume(capture, volumeMaxBoundary);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetVolume,when capture is empty.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetVolume_0003
* @tc.desc  Test AudioCaptureSetVolume,return -1 when capture is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureSetVolume_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float volume = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetVolume(captureNull, volume);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetVolume API via legal input.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetVolume_001
* @tc.desc  Test AudioCaptureGetVolume interface , return 0 if the audiocapture is get successful.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureGetVolume_001, TestSize.Level1)
{
    int32_t ret = -1;
    float volume = 0.60;
    float defaultVolume = 0.60;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetVolume(capture, volume);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volume);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(defaultVolume, volume);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetVolume when when capturing is in progress.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetVolume_002.
* @tc.desc  Test AudioCaptureGetVolume,return 0 when when capturing is in progress.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureGetVolume_002, TestSize.Level1)
{
    int32_t ret = -1;
    float volume = 0.60;
    float defaultVolume = 0.60;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = AudioCaptureStart(AUDIO_CAPTURE_FILE, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetVolume(capture, volume);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetVolume(capture, &volume);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(defaultVolume, volume);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetVolume,when capture is empty.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetVolume_0003
* @tc.desc  Test AudioCaptureGetVolume,return -1 when capture is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_HDI_AudioCaptureGetVolume_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float volume = 0.30;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetVolume(captureNull, &volume);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGainThreshold API via legal input
* @tc.number  SUB_Audio_hdi_CaptureGetGainThreshold_0001
* @tc.desc  test AudioCaptureGetGainThreshold interface, return 0 is call successfully.
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGainThreshold_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float max = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGainThreshold((AudioHandle)capture, &min, &max);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(min, GAIN_MIN);
    EXPECT_EQ(max, GAIN_MAX);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGainThreshold API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_hdi_CaptureGetGainThreshold_0002
* @tc.desc  test AudioCaptureGetGainThreshold interface, return -1 if the incoming parameter handle is nullptr.
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGainThreshold_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float max = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGainThreshold((AudioHandle)captureNull, &min, &max);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGainThreshold API via setting the incoming parameter min is nullptr
* @tc.number  SUB_Audio_hdi_CaptureGetGainThreshold_0003
* @tc.desc  test AudioCaptureGetGainThreshold interface, return -1 if the incoming parameter min is nullptr.
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGainThreshold_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float max = 0;
    float* minNull = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGainThreshold((AudioHandle)capture, minNull, &max);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGainThreshold API via setting the incoming parameter max is nullptr
* @tc.number  SUB_Audio_hdi_CaptureGetGainThreshold_0004
* @tc.desc  test AudioCaptureGetGainThreshold interface, return -1 if the incoming parameter max is nullptr.
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGainThreshold_0004, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float* maxNull = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGainThreshold((AudioHandle)capture, &min, maxNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetGain API via legal input
* @tc.number  SUB_Audio_hdi_CaptureSetGain_0001
* @tc.desc  test AudioCaptureSetGain interface, return 0 is call successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureSetGain_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float max = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGainThreshold((AudioHandle)capture, &min, &max);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    float gain = max - 1;
    float gainMax = max;
    float gainMin = min;
    float gainExpc = max - 1;
    float gainMaxExpc = max;
    float gainMinExpc = min;
    ret = capture->volume.SetGain((AudioHandle)capture, gainMax);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)capture, &gainMax);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(gainMaxExpc, gainMax);

    ret = capture->volume.SetGain((AudioHandle)capture, gainMin);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)capture, &gainMin);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(gainMinExpc, gainMin);

    ret = capture->volume.SetGain((AudioHandle)capture, gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)capture, &gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(gainExpc, gain);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetGain API via setting gain greater than the maximum and less than the minimum
* @tc.number  SUB_Audio_hdi_CaptureSetGain_0002
* @tc.desc  test AudioCaptureSetGain interface, return -1 if gain greater than the maximum and less than the minimum
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureSetGain_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float max = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGainThreshold((AudioHandle)capture, &min, &max);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    float gainOne = max+1;
    float gainSec = min-1;
    ret = capture->volume.SetGain((AudioHandle)capture, gainOne);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    ret = capture->volume.SetGain((AudioHandle)capture, gainSec);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSetGain API via setting the incoming parameter handle is nullptr.
* @tc.number  SUB_Audio_hdi_CaptureSetGain_0006
* @tc.desc  test AudioCaptureSetGain interface, return -1 if the incoming parameter handle is nullptr.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureSetGain_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float gain = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetGain((AudioHandle)captureNull, gain);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGain API via legal input
* @tc.number  SUB_Audio_hdi_CaptureGetGain_0001
* @tc.desc  test AudioCaptureGetGain interface, return 0 if CaptureGetGain is call successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGain_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    float max = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGainThreshold((AudioHandle)capture, &min, &max);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    float gain = min+1;
    float gainValue = min+1;
    ret = capture->volume.SetGain((AudioHandle)capture, gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)capture, &gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(gainValue, gain);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGain API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_hdi_CaptureGetGain_0002
* @tc.desc  test AudioCaptureGetGain interface, return -1 if the incoming parameter handle is nullptr.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGain_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float gainValue = 0;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)captureNull, &gainValue);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGain API via legal input in difference scenes
* @tc.number  SUB_Audio_hdi_CaptureGetGain_0003
* @tc.desc  test AudioCaptureGetGain interface, return 0 if get gain after creating the capture object.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGain_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float gain = GAIN_MAX-1;
    float gainOne = GAIN_MAX-1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.SetGain((AudioHandle)capture, gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->volume.GetGain((AudioHandle)capture, &gain);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_EQ(gainOne, gain);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureGetGain API via setting the parameter gain is nullptr
* @tc.number  SUB_Audio_hdi_CaptureGetGain_0004
* @tc.desc  test AudioCaptureGetGain interface, return -1 if the parameter gain is nullptr.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureVolumeTest, SUB_Audio_hdi_CaptureGetGain_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    float *gainNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->volume.GetGain((AudioHandle)capture, gainNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
}
