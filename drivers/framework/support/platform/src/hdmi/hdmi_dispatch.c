/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_dispatch.h"
#include "hdf_log.h"
#include "hdmi_core.h"
#include "hdmi_if.h"

#define HDF_LOG_TAG hdmi_dispatch_c

enum HdmiIoCmd {
    HDMI_CMD_OPEN,
    HDMI_CMD_CLOSE,
    HDMI_CMD_START,
    HDMI_CMD_STOP,
    HDMI_CMD_AVMUTE_SET,
    HDMI_CMD_DEEP_COLOR_SET,
    HDMI_CMD_DEEP_COLOR_GET,
    HDMI_CMD_VIDEO_ATTR_SET,
    HDMI_CMD_AUDIO_ATTR_SET,
    HDMI_CMD_HDR_ATTR_SET,
    HDMI_CMD_READ_SINK_EDID,
    HDMI_CMD_INFOFRAME_SET,
    HDMI_CMD_INFOFRAME_GET,
    HDMI_CMD_REGISTER_HPD_CALLBACK_FUNC,
    HDMI_CMD_UNREGISTER_HPD_CALLBACK_FUNC,
    HDMI_CMD_BUTT,
};

struct HdmiDispatchFunc {
    uint32_t cmd;
    int32_t (*func)(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply);
};

static int32_t HdmiCmdOpen(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return HdmiCntlrOpen(cntlr);
}

static int32_t HdmiCmdClose(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    HdmiCntlrClose(cntlr);
    return HDF_SUCCESS;
}

static int32_t HdmiCmdStart(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return HdmiCntlrStart(cntlr);
}

static int32_t HdmiCmdStop(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return HdmiCntlrStop(cntlr);
}

static int32_t HdmiCmdAvmuteSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    bool *enable = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&enable, &size)) {
        HDF_LOGE("HdmiCmdAvmuteSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    HdmiCntlrAvmuteSet(cntlr, *enable);
    return HDF_SUCCESS;
}

static int32_t HdmiCmdDeepColorSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    enum HdmiDeepColor *color = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&color, &size)) {
        HDF_LOGE("HdmiCmdDeepColorSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrDeepColorSet(cntlr, *color);
}

static int32_t HdmiCmdDeepColorGet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    enum HdmiDeepColor color;
    (void)data;

    if (reply == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = HdmiCntlrDeepColorGet(cntlr, &color);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (HdfSbufWriteBuffer(reply, &color, sizeof(color)) == false) {
        HDF_LOGE("HdmiCmdDeepColorGet: write back color fail!");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiCmdVideoAttrSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdmiVideoAttr *attr = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&attr, &size)) {
        HDF_LOGE("HdmiCmdVideoAttrSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrSetVideoAttribute(cntlr, attr);
}

static int32_t HdmiCmdAudioAttrSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdmiAudioAttr *attr = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&attr, &size)) {
        HDF_LOGE("HdmiCmdAudioAttrSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrSetAudioAttribute(cntlr, attr);
}

static int32_t HdmiCmdHdrAttrSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdmiHdrAttr *attr = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&attr, &size)) {
        HDF_LOGE("HdmiCmdHdrAttrSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrSetHdrAttribute(cntlr, attr);
}

static int32_t HdmiCmdReadSinkEdid(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t edid[HDMI_EDID_TOTAL_SIZE] = {0};
    (void)data;

    if (reply == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = HdmiCntlrGetSinkEdid(cntlr, edid, HDMI_EDID_TOTAL_SIZE);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (HdfSbufWriteBuffer(reply, edid, HDMI_EDID_TOTAL_SIZE) == false) {
        HDF_LOGE("HdmiCmdReadSinkEdid: write back edid fail!");
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t HdmiCmdInfoFrameSet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    enum HdmiPacketType type;
    union HdmiInfoFrameInfo *frame = NULL;
    size_t size;

    if (!HdfSbufReadUint16(data, (uint16_t *)&type)) {
        HDF_LOGE("HdmiCmdInfoFrameSet: sbuf read uint16 failed");
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&frame, &size)) {
        HDF_LOGE("HdmiCmdInfoFrameSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrInfoFrameSet(cntlr, type, frame);
}

static int32_t HdmiCmdInfoFrameGet(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    size_t size;
    int32_t ret;
    enum HdmiPacketType *type = NULL;
    union HdmiInfoFrameInfo frame = {0};

    if (!HdfSbufReadBuffer(data, (const void **)&type, &size)) {
        HDF_LOGE("HdmiCmdInfoFrameGet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }

    ret = HdmiCntlrInfoFrameGet(cntlr, *type, &frame);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (HdfSbufWriteBuffer(reply, &frame, sizeof(frame)) == false) {
        HDF_LOGE("HdmiCmdInfoFrameGet: write back frame fail!");
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t HdmiCmdRegisterHpdCallbackFunc(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t *addr = NULL;
    size_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&addr, &size)) {
        HDF_LOGE("HdmiCmdRegisterHpdCallbackFunc: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return HdmiCntlrRegisterHpdCallbackFunc(cntlr, (struct HdmiHpdCallbackInfo *)(uintptr_t)(*addr));
}

static int32_t HdmiCmdUnregisterHpdCallbackFunc(struct HdmiCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return HdmiCntlrUnregisterHpdCallbackFunc(cntlr);
}

int32_t HdmiIoDispatch(struct HdfDeviceIoClient *client, int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdmiCntlr *cntlr = NULL;
    uint32_t i, len;
    struct HdmiDispatchFunc dispatchFunc[] = {
        { HDMI_CMD_OPEN, HdmiCmdOpen },
        { HDMI_CMD_CLOSE, HdmiCmdClose },
        { HDMI_CMD_START, HdmiCmdStart },
        { HDMI_CMD_STOP, HdmiCmdStop },
        { HDMI_CMD_AVMUTE_SET, HdmiCmdAvmuteSet },
        { HDMI_CMD_DEEP_COLOR_SET, HdmiCmdDeepColorSet },
        { HDMI_CMD_DEEP_COLOR_GET, HdmiCmdDeepColorGet },
        { HDMI_CMD_VIDEO_ATTR_SET, HdmiCmdVideoAttrSet },
        { HDMI_CMD_AUDIO_ATTR_SET, HdmiCmdAudioAttrSet },
        { HDMI_CMD_HDR_ATTR_SET, HdmiCmdHdrAttrSet },
        { HDMI_CMD_READ_SINK_EDID, HdmiCmdReadSinkEdid },
        { HDMI_CMD_INFOFRAME_SET, HdmiCmdInfoFrameSet },
        { HDMI_CMD_INFOFRAME_GET, HdmiCmdInfoFrameGet },
        { HDMI_CMD_REGISTER_HPD_CALLBACK_FUNC, HdmiCmdRegisterHpdCallbackFunc },
        { HDMI_CMD_UNREGISTER_HPD_CALLBACK_FUNC, HdmiCmdUnregisterHpdCallbackFunc },
    };

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("HdmiIoDispatch: client or hdf dev obj is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct HdmiCntlr *)client->device->service;
    if (cntlr == NULL) {
        HDF_LOGE("HdmiIoDispatch: service is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    len = sizeof(dispatchFunc) / sizeof(dispatchFunc[0]);
    for (i = 0; i < len; i++) {
        if (dispatchFunc[i].cmd == cmd) {
            return dispatchFunc[i].func(cntlr, data, reply);
        }
    }

    HDF_LOGE("HdmiIoDispatch: cmd %d is not support", cmd);
    return HDF_ERR_NOT_SUPPORT;
}
