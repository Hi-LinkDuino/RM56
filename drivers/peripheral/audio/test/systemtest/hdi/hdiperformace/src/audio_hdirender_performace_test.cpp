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
 * @brief Test the delayTime of audio playback interface.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hdi_common.h
 *
 * @brief Declares APIs for operations related to the audio delayTime.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"
#include "audio_hdirender_performace_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const float COUNT = 1000;
const int32_t LOWLATENCY = 10000;
const int32_t NORMALLATENCY = 30000;
const int32_t HIGHLATENCY = 60000;

class AudioHdiRenderPerformaceTest : public testing::Test {
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
};

TestAudioManager *(*AudioHdiRenderPerformaceTest::GetAudioManager)() = nullptr;
void *AudioHdiRenderPerformaceTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiRenderPerformaceTest::SdkInit)() = nullptr;
    void (*AudioHdiRenderPerformaceTest::SdkExit)() = nullptr;
    void *AudioHdiRenderPerformaceTest::sdkSo = nullptr;
#endif

void AudioHdiRenderPerformaceTest::SetUpTestCase(void)
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

void AudioHdiRenderPerformaceTest::TearDownTestCase(void)
{
#ifdef AUDIO_MPI_SO
    if (SdkExit != nullptr) {
        SdkExit();
    }
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

void AudioHdiRenderPerformaceTest::SetUp(void) {}

void AudioHdiRenderPerformaceTest::TearDown(void) {}

/**
* @tc.name  the performace of AudioManagerGetAllAdapters
* @tc.number  SUB_Audio_HDI_AudioManagerGetAllAdapter_Performance_0001
* @tc.desc  tests the performace of AudioManagerGetAllAdapters interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioManagerGetAllAdapters_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct PrepareAudioPara audiopara = { .totalTime = 0 };

    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.manager->GetAllAdapters(audiopara.manager, &audiopara.descs, &size);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioManagerLoadAdapter
* @tc.number  SUB_Audio_HDI_AudioManagerLoadAdapter_Performance_0001
* @tc.desc  tests the performace of AudioManagerLoadAdapter interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioManagerLoadAdapter_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct PrepareAudioPara audiopara = { .totalTime = 0 };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = audiopara.manager->GetAllAdapters(audiopara.manager, &audiopara.descs, &size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    audiopara.desc = &audiopara.descs[0];
    ASSERT_NE(nullptr, audiopara.desc);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.manager->LoadAdapter(audiopara.manager, audiopara.desc, &audiopara.adapter);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.adapter = nullptr;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioManagerUnLoadAdapter
* @tc.number  SUB_Audio_HDI_AudioManagerUnLoadAdapter_Performance_0001
* @tc.desc  tests the performace of AudioManagerLoadAdapter interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioManagerUnLoadAdapter_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int size = 0;
    struct PrepareAudioPara audiopara = { .totalTime = 0 };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = audiopara.manager->GetAllAdapters(audiopara.manager, &audiopara.descs, &size);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    audiopara.desc = &audiopara.descs[0];
    ASSERT_NE(nullptr, audiopara.desc);

    for (int i = 0; i < COUNT; ++i) {
        ret = audiopara.manager->LoadAdapter(audiopara.manager, audiopara.desc, &audiopara.adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        gettimeofday(&audiopara.end, NULL);
        audiopara.adapter = nullptr;
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioInitAllPorts
* @tc.number  SUB_Audio_HDI_AudioManagerInitAllPorts_Performance_0001
* @tc.desc  tests the performace of AudioInitAllPorts interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioManagerInitAllPorts_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.adapter->InitAllPorts(audiopara.adapter);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioGetPortCapability
* @tc.number  SUB_Audio_HDI_AudioGetPortCapability_Performance_0001
* @tc.desc  tests the performace of AudioGetPortCapability interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioGetPortCapability_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.adapter->InitAllPorts(audiopara.adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.adapter->GetPortCapability(audiopara.adapter, audiopara.audioPort, &audiopara.capability);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioSetPassthroughMode
* @tc.number  SUB_Audio_HDI_AudioSetPassthroughMode_Performance_0001
* @tc.desc  tests the performace of AudioSetPassthroughMode interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioSetPassthroughMode_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .mode = PORT_PASSTHROUGH_LPCM,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.adapter->InitAllPorts(audiopara.adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.adapter->SetPassthroughMode(audiopara.adapter, audiopara.audioPort, audiopara.mode);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.adapter->GetPassthroughMode(audiopara.adapter, audiopara.audioPort, &audiopara.mode);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(PORT_PASSTHROUGH_LPCM, audiopara.mode);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioGetPassthroughMode
* @tc.number  SUB_Audio_HDI_AudioGetPassthroughMode_Performance_0001
* @tc.desc  tests the performace of AudioGetPassthroughMode interface by executing 1000 times,
* and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioGetPassthroughMode_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .mode = PORT_PASSTHROUGH_LPCM,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.adapter->InitAllPorts(audiopara.adapter);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.adapter->SetPassthroughMode(audiopara.adapter, audiopara.audioPort, audiopara.mode);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.adapter->GetPassthroughMode(audiopara.adapter, audiopara.audioPort, &audiopara.mode);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(PORT_PASSTHROUGH_LPCM, audiopara.mode);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioRenderGetLatency
* @tc.number  SUB_Audio_HDI_AudioRenderGetLatency_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetLatency interface by executing 1000 times,
* and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetLatency_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str(), .totalTime = 0
    };
    uint32_t latencyTimeExpc = 0;
    uint32_t latencyTime = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = PlayAudioFile(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        if (audiopara.render != nullptr) {
            gettimeofday(&audiopara.start, NULL);
            ret = audiopara.render->GetLatency(audiopara.render, &latencyTime);
            gettimeofday(&audiopara.end, NULL);
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            EXPECT_LT(latencyTimeExpc, latencyTime);
            audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
            audiopara.totalTime += audiopara.delayTime;
        }
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioCreateRender
* @tc.number  SUB_Audio_HDI_AudioCreateRender_Performance_0001
* @tc.desc  tests the performace of AudioCreateRender interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioCreateRender_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(audiopara.attrs);
    InitDevDesc(audiopara.devDesc, audiopara.audioPort->portId, audiopara.pins);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.adapter->CreateRender(audiopara.adapter, &audiopara.devDesc, &audiopara.attrs,
                                              &audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        if (ret < 0 || audiopara.render == nullptr) {
            audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
            audiopara.adapter = nullptr;
            ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        }
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(HIGHLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioDestroyRender
* @tc.number  SUB_Audio_HDI_AudioDestroyRender_Performance_0001
* @tc.desc  tests the performace of AudioDestroyRender interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioDestroyRender_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = GetLoadAdapter(audiopara.manager, audiopara.portType, audiopara.adapterName,
                         &audiopara.adapter, audiopara.audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(audiopara.attrs);
    InitDevDesc(audiopara.devDesc, audiopara.audioPort->portId, audiopara.pins);
    for (int i = 0; i < COUNT; ++i) {
        ret = audiopara.adapter->CreateRender(audiopara.adapter, &audiopara.devDesc, &audiopara.attrs,
                                              &audiopara.render);
        if (ret < 0 || audiopara.render == nullptr) {
            audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
            audiopara.adapter = nullptr;
            ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        }
        gettimeofday(&audiopara.start, NULL);
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioRenderGetRenderPosition
* @tc.number  SUB_Audio_HDI_AudioRenderGetRenderPosition_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetRenderPosition interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetRenderPosition_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str(), .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = PlayAudioFile(audiopara);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        if (audiopara.render != nullptr) {
            gettimeofday(&audiopara.start, NULL);
            ret = audiopara.render->GetRenderPosition(audiopara.render, &audiopara.character.getframes,
                &audiopara.time);
            gettimeofday(&audiopara.end, NULL);
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
            audiopara.totalTime += audiopara.delayTime;
        }
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderSetRenderSpeed
* @tc.number  SUB_Audio_HDI_AudioRenderSetRenderSpeed_Performance_0001
* @tc.desc  tests the performace of AudioRenderSetRenderSpeed interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderSetRenderSpeed_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float speedNormal = 30;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->SetRenderSpeed(audiopara.render, speedNormal);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        ret = audiopara.render->GetRenderSpeed(audiopara.render, &speedNormal);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderGetRenderSpeed
* @tc.number  SUB_Audio_HDI_AudioRenderGetRenderSpeed_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetRenderSpeed interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetRenderSpeed_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float speedValue = 30;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->GetRenderSpeed(audiopara.render, &speedValue);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderSetChannelMode
* @tc.number  SUB_Audio_HDI_AudioRenderSetChannelMode_Performance_0001
* @tc.desc  tests the performace of AudioRenderSetChannelMode interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderSetChannelMode_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->SetChannelMode(audiopara.render, mode);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.render->GetChannelMode(audiopara.render, &mode);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(AUDIO_CHANNEL_NORMAL, mode);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderGetChannelMode
* @tc.number  SUB_Audio_HDI_AudioRenderGetChannelMode_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetChannelMode interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetChannelMode_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioChannelMode mode = AUDIO_CHANNEL_NORMAL;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->SetChannelMode(audiopara.render, mode);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->GetChannelMode(audiopara.render, &mode);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(AUDIO_CHANNEL_NORMAL, mode);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderGetFrameCount
* @tc.number  SUB_Audio_HDI_AudioRenderGetFrameCount_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetFrameCount interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetFrameCount_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetFrameCount(audiopara.render, &audiopara.character.getframecount);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderGetCurrentChannelId
* @tc.number  SUB_Audio_HDI_AudioRenderGetCurrentChannelId_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetCurrentChannelId interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetCurrentChannelId_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetCurrentChannelId(audiopara.render, &audiopara.character.getcurrentchannelId);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioRenderFlush
* @tc.number  SUB_Audio_HDI_AudioRenderFlush_Performance_0001
* @tc.desc  tests the performace of AudioRenderFlush interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderFlush_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    for (int i = 0; i < COUNT; ++i) {
        ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                                &audiopara.render);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->control.Flush((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = StopAudio(audiopara);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioRenderGetFrameSize
* @tc.number  SUB_Audio_HDI_AudioRenderGetFrameSize_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetFrameSize interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetFrameSize_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t zero = 0;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetFrameSize(audiopara.render, &audiopara.character.getframesize);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT(audiopara.character.getframesize, zero);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.render->control.Stop((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}


/**
* @tc.name  the performace of AudioRenderCheckSceneCapability
* @tc.number  SUB_Audio_HDI_AudioRenderCheckSceneCapability_Performance_0001
* @tc.desc  tests the performace of AudioRenderCheckSceneCapability interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderCheckSceneCapability_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    struct AudioSceneDescriptor scenes = {.scene.id = 0, .desc.pins = PIN_OUT_SPEAKER};
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->scene.CheckSceneCapability(audiopara.render, &scenes, &audiopara.character.supported);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioRenderSelectScene
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_Performance_0001
* @tc.desc  tests the performace of AudioRenderSelectScene interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderSelectScene_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    struct AudioSceneDescriptor scenes = {.scene.id = 0, .desc.pins = PIN_OUT_SPEAKER};
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->scene.SelectScene(audiopara.render, &scenes);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderSetMute
* @tc.number  SUB_Audio_HDI_AudiorenderSetMute_Performance_0001
* @tc.desc  tests the performace of AudiorenderSetMute interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderSetMute_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.SetMute(audiopara.render, false);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.render->volume.GetMute(audiopara.render, &audiopara.character.getmute);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FALSE(audiopara.character.getmute);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderGetMute
* @tc.number  SUB_Audio_HDI_AudiorenderGetMute_Performance_0001
* @tc.desc  tests the performace of AudiorenderGetMute interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderGetMute_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->volume.SetMute(audiopara.render, false);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.GetMute(audiopara.render, &audiopara.character.getmute);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_FALSE(audiopara.character.getmute);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderSetVolume
* @tc.number  SUB_Audio_HDI_AudiorenderSetVolume_Performance_0001
* @tc.desc  tests the performace of AudiorenderSetVolume interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderSetVolume_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .character.setvolume = 0.8, .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.SetVolume(audiopara.render, audiopara.character.setvolume);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.render->volume.GetVolume(audiopara.render, &audiopara.character.getvolume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(audiopara.character.setvolume, audiopara.character.getvolume);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderGetVolume
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Performance_0001
* @tc.desc  tests the performace of AudiorenderGetVolume interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderGetVolume_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.GetVolume(audiopara.render, &audiopara.character.getvolume);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderGetGainThreshold
* @tc.number  SUB_Audio_HDI_AudiorenderGetGainThreshold_Performance_0001
* @tc.desc  tests the performace of AudiorenderGetGainThreshold interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderGetGainThreshold_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.GetGainThreshold(audiopara.render, &audiopara.character.gainthresholdmin,
                &audiopara.character.gainthresholdmax);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderGetGain
* @tc.number  SUB_Audio_HDI_AudiorenderGetGain_Performance_0001
* @tc.desc  tests the performace of AudiorenderGetGain interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderGetGain_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.GetGain(audiopara.render, &audiopara.character.getgain);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudiorenderSetGain
* @tc.number  SUB_Audio_HDI_AudiorenderSetGain_Performance_0001
* @tc.desc  tests the performace of AudiorenderSetGain interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudiorenderSetGain_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .character.setgain = 7, .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->volume.SetGain(audiopara.render, audiopara.character.setgain);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = audiopara.render->volume.GetGain(audiopara.render, &audiopara.character.getgain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(audiopara.character.setgain, audiopara.character.getgain);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}

/**
* @tc.name  the performace of AudioRenderFrame
* @tc.number  SUB_Audio_HDI_AudioRenderFrame_Performance_0001
* @tc.desc  tests the performace of AudioRenderFrame interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderFrame_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str(), .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    if (ret < 0) {
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = RenderFramePrepare(audiopara.path, audiopara.frame, audiopara.requestBytes);
    if (ret < 0) {
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->RenderFrame(audiopara.render, audiopara.frame, audiopara.requestBytes,
                                            &audiopara.replyBytes);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    if (audiopara.frame != nullptr) {
        free(audiopara.frame);
        audiopara.frame = nullptr;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(NORMALLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioRenderStart
* @tc.number  SUB_Audio_HDI_AudioRenderStart_Performance_0001
* @tc.desc  tests the performace of AudioRenderStart interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderStart_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    for (int i = 0; i < COUNT; ++i) {
        ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                                &audiopara.render);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        ret = StopAudio(audiopara);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}
/**
* @tc.name  the performace of AudioRenderStop
* @tc.number  SUB_Audio_HDI_AudioRenderStop_Performance_0001
* @tc.desc  tests the performace of AudioRenderStop interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderStop_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER, .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    for (int i = 0; i < COUNT; ++i) {
        ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                                &audiopara.render);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->control.Stop((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}
/**
* @tc.name  the performace of AudioRenderSetSampleAttributes
* @tc.number  SUB_Audio_HDI_AudioRenderSetSampleAttributes_Performance_0001
* @tc.desc  tests the performace of AudioRenderSetSampleAttributes interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderSetSampleAttributes_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER, .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(audiopara.attrs);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.SetSampleAttributes(audiopara.render, &audiopara.attrs);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}
/**
* @tc.name  the performace of AudioRenderPause
* @tc.number  SUB_Audio_HDI_AudioRenderPause_Performance_0001
* @tc.desc  tests the performace of AudioRenderPause interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderPause_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->control.Pause((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->control.Resume((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}

/**
* @tc.name  the performace of AudioRenderResume
* @tc.number  SUB_Audio_HDI_AudioRenderResume_Performance_0001
* @tc.desc  tests the performace of AudioRenderResume interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderResume_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int i = 0; i < COUNT; ++i) {
        ret = audiopara.render->control.Pause((AudioHandle)audiopara.render);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->control.Resume((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
/**
* @tc.name  the performace of AudioRenderGetSampleAttributes
* @tc.number  SUB_Audio_HDI_AudioRenderGetSampleAttributes_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetSampleAttributes interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetSampleAttributes_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(audiopara.attrs);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetSampleAttributes(audiopara.render, &audiopara.attrs);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
}
/**
* @tc.name  the performace of AudioRenderReqMmapBuffer
* @tc.number  SUB_Audio_HDI_AudioRenderReqMmapBuffer_Performance_0001
* @tc.desc  tests the performace of AudioRenderReqMmapBuffer interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderReqMmapBuffer_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool isRender = true;
    int32_t reqSize = 0;
    struct AudioMmapBufferDescripter desc = {};
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER, .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    for (int i = 0; i < COUNT; ++i) {
        audiopara.render = nullptr;
        FILE *fp = fopen(LOW_LATENCY_AUDIO_FILE.c_str(), "rb+");
        ASSERT_NE(nullptr, fp);
        ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                                &audiopara.render);
        if (ret < 0 || audiopara.render == nullptr) {
            fclose(fp);
            ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
            ASSERT_EQ(nullptr, audiopara.render);
        }
        InitAttrs(audiopara.attrs);
        audiopara.attrs.startThreshold = 0;
        ret = audiopara.render->attr.SetSampleAttributes(audiopara.render, &(audiopara.attrs));
        ret = InitMmapDesc(fp, desc, reqSize, isRender);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = audiopara.render->control.Start((AudioHandle)audiopara.render);
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.ReqMmapBuffer((AudioHandle)audiopara.render, reqSize, &desc);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        if (ret == 0) {
            munmap(desc.memoryAddress, reqSize);
        }
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
        audiopara.render->control.Stop((AudioHandle)audiopara.render);
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
        fclose(fp);
        usleep(500);
    }
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioRenderGetMmapPosition
* @tc.number  SUB_Audio_HDI_AudioRenderGetMmapPosition_Performance_0001
* @tc.desc  tests the performace of AudioRenderRenderGetMmapPosition interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetMmapPosition_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t framesRendering = 0;
    int64_t timeExp = 0;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_OUT_SPEAKER,
        .path = LOW_LATENCY_AUDIO_FILE.c_str(), .totalTime = 0
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = PlayMapAudioFile(audiopara);
    if (ret != 0) {
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetMmapPosition(audiopara.render, &framesRendering, &(audiopara.time));
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_GT((audiopara.time.tvSec) * SECTONSEC + (audiopara.time.tvNSec), timeExp);
        EXPECT_GT(framesRendering, INITIAL_VALUE);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
    audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
    audiopara.render = nullptr;
    audiopara.adapter = nullptr;
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioRenderSetExtraParams
* @tc.number  SUB_Audio_HDI_AudioRenderSetExtraParams_Performance_0001
* @tc.desc  tests the performace of AudioRenderSetExtraParams interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderSetExtraParams_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    char keyValueList[] = "attr-route=1;attr-format=32;attr-channels=2;attr-frame-count=82;attr-sampling-rate=48000";
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
    };
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);
    ret = AudioCreateStartRender(audiopara.manager, &audiopara.render, &audiopara.adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.SetExtraParams((AudioHandle)audiopara.render, keyValueList);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}

/**
* @tc.name  the performace of AudioRenderGetExtraParams
* @tc.number  SUB_Audio_HDI_AudioRenderGetExtraParams_Performance_0001
* @tc.desc  tests the performace of AudioRenderGetExtraParams interface by executing 1000 times,
*           and calculates the delay time and average of Delay Time.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiRenderPerformaceTest, SUB_Audio_HDI_AudioRenderGetExtraParams_Performance_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .self = this, .pins = PIN_OUT_SPEAKER, .path = AUDIO_FILE.c_str()
    };
    char keyValueList[] = "attr-format=24;attr-frame-count=4096;";
    char keyValueListExp[] = "attr-route=0;attr-format=24;attr-channels=2;attr-frame-count=4096;\
attr-sampling-rate=48000";
    int32_t listLenth = 256;
    ASSERT_NE(nullptr, GetAudioManager);
    audiopara.manager = GetAudioManager();
    ASSERT_NE(nullptr, audiopara.manager);

    ret = AudioCreateStartRender(audiopara.manager, &audiopara.render, &audiopara.adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->attr.SetExtraParams((AudioHandle)audiopara.render, keyValueList);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int i = 0; i < COUNT; ++i) {
        char keyValueListValue[256] = {};
        gettimeofday(&audiopara.start, NULL);
        ret = audiopara.render->attr.GetExtraParams((AudioHandle)audiopara.render, keyValueListValue, listLenth);
        gettimeofday(&audiopara.end, NULL);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_STREQ(keyValueListExp, keyValueListValue);
        audiopara.delayTime = (audiopara.end.tv_sec * MICROSECOND + audiopara.end.tv_usec) -
                              (audiopara.start.tv_sec * MICROSECOND + audiopara.start.tv_usec);
        audiopara.totalTime += audiopara.delayTime;
    }
    ret = StopAudio(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    audiopara.averageDelayTime = (float)audiopara.totalTime / COUNT;
    EXPECT_GT(LOWLATENCY, audiopara.averageDelayTime);
}
}