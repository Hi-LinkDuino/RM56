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
#include "audio_hdirender_control_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioHdiRenderControlTest : public testing::Test {
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

TestAudioManager *(*AudioHdiRenderControlTest::GetAudioManager)() = nullptr;
void *AudioHdiRenderControlTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiRenderControlTest::SdkInit)() = nullptr;
    void (*AudioHdiRenderControlTest::SdkExit)() = nullptr;
    void *AudioHdiRenderControlTest::sdkSo = nullptr;
#endif

void AudioHdiRenderControlTest::SetUpTestCase(void)
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

void AudioHdiRenderControlTest::TearDownTestCase(void)
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

void AudioHdiRenderControlTest::SetUp(void) {}

void AudioHdiRenderControlTest::TearDown(void) {}

/**
    * @tc.name  Test AudioRenderStart API via  legal input
    * @tc.number  SUB_Audio_HDI_RenderStart_0001
    * @tc.desc  Test AudioRenderStart interface,return 0 if the audiorender object is created successfully.
    * @tc.author: wangqian
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStart_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;

    ASSERT_NE(nullptr, GetAudioManager);
    manager = GetAudioManager();
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioRenderStart API via setting the incoming parameter handle is nullptr
    * @tc.number  SUB_Audio_HDI_RenderStart_0002
    * @tc.desc  Test AudioRenderStart interface, return -1 if the  incoming parameter handle is nullptr
    * @tc.author: wangqian
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStart_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    manager = GetAudioManager();
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderStart API via the interface is called twice in a row
* @tc.number  SUB_Audio_HDI_RenderStart_0003
* @tc.desc  Test AudioRenderStart interface,return -1 the second time if the RenderStart is called twice
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStart_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioRender *render = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_AO_BUSY, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderStop API via legal input
* @tc.number  SUB_Audio_HDI_RenderStop_0001
* @tc.desc  test AudioRenderStop interface. return 0 if the rendering is successfully stopped.
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStop_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioRender *render = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderStop API via the render does not start and stop only
* @tc.number  SUB_Audio_HDI_RenderStop_0002
* @tc.desc  test AudioRenderStop interface. return -4 if the render does not start and stop only
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStop_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test RenderStop API via the interface is called twice in a row
* @tc.number  SUB_Audio_HDI_RenderStop_0003
* @tc.desc  Test RenderStop interface,return -4 the second time if the RenderStop is called twice
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStop_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test RenderStop API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_RenderStop_0004
* @tc.desc  Test RenderStop interface, return -1 if the incoming parameter handle is nullptr
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderStop_0004, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderPause API via legal input
    * @tc.number  SUB_Audio_HDI_RenderPause_001
    * @tc.desc  test HDI RenderPause interface，return 0 if the render is paused after start
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderPause API via the interface is called twice in a row
* @tc.number  SUB_Audio_HDI_RenderPause_0002
* @tc.desc  Test AudioRenderPause interface, return -1 the second time if RenderPause is called twice
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    TestAudioManager* manager = {};

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    render->control.Stop((AudioHandle)render);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderPause API via the render is paused after created.
* @tc.number  SUB_Audio_HDI_RenderPause_0003
* @tc.desc  Test AudioRenderPause interface,return -1 if the render is paused after created.
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderPause API via the render is paused after resumed.
* @tc.number  SUB_Audio_HDI_RenderPause_0004
* @tc.desc  Test AudioRenderPause interface,return 0 if the render is paused after resumed.
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0004, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test AudioRenderPause API via the render is paused after stopped.
* @tc.number  SUB_Audio_HDI_RenderPause_0005
* @tc.desc  Test AudioRenderPause interface, return -1 the render is paused after stopped.
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    TestAudioManager* manager = {};

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test RenderPause API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_RenderPause_0006
* @tc.desc  Test RenderPause interface, return -1 if the incoming parameter handle is nullptr
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderPause_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;
    TestAudioManager* manager = {};

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderResume API via the render is resumed after started
    * @tc.number  SUB_Audio_HDI_RenderResume_0001
    * @tc.desc  test HDI RenderResume interface,return -1 if the render is resumed after started
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderResume API via the render is resumed after stopped
    * @tc.number  SUB_Audio_HDI_RenderResume_0002
    * @tc.desc  test HDI RenderResume interface,return -1 if the render is resumed after stopped
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderResume API via legal input
    * @tc.number  SUB_Audio_HDI_RenderResume_0003
    * @tc.desc  Test AudioRenderResume interface,return 0 if the render is resumed after paused
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderResume API via the interface is called twice in a row
    * @tc.number  SUB_Audio_HDI_RenderResume_0004
    * @tc.desc  Test RenderResume interface,return -1 the second time if the RenderResume is called twice
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderResume API via the render Continue to start after resume
    * @tc.number  SUB_Audio_HDI_RenderResume_0005
    * @tc.desc  test HDI RenderResume interface,return -1 if the render Continue to start after resume
    * @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Start((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_AO_BUSY, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test RenderResume API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_RenderResume_0007
* @tc.desc  Test RenderResume interface, return -1 if the incoming parameter handle is nullptr
* @tc.author: Xuhuandi
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderResume_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Resume((AudioHandle)renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCreateRender API via legal input.
    * @tc.number  SUB_Audio_HDI_CreateRender_0001
    * @tc.desc  test AudioCreateRender interface,return 0 if render is created successful.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCreateRender API when two renders is created successful.
    * @tc.number  SUB_Audio_HDI_AudioCreateRender_0002
    * @tc.desc  Test AudioCreateRender interface,return 0 when two renders is created successful.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_AudioCreateRender_0002, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapter2 = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *render2 = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();

    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);

    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter2, &render2);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter2->DestroyRender(adapter2, render2);
    manager->UnloadAdapter(manager, adapter2);
}
/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter pins is PIN_IN_MIC.
    * @tc.number  SUB_Audio_HDI_CreateRender_0003
    * @tc.desc  test AudioCreateRender interface,return -1 if the incoming parameter pins is PIN_IN_MIC.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioPort* renderPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_IN_MIC);

    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter attr is error.
    * @tc.number  SUB_Audio_HDI_CreateRender_0004
    * @tc.desc  test AudioCreateRender interface,return -1 if the incoming parameter attr is error.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort* renderPort = nullptr;
    uint32_t channelCountErr = 5;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);
    attrs.format = AUDIO_FORMAT_AAC_MAIN;
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    attrs.channelCount = channelCountErr;
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    attrs.type = AUDIO_IN_COMMUNICATION;
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter adapter is nullptr
    * @tc.number  SUB_Audio_HDI_CreateRender_0005
    * @tc.desc  test AudioCreateRender interface,Returns -1 if the incoming parameter adapter is nullptr.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    struct AudioPort* renderPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);

    ret = adapter->CreateRender(adapterNull, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter devDesc is nullptr
    * @tc.number  SUB_Audio_HDI_CreateRender_0006
    * @tc.desc  test AudioCreateRender interface,Returns -1 if the incoming parameter devDesc is nullptr.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioRender *render = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioPort* renderPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor *devDescNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitAttrs(attrs);

    ret = adapter->CreateRender(adapter, devDescNull, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter attrs is nullptr
    * @tc.number  SUB_Audio_HDI_CreateRender_0007
    * @tc.desc  test AudioCreateRender interface,Returns -1 if the incoming parameter attrs is nullptr.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0007, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* renderPort = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioSampleAttributes *attrsNull = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);

    ret = adapter->CreateRender(adapter, &devDesc, attrsNull, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter render is nullptr
    * @tc.number  SUB_Audio_HDI_CreateRender_0008
    * @tc.desc  test AudioCreateRender interface,Returns -1 if the incoming parameter render is nullptr.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0008, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender **renderNull = nullptr;
    struct AudioPort* renderPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);

    ret = adapter->CreateRender(adapter, &devDesc, &attrs, renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test AudioCreateRender API via setting the incoming parameter devDesc is error
    * @tc.number  SUB_Audio_HDI_CreateRender_0009
    * @tc.desc  test AudioCreateRender interface,Returns -1 if the incoming parameter devDesc is error.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_CreateRender_0009, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort* renderPort = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, PORT_OUT, ADAPTER_NAME_USB, &adapter, renderPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, PIN_OUT_SPEAKER);

    devDesc.portId = -5;
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    devDesc.pins = PIN_NONE;
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    devDesc.desc = "devtestname";
    ret = adapter->CreateRender(adapter, &devDesc, &attrs, &render);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioDestroyRender API via legal input.
    * @tc.number  SUB_Audio_HDI_DestroyRender_0001
    * @tc.desc  Test AudioDestroyRender interface, return 0 if render is destroyed successful.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_DestroyRender_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = adapter->DestroyRender(adapter, render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioDestroyRender API,where the parameter render is empty.
    * @tc.number  SUB_Audio_HDI_DestroyRender_0002
    * @tc.desc  Test AudioDestroyRender interface, return -1 if the parameter render is empty.
    * @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_DestroyRender_0002, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;
    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateRender(manager, pins, ADAPTER_NAME_USB, &adapter, &render);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = adapter->DestroyRender(adapter, renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test RenderFlush API via legal input Verify that the data in the buffer is flushed after stop
    * @tc.number  SUB_Audio_HDI_RenderFlush_0001
    * @tc.desc  Test RenderFlush interface,return -2 if the data in the buffer is flushed successfully after stop
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderFlush_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Flush((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test RenderFlush that the data in the buffer is flushed when handle is nullptr after paused
    * @tc.number  SUB_Audio_HDI_RenderFlush_0002
    * @tc.desc  Test RenderFlush, return -1 if the data in the buffer is flushed when handle is nullptr after paused
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiRenderControlTest, SUB_Audio_HDI_RenderFlush_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioRender *renderNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartRender(manager, &render, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Pause((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = render->control.Flush((AudioHandle)renderNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = render->control.Stop((AudioHandle)render);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
}
