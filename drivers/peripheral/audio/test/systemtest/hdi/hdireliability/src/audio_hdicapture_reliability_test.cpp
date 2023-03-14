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
#include "audio_hdicapture_reliability_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const int PTHREAD_SAMEADA_COUNT = 3;
const int PTHREAD_DIFFADA_COUNT = 3;
const uint32_t SAMPLERATEVALUE = 48000;
mutex g_testMutex;
struct PrepareAudioPara g_para[PTHREAD_DIFFADA_COUNT] = {
    {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str()
    }
};

class AudioHdiCaptureReliabilityTest : public testing::Test {
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
    static int32_t RelAudioCreateCapture(struct PrepareAudioPara& ptr);
    static int32_t RelGetAllAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelLoadAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureSetMute(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetMute(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureSetVolume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetVolume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureProcedure(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureStartAndCaputreFrame(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureSetGain(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetGain(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetGainThreshold(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetFrameSize(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetFrameCount(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetCurrentChannelId(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureSetSampleAttributes(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetSampleAttributes(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureSelectScene(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureCheckSceneCapability(struct PrepareAudioPara& ptr);
};

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiCaptureReliabilityTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureReliabilityTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureReliabilityTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureReliabilityTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureReliabilityTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureReliabilityTest::SetUpTestCase(void)
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

void AudioHdiCaptureReliabilityTest::TearDownTestCase(void)
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

void AudioHdiCaptureReliabilityTest::SetUp(void) {}

void AudioHdiCaptureReliabilityTest::TearDown(void) {}

int32_t AudioHdiCaptureReliabilityTest::RelGetAllAdapter(struct PrepareAudioPara& ptr)
{
    int size = 0;
    if (ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ptr.manager->GetAllAdapters(ptr.manager, &ptr.descs, &size);
    g_testMutex.unlock();
    if (ptr.descs == nullptr || size == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    } else {
        int index = SwitchAdapter(ptr.descs, ptr.adapterName, ptr.portType, ptr.audioPort, size);
        if (index < 0) {
            return AUDIO_HAL_ERR_INTERNAL;
        } else {
            ptr.desc = &ptr.descs[index];
        }
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureReliabilityTest::RelLoadAdapter(struct PrepareAudioPara& ptr)
{
    if (ptr.desc == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ptr.manager->LoadAdapter(ptr.manager, ptr.desc, &ptr.adapter);
    g_testMutex.unlock();
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCreateCapture(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    if (ptr.adapter == nullptr  || ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    InitAttrs(attrs);
    InitDevDesc(devDesc, ptr.audioPort->portId, ptr.pins);
    g_testMutex.lock();
    ret = ptr.adapter->CreateCapture(ptr.adapter, &devDesc, &attrs, &ptr.capture);
    g_testMutex.unlock();
    if (ret < 0) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return ret;
    }
    if (ptr.capture == nullptr) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureStartAndCaputreFrame(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    FILE *file = fopen(ptr.path, "wb+");
    if (file == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(attrs);

    g_testMutex.lock();
    ret = FrameStartCapture(ptr.capture, file, attrs);
    g_testMutex.unlock();
    fclose(file);
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureSetMute(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.SetMute(ptr.capture, ptr.character.setmute);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetMute(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.GetMute(ptr.capture, &(ptr.character.getmute));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureSetVolume(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.SetVolume(ptr.capture, ptr.character.setvolume);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetVolume(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.GetVolume(ptr.capture, &(ptr.character.getvolume));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureProcedure(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    ret = RelGetAllAdapter(ptr);
    if (ret < 0) {
        return ret;
    }

    ret = RelLoadAdapter(ptr);
    if (ret < 0) {
        return ret;
    }

    ret = RelAudioCreateCapture(ptr);
    if (ret < 0) {
        return ret;
    }

    ret = RelAudioCaptureStartAndCaputreFrame(ptr);
    if (ret < 0) {
        ptr.adapter->DestroyCapture(ptr.adapter, ptr.capture);
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetGainThreshold(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.GetGainThreshold(ptr.capture, &(ptr.character.gainthresholdmin),
        &(ptr.character.gainthresholdmax));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureSetGain(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.SetGain(ptr.capture, ptr.character.setgain);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetGain(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->volume.GetGain(ptr.capture, &(ptr.character.getgain));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetFrameSize(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->attr.GetFrameSize(ptr.capture, &(ptr.character.getframesize));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetFrameCount(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->attr.GetFrameCount(ptr.capture, &(ptr.character.getframecount));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetCurrentChannelId(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->attr.GetCurrentChannelId(ptr.capture, &(ptr.character.getcurrentchannelId));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureSetSampleAttributes(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->attr.SetSampleAttributes(ptr.capture, &(ptr.attrs));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureGetSampleAttributes(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->attr.GetSampleAttributes(ptr.capture, &(ptr.attrsValue));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureSelectScene(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->scene.SelectScene(ptr.capture, &(ptr.scenes));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureReliabilityTest::RelAudioCaptureCheckSceneCapability(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->scene.CheckSceneCapability(ptr.capture, &ptr.scenes, &(ptr.character.supported));
    g_testMutex.unlock();
    return ret;
}

/**
* @tc.name  test AudioCaptureSetGain API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetGain_Reliability_0001
* @tc.desc  test AudioCaptureSetGain interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureSetGain_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].character.setgain = 2;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureSetGain, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        ret = g_para[0].capture->volume.GetGain(g_para[0].capture, &(g_para[0].character.getgain));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(g_para[0].character.setgain, g_para[0].character.getgain);
    }
    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}

/**
* @tc.name  test AudioCaptureGetGain API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetGain_Reliability_0001
* @tc.desc  test AudioCaptureGetGain interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetGain_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].character.setgain = 8;
        ret = g_para[0].capture->volume.SetGain(g_para[0].capture, g_para[0].character.setgain);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetGain, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(8, g_para[0].character.getgain);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureGetGainThreshold API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetGainThreshold_Reliability_0001
* @tc.desc  test GetGainThreshold interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetGainThreshold_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetGainThreshold, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(0, g_para[0].character.gainthresholdmin);
        EXPECT_EQ(15, g_para[0].character.gainthresholdmax);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureGetFrameSize API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetFrameSize_Reliability_0001
* @tc.desc  test AudioCaptureGetFrameSize interface Reliability pass through pthread_create fun and adapterName is same
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetFrameSize_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint64_t sizeValue = 4096;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetFrameSize, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(sizeValue, g_para[0].character.getframesize);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureGetFrameCount API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetFrameCount_Reliability_0001
* @tc.desc  test CaptureGetFrameCount interface Reliability pass through pthread_create fun and adapterName is same
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetFrameCount_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetFrameCount, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(INITIAL_VALUE, g_para[0].character.getframecount);
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}

/**
* @tc.name  test AudioGetCurrentChannelId API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioGetCurrentChannelId_Reliability_0001
* @tc.desc  test AudioGetCurrentChannelId interface Reliability pass through pthread_create fun and adapterName is same
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioGetCurrentChannelId_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t channelIdValue = 2;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[i].capture = g_para[0].capture;
        g_para[i].character.getcurrentchannelId = 0;
    }

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetCurrentChannelId, &g_para[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(channelIdValue, g_para[i].character.getcurrentchannelId);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[i].capture = 0;
    }
}
/**
* @tc.name  AudioCapturesetMute
* @tc.number  SUB_Audio_HDI_AudioCaptureSetMute_0001
* @tc.desc  test AudioCaptureSetMute interface Reliability pass through pthread_create(adapterName is same)
* @tc.author: wangkang
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureSetMute_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = g_para[0].capture->volume.GetMute(g_para[0].capture, &(g_para[0].character.getmute));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        if (g_para[0].character.getmute == false) {
            g_para[0].character.setmute = true;
        } else {
            g_para[0].character.setmute = false;
        }
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureSetMute, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);

        ret = g_para[0].capture->volume.GetMute(g_para[0].capture, &(g_para[0].character.getmute));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        if (g_para[0].character.setmute == true) {
            EXPECT_TRUE(g_para[0].character.getmute);
        } else {
            EXPECT_FALSE(g_para[0].character.getmute);
        }
    }

    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}

/**
* @tc.name  RelAudioCaptureGetMute
* @tc.number  SUB_Audio_HDI_RelAudioCaptureGetMute_0001
* @tc.desc  test RelAudioCaptureGetMute interface Reliability pass through pthread_create(adapterName is same)
* @tc.author: wangkang
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetMute_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].character.setmute = true;
        ret = g_para[0].capture->volume.SetMute(g_para[0].capture, false);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetMute, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_FALSE(g_para[0].character.getmute);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureSetVolume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiocaptureSetVolume_Reliability_0001
* @tc.desc  test SetVolume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureSetVolume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeHighExpc = 0.70;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&g_para[0], sizeof(PrepareAudioPara), &g_para, sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = g_para[0].capture->volume.GetVolume(g_para[0].capture, &(g_para[0].character.getvolume));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].character.setvolume = 0.7;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureSetVolume, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        ret = g_para[0].capture->volume.GetVolume(g_para[0].capture, &(g_para[0].character.getvolume));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(volumeHighExpc, g_para[0].character.getvolume);
    }
    if (g_para[0].adapter != nullptr){
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}

/**
* @tc.name  test AudioCaptureGetVolume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetVolume_Reliability_0001
* @tc.desc  test GetVolume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetVolume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeHighExpc = 0.7;
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];

    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        arrpara[i].character.setvolume = 0.7;
        ret = arrpara[i].capture->volume.SetVolume(arrpara[i].capture, arrpara[i].character.setvolume);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetVolume, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(volumeHighExpc, arrpara[i].character.getvolume);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureSetSampleAttributes API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureSetSampleAttributes_Reliability_0001
* @tc.desc  test SetSampleAttributes interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureSetSampleAttributes_Reliability_0001,
         TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t count = 2;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(g_para[0].attrs);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {

        int32_t ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureSetSampleAttributes, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

        ret = g_para[0].capture->attr.GetSampleAttributes(g_para[0].capture, &(g_para[0].attrsValue));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(AUDIO_IN_MEDIA, g_para[0].attrsValue.type);
        EXPECT_FALSE(g_para[0].attrsValue.interleaved);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, g_para[0].attrsValue.format);
        EXPECT_EQ(SAMPLERATEVALUE, g_para[0].attrsValue.sampleRate);
        EXPECT_EQ(count, g_para[0].attrsValue.channelCount);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureGetSampleAttributes API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureGetSampleAttributes_Reliability_0001
* @tc.desc  test AudioCaptureGetSampleAttributes interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureGetSampleAttributes_Reliability_0001,
         TestSize.Level1)
{
    int32_t ret = -1;
    uint32_t count = 2;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);

    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(g_para[0].attrs);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = g_para[0].capture->attr.SetSampleAttributes(g_para[0].capture, &(g_para[0].attrs));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

        int32_t ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetSampleAttributes, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

        EXPECT_EQ(AUDIO_IN_MEDIA, g_para[0].attrsValue.type);
        EXPECT_FALSE(g_para[0].attrsValue.interleaved);
        EXPECT_EQ(AUDIO_FORMAT_PCM_16_BIT, g_para[0].attrsValue.format);
        EXPECT_EQ(SAMPLERATEVALUE, g_para[0].attrsValue.sampleRate);
        EXPECT_EQ(count, g_para[0].attrsValue.channelCount);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureSelectScene API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_Reliability_0001
* @tc.desc  test AudioCaptureSelectScene interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureSelectScene_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].scenes.scene.id = 0;
        g_para[0].scenes.desc.pins = PIN_IN_MIC;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureSelectScene, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureCheckSceneCapability API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureCheckSceneCapability_Reliability_0001
* @tc.desc  test AudioCaptureCheckSceneCapability interface Reliability pass through pthread_create fun and adapterName
            is same.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureReliabilityTest, SUB_Audio_HDI_AudioCaptureCheckSceneCapability_Reliability_0001,
         TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateCapture(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                             &g_para[0].capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        g_para[0].scenes.scene.id = 0;
        g_para[0].scenes.desc.pins = PIN_IN_MIC;
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureCheckSceneCapability, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}
}
