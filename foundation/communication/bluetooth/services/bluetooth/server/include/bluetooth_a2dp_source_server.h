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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SOURCE_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SOURCE_SERVER_H

#include "bluetooth_a2dp_src_stub.h"
#include "raw_address.h"
#include "bluetooth_types.h"
#include "i_bluetooth_a2dp_src.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpSourceServer : public BluetoothA2dpSrcStub {
public:
    BluetoothA2dpSourceServer();
    ~BluetoothA2dpSourceServer() override;

    // IBluetoothHost overrides:
    void RegisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer) override;
    int Connect(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;
    int GetDeviceState(const RawAddress &device) override;
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int32_t> &states) override;
    int GetPlayingState(const RawAddress &device) override;
    int SetConnectStrategy(const RawAddress &device, int strategy) override;
    int GetConnectStrategy(const RawAddress &device) override;
    int SetActiveSinkDevice(const RawAddress &device) override;
    RawAddress GetActiveSinkDevice() override;
    BluetoothA2dpCodecStatus GetCodecStatus(const RawAddress &device) override;
    int SetCodecPreference(const RawAddress &device, const BluetoothA2dpCodecInfo &info) override;
    void SwitchOptionalCodecs(const RawAddress &device, bool isEnable) override;
    int GetOptionalCodecsSupportState(const RawAddress &device ) override;
    int StartPlaying(const RawAddress &device) override;
    int SuspendPlaying(const RawAddress &device) override;
    int StopPlaying(const RawAddress &device) override;
    int WriteFrame(const uint8_t *data, uint32_t size) override;
    void GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothA2dpSourceServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SOURCE_SERVER_H