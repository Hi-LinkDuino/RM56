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

#include "generic_attribute_service.h"
#include "../common/compat.h"
#include "../gatt_connection_manager.h"
#include "../gatt_connection_observer.h"
#include "../gatt_defines.h"
#include "adapter_device_config.h"
#include "class_creator.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const size_t GenericAttributeService::CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH = 0x02;
const size_t GenericAttributeService::SERVICE_CHANGED_VALUE_LENGTH = 0x04;
const uint8_t GenericAttributeService::CLIENT_CHARACTERISTIC_CONFIGURATION_DEFAULT_VALUE[2] = {0};
const uint8_t GenericAttributeService::SERVICE_CHANGED_DEFAULT_VALUE[4] = {0};

GenericAttributeService::GenericAttributeService(GattServerService &service, utility::Dispatcher &dispatcher)
    : serverService_(service), dispatcher_(dispatcher),
    serviceCallback_(std::make_shared<GattServerCallbackImpl>(*this))
{
    LoadNotifyInformation();
}

GenericAttributeService::~GenericAttributeService()
{
    StoreNotifyInformation();
}

class GenericAttributeService::GattServerCallbackImpl : public IGattServerCallback {
public:
    void OnConnectionStateChanged(const GattDevice &device, int ret, int state) override
    {
        if (state == static_cast<int>(BTConnectState::CONNECTED)) {
            service_.dispatcher_.PostTask(std::bind(&GenericAttributeService::ConnectIncoming, &service_, device));
        } else if (state == static_cast<int>(BTConnectState::DISCONNECTED)) {
            service_.dispatcher_.PostTask(std::bind(&GenericAttributeService::Disconnected, &service_, device));
        }
    }

    void OnServiceChanged(const Service &service) override
    {
        LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
        service_.dispatcher_.PostTask(
            std::bind(&GenericAttributeService::NotifyAllDevices, &service_, service.handle_, service.endHandle_));
    }

    void OnNotifyConfirm(const GattDevice &device, const Characteristic &characteristic, int result) override
    {
        if (characteristic.uuid_ == Uuid::ConvertFrom16Bits(UUID_SERVICE_CHANGED) &&
            result == GattStatus::GATT_SUCCESS) {
            service_.dispatcher_.PostTask(std::bind(&GenericAttributeService::ComfirmNotify, &service_, device));
        }
    }

    explicit GattServerCallbackImpl(GenericAttributeService &service) : service_(service)
    {}
    ~GattServerCallbackImpl()
    {}

private:
    GenericAttributeService &service_;
};

int GenericAttributeService::RegisterService()
{
    appId_ = serverService_.RegisterApplicationSync(serviceCallback_);
    if (appId_ < 0) {
        return appId_;
    }

    instance_ = BuildService();
    int result = serverService_.AddServiceSync(appId_, *instance_);
    if (result != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}d:%{public}s : Add GATTS Failed! ret: %{public}d", __FILE__, __LINE__, __FUNCTION__, result);
    }

    return result;
}

void GenericAttributeService::DeregisterService() const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    serverService_.DeregisterApplicationSync(appId_);
}

int GenericAttributeService::RegisterSDP()
{
    if (instance_ == nullptr) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    sdpRegister_.reset(ClassCreator<GattServiceOverBredr>::NewInstance("GattServiceOverBredr"));
    if (sdpRegister_ != nullptr) {
        static_cast<GattServiceOverBredrInterface *>(sdpRegister_.get())
            ->RegisterSDP(instance_->startHandle_, instance_->endHandle_);
    }

    LOG_DEBUG("[GenericAttributeService]::%{public}s", __FUNCTION__);

    return GattStatus::GATT_SUCCESS;
}

void GenericAttributeService::DeregisterSDP() const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    if (sdpRegister_ != nullptr) {
        static_cast<GattServiceOverBredrInterface *>(sdpRegister_.get())->DeregisterSDP();
    }
}

void GenericAttributeService::StoreNotifyInformation() const
{
    bool needsave = false;
    for (auto &dev : devices_) {
        if (!dev.second.isNotified && GattConnectionManager::GetInstance().GetEncryptionInfo(dev.first)) {
            AdapterDeviceConfig::GetInstance()->SetValue(SECTION_GENERIC_ATTRIBUTE_SERVICE,
                dev.first.addr_.GetAddress(),
                PROPERTY_GATTS_START_HANDLE,
                dev.second.startHandle_);
            AdapterDeviceConfig::GetInstance()->SetValue(SECTION_GENERIC_ATTRIBUTE_SERVICE,
                dev.first.addr_.GetAddress(),
                PROPERTY_GATTS_END_HANDLE,
                dev.second.endHandle_);
            AdapterDeviceConfig::GetInstance()->SetValue(SECTION_GENERIC_ATTRIBUTE_SERVICE,
                dev.first.addr_.GetAddress(),
                PROPERTY_GATT_TRANSPORT,
                dev.first.transport_);
            needsave = true;
        }
    }
    if (needsave) {
        AdapterDeviceConfig::GetInstance()->Save();
    }
}

