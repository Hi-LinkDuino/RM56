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
 * @brief Test audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver ADM interface lib,and rendering audios
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_lib_common.h
 *
 * @brief Declares APIs for operations related to the audio ADM interface lib.
 *
 * @since 1.0
 * @version 1.0
 */
#include "audio_lib_common.h"
#include "audio_librender_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string BIND_CONTROL = "control";
const string BIND_RENDER = "render";
const string BIND_NAME_ERROR = "rendor";
const string ADAPTER_NAME_USB = "usb";

class AudioLibRenderTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static struct DevHandle *(*BindServiceRenderSo)(const char *);
    static int32_t (*InterfaceLibOutputRender)(struct DevHandle *, int, struct AudioHwRenderParam *);
    static int32_t (*InterfaceLibCtlRender)(struct DevHandle *, int, struct AudioHwRenderParam *);
    static void (*CloseServiceRenderSo)(struct DevHandle *);
    static void *PtrHandle;
#ifdef AUDIO_MPI_SO
    static int32_t (*SdkInit)();
    static void (*SdkExit)();
#endif
    uint32_t PcmBytesToFrames(const struct AudioFrameRenderMode &frameRenderMode, uint64_t bytes) const;
    int32_t FrameLibStart(FILE *file, struct AudioSampleAttributes attrs,
        struct AudioHeadInfo wavHeadInfo, struct AudioHwRender *hwRender) const;
    int32_t LibStartAndStream(const std::string path, struct AudioSampleAttributes attrs,
        struct DevHandle *handle, struct AudioHwRender *hwRender, struct AudioHeadInfo wavHeadInfo) const;
    int32_t LibHwOutputRender(struct AudioHwRender *hwRender, struct DevHandle *handlerender) const;
    int32_t BindServiceAndHwRender(struct AudioHwRender *&hwRender,
        const std::string BindName, const std::string adapterNameCase, struct DevHandle *&handle) const;
};

struct DevHandle *(*AudioLibRenderTest::BindServiceRenderSo)(const char *) = nullptr;
int32_t (*AudioLibRenderTest::InterfaceLibOutputRender)(struct DevHandle *, int, struct AudioHwRenderParam *) = nullptr;
int32_t (*AudioLibRenderTest::InterfaceLibCtlRender)(struct DevHandle *, int, struct AudioHwRenderParam *) = nullptr;
void (*AudioLibRenderTest::CloseServiceRenderSo)(struct DevHandle *) = nullptr;
void *AudioLibRenderTest::PtrHandle = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioLibRenderTest::SdkInit)() = nullptr;
    void (*AudioLibRenderTest::SdkExit)() = nullptr;
#endif

void AudioLibRenderTest::SetUpTestCase(void)
{
    char resolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_render");
    PtrHandle = dlopen(resolvedPath, RTLD_LAZY);
    if (PtrHandle == nullptr) {
        return;
    }
    BindServiceRenderSo = (struct DevHandle* (*)(const char *))dlsym(PtrHandle, "AudioBindServiceRender");
    InterfaceLibOutputRender = (int32_t (*)(struct DevHandle *, int,
        struct AudioHwRenderParam *))dlsym(PtrHandle, "AudioInterfaceLibOutputRender");
    InterfaceLibCtlRender = (int32_t (*)(struct DevHandle *, int,
        struct AudioHwRenderParam *))dlsym(PtrHandle, "AudioInterfaceLibCtlRender");
    CloseServiceRenderSo = (void (*)(struct DevHandle *))dlsym(PtrHandle, "AudioCloseServiceRender");
    if (BindServiceRenderSo == nullptr || CloseServiceRenderSo == nullptr ||
        InterfaceLibCtlRender == nullptr || InterfaceLibOutputRender == nullptr) {
        dlclose(PtrHandle);
        return;
    }
#ifdef AUDIO_MPI_SO
    SdkInit = (int32_t (*)())(dlsym(PtrHandle, "MpiSdkInit"));
    if (SdkInit == nullptr) {
        return;
    }
    SdkExit = (void (*)())(dlsym(PtrHandle, "MpiSdkExit"));
    if (SdkExit == nullptr) {
        return;
    }
    SdkInit();
#endif
}

