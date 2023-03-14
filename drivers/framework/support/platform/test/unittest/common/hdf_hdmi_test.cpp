/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"
#include "hdmi_if.h"

using namespace testing::ext;

enum HdmiTestCmd {
    HDMI_START_AND_STOP_01 = 0,
    HDMI_SET_AUDIO_ATTR_01 = 1,
    HDMI_SET_VIDEO_ATTR_01 = 2,
    HDMI_SET_HDR_ATTR_01 = 3,
    HDMI_SET_AVMUTE_01 = 4,
    HDMI_EDID_RAW_DATA_GET_01 = 5,
    HDMI_DEEP_COLOR_SET_AND_GET_01 = 6,
    HDMI_HPD_REGISTER_AND_UNREGISTER_01 = 7,
};

class HdfLiteHdmiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteHdmiTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteHdmiTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteHdmiTest::SetUp()
{
}

void HdfLiteHdmiTest::TearDown()
{
}

static void HdmiUserHpdHandle(void *data, bool hpd)
{
    if (data == nullptr) {
        printf("data is null");
    }
    if (hpd) {
        printf("HdmiUserHpdHandle: hpd.");
    } else {
        printf("HdmiUserHpdHandle: unhpd.");
    }
}

static void HdmiUserSetAttrCaseTest(DevHandle handle)
{
    struct HdmiVideoAttr videoAttr;
    struct HdmiAudioAttr audioAttr;
    struct HdmiHdrAttr hdrAttr;
    int32_t ret;

    videoAttr.colorSpace = HDMI_COLOR_SPACE_YCBCR444;
    videoAttr.colorimetry = HDMI_COLORIMETRY_EXTENDED;
    videoAttr.extColorimetry = HDMI_EXTENDED_COLORIMETRY_BT2020_CONST_LUM;
    videoAttr.quantization = HDMI_QUANTIZATION_RANGE_FULL;
    ret = HdmiSetVideoAttribute(handle, &videoAttr);
    if (ret != HDF_SUCCESS) {
        printf("HdmiSetVideoAttribute fail.");
    }
    audioAttr.codingType = HDMI_AUDIO_CODING_TYPE_MP3;
    audioAttr.ifType = HDMI_AUDIO_IF_TYPE_I2S;
    audioAttr.bitDepth = HDMI_ADIO_BIT_DEPTH_16;
    audioAttr.sampleRate = HDMI_SAMPLE_RATE_8K;
    audioAttr.channels = HDMI_AUDIO_FORMAT_CHANNEL_3;
    ret = HdmiSetAudioAttribute(handle, &audioAttr);
    if (ret != HDF_SUCCESS) {
        printf("HdmiSetAudioAttribute fail.");
    }
    hdrAttr.mode = HDMI_HDR_MODE_CEA_861_3;
    hdrAttr.userMode = HDMI_HDR_USERMODE_DOLBY;
    hdrAttr.eotfType = HDMI_EOTF_SMPTE_ST_2048;
    hdrAttr.metadataType = HDMI_DRM_STATIC_METADATA_TYPE_1;
    hdrAttr.colorimetry = HDMI_HDR_EXTENDED_COLORIMETRY_XV_YCC_709;
    ret = HdmiSetHdrAttribute(handle, &hdrAttr);
    if (ret != HDF_SUCCESS) {
        printf("HdmiSetHdrAttribute fail.");
    }
}

static void HdmiUserTest(void)
{
    DevHandle handle = nullptr;
    int32_t ret;
    enum HdmiDeepColor color = HDMI_DEEP_COLOR_24BITS;
    uint8_t data[128] = {0};
    struct HdmiHpdCallbackInfo callback;

    handle = HdmiOpen(0);
    if (handle == nullptr) {
        printf("HdmiOpen fail.");
    }
    callback.data = nullptr;
    callback.callbackFunc = HdmiUserHpdHandle;
    ret = HdmiRegisterHpdCallbackFunc(handle, &callback);
    if (ret != HDF_SUCCESS) {
        printf("HdmiRegisterHpdCallbackFunc fail.");
    }
    ret = HdmiAvmuteSet(handle, true);
    if (ret != HDF_SUCCESS) {
        printf("HdmiAvmuteSet fail.");
    }
    ret = HdmiDeepColorSet(handle, HDMI_DEEP_COLOR_48BITS);
    if (ret != HDF_SUCCESS) {
        printf("HdmiDeepColorSet fail.");
    }
    ret = HdmiDeepColorGet(handle, &color);
    if (ret != HDF_SUCCESS) {
        printf("HdmiDeepColorGet fail.");
    }
    printf("HdmiDeepColorGet success, color = %d.", color);

    HdmiUserSetAttrCaseTest(handle);

    ret = HdmiStart(handle);
    if (ret != HDF_SUCCESS) {
        printf("HdmiStart fail.");
    }
    ret = HdmiReadSinkEdid(handle, data, 128);
    if (ret < 0) {
        printf("HdmiReadSinkEdid fail.");
    }
    printf("HdmiReadSinkEdid success, data[6] = %d, data[8] = %d.", data[6], data[8]);
    ret = HdmiStop(handle);
    if (ret != HDF_SUCCESS) {
        printf("HdmiStop fail.");
    }
    ret = HdmiUnregisterHpdCallbackFunc(handle);
    if (ret != HDF_SUCCESS) {
        printf("HdmiUnregisterHpdCallbackFunc fail.");
    }
    HdmiClose(handle);
}

/**
  * @tc.name: HdmiStartAndStop001
  * @tc.desc: test HdmiStart/HdmiStop interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiStartAndStop001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_START_AND_STOP_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiSetAudioAttr001
  * @tc.desc: test HdmiSetAudioAttr interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiSetAudioAttr001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_SET_AUDIO_ATTR_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiSetVideoAttr001
  * @tc.desc: test HdmiSetVideoAttr interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiSetVideoAttr001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_SET_VIDEO_ATTR_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiSetHdrAttr001
  * @tc.desc: test HdmiSetHdrAttr interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiSetHdrAttr001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_SET_HDR_ATTR_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiSetAvmute001
  * @tc.desc: test HdmiSetAvmute interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiSetAvmute001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_SET_AVMUTE_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiEdidRawDataGet001
  * @tc.desc: test HdmiReadSinkEdid interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiEdidRawDataGet001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_EDID_RAW_DATA_GET_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiDeepColorSetAndGet001
  * @tc.desc: test HdmiDeepColorSet/HdmiDeepColorGet interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiDeepColorSetAndGet001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_DEEP_COLOR_SET_AND_GET_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiDeepColorSetAndGet001
  * @tc.desc: test HdmiDeepColorSet/HdmiDeepColorGet interface in kernel status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiHpdRegisterAndUngister001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_HDMI_TYPE, HDMI_HPD_REGISTER_AND_UNREGISTER_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdmiUserTest001
  * @tc.desc: test hdmi all interface in user status.
  * @tc.type: FUNC
  * @tc.require: SR000G4490
  */
HWTEST_F(HdfLiteHdmiTest, HdmiUserTest001, TestSize.Level1)
{
    HdmiUserTest();
}
