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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines basic profile for profile service.
 *
 * @since 6
 */

/**
 * @file interface_profile.h
 *
 * @brief basic profile interface.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_H
#define INTERFACE_PROFILE_H

#include <list>

#include "bt_def.h"
#include "raw_address.h"

/**
 * @brief forward declaration for class Context in namespace utility
 */
namespace utility {
class Context;
}

namespace OHOS {
namespace bluetooth {
/**
 * @brief profile service name Define
 */
const std::string PROFILE_NAME_GATT_CLIENT = "GattClientService";
const std::string PROFILE_NAME_GATT_SERVER = "GattServerService";
const std::string PROFILE_NAME_A2DP_SRC = "A2dpSrcService";
const std::string PROFILE_NAME_A2DP_SINK = "A2dpSnkService";
const std::string PROFILE_NAME_AVRCP_CT = "AvrcpCtService";
const std::string PROFILE_NAME_AVRCP_TG = "AvrcpTgService";
const std::string PROFILE_NAME_HFP_AG = "HfpAgService";
const std::string PROFILE_NAME_HFP_HF = "HfpHfService";
const std::string PROFILE_NAME_MAP_MCE = "MapMceService";
const std::string PROFILE_NAME_MAP_MSE = "MapMseService";
const std::string PROFILE_NAME_PAN = "PanService";
const std::string PROFILE_NAME_OPP = "OppService";
const std::string PROFILE_NAME_PBAP_PCE = "PbapPceService";
const std::string PROFILE_NAME_PBAP_PSE = "PbapPseService";
const std::string PROFILE_NAME_SPP = "SocketService";
const std::string PROFILE_NAME_DI = "DIService";
const std::string PROFILE_NAME_HID_HOST = "HidHostService";

/**
 * @brief profile connect state define, using to GetConnectState()...
 */
const uint8_t PROFILE_STATE_CONNECTED = 0x08;
const uint8_t PROFILE_STATE_CONNECTING = 0x04;
const uint8_t PROFILE_STATE_DISCONNECTING = 0x02;
const uint8_t PROFILE_STATE_DISCONNECTED = 0x01;

/**
 * @brief Represents basic profile for each profile service, including the common functions.
 *
 * @since 6
 */
class IProfile {
public:
    /**
     * @brief A destructor used to delete the <b>IProfile</b> instance.
     *
     * @since 6
     */
    virtual ~IProfile() = default;

    /**
     * @brief Connect with device.
     *
     * @param device Remote device address.
     * @return Returns Result for connect operation.
     * @since 6
     */
    virtual int Connect(const RawAddress &device) = 0;

    /**
     * @brief Disconnect with device.
     *
     * @param device Remote device address.
     * @return Returns Result for disconnect operation.
     * @since 6
     */
    virtual int Disconnect(const RawAddress &device) = 0;

    /**
     * @brief Get connected devices.
     *
     * @return Returns List for connected devices.
     * @since 6
     */
    virtual std::list<RawAddress> GetConnectDevices() = 0;

    /**
     * @brief Get connect state.
     *
     * @return Returns connect state for profile service.
     * @since 6
     */
    virtual int GetConnectState() = 0;

    /**
     * @brief Get max number profile can connect.
     *
     * @return Returns max number profile can connect.
     * @since 6
     */
    virtual int GetMaxConnectNum() = 0;

    /**
     * @brief Get utility::Context pointer for each profile service.
     *
     * @return Returns the pointer for adapter.
     * @since 6
     */
    virtual utility::Context *GetContext() = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_H