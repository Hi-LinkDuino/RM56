/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_EVENT_H
#define HDMI_EVENT_H

#include "hdf_base.h"
#include "hdmi_if.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* HPD(hot plug detect) */
#define HDMI_READ_HPD_STATUS_DELAY 110 /* ms */

enum HdmiEventType {
    HDMI_EVENT_HOTPLUG = 0,
    HDMI_EVENT_HOTUNPLUG = 1,
    HDMI_EVENT_DETECT_SINK = 2,
    HDMI_EVENT_CEC_MSG = 3,
    HDMI_EVENT_HDCP_MSG = 4,
    HDMI_EVENT_ZERO_DRMIF_TIMEOUT = 5,
    HDMI_EVENT_SWITCH_TO_HDRMODE_TIMEOUT = 6,
    HDMI_EVENT_BUTT,
};

struct HdmiEventMsg {
    struct PlatformMsg msg;
    struct OsalSem sem;
    bool block;
    void *priv;
};

struct HdmiEvent {
    bool plugged;
    bool hpdDetected;
    struct HdmiHpdCallbackInfo callback;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_EVENT_H */
