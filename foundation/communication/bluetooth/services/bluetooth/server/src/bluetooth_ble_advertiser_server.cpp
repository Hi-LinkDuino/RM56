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

#include "bluetooth_ble_advertiser_server.h"

#include "bluetooth_ble_central_manager_server.h"
#include "bluetooth_log.h"
#include "interface_adapter_ble.h"
#include "interface_adapter_manager.h"
#include "ipc_skeleton.h"
#include "remote_observer_list.h"
#include "permission_utils.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class BleAdvertiserCallback : public IBleAdvertiserCallback {
public:
    BleAdvertiserCallback() = default;
    ~BleAdvertiserCallback() override = default;

    void OnStartResultEvent(int result, uint8_t advHandle, int opcode) override
    {
        HILOGI("result: %{public}d, advHandle: %{public}d, opcode: %{public}d", result, advHandle, opcode);

        observers_->ForEach([this, result, advHandle, opcode](IBluetoothBleAdvertiseCallback *observer) {
            int32_t uid = observersUid_[observer->AsObject()];
            if (BluetoothBleCentralManagerServer::IsProxyUid(uid)) {
                HILOGD("uid:%{public}d is proxy uid, not callback.", uid);
                return;
            }
            observer->OnStartResultEvent(result, advHandle, opcode);
        });
    }

    void OnAutoStopAdvEvent(uint8_t advHandle) override
    {
        HILOGI("advHandle: %{public}d", advHandle);

        observers_->ForEach(
            [advHandle](IBluetoothBleAdvertiseCallback *observer) { observer->OnAutoStopAdvEvent(advHandle); });
    }

    void SetObserver(RemoteObserverList<IBluetoothBleAdvertiseCallback> *observers)
    {
        observers_ = observers;
    }

    std::map<sptr<IRemoteObject>, int32_t> observersUid_;

private:
    RemoteObserverList<IBluetoothBleAdvertiseCallback> *observers_;
};

struct BluetoothBleAdvertiserServer::impl {
    impl();
    ~impl();

    /// sys state observer
    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothBleAdvertiseCallback> observers_;
    std::unique_ptr<BleAdvertiserCallback> observerImp_ = std::make_unique<BleAdvertiserCallback>();
    IAdapterBle *bleService_ = nullptr;
    std::vector<sptr<IBluetoothBleAdvertiseCallback>> advCallBack_;
};

class BluetoothBleAdvertiserServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    explicit SystemStateObserver(BluetoothBleAdvertiserServer::impl *pimpl) : pimpl_(pimpl) {};
    void OnSystemStateChange(const BTSystemState state) override
    {
        pimpl_->bleService_ =
            static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));
        switch (state) {
            case BTSystemState::ON:
                if (pimpl_->bleService_ != nullptr) {
                    pimpl_->bleService_->RegisterBleAdvertiserCallback(*pimpl_->observerImp_.get());
                }
                break;
            case BTSystemState::OFF:
                pimpl_->bleService_ = nullptr;
                break;
            default:
                break;
        }
    };

private:
    BluetoothBleAdvertiserServer::impl *pimpl_ = nullptr;
};

BluetoothBleAdvertiserServer::impl::impl()
{}

BluetoothBleAdvertiserServer::impl::~impl()
{
    bleService_ = static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));
    if (bleService_ != nullptr) {
        bleService_->DeregisterBleAdvertiserCallback();
    }
}

BluetoothBleAdvertiserServer::BluetoothBleAdvertiserServer()
{
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    pimpl->bleService_ =
        static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));
    if (pimpl->bleService_ != nullptr) {
        pimpl->bleService_->RegisterBleAdvertiserCallback(*pimpl->observerImp_.get());
    }
}

BluetoothBleAdvertiserServer::~BluetoothBleAdvertiserServer()
{
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
}

