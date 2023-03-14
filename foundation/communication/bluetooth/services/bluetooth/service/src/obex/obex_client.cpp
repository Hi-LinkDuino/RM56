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

#include "obex_client.h"
#include <cstring>
#include <iostream>
#include "buffer.h"
#include "log.h"
#include "obex_socket_transport.h"
#include "obex_utils.h"
#include "transport/transport_l2cap.h"

namespace OHOS {
namespace bluetooth {
ObexClient::ObexClientTransportObserver::ObexClientTransportObserver(ObexClient &obexClient) : obexClient_(obexClient)
{}

void ObexClient::ObexClientTransportObserver::OnTransportConnected(ObexTransport &transport)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", obexClient_.clientId_.c_str());
    if (!transport.IsConnected()) {
        obexClient_.clientObserver_.OnTransportFailed(obexClient_, -1);
        return;
    }
    obexClient_.clientState_ = ObexClientState::TRANSPORT_CONNECTED;
    if (obexClient_.isSupportReliableSession_ && obexClient_.reliableSessionReqHeader_ != nullptr) {
        obexClient_.SendRequest(*obexClient_.reliableSessionReqHeader_);
    } else if (obexClient_.connectReqHeader_ != nullptr) {
        obexClient_.SendConnectRequest(*obexClient_.connectReqHeader_);
    }
}

void ObexClient::ObexClientTransportObserver::OnTransportDisconnected(ObexTransport &transport)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", obexClient_.GetClientId().c_str());

    obexClient_.clientState_ = ObexClientState::TRANSPORT_DISCONNECTED;
    obexClient_.isObexConnected_ = false;
    obexClient_.connectReqHeader_ = nullptr;
    obexClient_.clientObserver_.OnDisconnected(obexClient_);
}

void ObexClient::ObexClientTransportObserver::OnTransportDataBusy(ObexTransport &transport, uint8_t isBusy)
{
    OBEX_LOG_INFO("Call %{public}s, isBusy %{public}d", __PRETTY_FUNCTION__, isBusy);
    OBEX_LOG_INFO("ClientId: %{public}s", obexClient_.GetClientId().c_str());
    obexClient_.HandleTransportDataBusy(isBusy);
}

