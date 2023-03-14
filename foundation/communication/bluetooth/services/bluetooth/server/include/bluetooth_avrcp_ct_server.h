/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_CT_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_CT_SERVER_H

#include "bluetooth_avrcp_ct_stub.h"
#include "bluetooth_types.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpCtServer : public BluetoothAvrcpCtStub {
public:
    BluetoothAvrcpCtServer();
    ~BluetoothAvrcpCtServer();

    virtual void RegisterObserver(const sptr<IBluetoothAvrcpCtObserver> &observer) override;
    virtual void UnregisterObserver(const sptr<IBluetoothAvrcpCtObserver> &observer) override;

    virtual std::vector<RawAddress> GetConnectedDevices() override;
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int32_t> &states) override;
    virtual int32_t GetDeviceState(const RawAddress &device) override;
    virtual int32_t Connect(const RawAddress &device) override;
    virtual int32_t Disconnect(const RawAddress &device) override;
    virtual int32_t PressButton(const RawAddress &device, int32_t button) override;
    virtual int32_t ReleaseButton(const RawAddress &device, int32_t button) override;
    virtual int32_t GetUnitInfo(const RawAddress &device) override;
    virtual int32_t GetSubUnitInfo(const RawAddress &device) override;
    virtual int32_t GetSupportedCompanies(const RawAddress &device) override;
    virtual int32_t GetSupportedEvents(const RawAddress &device) override;
    virtual int32_t GetPlayerAppSettingAttributes(const RawAddress &device) override;
    virtual int32_t GetPlayerAppSettingValues(const RawAddress &device, int32_t attribute) override;
    virtual int32_t GetPlayerAppSettingCurrentValue(const RawAddress &device,
        const std::vector<int32_t> &attributes) override;
    virtual int32_t SetPlayerAppSettingCurrentValue(const RawAddress &device,
        const std::vector<int32_t> &attributes, const std::vector<int32_t> &values) override;
    virtual int32_t GetPlayerAppSettingAttributeText(const RawAddress &device,
        const std::vector<int32_t> &attributes) override;
    virtual int32_t GetPlayerAppSettingValueText(const RawAddress &device, int32_t attributes,
        const std::vector<int32_t> &values) override;
    virtual int32_t GetElementAttributes(const RawAddress &device,
        const std::vector<int32_t> &attributes) override;
    virtual int32_t GetPlayStatus(const RawAddress &device) override;
    virtual int32_t PlayItem(const RawAddress &device, int32_t scope, int64_t uid, int32_t uidCounter) override;
    virtual int32_t GetFolderItems(const RawAddress &device, int32_t startItem, int32_t endItem,
        const std::vector<int32_t> &attributes) override;
    virtual int32_t GetTotalNumberOfItems(const RawAddress &device, int32_t scope) override;
    virtual int32_t SetAbsoluteVolume(const RawAddress &device, int32_t volume) override;
    virtual int32_t EnableNotification(const RawAddress &device,
        const std::vector<int32_t> &events, int32_t interval) override;
    virtual int32_t DisableNotification(const RawAddress &device, const std::vector<int32_t> &events) override;
    virtual int32_t GetItemAttributes(const RawAddress &device, int64_t uid, int32_t uidCounter,
        const std::vector<int32_t> &attributes) override;
    virtual int32_t SetBrowsedPlayer(const RawAddress &device, int32_t playerId) override;
    virtual int32_t GetMeidaPlayerList(const RawAddress &device, int32_t startItem, int32_t endItem) override;
private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothAvrcpCtServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_CT_SERVER_H