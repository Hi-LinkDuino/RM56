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

#include "display_layer.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <securec.h>
#include "display_type.h"
#include "hdf_log.h"
#include "hdf_io_service_if.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"

#define DEV_ID                   0
#define LAYER_ID                 0
#define FB_PATH                  "/dev/fb0"
#define DISP_WIDTH               800
#define DISP_HEIGHT              480
#define BITS_PER_PIXEL           32
#define BITS_TO_BYTE             8
#define DISP_SERVICE_NAME        "hdf_disp"
#define DISP_CMD_GET_PANELINFO   1

#undef  HDF_LOG_TAG
#define HDF_LOG_TAG              display_layer_c

/* output timing */
enum IntfSync {
    OUTPUT_USER = 0,          /* User timing */
    OUTPUT_PAL,               /* PAL standard */
    OUTPUT_NTSC,              /* NTSC standard */
    OUTPUT_1080P24,           /* 1920 x 1080 at 24 Hz. */
    OUTPUT_1080P25,           /* 1920 x 1080 at 25 Hz. */
    OUTPUT_1080P30,           /* 1920 x 1080 at 30 Hz. */
    OUTPUT_720P50,            /* 1280 x  720 at 50 Hz. */
    OUTPUT_720P60,            /* 1280 x  720 at 60 Hz. */
    OUTPUT_1080I50,           /* 1920 x 1080 at 50 Hz, interlace. */
    OUTPUT_1080I60,           /* 1920 x 1080 at 60 Hz, interlace. */
    OUTPUT_1080P50,           /* 1920 x 1080 at 50 Hz. */
    OUTPUT_1080P60,           /* 1920 x 1080 at 60 Hz. */
    OUTPUT_576P50,            /* 720  x  576 at 50 Hz. */
    OUTPUT_480P60,            /* 720  x  480 at 60 Hz. */
    OUTPUT_800X600_60,        /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    OUTPUT_1024X768_60,       /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    OUTPUT_1280X1024_60,      /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    OUTPUT_1366X768_60,       /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    OUTPUT_1440X900_60,       /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    OUTPUT_1280X800_60,       /* 1280*800@60Hz VGA@60Hz */
    OUTPUT_1600X1200_60,      /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    OUTPUT_1680X1050_60,      /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    OUTPUT_1920X1200_60,      /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
    OUTPUT_640X480_60,        /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    OUTPUT_960H_PAL,          /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced) */
    OUTPUT_960H_NTSC,         /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced) */
    OUTPUT_1920X2160_30,      /* 1920x2160_30 */
    OUTPUT_2560X1440_30,      /* 2560x1440_30 */
    OUTPUT_2560X1440_60,      /* 2560x1440_60 */
    OUTPUT_2560X1600_60,      /* 2560x1600_60 */
    OUTPUT_3840X2160_24,      /* 3840x2160_24 */
    OUTPUT_3840X2160_25,      /* 3840x2160_25 */
    OUTPUT_3840X2160_30,      /* 3840x2160_30 */
    OUTPUT_3840X2160_50,      /* 3840x2160_50 */
    OUTPUT_3840X2160_60,      /* 3840x2160_60 */
    OUTPUT_4096X2160_24,      /* 4096x2160_24 */
    OUTPUT_4096X2160_25,      /* 4096x2160_25 */
    OUTPUT_4096X2160_30,      /* 4096x2160_30 */
    OUTPUT_4096X2160_50,      /* 4096x2160_50 */
    OUTPUT_4096X2160_60,      /* 4096x2160_60 */
    OUTPUT_320X240_60,        /* For ota5182 at 60 Hz (8bit) */
    OUTPUT_320X240_50,        /* For ili9342 at 50 Hz (6bit) */
    OUTPUT_240X320_50,        /* Hi3559AV100: For ili9341 at 50 Hz (6bit) */
    OUTPUT_240X320_60,        /* For ili9341 at 60 Hz (16bit) */
    OUTPUT_800X600_50,        /* For LCD     at 50 Hz (24bit) */
    OUTPUT_720X1280_60,       /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    OUTPUT_1080X1920_60,      /* For MIPI DSI Tx 1080x1920 at 60 Hz */
    OUTPUT_7680X4320_30,      /* For HDMI2.1 at 30 Hz */
};

struct DispInfo {
    uint32_t width;
    uint32_t hbp;
    uint32_t hfp;
    uint32_t hsw;
    uint32_t height;
    uint32_t vbp;
    uint32_t vfp;
    uint32_t vsw;
    uint32_t frameRate;
    uint32_t intfType;
    enum IntfSync intfSync;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t defLevel;
};

struct LayerPrivate {
    int32_t  fd;
    uint32_t width;
    uint32_t height;
    int32_t  pitch;
    void     *fbAddr;
    uint32_t fbSize;
    void     *layerAddr;
    PixelFormat pixFmt;
};

