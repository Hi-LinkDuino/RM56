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

#ifndef OBEX_SERVER_H
#define OBEX_SERVER_H

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>
#include "dispatcher.h"
#include "obex_headers.h"
#include "obex_session.h"
#include "obex_socket_transport.h"
#include "obex_types.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief ObexServerConfig
 */
struct ObexServerConfig {
    bool useRfcomm_ = false;                 // If using Rfcomm, set to true
    uint8_t rfcommScn_ = 0;                  // The server's number
    uint16_t rfcommMtu_ = 0;                 // The maximum size of data received at a time with rfcomm
    bool useL2cap_ = false;                  // If using L2Cap, set to true
    uint16_t l2capPsm_ = 0;                  // l2cap psm
    uint16_t l2capMtu_ = 0;                  // The maximum size of data received at a time with l2cap
    bool isSupportSrm_ = false;              // Is Support Single Request Mode
    bool isSupportReliableSession_ = false;  // Is Use Reliable Session
};
class ObexServerSession;
class ObexIncomingConnect;
class ObexServerObserver {
public:
    virtual ~ObexServerObserver() = default;
    virtual void OnTransportConnect(ObexIncomingConnect &incomingConnect);
    virtual void OnTransportConnected(const std::string &btAddr);
    virtual void OnTransportDisconnected(const std::string &btAddr);
    virtual void OnTransportError(const std::string &btAddr, int errCd, const std::string &msg);

    virtual void OnError(const int errCd, const std::string &msg);
    virtual void OnConnect(ObexServerSession &session, const ObexHeader &req) = 0;
    virtual void OnDisconnect(ObexServerSession &session, const ObexHeader &req) = 0;
    virtual void OnPut(ObexServerSession &session, const ObexHeader &req);
    virtual void OnGet(ObexServerSession &session, const ObexHeader &req);
    virtual void OnAbort(ObexServerSession &session, const ObexHeader &req);
    virtual void OnSetPath(ObexServerSession &session, const ObexHeader &req);
    virtual void OnAction(ObexServerSession &session, const ObexHeader &req);
    virtual void OnSession(ObexServerSession &session, const ObexHeader &req);

    virtual void OnBusy(ObexServerSession &session, bool isBusy) const;
};
class ObexPrivateServer {
public:
    /**
     * @brief ObexPrivateServerConfig
     */
    struct ObexPrivateServerConfig {
        uint16_t scn_ = 0;                       // The server's channel number
        uint16_t mtu_ = 0;                       // The maximum size of data received at a time with rfcomm
        bool isGoepL2capPSM_ = false;            // l2cap : true , rfcomm : false
        bool isSupportSrm_ = false;              // Is Support Single Request Mode
        bool isSupportReliableSession_ = false;  // Is Use Reliable Session
    };
    explicit ObexPrivateServer(
        const ObexPrivateServerConfig &config, ObexServerObserver &observer, utility::Dispatcher &dispatcher);
    virtual ~ObexPrivateServer() = default;
    int Startup() const;
    void Shutdown() const;
    int RemoveSession(ObexServerSession &session) const;

protected:
    class ObexServerTransportObserver : public ObexTransportObserver {
    public:
        explicit ObexServerTransportObserver(ObexPrivateServer &obexServer);
        ~ObexServerTransportObserver() override = default;
        // Transport callback
        void OnTransportConnectIncoming(ObexIncomingConnect &incomingConnect) override;
        void OnTransportIncomingDisconnected(const std::string &btAddr) override;
        void OnTransportConnected(ObexTransport &transport) override;
        void OnTransportDisconnected(ObexTransport &transport) override;
        void OnTransportDataAvailable(ObexTransport &transport, ObexPacket &obexPacket) override;
        void OnTransportDataBusy(ObexTransport &transport, uint8_t isBusy) override;
        void OnTransportError(ObexTransport &transport, int errCd) override;

    private:
        void HandleDataAvailableConnect(ObexServerSession &serverSession, const ObexHeader &req);
        ObexPrivateServer &obexServer_;
        std::unique_ptr<bluetooth::ObexHeader> GetObexHeaderFromPacket(ObexPacket &obexPacket) const;
        BT_DISALLOW_COPY_AND_ASSIGN(ObexServerTransportObserver);
    };
    virtual void HandleTransportDataBusy(ObexServerSession &serverSession, uint8_t isBusy);
    virtual void HandlePutRequest(ObexServerSession &session, ObexHeader &req) const;
    virtual void HandleGetRequest(ObexServerSession &session, ObexHeader &req);
    virtual void HandleSetPathRequest(ObexServerSession &session, ObexHeader &req);
    virtual void HandleAbortRequest(ObexServerSession &session, ObexHeader &req);
    virtual void SetBusy(ObexServerSession &session, bool isBusy) const;
    void RemoveSessionByTransport(ObexTransport &transport);

    ObexPrivateServer();
    uint16_t initMtu_ = 0;
    bool isSupportSrm_ = false;              // Is Support Single Request Mode
    bool isSupportReliableSession_ = false;  // Is Use Reliable Session
    ObexServerObserver &observer_;
    std::unique_ptr<ObexServerTransportObserver> transportObserver_ = nullptr;
    std::unique_ptr<ObexServerTransport> serverTransport_ = nullptr;

    std::unordered_map<ObexTransport *, std::unique_ptr<ObexServerSession>> serverSessionsMap_ {};
    std::list<std::unique_ptr<ObexServerSession>> invalidSessions_ {};
    utility::Dispatcher &dispatcher_;
    static const uint16_t MAX_TRASH_SESSION_COUNT;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexPrivateServer);
};
class ObexServer {
public:
    explicit ObexServer(const std::string &serviceName, const ObexServerConfig &config, ObexServerObserver &observer,
        utility::Dispatcher &dispatcher);
    virtual ~ObexServer() = default;
    int Startup() const;
    void Shutdown() const;

protected:
    ObexServer() = default;

    std::string serviceName_ = "";
    std::unique_ptr<ObexPrivateServer> rfcommServer_ = nullptr;
    std::unique_ptr<ObexPrivateServer> l2capServer_ = nullptr;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_SERVER_H
