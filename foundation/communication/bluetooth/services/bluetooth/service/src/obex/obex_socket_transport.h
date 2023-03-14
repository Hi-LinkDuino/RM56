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

#ifndef OBEX_SOCKET_TRANSPORT_H
#define OBEX_SOCKET_TRANSPORT_H

#include <atomic>
#include <cstdint>
#include <unordered_map>
#include "dispatcher.h"
#include "obex_packet.h"
#include "obex_transport.h"
#include "obex_types.h"
#include "transport/transport.h"

namespace OHOS {
namespace bluetooth {
class ObexSocketTransport : public ObexTransport {
public:
    ObexSocketTransport(
        DataTransport *dataTransport, uint16_t sendMtu, uint16_t recvMtu, const std::string &parentTranKey);
    virtual ~ObexSocketTransport();
    bool Write(Packet &pkt) override;
    // the maximum allowed OBEX packet that can be send over the transport
    int GetMaxSendPacketSize() override;
    // the maximum allowed OBEX packet that can be received over the transport
    int GetMaxReceivePacketSize() override;
    // This function is used to get the Bluetooth address of the peer of the connected channel
    virtual const RawAddress &GetRemoteAddress() override;
    // is transport Connected
    virtual bool IsConnected() override;
    // get transport key
    const std::string &GetTransportKey() override;

private:
    std::unique_ptr<DataTransport> dataTransport_ = nullptr;
    bool isConnected_ = false;
    std::string tranKey_ = "";
    uint16_t sendMtu_ = 0;
    uint16_t recvMtu_ = 0;
    RawAddress remoteBtAddr_ {""};
    BT_DISALLOW_COPY_AND_ASSIGN(ObexSocketTransport);
};

class ObexClientSocketTransport : public ObexClientTransport {
public:
    struct Option {
        BtAddr addr_ {};
        bool isGoepL2capPSM_ = false;
        uint16_t lpsm_ = 0;
        uint16_t scn_ = 0;
        uint16_t mtu_ = 0;
    };
    ObexClientSocketTransport(const Option &option, ObexTransportObserver &observer, utility::Dispatcher &dispatcher);
    virtual ~ObexClientSocketTransport();
    int Connect() override;
    int Disconnect() override;
    bool Write(Packet &pkt) override;
    // the maximum allowed OBEX packet that can be send over the transport
    int GetMaxSendPacketSize() override;
    // the maximum allowed OBEX packet that can be received over the transport
    int GetMaxReceivePacketSize() override;
    // This function is used to get the Bluetooth address of the peer of the connected channel
    const RawAddress &GetRemoteAddress() override;
    // is transport Connected
    bool IsConnected() override;
    // get transport key
    const std::string &GetTransportKey() override;

private:
    const RawAddress &PrivateGetRemoteAddress();
    static const uint8_t TRANSPORT_KEY_NUM_LEN = 4;
    class TransportObserver : public DataTransportObserver {
    public:
        explicit TransportObserver(ObexClientSocketTransport &obexTran);
        virtual ~TransportObserver() = default;
        // The event is triggered when server accept a new connection.
        void OnConnectIncoming(const RawAddress &addr, uint16_t port) override {};
        // The event is triggered when server accept a new connection.
        void OnIncomingDisconnected(const RawAddress &addr) override {};
        // The event is triggered when connection complete successfully.
        void OnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU) override;
        // The event is triggered when a disconnect request is received.
        void OnDisconnected(DataTransport *transport) override;
        // The event is triggered when the disconnection process is successful.
        void OnDisconnectSuccess(DataTransport *transport) override;
        // The event is triggered when data is received from stack.
        void OnDataAvailable(DataTransport *transport, Packet *pkt) override;
        void OnDataAvailable(DataTransport *transport) override;
        // The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
        void OnDataBusy(DataTransport *transport, uint8_t isBusy) override;
        // The event is triggered when process is failed.
        void OnTransportError(DataTransport *transport, int errType) override;

    private:
        ObexClientSocketTransport &obexTran_;
        // The event is triggered when connection complete successfully.
        void ProcessOnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU);
        // The event is triggered when a disconnect request is received.
        void ProcessOnDisconnected();
        // The event is triggered when the disconnection process is successful.
        void ProcessOnDisconnectSuccess();
        // The event is triggered when data is received from stack.
        void ProcessOnDataAvailable(DataTransport *transport, Packet *pkt);
        // The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
        void ProcessOnDataBusy(uint8_t isBusy);
        // The event is triggered when process is failed.
        void ProcessOnTransportError(int errType);
        BT_DISALLOW_COPY_AND_ASSIGN(TransportObserver);
    };
    std::string MakeTransportKey(bool isGoepL2capPSM, uint16_t scn);
    std::unique_ptr<TransportObserver> transportObserver_ = nullptr;
    std::unique_ptr<DataTransport> dataTransport_ = nullptr;
    bool isConnected_ = false;
    std::string tranKey_ = "";
    uint16_t sendMtu_ = 0;
    uint16_t recvMtu_ = 0;
    utility::Dispatcher &dispatcher_;
    RawAddress remoteBtAddr_ {""};
    BT_DISALLOW_COPY_AND_ASSIGN(ObexClientSocketTransport);
};
class ObexServerSocketTransport : public ObexServerTransport {
public:
    struct Option {
        bool isGoepL2capPSM_ = false;
        uint16_t scn_ = 0;
        uint16_t mtu_ = 0;
    };
    ObexServerSocketTransport(const Option &option, ObexTransportObserver &observer, utility::Dispatcher &dispatcher);
    ~ObexServerSocketTransport() override = default;
    int Listen() override;
    int Disconnect() override;
    int Disconnect(ObexTransport &subTransport) override;

