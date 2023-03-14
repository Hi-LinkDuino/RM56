/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdmi_core.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hdmi_event_c

static int32_t HdmiEventPostMsg(struct HdmiCntlr *cntlr, struct HdmiEventMsg *event)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (event == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (event->block) {
        (void)OsalSemInit(&event->sem, 0);
    }
    (void)PlatformQueueAddMsg(cntlr->msgQueue, &event->msg);
    if (!event->block) {
        return HDF_SUCCESS;
    }

    ret = OsalSemWait(&event->sem, HDF_WAIT_FOREVER);
    (void)OsalSemDestroy(&event->sem);
    OsalMemFree(event);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiEventPostMsg: wait hdmi event msg fail!");
    }
    return ret;
}

bool HdmiHpdStatusGet(struct HdmiCntlr *cntlr)
{
    bool ret = false;

    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->hotPlugStateGet == NULL) {
        return ret;
    }

    HdmiCntlrLock(cntlr);
    ret = cntlr->ops->hotPlugStateGet(cntlr);
    HdmiCntlrUnlock(cntlr);
    return ret;
}

bool HdmiHpdStatusDelayGet(struct HdmiCntlr *cntlr)
{
    uint32_t i;

    /*
     * An HDMI Sink shall indicate any change to the contents of the EDID by driving a low voltage level pulse
     * on the Hot Plug Detect pin. This pluse shall be at least 100 msec.
     */
    for (i = 0; i < HDMI_READ_HPD_STATUS_DELAY; i++) {
        if (HdmiHpdStatusGet(cntlr) == true) {
            return true;
        }
        OsalMSleep(1);
    }
    return false;
}

int32_t HdmiAddEventMsgToQueue(struct HdmiCntlr *cntlr, int32_t code, bool block, void *data)
{
    struct HdmiEventMsg *event = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    event = (struct HdmiEventMsg *)OsalMemCalloc(sizeof(struct HdmiEventMsg));
    if (event == NULL) {
        HDF_LOGE("HdmiAddEventMsgToQueue: OsalMemCalloc fail!n");
        return HDF_ERR_MALLOC_FAIL;
    }
    event->msg.code = code;
    event->msg.data = (void *)cntlr;
    event->block = block;
    event->priv = data;
    return HdmiEventPostMsg(cntlr, event);
}

