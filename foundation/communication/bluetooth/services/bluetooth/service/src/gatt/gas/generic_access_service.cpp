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

#include "generic_access_service.h"
#include <string>
#include "adapter_device_config.h"
#include "bt_def.h"
#include "gatt/gatt_defines.h"
#include "interface_adapter_ble.h"
#include "interface_adapter_classic.h"
#include "interface_adapter_manager.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const uint16_t GenericAccessService::GATT_UUID_GAP_SERVICE = 0x1800;
const uint16_t GenericAccessService::GATT_UUID_GAP_DEVICE_NAME = 0x2A00;
const uint16_t GenericAccessService::GATT_UUID_GAP_APPEARANCE = 0x2A01;
const uint16_t GenericAccessService::GATT_UUID_GAP_CENTRAL_ADDR_RESOLUTION = 0x2AA6;
const std::string GenericAccessService::DEFAULT_DEVICE_NAME = "BluetoothDevice";
const uint16_t GenericAccessService::DEFAULT_APPEARANCE = 0x02;
const size_t GenericAccessService::APPEARANCE_DATE_LENGTH = 0x02;
const size_t GenericAccessService::CENTRAL_ADDR_RESOLUTION_DATE_LENGTH = 0x01;

class GenericAccessService::GattServerCallbackImpl : public IGattServerCallback {
public:
    void OnCharacteristicReadRequest(const GattDevice &device, const Characteristic &characteristic) override
    {
        auto cIt = std::find_if(service_.instance_->characteristics_.begin(),
            service_.instance_->characteristics_.end(),
            [&characteristic](Characteristic &ccc) {
            return ccc.handle_ == characteristic.handle_;
        });

        if (cIt != service_.instance_->characteristics_.end()) {
            Characteristic result(characteristic.handle_, cIt->value_.get(), cIt->length_);
            service_.serverService_.RespondCharacteristicRead(device, result, GattStatus::GATT_SUCCESS);
            return;
        }

        Characteristic result(characteristic.handle_);
        service_.serverService_.RespondCharacteristicRead(device, result, GattStatus::GATT_FAILURE);
    }

    void OnCharacteristicReadByUuidRequest(const GattDevice &device, const Characteristic &characteristic) override
    {
        auto cIt = std::find_if(service_.instance_->characteristics_.begin(),
            service_.instance_->characteristics_.end(),
            [&characteristic](Characteristic &ccc) {
            return ccc.handle_ == characteristic.handle_;
        });

        if (cIt != service_.instance_->characteristics_.end()) {
            Characteristic result(characteristic.handle_, cIt->value_.get(), cIt->length_);
            service_.serverService_.RespondCharacteristicReadByUuid(device, result, GattStatus::GATT_SUCCESS);
            return;
        }

        Characteristic result(characteristic.handle_);
        service_.serverService_.RespondCharacteristicReadByUuid(device, result, GattStatus::GATT_FAILURE);
    }

    void OnConnectionStateChanged(const GattDevice &device, int ret, int state) override
    {}

    explicit GattServerCallbackImpl(const GenericAccessService &service) : service_(service)
    {}
    ~GattServerCallbackImpl()
    {}

private:
    const GenericAccessService &service_;
};

class GenericAccessService::AdapterBleObserverImpl : public IAdapterBleObserver {
public:
    explicit AdapterBleObserverImpl(GenericAccessService &service) : service_(service)
    {}
    ~AdapterBleObserverImpl() = default;
    void OnDiscoveryStateChanged(const int status)
    {}
    void OnDiscoveryResult(const RawAddress &device)
    {}
    void OnPairRequested(const BTTransport transport, const RawAddress &device)
    {}
    void OnPairConfirmed(const BTTransport transport, const RawAddress &device, const int reqType, const int number)
    {}
    void OnScanModeChanged(const int mode)
    {}
    void OnDeviceNameChanged(const std::string deviceName)
    {
        service_.dispatcher_.PostTask(std::bind(&GenericAccessService::OnDeviceNameChanged, &service_, deviceName));
    }
    void OnDeviceAddrChanged(const std::string address)
    {}
    void OnAdvertisingStateChanged(const int state)
    {
        service_.dispatcher_.PostTask(std::bind(&GenericAccessService::OnAdvertisingStateChanged, &service_, state));
    }

private:
    GenericAccessService &service_;
};

class GenericAccessService::AdapterClassicObserverImpl : public IAdapterClassicObserver {
public:
    explicit AdapterClassicObserverImpl(GenericAccessService &service) : service_(service)
    {}
    ~AdapterClassicObserverImpl() = default;
    void OnDiscoveryStateChanged(int status)
    {}
    void OnDiscoveryResult(const RawAddress &device)
    {}
    void OnPairRequested(const BTTransport transport, const RawAddress &device)
    {}
    void OnPairConfirmed(const BTTransport transport, const RawAddress &device, int reqType, int number)
    {}

