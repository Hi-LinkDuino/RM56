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

#ifndef OHOS_BLUETOOTH_STANDARD_AVRCP_TG_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_AVRCP_TG_SERVER_H

#include <map>

#include "bluetooth_avrcp_tg_stub.h"
#include "bluetooth_types.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothAvrcpTgServer : public BluetoothAvrcpTgStub {
public:
    explicit BluetoothAvrcpTgServer();
    ~BluetoothAvrcpTgServer() override;

    void RegisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer) override;
    void UnregisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer) override;
    void SetActiveDevice(const BluetoothRawAddress &addr) override;
    int32_t Connect(const BluetoothRawAddress &addr) override;
    int32_t Disconnect(const BluetoothRawAddress &addr) override;
    std::vector<BluetoothRawAddress> GetConnectedDevices() override;
    std::vector<BluetoothRawAddress> GetDevicesByStates(const std::vector<int32_t> &states) override;
    int32_t GetDeviceState(const BluetoothRawAddress &addr) override;
    void NotifyPlaybackStatusChanged(int32_t playStatus, int32_t playbackPos) override;
    void NotifyTrackChanged(int64_t uid, int32_t playbackPos) override;
    void NotifyTrackReachedEnd(int32_t playbackPos) override;
    void NotifyTrackReachedStart(int32_t playbackPos) override;
    void NotifyPlaybackPosChanged(int32_t playbackPos) override;
    void NotifyPlayerAppSettingChanged(const std::vector<int32_t> &attributes,
                                       const std::vector<int32_t> &values) override;
    void NotifyNowPlayingContentChanged() override;
    void NotifyAvailablePlayersChanged() override;
    void NotifyAddressedPlayerChanged(int32_t playerId, int32_t uidCounter) override;
    void NotifyUidChanged(int32_t uidCounter) override;
    void NotifyVolumeChanged(int32_t volume) override;

private:
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothAvrcpTgServer);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_AVRCP_TG_SERVER_H