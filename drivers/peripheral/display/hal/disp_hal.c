/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "disp_hal.h"
#include <securec.h>
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"

#define OFFSET_TWO_BYTE    16
#define MASK_TWO_BYTE      0xffff

static int32_t DispCmdSend(const uint32_t cmd, struct HdfSBuf *reqData, struct HdfSBuf *respData)
{
    struct HdfIoService *dispService = NULL;

    dispService = HdfIoServiceBind(DISP_SERVICE_NAME);
    if ((dispService == NULL) || (dispService->dispatcher == NULL) || (dispService->dispatcher->Dispatch == NULL)) {
        HDF_LOGE("%s:bad remote service found", __func__);
        goto EXIT;
    }
    int32_t ret = dispService->dispatcher->Dispatch(&dispService->object, cmd, reqData, respData);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: cmd=%u, ret=%d", __func__, cmd, ret);
        goto EXIT;
    }
    HDF_LOGI("%s: cmd=%u, ret=%d", __func__, cmd, ret);
    HdfIoServiceRecycle(dispService);
    return DISPLAY_SUCCESS;

EXIT:
    HdfIoServiceRecycle(dispService);
    return DISPLAY_FAILURE;
}

static int32_t GetInfo(uint32_t devId, struct DispInfo *info)
{
    struct DispInfo *tmpInfo = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (info == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return DISPLAY_FAILURE;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: obtain data sbuf fail", __func__);
        return DISPLAY_FAILURE;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: obtain reply sbuf fail", __func__);
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(data, devId)) {
        HDF_LOGE("HdfSbufWriteUint32 failure");
        goto EXIT;
    }
    if (DispCmdSend(DISP_CMD_GET_PANELINFO, data, reply) != DISPLAY_SUCCESS) {
        HDF_LOGE("cmd:DISP_CMD_GET_PANEL_INFO failure");
        goto EXIT;
    }
    uint32_t dataSize = 0;
    if (!HdfSbufReadBuffer(reply, (const void **)(&tmpInfo), &dataSize) || dataSize != sizeof(struct DispInfo)) {
        HDF_LOGE("HdfSbufReadBuffer failure");
        goto EXIT;
    }
    if (memcpy_s(info, sizeof(struct DispInfo), tmpInfo, dataSize) != EOK) {
        HDF_LOGE("memcpy_s failure");
        goto EXIT;
    }
    HDF_LOGI("tmpInfo->width = %u, tmpInfo->height = %u", tmpInfo->width, tmpInfo->height);
    HDF_LOGI("tmpInfo->hbp = %u, tmpInfo->hfp = %u", tmpInfo->hbp, tmpInfo->hfp);
    HDF_LOGI("tmpInfo->frameRate = %u", tmpInfo->frameRate);
    HDF_LOGI("tmpInfo->intfSync = %d", tmpInfo->intfSync);
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_SUCCESS;

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_FAILURE;
}

static int32_t DispGetParaProcess(uint32_t devId, const uint32_t cmd, uint32_t *value)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (value == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return DISPLAY_FAILURE;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: obtain data sbuf fail", __func__);
        return DISPLAY_FAILURE;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: obtain reply sbuf fail", __func__);
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(data, devId)) {
        HDF_LOGE("HdfSbufWriteUint32 failure");
        goto EXIT;
    }
    ret = DispCmdSend(cmd, data, reply);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("cmd:DISP_CMD_GET_PANEL_INFO failure");
        goto EXIT;
    }
    if (!HdfSbufReadUint32(reply, value)) {
        HDF_LOGE("HdfSbufReadUint32 failure");
        goto EXIT;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_SUCCESS;

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_FAILURE;
}

static int32_t DispEventProcess(uint32_t devId, const uint32_t cmd, uint32_t val)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: obtain data sbuf fail", __func__);
        return DISPLAY_FAILURE;
    }
    uint32_t para = (devId << OFFSET_TWO_BYTE) | (val & 0xffff);
    if (!HdfSbufWriteUint32(data, para)) {
        HDF_LOGE("HdfSbufWriteUint32 failure\n");
        goto EXIT;
    }
    ret = DispCmdSend(cmd, data, NULL);
    if (ret) {
        HDF_LOGE("cmd:DISP_CMD_SET_%s failure\n", (cmd == DISP_CMD_SET_POWERSTATUS) ? "POWERMODE" : "BACKLIGHT");
        goto EXIT;
    }
    HdfSbufRecycle(data);
    return DISPLAY_SUCCESS;

EXIT:
    HdfSbufRecycle(data);
    return DISPLAY_FAILURE;
}

static int32_t SetPowerStatus(uint32_t devId,  DispPowerStatus status)
{
    return DispEventProcess(devId, DISP_CMD_SET_POWERSTATUS, status);
}

static int32_t GetPowerStatus(uint32_t devId,  DispPowerStatus *pStatus)
{
    return DispGetParaProcess(devId, DISP_CMD_GET_POWERSTATUS, pStatus);
}

static int32_t SetBacklight(uint32_t devId, uint32_t level)
{
    return DispEventProcess(devId, DISP_CMD_SET_BACKLIGHT, level);
}

static int32_t GetBacklight(uint32_t devId, uint32_t *level)
{
    return DispGetParaProcess(devId, DISP_CMD_GET_BACKLIGHT, level);
}

HalFuncs *GetHalFuncs(void)
{
    static HalFuncs *hFuncs = NULL;

    if (hFuncs == NULL) {
        hFuncs = (HalFuncs *)malloc(sizeof(HalFuncs));
        if (hFuncs == NULL) {
            HDF_LOGE("%s: malloc fail", __func__);
            return NULL;
        }
        (void)memset_s(hFuncs, sizeof(HalFuncs), 0, sizeof(HalFuncs));
        hFuncs->SetPowerStatus = SetPowerStatus;
        hFuncs->GetPowerStatus = GetPowerStatus;
        hFuncs->SetBacklight = SetBacklight;
        hFuncs->GetBacklight = GetBacklight;
        hFuncs->GetInfo = GetInfo;
    }
    return hFuncs;
}
