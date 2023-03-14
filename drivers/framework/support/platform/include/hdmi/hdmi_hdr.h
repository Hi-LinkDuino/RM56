/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_HPR_H
#define HDMI_HPR_H

#include "hdf_base.h"
#include "hdmi_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* HDR(High Dynamic Range) Sysytem. */
#define HDMI_ZERO_DRMIF_SEND_TIME 2000
#define HDMI_HDR_STATE_CHANGE_TIME 500

enum HdmiHdrTimerType {
    HDMI_HDR_TIMER_ZERO_DRMIF,
    HDMI_HDR_TIMER_SDR_TO_HDR10,
    HDMI_HDR_TIMER_TYPE_BUTT
};

enum HdmiHdrState {
    HDMI_HDR_STATE_NONE,   /* HDR state change, SDR->HDR10 don't change anything */
    HDMI_HDR_STATE_OE,     /* HDR state change, SDR->HDR10 output disable and enable */
    HDMI_HDR_STATE_AVMUTE, /* HDR state change, SDR->HDR10 send avmute */
    HDMI_HDR_STATE_BUTT
};

struct HdmiHdrTimerConfig {
    enum HdmiHdrTimerType timerType;
    bool start;
    uint32_t time;
};

struct HdmiHdrInfo {
    enum HdmiHdrState state;    /* Make sure that switch smoothly from non-HDR10 to HDR10. */
    struct HdmiHdrTimerConfig zeroDrmIfTimer; /* Send zero DRM infoFrame when the HDR mode from HDR10 to non-HDR10. */
    struct HdmiHdrTimerConfig stateChangeTimer; /*  */
};

struct HdmiHdr {
    struct HdmiHdrInfo info;
    void *priv;
};

int32_t HdmiHdrAttrHandle(struct HdmiHdr *hdr, struct HdmiHdrAttr *curAttr);
int32_t HdmiHdrDrmInfoFrameStop(struct HdmiHdr *hdr);
int32_t HdmiHdrModeChangeTimeout(struct HdmiHdr *hdr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_HPR_H */
