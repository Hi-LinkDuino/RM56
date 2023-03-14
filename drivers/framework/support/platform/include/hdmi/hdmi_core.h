/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_CORE_H
#define HDMI_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdmi_cec.h"
#include "hdmi_common.h"
#include "hdmi_ddc.h"
#include "hdmi_edid.h"
#include "hdmi_event.h"
#include "hdmi_frl.h"
#include "hdmi_hdcp.h"
#include "hdmi_hdr.h"
#include "hdmi_if.h"
#include "hdmi_infoframe.h"
#include "hdmi_scdc.h"
#include "osal_mutex.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HDMI_ONE_BYTE_MARK 0xFF
#define HDMI_ONE_BYTE_SHIFT 8
#define HDMI_TWO_BYTES_SHIFT 16

struct HdmiPhyCfg {
    uint32_t pixelClk;
    uint32_t tmdsClk;               /* TMDS colck, KHz */
    enum HdmiDeepColor deepColor;        /* deep color(color depth) */
    enum HdmiPhyModeCfg modeCfg;    /* TMDS/FRL/tx_ffe */
    enum HdmiColorSpace colorSpace;
    enum HdmiFrlWorkMode rate;   /* lane and rate */
    enum HdmiFrlTxffeMode txffe[4]; /* tx_ffe */
};

struct HdmiTmdsConfig {
    uint32_t pixelClk;
    uint32_t tmdsClk;
    enum HdmiPhyModeCfg mode;
    enum HdmiDeepColor deepColor;
};

struct HdmiCntlr;

union HdmiCap {
    uint32_t data;
    struct CapBitsData {
        uint32_t scdc : 1;            /* bit0: support scdc */
        uint32_t frl : 1;             /* bit1: support frl */
        uint32_t hdr : 1;             /* bit2: support hdr */
        uint32_t hdcp : 1;            /* bit3: support hdcp */
        uint32_t cec : 1;             /* bit4: support cec */
        uint32_t hdmi14 : 1;          /* bit5: support hdmi1.4 spec */
        uint32_t hdmi20 : 1;          /* bit6: support hdmi2.0 spec */
        uint32_t hdmi21 : 1;          /* bit7: support hdmi2.1 spec */
        uint32_t hdcp14 : 1;          /* bit8: support hdcp1.4 spec */
        uint32_t hdcp22 : 1;          /* bit9: support hdcp2.2 spec */
        uint32_t rgb444 : 1;          /* bit10: support rgb444 */
        uint32_t ycbcr444 : 1;        /* bit11: support ycbcr444 */
        uint32_t ycbcr422 : 1;        /* bit12: support ycbcr422 */
        uint32_t ycbcr420 : 1;        /* bit13: support ycbcr420 */
        uint32_t deepColor10bits : 1; /* bit14: support deep color 10 bits */
        uint32_t deepColor12bits : 1; /* bit15: support deep color 12 bits */
        uint32_t deepColor16bits : 1; /* bit16: support deep color 16 bits */
        uint32_t scramble : 1;        /* bit17: support scramble */
        uint32_t cecRc : 1;           /* bit18: support CEC Remote Control */
        uint32_t rev : 13;            /* bit21~31: reserved */
    } bits;
};

enum HdmiCapMark {
    HDMI_CAP_SCDC_MARK = (1 << 0),
    HDMI_CAP_FRL_MARK = (1 << 1),
    HDMI_CAP_HDR_MARK = (1 << 2),
    HDMI_CAP_HDCP_MARK = (1 << 3),
    HDMI_CAP_CEC_MARK = (1 << 4),
    HDMI_CAP_HDMI14_MARK = (1 << 5),
    HDMI_CAP_HDMI20_MARK = (1 << 6),
    HDMI_CAP_HDMI21_MARK = (1 << 7),
    HDMI_CAP_HDCP14_MARK = (1 << 8),
    HDMI_CAP_HDCP22_MARK = (1 << 9),
    HDMI_CAP_RGB444_MARK = (1 << 10),
    HDMI_CAP_YCBCR444_MARK = (1 << 11),
    HDMI_CAP_YCBCR422_MARK = (1 << 12),
    HDMI_CAP_YCBCR420_MARK = (1 << 13),
    HDMI_CAP_DEEP_COLOR_10BITES_MARK = (1 << 14),
    HDMI_CAP_DEEP_COLOR_12BITES_MARK = (1 << 15),
    HDMI_CAP_DEEP_COLOR_16BITES_MARK = (1 << 16),
    HDMI_CAP_SCRAMBLE_MARK = (1 << 17),
};