void ObexClient::ObexClientTransportObserver::OnTransportDataAvailable(
    ObexTransport &transport, ObexPacket &obexPacket)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", obexClient_.GetClientId().c_str());
    obexClient_.isProcessing_ = false;
    auto resp = GetObexHeaderFromPacket(obexPacket);
    if (!resp) {
        return;
    }
    obexClient_.clientSession_->SetLastRespCd(resp->GetFieldCode());
    OBEX_LOG_DEBUG("lastOpeId : %02X", obexClient_.clientSession_->GetLastOpeId());
    switch (obexClient_.clientSession_->GetLastOpeId()) {
        case static_cast<uint8_t>(ObexOpeId::CONNECT):
            HandleDataAvailableConnect(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::DISCONNECT):
            HandleDataAvailableDisconnect(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::PUT):
        case static_cast<uint8_t>(ObexOpeId::PUT_FINAL):
            HandleDataAvailablePut(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::GET):
        case static_cast<uint8_t>(ObexOpeId::GET_FINAL):
            HandleDataAvailableGet(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::SETPATH):
            HandleDataAvailableSetPath(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::SESSION):
            HandleDataAvailableSession(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::ABORT):
            HandleDataAvailableAbort(*resp);
            break;
        case static_cast<uint8_t>(ObexOpeId::ACTION):
            HandleDataAvailableAction(*resp);
            break;
        default:
            break;
    }
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableAction(const ObexHeader &resp)
{
    obexClient_.clientObserver_.OnActionCompleted(obexClient_, resp);
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableAbort(const ObexHeader &resp)
{
    if (resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        // Skip unfinished get/put contniue packet
        OBEX_LOG_DEBUG("abort request is sended, therefore skip unfinished get/put contniue packet");
        return;
    }
    obexClient_.AbortDataAvailable(resp);
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableSession(const ObexHeader &resp)
{
    if (resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        auto sessionParams = resp.GetItemSessionParams();
        // sessionParams is not exists? do disconnect
        if (sessionParams == nullptr) {
            obexClient_.Disconnect();
            return;
        }
        obexClient_.isReliableSessionCreated_ = true;
        obexClient_.clientState_ = ObexClientState::RELIABLE_SESSION_CREATED;
        // will support in the future: COPY Session param to ClientSession
        obexClient_.SendConnectRequest(*obexClient_.connectReqHeader_);
    } else {
        obexClient_.clientObserver_.OnActionCompleted(obexClient_, resp);
    }
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableSetPath(const ObexHeader &resp)
{
    obexClient_.SetPathDataAvailable(resp);
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailablePut(const ObexHeader &resp)
{
    obexClient_.PutDataAvailable(resp);
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableGet(const ObexHeader &resp)
{
    obexClient_.GetDataAvailable(resp);
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableDisconnect(const ObexHeader &resp)
{
    obexClient_.clientObserver_.OnActionCompleted(obexClient_, resp);
    obexClient_.clientState_ = ObexClientState::OBEX_DISCONNECTED;
    obexClient_.isObexConnected_ = false;
    obexClient_.connectReqHeader_ = nullptr;
    obexClient_.clientTransport_->Disconnect();
}

void ObexClient::ObexClientTransportObserver::HandleDataAvailableConnect(const ObexHeader &resp)
{
    if (resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        obexClient_.clientState_ = ObexClientState::OBEX_CONNECTED;
        // set connected and mtu
        obexClient_.isObexConnected_ = true;
        uint16_t serverMaxPktLen = *resp.GetFieldMaxPacketLength();
        if (serverMaxPktLen > OBEX_MINIMUM_MTU && serverMaxPktLen < obexClient_.clientSession_->GetMaxPacketLength()) {
            obexClient_.clientSession_->SetMaxPacketLength(serverMaxPktLen);
            OBEX_LOG_DEBUG("MTU Reset to:%{public}d", serverMaxPktLen);
        }
        // connect id
        auto connectId = resp.GetItemConnectionId();
        if (connectId != nullptr) {
            obexClient_.clientSession_->SetConnectId(connectId->GetWord());
        }
        obexClient_.clientObserver_.OnConnected(obexClient_, resp);
    } else {
        obexClient_.clientObserver_.OnConnectFailed(obexClient_, resp);
    }
}

void ObexClient::ObexClientTransportObserver::OnTransportError(ObexTransport &transport, int errCd)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_ERROR("ClientId: %{public}s, errCd: %{public}d", obexClient_.GetClientId().c_str(), errCd);
    obexClient_.clientObserver_.OnTransportFailed(obexClient_, errCd);
}

std::unique_ptr<bluetooth::ObexHeader> ObexClient::ObexClientTransportObserver::GetObexHeaderFromPacket(
    ObexPacket &obexPacket)
{
    uint8_t *packetBuf = obexPacket.GetBuffer();
    size_t packetBufSize = obexPacket.GetSize();
    if (packetBufSize < ObexHeader::MIN_PACKET_LENGTH) {
        OBEX_LOG_ERROR("dataSize[%{public}zu] < min[%{public}d]", packetBufSize, ObexHeader::MIN_PACKET_LENGTH);
        return nullptr;
    }
    uint16_t packetLength = ObexUtils::GetBufData16(&packetBuf[0], 1);
    if (packetLength != packetBufSize) {
        OBEX_LOG_ERROR("packetLength[%{public}hu] != packetBufSize[%{public}zu]", packetLength, packetBufSize);
        return nullptr;
    }
    std::unique_ptr<bluetooth::ObexHeader> resp;

    if (obexClient_.clientSession_->GetLastOpeId() == static_cast<uint8_t>(ObexOpeId::CONNECT)) {
        resp = ObexHeader::ParseResponse(packetBuf, packetLength, true);
    } else {
        resp = ObexHeader::ParseResponse(packetBuf, packetLength, false);
    }
    if (resp == nullptr) {
        OBEX_LOG_ERROR("ParseResponse failure");
        return nullptr;
    }
    return resp;
}

ObexClient::ObexClient(const ObexClientConfig &config, ObexClientObserver &observer, utility::Dispatcher &dispatcher)
    : clientObserver_(observer), dispatcher_(dispatcher)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    isObexConnected_ = false;
    transportObserver_ = std::make_unique<ObexClientTransportObserver>(*this);
    ObexClientSocketTransport::Option option;
    option.addr_ = config.addr_;
    option.scn_ = config.scn_;
    option.mtu_ = config.mtu_;
    option.lpsm_ = config.lpsm_;

    option.isGoepL2capPSM_ = config.isGoepL2capPSM_;
    clientTransport_ = std::make_unique<ObexClientSocketTransport>(option, *transportObserver_, dispatcher);
    isSupportSrm_ = config.isSupportSrm_;  // srm mode
    isSupportReliableSession_ = config.isSupportReliableSession_;
    clientSession_ = std::make_unique<ObexClientSession>(RawAddress::ConvertToString(config.addr_.addr));
    clientSession_->SetMaxPacketLength(config.mtu_);
    clientSession_->SetServiceUUID(config.serviceUUID_);
    clientSession_->FreeSendObject();
    connectReqHeader_ = nullptr;
    clientState_ = ObexClientState::INIT;
}

// send obex request
int ObexClient::SendRequest(const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::SESSION)) {
        auto sessionParams = req.GetItemSessionParams();
        if (sessionParams == nullptr) {
            OBEX_LOG_ERROR("Request of session is invalid!");
            return -1;
        }
        const TlvTriplet *tlv = sessionParams->GetTlvtriplet(ObexSessionParameters::SESSION_OPCODE);
        if (tlv == nullptr) {
            OBEX_LOG_ERROR("Request of session is invalid!");
            return -1;
        }
        if (!CheckBeforeSession(tlv->GetVal()[0])) {
            return -1;
        }
    } else {
        if (!CheckBeforeRequest(req.GetFieldCode())) {
            return -1;
        }
    }

    if (req.GetFieldPacketLength() > clientSession_->GetMaxPacketLength()) {
        OBEX_LOG_ERROR(
            "Request packet length[%{public}d] > mtu[%{public}d].", req.GetFieldPacketLength(), clientSession_->GetMaxPacketLength());
        return -1;
    }
    auto obexPacket = req.Build();
    bool srmEnable = false;
    if (req.HasHeader(ObexHeader::SRM)) {
        srmEnable = req.GetItemSrm();
    }
    if ((req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::PUT)) &&
        !srmEnable) {
        isProcessing_ = false;
    } else {
        isProcessing_ = true;
    }
    bool ret = clientTransport_->Write(obexPacket->GetPacket());
    if (!ret) {
        OBEX_LOG_ERROR("SendRequest Fail!");
        isProcessing_ = false;
        return -1;
    }
    OBEX_LOG_DEBUG("Client Set lastOpeId: 0x%02X", req.GetFieldCode());

    clientSession_->SetLastOpeId(req.GetFieldCode());
    clientSession_->SetLastReqHeader(req);

    return 0;
}

// send obex connect request
int ObexClient::Connect()
{
    ObexConnectParams connectParams;
    return Connect(connectParams);
}

// send obex connect request with params
int ObexClient::Connect(ObexConnectParams &connectParams)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (!CheckBeforeRequest(static_cast<uint8_t>(ObexOpeId::CONNECT))) {
        return -1;
    }
    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::CONNECT);
    const BtUuid &serviceUUID = clientSession_->GetServiceUUID();
    if (serviceUUID.uuid128[0] != 0x00) {
        header->AppendItemTarget(&serviceUUID.uuid128[0], sizeof(serviceUUID.uuid128));
    }
    if (connectParams.appParams_ != nullptr) {
        header->AppendItemAppParams(*connectParams.appParams_);
    }
    if (connectParams.authChallenges_ != nullptr) {
        header->AppendItemAuthChallenges(*connectParams.authChallenges_);
    }
    if (connectParams.authResponses_ != nullptr) {
        header->AppendItemAuthResponse(*connectParams.authResponses_);
    }
    if (connectParams.count != nullptr) {
        header->AppendItemCount(*connectParams.count);
    }
    // create transport before obex connect
    if (!clientTransport_->IsConnected()) {
        connectReqHeader_ = std::move(header);
        return clientTransport_->Connect();
    }
    return SendConnectRequest(*header);
}

