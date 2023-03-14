/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_SCDC_H
#define HDMI_SCDC_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* SCDC(Status and Control Data Channel) */
#define HDMI_SCDC_HDMI20_VERSION 1
#define HDMI_SCDC_DEFAULT_SCRAMBLE_TIMEOUT 200  /* ms */

/*
 * SCDCS(Status and Control Data Channel Structure).
 * see hdmi2.0 spec table 10-15.
 */
enum HdmiScdcsOffset {
    HDMI_SCDCS_OFFSET_SINK_VERSION = 0x01,
    HDMI_SCDCS_OFFSET_SOURCE_VERSION = 0x02,
    HDMI_SCDCS_OFFSET_UPDATE_0 = 0x10,
    HDMI_SCDCS_OFFSET_UPDATE_1 = 0x11,
    HDMI_SCDCS_OFFSET_TMDS_CONFIG = 0x20,
    HDMI_SCDCS_OFFSET_SCRAMBLER_STATUS = 0x21,
    HDMI_SCDCS_OFFSET_CONFIG_0 = 0x30,
    HDMI_SCDCS_OFFSET_CONFIG_1 = 0x31,
    HDMI_SCDCS_OFFSET_TEST_CONFIG_1 = 0x35,
    HDMI_SCDCS_OFFSET_STASTUS_FLAG_0 = 0x40,
    HDMI_SCDCS_OFFSET_STASTUS_FLAG_1 = 0x41,
    HDMI_SCDCS_OFFSET_STASTUS_FLAG_2 = 0x42,
    HDMI_SCDCS_OFFSET_ERR_DET_0_L = 0x50,
    HDMI_SCDCS_OFFSET_ERR_DET_0_H = 0x51,
    HDMI_SCDCS_OFFSET_ERR_DET_1_L = 0x52,
    HDMI_SCDCS_OFFSET_ERR_DET_1_H = 0x53,
    HDMI_SCDCS_OFFSET_ERR_DET_2_L = 0x54,
    HDMI_SCDCS_OFFSET_ERR_DET_2_H = 0x55,
    HDMI_SCDCS_OFFSET_ERR_DET_CHECKSUM = 0x56,
    HDMI_SCDCS_OFFSET_TEST_CONFIG_0 = 0xC0,
    HDMI_SCDCS_OFFSET_IEEE_OUI_3TH = 0xD0,
    HDMI_SCDCS_OFFSET_IEEE_OUI_2ND = 0xD1,
    HDMI_SCDCS_OFFSET_IEEE_OUI_1ST = 0xD2,
    HDMI_SCDCS_OFFSET_DEVICE_ID_START = 0xD3,
    HDMI_SCDCS_OFFSET_DEVICE_ID_END = 0xDD,
    HDMI_SCDCS_OFFSET_SPECIFIC_START = 0xDE,
    HDMI_SCDCS_OFFSET_SPECIFIC_END = 0xFF,
};

/*
 * see HDMI2.1 section 10.4.1.3.
 * All update flags are readable and writable.
 * The purpose of these flags is to provide a mechanism for the sink device to efficiently inform the source device
 * that additional source device action may be required.
 */
union HdmiScdcsUpdateFlag0 {
    uint8_t data;
    struct {
        uint8_t statusUpdate : 1; /* The sink shall set this bit when value is changed in the Status Flag register. */
        uint8_t cedUpdate : 1;    /* ced(character error detection) register. */
        uint8_t rrTest : 1;       /* rr(Read Request) */
        uint8_t srcTestUpdate : 1;
        uint8_t frlStart : 1;
        uint8_t fltUpdate : 1;
        uint8_t rsedUpdate : 1;
        uint8_t reserved : 1;
    } bits;
};

#define HDMI_SCDC_STATUS_UPDATE_MARK (1 << 0)
#define HDMI_SCDC_CED_UPDATE_MARK (1 << 1)
#define HDMI_SCDC_RR_TEST_MARK (1 << 2)
#define HDMI_SCDC_SRC_TEST_UPDATE_MARK (1 << 3)
#define HDMI_SCDC_FRL_START_MARK (1 << 4)
#define HDMI_SCDC_FLT_UPDATE_MARK (1 << 5)
#define HDMI_SCDC_RSED_UPDATE_MARK (1 << 6)

/*
 * see HDMI2.1 section 10.4.1.4.
 * All Configuration fields are readable and writable.
 */
