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

#include "audio_hdi_common.h"
#include "audio_server_function_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";
const uint64_t FILESIZE = 1024;
const uint32_t CHANNELCOUNTEXOECT = 2;
const uint32_t SAMPLERATEEXOECT = 48000;

class AudioServerFunctionTest : public testing::Test {
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
    static int32_t GetManager(struct PrepareAudioPara& audiopara);
};

TestAudioManager *(*AudioServerFunctionTest::GetAudioManager)() = nullptr;
void *AudioServerFunctionTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioServerFunctionTest::SdkInit)() = nullptr;
    void (*AudioServerFunctionTest::SdkExit)() = nullptr;
    void *AudioServerFunctionTest::sdkSo = nullptr;
#endif
using THREAD_FUNC = void *(*)(void *);

void AudioServerFunctionTest::SetUpTestCase(void)
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

void AudioServerFunctionTest::TearDownTestCase(void)
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

void AudioServerFunctionTest::SetUp(void) {}

void AudioServerFunctionTest::TearDown(void) {}

int32_t AudioServerFunctionTest::GetManager(struct PrepareAudioPara& audiopara)
{
    auto *inst = (AudioServerFunctionTest *)audiopara.self;
    if (inst != nullptr && inst->GetAudioManager != nullptr) {
        audiopara.manager = inst->GetAudioManager();
    }
    if (audiopara.manager == nullptr) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/**
* @tc.name  Playing an audio file
* @tc.number  SUB_Audio_Function_Render_Test_0001
* @tc.desc  test StartRender interface,The audio file is played successfully.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Set audio file volume
* @tc.number  SUB_Audio_Function_Render_Test_0002
* @tc.desc  test Render function,set volume when the audio file is playing.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    float volumeMax = 1.0;
    bool muteFalse = false;
    float volumeValue[10] = {0};
    float volume[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->volume.SetMute(audiopara.render, muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.SetVolume(audiopara.render, volumeMax);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        for (int i = 0; i < 10; i++) {
            ret = audiopara.render->volume.SetVolume(audiopara.render, volume[i]);
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            ret = audiopara.render->volume.GetVolume(audiopara.render, &volumeValue[i]);
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            EXPECT_EQ(volume[i], volumeValue[i]);
            usleep(30000);
        }
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Mute audio files
* @tc.number  SUB_Audio_Function_Render_Test_0003
* @tc.desc  test Render function,set mute when the audio file is playing.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    bool muteTrue = true;
    bool muteFalse = false;
    float volume = 0.8;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->volume.SetVolume(audiopara.render, volume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sleep(1);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(true, muteTrue);
        sleep(1);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(false, muteFalse);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Pause Resume and Stop audio file
* @tc.number  SUB_Audio_Function_Render_Test_0004
* @tc.desc  test Render function,call pause,resume and stop interface when the audio file is playing.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0004, TestSize.Level1)
{
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    int32_t ret = -1;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        FrameStatus(0);
        usleep(1000);
        ret = audiopara.render->control.Pause((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sleep(1);
        ret = audiopara.render->control.Resume((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        FrameStatus(1);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get audio gainthreshold and set gain value
* @tc.number  SUB_Audio_Function_Render_Test_0005
* @tc.desc  test Render function,Call interface GetGainThreshold,SetGain and GetGain when playing.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    float gain = 0;
    float gainMax = 0;
    float gainMin = 0;
    float gainMinValue = 1;
    float gainMaxValue = 14;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->volume.GetGainThreshold(audiopara.render, &gainMin, &gainMax);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

        ret = audiopara.render->volume.SetGain(audiopara.render, gainMax - 1);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetGain(audiopara.render, &gain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(gainMaxValue, gain);

        ret = audiopara.render->volume.SetGain(audiopara.render, gainMin + 1);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetGain(audiopara.render, &gain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(gainMinValue, gain);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  set volume after the audio file is Paused and set mute after the audio file is resumed
* @tc.number  SUB_Audio_Function_Render_Test_0006
* @tc.desc  test Render function,set volume after pause and set mute after resume during playing.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    float volumeValue = 1.0;
    float volume = 1.0;
    bool muteTrue = true;
    bool muteFalse = false;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        FrameStatus(0);
        usleep(1000);
        ret = audiopara.render->control.Pause((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sleep(1);
        ret = audiopara.render->volume.SetVolume(audiopara.render, volume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetVolume(audiopara.render, &volume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(volumeValue, volume);
        ret = audiopara.render->control.Resume((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        FrameStatus(1);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(true, muteTrue);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(false, muteFalse);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  set mute after the audio file is Paused and set volume after the audio file is resumed
* @tc.number  SUB_Audio_Function_Render_Test_0007
* @tc.desc  test Render function,set mute after pause and set volume after resume during playing.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0007, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    float volumeMax = 1.0;
    bool muteTrue = true;
    bool muteFalse = false;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        FrameStatus(0);
        usleep(1000);
        ret = audiopara.render->control.Pause((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sleep(1);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteTrue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(true, muteTrue);
        ret = audiopara.render->volume.SetMute(audiopara.render, muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->volume.GetMute(audiopara.render, &muteFalse);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(false, muteFalse);
        sleep(1);
        ret = audiopara.render->control.Resume((AudioHandle)(audiopara.render));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        FrameStatus(1);
        sleep(1);
        ret = audiopara.render->volume.SetVolume(audiopara.render, volumeMax);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Current ChannelId during playing.
* @tc.number  SUB_Audio_Function_Render_Test_0008
* @tc.desc  test StartRender interface,The audio file is played out normally and Get Current ChannelId as expected.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0008, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    float speed = 3;
    uint32_t channelId = 0;
    uint32_t channelIdValue = CHANNELCOUNT;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->SetRenderSpeed(audiopara.render, speed);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        ret = audiopara.render->GetRenderSpeed(audiopara.render, &speed);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        ret = audiopara.render->attr.GetCurrentChannelId(audiopara.render, &channelId);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(channelId, channelIdValue);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Frame Size during playing
* @tc.number  SUB_Audio_Function_Render_Test_0009
* @tc.desc  test StartRender interface,The audio file is played out normally and Get Frame Sizeas expected.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0009, TestSize.Level1)
{
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    uint64_t size = 0;
    uint64_t sizeExpect = 0;
    int32_t ret = -1;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->attr.GetFrameSize(audiopara.render, &size);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sizeExpect = PcmFramesToBytes(audiopara.attrs);
        EXPECT_EQ(size, sizeExpect);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Frame Count during playing
* @tc.number  SUB_Audio_Function_Render_Test_0010
* @tc.desc  test StartRender interface,The audio file is played out normally and Get Frame Count as expected.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0010, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    uint64_t count = 0;
    uint64_t zero = 0;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->attr.GetFrameCount(audiopara.render, &count);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT(count, zero);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get render position when playing audio file
* @tc.number  SUB_Audio_Function_Render_Test_0011
* @tc.desc  test render functio by Get render position when playing audio file.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0011, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t frames = 0;
    int64_t timeExp = 0;
    struct AudioTimeStamp time = {.tvSec = 0};
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(2);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->GetRenderPosition(audiopara.render, &frames, &time);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT(time.tvSec, timeExp);
        EXPECT_GT(frames, INITIAL_VALUE);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Check Scene Capability during playing
* @tc.number  SUB_Audio_Function_Render_Test_0012
* @tc.desc  test StartRender interface,The audio file is played out normally and Check Scene Capability as expected.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0012, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = false;
    struct AudioSceneDescriptor scenes = {};
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        ret = audiopara.render->scene.CheckSceneCapability(audiopara.render, &scenes, &supported);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_TRUE(supported);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  when audio file playing SetSampleAttributes
* @tc.number  SUB_Audio_Function_Render_Test_0013
* @tc.desc  test StartRender interface,After setting SetSampleAttributes,
*           the playback will reset SetSampleAttributes.
* @tc.author: wangkang
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Render_Test_0013, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    uint32_t samplerateValue = 48000;
    uint32_t channelcountValue = 2;
    struct AudioSampleAttributes attrsValue = {};

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.render != nullptr) {
        audiopara.attrs.type = AUDIO_IN_MEDIA;
        audiopara.attrs.interleaved = false;
        audiopara.attrs.format = AUDIO_FORMAT_PCM_16_BIT;
        audiopara.attrs.sampleRate = 48000;
        audiopara.attrs.channelCount = 2;
        audiopara.attrs.period = 4096;

        ret = audiopara.render->attr.SetSampleAttributes(audiopara.render, &(audiopara.attrs));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->attr.GetSampleAttributes(audiopara.render, &attrsValue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

        EXPECT_EQ(AUDIO_IN_MEDIA, attrsValue.type);
        EXPECT_FALSE(attrsValue.interleaved);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, attrsValue.format);
        EXPECT_EQ(samplerateValue, attrsValue.sampleRate);
        EXPECT_EQ(channelcountValue, attrsValue.channelCount);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Record audio file
* @tc.number  SUB_Audio_Function_Capture_Test_0001
* @tc.desc  test capture function, The audio file is recorded successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Pause,resume and stop when recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0002
* @tc.desc  test capture function,Pause,resume and stop when recording.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    FrameStatus(1);
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        FrameStatus(0);
        sleep(1);
        ret = audiopara.capture->control.Pause((AudioHandle)(audiopara.capture));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sleep(1);
        ret = audiopara.capture->control.Resume((AudioHandle)(audiopara.capture));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        FrameStatus(1);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Set volume when recording audio file
* @tc.number  SUB_Audio_Function_Capture_Test_0003
* @tc.desc  Test capture function,set volume when recording audio file.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    float val = 0.9;
    float getVal = 0;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->volume.SetVolume((AudioHandle)(audiopara.capture), val);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.capture->volume.GetVolume((AudioHandle)(audiopara.capture), &getVal);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FLOAT_EQ(val, getVal);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Set Mute when recording audio file
* @tc.number  SUB_Audio_Function_Capture_Test_0004
* @tc.desc  Test capture function, Set mute when recording audio file.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0004, TestSize.Level1)
{
    int32_t ret = -1;
    bool isMute = false;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    bool mute = true;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->volume.SetMute((AudioHandle)(audiopara.capture), mute);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.capture->volume.GetMute((AudioHandle)(audiopara.capture), &isMute);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_TRUE(isMute);
        isMute = false;
        sleep(1);
        ret = audiopara.capture->volume.SetMute((AudioHandle)(audiopara.capture), isMute);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.capture->volume.GetMute((AudioHandle)(audiopara.capture), &isMute);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FALSE(isMute);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Set Gain when recording audio file
* @tc.number  SUB_Audio_Function_Capture_Test_0005
* @tc.desc  Test capture function, Set gain when recording audio file.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0005, TestSize.Level1)
{
    int32_t ret = -1;
    float gainMin = 0;
    float gainMax = 0;
    struct PrepareAudioPara para = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    float gainValue = 0;
    float gain = 0;
    ret = GetManager(para);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&para.tids, NULL, (THREAD_FUNC)RecordAudio, &para);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (para.capture != nullptr) {
        ret = para.capture->volume.GetGainThreshold(para.capture, &gainMin, &gainMax);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = para.capture->volume.SetGain((AudioHandle)(para.capture), gainMax - 1);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gainValue = gainMax - 1;
        ret = para.capture->volume.GetGain((AudioHandle)(para.capture), &gain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FLOAT_EQ(gainValue, gain);
        sleep(1);
        ret = para.capture->volume.SetGain((AudioHandle)(para.capture), gainMin + 1);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gainValue = gainMin + 1;
        ret = para.capture->volume.GetGain((AudioHandle)(para.capture), &gain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FLOAT_EQ(gainValue, gain);
    }
    ret = ThreadRelease(para);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Set SampleAttributes during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0006
* @tc.desc  test capture function,the sampleattributes were set success,and the audio file is recorded successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrsValue = {};
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    struct AudioSampleAttributes attrs = {};
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    attrs.sampleRate = SAMPLERATEEXOECT;

    FrameStatus(1);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    sleep(1);
    if (audiopara.capture != nullptr) {
        FrameStatus(0);
        usleep(300000);
        ret = audiopara.capture->attr.SetSampleAttributes(audiopara.capture, &attrs);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        usleep(300000);
        FrameStatus(1);
        sleep(1);
        ret = audiopara.capture->attr.GetSampleAttributes(audiopara.capture, &attrsValue);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(AUDIO_IN_MEDIA, attrsValue.type);
        EXPECT_FALSE(attrsValue.interleaved);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, attrsValue.format);
        EXPECT_EQ(SAMPLERATEEXOECT, attrsValue.sampleRate);
        EXPECT_EQ(CHANNELCOUNTEXOECT, attrsValue.channelCount);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get CurrentChannel Id during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0007
* @tc.desc  test capture function,the CurrentChannel Id were get success,and the audio file is recorded successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0007, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t channelId = 0;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    uint32_t channelIdValue = CHANNELCOUNT;

    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->attr.GetCurrentChannelId(audiopara.capture, &channelId);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(channelId, channelIdValue);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Frame Size during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0008
* @tc.desc  test capture function, the Frame Size were get success,and the audio file is recorded successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0008, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    uint64_t size = 0;
    uint64_t sizeExpect = 0;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(audiopara.attrs);
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->attr.GetFrameSize(audiopara.capture, &size);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        sizeExpect = PcmFramesToBytes(audiopara.attrs);
        EXPECT_EQ(size, sizeExpect);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Frame Count during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0009
* @tc.desc  test capture function, the Frame Count were get success,and the audio file is recorded successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0009, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    uint64_t count = 0;
    uint64_t zero = 0;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->attr.GetFrameCount(audiopara.capture, &count);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT(count, zero);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Get Gain during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0010
* @tc.desc  test capture function, the Gain were get success,and the audio file is recorded successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0010, TestSize.Level1)
{
    int32_t ret = -1;
    float min = 0;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    float max = 0;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->volume.GetGainThreshold((AudioHandle)(audiopara.capture), &min, &max);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(min, GAIN_MIN);
        EXPECT_EQ(max, GAIN_MAX);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  Check Scene Capability during recording.
* @tc.number  SUB_Audio_Function_Capture_Test_0011
* @tc.desc  test capture function, the Check Scene Capability success,and the audio file is recorded successfully.
* @tc.author: tiansuli
*/
HWTEST_F(AudioServerFunctionTest, SUB_Audio_Function_Capture_Test_0011, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = false;
    struct PrepareAudioPara audiopara = {
        .adapterName = ADAPTER_NAME_INTERNAL.c_str(), .self = this, .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str(), .fileSize = FILESIZE
    };
    struct AudioSceneDescriptor scenes = {};
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = GetManager(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)RecordAudio, &audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->scene.CheckSceneCapability(audiopara.capture, &scenes, &supported);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_TRUE(supported);
    }
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
}