void BluetoothBleAdvertiserServer::StartAdvertising(const BluetoothBleAdvertiserSettings &settings,
    const BluetoothBleAdvertiserData &advData, const BluetoothBleAdvertiserData &scanResponse, int32_t advHandle,
    bool isRawData)
{
    HILOGI("enter");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return;
    }

    pimpl->bleService_ =
        static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));

    if (pimpl->bleService_ != nullptr) {
        BleAdvertiserSettingsImpl settingsImpl;
        settingsImpl.SetConnectable(settings.IsConnectable());
        settingsImpl.SetInterval(settings.GetInterval());
        settingsImpl.SetLegacyMode(settings.IsLegacyMode());
        settingsImpl.SetTxPower(settings.GetTxPower());

        BleAdvertiserDataImpl bleAdvertiserData;
        if (!isRawData) {
            bleAdvertiserData.SetFlags(advData.GetAdvFlag());
        }
        std::map<uint16_t, std::string> manufacturerData = advData.GetManufacturerData();
        for (auto iter = manufacturerData.begin(); iter != manufacturerData.end(); iter++) {
            bleAdvertiserData.AddManufacturerData(iter->first, iter->second);
        }
        std::map<Uuid, std::string> serviceData = advData.GetServiceData();
        for (auto it = serviceData.begin(); it != serviceData.end(); it++) {
            bleAdvertiserData.AddServiceData(it->first, it->second);
        }
        std::vector<Uuid> serviceUuids = advData.GetServiceUuids();
        for (auto it = serviceUuids.begin(); it != serviceUuids.end(); it++) {
            bleAdvertiserData.AddServiceUuid(*it);
        }
        bleAdvertiserData.AddData(advData.GetPayload());

        BleAdvertiserDataImpl bleScanResponse;
        manufacturerData = scanResponse.GetManufacturerData();
        for (auto it = manufacturerData.begin(); it != manufacturerData.end(); it++) {
            bleScanResponse.AddManufacturerData(it->first, it->second);
        }
        serviceData = scanResponse.GetServiceData();
        for (auto it = serviceData.begin(); it != serviceData.end(); it++) {
            bleScanResponse.AddServiceData(it->first, it->second);
        }
        serviceUuids = scanResponse.GetServiceUuids();
        for (auto it = serviceUuids.begin(); it != serviceUuids.end(); it++) {
            bleScanResponse.AddServiceUuid(*it);
        }
        bleScanResponse.AddData(scanResponse.GetPayload());

        if (pimpl->bleService_ != nullptr) {
            pimpl->bleService_->StartAdvertising(settingsImpl, bleAdvertiserData, bleScanResponse, advHandle);
        }
    }
}

void BluetoothBleAdvertiserServer::StopAdvertising(int32_t advHandle)
{
    HILOGI("enter, advHandle: %{public}d", advHandle);
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return;
    }

    pimpl->bleService_ =
        static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));

    if (pimpl->bleService_ != nullptr) {
        pimpl->bleService_->StopAdvertising(advHandle);
    }
}

void BluetoothBleAdvertiserServer::Close(int32_t advHandle)
{
    HILOGI("enter, advHandle: %{public}d", advHandle);

    pimpl->bleService_ =
        static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));

    if (pimpl->bleService_ != nullptr) {
        pimpl->bleService_->Close(advHandle);
    }
}

void BluetoothBleAdvertiserServer::RegisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback)
{
    HILOGI("enter");

    if (callback == nullptr) {
        HILOGE("callback is null");
        return;
    }
    if (pimpl != nullptr) {
        pimpl->observerImp_->observersUid_[callback->AsObject()] = IPCSkeleton::GetCallingUid();
        pimpl->observers_.Register(callback);
        pimpl->advCallBack_.push_back(callback);
    }
}

void BluetoothBleAdvertiserServer::DeregisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback)
{
    HILOGI("enter");

    if (callback == nullptr || pimpl == nullptr) {
        HILOGE("callback is null, or pimpl is null");
        return;
    }
    for (auto iter = pimpl->advCallBack_.begin(); iter != pimpl->advCallBack_.end(); ++iter) {
        if ((*iter)->AsObject() == callback->AsObject()) {
            HILOGI("Deregister observer");
            pimpl->observers_.Deregister(*iter);
            pimpl->advCallBack_.erase(iter);
            break;
        }
    }
    for (auto iter = pimpl->observerImp_->observersUid_.begin(); iter != pimpl->observerImp_->observersUid_.end();
        ++iter) {
        if (iter->first == callback->AsObject()) {
            pimpl->observerImp_->observersUid_.erase(iter);
            break;
        }
    }
}

int32_t BluetoothBleAdvertiserServer::GetAdvertiserHandle()
{
    HILOGI("enter");

    int32_t advHandle = BLE_INVALID_ADVERTISING_HANDLE;

    pimpl->bleService_ =
        static_cast<IAdapterBle *>(IAdapterManager::GetInstance()->GetAdapter(BTTransport::ADAPTER_BLE));

    if (pimpl->bleService_ != nullptr) {
        advHandle = pimpl->bleService_->GetAdvertiserHandle();
    }

    return advHandle;
}
}  // namespace Bluetooth
}  // namespace OHOS