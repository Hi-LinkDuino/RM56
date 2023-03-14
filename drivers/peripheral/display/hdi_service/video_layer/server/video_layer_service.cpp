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

#include "video_layer_service.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include "display_gralloc.h"
#include "display_layer.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
static LayerFuncs *g_layerFuncs = nullptr;
static GrallocFuncs *g_grallocFuncs = nullptr;

DispErrCode VideoLayerService::InitDisplay(unsigned int devId)
{
    if (g_layerFuncs != nullptr) {
        return DISPLAY_SUCCESS;
    }

    int32_t ret = LayerInitialize(&g_layerFuncs);
    if (ret != DISPLAY_SUCCESS || g_layerFuncs == nullptr) {
        HDF_LOGE("layer init fail, ret=%{public}d",  ret);
        return DISPLAY_FAILURE;
    }

    if (g_layerFuncs->InitDisplay == nullptr || g_layerFuncs->CreateLayer == nullptr ||
        g_layerFuncs->GetDisplayInfo == nullptr) {
        HDF_LOGE("layer func is invalid");
        return DISPLAY_FAILURE;
    }

    ret = g_layerFuncs->InitDisplay(devId);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("init display fail, ret=%{public}d",  ret);
        return DISPLAY_FAILURE;
    }

    if (g_grallocFuncs == nullptr) {
        ret = GrallocInitialize(&g_grallocFuncs);
        if (ret != DISPLAY_SUCCESS || g_grallocFuncs == nullptr) {
            LayerUninitialize(g_layerFuncs);
            g_layerFuncs = nullptr;
            HDF_LOGE("%{public}s: GrallocInitialize fail, ret=%{public}d", __func__, ret);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerService::DeinitDisplay(unsigned int devId)
{
    if (g_layerFuncs != nullptr && g_layerFuncs->DeinitDisplay != nullptr) {
        (void)g_layerFuncs->DeinitDisplay(devId);
    }
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerService::GetDisplayInfo(unsigned int devId, std::shared_ptr<DisplayInfo> &dispInfo)
{
    if (g_layerFuncs == nullptr) {
        HDF_LOGE("video display not inited");
        return DISPLAY_FAILURE;
    }

    int32_t ret = g_layerFuncs->GetDisplayInfo(devId, dispInfo.get());
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("get display info fail, ret=%{public}d",  ret);
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerService::CreateLayer(unsigned int devId, LayerInfo &layerInfo, unsigned int &layerId)
{
    int32_t ret;
    LayerInfo info = layerInfo;
    if (g_layerFuncs == nullptr) {
        ret = InitDisplay(devId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("InitDisplay fail, ret=%{public}d",  ret);
            return DISPLAY_FAILURE;
        }
        std::shared_ptr<DisplayInfo> dispInfo = std::make_shared<DisplayInfo>();
        ret = GetDisplayInfo(devId, dispInfo);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("InitDisplay fail, ret=%{public}d",  ret);
            return DISPLAY_FAILURE;
        }
        if (info.height == 0 && info.width == 0) {
            info.width = dispInfo->width;
            info.height = dispInfo->height;
        }
    }

    ret = g_layerFuncs->CreateLayer(devId, &info, &layerId);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("create layer fail, ret=%{public}d",  ret);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerService::CloseLayer(unsigned int devId, unsigned int layerId)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->CloseLayer == nullptr) {
        HDF_LOGE("may not inited or CloseLayer nullptr");
        return DISPLAY_FAILURE;
    }
    (void)g_layerFuncs->CloseLayer(devId, layerId);
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerService::SetLayerVisible(unsigned int devId, unsigned int layerId, bool visible)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->SetLayerVisible == nullptr) {
        HDF_LOGE("may not inited or SetLayerVisible nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->SetLayerVisible(devId, layerId, visible);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::GetLayerVisibleState(unsigned int devId, unsigned int layerId, bool &visible)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->GetLayerVisibleState == nullptr) {
        HDF_LOGE("may not inited or GetLayerVisibleState nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->GetLayerVisibleState(devId, layerId, &visible);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::SetLayerRect(unsigned int devId, unsigned int layerId, IRect &rect)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->SetLayerSize == nullptr) {
        HDF_LOGE("may not inited or SetLayerSize nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->SetLayerSize(devId, layerId, &rect);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::GetLayerRect(unsigned int devId, unsigned int layerId, std::shared_ptr<IRect> &rect)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->GetLayerSize == nullptr) {
        HDF_LOGE("may not inited or GetLayerSize nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->GetLayerSize(devId, layerId, rect.get());

    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::SetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int zorder)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->SetLayerZorder == nullptr) {
        HDF_LOGE("may not inited or SetLayerZorder nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->SetLayerZorder(devId, layerId, zorder);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::GetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int &zorder)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->GetLayerZorder == nullptr) {
        HDF_LOGE("may not inited or GetLayerZorder nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->GetLayerZorder(devId, layerId, &zorder);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::SetTransformMode(unsigned int devId, unsigned int layerId, TransformType &type)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->SetTransformMode == nullptr) {
        HDF_LOGE("may not inited or SetTransformMode nullptr");
        return DISPLAY_FAILURE;
    }
    int32_t ret = g_layerFuncs->SetTransformMode(devId, layerId, type);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}

DispErrCode VideoLayerService::SetLayerBuffer(unsigned int devId, unsigned int layerId, const BufferHandle &buffer,
    int fence)
{
    if (g_layerFuncs == nullptr || g_layerFuncs->SetLayerBuffer == nullptr) {
        HDF_LOGE("may not inited or SetLayerBuffer nullptr");
        return DISPLAY_FAILURE;
    }
    BufferHandle bufferTemp = buffer;
    bufferTemp.virAddr = g_grallocFuncs->Mmap(&bufferTemp);
    int32_t ret = g_layerFuncs->SetLayerBuffer(devId, layerId, &bufferTemp, fence);
    (void)g_grallocFuncs->Unmap(&bufferTemp);
    return (ret == DISPLAY_SUCCESS) ? DISPLAY_SUCCESS : DISPLAY_FAILURE;
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS
