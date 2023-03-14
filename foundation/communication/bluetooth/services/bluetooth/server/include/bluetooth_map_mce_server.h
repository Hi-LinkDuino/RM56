/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_SERVER_H

#include <string>
#include <vector>

#include "i_bluetooth_map_mce.h"
#include "bluetooth_map_mce_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMceServer : public BluetoothMapMceStub {
public:
    explicit BluetoothMapMceServer();
    ~BluetoothMapMceServer() override;

    void RegisterObserver(
        const sptr<IBluetoothMapMceObserver> &observer) override;
    void DeregisterObserver(
        const sptr<IBluetoothMapMceObserver> &observer) override;
    int Connect(
        const BluetoothRawAddress &device) override;
    int Disconnect(
        const BluetoothRawAddress &device) override;
    int IsConnected(
        const BluetoothRawAddress &device) override;
    void GetConnectDevices(
        std::vector<BluetoothRawAddress> &devices) override;
    void GetDevicesByStates(const std::vector<int32_t> &statusList,
        std::vector<BluetoothRawAddress> &devices) override;
    int GetConnectionState(
        const BluetoothRawAddress &device) override;
    int SetConnectionStrategy(
        const BluetoothRawAddress &device, int32_t strategy) override;
    int GetConnectionStrategy(
        const BluetoothRawAddress &device) override;
    int GetSupportedFeatures(
        const BluetoothRawAddress &device) override;
    int SetNotificationFilter(
        const BluetoothRawAddress &device, int32_t mask) override;
    int UpdateInbox(
        const BluetoothRawAddress &device, int32_t msgType) override;
    int SetMessageStatus(const BluetoothRawAddress &device, int32_t msgType,
        const std::u16string &msgHandle, int32_t statusIndicator, int32_t statusValue,
        const std::string &extendedData) override;
    int GetOwnerStatus(const BluetoothRawAddress &device,
        const std::string &conversationId) override;
    int GetUnreadMessages(const BluetoothRawAddress &device,
        int32_t msgType, int32_t max) override;
    int GetMessage(const BluetoothRawAddress &device, int32_t msgType,
        const std::u16string &msgHandle, const BluetoothIProfileGetMessageParameters &para) override;
    int SendMessage(const BluetoothRawAddress &device,
        const BluetoothIProfileSendMessageParameters &msg) override;
    int GetMessagesListing(const BluetoothRawAddress &device,
        const BluetoothIProfileGetMessagesListingParameters &para) override;
    int GetConversationListing(const BluetoothRawAddress &device,
        const BluetoothIProfileGetConversationListingParameters &para) override;
    int SetOwnerStatus(const BluetoothRawAddress &device,
        const BluetoothIProfileSetOwnerStatusParameters &para) override;
    BluetoothIProfileMasInstanceInfoList GetMasInstanceInfo(const BluetoothRawAddress &device) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothMapMceServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_SERVER_H
