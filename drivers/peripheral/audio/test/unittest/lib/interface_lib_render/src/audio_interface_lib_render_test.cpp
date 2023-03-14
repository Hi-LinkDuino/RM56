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

#include "audio_interface_lib_render_test.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_interface_lib_render.h"

using namespace std;
using namespace testing::ext;
namespace {
extern "C" {
struct HdfSBuf *AudioObtainHdfSBuf();
int32_t IoctlWrite(const struct AudioHwRenderParam *handleData);
int32_t AudioServiceRenderDispatch(struct HdfIoService *service, int cmdId,
    struct HdfSBuf *sBuf, struct HdfSBuf *reply);
int32_t SetHwParams(const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetVolumeSBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetVolumeSBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetVolume(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetVolume(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderSetSpeed(struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetPauseBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetPauseStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetMuteBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetMuteStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetMuteSBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetMuteStu(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetGainBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetGainStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetGainSBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetGainStu(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSceneSelectSBuf(struct HdfSBuf *sBuf,
    struct AudioHwRenderParam *handleData, int32_t deviceIndex);
int32_t AudioCtlRenderSceneSelect(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSceneGetGainThresholdSBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSceneGetGainThreshold(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetChannelModeBuf(struct HdfSBuf *sBuf, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetChannelMode(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetChannelModeSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetChannelMode(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioInterfaceLibCtlRender(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t ParamsSbufWriteBuffer(struct HdfSBuf *sBuf);
int32_t FrameSbufWriteBuffer(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderHwParams(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderWrite(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderStop(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioInterfaceLibOutputRender(const struct DevHandle *handle, int cmdId,
    struct AudioHwRenderParam *handleData);
struct HdfIoService *HdfIoServiceBindName(const char *serviceName);
}

class AudioInterfaceLibRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void AudioInterfaceLibRenderTest::SetUpTestCase()
{
}

void AudioInterfaceLibRenderTest::TearDownTestCase()
{
}

HWTEST_F(AudioInterfaceLibRenderTest, SetHwParamsWhenHandleIsNull, TestSize.Level1)
{
    const struct AudioHwRenderParam *handleData = nullptr;
    int32_t ret = SetHwParams(handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
}

HWTEST_F(AudioInterfaceLibRenderTest, SetHwParamsWhenHandleIsVaild, TestSize.Level1)
{
    struct AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = SetHwParams((const struct AudioHwRenderParam *)handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetVolumeSBufWhenSbuffIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetVolumeSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetVolumeSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetVolumeSBufWhenSbuffIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderGetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetVolumeSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderGetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetVolumeSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderGetVolumeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetPauseBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSetPauseBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetPauseBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSetPauseBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetPauseBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSetPauseBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetMuteBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSetMuteBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetMuteBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSetMuteBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetMuteBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSetMuteBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetMuteSBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderGetMuteSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetMuteSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderGetMuteSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetMuteSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderGetMuteSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetGainBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSetGainBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetGainBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSetGainBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetGainBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSetGainBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetGainSBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderGetGainSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetGainSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderGetGainSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetGainSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderGetGainSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneSelectSBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t deviceIndex = 0;
    int32_t ret = AudioCtlRenderSceneSelectSBuf(sBuf, handleData, deviceIndex);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneSelectSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t deviceIndex = 0;
    int32_t ret = AudioCtlRenderSceneSelectSBuf(sBuf, handleData, deviceIndex);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneSelectSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t deviceIndex = 0;
    int32_t ret = AudioCtlRenderSceneSelectSBuf(sBuf, handleData, deviceIndex);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneGetGainThresholdSBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSceneGetGainThresholdSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneGetGainThresholdSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSceneGetGainThresholdSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSceneGetGainThresholdSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSceneGetGainThresholdSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetChannelModeBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderSetChannelModeBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetChannelModeBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderSetChannelModeBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderSetChannelModeBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderSetChannelModeBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetChannelModeSBufWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    int32_t ret = AudioCtlRenderGetChannelModeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetChannelModeSBufWhenHandleDataIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = nullptr;
    int32_t ret = AudioCtlRenderGetChannelModeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
}

HWTEST_F(AudioInterfaceLibRenderTest, AudioCtlRenderGetChannelModeSBufWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    AudioHwRenderParam *handleData = new AudioHwRenderParam;
    handleData->renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    int32_t ret = AudioCtlRenderGetChannelModeSBuf(sBuf, handleData);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
    delete(handleData);
    handleData = nullptr;
}

HWTEST_F(AudioInterfaceLibRenderTest, ParamsSbufWriteBufferWhenSbufIsNull, TestSize.Level1)
{
    struct HdfSBuf *sBuf = nullptr;
    int32_t ret = ParamsSbufWriteBuffer(sBuf);
    EXPECT_EQ(HDF_FAILURE, ret);
}

HWTEST_F(AudioInterfaceLibRenderTest, ParamsSbufWriteBufferWhenParamIsVaild, TestSize.Level1)
{
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    int32_t ret = ParamsSbufWriteBuffer(sBuf);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBuf);
}
}
