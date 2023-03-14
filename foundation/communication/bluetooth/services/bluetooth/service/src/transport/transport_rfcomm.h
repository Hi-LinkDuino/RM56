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

#ifndef TRANSPORT_RFCOMM_H
#define TRANSPORT_RFCOMM_H

#include <map>
#include <mutex>
#include <stdint.h>
#include "dispatcher.h"
#include "packet.h"
#include "raw_address.h"
#include "rfcomm.h"
#include "transport.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This RfcommTransport class provides a set of methods that is interactive with RFCOMM.
 */
class RfcommTransport : public DataTransport {
public:
    /**
     * @brief Constructor.
     */
    RfcommTransport(const RawAddress *addr, uint8_t scn, uint16_t mtu, DataTransportObserver &observer,
        utility::Dispatcher &dispatcher);

    /**
     * @brief Destructor.
     */
    virtual ~RfcommTransport();

    /**
     * @brief The client initiates the connection.
     *
     * @return int
     */
    int Connect() override;

    /**
     * @brief The client initiates the disconnection request.
     *
     * @return int
     */
    int Disconnect() override;

    /**
     * @brief The server register to RFCOMM.
     *
     * @return int
     */
    int RegisterServer() override;

    /**
     * @brief Close server.
     *
     * @return int
     */
    int RemoveServer(bool isDisable = true) override;

    /**
     * @brief The server accept the connection.
     *
     * @param addr remote device address.
     * @param scn the server channel.
     * @return int
     */
    int AcceptConnection(const RawAddress &addr, uint16_t scn) override;

    /**
     * @brief The server reject the connection.
     *
     * @param addr remote device address.
     * @param scn the server channel.
     * @return int
     */
    int RejectConnection(const RawAddress &addr, uint16_t scn) override;

    /**
     * @brief Receive data from RFCOMM.
     * @param pkt
     * @return int
     */
    int Read(Packet **pkt) override;

    /**
     * @brief Send data to RFCOMM.
     *
     * @param pkt
     * @return int
     */
    int Write(Packet *pkt) override;

    /**
     * @brief Get the remote device address.
     *
     * @return BtAddr
     */
    RawAddress GetRemoteAddress() override;

private:
    // remote device address.
    RawAddress remoteAddr_ {};
    // server channel number.
    uint8_t scn_ {0};
    // mtu
    uint16_t mtu_ {0};
    // is server or not.
    bool isServer_ {false};

    // RFCOMM connection handle.
    uint16_t rfcHandle_ {0};
    // std::mutex mutex_;
    std::mutex transportMutex_ {};
    // The map manages the correspondence between new transport and rfcomm handle.
    std::map<uint16_t, RfcommTransport *> transportMap_ {};
    // The map manages the correspondence between new transport and rfcomm handle.
    std::map<RfcommTransport *, RawAddress> remoteAddrMap_ {};
    // The map manages the correspondence between remote address and rfcomm handle.
    std::map<uint16_t, RawAddress> addressMap_ {};
    // The map manages the correspondence between remote address and rfcomm handle.
    std::map<RawAddress, uint16_t> handleMap_ {};
    // the pointer of the DataTransportObserver
    DataTransportObserver &observer_;
    utility::Dispatcher &dispatcher_;

    /**
     * @brief is server or not.
     *
     * @return true the role is server.
     * @return false the role is client
     */
    bool IsServer() const
    {
        return isServer_;
    }

    /**
     * @brief When server accept a connection request, generate a new transport.
     *
     * @param addr remote device address.
     * @param handle rfcomm connection handle.
     * @return RfcommTransport*
     */
    RfcommTransport *AddTransportInternal(RawAddress addr, uint16_t handle);

    /**
     * @brief Rfcomm event
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcEventCallback(uint16_t handle, uint32_t event, const void *eventData, void *context);

    /**
     * @brief Rfcomm event of connect success.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcConnectSuccess(uint16_t handle, const void *eventData, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of connect fail.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcConnectFail(uint16_t handle, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of disconnected.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcDisconnected(uint16_t handle, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of disconnect success.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcDisconnectSuccess(uint16_t handle, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of disconnect fail.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcDisconnectFail(uint16_t handle, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of flow control on.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcFcOn(uint16_t handle, RfcommTransport *transport);

    /**
     * @brief Rfcomm event of recv data.
     *
     * @param cid Rfcomm connection handle.
     * @param event Rfcomm event.
     * @param event_data Rfcomm event data.
     * @param context transport object.
     */
    static void TransportRfcRecvData(uint16_t handle, RfcommTransport *transport);

    BT_DISALLOW_COPY_AND_ASSIGN(RfcommTransport);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // TRANSPORT_RFCOMM_H