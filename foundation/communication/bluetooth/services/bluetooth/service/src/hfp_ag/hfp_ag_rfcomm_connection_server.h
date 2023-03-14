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

#ifndef HFP_AG_RFCOMM_CONNECTION_SERVER_H
#define HFP_AG_RFCOMM_CONNECTION_SERVER_H

#include <cstdint>

#include "base_def.h"
#include "rfcomm.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for rfcomm connection server.
 */
class HfpAgRfcommConnectionServer {
public:
    /**
     * @brief Construct a new HfpAgRfcommConnectionServer object.
     */
    HfpAgRfcommConnectionServer() = default;

    /**
     * @brief Destroy the HfpAgRfcommConnectionServer object.
     */
    ~HfpAgRfcommConnectionServer() = default;

    /**
     * @brief This function used to register rfcomm server.
     *
     * @param fn The pointer to rfcomm callback function.
     * @return Returns the result of register rfcomm server.
     */
    int RegisterServer(RFCOMM_EventCallback fn) const;

    /**
     * @brief This function used to deregister rfcomm server.
     *
     * @return Returns the result of remove rfcomm server.
     */
    int RemoveServer();

    /**
     * @brief This function called while rfcomm connection incoming.
     *
     * @param handle Rfcomm handle.
     * @return Returns the result of accept rfcomm connection.
     */
    static int AcceptConnection(uint16_t handle);

    /**
     * @brief This function called while rfcomm connection incoming.
     *
     * @param handle Rfcomm handle.
     * @return Returns the result of reject rfcomm connection.
     */
    static int RejectConnection(uint16_t handle);

    /**
     * @brief This function apply for server channel number.
     *
     * @return Returns assigned server channel number.
     */
    uint8_t AssignLocalScn();

    /**
     * @brief This function for get server channel number.
     *
     * @return Returns server channel number.
     */
    uint8_t GetLocalScn() const;

private:
    uint8_t localScn_ {0};
    inline static constexpr uint32_t HFP_RFCOMM_CONNECTION_SERVER_EVENTS =
        RFCOMM_CHANNEL_EV_CONNECT_INCOMING | RFCOMM_CHANNEL_EV_CONNECT_SUCCESS | RFCOMM_CHANNEL_EV_CONNECT_FAIL |
        RFCOMM_CHANNEL_EV_DISCONNECTED | RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS | RFCOMM_CHANNEL_EV_DISCONNECT_FAIL |
        RFCOMM_CHANNEL_EV_REV_DATA;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgRfcommConnectionServer);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_RFCOMM_CONNECTION_SERVER_H