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

#ifndef OBEX_TRANSPORT_H
#define OBEX_TRANSPORT_H

#include <memory>
#include "btstack.h"
#include "obex_packet.h"
#include "obex_types.h"
#include "packet.h"
#include "raw_address.h"
#include "transport/transport_def.h"

namespace OHOS {
namespace bluetooth {
class ObexTransport;
class ObexIncomingConnect;
class ObexTransportObserver {
public:
    virtual ~ObexTransportObserver(){};
    virtual void OnTransportConnectIncoming(ObexIncomingConnect &incomingConnect);
    virtual void OnTransportIncomingDisconnected(const std::string &btAddr);
    virtual void OnTransportConnected(ObexTransport &transport) = 0;
    virtual void OnTransportDisconnected(ObexTransport &transport) = 0;
    virtual void OnTransportDataBusy(ObexTransport &transport, uint8_t isBusy) = 0;
    virtual void OnTransportDataAvailable(ObexTransport &transport, ObexPacket &obexPacket) = 0;
    virtual void OnTransportError(ObexTransport &transport, int errCd) = 0;
};
class ObexTransport {
public:
    virtual ~ObexTransport(){};
    virtual bool Write(Packet &pkt) = 0;
    // the maximum allowed OBEX packet that can be send over the transport
    virtual int GetMaxSendPacketSize() = 0;
    // the maximum allowed OBEX packet that can be received over the transport
    virtual int GetMaxReceivePacketSize() = 0;
    // This function is used to get the Bluetooth address of the peer of the connected channel
    virtual const RawAddress &GetRemoteAddress() = 0;
    // is transport Connected
    virtual bool IsConnected() = 0;
    // get transport key
    virtual const std::string &GetTransportKey() = 0;
};

// ObexClientTransport
class ObexClientTransport : public ObexTransport {
public:
    ObexClientTransport(ObexTransportObserver &observer) : observer_(observer)
    {}
    ~ObexClientTransport() override{};
    virtual int Connect() = 0;
    virtual int Disconnect() = 0;

protected:
    ObexTransportObserver &observer_;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexClientTransport);
};
// ObexIncomingConnect
class ObexIncomingConnect {
public:
    virtual ~ObexIncomingConnect(){};
    //  accept the connection request
    virtual int AcceptConnection() = 0;
    //  reject the connection request
    virtual int RejectConnection() = 0;
    // This function is used to get the Bluetooth address of the peer of the connected channel
    virtual const RawAddress &GetRemoteAddress() = 0;
    // Port
    virtual int GetPort() = 0;
};
// ObexServerTransport
class ObexServerTransport {
public:
    ObexServerTransport(ObexTransportObserver &observer);
    virtual ~ObexServerTransport() = default;
    virtual int Listen() = 0;
    virtual int Disconnect() = 0;
    virtual int Disconnect(ObexTransport &subTransport) = 0;
    // Server accept the connection request
    virtual int AcceptConnection(ObexIncomingConnect &incomingConnect) = 0;
    // Server reject the connection request
    virtual int RejectConnection(ObexIncomingConnect &incomingConnect) = 0;
    // get transport key
    virtual const std::string &GetTransportKey() = 0;

protected:
    ObexTransportObserver &observer_;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerTransport);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_TRANSPORT_H
