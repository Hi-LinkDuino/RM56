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

#include "obex_server.h"
#include <cstring>
#include "bt_def.h"
#include "dispatcher.h"
#include "log.h"
#include "obex_utils.h"

namespace OHOS {
namespace bluetooth {
ObexServer::ObexServer(const std::string &serviceName, const ObexServerConfig &config, ObexServerObserver &observer,
    utility::Dispatcher &dispatcher)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    serviceName_ = serviceName;
    if (config.useRfcomm_) {
        ObexPrivateServer::ObexPrivateServerConfig option;
        option.isGoepL2capPSM_ = false;
        option.scn_ = config.rfcommScn_;
        option.mtu_ = config.rfcommMtu_;
        option.isSupportSrm_ = false;  // rfcomm not support srm mode
        option.isSupportReliableSession_ = config.isSupportReliableSession_;
        rfcommServer_ = std::make_unique<ObexPrivateServer>(option, observer, dispatcher);
    }
    if (config.useL2cap_) {
        ObexPrivateServer::ObexPrivateServerConfig option;
        option.isGoepL2capPSM_ = true;
        option.scn_ = config.l2capPsm_;
        option.mtu_ = config.l2capMtu_;
        option.isSupportSrm_ = config.isSupportSrm_;
        option.isSupportReliableSession_ = config.isSupportReliableSession_;
        l2capServer_ = std::make_unique<ObexPrivateServer>(option, observer, dispatcher);
    }
}

int ObexServer::Startup() const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (rfcommServer_ == nullptr && l2capServer_ == nullptr) {
        return -1;
    }
    bool rfcommResult = false;
    if (rfcommServer_ != nullptr) {
        int retRfcomm = rfcommServer_->Startup();
        if (retRfcomm != 0) {
            OBEX_LOG_ERROR("Error: Startup rfcommServer fail with code [0x%02X]", retRfcomm);
            return retRfcomm;
        }
        rfcommResult = true;
    }
    if (l2capServer_ != nullptr) {
        int retL2cap = l2capServer_->Startup();
        if (retL2cap != 0) {
            OBEX_LOG_ERROR("Error: Startup l2capServer fail with code [0x%02X]", retL2cap);
            if (rfcommResult) {
                rfcommServer_->Shutdown();
            }
            return retL2cap;
        }
    }
    return 0;
}

void ObexServer::Shutdown() const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (rfcommServer_ != nullptr) {
        rfcommServer_->Shutdown();
    }
    if (l2capServer_ != nullptr) {
        l2capServer_->Shutdown();
    }
}

void ObexServerObserver::OnTransportConnect(ObexIncomingConnect &incomingConnect)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    incomingConnect.AcceptConnection();
}

void ObexServerObserver::OnTransportConnected(const std::string &btAddr)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

void ObexServerObserver::OnTransportDisconnected(const std::string &btAddr)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

void ObexServerObserver::OnTransportError(const std::string &btAddr, int errCd, const std::string &msg)
{
    OBEX_LOG_ERROR("Call %{public}s, ERROR:%{public}d, %{public}s", __PRETTY_FUNCTION__, errCd, msg.c_str());
}


void ObexServerObserver::OnError(const int errCd, const std::string &msg)
{
    OBEX_LOG_ERROR("Call %{public}s, ERROR:%{public}d, %{public}s", __PRETTY_FUNCTION__, errCd, msg.c_str());
}

