/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include <string.h>
#include "display_layer.h"
#include "disp_hal.h"
#include "display_type.h"
#include "hdf_log.h"

#define DEV_ID 0
#define LAYER_ID 0
#define DISP_WIDTH 720
#define DISP_HEIGHT 480
#define BITS_PER_PIXEL 32
#define BITS_TO_BYTE 8

struct LayerPrivate {
    uint32_t width;
    uint32_t height;
    int32_t pitch;
    PixelFormat pixFmt;
    void *fbAddr;
    uint32_t fbSize;
};

static struct LayerPrivate *GetLayerInstance(void)
{
    // default layer config, rewritten by OpenLayer()
    static struct LayerPrivate layerPriv = {
        .width = DISP_WIDTH,
        .height = DISP_HEIGHT,
        .pixFmt = PIXEL_FMT_RGBA_8888,
    };
    return &layerPriv;
}

static int32_t InitDisplay(uint32_t devId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    if (DispInit(devId) != DISPLAY_SUCCESS) {
        HDF_LOGE("DispInit failed");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t DeinitDisplay(uint32_t devId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    DispOff(devId);
    return DISPLAY_SUCCESS;
}

static int32_t OpenLayer(uint32_t devId, const LayerInfo *layerInfo, uint32_t *layerId)
{
    if (layerInfo == NULL || layerId == NULL) {
        HDF_LOGE("pointer is null");
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    struct LayerPrivate *priv = GetLayerInstance();
    priv->width = layerInfo->width;
    priv->height = layerInfo->height;
    priv->pixFmt = layerInfo->pixFormat;
    priv->pitch = layerInfo->width * layerInfo->bpp / BITS_TO_BYTE;
    priv->fbSize = ((priv->pitch * priv->height) + 0xfff) & (~0xfff);

    if (DispOn(devId) != DISPLAY_SUCCESS) {
        HDF_LOGE("DispOn failed");
        return DISPLAY_FAILURE;
    }
    *layerId = LAYER_ID;
    return DISPLAY_SUCCESS;
}

static int32_t CloseLayer(uint32_t devId, uint32_t layerId)
{
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("layerId invalid");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t GetDisplayInfo(uint32_t devId, DisplayInfo *dispInfo)
{
    if (dispInfo == NULL) {
        HDF_LOGE("dispInfo is null");
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    struct DispInfo info = {0};
    if (DispGetInfo(devId, &info) != DISPLAY_SUCCESS) {
        HDF_LOGE("DispGetInfo failed");
        return DISPLAY_FAILURE;
    }
    dispInfo->width = info.width;
    dispInfo->height = info.height;
    dispInfo->rotAngle = ROTATE_NONE;
    return DISPLAY_SUCCESS;
}

static int32_t Flush(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("layerId invalid");
        return DISPLAY_FAILURE;
    }
    if (buffer == NULL) {
        HDF_LOGE("buffer is null");
        return DISPLAY_FAILURE;
    }
    DispFlush();
    return DISPLAY_SUCCESS;
}

static int32_t GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    if (buffer == NULL) {
        HDF_LOGE("buffer is null");
        return DISPLAY_NULL_PTR;
    }
    if (devId != DEV_ID) {
        HDF_LOGE("devId invalid");
        return DISPLAY_FAILURE;
    }
    if (layerId != LAYER_ID) {
        HDF_LOGE("layerId invalid");
        return DISPLAY_FAILURE;
    }
    struct LayerPrivate *priv = GetLayerInstance();
    buffer->fenceId = 0;
    buffer->width = priv->width;
    buffer->height = priv->height;
    buffer->pixFormat = priv->pixFmt;
    buffer->pitch = priv->pitch;
    // fbAddr may change
    priv->fbAddr = (void *)DispMmap(priv->fbSize);
    if (priv->fbAddr == NULL) {
        HDF_LOGE("mmap fb address failed, size 0x%x", priv->fbSize);
        priv->pitch = 0;
        priv->fbSize = 0;
        return DISPLAY_FAILURE;
    }
    buffer->data.phyAddr = (uint32_t)priv->fbAddr;
    buffer->data.virAddr = priv->fbAddr;
    if (buffer->data.virAddr == NULL) {
        HDF_LOGE("virAddr null");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

int32_t LayerInitialize(LayerFuncs **funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("funcs is null");
        return DISPLAY_NULL_PTR;
    }
    LayerFuncs *lFuncs = (LayerFuncs *)malloc(sizeof(LayerFuncs));
    if (lFuncs == NULL) {
        HDF_LOGE("lFuncs is null");
        return DISPLAY_NULL_PTR;
    }
    (void)memset(lFuncs, 0, sizeof(LayerFuncs));
    lFuncs->InitDisplay = InitDisplay;
    lFuncs->DeinitDisplay = DeinitDisplay;
    lFuncs->GetDisplayInfo = GetDisplayInfo;
    lFuncs->CreateLayer = OpenLayer;
    lFuncs->CloseLayer = CloseLayer;
    lFuncs->Flush = Flush;
    lFuncs->GetLayerBuffer = GetLayerBuffer;
    *funcs = lFuncs;
    return DISPLAY_SUCCESS;
}

int32_t LayerUninitialize(LayerFuncs *funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("funcs is null");
        return DISPLAY_NULL_PTR;
    }
    free(funcs);
    return DISPLAY_SUCCESS;
}
