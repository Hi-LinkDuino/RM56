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

#include <thread>

#include "bluetooth_a2dp_sink_server.h"
#include "bluetooth_a2dp_source_server.h"
#include "bluetooth_avrcp_ct_server.h"
#include "bluetooth_avrcp_tg_server.h"
#include "bluetooth_ble_advertiser_server.h"
#include "bluetooth_ble_central_manager_server.h"
#include "bluetooth_gatt_client_server.h"
#include "bluetooth_gatt_server_server.h"
#include "bluetooth_hfp_ag_server.h"
#include "bluetooth_hfp_hf_server.h"
#include "bluetooth_hid_host_server.h"
#include "bluetooth_host_dumper.h"
#include "bluetooth_host_server.h"
#include "bluetooth_log.h"
#include "bluetooth_map_mce_server.h"
#include "bluetooth_map_mse_server.h"
#include "bluetooth_pan_server.h"
#include "bluetooth_opp_server.h"
#include "bluetooth_pbap_pce_server.h"
#include "bluetooth_pbap_pse_server.h"
#include "bluetooth_socket_server.h"
#include "bluetooth_utils_server.h"
#include "file_ex.h"
#include "hisysevent.h"
#include "interface_adapter_manager.h"
#include "permission_utils.h"


#include "interface_adapter_ble.h"
#include "interface_adapter_classic.h"
#include "interface_profile_manager.h"
#include "ipc_skeleton.h"
#include "permission_utils.h"
#include "raw_address.h"
#include "remote_observer_list.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
struct BluetoothHostServer::impl {
    impl();
    ~impl();
    void Init();
    void Clear();
    /// service ptr
    IAdapterClassic *classicService_ = nullptr;
    IAdapterBle *bleService_ = nullptr;

    /// sys state observer
    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    /// adapter state observer
    class AdapterStateObserver;
    std::unique_ptr<AdapterStateObserver> observerImp_ = nullptr;

    /// classic observer
    class AdapterClassicObserver;
    std::unique_ptr<AdapterClassicObserver> classicObserverImp_ = nullptr;

    /// classic remote device observer
    class ClassicRemoteDeviceObserver;
    std::unique_ptr<ClassicRemoteDeviceObserver> remoteObserverImp_ = nullptr;

    /// ble observer
    class AdapterBleObserver;
    std::unique_ptr<AdapterBleObserver> bleObserverImp_ = nullptr;

    /// ble remote device observer
    class BlePeripheralCallback;
    std::unique_ptr<BlePeripheralCallback> bleRemoteObserverImp_ = nullptr;

    /// user regist observers
    RemoteObserverList<IBluetoothHostObserver> observers_;
    RemoteObserverList<IBluetoothHostObserver> bleObservers_;
    std::map<sptr<IRemoteObject>, uint32_t> observersToken_;
    std::map<sptr<IRemoteObject>, uint32_t> bleObserversToken_;
    std::map<sptr<IRemoteObject>, int32_t> observersUid_;
    std::map<sptr<IRemoteObject>, int32_t> bleObserversUid_;

    /// user regist remote observers
    RemoteObserverList<IBluetoothRemoteDeviceObserver> remoteObservers_;
    std::map<sptr<IRemoteObject>, uint32_t> remoteObserversToken_;
    std::map<sptr<IRemoteObject>, int32_t> remoteObserversUid_;

    /// user regist remote observers
    RemoteObserverList<IBluetoothBlePeripheralObserver> bleRemoteObservers_;
    std::map<sptr<IRemoteObject>, uint32_t> bleRemoteObserversToken_;

    std::map<std::string, sptr<IRemoteObject>> servers_;
    std::map<std::string, sptr<IRemoteObject>> bleServers_;

    std::vector<sptr<IBluetoothHostObserver>> hostObservers_;
    std::vector<sptr<IBluetoothRemoteDeviceObserver>> remoteDeviceObservers_;
    std::vector<sptr<IBluetoothHostObserver>> bleAdapterObservers_;
    std::vector<sptr<IBluetoothBlePeripheralObserver>> blePeripheralObservers_;

private:
    void createServers();
};

class BluetoothHostServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    SystemStateObserver(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~SystemStateObserver() override = default;

    void OnSystemStateChange(const BTSystemState state) override
    {
        if (!impl_) {
            HILOGI("failed: impl_ is null");
            return;
        }
        switch (state) {
            case BTSystemState::ON:
                /// update service ptr
                impl_->classicService_ = static_cast<IAdapterClassic *>(
                    IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BREDR));
                impl_->bleService_ =
                    static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));
                if (impl_->classicService_) {
                    impl_->classicService_->RegisterClassicAdapterObserver(
                        *(IAdapterClassicObserver *)impl_->classicObserverImp_.get());
                    impl_->classicService_->RegisterRemoteDeviceObserver(
                        *(IClassicRemoteDeviceObserver *)impl_->remoteObserverImp_.get());
                }
                if (impl_->bleService_) {
                    impl_->bleService_->RegisterBleAdapterObserver(
                        *(IAdapterBleObserver *)impl_->bleObserverImp_.get());
                    impl_->bleService_->RegisterBlePeripheralCallback(
                        *(IBlePeripheralCallback *)impl_->bleRemoteObserverImp_.get());
                }
                break;

            case BTSystemState::OFF:
                if (impl_->classicService_) {
                    impl_->classicService_->DeregisterClassicAdapterObserver(
                        *(IAdapterClassicObserver *)impl_->classicObserverImp_.get());
                    impl_->classicService_->DeregisterRemoteDeviceObserver(
                        *(IClassicRemoteDeviceObserver *)impl_->remoteObserverImp_.get());
                    impl_->classicService_ = nullptr;
                }
                if (impl_->bleService_) {
                    impl_->bleService_->DeregisterBleAdapterObserver(
                        *(IAdapterBleObserver *)impl_->bleObserverImp_.get());
                    impl_->bleService_->DeregisterBlePeripheralCallback(
                        *(IBlePeripheralCallback *)impl_->bleRemoteObserverImp_.get());
                    impl_->bleService_ = nullptr;
                }
                break;
            default:
                break;
        }
    }