void ObexServerObserver::OnPut(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnGet(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnAbort(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnSetPath(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnAction(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnSession(ObexServerSession &session, const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED, false));
}

void ObexServerObserver::OnBusy(ObexServerSession &session, bool isBusy) const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

const uint16_t ObexPrivateServer::MAX_TRASH_SESSION_COUNT = 50;

ObexPrivateServer::ObexServerTransportObserver::ObexServerTransportObserver(ObexPrivateServer &obexServer)
    : obexServer_(obexServer)
{}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportConnectIncoming(ObexIncomingConnect &incomingConnect)
{
    OBEX_LOG_INFO("Call %{public}s start", __PRETTY_FUNCTION__);
    obexServer_.observer_.OnTransportConnect(incomingConnect);
    OBEX_LOG_INFO("Call %{public}s end", __PRETTY_FUNCTION__);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportIncomingDisconnected(const std::string &btAddr)
{
    OBEX_LOG_INFO("Call %{public}s start", __PRETTY_FUNCTION__);
    obexServer_.observer_.OnTransportDisconnected(btAddr);
    OBEX_LOG_INFO("Call %{public}s end", __PRETTY_FUNCTION__);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportConnected(ObexTransport &transport)
{
    OBEX_LOG_INFO("Call %{public}s start", __PRETTY_FUNCTION__);
    auto session = std::make_unique<ObexServerSession>(transport,
        obexServer_.isSupportSrm_,
        obexServer_.dispatcher_,
        std::bind(&ObexPrivateServer::RemoveSession, &obexServer_, std::placeholders::_1),
        std::bind(&ObexPrivateServer::SetBusy, &obexServer_, std::placeholders::_1, std::placeholders::_2));
    std::string btAddrStr = session->GetRemoteAddr().GetAddress();
    session->SetMaxPacketLength(obexServer_.initMtu_);

    obexServer_.serverSessionsMap_[&transport] = std::move(session);
    obexServer_.observer_.OnTransportConnected(btAddrStr);
    OBEX_LOG_INFO("Call %{public}s end", __PRETTY_FUNCTION__);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportDisconnected(ObexTransport &transport)
{
    OBEX_LOG_INFO("Call %{public}s start", __PRETTY_FUNCTION__);
    std::string btAddrStr = transport.GetRemoteAddress().GetAddress();
    obexServer_.RemoveSessionByTransport(transport);
    obexServer_.observer_.OnTransportDisconnected(btAddrStr);
    OBEX_LOG_INFO("Call %{public}s end", __PRETTY_FUNCTION__);
}

void ObexPrivateServer::ObexServerTransportObserver::HandleDataAvailableConnect(
    ObexServerSession &serverSession, const ObexHeader &req)
{
    int minPacketSize = std::min(
        serverSession.GetTransport().GetMaxReceivePacketSize(), serverSession.GetTransport().GetMaxSendPacketSize());
    minPacketSize = std::min(minPacketSize, static_cast<int>(serverSession.GetMaxPacketLength()));
    minPacketSize = std::min(minPacketSize, static_cast<int>(*req.GetFieldMaxPacketLength()));
    if (minPacketSize < OBEX_MINIMUM_MTU) {
        minPacketSize = OBEX_MINIMUM_MTU;
    }
    serverSession.SetMaxPacketLength(minPacketSize);
    OBEX_LOG_INFO("Server Mtu set to:%{public}d", minPacketSize);
    obexServer_.observer_.OnConnect(serverSession, req);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportDataAvailable(
    ObexTransport &transport, ObexPacket &obexPacket)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (obexServer_.serverSessionsMap_.find(&transport) == obexServer_.serverSessionsMap_.end()) {
        OBEX_LOG_ERROR("ServerSession for has been erased, so skip available data!");
        return;
    }
    ObexServerSession &serverSession = *obexServer_.serverSessionsMap_.at(&transport);
    auto req = GetObexHeaderFromPacket(obexPacket);
    if (req == nullptr) {
        OBEX_LOG_ERROR("error:ParseRequest failure:response BAD_REQUEST");
        if (serverSession.SendSimpleResponse(ObexRspCode::BAD_REQUEST) != 0) {
            OBEX_LOG_ERROR("Response BAD_REQUEST send failure!");
            obexServer_.observer_.OnError(-1, "Response BAD_REQUEST send failure!");
        }
        return;
    }
    uint8_t code = req->GetFieldCode();
    switch (code) {
        case static_cast<uint8_t>(ObexOpeId::CONNECT):
            HandleDataAvailableConnect(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::DISCONNECT):
            obexServer_.observer_.OnDisconnect(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::PUT):
        case static_cast<uint8_t>(ObexOpeId::PUT_FINAL):
            obexServer_.HandlePutRequest(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::GET):
        case static_cast<uint8_t>(ObexOpeId::GET_FINAL):
            obexServer_.HandleGetRequest(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::SETPATH):
            obexServer_.HandleSetPathRequest(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::ACTION):
            obexServer_.observer_.OnAction(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::SESSION):
            obexServer_.observer_.OnSession(serverSession, *req);
            break;
        case static_cast<uint8_t>(ObexOpeId::ABORT):
            obexServer_.HandleAbortRequest(serverSession, *req);
            break;
        default:
            obexServer_.observer_.OnError(-1, "opcode is wrong!");
    }
}

std::unique_ptr<bluetooth::ObexHeader> ObexPrivateServer::ObexServerTransportObserver::GetObexHeaderFromPacket(
    ObexPacket &obexPacket) const
{
    uint8_t *packetBuf = obexPacket.GetBuffer();
    uint32_t packetBufSize = obexPacket.GetSize();
    if (packetBufSize < ObexHeader::MIN_PACKET_LENGTH) {
        OBEX_LOG_ERROR("error:dataSize < 3:%{public}d", int(packetBufSize));
        obexServer_.observer_.OnError(-1, "error:dataSize < 3");
        return nullptr;
    }
    uint16_t packetLength = ObexUtils::GetBufData16(&packetBuf[0], 1);
    if (packetLength != packetBufSize) {
        OBEX_LOG_ERROR("error:packetLength[%{public}d] != packetBufSize[%{public}d]",
            int(packetLength), int(packetBufSize));
        obexServer_.observer_.OnError(-1, "packetLength != packetBufSize");
        return nullptr;
    }
    return ObexHeader::ParseRequest(packetBuf, packetLength);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportDataBusy(ObexTransport &transport, uint8_t isBusy)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (obexServer_.serverSessionsMap_.find(&transport) == obexServer_.serverSessionsMap_.end()) {
        OBEX_LOG_ERROR("ServerSession for has been erased, so skip available data!");
        return;
    }
    ObexServerSession &serverSession = *obexServer_.serverSessionsMap_.at(&transport);
    obexServer_.HandleTransportDataBusy(serverSession, isBusy);
}

void ObexPrivateServer::ObexServerTransportObserver::OnTransportError(ObexTransport &transport, int errCd)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    obexServer_.observer_.OnError(errCd, "OnTransportError");
}

ObexPrivateServer::ObexPrivateServer(
    const ObexPrivateServerConfig &config, ObexServerObserver &observer, utility::Dispatcher &dispatcher)
    : observer_(observer), dispatcher_(dispatcher)
{
    transportObserver_ = std::make_unique<ObexServerTransportObserver>(*this);
    ObexServerSocketTransport::Option option;
    option.isGoepL2capPSM_ = config.isGoepL2capPSM_;
    option.scn_ = config.scn_;
    option.mtu_ = config.mtu_;
    initMtu_ = config.mtu_;

    isSupportSrm_ = config.isSupportSrm_;
    isSupportReliableSession_ = config.isSupportReliableSession_;
    serverTransport_ = std::make_unique<ObexServerSocketTransport>(option, *transportObserver_, dispatcher);
}

int ObexPrivateServer::Startup() const
{
    return serverTransport_->Listen();
}

void ObexPrivateServer::Shutdown() const
{
    serverTransport_->Disconnect();
}

int ObexPrivateServer::RemoveSession(ObexServerSession &session) const
{
    if (serverSessionsMap_.find(&session.GetTransport()) == serverSessionsMap_.end()) {
        return RET_NO_SUPPORT;
    }
    return serverTransport_->Disconnect(session.GetTransport());
}

void ObexPrivateServer::HandlePutRequest(ObexServerSession &session, ObexHeader &req) const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    observer_.OnPut(session, req);
}

void ObexPrivateServer::HandleGetRequest(ObexServerSession &session, ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    observer_.OnGet(session, req);
}

void ObexPrivateServer::HandleSetPathRequest(ObexServerSession &session, ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    observer_.OnSetPath(session, req);
}

void ObexPrivateServer::HandleAbortRequest(ObexServerSession &session, ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    observer_.OnAbort(session, req);
}

void ObexPrivateServer::HandleTransportDataBusy(ObexServerSession &session, uint8_t isBusy)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

void ObexPrivateServer::SetBusy(ObexServerSession &session, bool isBusy) const
{
    if (session.IsBusy() != isBusy) {
        session.SetBusy(isBusy);
        OBEX_LOG_INFO("[%{public}s] ObexBusy=%{public}d", session.GetTransport().GetTransportKey().c_str(), isBusy);
        observer_.OnBusy(session, isBusy);
    }
}

void ObexPrivateServer::RemoveSessionByTransport(ObexTransport &transport)
{
    auto target = serverSessionsMap_.find(&transport);
    if (target == serverSessionsMap_.end()) {
        return;
    }
    target->second->Invalid();
    invalidSessions_.push_back(std::move(target->second));
    serverSessionsMap_.erase(&transport);
    if (invalidSessions_.size() > MAX_TRASH_SESSION_COUNT) {
        invalidSessions_.pop_front();
    }
}
}  // namespace bluetooth
}  // namespace OHOS