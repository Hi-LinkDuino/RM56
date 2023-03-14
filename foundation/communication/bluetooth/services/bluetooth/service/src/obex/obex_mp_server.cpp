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

#include "obex_mp_server.h"
#include <memory>
#include "log.h"

namespace OHOS {
namespace bluetooth {
// create obex server
ObexMpServer::ObexMpServer(const std::string &serviceName, const ObexServerConfig &config,
    ObexServerObserver &observer, utility::Dispatcher &dispatcher)
{
    OBEX_LOG_INFO("Call ObexMpServer %{public}s", __PRETTY_FUNCTION__);
    serviceName_ = serviceName;
    if (config.useRfcomm_) {
        ObexPrivateServer::ObexPrivateServerConfig option;
        option.isGoepL2capPSM_ = false;
        option.scn_ = config.rfcommScn_;
        option.mtu_ = config.rfcommMtu_;
        option.isSupportSrm_ = false;  // rfcomm not support srm mode
        option.isSupportReliableSession_ = config.isSupportReliableSession_;
        rfcommServer_ = std::make_unique<ObexPrivateMpServer>(option, observer, dispatcher);
    }
    if (config.useL2cap_) {
        ObexPrivateServer::ObexPrivateServerConfig option;
        option.isGoepL2capPSM_ = true;
        option.scn_ = config.l2capPsm_;
        option.mtu_ = config.l2capMtu_;
        option.isSupportSrm_ = config.isSupportSrm_;
        option.isSupportReliableSession_ = config.isSupportReliableSession_;
        l2capServer_ = std::make_unique<ObexPrivateMpServer>(option, observer, dispatcher);
    }
}

ObexPrivateMpServer::ObexPrivateMpServer(
    const ObexPrivateServerConfig &config, ObexServerObserver &observer, utility::Dispatcher &dispatcher)
    : ObexPrivateServer(config, observer, dispatcher)
{}

void ObexPrivateMpServer::HandlePutRequest(ObexServerSession &session, ObexHeader &req) const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    if (req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::PUT)) {
        SetBusy(session, true);
        OBEX_LOG_DEBUG("HandlePutRequest:Server Received Put request without final bit!");
        if (session.GetReceivedObject() == nullptr) {
            OBEX_LOG_DEBUG("HandlePutRequest:Server Create ObexServerReceivedObject");
            std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();
            session.CreateReceivedObject(req, writer);
        }
        auto body = req.GetItemBody();
        if (body != nullptr) {
            OBEX_LOG_DEBUG("HandlePutRequest:Server Received Body From Request!");
            session.GetReceivedObject()->AppendBody(body->GetBytes().get(), body->GetHeaderDataSize());
        }
        if (session.GetReceivedObject()->IsSrmReceiving()) {
            return;
        }
        auto respHeader = ObexHeader::CreateResponse(ObexRspCode::CONTINUE);
        auto connectId = req.GetItemConnectionId();
        if (connectId != nullptr) {
            respHeader->AppendItemConnectionId(connectId->GetWord());
        }
        if (isSupportSrm_ && req.GetItemSrm()) {
            OBEX_LOG_DEBUG("HandlePutRequest:Server Response Set SRM = TRUE");
            respHeader->AppendItemSrm(true);
            session.GetReceivedObject()->SetSrmEnable(true);
        }
        if (session.GetReceivedObject()->IsSrmEnable() && req.GetItemSrmp()) {
            OBEX_LOG_DEBUG("HandlePutRequest:Server Receive With SRMP");
            session.GetReceivedObject()->SetSrmWait(req.GetItemSrmp());
        }
        if (session.GetReceivedObject()->IsSrmEnable() && !session.GetReceivedObject()->IsSrmWait()) {
            OBEX_LOG_DEBUG("HandlePutRequest:Start SRM Receive...");
            session.GetReceivedObject()->SetSrmReceiving();
        }

        // when srmp header wait is use ?
        int ret = session.SendResponse(*respHeader);
        OBEX_LOG_DEBUG("HandlePutRequest:Server Send Response with ret = %{public}d", ret);
        if (ret != 0) {
            observer_.OnError(ret, "send response fail");
        }
    } else {
        HandlePutFinalRequest(session, req);
    }
}

