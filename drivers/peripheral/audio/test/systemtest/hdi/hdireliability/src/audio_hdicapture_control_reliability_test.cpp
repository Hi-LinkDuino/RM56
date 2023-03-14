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
#include "audio_hdicapture_control_reliability_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const int PTHREAD_SAMEADA_COUNT = 3;
const int PTHREAD_DIFFADA_COUNT = 1;
const int BUFFER_SIZE = 16384;
mutex g_testMutex;
static struct PrepareAudioPara g_para[PTHREAD_DIFFADA_COUNT] = {
    {
        .portType = PORT_IN, .adapterName = ADAPTER_NAME_USB.c_str(), .pins = PIN_IN_MIC,
        .path = AUDIO_CAPTURE_FILE.c_str()
    }
};

class AudioHdiCaptureControlReliabilityTest : public testing::Test {
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
    static int32_t RelAudioCaptureStart(struct PrepareAudioPara& ptr);
    static int32_t RelGetAllAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelLoadAdapter(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureStop(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureResume(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCapturePause(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureProcedure(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureFrame(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureStartAndCaputreFrame(struct PrepareAudioPara& ptr);
    static int32_t RelAudioAdapterInitAllPorts(struct PrepareAudioPara& ptr);
    static int32_t RelAudioAdapterGetPortCapability(struct PrepareAudioPara& ptr);
    static int32_t RelAudioAdapterSetPassthroughMode(struct PrepareAudioPara& ptr);
    static int32_t RelAudioAdapterGetPassthroughMode(struct PrepareAudioPara& ptr);
    static int32_t RelAudioCaptureGetCapturePosition(struct PrepareAudioPara& ptr);
};

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiCaptureControlReliabilityTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureControlReliabilityTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureControlReliabilityTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureControlReliabilityTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureControlReliabilityTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureControlReliabilityTest::SetUpTestCase(void)
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

void AudioHdiCaptureControlReliabilityTest::TearDownTestCase(void)
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

void AudioHdiCaptureControlReliabilityTest::SetUp(void) {}

void AudioHdiCaptureControlReliabilityTest::TearDown(void) {}

int32_t AudioHdiCaptureControlReliabilityTest::RelGetAllAdapter(struct PrepareAudioPara& ptr)
{
    int size = 0;
    if (ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
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
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ptr.desc = &ptr.descs[index];
    if (ptr.desc == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelLoadAdapter(struct PrepareAudioPara& ptr)
{
    if (ptr.desc == nullptr || ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    int32_t ret = ptr.manager->LoadAdapter(ptr.manager, ptr.desc, &ptr.adapter);
    g_testMutex.unlock();
    if (ret < 0) {
        return ret;
    }
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCreateCapture(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    if (ptr.adapter == nullptr  || ptr.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    InitAttrs(ptr.attrs);
    InitDevDesc(ptr.devDesc, ptr.audioPort->portId, ptr.pins);
    g_testMutex.lock();
    ret = ptr.adapter->CreateCapture(ptr.adapter, &ptr.devDesc, &ptr.attrs, &ptr.capture);
    g_testMutex.unlock();
    if (ret < 0) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return ret;
    }
    if (ptr.capture == nullptr) {
        ptr.manager->UnloadAdapter(ptr.manager, ptr.adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureStart(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->control.Start((AudioHandle)(ptr.capture));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureFrame(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    uint64_t requestBytes = BUFFER_SIZE;
    uint64_t replyBytes = 0;

    char *frame = (char *)calloc(1, BUFFER_SIZE);
    if (frame == nullptr) {
        return HDF_ERR_MALLOC_FAIL;
    }

    g_testMutex.lock();
    ret = ptr.capture->CaptureFrame(ptr.capture, frame, requestBytes, &replyBytes);
    g_testMutex.unlock();
    free(frame);
    frame = nullptr;
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureStartAndCaputreFrame(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    FILE *file = fopen(ptr.path, "wb+");
    if (file == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(ptr.attrs);

    ret = FrameStartCapture(ptr.capture, file, ptr.attrs);
    if (ret < 0) {
        fclose(file);
        return ret;
    }
    fclose(file);
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureStop(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->control.Stop((AudioHandle)(ptr.capture));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCapturePause(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->control.Pause((AudioHandle)(ptr.capture));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureResume(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->control.Resume((AudioHandle)(ptr.capture));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureProcedure(struct PrepareAudioPara& ptr)
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

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioAdapterInitAllPorts(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ret = ptr.adapter->InitAllPorts(ptr.adapter);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioAdapterGetPortCapability(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ret = ptr.adapter->GetPortCapability(ptr.adapter, ptr.audioPort, &(ptr.capability));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioAdapterSetPassthroughMode(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ret = ptr.adapter->SetPassthroughMode(ptr.adapter, ptr.audioPort, ptr.mode);
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioAdapterGetPassthroughMode(struct PrepareAudioPara& ptr)
{
    int32_t ret = -1;
    if (ptr.adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    g_testMutex.lock();
    ret = ptr.adapter->GetPassthroughMode(ptr.adapter, ptr.audioPort, &(ptr.mode));
    g_testMutex.unlock();
    return ret;
}

int32_t AudioHdiCaptureControlReliabilityTest::RelAudioCaptureGetCapturePosition(struct PrepareAudioPara& ptr)
{
    if (ptr.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = -1;
    g_testMutex.lock();
    ret = ptr.capture->GetCapturePosition(ptr.capture, &(ptr.character.getframes), &(ptr.time));
    g_testMutex.unlock();
    return ret;
}

/**
* @tc.name  test AudioCaptureFrame API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureFrame_Reliability_0001
* @tc.desc  test AudioCaptureFrame interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioCaptureFrame_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateCapture(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCaptureStart(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureFrame, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}

/**
* @tc.name  test AudioCaptureStart API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureStart_Reliability_0001
* @tc.desc  test AudioCaptureStart interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioCaptureStart_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCreateCapture(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureStart, &g_para[0]);
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
            EXPECT_EQ(AUDIO_HAL_ERR_AI_BUSY, ret);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
    }
}


/**
* @tc.name  test AudioCaptureStop API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureStop_Reliability_0001
* @tc.desc  test AudioCaptureStop interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioCaptureStop_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureStop, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        if ((intptr_t)result == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, (intptr_t)result);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].manager != nullptr && g_para[0].adapter != nullptr) {
        g_para[0].adapter->DestroyCapture(g_para[0].adapter, g_para[0].capture);
        g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
    }
}


/**
* @tc.name  test AudioCapturePause API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCapturePause_Reliability_0001
* @tc.desc  test AudioCapturePause interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioCapturePause_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCapturePause, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        if ((intptr_t)result == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, (intptr_t)result);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
    }
}

/**
* @tc.name  test RelAudioAdapterInitAllPorts API via Multi thread calling mono card
* @tc.number  SUB_Audio_HDI_AudioInitAllPorts_Reliability_0002
* @tc.desc  test InitAllPorts interface, return 0 if the ports is initialize successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioInitAllPorts_Reliability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelGetAllAdapter(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioAdapterInitAllPorts, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test RelAudioAdapterGetPortCapability API via Multi thread calling mono card
* @tc.number  SUB_Audio_HDI_AudioGetPortCapability_Reliability_0002
* @tc.desc  test GetPortCapability interface,return 0 if the Get Port capability successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioGetPortCapability_Reliability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioAdapterInitAllPorts(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioAdapterGetPortCapability, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
    }
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test RelAudioAdapterSetPassthroughMode API via Multi thread calling mono card
* @tc.number  SUB_Audio_HDI_AudioSetPassthroughMode_Reliability_0002
* @tc.desc  test SetPassthroughMode interface,return 0 if the Set Passthrough Mode successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioSetPassthroughMode_Reliability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    g_para[0].mode = PORT_PASSTHROUGH_LPCM;
    g_para[0].portType = PORT_OUT;
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioAdapterInitAllPorts(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioAdapterSetPassthroughMode, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        ret = g_para[0].adapter->GetPassthroughMode(g_para[0].adapter, g_para[0].audioPort, &(g_para[0].mode));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(PORT_PASSTHROUGH_LPCM, g_para[0].mode);
    }
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test RelAudioAdapterGetPassthroughMode API via Multi thread calling mono card
* @tc.number  SUB_Audio_HDI_AudioGetPassthroughMode_Reliability_0002
* @tc.desc  test GetPassthroughMode interface,return 0 if the Get Passthrough Mode successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioGetPassthroughMode_Reliability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    g_para[0].portType = PORT_OUT;
    g_para[0].mode = PORT_PASSTHROUGH_LPCM;
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelGetAllAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelLoadAdapter(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioAdapterInitAllPorts(g_para[0]);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = RelAudioAdapterSetPassthroughMode(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioAdapterGetPassthroughMode, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_EQ(PORT_PASSTHROUGH_LPCM, g_para[0].mode);
    }
    g_para[0].manager->UnloadAdapter(g_para[0].manager, g_para[0].adapter);
}

/**
* @tc.name  test AudioCaptureResume API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioCaptureResume_Reliability_0001
* @tc.desc  test RelAudioCaptureResume interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioCaptureResume_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t failcount = 0;
    int32_t succeedcount = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = RelAudioCapturePause(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = memcpy_s(&g_para[0], sizeof(PrepareAudioPara), &g_para[0], sizeof(PrepareAudioPara));
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureResume, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        if ((intptr_t)result == 0) {
            EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
            succeedcount = succeedcount + 1;
        } else {
            EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, (intptr_t)result);
            failcount = failcount + 1;
        }
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
        EXPECT_EQ(failcount, PTHREAD_SAMEADA_COUNT - 1);
        EXPECT_EQ(succeedcount, 1);
    }
}

/**
* @tc.name  test AudioGetCapturePosition API via Multithread call.
* @tc.number  SUB_Audio_HDI_AudioGetCapturePosition_Reliability_0001
* @tc.desc  test AudioGetCapturePosition interface Reliability pass through pthread_create fun and adapterName is same.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioHdiCaptureControlReliabilityTest, SUB_Audio_HDI_AudioGetCapturePosition_Reliability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int64_t timeExp = 0;
    ASSERT_NE(nullptr, GetAudioManager);
    g_para[0].manager = GetAudioManager();
    ASSERT_NE(nullptr, g_para[0].manager);
    ret = RelAudioCaptureProcedure(g_para[0]);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    pthread_t tids[PTHREAD_SAMEADA_COUNT];
    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        ret = pthread_create(&tids[i], NULL, (THREAD_FUNC)RelAudioCaptureGetCapturePosition, &g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    for (int32_t i = 0; i < PTHREAD_SAMEADA_COUNT; ++i) {
        void *result = nullptr;
        pthread_join(tids[i], &result);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, (intptr_t)result);
        EXPECT_LT(INITIAL_VALUE, g_para[0].character.getframes);
        EXPECT_LT(timeExp, g_para[0].time.tvNSec);
    }
    if (g_para[0].adapter != nullptr) {
        ret = StopAudio(g_para[0]);
        EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
}
}