private:
    BluetoothHostServer::impl *impl_ = nullptr;
};

class BluetoothHostServer::impl::AdapterStateObserver : public IAdapterStateObserver {
public:
    AdapterStateObserver(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~AdapterStateObserver() override = default;

    void OnStateChange(const BTTransport transport, const BTStateID state) override
    {
        if (!impl_) {
            return;
        }
        if (transport == BTTransport::ADAPTER_BREDR) {
            impl_->observers_.ForEach([this, transport, state](sptr<IBluetoothHostObserver> observer) {
                int32_t uid = this->impl_->observersUid_[observer->AsObject()];
                if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                    HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                    return;
                }
                uint32_t tokenId = this->impl_->observersToken_[observer->AsObject()];
                if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                    HILOGE("false, check permission failed");
                } else {
                    observer->OnStateChanged(transport, state);
                }
            });
            if (state == BTStateID::STATE_TURN_ON || state == BTStateID::STATE_TURN_OFF) {
                int32_t pid = IPCSkeleton::GetCallingPid();
                int32_t uid = IPCSkeleton::GetCallingUid();
                HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "BR_SWITCH_STATE",
                    HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", state);
            }
        } else if (transport == BTTransport::ADAPTER_BLE) {
            impl_->bleObservers_.ForEach([this, transport, state](sptr<IBluetoothHostObserver> observer) {
                int32_t uid = this->impl_->bleObserversUid_[observer->AsObject()];
                if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                    HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                    return;
                }
                uint32_t  tokenId = this->impl_->bleObserversToken_[observer->AsObject()];
                if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                    HILOGE("false, check permission failed");
                } else {
                    observer->OnStateChanged(transport, state);
                }
            });
            if (state == BTStateID::STATE_TURN_ON || state == BTStateID::STATE_TURN_OFF) {
                int32_t pid = IPCSkeleton::GetCallingPid();
                int32_t uid = IPCSkeleton::GetCallingUid();
                HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "BLE_SWITCH_STATE",
                    HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", state);
            }
        }
    };

private:
    BluetoothHostServer::impl *impl_ = nullptr;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AdapterStateObserver);
};
class BluetoothHostServer::impl::AdapterClassicObserver : public IAdapterClassicObserver {
public:
    AdapterClassicObserver(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~AdapterClassicObserver() override = default;

    void OnDiscoveryStateChanged(const int32_t status) override
    {
        HILOGI("status: %{public}d", status);
        impl_->observers_.ForEach([this, status](sptr<IBluetoothHostObserver> observer) {
            int32_t uid = this->impl_->observersUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnDiscoveryStateChanged(static_cast<int32_t>(status));
        });
        if (status == DISCOVERY_STARTED || status == DISCOVERY_STOPED) {
            int32_t pid = IPCSkeleton::GetCallingPid();
            int32_t uid = IPCSkeleton::GetCallingUid();
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "DISCOVERY_STATE",
                HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", status);
        }
    }

    void OnDiscoveryResult(const RawAddress &device) override
    {
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        impl_->observers_.ForEach([this, device](IBluetoothHostObserver *observer) {
            int32_t uid = this->impl_->observersUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            uint32_t tokenId = this->impl_->observersToken_[observer->AsObject()];
            if (PermissionUtils::VerifyDiscoverBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("OnDiscoveryResult() false, check permission failed");
            } else {
                observer->OnDiscoveryResult(device);
            }
        });
    }

    void OnPairRequested(const BTTransport transport, const RawAddress &device) override
    {
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        impl_->observers_.ForEach(
            [transport, device](IBluetoothHostObserver *observer) { observer->OnPairRequested(transport, device); });
    }

    void OnPairConfirmed(
        const BTTransport transport, const RawAddress &device, int32_t reqType, int32_t number) override
    {
        HILOGI("device: %{public}s, reqType: %{public}d, number: %{public}d",
            GET_ENCRYPT_ADDR(device), reqType, number);
        impl_->observers_.ForEach([this, transport, device, reqType, number](IBluetoothHostObserver *observer) {
            uint32_t tokenId = this->impl_->observersToken_[observer->AsObject()];
            if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("false, check permission failed");
            } else {
                observer->OnPairConfirmed(transport, device, reqType, number);
            }
        });
    }

    void OnScanModeChanged(int32_t mode) override
    {
        HILOGI("mode: %{public}d", mode);
        impl_->observers_.ForEach([mode](IBluetoothHostObserver *observer) { observer->OnScanModeChanged(mode); });
    }

    void OnDeviceNameChanged(const std::string &deviceName) override
    {
        HILOGI("deviceName: %{public}s", deviceName.c_str());
        impl_->observers_.ForEach(
            [deviceName](IBluetoothHostObserver *observer) { observer->OnDeviceNameChanged(deviceName); });
    }

    void OnDeviceAddrChanged(const std::string &address) override
    {
        HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
        impl_->observers_.ForEach(
            [address](IBluetoothHostObserver *observer) { observer->OnDeviceAddrChanged(address); });
    }

private:
    BluetoothHostServer::impl *impl_ = nullptr;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AdapterClassicObserver);
};
class BluetoothHostServer::impl::ClassicRemoteDeviceObserver : public IClassicRemoteDeviceObserver {
public:
    ClassicRemoteDeviceObserver(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~ClassicRemoteDeviceObserver() override = default;

    void OnPairStatusChanged(const BTTransport transport, const RawAddress &device, const int32_t status) override
    {
        HILOGI("device: %{public}s, status: %{public}d", GET_ENCRYPT_ADDR(device), status);
        impl_->remoteObservers_.ForEach([this, transport, device, status](IBluetoothRemoteDeviceObserver *observer) {
            int32_t uid = this->impl_->remoteObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            uint32_t tokenId = this->impl_->remoteObserversToken_[observer->AsObject()];
            if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("false, check permission failed");
            } else {
                observer->OnPairStatusChanged(transport, device, status);
            }
        });
    }

