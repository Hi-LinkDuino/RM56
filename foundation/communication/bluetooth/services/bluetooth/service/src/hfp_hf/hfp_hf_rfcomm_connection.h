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

#ifndef HFP_HF_RFCOMM_CONNECTION_H
#define HFP_HF_RFCOMM_CONNECTION_H

#include <cstdint>
#include <map>
#include <mutex>
#include <string>

#include "base_def.h"
#include "btstack.h"
#include "hfp_hf_defines.h"
#include "hfp_hf_gap_client.h"
#include "packet.h"
#include "raw_address.h"
#include "rfcomm.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for rfcomm connection.
 */
class HfpHfRfcommConnection {
public:
    /**
     * @brief Init rfcomm connection.
     */
    static int Init();

    /**
     * @brief Clean up rfcomm connection.
     */
    static void CleanUp();

    /**
     * @brief Add device to device map.
     *
     * @param handle Device handle.
     * @param addr Device address.
     */
    static void AddConnectionDevice(uint16_t handle, std::string addr);

    /**
     * @brief Remove device from device map.
     *
     * @param handle Device handle.
     */
    static void RemoveConnectionDevice(uint16_t handle);

    /**
     * @brief Get the Remote Address By Handle object.
     *
     * @param handle Rfcomm handle.
     * @return Returns the remote device address.
     */
    static std::string GetRemoteAddressByHandle(uint16_t handle);

    /**
     * @brief Construct a new HfpHfRfcommConnection object.
     *
     * @param fn Rfcomm callback.
     */
    explicit HfpHfRfcommConnection(RFCOMM_EventCallback fn) : fn_(fn)
    {}

    /**
     * @brief Destroy the HfpHfRfcommConnection object.
     */
    ~HfpHfRfcommConnection() = default;

    /**
     * @brief This function used to connect peer device rfcomm channel after SDP discovery OK (Initiator).
     *
     * @return Returns the result of connect.
     */
    int Connect();

    /**
     * @brief This function used to close rfcomm connection.
     *
     * @return Returns the result of disconnect.
     */
    int Disconnect() const;

    /**
     * @brief This function used to write data to peer device.
     *
     * @param pkt The address of data packet pointer.
     * @return Returns the result of read data.
     */
    int ReadData(Packet **pkt) const;

    /**
     * @brief This function used to write data to peer device.
     *
     * @param pkt Data packet pointer.
     * @return Returns the result of write data.
     */
    int WriteData(Packet &pkt) const;

    /**
     * @brief This function set rfcomm connection handle.
     *
     * @param handle Rfcomm handle.
     */
    void SetConnectionHandle(uint16_t handle);

    /**
     * @brief This function get connection by handle.
     *
     * @return Returns rfcomm connection handle.
     */
    uint16_t GetConnectionHandle() const;

    /**
     * @brief This function set remote address.
     *
     * @param addr Remote device address.
     */
    void SetRemoteAddr(const std::string &addr);

    /**
     * @brief This function set remote server channel number.
     *
     * @param scn Remote server channel number
     */
    void SetRemoteScn(uint8_t scn);

    /**
     * @brief This function get remote server channel number.
     *
     * @return Remote server channel number
     */
    uint8_t GetRemoteScn() const;

private:
    /**
     * @brief This function create channel to connect rfcomm.
     *
     * @return Returns the result of create channel.
     */
    int Create();

    BtAddr remoteAddr_ {};
    uint8_t remoteScn_ {0};
    uint16_t connHandle_ {0};
    RFCOMM_EventCallback fn_;
    std::unique_ptr<HfpHfGapClient> gap_ {std::make_unique<HfpHfGapClient>()};
    static std::map<uint16_t, std::string> g_devMap;
    inline static constexpr uint32_t HFP_RFCOMM_CONNECTION_EVENTS =
        RFCOMM_CHANNEL_EV_CONNECT_SUCCESS | RFCOMM_CHANNEL_EV_CONNECT_FAIL | RFCOMM_CHANNEL_EV_DISCONNECTED |
        RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS | RFCOMM_CHANNEL_EV_DISCONNECT_FAIL | RFCOMM_CHANNEL_EV_REV_DATA;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfRfcommConnection);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_RFCOMM_CONNECTION_H