struct HdmiCntlrCap {
    union HdmiCap baseCap;
    uint32_t maxTmdsClock;
    uint32_t defTmdsClock; /* unit: MHz */
    uint32_t maxFrlRate;
    uint32_t videoTiming;
    uint32_t quantization;
    uint32_t colorSpace;
    uint32_t colorimetry;
    uint32_t audioIfType;
    uint32_t audioBitDepth;
    uint32_t audioSampleRate;
    uint32_t audioChannels;
    uint32_t hdrColorimetry;
    uint32_t hdrUserMode;
};

struct HdmiHardwareStatus {
    struct HdmiCommonStatus commonStatus;
    struct HdmiAudioStatus audioStatus;
    struct HdmiVideoStatus videoStatus;
    struct HdmiInfoFrameStatus infoFrameStatus;
    struct HdmiHdcpStatus hdcpstatus;
};

struct HdmiAudioConfigInfo {
    bool enable;
    bool downSample;
    uint32_t tmdsClock;
    uint32_t pixelRepeat;
    enum HdmiAudioInterfaceType ifType;
    enum HdmiAudioBitDepth bitDepth;
    enum HdmiSampleRate sampleRate;
    enum HdmiAudioFormatChannel channels;
};

struct HdmiCntlrOps {
    void (*hardWareInit)(struct HdmiCntlr *cntlr);
    void (*hardWareStatusGet)(struct HdmiCntlr *cntlr, struct HdmiHardwareStatus *status);
    void (*controllerReset)(struct HdmiCntlr *cntlr);
    bool (*hotPlugStateGet)(struct HdmiCntlr *cntlr);
    bool (*hotPlugInterruptStateGet)(struct HdmiCntlr *cntlr);
    void (*lowPowerSet)(struct HdmiCntlr *cntlr, bool enable);
    void (*tmdsModeSet)(struct HdmiCntlr *cntlr, enum HdmiTmdsModeType mode);
    int32_t (*tmdsConfigSet)(struct HdmiCntlr *cntlr, struct HdmiTmdsConfig mode);
    void (*infoFrameEnable)(struct HdmiCntlr *cntlr, enum HdmiPacketType infoFrameType, bool enable);
    int32_t (*infoFrameSend)(struct HdmiCntlr *cntlr, enum HdmiPacketType infoFrameType, uint8_t *data, uint32_t len);
    int32_t (*infoFrameDataSet)(struct HdmiCntlr *cntlr, uint32_t type, uint8_t *data, uint32_t len);
    int32_t (*cecMsgSend)(struct HdmiCntlr *cntlr, struct HdmiCecMsg *msg);
    void (*audioPathEnable)(struct HdmiCntlr *cntlr, bool enable);
    void (*audioPathSet)(struct HdmiCntlr *cntlr, struct HdmiAudioConfigInfo *config);
    void (*phyOutputEnable)(struct HdmiCntlr *cntlr, bool enable);
    void (*phyOutputSet)(struct HdmiCntlr *cntlr, struct HdmiPhyCfg *cfg);
    void (*blackDataSet)(struct HdmiCntlr *cntlr, bool enable);
    void (*videoMuteEnable)(struct HdmiCntlr *cntlr, bool enable);
    void (*videoPathSet)(struct HdmiCntlr *cntlr, struct HdmiVideoAttr *attr);
    void (*audioMuteEnable)(struct HdmiCntlr *cntlr, bool enable);
    void (*avmuteSet)(struct HdmiCntlr *cntlr, bool enable);
    int32_t (*ddcTransfer)(struct HdmiCntlr *cntlr, struct HdmiDdcCfg *ddcCfg);
    bool (*scdcSourceScrambleGet)(struct HdmiCntlr *cntlr);
    int32_t (*scdcSourceScrambleSet)(struct HdmiCntlr *cntlr, bool enable);
    void (*frlSet)(struct HdmiCntlr *cntlr);
    int32_t (*frlEnable)(struct HdmiCntlr *cntlr, bool enable);
    int32_t (*audioNctsSet)(struct HdmiCntlr *cntlr, struct HdmiFrlAudioNctsConfig *cfg);
    void (*frlTrainingConfigSet)(struct HdmiCntlr *cntlr, struct HdmiFrlTrainConfig *cfg);
    void (*frlTrainingStart)(struct HdmiCntlr *cntlr);
    void (*frlGetTriningRslt)(struct HdmiCntlr *cntlr, struct HdmiFrlTrainRslt *rslt);
    void (*hdcpRegInit)(struct HdmiCntlr *cntlr);
    int32_t (*hdcpGenerateAksvAndAn)(struct HdmiCntlr *cntlr);
    int32_t (*hdcpOptReg)(struct HdmiCntlr *cntlr, enum HdmiHdcpRegOptType type, uint8_t *data, uint32_t len);
    void (*hdrTimerSet)(struct HdmiCntlr *cntlr, struct HdmiHdrTimerConfig *config);
};

