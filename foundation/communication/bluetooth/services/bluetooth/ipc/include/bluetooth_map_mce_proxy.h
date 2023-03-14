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

#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_map_mce.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMceProxy : public IRemoteProxy<IBluetoothMapMce> {
public:
    explicit BluetoothMapMceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothMapMce>(impl)
    {}
    ~BluetoothMapMceProxy()
    {}

    void RegisterObserver(const sptr<IBluetoothMapMceObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothMapMceObserver> &observer) override;
    int Connect(const BluetoothRawAddress &device) override;
    int Disconnect(const BluetoothRawAddress &device) override;
    int IsConnected(const BluetoothRawAddress &device) override;
    void GetConnectDevices(std::vector<BluetoothRawAddress> &devices) override;
    void GetDevicesByStates(const std::vector<int32_t> &statusList, std::vector<BluetoothRawAddress> &devices) override;
    int GetConnectionState(const BluetoothRawAddress &device) override;
    int SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy) override;
    int GetConnectionStrategy(const BluetoothRawAddress &device) override;
    int GetUnreadMessages(const BluetoothRawAddress &device, const int32_t msgType, const int32_t max) override;
    int GetSupportedFeatures(const BluetoothRawAddress &device) override;
    int SendMessage(const BluetoothRawAddress &device, const BluetoothIProfileSendMessageParameters &msg) override;
    int SetNotificationFilter(const BluetoothRawAddress &device, int32_t mask) override;
    int GetMessagesListing(const BluetoothRawAddress &device,
        const BluetoothIProfileGetMessagesListingParameters &para) override;
    int GetMessage(const BluetoothRawAddress &device, int32_t msgType,
        const std::u16string &msgHandle, const BluetoothIProfileGetMessageParameters &para) override;
    int UpdateInbox(const BluetoothRawAddress &device, int32_t msgType) override;
    int GetConversationListing(const BluetoothRawAddress &device,
        const BluetoothIProfileGetConversationListingParameters &para) override;
    int SetMessageStatus(const BluetoothRawAddress &device, int32_t msgType, const std::u16string &msgHandle,
        int32_t statusIndicator, int32_t statusValue, const std::string &extendedData) override;
    int SetOwnerStatus(const BluetoothRawAddress &device,
        const BluetoothIProfileSetOwnerStatusParameters &para) override;
    int GetOwnerStatus(const BluetoothRawAddress &device, const std::string &conversationId) override;
    BluetoothIProfileMasInstanceInfoList GetMasInstanceInfo(const BluetoothRawAddress &device) override;

private:
    static inline BrokerDelegator<BluetoothMapMceProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_PROXY_H