static int32_t HdmiEventHotPlugHandleComm(struct HdmiCntlr *cntlr)
{
    int32_t ret;

    cntlr->event.plugged = true;
    HDF_LOGD("Hdmi sink device plugged!");
    if (cntlr->hdmi == NULL) {
        ret = HdmiCntlrAllocDev(cntlr);
        if (ret != HDF_SUCCESS) {
            cntlr->event.plugged = false;
            cntlr->event.hpdDetected = false;
            return ret;
        }
        HDF_LOGE("HdmiEventHotPlugHandleComm HdmiCntlrAllocDev success.");
    }

    /* Update EDID. */
    if (HdmiEdidReset(&(cntlr->hdmi->edid)) != HDF_SUCCESS) {
        HDF_LOGE("edid reset fail.");
        ret = HDF_ERR_IO;
        cntlr->event.plugged = false;
        cntlr->event.hpdDetected = false;
        return ret;
    }
    ret = HdmiEdidRawDataRead(&(cntlr->hdmi->edid), &(cntlr->ddc));
    if (ret != HDF_SUCCESS) {
        cntlr->event.plugged = false;
        cntlr->event.hpdDetected = false;
        return ret;
    }
    ret = HdmiEdidPhase(&(cntlr->hdmi->edid));
    if (ret != HDF_SUCCESS) {
        cntlr->event.plugged = false;
        cntlr->event.hpdDetected = false;
        return ret;
    }

    cntlr->event.hpdDetected = true;
    if (cntlr->event.callback.callbackFunc != NULL) {
        cntlr->event.callback.callbackFunc(cntlr->event.callback.data, true);
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEventHotPlugHandle(struct HdmiCntlr *cntlr)
{
    if (HdmiHpdStatusDelayGet(cntlr) == false) {
        HDF_LOGD("not detect HPD.");
        return HDF_ERR_IO;
    }

    if (cntlr->event.plugged == true) {
        HDF_LOGD("HdmiEventHotPlugHandle: hdp state not change");
        return HDF_SUCCESS;
    }
    return HdmiEventHotPlugHandleComm(cntlr);
}

static int32_t HdmiEventHotUnplugHandle(struct HdmiCntlr *cntlr)
{
    if (cntlr->event.plugged == false) {
        HDF_LOGD("HdmiEventHotUnplugHandle: plug state not change");
        return HDF_SUCCESS;
    }

    HdmiCntlrClose(cntlr);
    HdmiCntlrFreeDev(cntlr);
    cntlr->event.plugged = false;
    cntlr->event.hpdDetected = false;
    if (cntlr->event.callback.callbackFunc != NULL) {
        cntlr->event.callback.callbackFunc(cntlr->event.callback.data, false);
    }
    return HDF_SUCCESS;
}

static int32_t HdmiEventDetectSinkHandle(struct HdmiCntlr *cntlr)
{
    if (HdmiHpdStatusDelayGet(cntlr) == false) {
        HDF_LOGD("not detect HPD.");
        return HDF_SUCCESS;
    }
    return HdmiEventHotPlugHandleComm(cntlr);
}

static int32_t HdmiEventCecMsgHandle(struct HdmiCntlr *cntlr, struct HdmiCecMsg *msg)
{
    if (msg == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return HdmiCecReceivedMsg(cntlr->cec, msg);
}

static int32_t HdmiEventHdrZeroDrmIfTimeout(struct HdmiCntlr *cntlr)
{
    return HdmiHdrDrmInfoFrameStop(cntlr->hdr);
}

static int32_t HdmiEventSwitchToHdrModeTimeout(struct HdmiCntlr *cntlr)
{
    return HdmiHdrModeChangeTimeout(cntlr->hdr);
}

int32_t HdmiEventMsgHandleDefault(struct PlatformQueue *queue, struct PlatformMsg *msg)
{
    int32_t ret;
    struct HdmiCntlr *cntlr = NULL;
    struct HdmiEventMsg *event = NULL;

    if (queue == NULL || msg == NULL) {
        HDF_LOGE("HdmiEventMsgHandleDefault: msg or queue is null!");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct HdmiCntlr *)queue->data;
    if (cntlr == NULL) {
        HDF_LOGE("HdmiEventMsgHandleDefault: cntlr is null!");
        return HDF_ERR_INVALID_OBJECT;
    }
    event = (struct HdmiEventMsg *)msg;
    switch (msg->code) {
        case HDMI_EVENT_HOTPLUG:
            ret = HdmiEventHotPlugHandle(cntlr);
            break;
        case HDMI_EVENT_HOTUNPLUG:
            ret = HdmiEventHotUnplugHandle(cntlr);
            break;
        case HDMI_EVENT_DETECT_SINK:
            ret = HdmiEventDetectSinkHandle(cntlr);
            break;
        case HDMI_EVENT_CEC_MSG:
            ret = HdmiEventCecMsgHandle(cntlr, event->priv);
            break;
        case HDMI_EVENT_ZERO_DRMIF_TIMEOUT:
            ret = HdmiEventHdrZeroDrmIfTimeout(cntlr);
            break;
        case HDMI_EVENT_SWITCH_TO_HDRMODE_TIMEOUT:
            ret = HdmiEventSwitchToHdrModeTimeout(cntlr);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    if (!event->block) {
        OsalMemFree(event);
    } else {
        (void)OsalSemPost(&event->sem);
    }

    return ret;
}

int32_t HdmiEventHandle(struct HdmiCntlr *cntlr, enum HdmiEventType event, void *data)
{
    int32_t ret = HDF_SUCCESS;

    if (cntlr == NULL) {
        HDF_LOGE("HdmiEventHandleIrq: cntlr is null!");
        return HDF_ERR_INVALID_OBJECT;
    }
    switch (event) {
        case HDMI_EVENT_HOTPLUG:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_HOTPLUG, false, data);
            break;
        case HDMI_EVENT_HOTUNPLUG:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_HOTUNPLUG, false, data);
            break;
        case HDMI_EVENT_DETECT_SINK:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_DETECT_SINK, false, data);
            break;
        case HDMI_EVENT_CEC_MSG:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_CEC_MSG, false, data);
            break;
        case HDMI_EVENT_HDCP_MSG:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_HDCP_MSG, false, data);
            break;
        case HDMI_EVENT_ZERO_DRMIF_TIMEOUT:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_ZERO_DRMIF_TIMEOUT, false, data);
            break;
        case HDMI_EVENT_SWITCH_TO_HDRMODE_TIMEOUT:
            ret = HdmiAddEventMsgToQueue(cntlr, HDMI_EVENT_SWITCH_TO_HDRMODE_TIMEOUT, false, data);
            break;
        default:
            HDF_LOGE("event %d is not support", event);
            break;
    }
    return ret;
}
