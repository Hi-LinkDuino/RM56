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
#include "audio_hdirender_control_reliability_test.h"

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

class AudioHdiRenderControlReliabilityTest : public testing::Test {
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
    static int32_t RelGetAllAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelLoadAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelUnloadAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderStart(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderFrame(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderStop(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderProcedure(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderPause(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderResume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioRenderGetRenderPosition(struct PrepareAudioPara& ptr);
};

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiRenderControlReliabilityTest::GetAudioManager)() = nullptr;
void *AudioHdiRenderControlReliabilityTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiRenderControlReliabilityTest::SdkInit)() = nullptr;
    void (*AudioHdiRenderControlReliabilityTest::SdkExit)() = nullptr;
    void *AudioHdiRenderControlReliabilityTest::sdkSo = nullptr;
#endif

void AudioHdiRenderControlReliabilityTest::SetUpTestCase(void)
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

void AudioHdiRenderControlReliabilityTest::TearDownTestCase(void)
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

void AudioHdiRenderControlReliabilityTest::SetUp(void) {}

void AudioHdiRenderControlReliabilityTest::TearDown(void) {}

int32_t AudioHdiRenderControlReliabilityTest::RelGetAllAdapter(struct PrepareAudioPara& ptr)
{
    if (ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int size = 0;
    g_testMutex.lock();
    int32_t ret = ptr.manager->GetAllAdapters(ptr.manager, &ptr.descs, &size);
    g_testMutex.unlock();
    if (ret < 0) {
        return ret;
    }
    if (ptr.descs == nullptr || size == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int index = SwitchAdapter(ptr.descs, ptr.adapterName, ptr.portType, ptr.audioPort, size);
    if (index < 0) {
        return index;
    }
    ptr.desc = &ptr.descs[index];
    if (ptr.desc == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiRenderControlReliabilityTest::RelLoadAdapter(struct PrepareAudioPara& ptr)
{
    if (ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    int32_t ret = ptr.manager->LoadAdapter(ptr.manager, ptr.desc, &ptr.adapter);
    g_testMutex.unlock();
    if (ret < 0) {
        return ret;
    }

    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderStart(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->control.Start((AudioHandle)(ptr.render));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderFrame(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    uint64_t requestBytes = 0;
    uint64_t replyBytes = 0;
    char *frame = nullptr;

    ret = RenderFramePrepare(ptr.path, frame, requestBytes);
    if (ret < 0) {
        return ret;
    }
    g_testMutex.lock();
    ret = ptr.render->RenderFrame(ptr.render, frame, requestBytes, &replyBytes);
    g_testMutex.unlock();
    if (frame != nullptr) {
        free(frame);
        frame = nullptr;
    }
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderStop(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->control.Stop((AudioHandle)(ptr.render));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderPause(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->control.Pause((AudioHandle)(ptr.render));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderResume(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->control.Resume((AudioHandle)(ptr.render));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderProcedure(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    g_testMutex.lock();
    ret = AudioCreateRender(ptr.manager, ptr.pins, ptr.adapterName, &ptr.adapter, &ptr.render);
    g_testMutex.unlock();
    if (ret < 0) {
        return ret;
    }
    ret = AudioRenderStartAndOneFrame(ptr.render);
    return ret;
}

int32_t AudioHdiRenderControlReliabilityTest::RelAudioRenderGetRenderPosition(struct PrepareAudioPara& ptr)
{
    if (ptr.render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.render->GetRenderPosition(ptr.render, &(ptr.character.getframes), &(ptr.time));
    g_testMutex.unlock();
    return ret;
}

/**
* @tc.name  test GetAllAdapter API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioGetAllAdapter_Reliability_0001
* @tc.desc  test Reliability GetAllAdapters interface.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioGetAllAdapter_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelGetAllAdapter, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
}

/**
* @tc.name  test LoadAdapter API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioLoadlAdapter_Reliability_0001
* @tc.desc  test LoadAdapter interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioLoadlAdapter_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelLoadAdapter, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        ret = (intptr_t)result;
        if (ret == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOTREADY, ret);
            failcount = failcount + 1;
        }
    }
    EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
    EXPECT_EQ(succeedcount, 1);
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
    g_para[0].adapter = nullptr;
}

/**
* @tc.name  test AudioRenderStart API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderStart_Reliability_0001
* @tc.desc  test AudioRenderStart interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderStart_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = AudioCreateRender(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                            &g_para[0].render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderStart, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        ret = (intptr_t)result;
        if (ret == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_AO_BUSY, ret);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderFrame API via Multithread call.
* @tc.number  SUB_Audio_HDI_RelAudioRenderFrame_Reliability_0001
* @tc.desc  test AudioRenderFrame iinterface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderFrame_Reliability_0001, TestSize.Level1)
{
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    int32_t ret = -1;
    ret = AudioCreateRender(g_para[0].manager, g_para[0].pins, g_para[0].adapterName, &g_para[0].adapter,
                            &g_para[0].render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioRenderStart(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderFrame, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    ret = StopAudio(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    g_para[0].render = nullptr;
}

/**
* @tc.name  test AudioRenderStop API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderStop_Reliability_0001
* @tc.desc  test AudioRenderStop interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderStop_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderStop, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        ret = (intptr_t)result;
        if (ret == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].manager != nullptr && g_para[0].adapter != nullptr) {
        g_para[0].adapter->DestroyRender(g_para[0].adapter, g_para[0].render);
        g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderPause API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderPause_Reliability_0001
* @tc.desc  test AudioRenderPause interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderPause_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    int32_t succeedcount = 0;

    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderPause, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        ret = (intptr_t)result;
        if (ret == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderResume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioRenderResume_Reliability_0001
* @tc.desc  test RelAudioRenderResume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderResume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioRenderPause(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderResume, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        ret = (intptr_t)result;
        if (ret == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].render = nullptr;
    }
}

/**
* @tc.name  test AudioRenderGetRenderPosition API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudiorenderGetVolume_Reliability_0001
* @tc.desc  test GetRenderPosition interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiRenderControlReliabilityTest, SUB_Audio_HDI_AudioRenderGetRenderPosition_Reliability_0001,
         TestSize.Level1)
{
    int32_t ret = -1;
    int64_t timeExp = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    struct PrepareAudioPara arrpara[PTHREAD_SAMEADA_COUNT];
    ret = RelAudioRenderProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&arrpara[i], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        arrpara[i].time = {.tvSec = 0, .tvNSec = 0};
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioRenderGetRenderPosition, &arrpara[i]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(INITIAL_VALUE, arrpara[i].character.getframes);
        EXPECT_LT(timeExp, (arrpara[i].time.tvSec) * SECTONSEC + (arrpara[i].time.tvNSec));
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        g_para[0].render = nullptr;
    }
}
}