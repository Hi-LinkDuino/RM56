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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_avrcp_ct_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpCtObserverStub : public IRemoteStub<IBluetoothAvrcpCtObserver> {
public:
    BluetoothAvrcpCtObserverStub();
    virtual ~BluetoothAvrcpCtObserverStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPressButtonInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnReleaseButtonInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnSetBrowsedPlayerInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetCapabilitiesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayerAppSettingAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayerAppSettingValuesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnSetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayerAppSettingAttributeTextInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayerAppSettingValueTextInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetElementAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetPlayStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPlayItemInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetTotalNumberOfItemsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetItemAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnSetAbsoluteVolumeInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPlaybackStatusChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnTrackChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnTrackReachedEndInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnTrackReachedStartInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPlaybackPosChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPlayerAppSettingChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnNowPlayingContentChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnAvailablePlayersChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnAddressedPlayerChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnUidChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnVolumeChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetMediaPlayersInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnGetFolderItemsInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothAvrcpCtObserverStubFunc = ErrCode (BluetoothAvrcpCtObserverStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothAvrcpCtObserverStubFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothAvrcpCtObserverStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_CT_OBSERVER_STUB_H