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

#ifndef HFP_AG_DATA_CONNECTION_SERVER_H
#define HFP_AG_DATA_CONNECTION_SERVER_H

#include <cstdint>

#include "base_def.h"
#include "hfp_ag_rfcomm_connection_server.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for managining HFP AG data connection.
 */
class HfpAgDataConnectionServer {
public:
    /**
     * @brief Get the HfpAgDataConnectionServer instance.
     *
     * @return Returns the HfpAgDataConnectionServer instance.
     */
    static HfpAgDataConnectionServer &GetInstance();

    /**
     * @brief This function used to register rfcomm server.
     *
     * @return Returns the result of register rfcomm server.
     */
    int RegisterServer() const;

    /**
     * @brief This function used to deregister rfcomm server.
     *
     * @return Returns the result of remove rfcomm server.
     */
    int RemoveServer();

    /**
     * @brief This function called while rfcomm connection incoming.
     *
     * @param handle Rfcomm connection handle.
     * @return Returns the result of accept rfcomm connection.
     */
    int AcceptConnection(uint16_t handle) const;

    /**
     * @brief This function called while rfcomm connection incoming.
     *
     * @param handle Rfcomm connection handle.
     * @return Returns the result of reject rfcomm connection.
     */
    int RejectConnection(uint16_t handle) const;

    /**
     * @brief This function apply for server channel number.
     *
     * @return Returns the assigned server channel number.
     */
    uint8_t AssignLocalScn();

    /**
     * @brief This function for get server channel number.
     *
     * @return Returns server channel number.
     */
    uint8_t GetLocalScn() const;

private:
    /**
     * @brief Data connection server callback function.
     *
     * @param handle Handle.
     * @param eventId Event id.
     * @param eventData Event data.
     * @param context Context.
     */
    static void DataConnectionServerCallback(uint16_t handle, uint32_t eventId,
                                             const void *eventData, void *context);

    /**
     * @brief Process Data connection server callback function in hfp thread.
     *
     * @param handle Handle.
     * @param eventId Event id.
     * @param inComingAddr Incoming connect request bluetooth device address.
     */
    static void ProcessDataConnectionServerCallback(uint16_t handle, uint32_t eventId, const std::string &inComingAddr);

    /**
     * @brief Construct a new HfpHfDataConnectionServer object.
     */
    HfpAgDataConnectionServer() = default;

    /**
     * @brief Destroy the HfpHfDataConnectionServer object.
     */
    ~HfpAgDataConnectionServer() = default;

    HfpAgRfcommConnectionServer rfcommServer_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgDataConnectionServer);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_DATA_CONNECTION_SERVER_H