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
 * accessing a driver ADM interface lib, and captureing audios
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
#include "audio_libcapture_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
const string BIND_CONTROL = "control";
const string BIND_CAPTURE = "capture";
const string BIND_NAME_ERROR = "rendeo";
const string ADAPTER_NAME_INTERNAL = "internal";

class AudioLibCaptureTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static struct DevHandle *(*BindServiceCaptureSo)(const char *);
    static int32_t (*InterfaceLibOutputCapture)(struct DevHandle *, int, struct AudioHwCaptureParam *);
    static int32_t (*InterfaceLibCtlCapture)(struct DevHandle *, int, struct AudioHwCaptureParam *);
    static void (*CloseServiceCaptureSo)(struct DevHandle *);
#ifdef AUDIO_MPI_SO
    static int32_t (*SdkInit)();
    static void (*SdkExit)();
#endif
    static void *PtrHandle;
    int32_t BindServiceAndHwCapture(struct AudioHwCapture *&hwCapture, const std::string BindName,
                                    const std::string adapterNameCase, struct DevHandle *&handle) const;
};

struct DevHandle *(*AudioLibCaptureTest::BindServiceCaptureSo)(const char *) = nullptr;
int32_t (*AudioLibCaptureTest::InterfaceLibOutputCapture)(struct DevHandle *, int,
    struct AudioHwCaptureParam *) = nullptr;
int32_t (*AudioLibCaptureTest::InterfaceLibCtlCapture)(struct DevHandle *, int, struct AudioHwCaptureParam *) = nullptr;
void (*AudioLibCaptureTest::CloseServiceCaptureSo)(struct DevHandle *) = nullptr;
#ifdef AUDIO_MPI_SO
    int32_t (*AudioLibCaptureTest::SdkInit)() = nullptr;
    void (*AudioLibCaptureTest::SdkExit)() = nullptr;
#endif
void *AudioLibCaptureTest::PtrHandle = nullptr;