    void OnScanModeChanged(int mode)
    {
        service_.dispatcher_.PostTask(std::bind(&GenericAccessService::OnScanModeChanged, &service_, mode));
    }

    void OnDeviceNameChanged(const std::string &deviceName)
    {
        service_.dispatcher_.PostTask(std::bind(&GenericAccessService::OnDeviceNameChanged, &service_, deviceName));
    }
    void OnDeviceAddrChanged(const std::string &address)
    {}

private:
    GenericAccessService &service_;
};

GenericAccessService::GenericAccessService(GattServerService &service, utility::Dispatcher &dispatcher)
    : serviceCallback_(std::make_shared<GattServerCallbackImpl>(*this)),
      adapterBleObserver_(std::make_unique<AdapterBleObserverImpl>(*this)),
      adapterClassicObserver_(std::make_unique<AdapterClassicObserverImpl>(*this)),
      serverService_(service),
      dispatcher_(dispatcher)
{}

GenericAccessService::~GenericAccessService()
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
}

int GenericAccessService::RegisterService()
{
    IAdapterBle *adapterBle = (IAdapterBle *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BLE));
    if (adapterBle != nullptr) {
        adapterBle->RegisterBleAdapterObserver(*adapterBleObserver_.get());
        auto state = adapterBle->GetAdvertisingStatus();
        if (state == BleAdvState::BLE_ADV_STATE_ADVERTISING) {
            bleDiscoverable_ = true;
        } else {
            bleDiscoverable_ = false;
        }
        central_addr_resolution_ = adapterBle->IsLlPrivacySupported() ? 1 : 0;
    }

    IAdapterClassic *adapterClassic = (IAdapterClassic *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BREDR));
    if (adapterClassic != nullptr) {
        adapterClassic->RegisterClassicAdapterObserver(*adapterClassicObserver_.get());
        auto mode = adapterClassic->GetBtScanMode();
        if (mode == SCAN_MODE_GENERAL_DISCOVERABLE || mode == SCAN_MODE_LIMITED_DISCOVERABLE ||
            mode == SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE || mode == SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE) {
            classicDiscoverable_ = true;
        } else {
            classicDiscoverable_ = false;
        }
    }

    appId_ = serverService_.RegisterApplicationSync(serviceCallback_);
    if (appId_ < 0) {
        return appId_;
    }

    instance_ = BuildService();
    int result = serverService_.AddServiceSync(appId_, *instance_);
    if (result != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}d:%{public}s : Add GAS Failed! ret: %{public}d", __FILE__, __LINE__, __FUNCTION__, result);
    }

    return result;
}

void GenericAccessService::DeregisterService()
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    IAdapterBle *adapterBle = (IAdapterBle *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BLE));
    if (adapterBle != nullptr) {
        adapterBle->DeregisterBleAdapterObserver(*adapterBleObserver_.get());
    }

    IAdapterClassic *adapterClassic = (IAdapterClassic *)(IAdapterManager::GetInstance()->GetAdapter(ADAPTER_BREDR));
    if (adapterClassic != nullptr) {
        adapterClassic->DeregisterClassicAdapterObserver(*adapterClassicObserver_.get());
    }

    serverService_.DeregisterApplicationSync(appId_);
}

std::unique_ptr<Service> GenericAccessService::BuildService()
{
    std::unique_ptr<Service> svc = std::make_unique<Service>(Uuid::ConvertFrom16Bits(GATT_UUID_GAP_SERVICE), 0, 0, 0);
    svc->isPrimary_ = true;

    std::string value = GetDeviceName();
    svc->characteristics_.push_back(Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_GAP_DEVICE_NAME),
        0,
        (bleDiscoverable_ || classicDiscoverable_) ? CHARACTERISTIC_PROPERTIE_READ : 0,
        (bleDiscoverable_ || classicDiscoverable_) ? (int)GattPermission::READABLE : 0,
        (uint8_t *)value.c_str(),
        value.size()));

    svc->characteristics_.push_back(Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_GAP_APPEARANCE),
        0,
        CHARACTERISTIC_PROPERTIE_READ,
        (int)GattPermission::READABLE,
        GetAppearance().get(),
        APPEARANCE_DATE_LENGTH));

    svc->characteristics_.push_back(Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_GAP_CENTRAL_ADDR_RESOLUTION),
        0,
        CHARACTERISTIC_PROPERTIE_READ,
        (int)GattPermission::READABLE,
        &central_addr_resolution_,
        CENTRAL_ADDR_RESOLUTION_DATE_LENGTH));
    return svc;
}