    void OnRemoteUuidChanged(const RawAddress &device, const std::vector<Uuid> &uuids) override
    {
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        std::vector<bluetooth::Uuid> btUuids;
        for (const auto &val : uuids) {
            btUuids.push_back(val);
        }
        impl_->remoteObservers_.ForEach([this, device, btUuids](IBluetoothRemoteDeviceObserver *observer) {
            int32_t uid = this->impl_->remoteObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnRemoteUuidChanged(device, btUuids);
        });
    }

    void OnRemoteNameChanged(const RawAddress &device, const std::string &deviceName) override
    {
        HILOGI("device: %{public}s, deviceName: %{public}s", GET_ENCRYPT_ADDR(device), deviceName.c_str());
        impl_->remoteObservers_.ForEach([this, device, deviceName](IBluetoothRemoteDeviceObserver *observer) {
            int32_t uid = this->impl_->remoteObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnRemoteNameChanged(device, deviceName);
        });
    }

    void OnRemoteAliasChanged(const RawAddress &device, const std::string &alias) override
    {
        HILOGI("device: %{public}s, alias: %{public}s", GET_ENCRYPT_ADDR(device), alias.c_str());
        impl_->remoteObservers_.ForEach([device, alias](IBluetoothRemoteDeviceObserver *observer) {
            observer->OnRemoteAliasChanged(device, alias);
        });
    }

    void OnRemoteCodChanged(const RawAddress &device, int32_t cod) override
    {
        HILOGI("device: %{public}s, cod: %{public}d", GET_ENCRYPT_ADDR(device), cod);
        impl_->remoteObservers_.ForEach([this, device, cod](IBluetoothRemoteDeviceObserver *observer) {
            int32_t uid = this->impl_->remoteObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnRemoteCodChanged(device, cod);
        });
    }

    void OnRemoteBatteryLevelChanged(const RawAddress &device, const int32_t batteryLevel) override
    {
        HILOGI("device: %{public}s, batteryLevel: %{public}d", GET_ENCRYPT_ADDR(device), batteryLevel);
        impl_->remoteObservers_.ForEach([this, device, batteryLevel](IBluetoothRemoteDeviceObserver *observer) {
            int32_t uid = this->impl_->remoteObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnRemoteBatteryLevelChanged(device, batteryLevel);
        });
    }

private:
    BluetoothHostServer::impl *impl_ = nullptr;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(ClassicRemoteDeviceObserver);
};
class BluetoothHostServer::impl::AdapterBleObserver : public IAdapterBleObserver {
public:
    AdapterBleObserver(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~AdapterBleObserver() override = default;

    void OnDiscoveryStateChanged(const int32_t status) override
    {
        HILOGI("status: %{public}d", status);
        impl_->bleObservers_.ForEach([this, status](sptr<IBluetoothHostObserver> observer) {
            int32_t uid = this->impl_->bleObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnDiscoveryStateChanged(static_cast<int32_t>(status));
        });
    }

    void OnDiscoveryResult(const RawAddress &device) override
    {
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        impl_->bleObservers_.ForEach([this, device](IBluetoothHostObserver *observer) {
            int32_t uid = this->impl_->bleObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            uint32_t tokenId = this->impl_->bleObserversToken_[observer->AsObject()];
            if (PermissionUtils::VerifyDiscoverBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("false, check permission failed");
            } else {
                observer->OnDiscoveryResult(device);
            }
        });
    }

    void OnPairRequested(const BTTransport transport, const RawAddress &device) override
    {
        HILOGI("device: %{public}s", GET_ENCRYPT_ADDR(device));
        impl_->bleObservers_.ForEach(
            [transport, device](IBluetoothHostObserver *observer) { observer->OnPairRequested(transport, device); });
    }

    void OnPairConfirmed(
        const BTTransport transport, const RawAddress &device, const int32_t reqType, const int32_t number) override
    {
        HILOGI("device: %{public}s, reqType: %{public}d, number: %{public}d",
            GET_ENCRYPT_ADDR(device), reqType, number);
        impl_->bleObservers_.ForEach([this, transport, device, reqType, number](IBluetoothHostObserver *observer) {
            int32_t uid = this->impl_->bleObserversUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGI("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            uint32_t tokenId = this->impl_->bleObserversToken_[observer->AsObject()];
            if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("OnPairConfirmed() false, check permission failed");
            } else {
                observer->OnPairConfirmed(transport, device, reqType, number);
            }
        });
    }

    void OnScanModeChanged(const int32_t mode) override
    {
        HILOGI("mode: %{public}d", mode);
        impl_->bleObservers_.ForEach([mode](IBluetoothHostObserver *observer) { observer->OnScanModeChanged(mode); });
    }

    void OnDeviceNameChanged(const std::string deviceName) override
    {
        HILOGI("deviceName: %{public}s", deviceName.c_str());
        impl_->bleObservers_.ForEach(
            [deviceName](IBluetoothHostObserver *observer) { observer->OnDeviceNameChanged(deviceName); });
    }

    void OnDeviceAddrChanged(const std::string address) override
    {
        HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
        impl_->bleObservers_.ForEach(
            [address](IBluetoothHostObserver *observer) { observer->OnDeviceAddrChanged(address); });
    }

    void OnAdvertisingStateChanged(const int32_t state) override
    {}

private:
    BluetoothHostServer::impl *impl_ = nullptr;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AdapterBleObserver);
};
class BluetoothHostServer::impl::BlePeripheralCallback : public IBlePeripheralCallback {
public:
    BlePeripheralCallback(BluetoothHostServer::impl *impl) : impl_(impl){};
    ~BlePeripheralCallback() override = default;