// send obex disconnect request
int ObexClient::Disconnect(bool withObexReq)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (withObexReq) {
        if (!CheckBeforeRequest(static_cast<uint8_t>(ObexOpeId::DISCONNECT))) {
            return -1;
        }
        auto req = ObexHeader::CreateRequest(ObexOpeId::DISCONNECT);
        if (isReliableSessionCreated_) {
            req->AppendItemSessionSeqNum(clientSession_->GetReliableSession()->sessionSequenceNumber_++);
        }
        uint32_t connectId = clientSession_->GetConnectId();
        if (connectId != 0x00) {
            req->AppendItemConnectionId(connectId);
        }
        return SendRequest(*req);
    }
    return clientTransport_->Disconnect();
}

// send obex abort request
int ObexClient::Abort()
{
    dispatcher_.PostTask(std::bind(&ObexClient::ProcessAbort, this));
    return 0;
}

int ObexClient::ProcessAbort()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (isWaitingSendAbort_ || isAbortSended_) {
        OBEX_LOG_ERROR("Abort is processing!");
        return -1;
    }
    isAbortSended_ = false;
    if (isProcessing_) {
        isAbortSended_ = false;
        switch (clientSession_->GetLastOpeId()) {
            case static_cast<uint8_t>(ObexOpeId::PUT):
            case static_cast<uint8_t>(ObexOpeId::PUT_FINAL):
            case static_cast<uint8_t>(ObexOpeId::GET):
            case static_cast<uint8_t>(ObexOpeId::GET_FINAL):
                isWaitingSendAbort_ = true;
                break;
            default:
                isWaitingSendAbort_ = false;
                break;
        }
        return 0;
    }
    return SendAbortRequest();
}

