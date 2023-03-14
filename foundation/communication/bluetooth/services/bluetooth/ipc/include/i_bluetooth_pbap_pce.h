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
#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PCE_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PCE_INTERFACE_H

#include "../parcel/bluetooth_pbap_pce_parameter.h"
#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"
#include "pbap_pce_parameter.h"
#include "i_bluetooth_pbap_pce_observer.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothPbapPce : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothPbapPce");

    enum Code {
        PBAP_PCE_GET_DEVICE_STATE = 0,
        PBAP_PCE_CONNECT,
        PBAP_PCE_PULL_PHONEBOOK,
        PBAP_PCE_SET_PHONEBOOK,
        PBAP_PCE_PULL_VCARD_LISTING,
        PBAP_PCE_PULL_VCARD_ENTRY,
        PBAP_PCE_PULL_ISDOWNLOAGING,
        PBAP_PCE_ABORT_DOWNLOADING,
        PBAP_PCE_SET_DEVICE_PASSWORD,
        PBAP_PCE_DISCONNECT,
        PBAP_PCE_SET_CONNECT_STRATEGY,
        PBAP_PCE_GET_CONNECT_STRATEGY,
        PBAP_PCE_GET_DEVICES_BY_STATE,
        PBAP_PCE_REGISTER_OBSERVER,
        PBAP_PCE_DEREGISTER_OBSERVER,
    };

    virtual int GetDeviceState(const BluetoothRawAddress &device) = 0;
    virtual int Connect(const BluetoothRawAddress &device) = 0;
    virtual int PullPhoneBook(const BluetoothRawAddress &device, const BluetoothIPbapPullPhoneBookParam &param) = 0;
    virtual int SetPhoneBook(const BluetoothRawAddress &device, const std::u16string &name, int32_t flag) = 0;
    virtual int PullvCardListing(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardListingParam &param) = 0;
    virtual int PullvCardEntry(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardEntryParam &param) = 0;
    virtual bool IsDownloading(const BluetoothRawAddress &device) = 0;
    virtual int AbortDownloading(const BluetoothRawAddress &device) = 0;
    virtual int SetDevicePassword(const BluetoothRawAddress &device, const std::string &password, const std::string &userId) = 0;
    virtual int Disconnect(const BluetoothRawAddress &device) = 0;
    virtual int SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy) = 0;
    virtual int GetConnectionStrategy(const BluetoothRawAddress &device) = 0;
    virtual void GetDevicesByStates(const std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices) = 0;
    virtual void RegisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) = 0;
    virtual ::std::vector<BluetoothRawAddress> GetConnectedDevices() = 0;

};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PCE_INTERFACE_H