    void OnReadRemoteRssiEvent(const RawAddress &device, int32_t rssi, int32_t status) override
    {
        HILOGI("device: %{public}s, rssi: %{public}d, status: %{public}d",
            GET_ENCRYPT_ADDR(device), rssi, status);
        impl_->bleRemoteObservers_.ForEach([device, rssi, status](IBluetoothBlePeripheralObserver *observer) {
            observer->OnReadRemoteRssiEvent(device, rssi, status);
        });
    }

    void OnPairStatusChanged(const BTTransport transport, const RawAddress &device, int32_t status) override
    {
        HILOGI("device: %{public}s, status: %{public}d", GET_ENCRYPT_ADDR(device), status);
        impl_->bleRemoteObservers_.ForEach([this, transport, device, status](
            IBluetoothBlePeripheralObserver *observer) {
            uint32_t tokenId = this->impl_->bleRemoteObserversToken_[observer->AsObject()];
            if (PermissionUtils::VerifyUseBluetoothPermission(tokenId) == PERMISSION_DENIED) {
                HILOGE("false, check permission failed");
            } else {
                observer->OnPairStatusChanged(transport, device, status);
            }
        });
    }

private:
    BluetoothHostServer::impl *impl_ = nullptr;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BlePeripheralCallback);
};

std::mutex BluetoothHostServer::instanceLock;
sptr<BluetoothHostServer> BluetoothHostServer::instance;

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(BluetoothHostServer::GetInstance().GetRefPtr());

BluetoothHostServer::impl::impl()
{
    HILOGI("starts");
    systemStateObserver_ = std::make_unique<SystemStateObserver>(this);
    observerImp_ = std::make_unique<AdapterStateObserver>(this);
    classicObserverImp_ = std::make_unique<AdapterClassicObserver>(this);
    remoteObserverImp_ = std::make_unique<ClassicRemoteDeviceObserver>(this);
    bleObserverImp_ = std::make_unique<AdapterBleObserver>(this);
    bleRemoteObserverImp_ = std::make_unique<BlePeripheralCallback>(this);
}

BluetoothHostServer::impl::~impl()
{
    HILOGI("starts");
}

void BluetoothHostServer::impl::Init()
{
    HILOGI("starts");

    classicService_ =
        static_cast<IAdapterClassic *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BREDR));
    bleService_ = static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));

    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*systemStateObserver_);

    IAdapterManager::GetInstance()->Start();
    IAdapterManager::GetInstance()->RegisterStateObserver(*observerImp_);
    if (classicService_) {
        classicService_->RegisterClassicAdapterObserver(*classicObserverImp_.get());
        classicService_->RegisterRemoteDeviceObserver(*remoteObserverImp_.get());
    }

    if (bleService_) {
        bleService_->RegisterBleAdapterObserver(*bleObserverImp_.get());
        bleService_->RegisterBlePeripheralCallback(*bleRemoteObserverImp_.get());
    }

    createServers();
}

void BluetoothHostServer::impl::Clear()
{
    /// systerm state observer
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*systemStateObserver_);

    /// adapter state observer
    IAdapterManager::GetInstance()->Stop();
    IAdapterManager::GetInstance()->DeregisterStateObserver(*observerImp_);

    if (classicService_) {
        /// classic observer
        classicService_->DeregisterClassicAdapterObserver(*classicObserverImp_.get());
        /// classic remote observer
        classicService_->DeregisterRemoteDeviceObserver(*remoteObserverImp_.get());
    }

    if (bleService_) {
        /// ble observer
        bleService_->DeregisterBleAdapterObserver(*bleObserverImp_.get());
        /// ble remote observer
        bleService_->DeregisterBlePeripheralCallback(*bleRemoteObserverImp_.get());
    }
}

void BluetoothHostServer::impl::createServers()
{
    sptr<BluetoothSocketServer> socket = new BluetoothSocketServer();
    servers_[PROFILE_SPP] = socket->AsObject();

    sptr<BluetoothGattServerServer> gattserver = new BluetoothGattServerServer();
    servers_[PROFILE_GATT_SERVER] = gattserver->AsObject();

    sptr<BluetoothGattClientServer> gattclient = new BluetoothGattClientServer();
    servers_[PROFILE_GATT_CLIENT] = gattclient->AsObject();

    sptr<BluetoothHfpAgServer> hfpAg = new BluetoothHfpAgServer();
    servers_[PROFILE_HFP_AG] = hfpAg->AsObject();

    sptr<BluetoothHfpHfServer> hfpHf = new BluetoothHfpHfServer();
    servers_[PROFILE_HFP_HF] = hfpHf->AsObject();

    sptr<BluetoothAvrcpCtServer> avrcpCtServer = new BluetoothAvrcpCtServer();
    servers_[PROFILE_AVRCP_CT] = avrcpCtServer->AsObject();

    sptr<BluetoothAvrcpTgServer> avrcpTgServer = new BluetoothAvrcpTgServer();
    servers_[PROFILE_AVRCP_TG] = avrcpTgServer->AsObject();

    sptr<BluetoothBleAdvertiserServer> bleAdvertiser = new BluetoothBleAdvertiserServer();
    bleServers_[BLE_ADVERTISER_SERVER] = bleAdvertiser->AsObject();

    sptr<BluetoothBleCentralManagerServer> bleCentralManger = new BluetoothBleCentralManagerServer();
    bleServers_[BLE_CENTRAL_MANAGER_SERVER] = bleCentralManger->AsObject();

    sptr<BluetoothPbapPceServer> pbapPce = new BluetoothPbapPceServer();
    servers_[PROFILE_PBAP_PCE] = pbapPce->AsObject();

    sptr<BluetoothPbapPseServer> pbapPse = new BluetoothPbapPseServer();
    servers_[PROFILE_PBAP_PSE] = pbapPse->AsObject();

    sptr<BluetoothMapMceServer> mapMce = new BluetoothMapMceServer();
    servers_[PROFILE_MAP_MCE] = mapMce->AsObject();

    sptr<BluetoothMapMseServer> mapMse = new BluetoothMapMseServer();
    servers_[PROFILE_MAP_MSE] = mapMse->AsObject();

    sptr<BluetoothA2dpSourceServer> a2dpSource = new BluetoothA2dpSourceServer();
    servers_[PROFILE_A2DP_SRC] = a2dpSource->AsObject();

    sptr<BluetoothA2dpSinkServer> a2dpSink = new BluetoothA2dpSinkServer();
    servers_[PROFILE_A2DP_SINK] = a2dpSink->AsObject();

    sptr<BluetoothHidHostServer> hidHostServer = new BluetoothHidHostServer();
    servers_[PROFILE_HID_HOST_SERVER] = hidHostServer->AsObject();

    sptr<BluetoothPanServer> panServer = new BluetoothPanServer();
    servers_[PROFILE_PAN_SERVER] = panServer->AsObject();

    sptr<BluetoothOppServer> oppServer = new BluetoothOppServer();
    servers_[PROFILE_OPP_SERVER] = oppServer->AsObject();

    HILOGI("servers_ constructed, size is %{public}zu", servers_.size());
}

