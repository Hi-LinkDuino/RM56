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

#include "device_information_service.h"
#include <string>
#include "adapter_device_info.h"
#include "bt_def.h"
#include "class_creator.h"
#include "gatt/gatt_defines.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
const uint16_t DeviceInformationService::GATT_UUID_MANUFACTURER_NAME_STRING_CHAR = 0x2A29;
const uint16_t DeviceInformationService::GATT_UUID_MODEL_NUMBER_STRING_CHAR = 0x2A24;
const uint16_t DeviceInformationService::GATT_UUID_PNP_ID_CHAR = 0x2A50;
const uint16_t DeviceInformationService::GATT_UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST_CHAR = 0x2A2A;
const uint16_t DeviceInformationService::GATT_UUID_SERIAL_NUMBER_STRING_CHAR = 0x2A25;
const uint16_t DeviceInformationService::GATT_UUID_SOFTWARE_REVISION_STRING_CHAR = 0x2A28;
const uint16_t DeviceInformationService::GATT_UUID_SYSTEM_ID_CHAR = 0x2A23;
const uint16_t DeviceInformationService::GATT_UUID_HARDWARE_REVISION_STRING_CHAR = 0x2A27;
const uint16_t DeviceInformationService::GATT_UUID_FIRMWARE_REVISION_STRING_CHAR = 0x2A26;

const std::string DeviceInformationService::DEFAULT_MANUFACTURER_NAME = "Hikey960";
const std::string DeviceInformationService::DEFAULT_MODEL_NUMBER = "960";
const std::string DeviceInformationService::DEFAULT_SERIAL_NUMBER = "AAAA-S032-4321";
const std::string DeviceInformationService::DEFAULT_HARDWARE_REVISION = "1.0.0";
const std::string DeviceInformationService::DEFAULT_FIRMWARE_REVISION = "1.0.0";
const std::string DeviceInformationService::DEFAULT_SOFTWARE_REVISION = "1.0.0";
const std::string DeviceInformationService::DEFAULT_SYSTEM_ID = "-";
const std::string DeviceInformationService::DEFAULT_IEEE_REGULATORY_CERTIFICATION_DATA_LIST = "-";
const std::string DeviceInformationService::DEFAULT_PNP_ID = "-";

DeviceInformationService::DeviceInformationService(GattServerService &service)
    : serverService_(service), serviceCallback_(std::make_shared<GattServerCallbackImpl>(*this))
{
    AdapterDeviceInfo::GetInstance()->Load();
}

DeviceInformationService::~DeviceInformationService()
{}

class DeviceInformationService::GattServerCallbackImpl : public IGattServerCallback {
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
        for (auto &ccc : service_.instance_->characteristics_) {
            if (ccc.handle_ == characteristic.handle_) {
                Characteristic result(characteristic.handle_, ccc.value_.get(), ccc.length_);
                service_.serverService_.RespondCharacteristicReadByUuid(device, result, GattStatus::GATT_SUCCESS);
                return;
            }
        }

        Characteristic result(characteristic.handle_);
        service_.serverService_.RespondCharacteristicReadByUuid(device, result, GattStatus::GATT_FAILURE);
    }

    void OnConnectionStateChanged(const GattDevice &device, int ret, int state) override
    {}

    explicit GattServerCallbackImpl(const DeviceInformationService &service) : service_(service)
    {}
    ~GattServerCallbackImpl()
    {}

private:
    const DeviceInformationService &service_;
};

int DeviceInformationService::RegisterService()
{
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

void DeviceInformationService::DeregisterService() const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    serverService_.DeregisterApplicationSync(appId_);
}

int DeviceInformationService::RegisterSDP()
{
    if (instance_ == nullptr) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    sdpRegister_.reset(ClassCreator<DiServiceOverBredr>::NewInstance("DiServiceOverBredr"));
    if (sdpRegister_ != nullptr) {
        static_cast<DiServiceOverBredrInterface *>(sdpRegister_.get())
            ->RegisterSDP(instance_->startHandle_, instance_->endHandle_);
    }

    LOG_DEBUG("[DeviceInformationService]::%{public}s", __FUNCTION__);

    return GattStatus::GATT_SUCCESS;
}