enum HdmiCntlrState {
    HDMI_CNTLR_STATE_NONE = 0,
    HDMI_CNTLR_STATE_OPEN = (1 << 0),
    HDMI_CNTLR_STATE_START = (1 << 1),
    HDMI_CNTLR_STATE_STOP = (1 << 2),
    HDMI_CNTLR_STATE_CLOSE = (1 << 3),
};

struct HdmiDevice {
    struct HdmiEdid edid;    /* device cap */
    struct HdmiCntlr *cntlr;
    void *priv;
};

struct HdmiAttr {
    struct HdmiCommonAttr commAttr;
    struct HdmiAudioAttr audioAttr;
    struct HdmiVideoAttr videoAttr;
    struct HdmiHdrAttr hdrAttr;
};

struct HdmiCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *hdfDevObj;
    struct PlatformDevice device;
    struct OsalMutex mutex;
    struct PlatformQueue *msgQueue;
    struct HdmiCntlrCap cap;
    struct HdmiAttr attr;
    struct HdmiCntlrOps *ops;
    uint32_t deviceIndex;
    uint32_t state;                 /* cntlr state. */
    enum HdmiTmdsModeType tmdsMode;
    struct HdmiDevice *hdmi;
    struct HdmiInfoFrame infoFrame;
    struct HdmiScdc *scdc;
    struct HdmiDdc ddc;
    struct HdmiFrl *frl;
    struct HdmiHdcp *hdcp;
    struct HdmiCec *cec;
    struct HdmiEvent event;
    struct HdmiHdr *hdr;
    void *priv;
};

static inline void HdmiCntlrLock(struct HdmiCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexLock(&cntlr->mutex);
    }
}

static inline void HdmiCntlrUnlock(struct HdmiCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexUnlock(&cntlr->mutex);
    }
}

static inline struct HdmiCntlr *HdmiCntlrGetByBusNum(uint16_t num)
{
    struct PlatformDevice *device = PlatformManagerGetDeviceByNumber(PlatformManagerGet(PLATFORM_MODULE_HDMI), num);

    if (device == NULL) {
        return NULL;
    }
    return CONTAINER_OF(device, struct HdmiCntlr, device);
}