BluetoothHostServer::BluetoothHostServer() : SystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID, true)
{
    pimpl = std::make_unique<impl>();
}

BluetoothHostServer::~BluetoothHostServer()
{
    HILOGI("~BluetoothHostServer called.");
}

sptr<BluetoothHostServer> BluetoothHostServer::GetInstance()
{
    std::lock_guard<std::mutex> autoLock(instanceLock);
    if (instance == nullptr) {
        sptr<BluetoothHostServer> temp = new BluetoothHostServer();
        instance = temp;
    }
    return instance;
}

void BluetoothHostServer::OnStart()
{
    HILOGI("starting service.");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        HILOGI("service is already started.");
        return;
    }

    if (!Init()) {
        HILOGE("initiation failed");
        OnStop();
        return;
    }

    state_ = ServiceRunningState::STATE_RUNNING;

    HILOGI("service has been started successfully");
    return;
}

bool BluetoothHostServer::Init()
{
    if (!registeredToService_) {
        bool ret = Publish(BluetoothHostServer::GetInstance());
        if (!ret) {
            HILOGE("init publish failed!");
            return false;
        }
        registeredToService_ = true;
    }
    HILOGI("init success");
    pimpl->Init();
    return true;
}

void BluetoothHostServer::OnStop()
{
    HILOGI("stopping service.");

    pimpl->Clear();
    state_ = ServiceRunningState::STATE_IDLE;
    registeredToService_ = false;
    return;
}

void BluetoothHostServer::RegisterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    if (observer == nullptr) {
        HILOGE("RegisterObserver observer is null");
        return;
    }

    pimpl->observersToken_[observer->AsObject()] = IPCSkeleton::GetCallingTokenID();
    pimpl->observersUid_[observer->AsObject()] = IPCSkeleton::GetCallingUid();
    pimpl->observers_.Register(observer);
    pimpl->hostObservers_.push_back(observer);
}

void BluetoothHostServer::DeregisterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    if (observer == nullptr || pimpl == nullptr) {
        HILOGE("DeregisterObserver observer is null");
        return;
    }
    for (auto iter = pimpl->hostObservers_.begin(); iter != pimpl->hostObservers_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            pimpl->observers_.Deregister(*iter);
            pimpl->hostObservers_.erase(iter);
            break;
        }
    }
    for (auto iter =  pimpl->observersToken_.begin(); iter !=  pimpl->observersToken_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->observersToken_.erase(iter);
            break;
        }
    }
    for (auto iter = pimpl->observersUid_.begin(); iter != pimpl->observersUid_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->observersUid_.erase(iter);
            break;
        }
    }
}

bool BluetoothHostServer::EnableBt()
{
    return IAdapterManager::GetInstance()->Enable(bluetooth::BTTransport::ADAPTER_BREDR);
}

bool BluetoothHostServer::DisableBt()
{
    return IAdapterManager::GetInstance()->Disable(bluetooth::BTTransport::ADAPTER_BREDR);
}

int32_t BluetoothHostServer::GetBtState()
{
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_VALUE;
    }
    int32_t state = IAdapterManager::GetInstance()->GetState(bluetooth::BTTransport::ADAPTER_BREDR);
    HILOGI("state: %{public}d", state);
    return state;
}

sptr<IRemoteObject> BluetoothHostServer::GetProfile(const std::string &name)
{
    HILOGI("seraching %{public}s ", name.c_str());
    auto it = pimpl->servers_.find(name);
    if (it != pimpl->servers_.end()) {
        HILOGI("server serached %{public}s ", name.c_str());
        return pimpl->servers_[name];
    } else {
        return nullptr;
    }
}

sptr<IRemoteObject> BluetoothHostServer::GetBleRemote(const std::string &name)
{
    HILOGI("GetBleRemote %{public}s ", name.c_str());
    auto iter = pimpl->bleServers_.find(name);
    if (iter != pimpl->bleServers_.end()) {
        return pimpl->bleServers_[name];
    } else {
        return nullptr;
    }
}

// Fac_Res_CODE
bool BluetoothHostServer::BluetoothFactoryReset()
{
    return IAdapterManager::GetInstance()->FactoryReset();
}

int32_t BluetoothHostServer::GetDeviceType(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    RawAddress addr(address);
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->GetDeviceType(addr);
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->GetDeviceType(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return INVALID_VALUE;
}

std::string BluetoothHostServer::GetLocalAddress()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_MAC_ADDRESS;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->GetLocalAddress();
    } else if (IsBleEnabled()) {
        return pimpl->bleService_->GetLocalAddress();
    } else {
        HILOGE("GetLocalAddress failed");
    }
    return INVALID_MAC_ADDRESS;
}