std::string GenericAccessService::GetDeviceName()
{
    std::string result = DEFAULT_DEVICE_NAME;
    AdapterDeviceConfig::GetInstance()->GetValue(SECTION_HOST, PROPERTY_DEVICE_NAME, result);
    return result;
}

std::unique_ptr<uint8_t[]> GenericAccessService::GetAppearance() const
{
    int appearance = DEFAULT_APPEARANCE;
    AdapterDeviceConfig::GetInstance()->GetValue(SECTION_HOST, PROPERTY_BLE_APPEARANCE, appearance);

    std::unique_ptr<uint8_t[]> result = std::make_unique<uint8_t[]>(APPEARANCE_DATE_LENGTH);
    if (memcpy_s(result.get(), APPEARANCE_DATE_LENGTH, &appearance, APPEARANCE_DATE_LENGTH) != EOK) {
        LOG_DEBUG("%{public}s:%{public}d:%{public}s : memcpy_s fail", __FILE__, __LINE__, __FUNCTION__);
        return NULL;
    }

    return result;
}

void GenericAccessService::OnDeviceNameChanged(std::string &deviceName) const
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s : device name: %{public}s", __FILE__, __LINE__, __FUNCTION__, deviceName.c_str());

    auto cIt = std::find_if(instance_->characteristics_.begin(),
            instance_->characteristics_.end(),
            [](Characteristic &ccc) {
            return ccc.uuid_ == Uuid::ConvertFrom16Bits(GATT_UUID_GAP_DEVICE_NAME);
        });

    if (cIt != instance_->characteristics_.end()) {
        cIt->SetValue((uint8_t *)deviceName.c_str(), deviceName.length());
        serverService_.SetCharacteristicValue(*cIt);
        return;
    }
}

void GenericAccessService::OnAdvertisingStateChanged(int state)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s :state: %{public}d", __FILE__, __LINE__, __FUNCTION__, state);
    if (state == BleAdvState::BLE_ADV_STATE_ADVERTISING) {
        bleDiscoverable_ = true;
    } else {
        bleDiscoverable_ = false;
    }

    for (auto &ccc : instance_->characteristics_) {
        if (!(ccc.uuid_ == Uuid::ConvertFrom16Bits(GATT_UUID_GAP_DEVICE_NAME))) {
            continue;
        }
        if (classicDiscoverable_ || bleDiscoverable_) {
            if (!(ccc.properties_ & CHARACTERISTIC_PROPERTIE_READ)) {
                serverService_.SetCharacteristicPermission(ccc,
                    ccc.properties_ | CHARACTERISTIC_PROPERTIE_READ,
                    ccc.permissions_ | (int)GattPermission::READABLE);
            }
        } else {
            if (ccc.properties_ & CHARACTERISTIC_PROPERTIE_READ) {
                serverService_.SetCharacteristicPermission(ccc,
                    ccc.properties_ & ~CHARACTERISTIC_PROPERTIE_READ,
                    ccc.permissions_ & ~(int)GattPermission::READABLE);
            }
        }
        return;
    }
}

void GenericAccessService::OnScanModeChanged(int mode)
{
    LOG_DEBUG("%{public}s:%{public}d:%{public}s :mode: %{public}d", __FILE__, __LINE__, __FUNCTION__, mode);
    if (mode == SCAN_MODE_GENERAL_DISCOVERABLE || mode == SCAN_MODE_LIMITED_DISCOVERABLE ||
        mode == SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE || mode == SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE) {
        classicDiscoverable_ = true;
    } else {
        classicDiscoverable_ = false;
    }

    for (auto &ccc : instance_->characteristics_) {
        if (!(ccc.uuid_ == Uuid::ConvertFrom16Bits(GATT_UUID_GAP_DEVICE_NAME))) {
            continue;
        }
        if (classicDiscoverable_ || bleDiscoverable_) {
            if (!(ccc.properties_ & CHARACTERISTIC_PROPERTIE_READ)) {
                serverService_.SetCharacteristicPermission(ccc,
                    ccc.properties_ | CHARACTERISTIC_PROPERTIE_READ,
                    ccc.permissions_ | (int)GattPermission::READABLE);
            }
        } else {
            if (ccc.properties_ & CHARACTERISTIC_PROPERTIE_READ) {
                serverService_.SetCharacteristicPermission(ccc,
                    ccc.properties_ & ~CHARACTERISTIC_PROPERTIE_READ,
                    ccc.permissions_ & ~(int)GattPermission::READABLE);
            }
        }
        return;
    }
}
}  // namespace bluetooth
}  // namespace OHOS