void GenericAttributeService::LoadNotifyInformation()
{
    AdapterDeviceConfig::GetInstance()->Load();

    std::vector<std::string> devices;
    AdapterDeviceConfig::GetInstance()->GetSubSections(SECTION_GENERIC_ATTRIBUTE_SERVICE, devices);
    for (auto &dev : devices) {
        int startHandle = MIN_ATTRIBUTE_HANDLE;
        int endhandle = MAX_ATTRIBUTE_HANDLE;
        int transport = GATT_TRANSPORT_TYPE_LE;
        AdapterDeviceConfig::GetInstance()->GetValue(
            SECTION_GENERIC_ATTRIBUTE_SERVICE, dev, PROPERTY_GATTS_START_HANDLE, startHandle);
        AdapterDeviceConfig::GetInstance()->GetValue(
            SECTION_GENERIC_ATTRIBUTE_SERVICE, dev, PROPERTY_GATTS_END_HANDLE, endhandle);

        if (!AdapterDeviceConfig::GetInstance()->GetValue(
            SECTION_GENERIC_ATTRIBUTE_SERVICE, dev, PROPERTY_GATT_TRANSPORT, transport)) {
            LOG_WARN("%{public}s:%{public}d:%{public}s::Get device transport Failed!", __FILE__, __LINE__, __FUNCTION__);
        }

        devices_.emplace(std::make_pair<GattDevice, NotifyInformation>(
            GattDevice(RawAddress(dev), (uint8_t)transport), {
            .isNotified = false, .startHandle_ = (uint16_t)startHandle, .endHandle_ = (uint16_t)endhandle
            }));

        AdapterDeviceConfig::GetInstance()->RemoveSection(SECTION_GENERIC_ATTRIBUTE_SERVICE, dev);
        AdapterDeviceConfig::GetInstance()->Save();
    }
}

void GenericAttributeService::ComfirmNotify(GattDevice device)
{
    auto dev = devices_.find(device);
    if (dev != devices_.end()) {
        dev->second.isNotified = true;
    }
}

std::unique_ptr<Service> GenericAttributeService::BuildService()
{
    std::unique_ptr<Service> svc =
        std::make_unique<Service>(Uuid::ConvertFrom16Bits(UUID_GENERIC_ATTRIBUTE_SERVICE), 0, 0, 0);
    svc->isPrimary_ = true;

    Characteristic characteristic(Uuid::ConvertFrom16Bits(UUID_SERVICE_CHANGED),
        0,
        CHARACTERISTIC_PROPERTIE_INDICATE,
        0,
        SERVICE_CHANGED_DEFAULT_VALUE,
        SERVICE_CHANGED_VALUE_LENGTH);

    characteristic.descriptors_.push_back(Descriptor(Uuid::ConvertFrom16Bits(UUID_CLIENT_CHARACTERISTIC_CONFIGURATION),
        0,
        (int)GattPermission::READABLE | (int)GattPermission::WRITABLE,
        CLIENT_CHARACTERISTIC_CONFIGURATION_DEFAULT_VALUE,
        CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH));

    svc->characteristics_.push_back(characteristic);

    return svc;
}

void GenericAttributeService::NotifyAllDevices(uint16_t startHandle, uint16_t endHandle)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    Characteristic ccc(instance_->characteristics_[0].handle_);
    ccc.value_ = BuildValue(startHandle, endHandle);
    ccc.length_ = SERVICE_CHANGED_VALUE_LENGTH;

    for (auto& dev : devices_) {
        if (Compat::CompatCheck(CompatType::COMPAT_GATTC_NO_SERVICE_CHANGED_IND, dev.first.addr_.GetAddress())) {
            dev.second.isNotified = true;
            continue;
        }
        dev.second.isNotified = false;
        dev.second.startHandle_ = startHandle;
        dev.second.endHandle_ = endHandle;
        NotifyServiceChanged(dev.first, ccc);
    }
}

void GenericAttributeService::NotifyServiceChanged(const GattDevice& device, Characteristic& characteristic) const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    serverService_.NotifyClient(device, characteristic, true);
}

void GenericAttributeService::ConnectIncoming(const GattDevice& device)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto dev = devices_.emplace(device, NotifyInformation());
    if (!dev.second && !dev.first->second.isNotified) {
        if (GattConnectionManager::GetInstance().GetEncryptionInfo(device)) {
            Characteristic ccc(instance_->characteristics_[0].handle_);
            ccc.value_ = BuildValue(dev.first->second.startHandle_, dev.first->second.endHandle_);
            ccc.length_ = SERVICE_CHANGED_VALUE_LENGTH;

            NotifyServiceChanged(device, ccc);
        } else {
            dev.first->second.isNotified = true;
        }
    }
}

void GenericAttributeService::Disconnected(const GattDevice& device)
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    auto dev = devices_.find(device);
    if (dev != devices_.end() && dev->second.isNotified) {
        devices_.erase(device);
    }
}

std::unique_ptr<uint8_t[]> GenericAttributeService::BuildValue(uint16_t startHandle, uint16_t endHandle) const
{
    LOG_INFO("%{public}s:%{public}d:%{public}s", __FILE__, __LINE__, __FUNCTION__);
    std::unique_ptr<uint8_t[]> value = std::make_unique<uint8_t[]>(SERVICE_CHANGED_VALUE_LENGTH);

    static const size_t doubleByte = 2;
    if (memcpy_s(value.get(), SERVICE_CHANGED_VALUE_LENGTH, &startHandle, doubleByte) != EOK) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s::memcpy_s Failed!", __FILE__, __LINE__, __FUNCTION__);
        return NULL;
    }
    if (memcpy_s(value.get() + doubleByte, SERVICE_CHANGED_VALUE_LENGTH - doubleByte, &endHandle, doubleByte) != EOK) {
        LOG_ERROR("%{public}s:%{public}d:%{public}s::memcpy_s Failed!", __FILE__, __LINE__, __FUNCTION__);
        return NULL;
    }

    return value;
}
}  // namespace bluetooth
}  // namespace OHOS