bool BluetoothHostServer::EnableBle()
{
    HILOGI("Enter!");
    return IAdapterManager::GetInstance()->Enable(BTTransport::ADAPTER_BLE);
}

bool BluetoothHostServer::DisableBle()
{
    HILOGI("Enter!");
    return IAdapterManager::GetInstance()->Disable(BTTransport::ADAPTER_BLE);
}

bool BluetoothHostServer::IsBleEnabled()
{
    return IAdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE) == BTStateID::STATE_TURN_ON;
}

std::vector<uint32_t> BluetoothHostServer::GetProfileList()
{
    HILOGI("Enter!");
    return IProfileManager::GetInstance()->GetProfileServicesList();
}

int32_t BluetoothHostServer::GetMaxNumConnectedAudioDevices()
{
    HILOGI("Enter!");
    return IAdapterManager::GetInstance()->GetMaxNumConnectedAudioDevices();
}

int32_t BluetoothHostServer::GetBtConnectionState()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_VALUE;
    }
    return (int32_t)IAdapterManager::GetInstance()->GetAdapterConnectState();
}

int32_t BluetoothHostServer::GetBtProfileConnState(uint32_t profileId)
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_VALUE;
    }
    return (int32_t)IProfileManager::GetInstance()->GetProfileServiceConnectState(profileId);
}

void BluetoothHostServer::GetLocalSupportedUuids(std::vector<std::string> &uuids)
{
    HILOGI("Enter!");
    IProfileManager::GetInstance()->GetProfileServicesSupportedUuids(uuids);
}

int32_t BluetoothHostServer::GetLocalDeviceClass()
{
    HILOGI("Enter!");
    if (IsBtEnabled()) {
        return pimpl->classicService_->GetLocalDeviceClass();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return 0;
}

bool BluetoothHostServer::SetLocalDeviceClass(const int32_t &deviceClass)
{
    HILOGI("Enter!");
    if (IsBtEnabled()) {
        return pimpl->classicService_->SetLocalDeviceClass(deviceClass);
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return false;
}

std::string BluetoothHostServer::GetLocalName()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_NAME;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->GetLocalName();
    } else if (IsBleEnabled()) {
        return pimpl->bleService_->GetLocalName();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return INVALID_NAME;
}

bool BluetoothHostServer::SetLocalName(const std::string &name)
{
    HILOGI("name: %{public}s", name.c_str());
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        bool ret = pimpl->classicService_->SetLocalName(name);
        if (ret && (IsBleEnabled())) {
            return pimpl->bleService_->SetLocalName(name);
        } else {
            HILOGE("failed!");
            return ret;
        }
    } else if (IsBleEnabled()) {
        return pimpl->bleService_->SetLocalName(name);
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return false;
}

int32_t BluetoothHostServer::GetBtScanMode()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return SCAN_MODE_NONE;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->GetBtScanMode();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return SCAN_MODE_NONE;
}

bool BluetoothHostServer::SetBtScanMode(int32_t mode, int32_t duration)
{
    HILOGI("mode: %{public}d, duration: %{public}d", mode, duration);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->SetBtScanMode(mode, duration);
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return false;
}

int32_t BluetoothHostServer::GetBondableMode(int32_t transport)
{
    HILOGI("transport: %{public}d", transport);
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->GetBondableMode();
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->GetBondableMode();
    } else {
        HILOGE("Parameter::transport invalid or BT current state is not enabled!");
    }
    return BONDABLE_MODE_OFF;
}

bool BluetoothHostServer::SetBondableMode(int32_t transport, int32_t mode)
{
    HILOGI("transport: %{public}d, mode: %{public}d", transport, mode);
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->SetBondableMode(mode);
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->SetBondableMode(mode);
    } else {
        HILOGE("Parameter::transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::StartBtDiscovery()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED ||
        PermissionUtils::VerifyLocationPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->StartBtDiscovery();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::CancelBtDiscovery()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        return pimpl->classicService_->CancelBtDiscovery();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::IsBtDiscovering(int32_t transport)
{
    HILOGI("transport: %{public}d", transport);
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->IsBtDiscovering();
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->IsBtDiscovering();
    } else {
        HILOGE("Parameter::transport invalid or BT current state is not enabled!");
    }
    return false;
}

long BluetoothHostServer::GetBtDiscoveryEndMillis()
{
    HILOGI("Enter!");
    if (IsBtEnabled()) {
        return pimpl->classicService_->GetBtDiscoveryEndMillis();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return INVALID_VALUE;
}

std::vector<BluetoothRawAddress> BluetoothHostServer::GetPairedDevices(int32_t transport)
{
    HILOGI("transport: %{public}d", transport);
    std::vector<BluetoothRawAddress> rawAddrRetVec;
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return rawAddrRetVec;
    }
    std::vector<RawAddress> rawAddrVec;
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        rawAddrVec = pimpl->classicService_->GetPairedDevices();
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        rawAddrVec = pimpl->bleService_->GetPairedDevices();
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }

    for (auto it = rawAddrVec.begin(); it != rawAddrVec.end(); ++it) {
        BluetoothRawAddress rawAddr = BluetoothRawAddress(*it);
        rawAddrRetVec.emplace_back(rawAddr);
    }
    return rawAddrRetVec;
}

