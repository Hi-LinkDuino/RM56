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

#ifndef DEVICE_INFO_SERVICE_H
#define DEVICE_INFO_SERVICE_H

#include <cstdint>
#include <memory>
#include <string>
#include "../gatt_server_service.h"
#include "di_service_over_bredr.h"
#include "gatt_data.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
class DeviceInformationService {
public:
    /**
     * @brief Device Information Service.
     *
     */
    explicit DeviceInformationService(GattServerService &service);

    /**
     * @brief Destructor.
     */
    ~DeviceInformationService();

    int RegisterService();
    void DeregisterService() const;

    int RegisterSDP();
    void DeregisterSDP() const;

    BT_DISALLOW_COPY_AND_ASSIGN(DeviceInformationService);

private:
    static const uint16_t GATT_UUID_MANUFACTURER_NAME_STRING_CHAR;
    static const uint16_t GATT_UUID_MODEL_NUMBER_STRING_CHAR;
    static const uint16_t GATT_UUID_PNP_ID_CHAR;
    static const uint16_t GATT_UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST_CHAR;
    static const uint16_t GATT_UUID_SERIAL_NUMBER_STRING_CHAR;
    static const uint16_t GATT_UUID_SOFTWARE_REVISION_STRING_CHAR;
    static const uint16_t GATT_UUID_SYSTEM_ID_CHAR;
    static const uint16_t GATT_UUID_HARDWARE_REVISION_STRING_CHAR;
    static const uint16_t GATT_UUID_FIRMWARE_REVISION_STRING_CHAR;
    static const std::string DEFAULT_MANUFACTURER_NAME;
    static const std::string DEFAULT_MODEL_NUMBER;
    static const std::string DEFAULT_SERIAL_NUMBER;
    static const std::string DEFAULT_HARDWARE_REVISION;
    static const std::string DEFAULT_FIRMWARE_REVISION;
    static const std::string DEFAULT_SOFTWARE_REVISION;
    static const std::string DEFAULT_SYSTEM_ID;
    static const std::string DEFAULT_IEEE_REGULATORY_CERTIFICATION_DATA_LIST;
    static const std::string DEFAULT_PNP_ID;

    class GattServerCallbackImpl;

    std::unique_ptr<Service> instance_ = {nullptr};
    std::unique_ptr<DiServiceOverBredrInterface> sdpRegister_ = {nullptr};
    GattServerService &serverService_;
    int appId_ = -1;
    std::shared_ptr<GattServerCallbackImpl> serviceCallback_;

    std::unique_ptr<Service> BuildService();
    static std::string GetSystemId();
    static std::string GetPnpId();
    static std::string GetIEEEInfo();
    static std::string GetManufactureName();
    static std::string GetModelNumber();
    static std::string GetSerialNumber();
    static std::string GetHardwareRevision();
    static std::string GetFirmwareRevision();
    static std::string GetSoftwareRevision();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // DEVICE_INFO_SERVICE_H