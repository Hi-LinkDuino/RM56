/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_FRL_H
#define HDMI_FRL_H

#include "hdf_base.h"
#include "osal_time.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* FRL(Fix Rate Link) */
enum HdmiFrlMode {
    HDMI_FRL_MODE_TMDS = 0,
    HDMI_FRL_MODE_FRL = 1,
    HDMI_FRL_MODE_BUTT,
};

#define HDMI_FRL_3_LANES 3
#define HDMI_FRL_4_LANES 4

enum HdmiFrlWorkMode {
    HDMI_FRL_WORK_MODE_NONE = 0,
    HDMI_FRL_WORK_MODE_3L3G = 1,  /* 3 lanes, 3Gbps per lane */
    HDMI_FRL_WORK_MODE_3L6G = 2,  /* 3 lanes, 6Gbps per lane */
    HDMI_FRL_WORK_MODE_4L6G = 3,  /* 4 lanes, 6Gbps per lane */
    HDMI_FRL_WORK_MODE_4L8G = 4,  /* 4 lanes, 8Gbps per lane */
    HDMI_FRL_WORK_MODE_4L10G = 5, /* 4 lanes, 10Gbps per lane */
    HDMI_FRL_WORK_MODE_4L12G = 6, /* 4 lanes, 12Gbps per lane */
    HDMI_FRL_WORK_MODE_BUTT,
};

enum HdmiFrlBitRate {
    HDMI_FRL_BIT_RATE_3 = 3,
    HDMI_FRL_BIT_RATE_6 = 6,
    HDMI_FRL_BIT_RATE_8 = 8,
    HDMI_FRL_BIT_RATE_10 = 10,
    HDMI_FRL_BIT_RATE_12 = 12,
    HDMI_FRL_BIT_RATE_BUTT,
};

enum HdmiFrlTrainPattern {
    HDMI_FRL_TRAIN_PATTERN_NULL = 0,
    HDMI_FRL_TRAIN_PATTERN_LP1 = 1,
    HDMI_FRL_TRAIN_PATTERN_LP2 = 2,
    HDMI_FRL_TRAIN_PATTERN_LP3 = 3,
    HDMI_FRL_TRAIN_PATTERN_LP4 = 4,
    HDMI_FRL_TRAIN_PATTERN_LP5 = 5,
    HDMI_FRL_TRAIN_PATTERN_LP6 = 6,
    HDMI_FRL_TRAIN_PATTERN_LP7 = 7,
    HDMI_FRL_TRAIN_PATTERN_LP8 = 8,
    HDMI_FRL_TRAIN_PATTERN_REV = 9,
    HDMI_FRL_TRAIN_PATTERN_0E = 14,
    HDMI_FRL_TRAIN_PATTERN_0F = 15,
    HDMI_FRL_TRAIN_PATTERN_BUTT,
};

enum HdmiFrlTrainStatus {
    HDMI_FRL_TRAIN_STATUS_NULL = 0,
    HDMI_FRL_TRAIN_STATUS_FAIL = 1,
    HDMI_FRL_TRAIN_STATUS_SUCC = 2,
    HDMI_FRL_TRAIN_STATUS_BUSY = 3,
    HDMI_FRL_TRAIN_STATUS_BUTT,
};

enum HdmiFrlTrainingFailReason {
    HDMI_FRL_TRAIN_FAIL_NORMAL,
    HDMI_FRL_TRAIN_FAIL_FLT_TIMEOUT,
    HDMI_FRL_TRAIN_FAIL_FLT_STEP_TIMEOUT,
    HDMI_FRL_TRAIN_FAIL_RATE_CHANGE,
    HDMI_FRL_TRAIN_FAIL_FFE_CHANGE,
    HDMI_FRL_TRAIN_FAIL_BUTT
};

enum HdmiFrlTrainStep {
    HDMI_FRL_TRAIN_STEP_READR_CHECK = 0,
    HDMI_FRL_TRAIN_STEP_TRAIN_START = 1,
    HDMI_FRL_TRAIN_STEP_RESULT_CHECK = 2,
    HDMI_FRL_TRAIN_STEP_RATE_CHANGE = 3,
    HDMI_FRL_TRAIN_STEP_RESULT_HANDLE = 4,
    HDMI_FRL_TRAIN_STEP_RETRAIN_CHECK = 5,
    HDMI_FRL_TRAIN_STEP_TRAIN_STOP = 6,
    HDMI_FRL_TRAIN_STEP_BUTT,
};

