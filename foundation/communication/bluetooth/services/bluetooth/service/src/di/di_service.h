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

#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include "context.h"
#include "di_config.h"
#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
constexpr uint16_t UUID_SERVICE_CLASS_PNP_INFORMATION = 0x1200;
constexpr uint16_t ATTR_ID_SPECIFICATION_ID = 0x0200;
constexpr uint16_t ATTR_ID_VENDOR_ID = 0x0201;
constexpr uint16_t ATTR_ID_PRODUCT_ID = 0x0202;
constexpr uint16_t ATTR_ID_VERSION = 0x0203;
constexpr uint16_t ATTR_ID_PRIMARY_RECORD = 0x0204;
constexpr uint16_t ATTR_ID_VENDOR_ID_SOURCE = 0x0205;
constexpr uint16_t CLASS_ID_NUMBER = 0x0001;

constexpr uint16_t DEFAULT_SPECIFICATION_ID = 0x0103;
constexpr uint16_t DEFAULT_VENDOR_ID = 0x23A1;
constexpr uint16_t DEFAULT_PRODUCT_ID = 0x3020;
constexpr uint16_t DEFAULT_VERSION = 0x1234;
constexpr bool DEFAULT_PRIMARY_RECORD = false;
constexpr uint16_t DEFAULT_VENDOR_ID_SOURCE = 0x0001;

class DIService : public IProfile, public utility::Context {
public:
    DIService();
    ~DIService();

    void Enable() override;
    void Disable() override;
    utility::Context *GetContext() override;

    int Connect(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;
    std::list<RawAddress> GetConnectDevices() override;
    int GetConnectState() override;
    int GetMaxConnectNum() override;

    /**
     * @brief DIService start up. Load config file and register DI .
     *
     * @since 1.0
     * @version 1.0
     */
    void StartUp();

    /**
     * @brief DIService shut down. Save config file and disable stack.
     *
     * @since 1.0
     * @version 1.0
     */
    void ShutDown();

private:
    bool RegisterDIService();
    bool DeregisterDIService() const;
    void LoadDeviceInfo();

    DIConfig &config_;
    bool priRecord_ {DEFAULT_PRIMARY_RECORD};
    uint16_t specId_ {DEFAULT_SPECIFICATION_ID};
    uint16_t vendorId_ {DEFAULT_VENDOR_ID};
    uint16_t procId_ {DEFAULT_PRODUCT_ID};
    uint16_t version_ {DEFAULT_VERSION};
    uint16_t venIdSrc_ {DEFAULT_VENDOR_ID_SOURCE};
    uint32_t handle_ {};
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // DEVICE_INFO_H