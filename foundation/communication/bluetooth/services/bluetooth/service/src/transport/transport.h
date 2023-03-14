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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines a bluetooth system that provides basic blurtooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 */

/**
 * @file transport.h
 *
 * @brief Declares TRANSPORT service functions, including basic and observer functions.
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdint.h>
#include "packet.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class DataTransport;

/**
 * @brief This DataTransportObserver class provides a set of methods that from
 *        RFCOMM or L2CAP's callback.
 */
class DataTransportObserver {
public:
    /**
     * @brief The event is triggered when server accept a new connection.
     *
     * @param addr remote device address.
     * @param port rfcomm scn or l2cap psm.
     */
    virtual void OnConnectIncoming(const RawAddress &addr, uint16_t port) = 0;

    /**
     * @brief The event is triggered when server accept a new connection timeout.
     *
     * @param addr remote device address.
     */
    virtual void OnIncomingDisconnected(const RawAddress &addr) = 0;

    /**
     * @brief The event is triggered when connection complete successfully.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU) = 0;

    /**
     * @brief The event is triggered when the connection is disconnected.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnDisconnected(DataTransport *transport) = 0;

    /**
     * @brief The event is triggered when the disconnection process is successful.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnDisconnectSuccess(DataTransport *transport) = 0;

    /**
     * @brief The event is triggered when data is received from stack.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnDataAvailable(DataTransport *transport) = 0;

    /**
     * @brief The event is triggered when data is received from stack.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnDataAvailable(DataTransport *transport, Packet *pkt) = 0;

    /**
     * @brief The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
     *
     * @param transport the pointer of the transport.
     */
    virtual void OnDataBusy(DataTransport *transport, uint8_t isBusy) {};

    /**
     * @brief The event is triggered when process is failed.
     *
     * @param transport the pointer of the transport.
     * @param errType connection failed, disconnection failed.
     */
    virtual void OnTransportError(DataTransport *transport, int errType) = 0;

    /**
     * @brief Destructor.
     */
    virtual ~DataTransportObserver()
    {}
};

/**
 * @brief This DataTransport class provides a set of methods that
 *        the upper layer to interact with rfcomm or L2CAP
 */
class DataTransport {
public:
    /**
     * @brief Client connect rfcomm channel.
     *
     * @return int
     */
    virtual int Connect() = 0;

    /**
     * @brief Client disconnect rfcomm connection.
     *
     * @return int
     */
    virtual int Disconnect() = 0;

    /**
     * @brief Server register to RFCOMM and wait for the client to connect.
     *
     * @return int
     */
    virtual int RegisterServer() = 0;

    /**
     * @brief Server close and disconnect all channels connected to the server.
     *
     * @return int
     */
    virtual int RemoveServer(bool isDisable) = 0;

    /**
     * @brief Server accept the connection request.
     *
     * @param addr remote device address.
     * @param port rfcomm's scn or l2cap's psm.
     * @return int
     */
    virtual int AcceptConnection(const RawAddress &addr, uint16_t port) = 0;

    /**
     * @brief Server reject the connection request
     *
     * @param addr remote device address.
     * @param port rfcomm's scn or l2cap's psm.
     * @return int
     */
    virtual int RejectConnection(const RawAddress &addr, uint16_t port) = 0;

    /**
     * @brief This function is used to get the data sent by the peer from RFCOMM/L2cap.
     *
     * @param pkt receive data from RFCOMM/L2cap.
     * @return int
     */
    virtual int Read(Packet **pkt) = 0;

    /**
     * @brief This function is used to write the data to RFCOMM/L2cap.
     *
     * @param pkt send data to RFCOMM/L2cap.
     * @return int
     */
    virtual int Write(Packet *pkt) = 0;

    /**
     * @brief This function is used to get the Bluetooth address of the peer of the connected channel.
     *
     * @return RawAddress
     */
    virtual RawAddress GetRemoteAddress() = 0;

    /**
     * @brief Destructor.
     */
    virtual ~DataTransport()
    {}
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // TRANSPORT_H
