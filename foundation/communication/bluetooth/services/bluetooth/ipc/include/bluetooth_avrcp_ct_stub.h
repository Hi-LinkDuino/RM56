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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_CT_STUB_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_CT_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_avrcp_ct.h"
#include "bluetooth_avrcp_ct_observer_proxy.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpCtStub : public IRemoteStub<IBluetoothAvrcpCt> {
public:
    BluetoothAvrcpCtStub();
    virtual ~BluetoothAvrcpCtStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode UnregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectedDevicesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode PressButtonnner(MessageParcel &data, MessageParcel &reply);
    ErrCode ReleaseButtonInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetUnitInfoInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetSubUnitInfoInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetSupportedCompaniesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetSupportedEventsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayerAppSettingAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayerAppSettingValuesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayerAppSettingAttributeTextInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayerAppSettingValueTextInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetElementAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode PlayItemInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetFolderItemsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetTotalNumberOfItemsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetAbsoluteVolumeInner(MessageParcel &data, MessageParcel &reply);
    ErrCode EnableNotificationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisableNotificationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetItemAttributesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetBrowsedPlayerInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetMeidaPlayerListInner(MessageParcel &data, MessageParcel &reply);

    bool IsInvalidAttributesSize(int32_t attributesSize);
    bool IsInvalidDeviceStatesSize(int32_t statesSize);

    using BluetoothAvrcpCtStubFunc = ErrCode (BluetoothAvrcpCtStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothAvrcpCtStubFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothAvrcpCtStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_CT_STUB_H