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

#include "audio_capture_test.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_common_test.h"

using namespace std;
using namespace comfun;
using namespace testing::ext;
namespace {
class AudioCaptureTest : public testing::Test {
public:
    struct AudioManager *managerFuncs = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapter *adapter = nullptr;
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioSampleAttributes attrs = {};
    struct AudioCapture *capture = nullptr;

    virtual void SetUp();
    virtual void TearDown();
};

void AudioCaptureTest::SetUp()
{
    managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    ASSERT_EQ(HDF_SUCCESS,  managerFuncs->GetAllAdapters(managerFuncs, &descs, &size));

    desc = &descs[0];
    ASSERT_EQ(HDF_SUCCESS, managerFuncs->LoadAdapter(managerFuncs, desc, &adapter));
    ASSERT_EQ(HDF_SUCCESS, InitDevDescCapture(devDesc));
    ASSERT_EQ(HDF_SUCCESS, InitAttrsCapture(attrs));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioAdapterCreateCapture(adapter, &devDesc, &attrs, &capture));
}

void AudioCaptureTest::TearDown()
{
    ASSERT_EQ(HDF_SUCCESS, AudioAdapterDestroyCapture(adapter, capture));
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureStartWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureStart(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureStartWhenBufferIsNotNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_ERR_AI_BUSY, AudioCaptureStart(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureStartWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureStart(handle));
    hwCapture->devDataHandle = devDataHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureStartWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureStart(handle));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureStartWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
}


HWTEST_F(AudioCaptureTest, AudioCaptureStopWhenHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureStop(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureStopWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureStop(handle));
    hwCapture->devDataHandle = devDataHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureStopWhenBufferIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    hwCapture->captureParam.frameCaptureMode.buffer = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioCaptureStop(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureStopWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureStop(handle));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureStopWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStop(handle));
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCapturePause(handle));
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenBufferIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    hwCapture->captureParam.frameCaptureMode.buffer = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCapturePause(handle));
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenPauseIsTrue, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    hwCapture->captureParam.captureMode.ctlParam.pause = true;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_ERR_NOT_SUPPORT, AudioCapturePause(handle));
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCapturePause(handle));
    hwCapture->devDataHandle = devDataHandle;
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCapturePause(handle));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCapturePauseWhenParamVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCapturePause(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureResumeWhenHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureResume(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureResumeWhenPauseIsFalse, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    hwCapture->captureParam.captureMode.ctlParam.pause = false;
    handle = (AudioHandle)hwCapture;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioCaptureResume(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureResumeWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCapturePause(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureResume(handle));
    hwCapture->devDataHandle = devDataHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureResumeWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    AudioHandle handle = (AudioHandle)hwCapture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCapturePause(handle));
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureResume(handle));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureResumeWhenParamVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCapturePause(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureResume(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureFlushWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureFlush(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureFlushWhenParamIsVaild, TestSize.Level1)
{
    AudioHwCapture *hwCapture = new AudioHwCapture;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioCaptureFlush(handle));
    delete(hwCapture);
    hwCapture = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameSizeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    uint64_t size = FRAME_DATA;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetFrameSize(handle, &size));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameSizeWhenSizeIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    uint64_t *frameSize = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetFrameSize(handle, frameSize));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameSizeWhenParamIsNotSupport, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    hwCapture->captureParam.frameCaptureMode.attrs.format = AUDIO_FORMAT_AAC_MAIN;
    AudioHandle handle = (AudioHandle)hwCapture;
    uint64_t frameSize = FRAME_DATA;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioCaptureGetFrameSize(handle, &frameSize));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameSizeWhenParamVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    uint64_t frameSize = FRAME_DATA;
    EXPECT_EQ(HDF_SUCCESS, AudioCaptureGetFrameSize(handle, &frameSize));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameCountWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    uint64_t count = FRAME_DATA;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetFrameCount(handle, &count));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameCountWhenCountIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    uint64_t *count = nullptr;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetFrameCount(handle, count));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetFrameCountWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    uint64_t count = FRAME_DATA;
    EXPECT_EQ(HDF_SUCCESS, AudioCaptureGetFrameCount(handle, &count));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenHandleIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetSampleAttributes(nullptr, &attrs));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenAttrsIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetSampleAttributes(handle, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenDevDataHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetSampleAttributes(handle, &attrs));
    hwCapture->devDataHandle = devDataHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenFormatIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    enum AudioFormat format = attrs.format;
    attrs.format = AUDIO_FORMAT_G711A;
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, AudioCaptureSetSampleAttributes(handle, &attrs));
    attrs.format = format;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenDevDataHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetSampleAttributes(handle, &attrs));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetSampleAttributesWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureSetSampleAttributes(handle, &attrs));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetSampleAttributesWhenHandleIsNull, TestSize.Level1)
{
    AudioSampleAttributes *attrs = new AudioSampleAttributes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetSampleAttributes(nullptr, attrs));
    delete(attrs);
    attrs = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetSampleAttributesWhenAttrsIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetSampleAttributes(handle, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetSampleAttributesWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(HDF_SUCCESS, AudioCaptureGetSampleAttributes(handle, &attrs));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCurrentChannelIdWhenHandleIsNull, TestSize.Level1)
{
    uint32_t channelIdOne = 1;
    uint32_t *channelId = &channelIdOne;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetCurrentChannelId(nullptr, channelId));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCurrentChannelIdWhenChannelIdIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetCurrentChannelId(handle, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCurrentChannelIdWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    uint32_t channelId = 1;
    EXPECT_EQ(HDF_SUCCESS, AudioCaptureGetCurrentChannelId(handle, &channelId));
}

