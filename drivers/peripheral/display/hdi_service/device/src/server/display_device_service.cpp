/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <hdf_base.h>
#include "display_device_service.h"

namespace OHOS {
namespace Display {
namespace Device {
namespace Server {
DisplayDeviceService::DisplayDeviceService() : status_(0)
{
    if (DeviceInitialize(&displayDevice_) != HDF_SUCCESS) {
        status_ = HDF_FAILURE;
        return;
    }

    status_ = LayerInitialize(&displayLayer_);
}

DisplayDeviceService::~DisplayDeviceService()
{
    if (displayDevice_ != nullptr) {
        DeviceUninitialize(displayDevice_);
    }
    if (displayLayer_ != nullptr) {
        LayerUninitialize(displayLayer_);
    }
}

/* device proxy */
int32_t DisplayDeviceService::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    return displayDevice_->RegHotPlugCallback(callback, data);
}

int32_t DisplayDeviceService::GetDisplayCapability(uint32_t devId, DisplayCapability &info)
{
    return displayDevice_->GetDisplayCapability(devId, &info);
}

int32_t DisplayDeviceService::GetDisplaySupportedModes(uint32_t devId, uint32_t &num, DisplayModeInfo *modes)
{
    return displayDevice_->GetDisplaySupportedModes(devId, &num, modes);
}

int32_t DisplayDeviceService::GetDisplayMode(uint32_t devId, uint32_t &modeId)
{
    return displayDevice_->GetDisplayMode(devId, &modeId);
}

int32_t DisplayDeviceService::SetDisplayMode(uint32_t devId, uint32_t modeId)
{
    return displayDevice_->SetDisplayMode(devId, modeId);
}

int32_t DisplayDeviceService::GetDisplayPowerStatus(uint32_t devId, DispPowerStatus &status)
{
    return displayDevice_->GetDisplayPowerStatus(devId, &status);
}

int32_t DisplayDeviceService::SetDisplayPowerStatus(uint32_t devId, DispPowerStatus status)
{
    return displayDevice_->SetDisplayPowerStatus(devId, status);
}

int32_t DisplayDeviceService::GetDisplayBacklight(uint32_t devId, uint32_t &level)
{
    return displayDevice_->GetDisplayBacklight(devId, &level);
}

int32_t DisplayDeviceService::SetDisplayBacklight(uint32_t devId, uint32_t level)
{
    return displayDevice_->SetDisplayBacklight(devId, level);
}

int32_t DisplayDeviceService::GetDisplayProperty(uint32_t devId, uint32_t id, uint64_t &value)
{
    return displayDevice_->GetDisplayProperty(devId, id, &value);
}

int32_t DisplayDeviceService::SetDisplayProperty(uint32_t devId, uint32_t id, uint64_t value)
{
    return displayDevice_->SetDisplayProperty(devId, id, value);
}

int32_t DisplayDeviceService::PrepareDisplayLayers(uint32_t devId, bool &needFlushFb)
{
    return displayDevice_->PrepareDisplayLayers(devId, &needFlushFb);
}

int32_t DisplayDeviceService::GetDisplayCompChange(uint32_t devId, uint32_t &num, uint32_t *layers, int32_t *type)
{
    return displayDevice_->GetDisplayCompChange(devId, &num, layers, type);
}

int32_t DisplayDeviceService::SetDisplayClientCrop(uint32_t devId, const IRect *rect)
{
    return displayDevice_->SetDisplayClientCrop(devId, const_cast<IRect *>(rect));
}

int32_t DisplayDeviceService::SetDisplayClientDestRect(uint32_t devId, const IRect &rect)
{
    return displayDevice_->SetDisplayClientDestRect(devId, const_cast<IRect *>(&rect));
}

int32_t DisplayDeviceService::SetDisplayClientBuffer(uint32_t devId, const BufferHandle &buffer, int32_t fence)
{
    return displayDevice_->SetDisplayClientBuffer(devId, &buffer, fence);
}

int32_t DisplayDeviceService::SetDisplayClientDamage(uint32_t devId, uint32_t num, const IRect &rect)
{
    return displayDevice_->SetDisplayClientDamage(devId, num, const_cast<IRect *>(&rect));
}

int32_t DisplayDeviceService::SetDisplayVsyncEnabled(uint32_t devId, bool enabled)
{
    return displayDevice_->SetDisplayVsyncEnabled(devId, enabled);
}

int32_t DisplayDeviceService::RegDisplayVBlankCallback(uint32_t devId, VBlankCallback callback, void *data)
{
    return displayDevice_->RegDisplayVBlankCallback(devId, callback, data);
}

int32_t DisplayDeviceService::GetDisplayReleaseFence(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *fences)
{
    return displayDevice_->GetDisplayReleaseFence(devId, num, layers, fences);
}

int32_t DisplayDeviceService::Commit(uint32_t devId, int32_t &fence)
{
    return displayDevice_->Commit(devId, &fence);
}

int32_t DisplayDeviceService::InvokeDisplayCmd(uint32_t devId, ...)
{
    (void)devId;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t DisplayDeviceService::CreateVirtualDisplay(uint32_t width, uint32_t height, int32_t &format, uint32_t &devId)
{
    return displayDevice_->CreateVirtualDisplay(width, height, &format, &devId);
}

int32_t DisplayDeviceService::DestroyVirtualDisplay(uint32_t devId)
{
    return displayDevice_->DestroyVirtualDisplay(devId);
}

int32_t DisplayDeviceService::SetVirtualDisplayBuffer(uint32_t devId, const BufferHandle *buffer, int32_t fence)
{
    return displayDevice_->SetVirtualDisplayBuffer(devId, const_cast<BufferHandle *>(buffer), fence);
}

int32_t DisplayDeviceService::RegDisplayRefreshCallback(uint32_t devId, RefreshCallback callback, void *data)
{
    if (displayDevice_->RegDisplayRefreshCallback == nullptr) {
        return DISPLAY_NOT_SUPPORT;
    }
    return displayDevice_->RegDisplayRefreshCallback(devId, callback, data);
}

int32_t DisplayDeviceService::GetWriteBackFrame(uint32_t devId, BufferHandle &buffer, int32_t &fence)
{
    (void)devId;
    (void)buffer;
    (void)fence;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t DisplayDeviceService::CreateWriteBack(uint32_t &devId, uint32_t width, uint32_t height, int32_t &format)
{
    (void)devId;
    (void)width;
    (void)height;
    (void)format;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t DisplayDeviceService::DestroyWriteBack(uint32_t devId)
{
    (void)devId;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t DisplayDeviceService::SetProxyRemoteCallback(const OHOS::sptr<DisplayRegisterCallbackBase> &callback)
{
    (void)callback;
    return HDF_ERR_NOT_SUPPORT;
}

/* layer proxy */
int32_t DisplayDeviceService::CreateLayer(uint32_t devId, const LayerInfo &layerInfo, uint32_t &layerId)
{
    return displayLayer_->CreateLayer(devId, &layerInfo, &layerId);
}

int32_t DisplayDeviceService::SetLayerVisible(uint32_t devId, uint32_t layerId, bool visible)
{
    return displayLayer_->SetLayerVisible(devId, layerId, visible);
}

int32_t DisplayDeviceService::GetLayerVisibleState(uint32_t devId, uint32_t layerId, bool &visible)
{
    return displayLayer_->GetLayerVisibleState(devId, layerId, &visible);
}

int32_t DisplayDeviceService::SetLayerCrop(uint32_t devId, uint32_t layerId, const IRect *rect)
{
    return displayLayer_->SetLayerCrop(devId, layerId, const_cast<IRect *>(rect));
}

int32_t DisplayDeviceService::SetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t zorder)
{
    return displayLayer_->SetLayerZorder(devId, layerId, zorder);
}

int32_t DisplayDeviceService::GetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t &zorder)
{
    return displayLayer_->GetLayerZorder(devId, layerId, &zorder);
}

int32_t DisplayDeviceService::SetLayerPreMulti(uint32_t devId, uint32_t layerId, bool preMul)
{
    return displayLayer_->SetLayerPreMulti(devId, layerId, preMul);
}

int32_t DisplayDeviceService::GetLayerPreMulti(uint32_t devId, uint32_t layerId, bool &preMul)
{
    return displayLayer_->GetLayerPreMulti(devId, layerId, &preMul);
}

int32_t DisplayDeviceService::SetLayerAlpha(uint32_t devId, uint32_t layerId, const LayerAlpha &alpha)
{
    return displayLayer_->SetLayerAlpha(devId, layerId, const_cast<LayerAlpha *>(&alpha));
}

int32_t DisplayDeviceService::GetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha &alpha)
{
    return displayLayer_->GetLayerAlpha(devId, layerId, &alpha);
}

int32_t DisplayDeviceService::SetLayerColorKey(uint32_t devId, uint32_t layerId, bool enable, uint32_t key)
{
    return displayLayer_->SetLayerColorKey(devId, layerId, enable, key);
}

int32_t DisplayDeviceService::GetLayerColorKey(uint32_t devId, uint32_t layerId, bool *enable, uint32_t *key)
{
    return displayLayer_->GetLayerColorKey(devId, layerId, enable, key);
}

int32_t DisplayDeviceService::SetLayerPalette(uint32_t devId, uint32_t layerId, const uint32_t *palette, uint32_t len)
{
    return displayLayer_->SetLayerPalette(devId, layerId, const_cast<uint32_t *>(palette), len);
}

int32_t DisplayDeviceService::GetLayerPalette(uint32_t devId, uint32_t layerId, uint32_t &palette, uint32_t len)
{
    return displayLayer_->GetLayerPalette(devId, layerId, &palette, len);
}

int32_t DisplayDeviceService::SetLayerCompression(uint32_t devId, uint32_t layerId, int32_t compType)
{
    return displayLayer_->SetLayerCompression(devId, layerId, compType);
}

int32_t DisplayDeviceService::GetLayerCompression(uint32_t devId, uint32_t layerId, int32_t &compType)
{
    return displayLayer_->GetLayerCompression(devId, layerId, &compType);
}

int32_t DisplayDeviceService::Flush(uint32_t devId, uint32_t layerId, LayerBuffer &buffer)
{
    return displayLayer_->Flush(devId, layerId, &buffer);
}

int32_t DisplayDeviceService::SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &region)
{
    (void)layerId;
    (void)num;
    return displayLayer_->SetLayerDirtyRegion(devId, layerId, const_cast<IRect *>(&region));
}

int32_t DisplayDeviceService::GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    return displayLayer_->GetLayerBuffer(devId, layerId, buffer);
}

int32_t DisplayDeviceService::SetTransformMode(uint32_t devId, uint32_t layerId, TransformType type)
{
    return displayLayer_->SetTransformMode(devId, layerId, type);
}

int32_t DisplayDeviceService::CloseLayer(uint32_t devId, uint32_t layerId)
{
    return displayLayer_->CloseLayer(devId, layerId);
}

int32_t DisplayDeviceService::SetLayerSize(uint32_t devId, uint32_t layerId, const IRect *rect)
{
    return displayLayer_->SetLayerSize(devId, layerId, const_cast<IRect *>(rect));
}

int32_t DisplayDeviceService::GetLayerSize(uint32_t devId, uint32_t layerId, IRect &rect)
{
    return displayLayer_->GetLayerSize(devId, layerId, &rect);
}

int32_t DisplayDeviceService::WaitForVBlank(uint32_t devId, uint32_t layerId, int32_t timeOut)
{
    return displayLayer_->WaitForVBlank(devId, layerId, timeOut);
}

int32_t DisplayDeviceService::InitDisplay(uint32_t devId)
{
    return displayLayer_->InitDisplay(devId);
}

int32_t DisplayDeviceService::DeinitDisplay(uint32_t devId)
{
    return displayLayer_->DeinitDisplay(devId);
}

int32_t DisplayDeviceService::GetDisplayInfo(uint32_t devId, DisplayInfo &dispInfo)
{
    return displayLayer_->GetDisplayInfo(devId, &dispInfo);
}

int32_t DisplayDeviceService::SnapShot(uint32_t devId, LayerBuffer &buffer)
{
    return displayLayer_->SnapShot(devId, &buffer);
}

int32_t DisplayDeviceService::SetLayerVisibleRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &rect)
{
    return displayLayer_->SetLayerVisibleRegion(devId, layerId, num, const_cast<IRect *>(&rect));
}

int32_t DisplayDeviceService::SetLayerBuffer(
    uint32_t devId, uint32_t layerId, const BufferHandle &buffer, int32_t fence)
{
    return displayLayer_->SetLayerBuffer(devId, layerId, &buffer, fence);
}

int32_t DisplayDeviceService::InvokeLayerCmd(uint32_t devId, uint32_t layerId, uint32_t cmd, ...)
{
    (void)devId;
    (void)layerId;
    (void)cmd;
    return HDF_ERR_NOT_SUPPORT;
}

int32_t DisplayDeviceService::SetLayerCompositionType(uint32_t devId, uint32_t layerId, CompositionType type)
{
    return displayLayer_->SetLayerCompositionType(devId, layerId, type);
}

int32_t DisplayDeviceService::SetLayerBlendType(uint32_t devId, uint32_t layerId, BlendType type)
{
    return displayLayer_->SetLayerBlendType(devId, layerId, type);
}

bool DisplayDeviceService::IsValid()
{
    return status_ == HDF_SUCCESS;
}
} // namespace Server
} // namespace Device
} // namespace Display
} // namespace OHOS