struct LayerManager {
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
    int32_t count;
};

static struct LayerManager g_layerManager;

static void GetLayerMgr(void)
{
    g_layerManager.count++;
}

static int32_t PutLayerMgr(void)
{
    g_layerManager.count--;
    return g_layerManager.count;
}

static void LockLayerMgr(void)
{
    pthread_mutex_lock(&g_layerManager.mutex);
}

static void UnlockLayerMgr(void)
{
    pthread_mutex_unlock(&g_layerManager.mutex);
}

static int32_t DispCmdSend(const uint32_t cmd, struct HdfSBuf *reqData, struct HdfSBuf *respData)
{
    struct HdfIoService *dispService = NULL;

    dispService = HdfIoServiceBind(DISP_SERVICE_NAME);
    if ((dispService == NULL) || (dispService->dispatcher == NULL) || (dispService->dispatcher->Dispatch == NULL)) {
        HDF_LOGE("%s:bad remote service found", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = dispService->dispatcher->Dispatch(&dispService->object, cmd, reqData, respData);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: cmd=%u, ret=%d", __func__, cmd, ret);
        HdfIoServiceRecycle(dispService);
        return DISPLAY_FAILURE;
    }
    HdfIoServiceRecycle(dispService);
    return DISPLAY_SUCCESS;
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
        HdfSbufRecycle(data);
        return DISPLAY_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, devId)) {
        HDF_LOGE("HdfSbufWriteUint32 failure");
        goto ERR;
    }
    if (DispCmdSend(DISP_CMD_GET_PANELINFO, data, reply) != DISPLAY_SUCCESS) {
        HDF_LOGE("cmd:DISP_CMD_GET_PANEL_INFO failure");
        goto ERR;
    }
    uint32_t dataSize = 0;
    if (!HdfSbufReadBuffer(reply, (const void **)(&tmpInfo), &dataSize) || dataSize != sizeof(struct DispInfo)) {
        HDF_LOGE("HdfSbufReadBuffer failure");
        goto ERR;
    }
    if (memcpy_s(info, sizeof(struct DispInfo), tmpInfo, dataSize) != EOK) {
        HDF_LOGE("memcpy_s failure");
        goto ERR;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_SUCCESS;

ERR:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return DISPLAY_FAILURE;
}

static struct LayerPrivate *GetLayerInstance(void)
{
    static int32_t count;
    static struct DispInfo info;
    static struct LayerPrivate layerPriv = {
        .fd = -1,
        .width = DISP_WIDTH,
        .height = DISP_HEIGHT,
        .pixFmt = PIXEL_FMT_RGBA_8888,
    };

    if (count == 0) {
        count = 1;
        if (GetInfo(DEV_ID, &info) == DISPLAY_SUCCESS) {
            layerPriv.width = info.width;
            layerPriv.height = info.height;
        } else {
            HDF_LOGI("%s: GetInfo failed, use default setting", __func__);
        }
    }
    return &layerPriv;
}