HWTEST_F(AudioCaptureTest, AudioCaptureCheckSceneCapabilityWhenHandleIsNull, TestSize.Level1)
{
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    bool supported = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCheckSceneCapability(nullptr, scene, &supported));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCheckSceneCapabilityWhenSceneIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    bool supported = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCheckSceneCapability(handle, nullptr, &supported));
}

HWTEST_F(AudioCaptureTest, AudioCaptureCheckSceneCapabilityWhenSupportedIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCheckSceneCapability(handle, scene, nullptr));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCheckSceneCapabilityWhenPinsIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_NONE;
    bool supported = false;
    /* to support different products */
    int32_t ret = AudioCaptureCheckSceneCapability(handle, &scene, &supported);
    if ((ret == AUDIO_HAL_ERR_NOT_SUPPORT) || (ret == AUDIO_HAL_ERR_INTERNAL)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioCaptureTest, AudioCaptureCheckSceneCapabilityWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_IN_MIC;
    bool supported = false;
    /* to support different products */
    int32_t ret = AudioCaptureCheckSceneCapability(handle, &scene, &supported);
    if ((ret == AUDIO_HAL_SUCCESS) || (ret == AUDIO_HAL_ERR_NOT_SUPPORT)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioCaptureTest, AudioCaptureSelectSceneWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    const struct AudioSceneDescriptor *scene = new struct AudioSceneDescriptor;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSelectScene(handle, scene));
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSelectSceneWhenSceneIsNull, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSelectScene(handle, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSelectSceneWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    struct AudioSceneDescriptor *scene = new AudioSceneDescriptor;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSelectScene(handle, scene));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(scene);
    scene = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSelectSceneWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_IN_MIC;
    /* to support different products */
    int32_t ret = AudioCaptureSelectScene(handle, &scene);
    if ((ret == AUDIO_HAL_ERR_NOT_SUPPORT) || (ret == AUDIO_HAL_ERR_INTERNAL)) {
        EXPECT_TRUE(true);
    }
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSelectSceneWhenParamVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    struct AudioSceneDescriptor scene;
    scene.scene.id = AUDIO_IN_MEDIA;
    scene.desc.pins = PIN_IN_MIC;
    /* to support different products */
    int32_t ret =  AudioCaptureSelectScene(handle, &scene);
    if ((ret == AUDIO_HAL_ERR_NOT_SUPPORT) || (ret == AUDIO_HAL_SUCCESS)) {
        EXPECT_TRUE(true);
    }
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetMuteWhenHandleIsNull, TestSize.Level1)
{
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetMute(nullptr, mute));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetMuteWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetMute(handle, mute));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetMuteWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetMute(handle, mute));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetMuteWhenParamVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    bool mute = true;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureSetMute(handle, mute));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetMuteWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetMute(handle, &mute));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetMuteWhenMuteIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetMute((AudioHandle)capture, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetMuteWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetMute(handle, &mute));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetMuteWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetMute(handle, &mute));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetMuteWhenParamVaild, TestSize.Level1)
{
    bool mute = false;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureGetMute((AudioHandle)capture, &mute));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenHandleIsNull, TestSize.Level1)
{
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetVolume(nullptr, volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetVolume(handle, volume));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenVolumeLessThanZero, TestSize.Level1)
{
    float volume = -1;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetVolume((AudioHandle)capture, volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax = 0;
    hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin = 1;
    AudioHandle handle = (AudioHandle)capture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetVolume(handle, volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetVolume(handle, volume));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetVolumeWhenParamVaild, TestSize.Level1)
{
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureSetVolume((AudioHandle)capture, volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenHandleIsNull, TestSize.Level1)
{
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetVolume(nullptr, &volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenVolumeIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetVolume((AudioHandle)capture, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetVolume(handle, &volume));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetVolume(handle, &volume));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenVolumeIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax = 1;
    hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin = 1;
    AudioHandle handle = (AudioHandle)capture;
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetVolume(handle, &volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetVolumeWhenParamVaild, TestSize.Level1)
{
    float volume = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureGetVolume((AudioHandle)capture, &volume));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenCaptureIsNull, TestSize.Level1)
{
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetGainThreshold(nullptr, &min, &max));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenMinIsNull, TestSize.Level1)
{
    float *min = nullptr;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetGainThreshold((AudioHandle)capture, min, &max));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenMaxIsNull, TestSize.Level1)
{
    float min = GAIN_MIN;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetGainThreshold((AudioHandle)capture, &min, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenDevCtlHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetGainThreshold(handle, &min, &max));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenDevCtlHandleIsError, TestSize.Level1)
{
    AudioHwCapture *hwCapture = (AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetGainThreshold(handle, &min, &max));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainThresholdWhenParamVaild, TestSize.Level1)
{
    float min = GAIN_MIN;
    float max = GAIN_MAX;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureGetGainThreshold((AudioHandle)capture, &min, &max));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainWhenHandleIsNull, TestSize.Level1)
{
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetGain(nullptr, &gain));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainWhenGainIsNull, TestSize.Level1)
{
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetGain((AudioHandle)capture, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetGain(handle, &gain));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureGetGain(handle, &gain));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetGainWhenParamVaild, TestSize.Level1)
{
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureGetGain((AudioHandle)capture, &gain));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetGainWhenHandleIsNull, TestSize.Level1)
{
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetGain(nullptr, gain));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetGainWhenGainIsLessThanZero, TestSize.Level1)
{
    float gain = -1;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureSetGain((AudioHandle)capture, gain));
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetGainWhenDevCtlHandleIsNull, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    hwCapture->devCtlHandle = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetGain(handle, gain));
    hwCapture->devCtlHandle = devCtlHandle;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetGainWhenDevCtlHandleIsError, TestSize.Level1)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devCtlHandle = hwCapture->devCtlHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devCtlHandle = (struct DevHandleCapture *)service;
    AudioHandle handle = (AudioHandle)hwCapture;
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureSetGain(handle, gain));
    hwCapture->devCtlHandle = devCtlHandle;
    delete(service);
    service = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureSetGainWhenParamVaild, TestSize.Level1)
{
    float gain = 0;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureSetGain((AudioHandle)capture, gain));
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenCaptureIsNull, TestSize.Level1)
{
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCaptureFrame(nullptr, frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenFrameIsNull, TestSize.Level1)
{
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCaptureFrame(capture, nullptr, requestBytes, &replyBytes));
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenReplyBytesIsNull, TestSize.Level1)
{
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureCaptureFrame(capture, frame, requestBytes, nullptr));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenDevDataHandleIsNull, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart((AudioHandle)capture));
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    hwCapture->devDataHandle = nullptr;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureCaptureFrame(capture, frame, requestBytes, &replyBytes));
    hwCapture->devDataHandle = devDataHandle;
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenDevDataHandleIsError, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart((AudioHandle)capture));
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    struct DevHandleCapture *devDataHandle = hwCapture->devDataHandle;
    struct HdfIoService *service = new HdfIoService;
    hwCapture->devDataHandle = (struct DevHandleCapture *)service;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureCaptureFrame(capture, frame, requestBytes, &replyBytes));
    hwCapture->devDataHandle = devDataHandle;
    delete(service);
    service = nullptr;
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenRequestBytesLessThanBufferSize, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart((AudioHandle)capture));
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA - 1;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL,  AudioCaptureCaptureFrame(capture, frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenSampleRateIsZero, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart((AudioHandle)capture));
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    hwCapture->captureParam.frameCaptureMode.attrs.sampleRate = 0;
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureCaptureFrame(capture, frame, requestBytes, &replyBytes));
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureCaptureFrameWhenParamIsVaild, TestSize.Level1)
{
    ASSERT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart((AudioHandle)capture));
    void *frame = (void *)calloc(1, FRAME_DATA);
    ASSERT_NE(nullptr, frame);
    uint64_t requestBytes = FRAME_DATA;
    uint64_t replyBytes;
    /* to support different products */
    int32_t ret = AudioCaptureCaptureFrame(capture, frame, requestBytes, &replyBytes);
    if ((ret == AUDIO_HAL_SUCCESS) || (ret == AUDIO_HAL_ERR_INTERNAL)) {
        EXPECT_TRUE(true);
    }
    free(frame);
    frame = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCapturePositionWhenCaptureIsNull, TestSize.Level1)
{
    uint64_t frames = 1024;
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetCapturePosition(nullptr, &frames, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCapturePositionWhenFramesIsNull, TestSize.Level1)
{
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetCapturePosition(capture, nullptr, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCapturePositionWhenTimeIsNull, TestSize.Level1)
{
    uint64_t frames = 1024;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureGetCapturePosition(capture, &frames, nullptr));
}

HWTEST_F(AudioCaptureTest, AudioCaptureGetCapturePositionWhenParamIsVaild, TestSize.Level1)
{
    uint64_t frames = 1024;
    struct AudioTimeStamp *time = new AudioTimeStamp;
    EXPECT_EQ(HDF_SUCCESS, AudioCaptureGetCapturePosition(capture, &frames, time));
    delete(time);
    time = nullptr;
}

HWTEST_F(AudioCaptureTest, AudioCaptureTurnStandbyModeWhenHandleIsNull, TestSize.Level1)
{
    AudioHwCapture *hwCapture = nullptr;
    AudioHandle handle = (AudioHandle)hwCapture;
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, AudioCaptureTurnStandbyMode(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureTurnStandbyModeWhenStopIsError, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_ERR_INTERNAL, AudioCaptureTurnStandbyMode(handle));
}

HWTEST_F(AudioCaptureTest, AudioCaptureTurnStandbyModeWhenParamIsVaild, TestSize.Level1)
{
    AudioHandle handle = (AudioHandle)capture;
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureStart(handle));
    EXPECT_EQ(AUDIO_HAL_SUCCESS, AudioCaptureTurnStandbyMode(handle));
}
}
