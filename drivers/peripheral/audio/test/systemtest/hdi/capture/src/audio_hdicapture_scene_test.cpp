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
#include "audio_hdicapture_scene_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_HDMI = "hdmi";
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioHdiCaptureSceneTest : public testing::Test {
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

TestAudioManager *(*AudioHdiCaptureSceneTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureSceneTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureSceneTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureSceneTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureSceneTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureSceneTest::SetUpTestCase(void)
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

void AudioHdiCaptureSceneTest::TearDownTestCase(void)
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
int32_t AudioHdiCaptureSceneTest::AudioCaptureStart(const string path, struct AudioCapture *capture) const
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};

    ret = InitAttrs(attrs);
    if (ret < 0) {
        return ret;
    }
    FILE *file = fopen(path.c_str(), "wb+");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    ret = FrameStartCapture(capture, file, attrs);
    fclose(file);
    return ret;
}
void AudioHdiCaptureSceneTest::SetUp(void) {}
void AudioHdiCaptureSceneTest::TearDown(void) {}

/**
* @tc.name   Test AudioCaptureCheckSceneCapability API and check scene's capability
* @tc.number  SUB_Audio_HDI_CaptureCheckSceneCapability_0001
* @tc.desc  Test AudioCaptureCheckSceneCapability interface,return 0 if check scene's capability successful.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_CaptureCheckSceneCapability_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = false;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.CheckSceneCapability(capture, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    EXPECT_TRUE(supported);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the scene is not configured in the json.
* @tc.number  SUB_Audio_HDI_CaptureCheckSceneCapability_0002
* @tc.desc  Test AudioCreateCapture interface,return -1 if the scene is not configured in the json.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_CaptureCheckSceneCapability_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    scenes.scene.id = 5;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.CheckSceneCapability(capture, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the capture is empty
* @tc.number  SUB_Audio_HDI_CaptureCheckSceneCapability_0003
* @tc.desc  Test AudioCreateCapture interface,return -1 if the capture is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_CaptureCheckSceneCapability_0003, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.CheckSceneCapability(captureNull, &scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = AudioCaptureStart(AUDIO_CAPTURE_FILE, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the scene is empty
* @tc.number  SUB_Audio_HDI_CaptureCheckSceneCapability_0004
* @tc.desc  Test AudioCreateCapture interface,return -1 if the scene is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_CaptureCheckSceneCapability_0004, TestSize.Level1)
{
    int32_t ret = -1;
    bool supported = true;
    struct AudioSceneDescriptor *scenes = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->scene.CheckSceneCapability(capture, scenes, &supported);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = AudioCaptureStart(AUDIO_CAPTURE_FILE, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name   Test checking scene's capability where the parameter supported is empty.
* @tc.number  SUB_Audio_HDI_CaptureCheckSceneCapability_0005
* @tc.desc  Test AudioCreateCapture interface,return -1 if the parameter supported is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_CaptureCheckSceneCapability_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.CheckSceneCapability(capture, &scenes, nullptr);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSelectScene API via legal input
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_0001
* @tc.desc  Test AudioCaptureSelectScene interface,return 0 if select capture's scene successful.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_AudioCaptureSelectScene_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.SelectScene(capture, &scenes);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSelectScene API after capture start.
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_0002
* @tc.desc  Test AudioCaptureSelectScene, return 0 if select capture's scene successful after capture start.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_AudioCaptureSelectScene_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = AudioCaptureStart(AUDIO_CAPTURE_FILE, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.SelectScene(capture, &scenes);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSelectScene API where the parameter handle is empty.
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_0003
* @tc.desc  Test AudioCaptureSelectScene, return -1 if the parameter handle is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_AudioCaptureSelectScene_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 0;
    scenes.desc.pins = PIN_IN_MIC;
    ret = capture->scene.SelectScene(captureNull, &scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSelectScene API where the parameter scene is empty.
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_0004
* @tc.desc  Test AudioCaptureSelectScene, return -1 if the parameter scene is empty.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_AudioCaptureSelectScene_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor *scenes = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->scene.SelectScene(capture, scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureSelectScene API where the scene is not configured in the json.
* @tc.number  SUB_Audio_HDI_AudioCaptureSelectScene_0005
* @tc.desc  Test AudioCaptureSelectScene, return -1 if the scene is not configured in the json.
* @tc.author: ZHANGHAILIN
*/
HWTEST_F(AudioHdiCaptureSceneTest, SUB_Audio_HDI_AudioCaptureSelectScene_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioSceneDescriptor scenes = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_NAME_INTERNAL, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);

    scenes.scene.id = 5;
    scenes.desc.pins = PIN_OUT_HDMI;
    ret = capture->scene.SelectScene(capture, &scenes);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
}