int ObexClient::SendAbortRequest()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (!CheckBeforeRequest(static_cast<uint8_t>(ObexOpeId::ABORT))) {
        return -1;
    }
    auto req = ObexHeader::CreateRequest(ObexOpeId::ABORT);
    uint32_t connectId = clientSession_->GetConnectId();
    if (connectId != 0x00) {
        req->AppendItemConnectionId(connectId);
    }
    isWaitingSendAbort_ = false;
    isAbortSended_ = true;
    return SendRequest(*req);
}

// send obex put request
int ObexClient::Put(const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (!CheckBeforeRequest(req.GetFieldCode())) {
        return -1;
    }
    if (req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::PUT) &&
        req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::PUT_FINAL)) {
        OBEX_LOG_ERROR("Opcode is wrong.");
        return -1;
    }
    return SendRequest(req);
}

// send obex get request
int ObexClient::Get(const ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (!CheckBeforeRequest(req.GetFieldCode())) {
        return -1;
    }
    if (req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::GET) &&
        req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::GET_FINAL)) {
        OBEX_LOG_ERROR("Opcode is wrong.");
        return -1;
    }
    return SendRequest(req);
}

// send obex set_path request
int ObexClient::SetPath(uint8_t flag, const std::u16string &path)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    if (!CheckBeforeRequest(static_cast<uint8_t>(ObexOpeId::SETPATH))) {
        return -1;
    }
    if (path.find(u"/") != std::u16string::npos) {
        OBEX_LOG_ERROR("The path shall not include any path information!");
        return -1;
    }
    auto req = ObexHeader::CreateRequest(ObexOpeId::SETPATH);
    req->SetFieldFlags(flag);
    uint32_t connectId = clientSession_->GetConnectId();
    if (connectId != 0x00) {
        req->AppendItemConnectionId(connectId);
    }
    if (path.length() > 0) {
        req->AppendItemName(path);
    }
    return SendRequest(*req);
}

// send obex action request
int ObexClient::Copy(const std::u16string &srcName, const std::u16string &destName)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    // will support in the future
    return 0;
}

int ObexClient::Move(const std::u16string &srcName, const std::u16string &destName)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    // will support in the future
    return 0;
}

int ObexClient::SetPermissions(const std::u16string &name, const uint32_t permissions)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("ClientId: %{public}s", GetClientId().c_str());
    // will support in the future
    return 0;
}

// send obex session request
// This command must include a Session-Parameters header containing the Session Opcode, Device Address and Nonce
// fields. Optionally, a Timeout field can be included.
int ObexClient::CreateSession()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    // will support in the future
    return CreateSession(OBEX_SESSION_MAX_TIMEOUT_SEC);
}

int ObexClient::CreateSession(uint32_t timeoutSec)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return 0;
}

// This command must include a Session-Parameters header containing the Session Opcode field.
// Optionally, a Timeout field can be included.
int ObexClient::SuspendSession()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return 0;
}

// This command must include a Session-Parameters header containing the Session Opcode, Device Address, Nonce,
// and Session ID and fields. Optionally, a Timeout field can be included.
int ObexClient::ResumeSession()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return 0;
}

// This command must include a Session-Parameters header containing the Session Opcode and Session ID fields.
int ObexClient::CloseSession()
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return 0;
}

// This command must include a Session-Parameters header containing the Session Opcode field.
// Optionally, a Timeout field can be included.
int ObexClient::SetSessionTimeout(uint32_t timeoutSec)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    return 0;
}

bool ObexClient::CheckBeforeSession(uint8_t sessionOpcode)
{
    return true;
}

