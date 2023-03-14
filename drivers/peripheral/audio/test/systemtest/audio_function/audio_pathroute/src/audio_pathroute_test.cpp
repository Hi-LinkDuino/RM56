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
 * @brief Test audio route path function
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"
#include "audio_pathroute_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_USB = "usb";
const string ADAPTER_INTERNAL = "internal";
const int REGISTER_STATUS_ON = 1;
const int REGISTER_STATUS_OFF = 0;
static struct AudioCtlElemValue g_elemValues[4] = {
    {
        .id.cardServiceName = "hdf_audio_codec_dev0",
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Dacl enable",
        .value[0] = 0,
    }, {
        .id.cardServiceName = "hdf_audio_codec_dev0",
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Dacr enable",
        .value[0] = 0,
    }, {
        .id.cardServiceName = "hdf_audio_codec_dev0",
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_PGA,
        .id.itemName = "LPGA MIC Switch",
        .value[0] = 0,
    }, {
        .id.cardServiceName = "hdf_audio_codec_dev0",
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_PGA,
        .id.itemName = "RPGA MIC Switch",
        .value[0] = 0,
    }
};
class AudioPathRouteTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static TestAudioManager *(*GetAudioManager)();
    static void *handleSo;
};

TestAudioManager *(*AudioPathRouteTest::GetAudioManager)() = nullptr;
void *AudioPathRouteTest::handleSo = nullptr;

void AudioPathRouteTest::SetUpTestCase(void)
{
    char resolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    handleSo = dlopen(resolvedPath, RTLD_LAZY);
    if (handleSo == nullptr) {
        return;
    }
    GetAudioManager = (TestAudioManager* (*)())(dlsym(handleSo, "GetAudioManagerFuncs"));
    if (GetAudioManager == nullptr) {
        return;
    }
}

void AudioPathRouteTest::TearDownTestCase(void)
{
    if (handleSo != nullptr) {
        dlclose(handleSo);
        handleSo = nullptr;
    }
    if (GetAudioManager != nullptr) {
        GetAudioManager = nullptr;
    }
}

void AudioPathRouteTest::SetUp(void) {}

void AudioPathRouteTest::TearDown(void) {}

