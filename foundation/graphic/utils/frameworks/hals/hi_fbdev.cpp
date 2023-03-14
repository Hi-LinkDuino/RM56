/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "hals/hi_fbdev.h"

#include "display_layer.h"
#include "display_type.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"

namespace OHOS {
struct DisplayDesc {
    LayerFuncs* layerFuncs;
    uint32_t devId;
    uint32_t layerId;
    LayerBuffer buffer;
    LayerRotateType rotateType;
};

static LayerInfo g_layerInfo = {};
static DisplayDesc g_display = {};
constexpr const uint8_t DISPALY_DEV_ID = 0;
#ifdef LAYER_PF_ARGB1555
constexpr const uint8_t LAYER_BPP = 16;
constexpr const PixelFormat HDI_LAYER_PIXEL_FORMAT = PIXEL_FMT_RGBA_5551;
constexpr const ImagePixelFormat LAYER_PIXEL_FORMAT = IMAGE_PIXEL_FORMAT_ARGB1555;
#elif defined LAYER_PF_ARGB8888
constexpr const uint8_t LAYER_BPP = 32;
constexpr const PixelFormat HDI_LAYER_PIXEL_FORMAT = PIXEL_FMT_RGBA_8888;
constexpr const ImagePixelFormat LAYER_PIXEL_FORMAT = IMAGE_PIXEL_FORMAT_ARGB8888;
#endif
constexpr const uint8_t BITS_PER_BYTE = 8;
static LiteSurfaceData g_devSurfaceData = {};

void LcdFlush()
{
    if (g_display.layerFuncs->Flush != nullptr) {
        int32_t ret =
            g_display.layerFuncs->Flush(g_display.devId, g_display.layerId, &g_display.buffer);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("flush fail");
            return;
        }
    }
}

LayerRotateType GetLayerRotateType()
{
    return g_display.rotateType;
}

LiteSurfaceData* GetDevSurfaceData()
{
    return &g_devSurfaceData;
}

static void DisplayInit(void)
{
    int32_t ret = LayerInitialize(&g_display.layerFuncs);
    if (ret != DISPLAY_SUCCESS || g_display.layerFuncs == nullptr) {
        GRAPHIC_LOGE("layer initialize failed");
        return;
    }
    if (g_display.layerFuncs->InitDisplay != nullptr) {
        ret = g_display.layerFuncs->InitDisplay(DISPALY_DEV_ID);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("InitDisplay fail");
            return;
        }
    }
}

static void OpenLayer(void)
{
    if (g_display.layerFuncs->GetDisplayInfo == nullptr) {
        return;
    }
    g_display.devId = DISPALY_DEV_ID;
    DisplayInfo displayInfo = {};
    int32_t ret = g_display.layerFuncs->GetDisplayInfo(g_display.devId, &displayInfo);
    if (ret != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("GetDisplayInfo fail");
        return;
    }
    g_display.rotateType = static_cast<LayerRotateType>(displayInfo.rotAngle);
    g_layerInfo.width = displayInfo.width;
    g_layerInfo.height = displayInfo.height;
    g_layerInfo.bpp = LAYER_BPP;
    g_layerInfo.pixFormat = HDI_LAYER_PIXEL_FORMAT;
    g_layerInfo.type = LAYER_TYPE_GRAPHIC;
    if (g_display.layerFuncs->CreateLayer != nullptr) {
        ret = g_display.layerFuncs->CreateLayer(g_display.devId, &g_layerInfo, &g_display.layerId);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("CreateLayer fail");
            return;
        }
    }
}

static void SetLayerVisible(bool visibled)
{
    if (g_display.layerFuncs->SetLayerVisible != nullptr) {
        int32_t ret = g_display.layerFuncs->SetLayerVisible(g_display.devId, g_display.layerId, visibled);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("setLayerVisible fail");
            return;
        }
    }
}

static void SetLayerDirtyRegion(void)
{
    IRect rect = {0, 0, g_layerInfo.width, g_layerInfo.height};
    if (g_display.layerFuncs->SetLayerDirtyRegion != nullptr) {
        int32_t ret = g_display.layerFuncs->SetLayerDirtyRegion(g_display.devId, g_display.layerId, &rect);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("setLayerDirtyRegion fail");
            return;
        }
    }
}

static void AllocDisplayBuffer(void)
{
    if (g_display.layerFuncs->GetLayerBuffer != nullptr) {
        int32_t ret =
            g_display.layerFuncs->GetLayerBuffer(g_display.devId, g_display.layerId, &g_display.buffer);
        if (ret != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("getLayerBuffer fail");
            return;
        }
    }
}

void HiFbdevInit()
{
    DisplayInit();
    OpenLayer();
    SetLayerVisible(true);
    SetLayerDirtyRegion();
    AllocDisplayBuffer();
    uintptr_t phyAddr = g_display.buffer.data.phyAddr;
    g_devSurfaceData.phyAddr = reinterpret_cast<uint8_t*>(phyAddr);
    g_devSurfaceData.pixelFormat = LAYER_PIXEL_FORMAT;
    g_devSurfaceData.width = g_layerInfo.width;
    g_devSurfaceData.height = g_layerInfo.height;
    g_devSurfaceData.stride = g_display.buffer.pitch;
    g_devSurfaceData.virAddr = static_cast<uint8_t*>(g_display.buffer.data.virAddr);
    g_devSurfaceData.bytePerPixel = g_layerInfo.bpp / BITS_PER_BYTE;
}

void HiFbdevClose()
{
    if (g_display.layerFuncs->CloseLayer == nullptr) {
        return;
    }
    if (g_display.layerFuncs->DeinitDisplay == nullptr) {
        return;
    }
    int32_t ret = g_display.layerFuncs->CloseLayer(g_display.devId, g_display.layerId);
    if (ret != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("CloseLayer fail");
        return;
    }
    ret = g_display.layerFuncs->DeinitDisplay(g_display.devId);
    if (ret != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("DeinitDisplay fail");
        return;
    }
    ret = LayerUninitialize(g_display.layerFuncs);
    if (ret != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("LayerUninitialize fail");
    }
}
} // namespace OHOS
