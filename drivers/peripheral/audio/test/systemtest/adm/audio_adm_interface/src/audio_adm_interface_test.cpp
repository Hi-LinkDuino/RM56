/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
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
 * @brief Test audio adm interface
 *
 * @since 1.0
 * @version 1.0
 */
#include "audio_adm_common.h"
#include "audio_adm_interface_test.h"

using namespace std;
using namespace testing::ext;
using namespace HMOS::Audio;

namespace {
    const int CONTROL_DISP_METHOD_CMD_ILLEGAL = 5;
    const int STREAM_DISP_METHOD_CMD_ILLEGAL = 30;
    const int CHANEL_MODE_ILLEGAL = 9;
    const int MAX_GAIN_VALUE = 15;
    const int MIN_GAIN_VALUE = 0;
    const int ERROR_GAIN_VALUE = MAX_GAIN_VALUE + 1;

    class AudioAdmInterfaceTest : public testing::Test {
    public:
        static void SetUpTestCase(void);
        static void TearDownTestCase(void);
        void SetUp();
        void TearDown();
};

void AudioAdmInterfaceTest::SetUpTestCase(void) {}

void AudioAdmInterfaceTest::TearDownTestCase(void) {}

void AudioAdmInterfaceTest::SetUp(void) {}

void AudioAdmInterfaceTest::TearDown(void) {}

/**
* @tc.name  Test the ADM ctrl data analysis function via setting the incoming parameter cmdid is illegal
* @tc.number  SUB_Audio_ControlDispatch_0001
* @tc.desc  Test the ADM ctrl data analysis function,return -1 when setting the incoming parameter cmdid is illegal
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlDispatch_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *writeBuf = nullptr;
    struct HdfSBuf *writeReply = nullptr;

    struct AudioCtlElemValue writeElemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = 5,
    };

    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);

    writeBuf = HdfSbufObtainDefaultSize();
    if (writeBuf == nullptr) {
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, writeBuf);
    }
    ret = WriteEleValueToBuf(writeBuf, writeElemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(&service->object, CONTROL_DISP_METHOD_CMD_ILLEGAL, writeBuf, writeReply);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(writeBuf);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl data analysis function via setting the incoming parameter object is nullptr
* @tc.number  SUB_Audio_ControlDispatch_0002
* @tc.desc  Test the ADM ctrl data analysis function,return -1 when setting the incoming parameter object is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlDispatch_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *writeBuf = nullptr;
    struct HdfSBuf *writeReply = nullptr;
    struct HdfObject *objectNull = nullptr;
    struct AudioCtlElemValue writeElemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = 6,
    };

    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);

    writeBuf = HdfSbufObtainDefaultSize();
    if (writeBuf == nullptr) {
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, writeBuf);
    }

    ret = WriteEleValueToBuf(writeBuf, writeElemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(objectNull, AUDIODRV_CTRL_IOCTRL_ELEM_WRITE, writeBuf, writeReply);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(writeBuf);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream data analysis function via setting the incoming parameter cmdid is illegal
* @tc.number  SUB_Audio_StreamDispatch_0001
* @tc.desc  Test the ADM stream data analysis function,return -1 when setting the incoming parameter cmdid is illegal
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamDispatch_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBuf = nullptr;
    struct HdfSBuf *reply = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 2048, .rate = 11025,
        .periodCount = 32, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16385
    };

    service = HdfIoServiceBind(HDF_RENDER_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);

    sBuf = HdfSbufObtainDefaultSize();
    if (sBuf == nullptr) {
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, sBuf);
    }
    ret = WriteHwParamsToBuf(sBuf, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(&service->object, STREAM_DISP_METHOD_CMD_ILLEGAL, sBuf, reply);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream data analysis function via setting the incoming parameter object is nullptr
* @tc.number  SUB_Audio_StreamDispatch_0002
* @tc.desc  Test the ADM stream data analysis function,return -1 when setting the incoming parameter object is nullptr
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamDispatch_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBuf = nullptr;
    struct HdfSBuf *reply = nullptr;
    struct HdfObject *objectNull = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 2048, .rate = 11025,
        .periodCount = 32, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16385
    };

    service = HdfIoServiceBind(HDF_RENDER_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);

    sBuf = HdfSbufObtainDefaultSize();
    if (sBuf == nullptr) {
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, sBuf);
    }
    ret = WriteHwParamsToBuf(sBuf, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(objectNull, AUDIO_DRV_PCM_IOCTRL_HW_PARAMS, sBuf, reply);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfSbufRecycle(sBuf);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting gain is in the range
* @tc.number  SUB_Audio_ControlHostElemWrite_Read_0001
* @tc.desc  Test the ADM ctrl function,return 0 when setting gain's value is in the range(value=5)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_Read_0001, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = 5;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = 5,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting gain is min value
* @tc.number  SUB_Audio_ControlHostElemWrite_Read_0002
* @tc.desc  Test the ADM ctrl function,return 0 when setting gain's value is min value
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_Read_0002, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = MIN_GAIN_VALUE;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = MIN_GAIN_VALUE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting gain is max value
* @tc.number  SUB_Audio_ControlHostElemWrite_Read_0003
* @tc.desc  Test the ADM ctrl function,return 0 when setting gain's value is max value
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_Read_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    int32_t expectValue = MAX_GAIN_VALUE;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = MAX_GAIN_VALUE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting gain is out of the range
* @tc.number  SUB_Audio_ControlHostElemWrite_Read_0004
* @tc.desc  Test the ADM ctrl function,return -1 when setting gain's value is out of the range(value=16)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_Read_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = ERROR_GAIN_VALUE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_NORMAL"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0005
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_NORMAL"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0005, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_NORMAL;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_NORMAL,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_BOTH_LEFT"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0006
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_BOTH_LEFT"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0006, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_BOTH_LEFT;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_BOTH_LEFT,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_BOTH_RIGHT"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0007
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_BOTH_RIGHT"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0007, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_BOTH_RIGHT;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_BOTH_RIGHT,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_EXCHANGE"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0008
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_EXCHANGE"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0008, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_EXCHANGE;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_EXCHANGE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_MIX"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0009
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_MIX"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0009, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_MIX;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_MIX,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_LEFT_MUTE"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0010
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_LEFT_MUTE"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0010, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_LEFT_MUTE;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_LEFT_MUTE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_RIGHT_MUTE"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_00011
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_RIGHT_MUTE"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0011, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_RIGHT_MUTE;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_RIGHT_MUTE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "AUDIO_CHANNEL_BOTH_MUTE"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_00012
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "AUDIO_CHANNEL_BOTH_MUTE"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0012, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = AUDIO_CHANNEL_BOTH_MUTE;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = AUDIO_CHANNEL_BOTH_MUTE,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via setting channelmode is "CHANEL_MODE_ILLEGAL"
* @tc.number  SUB_Audio_ControlHostElemWrite_read_00013
* @tc.desc  Test the ADM ctrl function,return 0 when setting channelmode is "CHANEL_MODE_ILLEGAL"
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0013, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO,
        .id.itemName = "Render Channel Mode",
        .value[0] = CHANEL_MODE_ILLEGAL,
    };
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via getting gainthreshold
* @tc.number  SUB_Audio_ControlHostElemWrite_Read_00014
* @tc.desc  Test the ADM ctrl function,return 0 when getting gainthreshold
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_Read_0014, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectMinValue = MIN_GAIN_VALUE;
    int32_t expectMaxValue = MAX_GAIN_VALUE;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *readBuf = nullptr;
    struct HdfSBuf *readReply = nullptr;
    struct AudioCtlElemId id = {
        .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .itemName = "Mic Left Gain",
    };
    struct AudioCtlElemValue readElemValue = {};
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);

    readReply = HdfSbufObtainDefaultSize();
    if (readReply == nullptr) {
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, readReply);
    }
    readBuf = HdfSbufObtainDefaultSize();
    if (readBuf == nullptr) {
        HdfSbufRecycle(readReply);
        HdfIoServiceRecycle(service);
        ASSERT_NE(nullptr, readBuf);
    }
    ret = WriteIdToBuf(readBuf, id);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(&service->object, AUDIODRV_CTRL_IOCTRL_ELEM_READ, readBuf, readReply);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufReadInt32(readReply, &readElemValue.value[0]);
    EXPECT_EQ(expectMaxValue, readElemValue.value[0]);
    HdfSbufReadInt32(readReply, &readElemValue.value[1]);
    EXPECT_EQ(expectMinValue, readElemValue.value[1]);

    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(readReply);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing normal value of volume and reading volume.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0015
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0015, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = 100;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Main Playback Volume",
        .value[0] = 100,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}

/**
* @tc.name  Test the ADM control data which is writing mix value of volume and reading volume.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0016
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0016, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = 40;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Main Playback Volume",
        .value[0] = 40,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing max value of volume and reading volume.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0017
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0017, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = 127;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Main Playback Volume",
        .value[0] = 127,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing invlaid value of volume.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0018
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0018, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Main Playback Volume",
        .value[0] = 128,
    };
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing invlaid value of volume.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0019
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0019, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Main Playback Volume",
        .value[0] = 39,
    };
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing normal value of mute and reading mute.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0020
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0020, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t expectValue = 0;
    struct HdfIoService *service = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Playback Mute",
        .value[0] = 0,
    };
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = ReadCtrlInfo(service, elemValue.id, expectValue);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM control data which is writing invalid value of mute,so return -1.
* @tc.number  SUB_Audio_ControlHostElemWrite_read_0021
* @tc.desc  Test the ADM control data,cmdid is AUDIODRV_CTRL_IOCTRL_ELEM_WRITE and AUDIODRV_CTRL_IOCTRL_ELEM_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_ControlHostElemWrite_read_0021, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioCtlElemValue elemValue = {
        .id.cardServiceName = CARD_SEVICE_NAME.c_str(),
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC,
        .id.itemName = "Playback Mute",
        .value[0] = 2,
    };
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    ASSERT_NE(nullptr, service);
    ASSERT_NE(nullptr, service->dispatcher);
    ret = WriteCtrlInfo(service, elemValue);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS
*    that format is AUDIO_FORMAT_PCM_8_BIT.
* @tc.number  SUB_Audio_StreamHostHwParams_0001
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_8_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_PCM_16_BIT.
* @tc.number  SUB_Audio_StreamHostHwParams_0002
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_16_BIT、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0002, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 22050,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_16_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_PCM_24_BIT.
* @tc.number  SUB_Audio_StreamHostHwParams_0003
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_24_BIT、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0003, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 24000,
        .periodCount = 4, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 162140
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_PCM_32_BIT.
* @tc.number  SUB_Audio_StreamHostHwParams_0004
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_32_BIT 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0004, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 48190,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_32_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_AAC_MAIN.
* @tc.number  SUB_Audio_StreamHostHwParams_0005
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_MAIN 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0005, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 44100,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_MAIN, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_AAC_LC.
* @tc.number  SUB_Audio_StreamHostHwParams_0006
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_LC 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0006, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 8190,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_LC, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_AAC_LD.
* @tc.number  SUB_Audio_StreamHostHwParams_0007
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_LD 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0007, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 32000,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_LD, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_AAC_ELD.
* @tc.number  SUB_Audio_StreamHostHwParams_0008
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_ELD 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0008, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 50000,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_ELD, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS
*    that format is AUDIO_FORMAT_AAC_HE_V1.
* @tc.number  SUB_Audio_StreamHostHwParams_0009
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_HE_V1 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0009, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 47250,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_HE_V1, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_AAC_HE_V2.
* @tc.number  SUB_Audio_StreamHostHwParams_0010
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_AAC_HE_V2 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0010, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 47250,
        .periodCount = 8, .format = AUDIO_FORMAT_AAC_HE_V2, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8190
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM capture stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    format is AUDIO_FORMAT_PCM_24_BIT.
* @tc.number  SUB_Audio_StreamHostHwParams_0011
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_24_BIT 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0011, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 4, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS
*    that format is channels is 1.
* @tc.number  SUB_Audio_StreamHostHwParams_0012
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_24_BIT 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0012, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 1, .period = 2048, .rate = 24000,
        .periodCount = 4, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that format is channels is 8.
* @tc.number  SUB_Audio_StreamHostHwParams_0013
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0013, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 8, .period = 2048, .rate = 48000,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_8_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 32766
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that format is periodSize is 4095.
* @tc.number  SUB_Audio_StreamHostHwParams_0014
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0014, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2047, .rate = 48000,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_8_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 32766
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS
            that format is periodSize is 16385(16 * 1024)
* @tc.number  SUB_Audio_StreamHostHwParams_0015
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0015, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 8192, .rate = 48000,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_8_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 32766
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that format is periodCount is 7.
* @tc.number  SUB_Audio_StreamHostHwParams_0016
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0016, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 8, .period = 2048, .rate = 24000,
        .periodCount = 7, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that periodCount is 33.
* @tc.number  SUB_Audio_StreamHostHwParams_0017
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0017, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 8193, .rate = 24000,
        .periodCount = 33, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that startThreshold is Over value.
* @tc.number  SUB_Audio_StreamHostHwParams_0018
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0018, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 24000,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 8193
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM render stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that startThreshold is 0.
* @tc.number  SUB_Audio_StreamHostHwParams_0019
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_8_BIT 、channels is 8、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0019, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 2048, .rate = 24000,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .startThreshold = 0
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM capture stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    silenceThreshold is Less than minimum.
* @tc.number  SUB_Audio_StreamHostHwParams_0020
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_24_BIT 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0020, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 4, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 4095
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM capture stream data,cmdid is AUDIO_DRV_PCM_IOCTRL_HW_PARAMS that
*    silenceThreshold is Greater than maximum.
* @tc.number  SUB_Audio_StreamHostHwParams_0021
* @tc.desc  Test the ADM render stream data which is issuing hardware parameters that
*           format is AUDIO_FORMAT_PCM_24_BIT 、channels is 2、cardServiceName is hdf_audio_codec_dev0.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostHwParams_0021, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 32, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16385
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    EXPECT_EQ(HDF_FAILURE, ret);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream data which is sending play data stream.
* @tc.number  SUB_Audio_StreamHostWrite_0001
* @tc.desc  Test the ADM control data,cmdid is AUDIO_DRV_PCM_IOCTRL_WRITE.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostWrite_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufT = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct HdfSBuf *reply = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 4, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };

    ret = WriteFrameToSBuf(sBufT, AUDIO_FILE);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    if (ret < 0) {
        HdfSbufRecycle(sBufT);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_WRITE, sBufT, reply);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufT);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream data which is recording data stream.
* @tc.number  SUB_Audio_StreamHostRead_0001
* @tc.desc  Test the ADM control data,cmdid is AUDIO_DRV_PCM_IOCTRL_READ.
* @tc.author: zhouyongxiao
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostRead_0001, TestSize.Level1)
{
    int32_t tryNumReply = 100;
    uint32_t buffStatus = 0;
    uint32_t readSize = 0;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioXferi transfer;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    int32_t ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    struct HdfSBuf *reply = HdfSbufTypedObtainCapacity(SBUF_RAW, (AUDIO_SIZE_FRAME + AUDIO_REPLY_EXTEND));
    ASSERT_NE(nullptr, reply);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    do {
        ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_READ, nullptr, reply);
        EXPECT_EQ(HDF_SUCCESS, ret);
        EXPECT_GE(HdfSbufReadUint32(reply, &buffStatus), HDF_SUCCESS);
        if (buffStatus == CIR_BUFF_EMPTY) {
            tryNumReply--;
            HdfSbufFlush(reply);
            usleep(5000);
            continue;
        }
        break;
    } while (tryNumReply > 0);
    EXPECT_GE(tryNumReply, 0);
    ret = HdfSbufReadBuffer(reply, (const void **) & (transfer.buf), &readSize);
    EXPECT_NE(transfer.buf, nullptr);
    EXPECT_NE(readSize, (uint32_t)0);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(reply);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream function via calling prepare function(render service)
* @tc.number  SUB_Audio_StreamHostRenderPrepare_0001
* @tc.desc  Test the ADM stream function,return 0 when calling prepare function(render service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostRenderPrepare_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}

/**
* @tc.name  Test the ADM stream function via calling prepare function(capture service)
* @tc.number  SUB_Audio_StreamHostCapturePrepare_0001
* @tc.desc  Test the ADM stream function,return 0 when calling prepare function(capture service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostCapturePrepare_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);

    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream function via calling start function(render service)
* @tc.number  SUB_StreamHostRenderStart_0001
* @tc.desc  Test the ADM stream function,return 0 when calling start function(render service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_StreamHostRenderStart_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM stream function via calling start function(capture service)
* @tc.number  SUB_StreamHostCaptureStart_0001
* @tc.desc  Test the ADM stream function,return 0 when calling start function(capture service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_StreamHostCaptureStart_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling stop function(render service)
* @tc.number  SUB_Audio_StreamHostRenderStop_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling stop function(render service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostRenderStop_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 44100,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling stop function(capture service)
* @tc.number  SUB_Audio_StreamHostCaptureStop_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling stop function(capture service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostCaptureStop_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 44100,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling pause function(render service)
* @tc.number  SUB_Audio_StreamHostRenderPause_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling pause function(render service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostRenderPause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PAUSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling pause function(capture service)
* @tc.number  SUB_Audio_StreamHostCapturePause_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling pause function(capture service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostCapturePause_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PAUSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling resume function(render service)
* @tc.number  SUB_Audio_StreamHostRenderResume_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling resume function(render service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostRenderResume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_RENDER_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_RENDER_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_PAUSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_RESUME, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_RENDER_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
/**
* @tc.name  Test the ADM ctrl function via calling resume function(capture service)
* @tc.number  SUB_Audio_StreamHostCaptureResume_0001
* @tc.desc  Test the ADM ctrl function,return 0 when calling resume function(capture service)
* @tc.author: liweiming
*/
HWTEST_F(AudioAdmInterfaceTest, SUB_Audio_StreamHostCaptureResume_0001, TestSize.Level1)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBufTStop = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .period = 4096, .rate = 11025,
        .periodCount = 8, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = CARD_SEVICE_NAME.c_str(),
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16384
    };
    ret = WriteHwParams(HDF_CAPTURE_SERVICE, service, hwParams);
    ASSERT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_START, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_PAUSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_RESUME, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = WriteToSBuf(sBufTStop);
    if (ret < 0) {
        HdfIoServiceRecycle(service);
        ASSERT_EQ(HDF_SUCCESS, ret);
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP, sBufTStop, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE, nullptr, nullptr);
    EXPECT_EQ(HDF_SUCCESS, ret);
    HdfSbufRecycle(sBufTStop);
    HdfIoServiceRecycle(service);
}
}