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
#include "audio_hdicapture_control_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string ADAPTER_NAME_HDMI = "hdmi";
const string ADAPTER_NAME_USB = "usb";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioHdiCaptureControlTest : public testing::Test {
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

TestAudioManager *(*AudioHdiCaptureControlTest::GetAudioManager)() = nullptr;
void *AudioHdiCaptureControlTest::handleSo = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioHdiCaptureControlTest::SdkInit)() = nullptr;
    void (*AudioHdiCaptureControlTest::SdkExit)() = nullptr;
    void *AudioHdiCaptureControlTest::sdkSo = nullptr;
#endif

void AudioHdiCaptureControlTest::SetUpTestCase(void)
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

void AudioHdiCaptureControlTest::TearDownTestCase(void)
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

void AudioHdiCaptureControlTest::SetUp(void) {}

void AudioHdiCaptureControlTest::TearDown(void) {}

/**
* @tc.name  Test AudioCreateCapture API via legal input
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0001
* @tc.desc  Test AudioCreateCapture interface,Returns 0 if the AudioCapture object is created successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0001, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via creating a capture object when a render object was created
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0002
* @tc.desc  test AudioCreateCapture interface:
     (1)service mode:Returns 0,if the AudioCapture object can be created successfully which was created
     (2)passthrough mode: Returns -1,if the AudioCapture object can't be created which was created
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0002, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortDirection portType = PORT_OUT_IN;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *firstCapture = nullptr;
    struct AudioCapture *secondCapture = nullptr;
    struct AudioPort* audioPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor DevDesc = {};

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    InitDevDesc(DevDesc, audioPort->portId, PIN_IN_MIC);
    ret = adapter->CreateCapture(adapter, &DevDesc, &attrs, &firstCapture);
    if (ret < 0) {
        manager->UnloadAdapter(manager, adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = adapter->CreateCapture(adapter, &DevDesc, &attrs, &secondCapture);
#if defined (AUDIO_ADM_SERVICE) || defined (AUDIO_MPI_SERVICE)
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, secondCapture);
#endif
#if defined (AUDIO_ADM_SO) || defined (AUDIO_MPI_SO) || defined (__LITEOS__)
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    adapter->DestroyCapture(adapter, firstCapture);
#endif
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via creating a capture object when a render object was created
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0003
* @tc.desc  test AudioCreateCapture interface,Returns 0 if the AudioCapture object can be created successfully
            when AudioRender was created
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0003, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortDirection portType = PORT_OUT_IN;
    struct AudioAdapter *adapter = nullptr;
    struct AudioRender *render = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioPort* audioPort = nullptr;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor renderDevDesc = {};
    struct AudioDeviceDescriptor captureDevDesc = {};

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, audioPort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    InitAttrs(attrs);
    InitDevDesc(renderDevDesc, audioPort->portId, PIN_OUT_SPEAKER);
    InitDevDesc(captureDevDesc, audioPort->portId, PIN_IN_MIC);
    ret = adapter->CreateRender(adapter, &renderDevDesc, &attrs, &render);
    if (ret < 0) {
        manager->UnloadAdapter(manager, adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    ret = adapter->CreateCapture(adapter, &captureDevDesc, &attrs, &capture);
    if (ret < 0) {
        adapter->DestroyRender(adapter, render);
        manager->UnloadAdapter(manager, adapter);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    adapter->DestroyRender(adapter, render);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via creating two capture objects
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0004
* @tc.desc  Test AudioCreateCapture interface,return 0 if the the two audiocapture objects are created successfully
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0004, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapterFirst = nullptr;
    struct AudioAdapter *adapterSecond = nullptr;
    struct AudioCapture *captureFirst = nullptr;
    struct AudioCapture *captureSecond = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapterFirst, &captureFirst);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_INTERNAL, &adapterSecond, &captureSecond);
    if (ret < 0) {
        adapterFirst->DestroyCapture(adapterFirst, captureFirst);
        manager->UnloadAdapter(manager, adapterFirst);
        ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    }
    adapterFirst->DestroyCapture(adapterFirst, captureFirst);
    adapterSecond->DestroyCapture(adapterSecond, captureSecond);
    manager->UnloadAdapter(manager, adapterFirst);
    manager->UnloadAdapter(manager, adapterSecond);
}
/**
* @tc.name  Test AudioCreateCapture API via setting the incoming parameter adapter is nullptr
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0005
* @tc.desc  Test AudioCreateCapture interface,Returns -1 if the incoming parameter adapter is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioSampleAttributes attrs = {};
    enum AudioPortDirection portType = PORT_IN;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    struct AudioCapture *capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitAttrs(attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitDevDesc(devDesc, capturePort->portId, pins);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->CreateCapture(adapterNull, &devDesc, &attrs, &capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via setting the incoming parameter desc is nullptr
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0006
* @tc.desc  Test AudioCreateCapture interface,Returns -1 if the incoming parameter desc is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    struct AudioSampleAttributes attrs = {};
    enum AudioPortDirection portType = PORT_IN;
    struct AudioDeviceDescriptor *devDesc = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitAttrs(attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->CreateCapture(adapter, devDesc, &attrs, &capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via setting the incoming parameter attrs is nullptr
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0007
* @tc.desc  Test AudioCreateCapture interface,Returns -1 if the incoming parameter attrs is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0007, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    enum AudioPortDirection portType = PORT_IN;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioSampleAttributes *attrs = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitDevDesc(devDesc, capturePort->portId, pins);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->CreateCapture(adapter, &devDesc, attrs, &capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via setting the incoming parameter capture is nullptr
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0008
* @tc.desc  Test AudioCreateCapture interface,Returns -1 if the incoming parameter capture is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0008, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioSampleAttributes attrs = {};
    enum AudioPortDirection portType = PORT_IN;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture **capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitAttrs(attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitDevDesc(devDesc, capturePort->portId, pins);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->CreateCapture(adapter, &devDesc, &attrs, capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCreateCapture API via setting the incoming parameter adapter which port type is PORT_OUT
* @tc.number  SUB_Audio_HDI_AudioCreateCapture_0008
* @tc.desc  Test AudioCreateCapture interface,Returns -1 if the incoming parameter adapter which port type is PORT_OUT
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioCreateCapture_0009, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioSampleAttributes attrs = {};
    enum AudioPortDirection portType = PORT_OUT;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_HDMI, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitAttrs(attrs);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = InitDevDesc(devDesc, capturePort->portId, pins);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->CreateCapture(adapter, &devDesc, &attrs, &capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioDestroyCapture API via legal input
* @tc.number  SUB_Audio_HDI_AudioDestroyCapture_0001
* @tc.desc  Test AudioDestroyCapture interface,Returns 0 if the AudioCapture object is destroyed
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioDestroyCapture_0001, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->DestroyCapture(adapter, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioDestroyCapture API via setting the incoming parameter adapter is nullptr
* @tc.number  SUB_Audio_HDI_AudioDestroyCapture_0002
* @tc.desc  Test AudioDestroyCapture interface,Returns -1 if the incoming parameter adapter is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioDestroyCapture_0002, TestSize.Level1)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioAdapter *adapterNull = nullptr;
    struct AudioCapture *capture = nullptr;

    TestAudioManager* manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->DestroyCapture(adapterNull, capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    ret = adapter->DestroyCapture(adapter, capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioDestroyCapture API via setting the incoming parameter capture is nullptr
* @tc.number  SUB_Audio_HDI_AudioDestroyCapture_0003
* @tc.desc  Test AudioDestroyCapture interface,Returns -1 if the incoming parameter capture is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_AudioDestroyCapture_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioPort* capturePort = nullptr;
    enum AudioPortDirection portType = PORT_IN;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    ASSERT_NE(nullptr, GetAudioManager);
    TestAudioManager* manager = GetAudioManager();
    ret = GetLoadAdapter(manager, portType, ADAPTER_NAME_USB, &adapter, capturePort);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = adapter->DestroyCapture(adapter, capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureStart API via legal input
* @tc.number  SUB_Audio_HDI_StartCapture_0001
* @tc.desc  Test AudioCaptureStart interface,return 0 if the audiocapture object is started successfully
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStart_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test CaptureStart API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_CaptureStart_0002
* @tc.desc  Test CaptureStart interface,return -1 if the incoming parameter handle is nullptr
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStart_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)captureNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test AudioCaptureStart API via start two capture object continuously
* @tc.number  SUB_Audio_HDI_CaptureStart_0003
* @tc.desc  Test AudioCaptureStart interface,return 0 if the Audiocapturestart was successfully called twice
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStart_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_AI_BUSY, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCaptureStop API via legal input
    * @tc.number  SUB_Audio_HDI_CaptureStop_0001
    * @tc.desc  Test AudioCaptureStop interface,return 0 if the audiocapture object is stopped successfully
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStop_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCaptureStop API via stop two capture object continuously
    * @tc.number  SUB_Audio_HDI_CaptureStop_0002
    * @tc.desc  Test AudioCaptureStop interface,return -4 if Audiocapturestop was successfully called twice
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStop_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCaptureStop API via start an audio capture after stopping
    * @tc.number  SUB_Audio_HDI_CaptureStop_0003
    * @tc.desc  Test AudioCaptureStop interface,return 0 if stop and start an audio capture successfully
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStop_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test AudioCaptureStop API via the capture does not start and stop only
    * @tc.number  SUB_Audio_HDI_CaptureStop_0004
    * @tc.desc  Test AudioCaptureStop interface,return -4 if the capture does not start and stop only
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStop_0004, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name Test CaptureStop API via setting the incoming parameter handle is nullptr
* @tc.number  SUB_Audio_HDI_CaptureStop_0005
* @tc.desc  Test CaptureStop interface, return -1 if the incoming parameter handle is nullptr
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureStop_0005, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)captureNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CapturePause API via legal input
    * @tc.number  SUB_Audio_HDI_CapturePause_0001
    * @tc.desc  test HDI CapturePause interface，return 0 if the capture is paused after start
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CapturePause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CapturePause API via the interface is called twice in a row
    * @tc.number  SUB_Audio_HDI_CapturePause_0002
    * @tc.desc  Test CapturePause interface, return -1 the second time if CapturePause is called twice
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CapturePause_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CapturePause API via setting the incoming parameter handle is nullptr
    * @tc.number  SUB_Audio_HDI_CapturePause_0003
    * @tc.desc  Test CapturePause interface,return -1 if the incoming parameter handle is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CapturePause_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)captureNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CapturePause API via the capture is not Started and paused only.
    * @tc.number  SUB_Audio_HDI_CapturePause_0004
    * @tc.desc  Test AudioRenderPause interface,return -1 if the capture is not Started and paused only.
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CapturePause_0004, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateCapture(manager, pins, ADAPTER_NAME_USB, &adapter, &capture);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CapturePause API via the capture is paused after stopped.
    * @tc.number  SUB_Audio_HDI_CapturePause_0005
    * @tc.desc  Test CapturePause interface, return -1 the capture is paused after stopped.
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CapturePause_0005, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);

    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CaptureResume API via legal input
    * @tc.number  SUB_Audio_HDI_CaptureResume_0001
    * @tc.desc  Test CaptureResume interface,return 0 if the capture is resumed after paused
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CaptureResume API via the interface is called twice in a row
    * @tc.number  SUB_Audio_HDI_CaptureResume_0002
    * @tc.desc  Test CaptureResume interface,return -1 the second time if the CaptureResume is called twice
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}

/**
    * @tc.name  Test CaptureResume API via the capture is resumed after started
    * @tc.number  SUB_Audio_HDI_CaptureResume_0003
    * @tc.desc  test HDI CaptureResume interface,return -1 if the capture is resumed after started
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0003, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CaptureResume API via setting the incoming parameter handle is nullptr
    * @tc.number  SUB_Audio_HDI_CaptureResume_0004
    * @tc.desc  Test CaptureResume interface, return -1 if the incoming parameter handle is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0004, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)captureNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test CaptureResume API via the capture is resumed after stopped
* @tc.number  SUB_Audio_HDI_CaptureResume_0005
* @tc.desc  test HDI CaptureResume interface,return -1 if the capture is resumed after stopped
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0005, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test CaptureResume API via the capture Continue to start after resume
* @tc.number  SUB_Audio_HDI_CaptureResume_0006
* @tc.desc  test HDI CaptureResume interface,return -1 if the capture Continue to start after resume
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0006, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Pause((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Resume((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Start((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_ERR_AI_BUSY, ret);

    capture->control.Stop((AudioHandle)capture);
    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
* @tc.name  Test RenderResume API via the different capture objects is started、paused、resumed and stopped.
* @tc.number  SUB_Audio_HDI_CaptureResume_0007
* @tc.desc  test HDI CaptureResume interface,return 0 if the different objects is started、paused、resumed and stopped.
* @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureResume_0007, TestSize.Level1)
{
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapterOne = nullptr;
    struct AudioAdapter *adapterSec = nullptr;
    struct AudioCapture *captureOne = nullptr;
    struct AudioCapture *captureSec = nullptr;
    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret1 = AudioCreateStartCapture(manager, &captureOne, &adapterOne, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret1);
    ret1 = captureOne->control.Pause((AudioHandle)captureOne);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret1);
    ret1 = captureOne->control.Resume((AudioHandle)captureOne);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret1);
    ret1 = captureOne->control.Stop((AudioHandle)captureOne);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret1);
    adapterOne->DestroyCapture(adapterOne, captureOne);
    manager->UnloadAdapter(manager, adapterOne);
    ret2 = AudioCreateStartCapture(manager, &captureSec, &adapterSec, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret2);
    ret2 = captureSec->control.Pause((AudioHandle)captureSec);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret2);
    ret2 = captureSec->control.Resume((AudioHandle)captureSec);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret2);
    ret2 = captureSec->control.Stop((AudioHandle)captureSec);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret2);
    adapterSec->DestroyCapture(adapterSec, captureSec);
    manager->UnloadAdapter(manager, adapterSec);
}
/**
    * @tc.name  Test CaptureFlush API via legal input Verify that the data in the buffer is flushed after stop
    * @tc.number  SUB_Audio_HDI_CaptureFlush_0001
    * @tc.desc  Test CaptureFlush interface,return -2 if the data in the buffer is flushed successfully after stop
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureFlush_0001, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Flush((AudioHandle)capture);
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
/**
    * @tc.name  Test CaptureFlush that the data in the buffer is flushed when handle is nullptr
    * @tc.number  SUB_Audio_HDI_CaptureFlush_0002
    * @tc.desc  Test CaptureFlush, return -1 if the data in the buffer is flushed when handle is nullptr
    * @tc.author: tiansuli
*/
HWTEST_F(AudioHdiCaptureControlTest, SUB_Audio_HDI_CaptureFlush_0002, TestSize.Level1)
{
    int32_t ret = -1;
    TestAudioManager* manager = {};
    struct AudioAdapter *adapter = nullptr;
    struct AudioCapture *capture = nullptr;
    struct AudioCapture *captureNull = nullptr;

    manager = GetAudioManager();
    ASSERT_NE(nullptr, GetAudioManager);
    ret = AudioCreateStartCapture(manager, &capture, &adapter, ADAPTER_NAME_USB);
    ASSERT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Stop((AudioHandle)capture);
    EXPECT_EQ(AUDIO_HAL_SUCCESS, ret);
    ret = capture->control.Flush((AudioHandle)captureNull);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);

    adapter->DestroyCapture(adapter, capture);
    manager->UnloadAdapter(manager, adapter);
}
}