    // Server accept the connection request
    int AcceptConnection(ObexIncomingConnect &incomingConnect) override;
    // Server reject the connection request
    int RejectConnection(ObexIncomingConnect &incomingConnect) override;
    // get transport key
    const std::string &GetTransportKey() override;

private:
    static const uint8_t TRANSPORT_KEY_NUM_LEN = 4;

    class TransportObserver : public DataTransportObserver {
    public:
        explicit TransportObserver(ObexServerSocketTransport &mainTran);
        virtual ~TransportObserver() = default;
        // The event is triggered when server accept a new connection.
        void OnConnectIncoming(const RawAddress &addr, uint16_t port) override;
        // The event is triggered when new connection disconnect before accept.
        void OnIncomingDisconnected(const RawAddress &addr) override;
        // The event is triggered when connection complete successfully.
        void OnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU) override;
        // The event is triggered when a disconnect request is received.
        void OnDisconnected(DataTransport *transport) override;
        // The event is triggered when the disconnection process is successful.
        void OnDisconnectSuccess(DataTransport *transport) override;
        // The event is triggered when data is received from stack.
        void OnDataAvailable(DataTransport *transport, Packet *pkt) override;
        void OnDataAvailable(DataTransport *transport) override;
        // The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
        void OnDataBusy(DataTransport *transport, uint8_t isBusy) override;
        // The event is triggered when process is failed.
        void OnTransportError(DataTransport *transport, int errType) override;

    private:
        ObexServerSocketTransport &mainTran_;
        // The event is triggered when server accept a new connection.
        void ProcessOnConnectIncoming(RawAddress btAddr, uint16_t port);
        // The event is triggered when new connection disconnect before accept.
        void ProcessOnIncomingDisconnected(RawAddress btAddr);
        // The event is triggered when connection complete successfully.
        void ProcessOnConnected(DataTransport *transport, uint16_t sendMTU, uint16_t recvMTU);
        // The event is triggered when a disconnect request is received.
        void ProcessOnDisconnected(DataTransport *transport);
        // The event is triggered when the disconnection process is successful.
        void ProcessOnDisconnectSuccess(DataTransport *transport);
        // The event is triggered when data is received from stack.
        void ProcessOnDataAvailable(DataTransport *transport, Packet *pkt);
        // The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
        void ProcessOnDataBusy(DataTransport *transport, uint8_t isBusy);
        // The event is triggered when process is failed.
        void ProcessOnTransportError(DataTransport *transport, int errType);
        BT_DISALLOW_COPY_AND_ASSIGN(TransportObserver);
    };
    static std::string MakeTransportKey(bool isGoepL2capPSM, uint16_t scn);
    std::unique_ptr<TransportObserver> transportObserver_ = nullptr;
    std::unique_ptr<DataTransport> dataTransport_ = nullptr;

    std::unordered_map<DataTransport *, std::unique_ptr<ObexSocketTransport>> subTranMap_ {};
    std::unordered_map<std::string, std::unique_ptr<ObexIncomingConnect>> incomingConnectMap_ {};
    std::string tranKey_ = "";
    std::atomic_bool isOnListening_ {false};
    utility::Dispatcher &dispatcher_;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerSocketTransport);
};
class ObexServerIncomingConnect : public ObexIncomingConnect {
public:
    ObexServerIncomingConnect(ObexServerSocketTransport &obexTransport, const RawAddress &btAddr, int port);
    virtual ~ObexServerIncomingConnect();
    //  accept the connection request
    int AcceptConnection() override;
    //  reject the connection request
    int RejectConnection() override;
    // This function is used to get the Bluetooth address of the peer of the connected channel
    const RawAddress &GetRemoteAddress() override;
    // Port
    int GetPort() override;

private:
    ObexServerSocketTransport &obexTransport_;
    RawAddress btAddr_ {""};
    uint16_t port_ = 0;
    bool operDone_ = false;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerIncomingConnect);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_SOCKET_TRANSPORT_H
