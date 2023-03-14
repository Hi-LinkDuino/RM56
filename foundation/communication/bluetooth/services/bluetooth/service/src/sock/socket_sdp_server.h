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

#ifndef SOCKET_SDP_SERVER_H
#define SOCKET_SDP_SERVER_H

#include <stdint.h>
#include <string>
#include "base_def.h"
#include "bt_uuid.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods used to interact with the SDP protocol.
 * @see SDP Service Records for Serial Port Profile. Table 6.1.
 */
class SocketSdpServer {
public:
    /**
     * @brief Constructor.
     */
    SocketSdpServer() = default;

    /**
     * @brief Destructor.
     */
    virtual ~SocketSdpServer() = default;

    /**
     * @brief Registers a record of the SPP into the SDP.
     *
     * @param name service name.
     * @param uuid server uuid.
     * @param scn server channel number.
     * @return int
     */
    int RegisterSdpService(const std::string &name, const Uuid &uuid, uint8_t scn);

    /**
     * @brief Unregisters a record of the SPP from the SDP.
     *
     * @return int
     */
    int UnregisterSdpService();

private:
    BT_DISALLOW_COPY_AND_ASSIGN(SocketSdpServer);

    /**
     * @brief Add service class id info.
     * @details Add service class id info to the previously created SDP service record.
     * @return int
     */
    int AddServiceClassId(const Uuid &uuid);

    /**
     * @brief Add protocol info.
     * @details Add protocol info to the previously created SDP service record.
     * @param scn Server channel number
     * @return int
     */
    int AddProtocol(uint8_t scn);

    /**
     * @brief Add profile info.
     * @details Add profile info to the previously created SDP service record.
     * @return int
     */
    int AddProfile();

    /**
     * @brief Add service name info.
     * @details Add service name info to the previously created SDP service record.
     * @return int
     */
    int AddServiceName(const std::string &name);

    /**
     * @brief Add browse group list info.
     * @details Add browse group list info to the previously created SDP service record.
     * @return int
     */
    int AddBrowseGroupList();

    // SDP service record handle
    uint32_t sdpHandle_ {0};
    inline static constexpr uint16_t SPP_CLASSID_NUM = 1;
    inline static constexpr uint16_t SPP_PRRTOCOL_NUM = 2;
    inline static constexpr uint16_t SPP_PROFILE_NUM = 1;
    inline static constexpr uint16_t SPP_BROWSE_LIST_NUM = 1;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // SOCKET_SDP_SERVER_H