bool ObexClient::CheckBeforeRequest(uint8_t opeId) const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (isProcessing_) {
        OBEX_LOG_ERROR("Another operation is being processed, please try again later.");
        return false;
    }
    bool checkConnect = true;
    bool needConnected = true;
    switch (opeId) {
        case static_cast<uint8_t>(ObexOpeId::CONNECT):
            needConnected = false;
            break;
        case static_cast<uint8_t>(ObexOpeId::SESSION):
            checkConnect = false;  // Session's connect check is in CheckBeforeSession
            break;
        case static_cast<uint8_t>(ObexOpeId::DISCONNECT):
            if (!isObexConnected_) {
                OBEX_LOG_ERROR("Already Disconnected from the server.");
                return false;
            }
            break;
        default:
            break;
    }
    if (checkConnect) {
        if (needConnected && !isObexConnected_) {
            OBEX_LOG_ERROR(
                "Please connect first. Before obex connected, only SESSION(Create) and CONNECT Operation can "
                "be called.");
            return false;
        }
        if (!needConnected && isObexConnected_) {
            OBEX_LOG_ERROR("Already connected to server.");
            return false;
        }
    }
    return true;
}

void ObexClient::PutDataAvailable(const ObexHeader &resp)
{
    if (isWaitingSendAbort_) {
        isProcessing_ = false;
        SendAbortRequest();
        return;
    }
    SetBusy(false);
    clientObserver_.OnActionCompleted(*this, resp);
}

void ObexClient::GetDataAvailable(const ObexHeader &resp)
{
    if (isWaitingSendAbort_) {
        isProcessing_ = false;
        SendAbortRequest();
        return;
    }
    SetBusy(false);
    clientObserver_.OnActionCompleted(*this, resp);
}

void ObexClient::SetPathDataAvailable(const ObexHeader &resp)
{
    clientObserver_.OnActionCompleted(*this, resp);
}

void ObexClient::AbortDataAvailable(const ObexHeader &resp)
{
    isWaitingSendAbort_ = false;
    isAbortSended_ = false;
    SetBusy(false);
    clientObserver_.OnActionCompleted(*this, resp);
}

void ObexClient::HandleTransportDataBusy(uint8_t isBusy)
{
    OBEX_LOG_INFO("Call %{public}s, isBusy %{public}d", __PRETTY_FUNCTION__, isBusy);
    clientObserver_.OnBusy(*this, isBusy);
}

int ObexClient::SendConnectRequest(ObexHeader &header)
{
    int minPacketSize = std::min(clientTransport_->GetMaxSendPacketSize(),
        clientTransport_->GetMaxReceivePacketSize());
    minPacketSize = std::min(minPacketSize, (int)clientSession_->GetMaxPacketLength());
    if (minPacketSize > OBEX_MAXIMUM_MTU) {
        minPacketSize = OBEX_MAXIMUM_MTU;
    } else if (minPacketSize < OBEX_MINIMUM_MTU) {
        minPacketSize = OBEX_MINIMUM_MTU;
    }
    header.SetFieldMaxPacketLength(minPacketSize);
    clientSession_->SetMaxPacketLength(minPacketSize);
    OBEX_LOG_DEBUG("SendConnect with mtu:%{public}d", minPacketSize);
    return SendRequest(header);
}

ObexClientSession &ObexClient::GetClientSession() const
{
    return *clientSession_;
}

const std::string &ObexClient::GetClientId()
{
    if (clientId_.empty()) {
        clientId_ = (clientTransport_ == nullptr) ? "" : clientTransport_->GetTransportKey();
    }
    return clientId_;
}

int ObexClient::RegisterL2capLPsm(uint16_t lpsm)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("RegisterL2capLPsm: 0x%04X", lpsm);
    return L2capTransport::RegisterClientPsm(lpsm);
}

void ObexClient::DeregisterL2capLPsm(uint16_t lpsm)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_INFO("DeregisterL2capLPsm: 0x%04X", lpsm);
    L2capTransport::DeregisterClientPsm(lpsm);
}

void ObexClient::SetBusy(bool isBusy)
{
    if (clientSession_->IsBusy() != isBusy) {
        clientSession_->SetBusy(isBusy);
        OBEX_LOG_INFO("[%{public}s] ObexBusy=%{public}d", GetClientId().c_str(), isBusy);
        clientObserver_.OnBusy(*this, isBusy);
    }
}

void ObexClientObserver::OnBusy(ObexClient &client, bool isBusy)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}
}  // namespace bluetooth
}  // namespace OHOS