union HdmiScdcsTmdsConfig {
    uint8_t data;
    struct {
        uint8_t scramblingEnable : 1;
        uint8_t tmdsBitClockRatio : 1; /*
                                        * 0: TMDS_Bit_Period/TMDS_Clock_Period is 1/10;
                                        * 1: TMDS_Bit_Period/TMDS_Clock_Period is 1/40.
                                        */
        uint8_t reserved : 6;
    } bits;
};

/*
 * see HDMI2.1 section 10.4.1.5.
 * The Status Flags are all Read Only.
 */
union HdmiScdcsScramblerStatus {
    uint8_t data;
    struct {
        uint8_t scramblingStatus : 1;
        uint8_t reserved : 7;
    } bits;
};

/*
 * see hdmi2.0 spec table 10-21.
 * All Configuration fields are readable and writable.
 */
union HdmiScdcsConfig0 {
    uint8_t data;
    struct {
        uint8_t rrEnable : 1; /*
                               * The source set this bit when the source supports Read Request.
                               * The sink shall reset this bit when SCDC of the sink goes from the disabled to enabled.
                               */
        uint8_t fltNoRetrain : 1;
        uint8_t reserved : 6;
    } bits;
};

/*
 * see HDMI2.1 section 10.4.1.6.
 */
union HdmiScdcsConfig1 {
    uint8_t data;
    struct {
        uint8_t frlRate : 4;  /*
                               * Source select the FRL rate and Lane count by writing into this register.
                               * This field is written by the Source during the Link Training protocol.
                               */
        uint8_t ffeLevels : 4;
    } bits;
};

/*
 * see HDMI2.1 section 10.4.1.6.
 */
union HdmiScdcsTestConfig1 {
    uint8_t data;
    struct {
        uint8_t reserved0 : 1;
        uint8_t preShootIOnly : 1;
        uint8_t deEmphasisOnly : 1;
        uint8_t noFfe : 1;
        uint8_t reserved1 : 1;
        uint8_t fltNoTimeout : 1;
        uint8_t dscFrlMax : 1;
        uint8_t frlMax : 1;
    } bits;
};

#define HDMI_SCDC_PRE_SHOOT_ONLY_MARK (1 << 1)
#define HDMI_SCDC_DE_EMPHASIS_ONLY_MARK (1 << 2)
#define HDMI_SCDC_NO_FFE_MARK (1 << 3)
#define HDMI_SCDC_FLT_NO_TIMEOUT_MARK (1 << 5)
#define HDMI_SCDC_DSC_FRL_MAX_MARK (1 << 6)
#define HDMI_SCDC_FRL_MAX_MARK (1 << 7)

/*
 * see HDMI2.1 section 10.4.1.7.
 * The Status Flags are all Read Only.
 */
union HdmiScdcsStatusFlag0 {
    uint8_t data;
    struct {
        uint8_t clockDetected : 1; /* This bit shall be set by the sink when the sink detects a valid clock signal. */
        uint8_t ch0Locked : 1; /*
                                * This bit shall be set by the sink when the sink is successfully decoding
                                * data on HDMI Channel 0.
                                */
        uint8_t ch1Locked : 1; /*
                                * This bit shall be set by the sink when the sink is successfully decoding
                                * data on HDMI Channel 1.
                                */
        uint8_t ch2Locked : 1; /*
                                * This bit shall be set by the sink when the sink is successfully decoding
                                * data on HDMI Channel 2.
                                */
        uint8_t ch3Locked : 1;
        uint8_t reserved : 1;
        uint8_t fltReady : 1;
        uint8_t dscDecodeFail : 1;
    } bits;
};

#define HDMI_SCDC_CLOCK_DETECTED_MARK (1 << 0)
#define HDMI_SCDC_CH0_LOCKED_MARK (1 << 1)
#define HDMI_SCDC_CH1_LOCKED_MARK (1 << 2)
#define HDMI_SCDC_CH2_LOCKED_MARK (1 << 3)
#define HDMI_SCDC_CH3_LOCKED_MARK (1 << 4)
#define HDMI_SCDC_FLT_READY_MARK (1 << 6)
#define HDMI_SCDC_DSC_DECODE_FAIL_MARK (1 << 7)

/*
 * see HDMI2.1 section 10.4.1.7.
 * The Status Flags are all Read Only.
 */
union HdmiScdcsStatusFlag1 {
    uint8_t data;
    struct {
        uint8_t ln0LtpReq : 4;
        uint8_t ln1LtpReq : 4;
    } bits;
};

/*
 * see HDMI2.1 section 10.4.1.7.
 * The Status Flags are all Read Only.
 */
union HdmiScdcsStatusFlag2 {
    uint8_t data;
    struct {
        uint8_t ln2LtpReq : 4;
        uint8_t ln3LtpReq : 4;
    } bits;
};

