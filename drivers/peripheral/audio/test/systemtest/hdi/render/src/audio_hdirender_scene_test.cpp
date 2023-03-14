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
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hdi_common.h
 *
 * @brief Declares APIs for operations related to the audio render adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"
#include "audio_hdirender_scene_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioHdiRenderSceneTest : public testing::Test {
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

using THREAD_FUNC = void *(*)(void *);

TestAudioManager *(*AudioHdiRenderSceneTest::GetAudioManager)() = nullptr;
void *AudioHdiRenderSceneTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiRenderSceneTest::SdkInit)() = nullptr;
    void (*AudioHdiRenderSceneTest::SdkExit)() = nullptr;
    void *AudioHdiRenderSceneTest::sdkSo = nullptr;
#endif

void AudioHdiRenderSceneTest::SetUpTestCase(void)
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

void AudioHdiRenderSceneTest::TearDownTestCase(void)
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

void AudioHdiRenderSceneTest::SetUp(void) {}

void AudioHdiRenderSceneTest::TearDown(void) {}

int32_t AudioHdiRenderSceneTest::GetManager(struct PrepareAudioPara& audiopara)
{
    auto *inst = (AudioHdiRenderSceneTest *)audiopara.self;
    if (inst != nullptr && inst->GetAudioManager != nullptr) {
        audiopara.manager = inst->GetAudioManager();
    }
    if (audiopara.manager == nullptr) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
/**
* @tc.name   Test AudioRenderCheckSceneCapability API and check scene's capability
* @tc.number  SUB_Audio_HDI_RenderCheckSceneCapability_0001
* @tc.desc  Test AudioRenderCheckSceneCapability interface,return 0 if check scene's capability successful.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_RenderCheckSceneCapability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = false;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;
    ret = render->scene.CheckSceneCapability(render, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_TRUE(supported);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the scene is not configed in the josn.
* @tc.number  SUB_Audio_HDI_RenderCheckSceneCapability_0002
* @tc.desc  Test RenderCheckSceneCapability interface,return -1 if the scene is not configed in the josn.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_RenderCheckSceneCapability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 5;
    scenes.desc.pins = PIN_OUT_SPEAKER;
    ret = render->scene.CheckSceneCapability(render, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the render is empty
* @tc.number  SUB_Audio_HDI_RenderCheckSceneCapability_0003
* @tc.desc  Test AudioRenderCheckSceneCapability,return -1 if the render is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_RenderCheckSceneCapability_0003, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;
    ret = render->scene.CheckSceneCapability(renderNull, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test AudioRenderCheckSceneCapability API and check scene's capability
* @tc.number  SUB_Audio_HDI_RenderCheckSceneCapability_0004
* @tc.desc  Test AudioRenderCheckSceneCapability interface,return -1 if the scene is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_RenderCheckSceneCapability_0004, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor *scenes = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->scene.CheckSceneCapability(render, scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test AudioRenderCheckSceneCapability API and check scene's capability
* @tc.number  SUB_Audio_HDI_RenderCheckSceneCapability_0005
* @tc.desc  Test AudioRenderCheckSceneCapability interface,return -1 if the supported is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_RenderCheckSceneCapability_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;
    ret = render->scene.CheckSceneCapability(render, &scenes, nullptr);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderSelectScene API via legal input
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_0001
* @tc.desc  Test AudioRenderSelectScene interface,return 0 if select Render's scene successful.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderSelectScene_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;

    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->scene.SelectScene(render, &scenes);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = AudioRenderStartAndOneFrame(render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderSelectScene API after Render start.
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_0002
* @tc.desc  Test AudioRenderSelectScene, return 0 if select Render's scene successful after Render start.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderSelectScene_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    manager = GetAudioManager();
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_OUT_SPEAKER;
    ret = render->scene.SelectScene(render, &scenes);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderSelectScene API where the parameter handle is empty.
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_0003
* @tc.desc  Test AudioRenderSelectScene, return -1 if the parameter handle is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderSelectScene_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = render->scene.SelectScene(renderNull, &scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderSelectScene API where the parameter scene is empty.
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_0004
* @tc.desc  Test AudioRenderSelectScene, return -1 if the parameter scene is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderSelectScene_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor *scenes = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->scene.SelectScene(render, scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderSelectScene API where the scene is not configed in the josn.
* @tc.number  SUB_Audio_HDI_AudioRenderSelectScene_0005
* @tc.desc  Test AudioRenderSelectScene, return -1 if the scene is not configed in the josn.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderSelectScene_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 5;
    scenes.desc.pins = PIN_OUT_HDMI;
    ret = render->scene.SelectScene(render, &scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
#ifdef AUDIO_ADM_SO
/**
* @tc.name  Test AudioRenderTurnStandbyMode API via input "AUDIO_FLUSH_COMPLETED"
* @tc.number  SUB_Audio_HDI_AudioRenderRegCallback_0001
* @tc.desc  Test AudioRenderTurnStandbyMode interface,return 0 if the interface use correctly.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderRegCallback_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    ASSERT_NE(GetAudioManager, nullptr);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->RegCallback(render, AudioRenderCallback, nullptr);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Flush((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = CheckFlushValue();
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}

/**
* @tc.name  Test AudioRenderRegCallback API via input "AUDIO_NONBLOCK_WRITE_COMPELETED"
* @tc.number  SUB_Audio_HDI_AudioRenderRegCallback_0002
* @tc.desc  Test AudioRenderRegCallback interface,return 0 if the interface use correctly.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderRegCallback_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioSampleAttributes attrs;
    struct AudioHeadInfo headInfo;
    char absPath[PATH_MAX] = {0};
    realpath(AUDIO_FILE.c_str(), absPath);
    ASSERT_NE(realpath(AUDIO_FILE.c_str(), absPath), nullptr);

    FILE *file = fopen(absPath, "rb");
    ASSERT_NE(file, nullptr);
    ASSERT_NE(GetAudioManager, nullptr);
    TestAudioManager* manager = GetAudioManager();
    ret = WavHeadAnalysis(headInfo, file, attrs);
    if (ret < 0) {
        fclose(file);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->RegCallback(render, AudioRenderCallback, nullptr);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = FrameStart(headInfo, render, file, attrs);
    if (ret < 0) {
        adapter->DestroyRender(adapter, render);
        manager->UnloadAdapter(manager, adapter);
        fclose(file);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }

    ret = CheckWriteCompleteValue();
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = CheckRenderFullValue();
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
    fclose(file);
}
/**
* @tc.name  Test AudioRenderTurnStandbyMode API via
* @tc.number  SUB_Audio_HDI_AudioRenderTurnStandbyMode_0001
* @tc.desc  Test AudioRenderTurnStandbyMode interface,return 0 if the interface use correctly.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderTurnStandbyMode_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    ASSERT_NE(GetAudioManager, nullptr);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.TurnStandbyMode((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    sleep(3);

    ret = render->control.Stop((AudioHandle)render);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioRenderAudioDevDump API via
* @tc.number  SUB_Audio_HDI_AudioRenderAudioDevDump_0001
* @tc.desc  Test AudioRenderAudioDevDump interface,return 0 if the interface use correctly.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderSceneTest, SUB_Audio_HDI_AudioRenderAudioDevDump_0001, TestSize.Level1)
{
    int32_t ret = -1;
    char pathBuf[] = "./DevDump.log";
    FILE *fp = fopen(pathBuf, "wb+");
    ASSERT_NE(nullptr, fp);
    int fd = fileno(fp);
    if (fd == -1) {
        fclose(fp);
        ASSERT_NE(fd, -1);
    }
    struct PrepareAudioPara audiopara = {
        .portType = PORT_OUT, .adapterName = ADAPTER_NAME_USB.c_str(), .self = this, .pins = PIN_OUT_SPEAKER,
        .path = AUDIO_FILE.c_str()
    };
    ret = GetManager(audiopara);
    if (ret < 0) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = pthread_create(&audiopara.tids, NULL, (THREAD_FUNC)PlayAudioFile, &audiopara);
    if (ret < 0) {
        fclose(fp);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    sleep(1);
    ret = audiopara.render->control.Pause((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    sleep(1);
    ret = audiopara.render->control.Resume((AudioHandle)audiopara.render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = audiopara.render->control.AudioDevDump((AudioHandle)audiopara.render, RANGE, fd);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    fclose(fp);
    ret = ThreadRelease(audiopara);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
}
#endif
}