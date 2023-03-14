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

#ifndef SOCKET_SDP_CLIENT_H
#define SOCKET_SDP_CLIENT_H

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
class SocketSdpClient {
public:
    /**
     * @brief Constructor.
     */
    SocketSdpClient() = default;

    /**
     * @brief Destructor.
     */
    virtual ~SocketSdpClient();

    /**
     * @brief Finds a record of the SPP from the SDP.
     *
     * @param addr The address of the bluetooth device.
     * @param uuid The uuid is used to search server channel number.
     * @param context The context is used to send the event in the callback.
     * @return BT_NO_ERROR            : The function is executed successfully.
     */
    static int StartDiscovery(const std::string &addr, const Uuid &uuid, void *context);

    /**
     * @brief Callback of SDP discovery.
     *
     * @param addr The address of the bluetooth device.
     * @param serviceAry The serviceAry is Array of services discovered.
     * @param serviceNum The serviceNum is number of services discovered.
     * @param context The context is used to send the event in the callback.
     */
    static void SdpSearchCallback(const BtAddr *addr, const SdpService *serviceAry,
        uint16_t serviceNum, void *context);

private:
    BT_DISALLOW_COPY_AND_ASSIGN(SocketSdpClient);

    /**
     * @brief Search the attributes of serviceAry.
     *
     * @param addr The address of the bluetooth device.
     * @param serviceAry The serviceAry is Array of services discovered.
     * @param serviceNum The serviceNum is number of services discovered.
     * @return uint8_t server channel number.
     */
    static uint8_t SearchAttributes(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum);

    /**
     * @brief Search the server channel number.
     *
     * @param protocolNum Protocol descriptor number.
     * @param protocol Protocol descriptor.
     * @param scn server channel number.
     * @return true Function executed successfully.
     * @return false  Function executed failed.
     */
    static bool SearchScn(uint16_t protocolNum, const SdpProtocolDescriptor *protocol, uint8_t &scn);

    static constexpr uint16_t SPP_CLASSID_NUM = 1;
    static constexpr uint16_t SPP_ATTR_NUM = 3;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // SOCKET_SDP_CLIENT_H