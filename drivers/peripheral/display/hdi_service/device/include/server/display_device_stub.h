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

#ifndef _HDI_DISPLAY_HOST_DRIVER_H_
#define _HDI_DISPLAY_HOST_DRIVER_H_

#include <hdf_device_desc.h>
#include <memory>
#include <message_parcel.h>
#include <string>

#include "display_device.h"
#include "display_device_callback_proxy.h"
#include "display_device_common.h"
#include "display_device_service.h"

namespace OHOS {
namespace Display {
namespace Device {
namespace Server {
using OHOS::MessageParcel;
class DisplayDeviceServerStub {
public:
    DisplayDeviceServerStub();
    ~DisplayDeviceServerStub() = default;
    int32_t OnRemoteRequest(int cmdId, MessageParcel *data, MessageParcel *reply);
    // device
    int32_t RegHotPlugCallback(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayCapability(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplaySupportedModes(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayMode(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayMode(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayPowerStatus(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayPowerStatus(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayBackLight(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayBackLight(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayProperty(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayProperty(MessageParcel *data, MessageParcel *reply);
    int32_t PrepareDisplayLayers(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayCompChange(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayClientCrop(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayClientDestRect(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayClientBuffer(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayClientDamage(MessageParcel *data, MessageParcel *reply);
    int32_t SetDisplayVsyncEnabled(MessageParcel *data, MessageParcel *reply);
    int32_t RegDisplayVBlankCallback(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayReleaseFence(MessageParcel *data, MessageParcel *reply);
    int32_t Commit(MessageParcel *data, MessageParcel *reply);
    int32_t InvokeDisplayCmd(MessageParcel *data, MessageParcel *reply);
    int32_t CreateVirtualDisplay(MessageParcel *data, MessageParcel *reply);
    int32_t DestroyVirtualDisplay(MessageParcel *data, MessageParcel *reply);
    int32_t SetVirtualDisplayBuffer(MessageParcel *data, MessageParcel *reply);
    int32_t RegDisplayRefreshCallback(MessageParcel *data, MessageParcel *reply);
    int32_t GetWriteBackFrame(MessageParcel *data, MessageParcel *reply);
    int32_t CreateWriteBack(MessageParcel *data, MessageParcel *reply);
    int32_t DestroyWriteBack(MessageParcel *data, MessageParcel *reply);
    int32_t FileTest(MessageParcel *data, MessageParcel *reply);
    int32_t SetProxyRemoteCallback(MessageParcel *data, MessageParcel *reply);
    // layer
    int32_t CreateLayer(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerVisible(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerVisibleState(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerCrop(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerZorder(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerZorder(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerPreMulti(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerPreMulti(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerAlpha(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerAlpha(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerColorKey(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerColorKey(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerPalette(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerPalette(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerCompression(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerCompression(MessageParcel *data, MessageParcel *reply);
    int32_t Flush(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerVisibleRegion(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerDirtyRegion(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerBuffer(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerBuffer(MessageParcel *data, MessageParcel *reply);
    int32_t InvokeLayerCmd(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerCompositionType(MessageParcel *data, MessageParcel *reply);
    int32_t InitDisplay(MessageParcel *data, MessageParcel *reply);
    int32_t DeinitDisplay(MessageParcel *data, MessageParcel *reply);
    int32_t GetDisplayInfo(MessageParcel *data, MessageParcel *reply);
    int32_t CloseLayer(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerSize(MessageParcel *data, MessageParcel *reply);
    int32_t GetLayerSize(MessageParcel *data, MessageParcel *reply);
    int32_t SetTransformMode(MessageParcel *data, MessageParcel *reply);
    int32_t WaitForVBlank(MessageParcel *data, MessageParcel *reply);
    int32_t SnapShot(MessageParcel *data, MessageParcel *reply);
    int32_t SetLayerBlendType(MessageParcel *data, MessageParcel *reply);
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }

private:
    static inline const std::u16string metaDescriptor_ = IDisplayDevice::GetDescriptor();
    int32_t SetCallBackObject(sptr<IRemoteObject> callbackRemote);
    sptr<DisplayDeviceCallbackProxy> callbackRemote_;
    std::unique_ptr<DisplayDeviceService> device_;
};

using DisplayDeviceServerStubFunc = int32_t (DisplayDeviceServerStub::*)(MessageParcel *, MessageParcel *);
} // namespace Server
} // namespace Device
} // namespace Display
} // namespace OHOS
#endif // _HDI_DISPLAY_HOST_DRIVER_H_
