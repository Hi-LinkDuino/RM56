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


#ifndef _HDI_DISPLAY_HOST_PROXY_BASE_H_
#define _HDI_DISPLAY_HOST_PROXY_BASE_H_

#include <iremote_proxy.h>
#include "display_device_common.h"
#include "ihdi_display_register_callback_framework.h"

#undef HDF_LOG_TAG
#define HDF_LOG_TAG DisplayHostProxyBase

using ComposerServerTmp = struct {
    DisplayDeviceCommandId cmdId;
    int32_t returnVal;
    int32_t arrayNum;
};

namespace OHOS {
namespace Display {
namespace Device {
using OHOS::MessageOption;
using OHOS::MessageParcel;
using OHOS::sptr;
class IDisplayDevice : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DISPLAY.Device.Client.IDisplayDevice.V1_0");
    virtual ~IDisplayDevice() = default;
    static void Initialize(void);
    /* device proxy */
    virtual int32_t RegHotPlugCallback(HotPlugCallback callback, void *data) = 0;
    virtual int32_t GetDisplayCapability(uint32_t devId, DisplayCapability &info) = 0;
    virtual int32_t GetDisplaySupportedModes(uint32_t devId, uint32_t &num, DisplayModeInfo *modes) = 0;
    virtual int32_t GetDisplayMode(uint32_t devId, uint32_t &modeId) = 0;
    virtual int32_t SetDisplayMode(uint32_t devId, uint32_t modeId) = 0;
    virtual int32_t GetDisplayPowerStatus(uint32_t devId, DispPowerStatus &status) = 0;
    virtual int32_t SetDisplayPowerStatus(uint32_t devId, DispPowerStatus status) = 0;
    virtual int32_t GetDisplayBacklight(uint32_t devId, uint32_t &level) = 0;
    virtual int32_t SetDisplayBacklight(uint32_t devId, uint32_t level) = 0;
    virtual int32_t GetDisplayProperty(uint32_t devId, uint32_t id, uint64_t &value) = 0;
    virtual int32_t SetDisplayProperty(uint32_t devId, uint32_t id, uint64_t value) = 0;
    virtual int32_t PrepareDisplayLayers(uint32_t devId, bool &needFlushFb) = 0;
    virtual int32_t GetDisplayCompChange(uint32_t devId, uint32_t &num, uint32_t *layers, int32_t *type) = 0;
    virtual int32_t SetDisplayClientCrop(uint32_t devId, const IRect *rect) = 0;
    virtual int32_t SetDisplayClientDestRect(uint32_t devId, const IRect &rect) = 0;
    virtual int32_t SetDisplayClientBuffer(uint32_t devId, const BufferHandle &buffer, int32_t fence) = 0;
    virtual int32_t SetDisplayClientDamage(uint32_t devId, uint32_t num, const IRect &rect) = 0;
    virtual int32_t SetDisplayVsyncEnabled(uint32_t devId, bool enabled) = 0;
    virtual int32_t RegDisplayVBlankCallback(uint32_t devId, VBlankCallback callback, void *data) = 0;
    virtual int32_t GetDisplayReleaseFence(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *fences) = 0;
    virtual int32_t Commit(uint32_t devId, int32_t &fence) = 0;
    virtual int32_t InvokeDisplayCmd(uint32_t devId, ...) = 0;
    virtual int32_t CreateVirtualDisplay(uint32_t width, uint32_t height, int32_t &format, uint32_t &devId) = 0;
    virtual int32_t DestroyVirtualDisplay(uint32_t devId) = 0;
    virtual int32_t SetVirtualDisplayBuffer(uint32_t devId, const BufferHandle *buffer, int32_t fence) = 0;
    virtual int32_t RegDisplayRefreshCallback(uint32_t devId, RefreshCallback callback, void *data) = 0;
    virtual int32_t GetWriteBackFrame(uint32_t devId, BufferHandle &buffer, int32_t &fence) = 0;
    virtual int32_t CreateWriteBack(uint32_t &devId, uint32_t width, uint32_t height, int32_t &format) = 0;
    virtual int32_t DestroyWriteBack(uint32_t devId) = 0;
    virtual int32_t SetProxyRemoteCallback(const OHOS::sptr<DisplayRegisterCallbackBase> &callback) = 0;
    /* layer proxy */
    virtual int32_t CreateLayer(uint32_t devId, const LayerInfo &layerInfo, uint32_t &layerId) = 0;
    virtual int32_t SetLayerVisible(uint32_t devId, uint32_t layerId, bool visible) = 0;
    virtual int32_t GetLayerVisibleState(uint32_t devId, uint32_t layerId, bool &visible) = 0;
    virtual int32_t SetLayerCrop(uint32_t devId, uint32_t layerId, const IRect *rect) = 0;
    virtual int32_t SetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t zorder) = 0;
    virtual int32_t GetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t &zorder) = 0;
    virtual int32_t SetLayerPreMulti(uint32_t devId, uint32_t layerId, bool preMul) = 0;
    virtual int32_t GetLayerPreMulti(uint32_t devId, uint32_t layerId, bool &preMul) = 0;
    virtual int32_t SetLayerAlpha(uint32_t devId, uint32_t layerId, const LayerAlpha &alpha) = 0;
    virtual int32_t GetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha &alpha) = 0;
    virtual int32_t SetLayerColorKey(uint32_t devId, uint32_t layerId, bool enable, uint32_t key) = 0;
    virtual int32_t GetLayerColorKey(uint32_t devId, uint32_t layerId, bool *enable, uint32_t *key) = 0;
    virtual int32_t SetLayerPalette(uint32_t devId, uint32_t layerId, const uint32_t *palette, uint32_t len) = 0;
    virtual int32_t GetLayerPalette(uint32_t devId, uint32_t layerId, uint32_t &palette, uint32_t len) = 0;
    virtual int32_t SetLayerCompression(uint32_t devId, uint32_t layerId, int32_t compType) = 0;
    virtual int32_t GetLayerCompression(uint32_t devId, uint32_t layerId, int32_t &compType) = 0;
    virtual int32_t Flush(uint32_t devId, uint32_t layerId, LayerBuffer &buffer) = 0;
    virtual int32_t SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &region) = 0;
    virtual int32_t GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer) = 0;
    virtual int32_t SetTransformMode(uint32_t devId, uint32_t layerId, TransformType type) = 0;
    virtual int32_t CloseLayer(uint32_t devId, uint32_t layerId) = 0;
    virtual int32_t SetLayerSize(uint32_t devId, uint32_t layerId, const IRect *rect) = 0;
    virtual int32_t GetLayerSize(uint32_t devId, uint32_t layerId, IRect &rect) = 0;
    virtual int32_t WaitForVBlank(uint32_t devId, uint32_t layerId, int32_t timeOut) = 0;
    virtual int32_t InitDisplay(uint32_t devId) = 0;
    virtual int32_t DeinitDisplay(uint32_t devId) = 0;
    virtual int32_t GetDisplayInfo(uint32_t devId, DisplayInfo &dispInfo) = 0;
    virtual int32_t SnapShot(uint32_t devId, LayerBuffer &buffer) = 0;
    virtual int32_t SetLayerVisibleRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &rect) = 0;
    virtual int32_t SetLayerBuffer(uint32_t devId, uint32_t layerId, const BufferHandle &buffer, int32_t fence) = 0;
    virtual int32_t InvokeLayerCmd(uint32_t devId, uint32_t layerId, uint32_t cmd, ...) = 0;
    virtual int32_t SetLayerCompositionType(uint32_t devId, uint32_t layerId, CompositionType type) = 0;
    virtual int32_t SetLayerBlendType(uint32_t devId, uint32_t layerId, BlendType type) = 0;
    sptr<DisplayRegisterCallbackFramework> m_hotPlugObject_;
};
} // namespace Device
} // namespace Display
} // namespace OHOS

#endif // _HDI_DISPLAY_HOST_PROXY_BASE_H_