static int32_t InitDisplay(uint32_t devId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t DeinitDisplay(uint32_t devId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static void SetBackground(void)
{
    struct LayerPrivate *priv = GetLayerInstance();
    uint32_t i;
    uint32_t j;
    uint32_t *framebuffer = (uint32_t *)priv->fbAddr;
    for (j = 0; j < priv->height; j++) {
        for (i = 0; i < priv->width; i++) {
            framebuffer[i + j * priv->width] = 0xFF; // Blue background
        }
    }
}

static int32_t CreateLayer(uint32_t devId, const LayerInfo *layerInfo, uint32_t *layerId)
{
    if (layerInfo == NULL || layerId == NULL) {
        HDF_LOGE("%s: pointer is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    LockLayerMgr();
    struct LayerPrivate *priv = GetLayerInstance();
    priv->fd = open(FB_PATH, O_RDWR, 0);
    if (priv->fd < 0) {
        HDF_LOGE("%s: open fb dev failed", __func__);
        UnlockLayerMgr();
        return DISPLAY_FD_ERR;
    }
    priv->pitch = layerInfo->width * BITS_PER_PIXEL / BITS_TO_BYTE;
    priv->fbSize = ((priv->pitch * priv->height) + 0xfff) & (~0xfff);
    priv->fbAddr = (void *)mmap(NULL, priv->fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, priv->fd, 0);
    if (priv->fbAddr == NULL) {
        HDF_LOGE("%s: mmap fb address failure, errno: %d", __func__, errno);
        close(priv->fd);
        priv->fd = -1;
        priv->pitch = 0;
        priv->fbSize = 0;
        UnlockLayerMgr();
        return DISPLAY_FAILURE;
    }
    SetBackground();
    *layerId = LAYER_ID;
    HDF_LOGI("%s: open layer success", __func__);
    UnlockLayerMgr();
    return DISPLAY_SUCCESS;
}

static int32_t CloseLayer(uint32_t devId, uint32_t layerId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("%s: layerId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    LockLayerMgr();
    struct LayerPrivate *priv = GetLayerInstance();
    if (priv->fd >= 0) {
        close(priv->fd);
    }
    if (priv->layerAddr != NULL) {
        free(priv->layerAddr);
        priv->layerAddr = NULL;
    }
    if (priv->fbAddr != NULL) {
        munmap(priv->fbAddr, priv->fbSize);
    }
    priv->fd = -1;
    UnlockLayerMgr();
    return DISPLAY_SUCCESS;
}

static int32_t GetDisplayInfo(uint32_t devId, DisplayInfo *dispInfo)
{
    if (dispInfo == NULL) {
        HDF_LOGE("%s: dispInfo is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    LockLayerMgr();
    struct LayerPrivate *priv = GetLayerInstance();
    dispInfo->width = priv->width;
    dispInfo->height = priv->height;
    dispInfo->rotAngle = ROTATE_NONE;
    HDF_LOGD("%s: width = %u, height = %u, rotAngle = %u", __func__, dispInfo->width,
        dispInfo->height, dispInfo->rotAngle);
    UnlockLayerMgr();
    return DISPLAY_SUCCESS;
}

static int32_t Flush(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    int32_t ret;

    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("%s: layerId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is null", __func__);
        return DISPLAY_FAILURE;
    }

    HDF_LOGD("%s: width = %d, height = %d, pixFormat = %d, pitch = %d", __func__, buffer->width,
        buffer->height, buffer->pixFormat, buffer->pitch);
    LockLayerMgr();
    struct LayerPrivate *priv = GetLayerInstance();
    ret = memcpy_s(priv->fbAddr, priv->fbSize, buffer->data.virAddr, priv->fbSize);
    if (ret != EOK) {
        HDF_LOGE("%s: memcpy_s fail, ret %d", __func__, ret);
        UnlockLayerMgr();
        return ret;
    }
    UnlockLayerMgr();
    return DISPLAY_SUCCESS;
}

static int32_t GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("%s: devId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("%s: layerId invalid", __func__);
        return DISPLAY_FAILURE;
    }
    LockLayerMgr();
    struct LayerPrivate *priv = GetLayerInstance();
    if (priv->fd < 0) {
        HDF_LOGE("%s: fd invalid", __func__);
        UnlockLayerMgr();
        return DISPLAY_FAILURE;
    }
    buffer->fenceId = 0;
    buffer->width = priv->width;
    buffer->height = priv->height;
    buffer->pixFormat = priv->pixFmt;
    buffer->pitch = priv->pitch;
    buffer->data.virAddr = malloc(priv->fbSize);
    if (buffer->data.virAddr == NULL) {
        HDF_LOGE("%s: malloc failure", __func__);
        UnlockLayerMgr();
        return DISPLAY_FAILURE;
    }
    priv->layerAddr = buffer->data.virAddr;
    (void)memset_s(buffer->data.virAddr, priv->fbSize, 0x00, priv->fbSize);
    HDF_LOGD("%s: fenceId = %d, width = %d, height = %d, pixFormat = %d, pitch = %d", __func__, buffer->fenceId,
        buffer->width, buffer->height, buffer->pixFormat, buffer->pitch);
    UnlockLayerMgr();
    return DISPLAY_SUCCESS;
}

int32_t LayerInitialize(LayerFuncs **funcs)
{
    static LayerFuncs *lFuncs = NULL;

    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (lFuncs == NULL) {
        lFuncs = (LayerFuncs *)OsalMemCalloc(sizeof(LayerFuncs));
        if (lFuncs == NULL) {
            HDF_LOGE("%s: lFuncs is null", __func__);
            return DISPLAY_NULL_PTR;
        }
        pthread_mutexattr_init(&g_layerManager.mutexattr);
        pthread_mutexattr_setpshared(&g_layerManager.mutexattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&g_layerManager.mutex, &g_layerManager.mutexattr);
        lFuncs->InitDisplay = InitDisplay;
        lFuncs->DeinitDisplay = DeinitDisplay;
        lFuncs->GetDisplayInfo = GetDisplayInfo;
        lFuncs->CreateLayer = CreateLayer;
        lFuncs->CloseLayer = CloseLayer;
        lFuncs->Flush = Flush;
        lFuncs->GetLayerBuffer = GetLayerBuffer;
    }
    *funcs = lFuncs;
    GetLayerMgr();
    HDF_LOGI("%s: success", __func__);
    return DISPLAY_SUCCESS;
}

int32_t LayerUninitialize(LayerFuncs *funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (PutLayerMgr() == 0) {
        pthread_mutexattr_destroy(&g_layerManager.mutexattr);
        pthread_mutex_destroy(&g_layerManager.mutex);
        OsalMemFree(funcs);
    }
    HDF_LOGI("%s: layer uninitialize success", __func__);
    return DISPLAY_SUCCESS;
}