void AudioLibCaptureTest::SetUpTestCase(void)
{
    char resolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_capture");
    PtrHandle = dlopen(resolvedPath, RTLD_LAZY);
    if (PtrHandle == nullptr) {
        return;
    }
    BindServiceCaptureSo = (struct DevHandle* (*)(const char *))dlsym(PtrHandle, "AudioBindServiceCapture");
    InterfaceLibOutputCapture = (int32_t (*)(struct DevHandle *, int,
                                 struct AudioHwCaptureParam *))dlsym(PtrHandle, "AudioInterfaceLibOutputCapture");
    InterfaceLibCtlCapture = (int32_t (*)(struct DevHandle *, int,
                                          struct AudioHwCaptureParam *))dlsym(PtrHandle, "AudioInterfaceLibCtlCapture");
    CloseServiceCaptureSo = (void (*)(struct DevHandle *))dlsym(PtrHandle, "AudioCloseServiceCapture");
    if (BindServiceCaptureSo == nullptr || CloseServiceCaptureSo == nullptr ||
        InterfaceLibCtlCapture == nullptr || InterfaceLibOutputCapture == nullptr) {
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

void AudioLibCaptureTest::TearDownTestCase(void)
{
    if (BindServiceCaptureSo != nullptr) {
        BindServiceCaptureSo = nullptr;
    }
    if (CloseServiceCaptureSo != nullptr) {
        CloseServiceCaptureSo = nullptr;
    }
    if (InterfaceLibCtlCapture != nullptr) {
        InterfaceLibCtlCapture = nullptr;
    }
    if (InterfaceLibOutputCapture != nullptr) {
        InterfaceLibOutputCapture = nullptr;
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

void AudioLibCaptureTest::SetUp(void) {}

void AudioLibCaptureTest::TearDown(void) {}

int32_t AudioLibCaptureTest::BindServiceAndHwCapture(struct AudioHwCapture *&hwCapture,
    const std::string BindName, const std::string adapterNameCase, struct DevHandle *&handle) const
{
    handle = BindServiceCaptureSo(BindName.c_str());
    if (handle == nullptr) {
        return HDF_FAILURE;
    }
    hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(*hwCapture));
    if (hwCapture == nullptr) {
        CloseServiceCaptureSo(handle);
        return HDF_FAILURE;
    }
    if (InitHwCapture(hwCapture, adapterNameCase)) {
        free(hwCapture);
        hwCapture = nullptr;
        CloseServiceCaptureSo(handle);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/**
* @tc.name  Test AudioBindServiceCapture API via legal input
* @tc.number   SUB_Audio_InterfaceLibBindServiceCapture_0001
* @tc.desc  Test AudioBindServiceCapture interface,return 0 is call successfully
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibBindServiceCapture_0001, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    handle = BindServiceCaptureSo(BIND_CONTROL.c_str());
    EXPECT_NE(nullptr, handle);
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  Test AudioBindServiceCapture API via invalid input.
* @tc.number  SUB_Audio_InterfaceLibBindServiceCapture_0002
* @tc.desc  test Binding failed Service which invalid service Name is rendeo.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibBindServicecapture_0002, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    handle = BindServiceCaptureSo(BIND_NAME_ERROR.c_str());
    if (handle != nullptr) {
        CloseServiceCaptureSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  Test AudioBindServiceCapture API via setting the incoming parameter handle is empty.
* @tc.number   SUB_Audio_InterfaceLibBindServiceCapture_0003
* @tc.desc  test Binding failed Service, nullptr pointer passed in.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibBindServiceCapture_0003, TestSize.Level1)
{
    struct DevHandle *handle = {};
    char *bindNameNull = nullptr;
    handle = BindServiceCaptureSo(bindNameNull);
    if (handle != nullptr) {
        CloseServiceCaptureSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test BindServiceCaptureSo API via name lens is 25.
* @tc.number  SUB_Audio_InterfaceLibBindServiceCapture_0004
* @tc.desc  test Binding failed Service, Log printing 'service name not support!' and 'Failed to get service!'.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibBindServiceCapture_0004, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    string bindNameLen = "capturecaptureededededede";
    handle = BindServiceCaptureSo(bindNameLen.c_str());
    if (handle != nullptr) {
        CloseServiceCaptureSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test BindServiceCaptureSo API via name lens is 26.
* @tc.number  SUB_Audio_InterfaceLibBindServiceCapture_0005
* @tc.desc  test Binding failed Service, Log printing 'Failed to snprintf_s'.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibBindServiceCapture_0005, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    string bindNameLen = "capturecaptureedededededer";
    handle = BindServiceCaptureSo(bindNameLen.c_str());
    if (handle != nullptr) {
        CloseServiceCaptureSo(handle);
        ASSERT_EQ(nullptr, handle);
    }
    EXPECT_EQ(nullptr, handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing mute value that include 1 and 0 and reading mute value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_MuteWrite_Read_0001
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_MuteWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteValue = 1;
    bool wishValue = 0;
    bool expectedValue = 1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;

    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwCapture->captureParam.captureMode.ctlParam.mute = 0;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwCapture->captureParam.captureMode.ctlParam.mute;
    EXPECT_EQ(wishValue, muteValue);
    hwCapture->captureParam.captureMode.ctlParam.mute = 1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwCapture->captureParam.captureMode.ctlParam.mute;
    EXPECT_EQ(expectedValue, muteValue);
    free(hwCapture);
    hwCapture = nullptr;
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing mute value that is 2 and read mute value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_MuteWrite_Read_0002
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_MuteWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    bool muteValue = 0;
    bool wishValue = 0;
    bool expectedValue = 1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    hwCapture->captureParam.captureMode.ctlParam.mute = 2;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwCapture->captureParam.captureMode.ctlParam.mute;
    EXPECT_EQ(expectedValue, muteValue);
    hwCapture->captureParam.captureMode.ctlParam.mute = 0;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    muteValue = hwCapture->captureParam.captureMode.ctlParam.mute;
    EXPECT_EQ(wishValue, muteValue);
    free(hwCapture);
    hwCapture = nullptr;
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing volume value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0001
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeValue;
    float expectedValue1 = 86;
    float expectedValue2 = 1;
    float volumeThresholdValueMax = 0;
    float volumeThresholdValueMin = 0;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin;

    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMax-1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwCapture->captureParam.captureMode.ctlParam.volume;
    if (IS_ADM == false) {
        EXPECT_EQ(126, volumeValue);
    } else {
        EXPECT_EQ(expectedValue1, volumeValue);
    }

    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMin+1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwCapture->captureParam.captureMode.ctlParam.volume;
    EXPECT_EQ(expectedValue2, volumeValue);

    free(hwCapture);
    hwCapture = nullptr;
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing volume value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0002
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    float volumeValue = 0;
    float expectedValueMax = 87;
    float expectedValueMin = 0;
    float volumeThresholdValueMax = 0;
    float volumeThresholdValueMin = 0;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin;

    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMin;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwCapture->captureParam.captureMode.ctlParam.volume;
    EXPECT_EQ(expectedValueMin, volumeValue);

    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMax;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeValue = hwCapture->captureParam.captureMode.ctlParam.volume;
    if (IS_ADM == false) {
        EXPECT_EQ(127, volumeValue);
    } else {
        EXPECT_EQ(expectedValueMax, volumeValue);
    }

    free(hwCapture);
    hwCapture = nullptr;
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing volume value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0003
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_VolumeWrite_Read_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeThresholdValueMax = 0;
    struct AudioHwCapture *hwCapture = nullptr;
    float volumeThresholdValueMin = 0;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin;

    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMax+1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    hwCapture->captureParam.captureMode.ctlParam.volume = volumeThresholdValueMin-1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    free(hwCapture);
    hwCapture = nullptr;
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing GetGainthreshold value of Hardware equipment.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_GetVolthresholdRead_0001
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_GetVolthresholdRead_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float volumeThresholdValueMax = 0;
    float volumeThresholdValueMin = 0;
    float expMax = 87;
    float expMix = 0;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    volumeThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMax;
    volumeThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.volThreshold.volMin;
    if (IS_ADM == false) {
        EXPECT_EQ(127, volumeThresholdValueMax);
    } else {
        EXPECT_EQ(expMax, volumeThresholdValueMax);
    }
    EXPECT_EQ(expMix, volumeThresholdValueMin);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via selecting scene.
* @tc.number  SUB_Audio_InterfaceLib_CtlCapture_SelectScene_0001
* @tc.desc  test InterfaceLibCtlCapture,cmdId is AUDIODRV_CTL_IOCTL_SCENESELECT_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_SelectScene_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle* handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    struct AudioSceneDescriptor scene = {
        .scene.id = 0,
        .desc.pins = PIN_IN_HS_MIC,
    };
    hwCapture->captureParam.captureMode.hwInfo.pathSelect.deviceInfo.deviceNum = 1;
    ret = strcpy_s(hwCapture->captureParam.captureMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[0].deviceSwitch,
        PATHPLAN_COUNT, "LPGA MIC Switch");
    hwCapture->captureParam.captureMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[0].value = 0;
    hwCapture->captureParam.frameCaptureMode.attrs.type = (enum AudioCategory)(scene.scene.id);
    hwCapture->captureParam.captureMode.hwInfo.deviceDescript.pins = scene.desc.pins;

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_SCENESELECT_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);

    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing normal value of gain and reading gain value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0001
* @tc.desc  test InterfaceLibCtlCapture,cmdId is AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float gainValue = 0;
    float gainThresholdValueMax = 0;
    float gainThresholdValueMin = 0;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMin;
    ret = InitHwCaptureMode(hwCapture->captureParam.captureMode);
    EXPECT_EQ(HDF_SUCCESS, ret);

    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMax - 1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwCapture->captureParam.captureMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMax - 1, gainValue);
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMin + 1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwCapture->captureParam.captureMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMin + 1, gainValue);
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = 2.3;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwCapture->captureParam.captureMode.ctlParam.audioGain.gain;
    EXPECT_EQ(2, gainValue);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing boundary value of gain and reading gain value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0002
* @tc.desc  test InterfaceLibCtlCapture,cmdId is AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE and
*    AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    float gainValue = 0;
    float gainThresholdValueMax = 0;
    float gainThresholdValueMin = 0;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMin;
    ret = InitHwCaptureMode(hwCapture->captureParam.captureMode);
    EXPECT_EQ(HDF_SUCCESS, ret);

    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMax;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwCapture->captureParam.captureMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMax, gainValue);
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMin;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainValue = hwCapture->captureParam.captureMode.ctlParam.audioGain.gain;
    EXPECT_EQ(gainThresholdValueMin, gainValue);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing invalid value of gain.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0003
* @tc.desc  test InterfaceLibCtlCapture ,return -1,If the threshold is invalid.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_GainWrite_Read_0003, TestSize.Level1)
{
    int32_t ret = -1;
    float gainThresholdValueMax = 0;
    float gainThresholdValueMin = 0;
    struct AudioHwCapture *hwCapture = nullptr;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMin;
    ret = InitHwCaptureMode(hwCapture->captureParam.captureMode);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMax + 1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gain = gainThresholdValueMin - 1;
    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via writing GetGainthreshold value.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_GetGainthresholdRead_0001
* @tc.desc  test InterfaceLibCtlCapture ,cmdId is AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE(23).
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_GetGainthresholdRead_0001, TestSize.Level1)
{
    int32_t ret = -1;
    float gainThresholdValueMax, gainThresholdValueMin;
    float expMax = 10;
    float expMix = 0;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    gainThresholdValueMax = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMax;
    gainThresholdValueMin = hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMin;
    EXPECT_LT(expMax, gainThresholdValueMax);
    EXPECT_EQ(expMix, gainThresholdValueMin);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via inputting cmdid invalid.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_Abnormal_0001
* @tc.desc  test InterfaceLibCtlCapture, cmdId is invalid eg 30,so return -1.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_Abnormal_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibCtlCapture(handle, 30, &hwCapture->captureParam);
    if (ret == 0) {
        CloseServiceCaptureSo(handle);
        free(hwCapture);
        hwCapture = nullptr;
        ASSERT_EQ(HDF_FAILURE, ret);
    }
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibCtlCapture API via inputting handleData invalid.
* @tc.number  SUB_Audio_InterfaceLibCtlCapture_Abnormal_0002
* @tc.desc  test InterfaceLibCtlCapture, handleData is invalid,so return -1.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibCtlCapture_Abnormal_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCaptureParam *handleData = nullptr;
    handle = BindServiceCaptureSo(BIND_CONTROL.c_str());
    ASSERT_NE(nullptr, handle);
    ret = InterfaceLibCtlCapture(handle, AUDIO_DRV_PCM_IOCTL_READ, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceCaptureSo(handle);
}
/**
* @tc.name  test InterfaceLibOutputCapture API via cmdid is AUDIO_DRV_PCM_IOCTL_HW_PARAMS.
* @tc.number  SUB_Audio_InterfaceLibOutputCapture_HwParams_0001
* @tc.desc  Test AudioOutputcapture interface,return 0 is call successfully.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest,  SUB_Audio_InterfaceLibOutputCapture_HwParams_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputCapture API via cmdid is AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE.
* @tc.number  SUB_Audio_InterfaceLib_OutputCapture_Prepare_0001
* @tc.desc  test InterfaceLibOutputCapture,cmdId is AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest,  SUB_Audio_InterfaceLibOutputCapture_Prepare_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioHwCapture *hwCapture = nullptr;
    struct DevHandle *handle = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputCapture API via cmdid is AUDIO_DRV_PCM_IOCTRL_START_CAPTURE.
* @tc.number   SUB_Audio_InterfaceLib_OutputCapture_Start_0001
* @tc.desc  test InterfaceLibOutputCapture,cmdId is AUDIO_DRV_PCM_IOCTRL_START_CAPTURE.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest,  SUB_Audio_InterfaceLibOutputCapture_Start_0001, TestSize.Level1)
{
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    int32_t ret = -1;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_START_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  Test AudioOutputcapture API via cmdid is AUDIO_DRV_PCM_IOCTL_READ.
            and AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE.
* @tc.number   SUB_Audio_InterfaceLibOutputCapture_Read_Stop_0001
* @tc.desc  test InterfaceLibOutputCapture,cmdId is AUDIO_DRV_PCM_IOCTL_READ and AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_Read_Stop_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_START_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwCapture->captureParam.frameCaptureMode.buffer = (char *)calloc(1, 16384);
    if (hwCapture->captureParam.frameCaptureMode.buffer == nullptr) {
        CloseServiceCaptureSo(handle);
        free(hwCapture);
        hwCapture = nullptr;
        ASSERT_NE(nullptr, hwCapture);
    }
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_READ, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture->captureParam.frameCaptureMode.buffer);
    hwCapture->captureParam.frameCaptureMode.buffer = nullptr;
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  Test AudioOutputcapture API data flow and control flow are serial.
* @tc.number  SUB_Audio_InterfaceLibOutputCapture_0001
* @tc.desc  test Audio lib Interface CtlCapture and OutputCapture,Data stream and control stream send successful.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_0001, TestSize.Level1)
{
    struct DevHandle *handle1 = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    int32_t ret = BindServiceAndHwCapture(hwCapture, BIND_CONTROL.c_str(), ADAPTER_NAME_INTERNAL, handle1);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct DevHandle *handle2 = BindServiceCaptureSo(BIND_CAPTURE.c_str());
    if (handle2 == nullptr) {
        CloseServiceCaptureSo(handle1);
        free(hwCapture);
        ASSERT_NE(nullptr, handle2);
    }
    if (hwCapture != nullptr) {
        hwCapture->captureParam.captureMode.ctlParam.mute = 0;
        ret = InterfaceLibCtlCapture(handle1, AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibCtlCapture(handle1, AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTRL_START_CAPTURE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        hwCapture->captureParam.frameCaptureMode.buffer = (char *)calloc(1, 16384);
        if (hwCapture->captureParam.frameCaptureMode.buffer == nullptr) {
            CloseServiceCaptureSo(handle1);
            CloseServiceCaptureSo(handle2);
            free(hwCapture);
            hwCapture = nullptr;
            ASSERT_NE(nullptr, hwCapture);
        }
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTL_READ, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        ret = InterfaceLibOutputCapture(handle2, AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE, &hwCapture->captureParam);
        EXPECT_EQ(HDF_SUCCESS, ret);
        if (hwCapture->captureParam.frameCaptureMode.buffer != nullptr) {
            free(hwCapture->captureParam.frameCaptureMode.buffer);
        }
        free(hwCapture);
    }
    CloseServiceCaptureSo(handle1);
    CloseServiceCaptureSo(handle2);
}
/**
* @tc.name  test InterfaceLibOutputCapture API via pause.
* @tc.number  SUB_Audio_InterfaceLibctlcapture_Pause_0001
* @tc.desc  test InterfaceLibOutputCapture,cmdId is AUDIODRV_CTL_IOCTL_PAUSE_WRITE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_Pause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = {};
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_START_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwCapture->captureParam.captureMode.ctlParam.pause = 1;
    ret = InterfaceLibOutputCapture(handle, AUDIODRV_CTL_IOCTL_PAUSE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  test InterfaceLibOutputCapture API via resume.
* @tc.number  SUB_Audio_InterfaceLibctlcapture_Resume_0001
* @tc.desc  test InterfaceLibOutputCapture,cmdId is AUDIODRV_CTL_IOCTL_PAUSE_WRITE.
* @tc.author: wangkang
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_Resume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct AudioHwCapture *hwCapture = nullptr;
    struct DevHandle *handle = {};
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_HW_PARAMS, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTRL_START_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    hwCapture->captureParam.captureMode.ctlParam.pause = 0;
    ret = InterfaceLibOutputCapture(handle, AUDIODRV_CTL_IOCTL_PAUSE_WRITE_CAPTURE, &hwCapture->captureParam);
    EXPECT_EQ(HDF_SUCCESS, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  Test InterfaceLibOutputCapture API via setting the cmdId(30) is invalid
* @tc.number  SUB_Audio_InterfaceLibOutputCapture_Abnormal_0001
* @tc.desc  test OutputCapture interface, return -1 if the cmdId is invalid.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_Abnormal_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, 30, &hwCapture->captureParam);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
/**
* @tc.name  Test Outputcapture API via setting the incoming parameter handleData is empty
* @tc.number  SUB_Audio_InterfaceLibOutputCapture_Abnormal_0002
* @tc.desc   Test Outputcapture interface, return -1 if the incoming parameter handleData is empty.
* @tc.author: liutian
*/
HWTEST_F(AudioLibCaptureTest, SUB_Audio_InterfaceLibOutputCapture_Abnormal_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct DevHandle *handle = nullptr;
    struct AudioHwCaptureParam *handleData = nullptr;
    struct AudioHwCapture *hwCapture = nullptr;
    ret = BindServiceAndHwCapture(hwCapture, BIND_CAPTURE.c_str(), ADAPTER_NAME_INTERNAL, handle);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = InterfaceLibOutputCapture(handle, AUDIO_DRV_PCM_IOCTL_READ, handleData);
    EXPECT_EQ(HDF_FAILURE, ret);
    CloseServiceCaptureSo(handle);
    free(hwCapture);
    hwCapture = nullptr;
}
}