enum HdmiFrlRateSelect {
    HDMI_FRL_RATE_LITTLE = 0,
    HDMI_FRL_RATE_BIG = 1,
    HDMI_FRL_RATE_BUTT,
};

enum HdmiFrlPixelFormatMode {
    HDMI_FRL_PIXEL_FORMAT_MODE_0 = 0,
    HDMI_FRL_PIXEL_FORMAT_MODE_1 = 1,
    HDMI_FRL_PIXEL_FORMAT_MODE_2 = 2,
    HDMI_FRL_PIXEL_FORMAT_MODE_3 = 3,
};

enum HdmiFrlBypass {
    HDMI_FRL_BYPASS_READ_CHECK = 0x01,
    HDMI_FRL_BYPASS_RESULT_CHECK = 0x02,
    HDMI_FRL_BYPASS_RETRAIN_CHECK = 0x04,
};

/*
 * Feed Forward Equalizer(FFE)
 * Tx terminal will use 0=TxFFE0 FFE in Link Training. If higher rate signal needs to be transmitted,
 * Tx will decide higher FFE compensation through Link Training to ensure
 * that video and audio data can be completely transmitted to the Sink terminal.
 */
enum HdmiFrlTxffeMode {
    HDMI_FRL_TXFFE_MODE_0 = 0,
    HDMI_FRL_TXFFE_MODE_1 = 1,
    HDMI_FRL_TXFFE_MODE_2 = 2,
    HDMI_FRL_TXFFE_MODE_3 = 3,
    HDMI_FRL_TXFFE_MODE_BUTT,
};

enum HdmiFrlStrategyMode {
    HDMI_FRL_STRATEGY_MODE_1,
    HDMI_FRL_STRATEGY_MODE_2,
    HDMI_FRL_STRATEGY_MODE_3,
    HDMI_FRL_STRATEGY_MODE_BUTT
};

enum HdmiFrlStateMachineRunningState {
    HDMI_FRL_STATE_MACHINE_NULL = 0,
    HDMI_FRL_STATE_MACHINE_START = 1,
    HDMI_FRL_STATE_MACHINE_STOP = 2,
};

struct HdmiFrlStateMachineInfo {
    bool start;
    enum HdmiFrlTrainStep trainingState;
    enum HdmiFrlStateMachineRunningState machineState;
    uint64_t startTime;        /* ms */
    uint32_t waitReadyTime;    /* ms */
    uint32_t waitHandleTime;   /* ms */
    uint32_t waitRetrainTime;  /* ms */
    uint32_t trainTimeout;     /* ms */
    uint32_t timeoutCnt;
};

struct HdmiFrlInfo {
    uint8_t curFrlRate;
    uint8_t minFrlRate;
    uint8_t maxFrlRate;
    uint8_t preFrlRate;
    uint32_t trainFailCnt;
    uint32_t trainMaxFailTimes;
    uint32_t tmdsClock;
    enum HdmiFrlMode perMode;
    enum HdmiFrlMode mode;
    enum HdmiFrlStrategyMode strategy;
    enum HdmiFrlRateSelect rateSelect;
    bool scdc;
    bool ctsMode;
    bool start;
    bool work;
    struct HdmiFrlStateMachineInfo machineInfo;
    uint32_t trainingFailCnt;
    uint32_t trainingMaxFailTimes;
};

struct HdmiFrl {
    struct HdmiFrlInfo info;
    void *priv;
};

struct HdmiFrlAudioNctsConfig {
    enum HdmiSampleRate sampleRate;
    enum HdmiFrlMode mode;
    uint8_t frlRate;
    uint32_t pixelClk;
    uint32_t n;
    uint32_t cts;
};

struct HdmiFrlTrainConfig {
    bool frlNoTimeout;
    uint8_t frlRate;
    uint8_t txffe;
    uint32_t trainTimeout;
};

struct HdmiFrlTrainRslt {
    enum HdmiFrlTrainStatus status;
    enum HdmiFrlTrainPattern pattern[HDMI_FRL_4_LANES];
    enum HdmiFrlTrainingFailReason failReason;
};

void HdmiFrlEnable(struct HdmiFrl *frl, bool enable);
bool HdmiFrlSupport(struct HdmiFrl *frl);
int32_t HdmiFrlModeSelect(struct HdmiFrl *frl);
bool HdmiFrlModeChanged(struct HdmiFrl *frl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_FRL_H */