void ObexPrivateMpServer::HandlePutFinalRequest(ObexServerSession &session, ObexHeader &req) const
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    OBEX_LOG_DEBUG("HandlePutRequest:Server Received Put request with final bit!");
    // final bit
    if (session.GetReceivedObject() != nullptr) {
        OBEX_LOG_DEBUG("HandlePutRequest:Server Has ObexServerReceivedObject!");
        auto body = req.GetItemEndBody();
        if (body != nullptr) {
            OBEX_LOG_DEBUG("HandlePutRequest:Server Received End-Body From Request!");
            session.GetReceivedObject()->AppendBody(body->GetBytes().get(), body->GetHeaderDataSize());
        }
        ObexPrivateServer::HandlePutRequest(session, *session.GetReceivedObject()->CreateReceivedHeader());
        session.FreeReceivedObject();
        return;
    }
    OBEX_LOG_DEBUG("HandlePutRequest:Server Has not ObexServerReceivedObject!");
    auto body = req.GetItemEndBody();
    if (body != nullptr) {
        auto writer = std::make_shared<ObexArrayBodyObject>();
        writer->Write(body->GetBytes().get(), body->GetHeaderDataSize());
        OBEX_LOG_DEBUG("HandlePutRequest:Server Received End-Body From Request!");
        req.RemoveItem(ObexHeader::END_OF_BODY);
        req.SetExtendBodyObject(writer);
    }
    SetBusy(session, false);
    ObexPrivateServer::HandlePutRequest(session, req);
}

void ObexPrivateMpServer::HandleTransportDataBusy(ObexServerSession &session, uint8_t isBusy)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto &sendObject = session.GetSendObject();
    if (!sendObject) {
        return;
    }
    bool oldBusy = sendObject->IsBusy();
    bool newBusy = (isBusy == 0x01);
    sendObject->SetBusy(newBusy);
    // busy -> not busy , srm data is sending
    if (oldBusy && !newBusy && sendObject->IsSrmSending()) {
        OBEX_LOG_DEBUG("Transport change to not busy, continue send srm response data.");
        session.SendSrmResponse();
    }
}

void ObexPrivateMpServer::HandleGetRequest(ObexServerSession &session, ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    auto &sendObject = session.GetSendObject();
    if (!sendObject) {
        ObexPrivateServer::HandleGetRequest(session, req);
        return;
    }
    OBEX_LOG_DEBUG("HandleGetRequest:Server Has ObexServerSendObject");
    if (sendObject->IsSrmEnable()) {
        if (sendObject->IsSrmWait() && !req.GetItemSrmp()) {
            OBEX_LOG_DEBUG("HandleGetRequest:Server srmp wait end!");
            sendObject->SetSrmWait(false);
        }
    }
    sendObject->SetStartBodyResp(req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::GET_FINAL));
    if (sendObject->IsSrmEnable() && !sendObject->IsSrmWait()) {
        if (req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::GET_FINAL)) {
            OBEX_LOG_DEBUG("HandleGetRequest: wait the GET request + Final Bit has been issued");
            // GEOP:4.6 Using Single Response Mode
            // GET operations with SRM enabled shall not send a GET response until the GET
            // request + Final Bit has been issued.
            return;
        }
        if (sendObject->IsSrmSending()) {
            OBEX_LOG_ERROR("HandleGetRequest:other get srm is in progress, ignores this get request!");
            return;
        }
        int ret = session.SendSrmResponse();
        if (ret != 0) {
            session.FreeSendObject();
        }
    } else {
        OBEX_LOG_DEBUG("Send next get multi-packet responses");
        auto nextReqHdr = sendObject->GetNextRespHeader();
        if (nextReqHdr == nullptr) {
            OBEX_LOG_ERROR("nextReqHdr is null!");
            nextReqHdr = ObexHeader::CreateResponse(ObexRspCode::INTERNAL_SERVER_ERROR);
        }
        int ret = session.SendResponse(*nextReqHdr);
        if (ret != 0) {
            session.FreeSendObject();
            observer_.OnError(ret, "send response fail");
        }
        if (nextReqHdr->GetFieldCode() != static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
            OBEX_LOG_DEBUG("session.sendObject reset to null");
            session.FreeSendObject();
            SetBusy(session, false);
        }
    }
}

void ObexPrivateMpServer::HandleAbortRequest(ObexServerSession &session, ObexHeader &req)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.FreeSendObject();
    session.FreeReceivedObject();
    auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    int ret = session.SendResponse(*header);
    SetBusy(session, false);
    OBEX_LOG_DEBUG("send abort response %{public}d", ret);
}
}  // namespace bluetooth
}  // namespace OHOS