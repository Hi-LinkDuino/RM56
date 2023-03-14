/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_TG_STUB_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_TG_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_avrcp_tg.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpTgStub : public IRemoteStub<IBluetoothAvrcpTg> {
public:
    BluetoothAvrcpTgStub();
    virtual ~BluetoothAvrcpTgStub();

    ErrCode OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode UnregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetActiveDeviceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectedDevicesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyPlaybackStatusChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyTrackChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyTrackReachedEndInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyTrackReachedStartInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyPlaybackPosChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyPlayerAppSettingChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyNowPlayingContentChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyAvailablePlayersChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyAddressedPlayerChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyUidChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyVolumeChangedInner(MessageParcel &data, MessageParcel &reply);

    bool IsInvalidAttributesSize(int32_t attributesSize);
    bool IsInvalidDeviceStatesSize(int32_t statesSize);

    using BluetoothHostFunc = ErrCode (BluetoothAvrcpTgStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHostFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothAvrcpTgStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_TG_STUB_H