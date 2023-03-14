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
#include <pthread.h>
#include "audio_hdirender_reliability_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const int PTHREAD_SAMEADA_COUNT = 10;
const int PTHREAD_DIFFADA_COUNT = 1;
mutex g_testMutex;
static struct PrepareAudioPara g_para[PTHREAD_DIFFADA_COUNT] = {
    {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(),  .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    }
};

class AudioHdiRenderReliabilityTest : public testing::Test {
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
    static int32_t RelAudioCreateRender(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderProcedure(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSetGain(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetGain(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetGainThreshold(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSetMute(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetMute(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSetVolume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetVolume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetFrameSize(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetFrameCount(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetCurrentChannelId(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSetChannelMode(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetChannelMode(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSetSampleAttributes(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetSampleAttributes(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderSelectScene(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderCheckSceneCapability(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetLatency(struct PrepareAudioPara& ptr);
};

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiRenderReliabilityTest::GetAudioManager)() = nullptr;
void *AudioHdiRenderReliabilityTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiRenderReliabilityTest::SdkInit)() = nullptr;
    void (*AudioHdiRenderReliabilityTest::SdkExit)() = nullptr;
    void *AudioHdiRenderReliabilityTest::sdkSo = nullptr;
#endif

void AudioHdiRenderReliabilityTest::SetUpTestCase(void)
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

void AudioHdiRenderReliabilityTest::TearDownTestCase(void)
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

void AudioHdiRenderReliabilityTest::SetUp(void) {}

void AudioHdiRenderReliabilityTest::TearDown(void) {}

int32_t AudioHdiRenderReliabilityTest::RelAudioCreateRender(struct PrepareAudioPara& ptr)
{
    if (ptr.manager == nullptr || ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    InitAttrs(attrs);
    InitDevDesc(devDesc, ptr.audioPort->portId, ptr.pins);
    g_testMutex.lock();
    int32_t ret = ptr.adapter->CreateRender(ptr.adapter, &devDesc, &attrs, &ptr.render);
    g_testMutex.unlock();
    if (ret < 0) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return ret;
    }
    if (ptr.render == nullptr) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetGainThreshold(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.GetGainThreshold(ptr.render, &(ptr.character.gainthresholdmin),
                                              &(ptr.character.gainthresholdmax));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSetGain(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.SetGain(ptr.render, ptr.character.setgain);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetGain(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.GetGain(ptr.render, &(ptr.character.getgain));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderProcedure(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    ret = GetLoadAdapter(ptr.manager, ptr.portType, ptr.adapterName, &ptr.adapter, ptr.audioPort);
    if (ret < 0) {
        return ret;
    }
    ret = RelAudioCreateRender(ptr);
    if (ret < 0) {
        return ret;
    }
    ret = AudioRenderStartAndOneFrame(ptr.render);
    if (ret < 0) {
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSetMute(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.SetMute(ptr.render, ptr.character.setmute);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetMute(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.GetMute(ptr.render, &(ptr.character.getmute));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSetVolume(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.SetVolume(ptr.render, ptr.character.setvolume);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetVolume(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->volume.GetVolume(ptr.render, &(ptr.character.getvolume));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetFrameSize(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->attr.GetFrameSize(ptr.render, &(ptr.character.getframesize));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetFrameCount(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->attr.GetFrameCount(ptr.render, &(ptr.character.getframecount));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetCurrentChannelId(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->attr.GetCurrentChannelId(ptr.render, &(ptr.character.getcurrentchannelId));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSetSampleAttributes(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->attr.SetSampleAttributes(ptr.render, &(ptr.attrs));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetSampleAttributes(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->attr.GetSampleAttributes(ptr.render, &(ptr.attrsValue));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSelectScene(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->scene.SelectScene(ptr.render, &(ptr.scenes));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderCheckSceneCapability(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->scene.CheckSceneCapability(ptr.render, &ptr.scenes, &(ptr.character.supported));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderSetChannelMode(struct PrepareAudioPara &ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->SetChannelMode(ptr.render, ptr.character.setmode);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetChannelMode(struct PrepareAudioPara &ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->GetChannelMode(ptr.render, &(ptr.character.getmode));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderReliabilityTest::RelAudioRenderGetLatency(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->GetLatency(ptr.render, &(ptr.character.latencyTime));
    g_testMutex.unlock();
    return ret;
}

/**
* @tc.name  test AudioRenderGetFrameSize API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001
* @tc.desc  test GetFrameSize interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetFrameSize_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetFrameSize, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(INITIAL_VALUE, g_para[0].character.getframesize);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderGetFrameCount API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001
* @tc.desc  test GetFrameCount interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetFrameCount_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetFrameCount, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(INITIAL_VALUE, g_para[0].character.getframecount);
    }
    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderGetVolume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001
* @tc.desc  test GetCurrentChannelId interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetCurrentChannelId_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t channelIdValue = 2;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.getcurrentchannelId = 0;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetCurrentChannelId, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(channelIdValue, arrpara[i].character.getcurrentchannelId);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSetMute API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderSetMute_Reliability_0001
* @tc.desc  test AudioRenderSetMute interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudiorenderSetMute_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];

    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = arrpara[i].render->volume.GetMute(arrpara[i].render, &(arrpara[i].character.getmute));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        if (arrpara[i].character.getmute == false) {
            arrpara[i].character.setmute = true;
        } else {
            arrpara[i].character.setmute = false;
        }
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetMute, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderGetMute API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetMute_Reliability_0001
* @tc.desc  test AudioRenderGetMute interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudiorenderGetMute_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = arrpara[i].render->volume.SetMute(arrpara[i].render, false);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetMute, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_FALSE(arrpara[i].character.getmute);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSetVolume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderSetVolume_Reliability_0001
* @tc.desc  test SetVolume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudiorenderSetVolume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeHighExpc = 0.70;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];

    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setvolume = 0.70;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetVolume, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        ret = arrpara[i].render->volume.GetVolume(arrpara[i].render, &(arrpara[i].character.getvolume));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(volumeHighExpc, arrpara[i].character.getvolume);
    }
    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderGetVolume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001
* @tc.desc  test GetVolume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeHighExpc = 0.7;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setvolume = 0.7;
        ret = arrpara[i].render->volume.SetVolume(arrpara[i].render, arrpara[i].character.setvolume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetVolume, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(volumeHighExpc, arrpara[i].character.getvolume);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSetSampleAttributes API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderSetSampleAttributes_Reliability_0001
* @tc.desc  test AudioRenderSetSampleAttributes interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderSetSampleAttributes_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t count = 2;
    uint32_t rateExpc = 48000;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(g_para[0].attrs);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetSampleAttributes, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        ret = arrpara[i].render->attr.GetSampleAttributes(arrpara[i].render, &(arrpara[i].attrsValue));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(AUDIO_IN_MEDIA, arrpara[i].attrsValue.type);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, arrpara[i].attrsValue.format);
        EXPECT_EQ(rateExpc, arrpara[i].attrsValue.sampleRate);
        EXPECT_EQ(count, arrpara[i].attrsValue.channelCount);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderGetSampleAttributes API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderGetSampleAttributes_Reliability_0001
* @tc.desc  test AudioRenderGetSampleAttributes interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetSampleAttributes_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t count = 2;
    uint32_t rateExpc = 48000;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(g_para[0].attrs);
    ret = g_para[0].render->attr.SetSampleAttributes(g_para[0].render, &(g_para[0].attrs));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetSampleAttributes, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(AUDIO_IN_MEDIA, arrpara[i].attrsValue.type);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, arrpara[i].attrsValue.format);
        EXPECT_EQ(rateExpc, arrpara[i].attrsValue.sampleRate);
        EXPECT_EQ(count, arrpara[i].attrsValue.channelCount);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSelectScene API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_Reliability_0001
* @tc.desc  test AudioRenderSelectScene interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderSelectScene_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].scenes.scene.id = 0;
        g_para[0].scenes.desc.pins = PIN_OUT_SPEAKER;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSelectScene, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderCheckSceneCapability API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderCheckSceneCapability_Reliability_0001
* @tc.desc  test AudioRenderCheckSceneCapability interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderCheckSceneCapability_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].scenes.scene.id = 0;
        g_para[0].scenes.desc.pins = PIN_OUT_SPEAKER;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderCheckSceneCapability, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSetGain API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderSetGain_Reliability_0001
* @tc.desc  test AudioRenderSetGain interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderSetGain_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].character.setgain = 5;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetGain, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(GAIN_MIN, g_para[0].character.setgain);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderGetGain API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderGetGain_Reliability_0001
* @tc.desc  test GetGain interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetGain_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setgain = 7;
        ret = arrpara[i].render->volume.SetGain(arrpara[i].render, arrpara[i].character.setgain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetGain, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(GAIN_MIN, arrpara[i].character.setgain);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderGetGainThreshold API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderGetGainThreshold_Reliability_0001
* @tc.desc  test GetGainThreshold interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetGainThreshold_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetGainThreshold, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(0, arrpara[i].character.gainthresholdmin);
        EXPECT_EQ(15, arrpara[i].character.gainthresholdmax);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderSetChannelMode API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderSetChannelMode_Reliability_0001
* @tc.desc  test SetChannelMode interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderSetChannelMode_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setmode = AUDIO_CHANNEL_NORMAL;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetChannelMode, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(AUDIO_CHANNEL_NORMAL, arrpara[i].character.getmode);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderGetChannelMode API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderGetChannelMode_Reliability_0001
* @tc.desc  test GetChannelMode interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderGetChannelMode_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setmode = AUDIO_CHANNEL_NORMAL;
        ret = arrpara[i].render->SetChannelMode(arrpara[i].render, arrpara[i].character.setmode);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderSetChannelMode, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(AUDIO_CHANNEL_NORMAL, arrpara[i].character.getmode);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderRenderGetLatency API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderRenderGetLatency_Reliability_0001
* @tc.desc  test GetLatency interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: wangkang
*/
HWTEST_F(AudioHdiRenderReliabilityTest, SUB_Audio_HDI_AudioRenderRenderGetLatency_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t expectLatency = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = GetLoadAdapter(g_para[0].manager, g_para[0].portType, g_para[0].adapterName, &g_para[0].adapter,
                         g_para[0].audioPort);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateRender(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetLatency, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(expectLatency, arrpara[i].character.latencyTime);
    }
    g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].render = nullptr;
}
}