void AudioLibRenderTest::TearDownTestCase(void)
{
    if (BindServiceRenderSo != nullptr) {
        BindServiceRenderSo = nullptr;
    }
    if (CloseServiceRenderSo != nullptr) {
        CloseServiceRenderSo = nullptr;
    }
    if (InterfaceLibOutputRender != nullptr) {
        InterfaceLibOutputRender = nullptr;
    }
    if (InterfaceLibCtlRender != nullptr) {
        InterfaceLibCtlRender = nullptr;
    }
#ifdef AUDIO_MPI_SO
    SdkExit();
    if (SdkInit != nullptr) {
        SdkInit = nullptr;
    }
    if (SdkExit != nullptr) {
        SdkExit = nullptr;
    }
#endif
    if (PtrHandle != nullptr) {
        dlclose(PtrHandle);
        PtrHandle = nullptr;
    }
}

void AudioLibRenderTest::SetUp(void) {}

void AudioLibRenderTest::TearDown(void) {}

uint32_t AudioLibRenderTest::PcmBytesToFrames(const struct AudioFrameRenderMode &frameRenderMode, uint64_t bytes) const
{
    uint32_t replyByte = static_cast<uint32_t>(bytes);
    return replyByte / (frameRenderMode.attrs.channelCount * (PcmFormatToBits(frameRenderMode.attrs.format) >>
                        MOVE_RIGHT_NUM));
}

/**
 * @brief Reading audio file frame.
 *
 * @param file audio file path
 * @param AudioSampleAttributes
 * @param struct AudioHeadInfo wavHeadInfo
 * @param struct AudioHwRender *hwRender
 *
 * @return Returns <b>0</b> if the initialization is successful; returns a negative value otherwise.
 */
int32_t AudioLibRenderTest::FrameLibStart(FILE *file, struct AudioSampleAttributes attrs,
    struct AudioHeadInfo wavHeadInfo, struct AudioHwRender *hwRender) const
{
    if (hwRender == nullptr) {
        return HDF_FAILURE;
    }
    size_t numRead = 0;
    uint32_t remainingDataSize = wavHeadInfo.dataSize;
    uint32_t bufferSize = PcmFramesToBytes(attrs);
    if (bufferSize <= 0) {
        return HDF_FAILURE;
    }
    hwRender->renderParam.frameRenderMode.buffer = (char *)calloc(1, bufferSize);
    if (hwRender->renderParam.frameRenderMode.buffer == nullptr) {
        return HDF_FAILURE;
    }

    uint32_t readSize = (remainingDataSize > bufferSize) ? bufferSize : remainingDataSize;
    numRead = fread(hwRender->renderParam.frameRenderMode.buffer, readSize, 1, file);
    if (numRead < 1) {
        free(hwRender->renderParam.frameRenderMode.buffer);
        hwRender->renderParam.frameRenderMode.buffer = nullptr;
        return HDF_FAILURE;
    }
    hwRender->renderParam.frameRenderMode.bufferSize = readSize;
    hwRender->renderParam.frameRenderMode.bufferFrameSize =
        PcmBytesToFrames(hwRender->renderParam.frameRenderMode, readSize);
    return HDF_SUCCESS;
}

int32_t AudioLibRenderTest::LibStartAndStream(const std::string path, struct AudioSampleAttributes attrs,
    struct DevHandle *handle, struct AudioHwRender *hwRender, struct AudioHeadInfo wavHeadInfo) const
{
    int ret = -1;
    if (handle == nullptr || hwRender == nullptr) {
        return HDF_FAILURE;
    }
    if (InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_PREPARE, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTRL_START, &hwRender->renderParam)) {
        return HDF_FAILURE;
    }
    char absPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), absPath) == nullptr) {
        return HDF_FAILURE;
    }
    FILE *file = fopen(absPath, "rb");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    if (WavHeadAnalysis(wavHeadInfo, file, attrs)) {
        fclose(file);
        return HDF_FAILURE;
    }
    ret = FrameLibStart(file, attrs, wavHeadInfo, hwRender);
    if (ret < 0) {
        fclose(file);
        return HDF_FAILURE;
    }
    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_WRITE, &hwRender->renderParam);
    if (ret < 0) {
        fclose(file);
        free(hwRender->renderParam.frameRenderMode.buffer);
        hwRender->renderParam.frameRenderMode.buffer = nullptr;
        return HDF_FAILURE;
    }
    fclose(file);
    free(hwRender->renderParam.frameRenderMode.buffer);
    hwRender->renderParam.frameRenderMode.buffer = nullptr;
    return HDF_SUCCESS;
}