bool BluetoothHostServer::RemovePair(int32_t transport, const sptr<BluetoothRawAddress> &device)
{
    if (device == nullptr) {
        HILOGE("device is nullptr.");
        return false;
    }
    HILOGI("addr:%{public}s, transport:%{public}d", GET_ENCRYPT_ADDR(*device), transport);
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return false;
    }
    if ((transport == BTTransport::ADAPTER_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->RemovePair(*device);
    } else if ((transport == BTTransport::ADAPTER_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->RemovePair(*device);
    } else {
        HILOGE("transport invalid or BT/BLE current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::RemoveAllPairs()
{
    HILOGI("Enter!");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return false;
    }
    if (BTStateID::STATE_TURN_ON != IAdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BREDR) &&
        BTStateID::STATE_TURN_ON != IAdapterManager::GetInstance()->GetState(BTTransport::ADAPTER_BLE)) {
        HILOGW("BT current state is not enabled!");
        return false;
    }

    bool ret = true;
    if (IsBtEnabled()) {
        ret = pimpl->classicService_->RemoveAllPairs();
        if (!ret) {
            HILOGE("BREDR RemoveAllPairs failed");
        }
    }

    if (IsBleEnabled()) {
        ret &= pimpl->bleService_->RemoveAllPairs();
        if (!ret) {
            HILOGE("BLE RemoveAllPairs failed");
        }
    }
    return ret;
}

int32_t BluetoothHostServer::GetBleMaxAdvertisingDataLength()
{
    HILOGI("Enter!");
    if (IsBleEnabled()) {
        return pimpl->bleService_->GetBleMaxAdvertisingDataLength();
    } else {
        HILOGW("BT current state is not enabled!");
    }
    return INVALID_VALUE;
}

int32_t BluetoothHostServer::GetPhonebookPermission(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    return (int32_t)IAdapterManager::GetInstance()->GetPhonebookPermission(address);
}

bool BluetoothHostServer::SetPhonebookPermission(const std::string &address, int32_t permission)
{
    HILOGI("address: %{public}s, permission: %{public}d", GetEncryptAddr(address).c_str(), permission);
    return IAdapterManager::GetInstance()->SetPhonebookPermission(address, (BTPermissionType)permission);
}

int32_t BluetoothHostServer::GetMessagePermission(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    return (int32_t)IAdapterManager::GetInstance()->GetMessagePermission(address);
}

bool BluetoothHostServer::SetMessagePermission(const std::string &address, int32_t permission)
{
    HILOGI("address: %{public}s, permission: %{public}d", GetEncryptAddr(address).c_str(), permission);
    return IAdapterManager::GetInstance()->SetMessagePermission(address, (BTPermissionType)permission);
}

int32_t BluetoothHostServer::GetPowerMode(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    return IAdapterManager::GetInstance()->GetPowerMode(address);
}

std::string BluetoothHostServer::GetDeviceName(int32_t transport, const std::string &address)
{
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_NAME;
    }
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->GetDeviceName(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->GetDeviceName(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return INVALID_NAME;
}

std::string BluetoothHostServer::GetDeviceAlias(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_NAME;
    }
    if (IsBtEnabled()) {
        RawAddress addr(address);
        return pimpl->classicService_->GetAliasName(addr);
    } else {
        HILOGE("BT current state is not enabled");
    }
    return INVALID_NAME;
}

bool BluetoothHostServer::SetDeviceAlias(const std::string &address, const std::string &aliasName)
{
    HILOGI("address: %{public}s, aliasName: %{public}s", GetEncryptAddr(address).c_str(), aliasName.c_str());
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        RawAddress addr(address);
        return pimpl->classicService_->SetAliasName(addr, aliasName);
    } else {
        HILOGE("BT current state is not enabled");
    }
    return false;
}

int32_t BluetoothHostServer::GetDeviceBatteryLevel(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    if (IsBtEnabled()) {
        RawAddress addr(address);
        return pimpl->classicService_->GetDeviceBatteryLevel(addr);
    } else {
        HILOGE("BT current state is not enabled");
    }
    return INVALID_VALUE;
}

int32_t BluetoothHostServer::GetPairState(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->GetPairState(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->GetPairState(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return INVALID_VALUE;
}

bool BluetoothHostServer::StartPair(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("StartPair false, check permission failed");
        return false;
    }
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->StartPair(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->StartPair(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::CancelPairing(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->CancelPairing(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->CancelPairing(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::IsBondedFromLocal(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->IsBondedFromLocal(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->IsBondedFromLocal(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::IsAclConnected(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->IsAclConnected(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->IsAclConnected(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::IsAclEncrypted(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->IsAclEncrypted(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->IsAclEncrypted(addr);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

int32_t BluetoothHostServer::GetDeviceClass(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return INVALID_VALUE;
    }
    if (IsBtEnabled()) {
        RawAddress addr(address);
        return pimpl->classicService_->GetDeviceClass(addr);
    } else {
        HILOGE("BT current state is not enabled!");
    }
    return INVALID_VALUE;
}

std::vector<bluetooth::Uuid> BluetoothHostServer::GetDeviceUuids(int32_t transport, const std::string &address)
{
    HILOGI("transport: %{public}d, address: %{public}s", transport, GetEncryptAddr(address).c_str());
    std::vector<bluetooth::Uuid> parcelUuids;
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        parcelUuids = pimpl->classicService_->GetDeviceUuids(addr);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        parcelUuids = pimpl->bleService_->GetDeviceUuids(addr);
    } else {
        HILOGE("BT current state is not enabled!");
    }
    return parcelUuids;
}

bool BluetoothHostServer::SetDevicePin(const std::string &address, const std::string &pin)
{
    HILOGI("address: %{public}s, pin: %{public}s", GetEncryptAddr(address).c_str(), pin.c_str());
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBtEnabled()) {
        RawAddress addr(address);
        return pimpl->classicService_->SetDevicePin(addr, pin);
    } else {
        HILOGE("BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::SetDevicePairingConfirmation(int32_t transport, const std::string &address, bool accept)
{
    HILOGI("transport: %{public}d, address: %{public}s, accept: %{public}d",
        transport, GetEncryptAddr(address).c_str(), accept);
    if (PermissionUtils::VerifyManageBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->SetDevicePairingConfirmation(addr, accept);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->SetDevicePairingConfirmation(addr, accept);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::SetDevicePasskey(int32_t transport, const std::string &address, int32_t passkey, bool accept)
{
    HILOGI("transport: %{public}d, address: %{public}s, passkey: %{public}d, accept: %{public}d",
        transport, GetEncryptAddr(address).c_str(), passkey, accept);
    if (PermissionUtils::VerifyManageBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->SetDevicePasskey(addr, passkey, accept);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->SetDevicePasskey(addr, passkey, accept);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::PairRequestReply(int32_t transport, const std::string &address, bool accept)
{
    HILOGI("transport: %{public}d, address: %{public}s, accept: %{public}d",
        transport, GetEncryptAddr(address).c_str(), accept);
    RawAddress addr(address);
    if ((transport == BT_TRANSPORT_BREDR) && IsBtEnabled()) {
        return pimpl->classicService_->PairRequestReply(addr, accept);
    } else if ((transport == BT_TRANSPORT_BLE) && IsBleEnabled()) {
        return pimpl->bleService_->PairRequestReply(addr, accept);
    } else {
        HILOGE("transport invalid or BT current state is not enabled!");
    }
    return false;
}

bool BluetoothHostServer::ReadRemoteRssiValue(const std::string &address)
{
    HILOGI("address: %{public}s", GetEncryptAddr(address).c_str());
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return false;
    }
    if (IsBleEnabled()) {
        RawAddress addr(address);
        return pimpl->bleService_->ReadRemoteRssiValue(addr);
    } else {
        HILOGE("BT current state is not enabled!");
    }
    return false;
}

void BluetoothHostServer::RegisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer)
{
    HILOGI("Enter!");
    if (observer == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    pimpl->remoteObserversToken_[observer->AsObject()] = IPCSkeleton::GetCallingTokenID();
    pimpl->remoteObserversUid_[observer->AsObject()] = IPCSkeleton::GetCallingUid();
    pimpl->remoteObservers_.Register(observer);
    pimpl->remoteDeviceObservers_.push_back(observer);
}

void BluetoothHostServer::DeregisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer)
{
    HILOGI("Enter!");
    if (observer == nullptr || pimpl == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    for (auto iter = pimpl->remoteDeviceObservers_.begin(); iter != pimpl->remoteDeviceObservers_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            pimpl->remoteObservers_.Deregister(*iter);
            pimpl->remoteDeviceObservers_.erase(iter);
            break;
        }
    }
    for (auto iter =  pimpl->remoteObserversToken_.begin(); iter !=  pimpl->remoteObserversToken_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->remoteObserversToken_.erase(iter);
            break;
        }
    }
    for (auto iter = pimpl->remoteObserversUid_.begin(); iter != pimpl->remoteObserversUid_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->remoteObserversUid_.erase(iter);
            break;
        }
    }
}

bool BluetoothHostServer::IsBtEnabled()
{
    bool isEnabled = (GetBtState() == static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON)) ? true : false;
    HILOGI("%{public}s", isEnabled ? "true" : "false");
    return isEnabled;
}

void BluetoothHostServer::RegisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    HILOGI("start.");

    if (observer == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    pimpl->bleObserversToken_[observer->AsObject()] = IPCSkeleton::GetCallingTokenID();
    pimpl->bleObserversUid_[observer->AsObject()] = IPCSkeleton::GetCallingUid();
    pimpl->bleObservers_.Register(observer);
    pimpl->bleAdapterObservers_.push_back(observer);
}

void BluetoothHostServer::DeregisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    HILOGI("start.");

    if (observer == nullptr || pimpl == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    for (auto iter = pimpl->bleAdapterObservers_.begin(); iter != pimpl->bleAdapterObservers_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            pimpl->bleObservers_.Deregister(*iter);
            pimpl->bleAdapterObservers_.erase(iter);
            break;
        }
    }
    for (auto iter =  pimpl->bleObserversToken_.begin(); iter !=  pimpl->bleObserversToken_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->bleObserversToken_.erase(iter);
            break;
        }
    }
    for (auto iter = pimpl->bleObserversUid_.begin(); iter != pimpl->bleObserversUid_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->bleObserversUid_.erase(iter);
            break;
        }
    }
}

void BluetoothHostServer::RegisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer)
{
    HILOGI("start.");

    if (observer == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    pimpl->bleRemoteObserversToken_[observer->AsObject()] = IPCSkeleton::GetCallingTokenID();
    pimpl->bleRemoteObservers_.Register(observer);
    pimpl->blePeripheralObservers_.push_back(observer);
}

void BluetoothHostServer::DeregisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer)
{
    HILOGI("start.");

    if (observer == nullptr) {
        HILOGE("observer is nullptr!");
        return;
    }
    for (auto iter = pimpl->blePeripheralObservers_.begin(); iter != pimpl->blePeripheralObservers_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            if (pimpl != nullptr) {
                pimpl->bleRemoteObservers_.Deregister(*iter);
                pimpl->blePeripheralObservers_.erase(iter);
                break;
            }
        }
    }
    for (auto iter =  pimpl->bleRemoteObserversToken_.begin(); iter !=  pimpl->bleRemoteObserversToken_.end(); ++iter) {
        if (iter->first == observer->AsObject()) {
            pimpl->bleRemoteObserversToken_.erase(iter);
            break;
        }
    }
}

int32_t BluetoothHostServer::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    HILOGI("fd: %{public}d", fd);
    std::vector<std::string> argsInStr8;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr8), [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    std::string result;
    BluetoothHostDumper::BluetoothDump(argsInStr8, result);

    if (!SaveStringToFd(fd, result)) {
        HILOGE("bluetooth dump save string to fd failed!");
        return ERR_INVALID_OPERATION;
    }
    return ERR_OK;
}
}  // namespace Bluetooth
}  // namespace OHOS