/**
* @tc.name  Test the audio path route selection function of palyback scene
* @tc.number  SUB_Audio_AudioPathRoute_0001
* @tc.desc  The audio path route can be opened successfully,When it is set to
            palyback scene(attrs.type = AUDIO_IN_MEDIA,pins = PIN_OUT_SPEAKER)
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0001, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioRender *render = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ret = PowerOff(g_elemValues[0], g_elemValues[1]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, pins, ADAPTER_USB, &adapter, &render);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[0].id, g_elemValues[1].id, REGISTER_STATUS_ON, REGISTER_STATUS_ON);
    EXPECT_EQ(HDF_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function via switching device SPEAKER to HEADSET
* @tc.number  SUB_Audio_AudioPathRoute_0002
* @tc.desc  The audio path route can be opened successfully,When switching
            device(attrs.type = AUDIO_IN_MEDIA,pins = PIN_OUT_HEADSET)
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0002, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioRender *render = nullptr;
    struct AudioAdapter *adapter = nullptr;
    g_elemValues[0].value[0] = 1;
    g_elemValues[1].value[0] = 1;
    ret = PowerOff(g_elemValues[0], g_elemValues[1]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, pins, ADAPTER_USB, &adapter, &render);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_OUT_HEADSET,
    };
    ret = render->scene.SelectScene(AudioHandle(render), &scene);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[0].id, g_elemValues[1].id, REGISTER_STATUS_OFF, REGISTER_STATUS_OFF);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function of playback sence
            when the audio path route has been opened
* @tc.number  SUB_Audio_AudioPathRoute_0003
* @tc.desc  The audio path route of playback scene can be opened successfully,When The current
            audio path route has been opened
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioRender *render = nullptr;
    struct AudioAdapter *adapter = nullptr;
    ret = PowerOff(g_elemValues[0], g_elemValues[1]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_USB, &adapter, &render);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_OUT_SPEAKER,
    };
    ret = render->scene.SelectScene(AudioHandle(render), &scene);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[0].id, g_elemValues[1].id, REGISTER_STATUS_ON, REGISTER_STATUS_ON);
    EXPECT_EQ(HDF_SUCCESS, ret);
    adapter->DestroyRender(adapter, render);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function of recording scene
* @tc.number  SUB_Audio_AudioPathRoute_0004
* @tc.desc  The audio path route can be opened successfully,When it is set to
            recording scene(attrs.type = AUDIO_IN_MEDIA,pins = PIN_IN_MIC)
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0004, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    g_elemValues[3].value[0] = 1;
    ret = PowerOff(g_elemValues[2], g_elemValues[3]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, pins, ADAPTER_INTERNAL, &adapter, &capture);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[2].id, g_elemValues[3].id, REGISTER_STATUS_ON, REGISTER_STATUS_OFF);
    EXPECT_EQ(HDF_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function via switching device MIC to HS_MIC
* @tc.number  SUB_Audio_AudioPathRoute_0005
* @tc.desc  The audio path route can be opened successfully,When it is set to
            recording scene(attrs.type = AUDIO_IN_MEDIA,pins = PIN_IN_HS_MIC)
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0005, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    g_elemValues[2].value[0] = 1;
    g_elemValues[3].value[0] = 1;
    ret = PowerOff(g_elemValues[2], g_elemValues[3]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, pins, ADAPTER_INTERNAL, &adapter, &capture);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_IN_HS_MIC,
    };
    ret = capture->scene.SelectScene(AudioHandle(capture), &scene);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[2].id, g_elemValues[3].id, REGISTER_STATUS_OFF, REGISTER_STATUS_OFF);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function of recording sence
            when the audio path route has been opened
* @tc.number  SUB_Audio_AudioPathRoute_0006
* @tc.desc  The audio path route of recording scene can be opened successfully,When The current
            audio path route has been opened
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    g_elemValues[3].value[0] = 1;
    ret = PowerOff(g_elemValues[2], g_elemValues[3]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_INTERNAL, &adapter, &capture);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_IN_MIC,
    };
    ret = capture->scene.SelectScene(AudioHandle(capture), &scene);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[2].id, g_elemValues[3].id, REGISTER_STATUS_ON, REGISTER_STATUS_OFF);
    EXPECT_EQ(HDF_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test the audio path route selection function via running multi service scenarios
* @tc.number  SUB_Audio_AudioPathRoute_0007
* @tc.desc  The audio path route can be opened successfully,When running multi service scenarios
* @tc.author: liweiming
*/
HWTEST_F(AudioPathRouteTest, SUB_Audio_AudioPathRoute_0007, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioAdapter *captureAdapter = nullptr;
    struct AudioAdapter *renderAdapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioRender *render = nullptr;
    ret = PowerOff(g_elemValues[0], g_elemValues[1]);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = PowerOff(g_elemValues[2], g_elemValues[3]);
    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, PIN_IN_MIC, ADAPTER_INTERNAL, &captureAdapter, &capture);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = CheckRegisterStatus(g_elemValues[2].id, g_elemValues[3].id, REGISTER_STATUS_ON, REGISTER_STATUS_OFF);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = AudioCreateRender(manager, PIN_OUT_SPEAKER, ADAPTER_USB, &renderAdapter, &render);
    if (ret < 0) {
        captureAdapter->DestroyCapture(captureAdapter, capture);
        manager->UnloadAdapter(manager, captureAdapter);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = CheckRegisterStatus(g_elemValues[0].id, g_elemValues[1].id, REGISTER_STATUS_ON, REGISTER_STATUS_ON);
    EXPECT_EQ(HDF_SUCCESS, ret);
    captureAdapter->DestroyCapture(captureAdapter, capture);
    manager->UnloadAdapter(manager, captureAdapter);
    renderAdapter->DestroyRender(renderAdapter, render);
    manager->UnloadAdapter(manager, renderAdapter);
}
}