int32_t AudioLibRenderTest::LibHwOutputRender(struct AudioHwRender *hwRender, struct DevHandle *handlerender) const
{
    if (hwRender == nullptr || handlerender == nullptr) {
        return HDF_FAILURE;
    }
    if (InterfaceLibOutputRender(handlerender, AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handlerender, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handlerender, AUDIO_DRV_PCM_IOCTL_PREPARE, &hwRender->renderParam) ||
        InterfaceLibOutputRender(handlerender, AUDIO_DRV_PCM_IOCTRL_START, &hwRender->renderParam)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioLibRenderTest::BindServiceAndHwRender(struct AudioHwRender *&hwRender,
    const std::string BindName, const std::string adapterNameCase, struct DevHandle *&handle) const
{
    handle = BindServiceRenderSo(BindName.c_str());
    if (handle == nullptr) {
        return HDF_FAILURE;
    }
    hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    if (hwRender == nullptr) {
        CloseServiceRenderSo(handle);
        return HDF_FAILURE;
    }
    if (InitHwRender(hwRender, adapterNameCase)) {
        CloseServiceRenderSo(handle);
        free(hwRender);
        hwRender = nullptr;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
/**
* @tc.name  test BindServiceRenderSo API via legal input.
* @tc.number  SUB_Audio_InterfaceLib_BindServiceRender_0001
* @tc.desc  test Binding succeeded Service which service Name is control_service and close Service.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_BindServiceRender_0001, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    handle = BindServiceRenderSo(BIND_CONTROL.c_str());
    ASSERT_NE(nullptr, handle);
    CloseServiceRenderSo(handle);
}
/**
* @tc.name  test BindServiceRenderSo API via invalid input.
* @tc.number  SUB_Audio_InterfaceLib_BindServiceRender_0002
* @tc.desc  test Binding failed service, where the service name is wrong.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_BindServiceRender_0002, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    handle = BindServiceRenderSo(BIND_NAME_ERROR.c_str());
    if (handle != nullptr) {
        CloseServiceRenderSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test BindServiceRenderSo API via nullptr input.
* @tc.number  SUB_Audio_InterfaceLib_BindServiceRender_0003
* @tc.desc  test Binding failed Service, nullptr pointer passed in.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_BindServiceRender_0003, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    char *bindNameNull = nullptr;
    handle = BindServiceRenderSo(bindNameNull);
    if (handle != nullptr) {
        CloseServiceRenderSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test BindServiceRenderSo API via binding service name lens is 25.
* @tc.number  SUB_Audio_InterfaceLib_BindServiceRender_0004
* @tc.desc  test Binding failed Service, Log printing 'service name not support!' and 'Failed to get service!'.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_BindServiceRender_0004, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    string bindNameLen = "renderrenderedededededede";
    handle = BindServiceRenderSo(bindNameLen.c_str());
    if (handle != nullptr) {
        CloseServiceRenderSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test BindServiceRenderSo API via binding service name lens is 26.
* @tc.number  SUB_Audio_InterfaceLib_BindServiceRender_0005
* @tc.desc  test Binding failed Service, Log printing 'Failed to snprintf_s'.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_BindServiceRender_0005, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    string bindNameLen = "renderrenderededededededer";
    handle = BindServiceRenderSo(bindNameLen.c_str());
    if (handle != nullptr) {
        CloseServiceRenderSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing volume value of AcodecIn is normal value and reading
*    this value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE and AUDIODRV_CTL_IOCTL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeValue = 0;
    float volumeThresholdValueMaxIn = 0;
    float volumeThresholdValueMinIn = 0;
    float volumeBoundaryValueIn = 127.9;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMaxIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMinIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMaxIn - 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwRender->renderParam.renderMode.ctlParam.volume;
    EXPECT_EQ(126, volumeValue);

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMinIn + 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwRender->renderParam.renderMode.ctlParam.volume;
    if (IS_ADM) {
        EXPECT_EQ(41, volumeValue);
    } else {
        EXPECT_EQ(1, volumeValue);
    }
    hwRender->renderParam.renderMode.ctlParam.volume = volumeBoundaryValueIn;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwRender->renderParam.renderMode.ctlParam.volume;
    EXPECT_EQ(127, volumeValue);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing volume value of AcodecIn is boundary value and reading
*    this value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0002
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE and AUDIODRV_CTL_IOCTL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioHwRender *hwRender = nullptr;
    struct DevHandle *handle = nullptr;
    float volumeThresholdValueMaxIn = 0;
    float volumeThresholdValueMinIn = 0;
    float volumeValue = 0;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMaxIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMinIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMinIn;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwRender->renderParam.renderMode.ctlParam.volume;
    if (IS_ADM == false) {
        EXPECT_EQ(0, volumeValue);
    } else {
        EXPECT_EQ(40, volumeValue);
    }

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMaxIn;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwRender->renderParam.renderMode.ctlParam.volume;
    EXPECT_EQ(127, volumeValue);

    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing volume value is invalid value of AcodecIn.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0003
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE and AUDIODRV_CTL_IOCTL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Volume_AcodecIn_Write_Read_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    float volumeThresholdValueMaxIn = 0;
    float volumeThresholdValueMinIn = 0;

    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMaxIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMinIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMaxIn + 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_FAILURE, ret);

    hwRender->renderParam.renderMode.ctlParam.volume = volumeThresholdValueMinIn - 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_FAILURE, ret);

    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}

/**
* @tc.name  test InterfaceLibCtlRender API via writing GetVolthreshold value that
*    Hardware equipment of Acodec_ChangeIn.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_GetVolthresholdRead_0002
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_GetVolthresholdRead_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeThresholdValueMaxIn = 0;
    float volumeThresholdValueMinIn = 0;
    float expMax = 127;
    float expMix = 40;
    struct AudioHwRender *hwRender = nullptr;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMaxIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMinIn = hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;
    EXPECT_EQ(expMax, volumeThresholdValueMaxIn);
    if (IS_ADM == false) {
        EXPECT_EQ(0, volumeThresholdValueMinIn);
    } else {
        EXPECT_EQ(expMix, volumeThresholdValueMinIn);
    }
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing ChannelMode value is normal value and reading this value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_ChannelMode_Write_Read_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE
*    and AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_ChannelMode_Write_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float channelModeExc = 1;
    struct DevHandle *handle = nullptr;
    struct DevHandle *handleRender = nullptr;
    struct AudioHwRender *impl = nullptr;
    ret = BindServiceAndHwRender(impl, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    handleRender = BindServiceRenderSo(BIND_RENDER.c_str());
    if (handleRender == nullptr) {
        CloseServiceRenderSo(handle);
        free(impl);
        impl = nullptr;
        ASSERT_NE(nullptr, handleRender);
    }
    ret = LibHwOutputRender(impl, handleRender);
    EXPECT_EQ(HDF_SUCCESS, ret);
    if (impl != nullptr) {
        impl->renderParam.frameRenderMode.mode = AUDIO_CHANNEL_BOTH_RIGHT;
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        channelModeExc = impl->renderParam.frameRenderMode.mode;
        EXPECT_EQ(AUDIO_CHANNEL_BOTH_RIGHT, channelModeExc);

        impl->renderParam.frameRenderMode.mode = AUDIO_CHANNEL_RIGHT_MUTE;
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        channelModeExc = impl->renderParam.frameRenderMode.mode;
        EXPECT_EQ(AUDIO_CHANNEL_RIGHT_MUTE, channelModeExc);
    }
    CloseServiceRenderSo(handleRender);
    CloseServiceRenderSo(handle);
    free(impl);
    impl = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing ChannelMode value is boundary value and reading this value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_ChannelMode_Write_Read_0002
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE
*    and AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_ChannelMode_Write_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float channelModeExc = 1;
    struct DevHandle *handleRender = nullptr;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *impl = nullptr;
    ret = BindServiceAndHwRender(impl, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    handleRender = BindServiceRenderSo(BIND_RENDER.c_str());
    if (handleRender == nullptr) {
        free(impl);
        impl = nullptr;
        CloseServiceRenderSo(handle);
        ASSERT_NE(nullptr, handleRender);
    }
    ret = LibHwOutputRender(impl, handleRender);
    EXPECT_EQ(HDF_SUCCESS, ret);
    if (impl != nullptr){
        impl->renderParam.frameRenderMode.mode = AUDIO_CHANNEL_NORMAL;
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        channelModeExc = impl->renderParam.frameRenderMode.mode;
        EXPECT_EQ(AUDIO_CHANNEL_NORMAL, channelModeExc);

        impl->renderParam.frameRenderMode.mode = AUDIO_CHANNEL_BOTH_MUTE;
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ, &impl->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        channelModeExc = impl->renderParam.frameRenderMode.mode;
        EXPECT_EQ(AUDIO_CHANNEL_BOTH_MUTE, channelModeExc);
    }
    CloseServiceRenderSo(handleRender);
    CloseServiceRenderSo(handle);
    free(impl);
    impl = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing mute value that include 1 and 0 and reading mute value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_MuteWrite_Read_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_MUTE_WRITE and AUDIODRV_CTL_IOCTL_MUTE_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_MuteWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteValue = 1;
    bool wishValue = 0;
    bool expectedValue = 1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.ctlParam.mute = 0;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwRender->renderParam.renderMode.ctlParam.mute;
    EXPECT_EQ(wishValue, muteValue);
    hwRender->renderParam.renderMode.ctlParam.mute = 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwRender->renderParam.renderMode.ctlParam.mute;
    EXPECT_EQ(expectedValue, muteValue);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing mute value that include 2 and reading mute value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_MuteWrite_Read_0002
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_MUTE_WRITE and AUDIODRV_CTL_IOCTL_MUTE_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_MuteWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteValue = 0;
    bool wishValue = 0;
    bool expectedValue = 1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.ctlParam.mute = 2;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwRender->renderParam.renderMode.ctlParam.mute;
    EXPECT_EQ(expectedValue, muteValue);
    hwRender->renderParam.renderMode.ctlParam.mute = 0;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_MUTE_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwRender->renderParam.renderMode.ctlParam.mute;
    EXPECT_EQ(wishValue, muteValue);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via selecting scene.
* @tc.number  SUB_Audio_InterfaceLib_CtlRender_SelectScene_0001
* @tc.desc  test InterfaceLibCtlRender,cmdId is AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLib_CtlRender_SelectScene_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_OUT_HEADSET,
    };
    hwRender->renderParam.renderMode.hwInfo.pathSelect.deviceInfo.deviceNum = 1;
    ret = strcpy_s(hwRender->renderParam.renderMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[0].deviceSwitch,
        PATHPLAN_COUNT, "Dacl enable");
    hwRender->renderParam.renderMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[0].value = 0;
    hwRender->renderParam.frameRenderMode.attrs.type = (enum AudioCategory)(scene.scene.id);
    hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins = scene.desc.pins;

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);

    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing normal gain value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_GAIN_WRITE and AUDIODRV_CTL_IOCTL_GAIN_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioHwRender *hwRender = nullptr;
    struct DevHandle *handle = nullptr;
    float gainValue = 0;
    float gainThresholdValueMax, gainThresholdValueMin;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin;
    ret = InitHwRenderMode(hwRender->renderParam.renderMode);
    EXPECT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMax - 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwRender->renderParam.renderMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMax - 1, gainValue);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMin + 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwRender->renderParam.renderMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMin + 1, gainValue);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = 2.3;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwRender->renderParam.renderMode.ctlParam.audioGain.gain;
    EXPECT_EQ(2, gainValue);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing boundary value of gain and reading gain value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0002
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_GAIN_WRITE and AUDIODRV_CTL_IOCTL_GAIN_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    float gainValue = 0;
    float gainThresholdValueMax, gainThresholdValueMin;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin;
    ret = InitHwRenderMode(hwRender->renderParam.renderMode);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMin;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwRender->renderParam.renderMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMin, gainValue);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMax;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwRender->renderParam.renderMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMax, gainValue);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via writing gain invalid value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0003
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_GAIN_WRITE and AUDIODRV_CTL_IOCTL_GAIN_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_GainWrite_Read_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float gainThresholdValueMax, gainThresholdValueMin;
    struct AudioHwRender *hwRender = nullptr;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin;
    ret = InitHwRenderMode(hwRender->renderParam.renderMode);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMax + 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    hwRender->renderParam.renderMode.ctlParam.audioGain.gain = gainThresholdValueMin - 1;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via getting gainthreshold value.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_GetGainthresholdRead_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_GetGainthresholdRead_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float gainThresholdValueMaxGet, gainThresholdValueMinGet;
    float expMax = 10;
    float expMix = 0;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMaxGet = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMinGet = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin;
    EXPECT_LT(expMax, gainThresholdValueMaxGet);
    EXPECT_EQ(expMix, gainThresholdValueMinGet);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via using Acodec_ChangeIn.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Acodec_ChangeIn_0001
* @tc.desc  test InterfaceLibCtlRender ,cmdId is AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Acodec_ChangeIn_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via inputting invalid cmdid.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Abnormal_0001
* @tc.desc  test InterfaceLibCtlRender, cmdId is invalid eg 50,so return -1.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Abnormal_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_CONTROL.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlRender(handle, 50, &hwRender->renderParam);
    if (ret == 0) {
        CloseServiceRenderSo(handle);
        free(hwRender);
        hwRender = nullptr;
        ASSERT_EQ(HDF_FAILURE, ret);
    }
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlRender API via inputting handleData invalid.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Abnormal_0002
* @tc.desc  test InterfaceLibCtlRender, handleData is nullptr,so return -1.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibCtlRender_Abnormal_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRenderParam *handleData = nullptr;
    handle = BindServiceRenderSo(BIND_CONTROL.c_str());
    ASSERT_NE(nullptr, handle);
    ret = InterfaceLibCtlRender(handle, AUDIODRV_CTL_IOCTL_ELEM_READ, handleData);
    if (ret == 0) {
        CloseServiceRenderSo(handle);
        ASSERT_EQ(HDF_FAILURE, ret);
    }
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceRenderSo(handle);
}

/**
* @tc.name  test InterfaceLibOutputRender API via cmdid is AUDIO_DRV_PCM_IOCTL_HW_PARAMS.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_HwParams_0001
* @tc.desc  test Audio lib Interface OutputRender.return 0 if the Interface call successful.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_HwParams_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via cmdid is AUDIO_DRV_PCM_IOCTL_PREPARE.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_Prepare_0001
* @tc.desc  test Audio lib Interface OutputRender.return 0 if the Interface call successful.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Prepare_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioHwRender *hwRender = nullptr;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_PREPARE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via cmdid is AUDIO_DRV_PCM_IOCTRL_START.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_Start_0001
* @tc.desc  test Audio lib Interface OutputRender.return 0 if the Interface call successful.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Start_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_PREPARE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTRL_START, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via cmdid is AUDIO_DRV_PCM_IOCTL_WRITE and AUDIO_DRV_PCM_IOCTRL_STOP.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_Write_Stop_0001
* @tc.desc  test Audio lib Interface OutputRender and Normal data flow distribution.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Write_Stop_0001, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    struct AudioHeadInfo wavHeadInfo = {};
    struct AudioHwRender *hwRender = nullptr;
    int32_t ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = LibHwOutputRender(hwRender, handle);
    EXPECT_EQ(HDF_SUCCESS, ret);
    char absPath[PATH_MAX] = {0};
    if (realpath(AUDIO_FILE.c_str(), absPath) == nullptr) {
        free(hwRender);
        CloseServiceRenderSo(handle);
        ASSERT_NE(nullptr, realpath(AUDIO_FILE.c_str(), absPath));
    }
    FILE *file = fopen(absPath, "rb");
    if (file == nullptr) {
        free(hwRender);
        CloseServiceRenderSo(handle);
        ASSERT_NE(nullptr, file);
    }
    if (hwRender != nullptr) {
        ret = WavHeadAnalysis(wavHeadInfo, file, hwRender->renderParam.frameRenderMode.attrs);
        if (ret < 0) {
            free(hwRender);
            fclose(file);
            CloseServiceRenderSo(handle);
            ASSERT_EQ(HDF_SUCCESS, ret);
        }
        ret = FrameLibStart(file, hwRender->renderParam.frameRenderMode.attrs, wavHeadInfo, hwRender);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_WRITE, &hwRender->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTRL_STOP, &hwRender->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, &hwRender->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        if (hwRender->renderParam.frameRenderMode.buffer != nullptr) {
            free(hwRender->renderParam.frameRenderMode.buffer);
        }
        free(hwRender);
    }
    CloseServiceRenderSo(handle);
    fclose(file);
}
/**
* @tc.name  test InterfaceLibCtlRender and InterfaceLibOutputRender API via Serial
    transmission of data flow and control flow.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_Write_0001
* @tc.desc  test Audio lib Interface CtlRender and OutputRender, Data stream and control stream send successful.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Write_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float muteValue = 0;
    float expectedValue = 0;
    struct DevHandle *handler = nullptr;
    struct DevHandle *handlec = nullptr;
    struct AudioHeadInfo wavHeadInfo = {};
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handler);
    ASSERT_EQ(HDF_SUCCESS, ret);
    handlec = BindServiceRenderSo(BIND_CONTROL.c_str());
    if (handlec == nullptr) {
        CloseServiceRenderSo(handler);
        free(hwRender);
        hwRender = nullptr;
        ASSERT_NE(nullptr, handlec);
    }
    if (hwRender != nullptr) {
        hwRender->renderParam.renderMode.ctlParam.mute = muteValue;
        ret = InterfaceLibCtlRender(handlec, AUDIODRV_CTL_IOCTL_MUTE_WRITE, &hwRender->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlRender(handlec, AUDIODRV_CTL_IOCTL_MUTE_READ, &hwRender->renderParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        expectedValue = hwRender->renderParam.renderMode.ctlParam.mute;
        EXPECT_EQ(expectedValue, muteValue);

        ret = LibStartAndStream(AUDIO_FILE, hwRender->renderParam.frameRenderMode.attrs,
            handler, hwRender, wavHeadInfo);
        if (ret < 0) {
            CloseServiceRenderSo(handler);
            CloseServiceRenderSo(handlec);
            free(hwRender);
            ASSERT_EQ(HDF_SUCCESS, ret);
        }
    ret = InterfaceLibOutputRender(handler, AUDIO_DRV_PCM_IOCTRL_STOP, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputRender(handler, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    }
    CloseServiceRenderSo(handler);
    CloseServiceRenderSo(handlec);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via pause.
* @tc.number  SUB_Audio_InterfaceLibCtlRender_Pause_0001
* @tc.desc  test InterfaceLibOutputRender,cmdId is AUDIODRV_CTL_IOCTL_PAUSE_WRITE.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Pause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = LibHwOutputRender(hwRender, handle);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.ctlParam.pause = 1;
    ret = InterfaceLibOutputRender(handle, AUDIODRV_CTL_IOCTL_PAUSE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via resuming.
* @tc.number  SUB_Audio_InterfaceLib_CtlRender_Resume_0001
* @tc.desc  test InterfaceLibOutputRender,cmdId is AUDIODRV_CTL_IOCTL_PAUSE_WRITE.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Resume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = LibHwOutputRender(hwRender, handle);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwRender->renderParam.renderMode.ctlParam.pause = 0;
    ret = InterfaceLibOutputRender(handle, AUDIODRV_CTL_IOCTL_PAUSE_WRITE, &hwRender->renderParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via setting the cmdId is invalid.
* @tc.number  SUB_Audio_InterfaceLibOutputRender__Abnormal_0001
* @tc.desc  test Audio lib Interface OutputRender via cmdid is invalid and cmdid is 30,so Interface return -1.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Abnormal_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRender *hwRender = nullptr;
    ret = BindServiceAndHwRender(hwRender, BIND_RENDER.c_str(), ADAPTER_NAME_USB, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputRender(handle, 30, &hwRender->renderParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceRenderSo(handle);
    free(hwRender);
    hwRender = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputRender API via inputting handleData is nullptr.
* @tc.number  SUB_Audio_InterfaceLibOutputRender_Abnormal_0002
* @tc.desc  test Audio lib Interface OutputRender, handleData is nullptr.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioLibRenderTest, SUB_Audio_InterfaceLibOutputRender_Abnormal_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwRenderParam *handleData = nullptr;
    handle = BindServiceRenderSo(BIND_RENDER.c_str());
    ASSERT_NE(nullptr, handle);
    ret = InterfaceLibOutputRender(handle, AUDIO_DRV_PCM_IOCTL_WRITE, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceRenderSo(handle);
}
}