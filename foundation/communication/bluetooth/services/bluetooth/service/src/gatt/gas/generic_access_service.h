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

#ifndef GENERIC_ACCESS_SERVICE_H
#define GENERIC_ACCESS_SERVICE_H

#include <cstdint>
#include <memory>
#include <string>
#include "../gatt_server_service.h"
#include "base_def.h"
#include "dispatcher.h"
#include "gatt_data.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
class GenericAccessService {
public:
    /**
     * @brief Destructor.
     */
    ~GenericAccessService();

    /**
     * @brief Device GenericAccess Service.
     *
     */
    GenericAccessService(GattServerService &service, utility::Dispatcher &dispatcher);

    int RegisterService();
    void DeregisterService();

    BT_DISALLOW_COPY_AND_ASSIGN(GenericAccessService);

private:
    static const uint16_t GATT_UUID_GAP_SERVICE;
    static const uint16_t GATT_UUID_GAP_DEVICE_NAME;
    static const uint16_t GATT_UUID_GAP_APPEARANCE;
    static const uint16_t GATT_UUID_GAP_CENTRAL_ADDR_RESOLUTION;
    static const std::string DEFAULT_DEVICE_NAME;
    static const uint16_t DEFAULT_APPEARANCE;
    static const size_t APPEARANCE_DATE_LENGTH;
    static const size_t CENTRAL_ADDR_RESOLUTION_DATE_LENGTH;

    class GattServerCallbackImpl;
    class AdapterBleObserverImpl;
    class AdapterClassicObserverImpl;

    bool bleDiscoverable_ = {false};
    bool classicDiscoverable_ = {false};
    uint8_t central_addr_resolution_ = {0};
    std::unique_ptr<Service> instance_ = {nullptr};
    std::shared_ptr<GattServerCallbackImpl> serviceCallback_;
    std::unique_ptr<AdapterBleObserverImpl> adapterBleObserver_;
    std::unique_ptr<AdapterClassicObserverImpl> adapterClassicObserver_;
    GattServerService &serverService_;
    utility::Dispatcher &dispatcher_;
    int appId_ = -1;

    std::unique_ptr<Service> BuildService();
    static std::string GetDeviceName();
    std::unique_ptr<uint8_t[]> GetAppearance() const;
    void OnDeviceNameChanged(std::string &deviceName) const;
    void OnAdvertisingStateChanged(int state);
    void OnScanModeChanged(int mode);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GENERIC_ACCESS_SERVICE_H