/*
 * see hdmi2.0 spec table 10-23. Offeset 0x50~0x55.
 * The Character Error Detection counters are not writable by the source and are cleared on read by the source.
 */
union HdmiScdcsCharacterErrorDetection {
    uint16_t data;
    struct {
        uint16_t chErrCntLower : 8;  /* Channel 0/1/2 Erroe Count bits 7->0 */
        uint16_t chErrCntUpper : 7;  /* Channel 0/1/2 Erroe Count bits 14->8 */
        uint16_t chValid : 1;        /* Channel 0/1/2 valid flag */
    } bits;
};

/*
 * see hdmi2.0 spec table 10-24.
 * The Test Configuration registers are privided to facilitate compliance testing.
 */
union HdmiScdcsTestConfig0 {
    uint8_t data;
    struct {
        uint8_t testReadRequestDelay : 7;
        uint8_t testReadRequest : 1; /* Read/Write */
    } bits;
};

struct HdmiScdcsRegStatus {
    uint8_t sinkVersion;
    uint8_t sourceVersion;               /* R/W */
    union HdmiScdcsUpdateFlag0 upfate0;  /* R/W */
    union HdmiScdcsTmdsConfig tmdsCfg;   /* R/W */
    union HdmiScdcsScramblerStatus scramblerStatus;
    union HdmiScdcsConfig0 cfg0;         /* R/W */
    union HdmiScdcsStatusFlag0 statusFlag0;
    union HdmiScdcsCharacterErrorDetection errDet0;
    union HdmiScdcsCharacterErrorDetection errDet1;
    union HdmiScdcsCharacterErrorDetection errDet2;
    uint8_t errDetChecksum;
    union HdmiScdcsTestConfig0 testCfg0;  /* R/W */
    uint8_t ieeeOui[HDMI_SCDCS_OFFSET_IEEE_OUI_1ST - HDMI_SCDCS_OFFSET_IEEE_OUI_3TH + 1];
    uint8_t deviceId[HDMI_SCDCS_OFFSET_DEVICE_ID_END - HDMI_SCDCS_OFFSET_DEVICE_ID_START + 1];
};

struct HdmiScdcScrambleCap {
    bool sinkScramble;
    bool sourceScramble;
    bool tmdsBitClockRatio40;
};

struct HdmiScdcAttribute {
    bool sinkReadRequest;
    bool srcScramble;
    bool sinkScramble;
    bool tmdsBitClockRatio40;
    uint32_t ScrambleTimeOut;
    uint32_t ScrambleInterval;
};

struct HdmiScdc {
    struct HdmiScdcsRegStatus status;
    struct HdmiScdcAttribute attr;
    void *priv;
};

/* used for frl. */
enum HdmiScdcOptMsg {
    HDMI_SCDC_OPT_SET_SOURCE_VER = 0,
    HDMI_SCDC_OPT_GET_SOURCE_VER = 1,
    HDMI_SCDC_OPT_GET_SINK_VER = 2,
    HDMI_SCDC_OPT_SET_FLT_UPDATE = 3,
    HDMI_SCDC_OPT_GET_FLT_UPDATE = 4,
    HDMI_SCDC_OPT_SET_FRL_START = 5,
    HDMI_SCDC_OPT_GET_FRL_START = 6,
    HDMI_SCDC_OPT_SET_CONFIG1 = 7,
    HDMI_SCDC_OPT_GET_CONFIG1 = 8,
    HDMI_SCDC_OPT_GET_TEST_CONFIG_1 = 9,
    HDMI_SCDC_OPT_GET_FLT_READY = 10,
    HDMI_SCDC_OPT_GET_LTP_REQ = 11,
    HDMI_SCDC_OPT_BUTT,
};

int32_t HdmiScdcScrambleSet(struct HdmiScdc *scdc, struct HdmiScdcScrambleCap *scramble);
int32_t HdmiScdcScrambleGet(struct HdmiScdc *scdc, struct HdmiScdcScrambleCap *scramble);
bool HdmiScdcSinkSupport(struct HdmiScdc *scdc);
void HdmiScdcReset(struct HdmiScdc *scdc);
int32_t HdmiScdcOptMsgHandle(struct HdmiScdc *scdc, enum HdmiScdcOptMsg msg, uint8_t *buffer, uint32_t len);
int32_t HdmiScdcFillScrambleCap(struct HdmiScdc *scdc, struct HdmiScdcScrambleCap *scramble,
    enum HdmiTmdsModeType *tmdsMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_SCDC_H */
