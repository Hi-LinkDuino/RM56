/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_if.h"
#ifndef __USER__
#include "hdmi_core.h"
#endif
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#ifdef __USER__
#include "hdf_io_service_if.h"
#include "securec.h"
#endif

#define HDF_LOG_TAG hdmi_if_c

#define HDMI_SERVICE_NAME_LEN 32

#ifdef __USER__

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

static int32_t HdmiGetDataFromReply(struct HdfSBuf *reply, uint8_t *data, uint32_t size)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (HdfSbufReadBuffer(reply, &rBuf, &rLen) == false) {
        HDF_LOGE("HdmiGetDataFromReply: read rBuf fail!");
        return HDF_ERR_IO;
    }
    if (size != rLen) {
        HDF_LOGE("HdmiGetDataFromReply: err len:%u, rLen:%u", size, rLen);
        if (rLen > size) {
            rLen = size;
        }
    }

    if (memcpy_s(data, size, rBuf, rLen) != EOK) {
        HDF_LOGE("HdmiGetDataFromReply: memcpy rBuf fail!");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static void HdmiUserClose(DevHandle handle)
{
    struct HdfIoService *service = (struct HdfIoService *)handle;
    int32_t ret;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserClose: service is invalid");
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_CLOSE, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserClose: failed to send service call:%d", ret);
    }
}

