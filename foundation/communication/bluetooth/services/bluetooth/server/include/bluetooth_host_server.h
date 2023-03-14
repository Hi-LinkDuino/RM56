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

#ifndef OHOS_BLUETOOTH_STANDARD_HOST_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_HOST_SERVER_H

#include <map>

#include "bt_def.h"
#include "bluetooth_host_stub.h"
#include "bluetooth_types.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
enum class ServiceRunningState { STATE_IDLE, STATE_RUNNING };

class BluetoothHostServer : public SystemAbility, public BluetoothHostStub {
    DECLARE_SYSTEM_ABILITY(BluetoothHostServer);

public:
    explicit BluetoothHostServer();
    ~BluetoothHostServer() override;

    static sptr<BluetoothHostServer> GetInstance();

    void OnStart() override;
    void OnStop() override;

    void RegisterObserver(const sptr<IBluetoothHostObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothHostObserver> &observer) override;
    bool EnableBt() override;
    bool DisableBt() override;
    int32_t GetBtState() override;
    bool BluetoothFactoryReset() override;
    int32_t GetDeviceType(int32_t transport, const std::string &address) override;

    std::string GetLocalAddress() override;
    sptr<IRemoteObject> GetProfile(const std::string &name) override;
    sptr<IRemoteObject> GetBleRemote(const std::string &name) override;

    ServiceRunningState QueryServiceState() const
    {
        return state_;
    }

    bool Start();
    void Stop();
    bool DisableBle() override;
    bool EnableBle() override;
    bool IsBleEnabled() override;
    std::vector<uint32_t> GetProfileList() override;
    int32_t GetMaxNumConnectedAudioDevices() override;
    int32_t GetBtConnectionState() override;
    int32_t GetBtProfileConnState(uint32_t profileId) override;
    int32_t GetLocalDeviceClass() override;
    bool SetLocalDeviceClass(const int32_t &deviceClass) override;
    std::string GetLocalName() override;
    bool SetLocalName(const std::string &name) override;
    int32_t GetBtScanMode() override;
    bool SetBtScanMode(int32_t mode, int32_t duration) override;
    int32_t GetBondableMode(const int32_t transport) override;
    bool SetBondableMode(int32_t transport, int32_t mode) override;
    bool StartBtDiscovery() override;
    bool CancelBtDiscovery() override;
    bool IsBtDiscovering(const int32_t transport) override;
    long GetBtDiscoveryEndMillis() override;
    std::vector<BluetoothRawAddress> GetPairedDevices(const int32_t transport) override;
    bool RemovePair(const int32_t transport, const sptr<BluetoothRawAddress> &device) override;
    bool RemoveAllPairs() override;
    void RegisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer) override;
    void DeregisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer) override;
    int32_t GetBleMaxAdvertisingDataLength() override;
    int32_t GetPhonebookPermission(const std::string &address) override;
    bool SetPhonebookPermission(const std::string &address, int32_t permission) override;
    int32_t GetMessagePermission(const std::string &address) override;
    bool SetMessagePermission(const std::string &address, int32_t permission) override;
    int32_t GetPowerMode(const std::string &address) override;
    std::string GetDeviceName(int32_t transport, const std::string &address) override;
    std::string GetDeviceAlias(const std::string &address) override;
    bool SetDeviceAlias(const std::string &address, const std::string &aliasName) override;
    int32_t GetDeviceBatteryLevel(const std::string &address) override;
    int32_t GetPairState(int32_t transport, const std::string &address) override;
    bool StartPair(int32_t transport, const std::string &address) override;
    bool CancelPairing(int32_t transport, const std::string &address) override;
    bool IsBondedFromLocal(int32_t transport, const std::string &address) override;
    bool IsAclConnected(int32_t transport, const std::string &address) override;
    bool IsAclEncrypted(int32_t transport, const std::string &address) override;
    int32_t GetDeviceClass(const std::string &address) override;
    bool SetDevicePin(const std::string &address, const std::string &pin) override;
    bool SetDevicePairingConfirmation(int32_t transport, const std::string &address, bool accept) override;
    bool SetDevicePasskey(int32_t transport, const std::string &address, int32_t passkey, bool accept) override;
    bool PairRequestReply(int32_t transport, const std::string &address, bool accept) override;
    bool ReadRemoteRssiValue(const std::string &address) override;
    void RegisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer) override;
    void DeregisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer) override;
    void RegisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer) override;
    void DeregisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer) override;
    void GetLocalSupportedUuids(std::vector<std::string> &uuids) override;
    std::vector<bluetooth::Uuid> GetDeviceUuids(int32_t transport, const std::string &address) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

private:
    static sptr<BluetoothHostServer> instance;
    static std::mutex instanceLock;
    int32_t transport_ = bluetooth::BT_TRANSPORT_BREDR;
    bool Init();

    bool registeredToService_ = false;
    ServiceRunningState state_ = ServiceRunningState::STATE_IDLE;

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothHostServer);
    BLUETOOTH_DECLARE_IMPL();
    bool IsBtEnabled();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_HOST_SERVER_H