void DeviceInformationService::DeregisterSDP() const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (sdpRegister_ != nullptr) {
        static_cast<DiServiceOverBredrInterface *>(sdpRegister_.get())->DeregisterSDP();
    }
}

std::unique_ptr<Service> DeviceInformationService::BuildService()
{
    std::unique_ptr<Service> svc =
        std::make_unique<Service>(Uuid::ConvertFrom16Bits(UUID_DEVICE_INFORMATION_SERVICE), 0, 0, 0);
    svc->isPrimary_ = true;

    std::string value = GetManufactureName();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_MANUFACTURER_NAME_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetModelNumber();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_MODEL_NUMBER_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetPnpId();
    svc->characteristics_.push_back(Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_PNP_ID_CHAR),
        0,
        CHARACTERISTIC_PROPERTIE_READ,
        (int)GattPermission::READABLE,
        (uint8_t *)value.c_str(),
        value.size()));

    value = GetIEEEInfo();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetSerialNumber();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_SERIAL_NUMBER_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetSoftwareRevision();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_SOFTWARE_REVISION_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetSystemId();
    svc->characteristics_.push_back(Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_SYSTEM_ID_CHAR),
        0,
        CHARACTERISTIC_PROPERTIE_READ,
        (int)GattPermission::READABLE,
        (uint8_t *)value.c_str(),
        value.size()));

    value = GetHardwareRevision();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_HARDWARE_REVISION_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    value = GetFirmwareRevision();
    svc->characteristics_.push_back(
        Characteristic(Uuid::ConvertFrom16Bits(GATT_UUID_FIRMWARE_REVISION_STRING_CHAR),
            0,
            CHARACTERISTIC_PROPERTIE_READ,
            (int)GattPermission::READABLE,
            (uint8_t *)value.c_str(),
            value.size()));

    return svc;
}

std::string DeviceInformationService::GetSystemId()
{
    std::string result = DEFAULT_SYSTEM_ID;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_SYSYTEM_ID, result);
    return result;
}

std::string DeviceInformationService::GetPnpId()
{
    std::string result = DEFAULT_PNP_ID;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_PNP_ID, result);
    return result;
}

std::string DeviceInformationService::GetIEEEInfo()
{
    std::string result = DEFAULT_IEEE_REGULATORY_CERTIFICATION_DATA_LIST;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_IEEE_INFO, result);
    return result;
}

std::string DeviceInformationService::GetManufactureName()
{
    std::string result = DEFAULT_MANUFACTURER_NAME;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_MANUFACTURER_NAME, result);
    return result;
}

std::string DeviceInformationService::GetModelNumber()
{
    std::string result = DEFAULT_MODEL_NUMBER;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_MODEL_NUMBER, result);
    return result;
}

std::string DeviceInformationService::GetSerialNumber()
{
    std::string result = DEFAULT_SERIAL_NUMBER;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_SERIAL_NUMBER, result);
    return result;
}

std::string DeviceInformationService::GetHardwareRevision()
{
    std::string result = DEFAULT_HARDWARE_REVISION;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_HARDWARE_REVISION, result);
    return result;
}

std::string DeviceInformationService::GetFirmwareRevision()
{
    std::string result = DEFAULT_FIRMWARE_REVISION;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_FIRMWARE_REVISION, result);
    return result;
}

std::string DeviceInformationService::GetSoftwareRevision()
{
    std::string result = DEFAULT_SOFTWARE_REVISION;
    AdapterDeviceInfo::GetInstance()->GetValue(SECTION_BLE, PROPERTY_SOFTWARE_REVISION, result);
    return result;
}
}  // namespace bluetooth
}  // namespace OHOS