static int32_t HdmiUserStart(DevHandle handle)
{
    struct HdfIoService *service = (struct HdfIoService *)handle;
    int32_t ret;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserStart: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_START, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserStart: failed to send service call:%d", ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiUserStop(DevHandle handle)
{
    struct HdfIoService *service = (struct HdfIoService *)handle;
    int32_t ret;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserStop: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_STOP, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserStop: failed to send service call:%d", ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiUserAvmuteSet(DevHandle handle, bool enable)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserAvmuteSet: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(enable));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserAvmuteSet: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, &enable, sizeof(enable))) {
        HDF_LOGE("HdmiUserAvmuteSet: sbuf write buffer failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_AVMUTE_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserAvmuteSet: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserDeepColorSet(DevHandle handle, enum HdmiDeepColor color)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserDeepColorSet: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(color));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserDeepColorSet: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, &color, sizeof(color))) {
        HDF_LOGE("HdmiUserDeepColorSet: sbuf write color failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_DEEP_COLOR_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserDeepColorSet: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserSetVideoAttribute(DevHandle handle, struct HdmiVideoAttr *attr)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserSetVideoAttribute: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (attr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(struct HdmiVideoAttr));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserSetVideoAttribute: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, attr, sizeof(struct HdmiVideoAttr))) {
        HDF_LOGE("HdmiUserSetVideoAttribute: sbuf write attr failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_VIDEO_ATTR_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserSetVideoAttribute: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserSetAudioAttribute(DevHandle handle, struct HdmiAudioAttr *attr)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserSetAudioAttribute: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (attr == NULL) {
        HDF_LOGE("HdmiUserSetAudioAttribute: attr is NULL");
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(struct HdmiAudioAttr));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserSetAudioAttribute: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, attr, sizeof(struct HdmiAudioAttr))) {
        HDF_LOGE("HdmiUserSetAudioAttribute: sbuf write attr failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_AUDIO_ATTR_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserSetAudioAttribute: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserSetHdrAttribute(DevHandle handle, struct HdmiHdrAttr *attr)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserSetHdrAttribute: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (attr == NULL) {
        HDF_LOGE("HdmiUserSetHdrAttribute: attr is NULL");
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(struct HdmiHdrAttr));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserSetHdrAttribute: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, attr, sizeof(struct HdmiHdrAttr))) {
        HDF_LOGE("HdmiUserSetHdrAttribute: sbuf write attr failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_HDR_ATTR_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserSetAudioAttribute: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserDeepColorGet(DevHandle handle, enum HdmiDeepColor *color)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserDeepColorGet: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (color == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* Four bytes are used to store the buffer length, and four bytes are used to align the memory. */
    reply = HdfSbufObtain(sizeof(*color) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("HdmiUserDeepColorGet: failed to obtain reply");
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_DEEP_COLOR_GET, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserDeepColorGet: failed to write, ret %d", ret);
    } else {
        ret = HdmiGetDataFromReply(reply, (uint8_t *)color, sizeof(*color));
    }

__EXIT :
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return ret;
}

static int32_t HdmiUserGetSinkEdid(DevHandle handle, uint8_t *buffer, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserGetSinkEdid: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (buffer == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    reply = HdfSbufObtain(len);
    if (reply == NULL) {
        HDF_LOGE("HdmiUserGetSinkEdid: failed to obtain reply");
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_READ_SINK_EDID, NULL, reply);
    if (ret <= 0) {
        HDF_LOGE("HdmiUserGetSinkEdid: failed to write, ret %d", ret);
    } else {
        ret = HdmiGetDataFromReply(reply, buffer, len);
    }

__EXIT :
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return ret;
}

static int32_t HdmiUserRegisterHpdCallbackFunc(DevHandle handle, struct HdmiHpdCallbackInfo *callback)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;
    uint32_t addr = (uint32_t)(uintptr_t)callback;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserRegisterHpdCallbackFunc: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (callback == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtain(sizeof(uint32_t));
    if (buf == NULL) {
        HDF_LOGE("HdmiUserRegisterHpdCallbackFunc: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, (void *)&addr, sizeof(uint32_t))) {
        HDF_LOGE("HdmiUserRegisterHpdCallbackFunc: sbuf write attr failed");
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_REGISTER_HPD_CALLBACK_FUNC, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserRegisterHpdCallbackFunc: failed to write, ret %d", ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

static int32_t HdmiUserUnregisterHpdCallbackFunc(DevHandle handle)
{
    struct HdfIoService *service = (struct HdfIoService *)handle;
    int32_t ret;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiUserUnregisterHpdCallbackFunc: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_UNREGISTER_HPD_CALLBACK_FUNC, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiUserUnregisterHpdCallbackFunc: failed to send service call:%d", ret);
        return ret;
    }
    return HDF_SUCCESS;
}
#endif

static void *HdmiCntlrObjGet(uint16_t busNum)
{
    char *serviceName = NULL;
    void *obj = NULL;

    serviceName = (char *)OsalMemCalloc(HDMI_SERVICE_NAME_LEN + 1);
    if (serviceName == NULL) {
        HDF_LOGE("HDMI service name malloc fail.");
        return NULL;
    }
    if (snprintf_s(serviceName, (HDMI_SERVICE_NAME_LEN + 1),
        HDMI_SERVICE_NAME_LEN, "HDF_PLATFORM_HDMI_%u", busNum) < 0) {
        HDF_LOGE("get HDMI service name fail.");
        OsalMemFree(serviceName);
        return obj;
    }
#ifdef __USER__
    obj = (void *)HdfIoServiceBind(serviceName);
#else
    obj = (void *)HdmiCntlrGetByBusNum(busNum);
#endif
    OsalMemFree(serviceName);
    return obj;
}

DevHandle HdmiOpen(uint16_t busNum)
{
    DevHandle *obj = (DevHandle *)HdmiCntlrObjGet(busNum);
    int32_t ret;

    if (obj == NULL) {
        return NULL;
    }
#ifdef __USER__
    struct HdfIoService *service = (struct HdfIoService *)obj;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("HdmiOpen: dispatcher or Dispatch is NULL!");
        return NULL;
    }
    ret = service->dispatcher->Dispatch(&service->object, HDMI_CMD_OPEN, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdmiOpen: failed to send service call:%d", ret);
        return NULL;
    }
#else
    ret = HdmiCntlrOpen((struct HdmiCntlr *)obj);
    if (ret != HDF_SUCCESS) {
        return NULL;
    }
#endif
    return obj;
}

int32_t HdmiStart(DevHandle handle)
{
#ifdef __USER__
    return HdmiUserStart(handle);
#else
    return HdmiCntlrStart((struct HdmiCntlr *)handle);
#endif
}

int32_t HdmiStop(DevHandle handle)
{
#ifdef __USER__
    return HdmiUserStop(handle);
#else
    return HdmiCntlrStop((struct HdmiCntlr *)handle);
#endif
}

int32_t HdmiAvmuteSet(DevHandle handle, bool enable)
{
#ifdef __USER__
    return HdmiUserAvmuteSet(handle, enable);
#else
    HdmiCntlrAvmuteSet((struct HdmiCntlr *)handle, enable);
    return HDF_SUCCESS;
#endif
}

int32_t HdmiDeepColorSet(DevHandle handle, enum HdmiDeepColor color)
{
#ifdef __USER__
    return HdmiUserDeepColorSet(handle, color);
#else
    return HdmiCntlrDeepColorSet((struct HdmiCntlr *)handle, color);
#endif
}

int32_t HdmiDeepColorGet(DevHandle handle, enum HdmiDeepColor *color)
{
#ifdef __USER__
    return HdmiUserDeepColorGet(handle, color);
#else
    return HdmiCntlrDeepColorGet((struct HdmiCntlr *)handle, color);
#endif
}

int32_t HdmiSetVideoAttribute(DevHandle handle, struct HdmiVideoAttr *attr)
{
#ifdef __USER__
    return HdmiUserSetVideoAttribute(handle, attr);
#else
    return HdmiCntlrSetVideoAttribute((struct HdmiCntlr *)handle, attr);
#endif
}

int32_t HdmiSetAudioAttribute(DevHandle handle, struct HdmiAudioAttr *attr)
{
#ifdef __USER__
    return HdmiUserSetAudioAttribute(handle, attr);
#else
    return HdmiCntlrSetAudioAttribute((struct HdmiCntlr *)handle, attr);
#endif
}

int32_t HdmiSetHdrAttribute(DevHandle handle, struct HdmiHdrAttr *attr)
{
#ifdef __USER__
    return HdmiUserSetHdrAttribute(handle, attr);
#else
    return HdmiCntlrSetHdrAttribute((struct HdmiCntlr *)handle, attr);
#endif
}

int32_t HdmiReadSinkEdid(DevHandle handle, uint8_t *buffer, uint32_t len)
{
#ifdef __USER__
    return HdmiUserGetSinkEdid(handle, buffer, len);
#else
    return HdmiCntlrGetSinkEdid((struct HdmiCntlr *)handle, buffer, len);
#endif
}

int32_t HdmiRegisterHpdCallbackFunc(DevHandle handle, struct HdmiHpdCallbackInfo *callback)
{
#ifdef __USER__
    return HdmiUserRegisterHpdCallbackFunc(handle, callback);
#else
    return HdmiCntlrRegisterHpdCallbackFunc((struct HdmiCntlr *)handle, callback);
#endif
}

int32_t HdmiUnregisterHpdCallbackFunc(DevHandle handle)
{
#ifdef __USER__
    return HdmiUserUnregisterHpdCallbackFunc(handle);
#else
    return HdmiCntlrUnregisterHpdCallbackFunc((struct HdmiCntlr *)handle);
#endif
}

void HdmiClose(DevHandle handle)
{
#ifdef __USER__
    HdmiUserClose(handle);
#else
    HdmiCntlrClose((struct HdmiCntlr *)handle);
#endif
}