static inline void HdmiCntlrLowPowerSet(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->lowPowerSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->lowPowerSet(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrHardWareInit(struct HdmiCntlr *cntlr)
{
    if (cntlr->ops != NULL && cntlr->ops->hardWareInit != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->hardWareInit(cntlr);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrAudioPathEnable(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->audioPathEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->audioPathEnable(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrAudioPathSet(struct HdmiCntlr *cntlr, struct HdmiAudioConfigInfo *config)
{
    if (cntlr->ops != NULL && cntlr->ops->audioPathSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->audioPathSet(cntlr, config);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrAudioMuteEnable(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->audioMuteEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->audioMuteEnable(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrVideoPathSet(struct HdmiCntlr *cntlr, struct HdmiVideoAttr *attr)
{
    if (cntlr->ops != NULL && cntlr->ops->videoPathSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->videoPathSet(cntlr, attr);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrTmdsModeSet(struct HdmiCntlr *cntlr, enum HdmiTmdsModeType mode)
{
    if (cntlr->ops != NULL && cntlr->ops->tmdsModeSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->tmdsModeSet(cntlr, mode);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrReset(struct HdmiCntlr *cntlr)
{
    if (cntlr->ops != NULL && cntlr->ops->controllerReset != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->controllerReset(cntlr);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrBlackDataSet(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->blackDataSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->blackDataSet(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrPhyOutputEnablet(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->phyOutputEnable != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->phyOutputEnable(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrHdrTimerSet(struct HdmiCntlr *cntlr, struct HdmiHdrTimerConfig *config)
{
    if (cntlr->ops != NULL && cntlr->ops->hdrTimerSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->hdrTimerSet(cntlr, config);
        HdmiCntlrUnlock(cntlr);
    }
}

static inline void HdmiCntlrAvmuteSet(struct HdmiCntlr *cntlr, bool enable)
{
    if (cntlr->ops != NULL && cntlr->ops->avmuteSet != NULL) {
        HdmiCntlrLock(cntlr);
        cntlr->ops->avmuteSet(cntlr, enable);
        HdmiCntlrUnlock(cntlr);
    }
}

int32_t HdmiCntlrParse(struct HdmiCntlr *cntlr, struct HdfDeviceObject *obj);
int32_t HdmiCntlrAdd(struct HdmiCntlr *cntlr);
void HdmiCntlrRemove(struct HdmiCntlr *cntlr);

int32_t HdmiCntlrOpen(struct HdmiCntlr *cntlr);
int32_t HdmiCntlrGetSinkEdid(struct HdmiCntlr *cntlr, uint8_t *buffer, uint32_t len);
int32_t HdmiCntlrStart(struct HdmiCntlr *cntlr);
int32_t HdmiCntlrStop(struct HdmiCntlr *cntlr);
int32_t HdmiCntlrDeepColorSet(struct HdmiCntlr *cntlr, enum HdmiDeepColor color);
int32_t HdmiCntlrDeepColorGet(struct HdmiCntlr *cntlr, enum HdmiDeepColor *color);
int32_t HdmiCntlrSetVideoAttribute(struct HdmiCntlr *cntlr, struct HdmiVideoAttr *attr);
int32_t HdmiCntlrSetAudioAttribute(struct HdmiCntlr *cntlr, struct HdmiAudioAttr *attr);
int32_t HdmiCntlrSetHdrAttribute(struct HdmiCntlr *cntlr, struct HdmiHdrAttr *attr);
int32_t HdmiCntlrInfoFrameGet(struct HdmiCntlr *cntlr, enum HdmiPacketType type, union HdmiInfoFrameInfo *frame);
int32_t HdmiCntlrInfoFrameSet(struct HdmiCntlr *cntlr, enum HdmiPacketType type, union HdmiInfoFrameInfo *frame);
int32_t HdmiCntlrRegisterHpdCallbackFunc(struct HdmiCntlr *cntlr, struct HdmiHpdCallbackInfo *callback);
int32_t HdmiCntlrUnregisterHpdCallbackFunc(struct HdmiCntlr *cntlr);
void HdmiCntlrClose(struct HdmiCntlr *cntlr);

bool HdmiHpdStatusDelayGet(struct HdmiCntlr *cntlr);
bool HdmiHpdStatusGet(struct HdmiCntlr *cntlr);
int32_t HdmiEventHandle(struct HdmiCntlr *cntlr, enum HdmiEventType event, void *data);
int32_t HdmiEventMsgHandleDefault(struct PlatformQueue *queue, struct PlatformMsg *msg);

bool HdmiEdidSupportFrl(struct HdmiDevice *hdmi);
uint8_t HdmiEdidGetMaxFrlRate(struct HdmiDevice *hdmi);
bool HdmiEdidScdcSupport(struct HdmiDevice *hdmi);
int32_t HdmiCntlrAllocDev(struct HdmiCntlr *cntlr);
void HdmiCntlrFreeDev(struct HdmiCntlr *cntlr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_CORE_H */
