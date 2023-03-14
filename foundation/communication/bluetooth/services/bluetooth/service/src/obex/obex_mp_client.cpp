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

#include "obex_mp_client.h"
#include "log.h"
namespace OHOS {
namespace bluetooth {
ObexMpClient::ObexMpClient(
    const ObexClientConfig &config, ObexClientObserver &observer, utility::Dispatcher &dispatcher)
    : ObexClient(config, observer, dispatcher)
{}

int ObexMpClient::Put(const ObexHeader &req)
{
    return ObexClient::Put(req);
}

int ObexMpClient::Put(const ObexHeader &req, std::shared_ptr<ObexBodyObject> reader)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (!CheckBeforeRequest(req.GetFieldCode())) {
        return -1;
    }
    if (req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::PUT) &&
        req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::PUT_FINAL)) {
        OBEX_LOG_ERROR("Error: Opcode is wrong.");
        return -1;
    }
    if (req.HasHeader(ObexHeader::BODY) || req.HasHeader(ObexHeader::END_OF_BODY)) {
        OBEX_LOG_ERROR("Error: Can't include body/end-of-body header in this method.");
        return -1;
    }

    uint16_t mtu = clientSession_->GetMaxPacketLength();
    OBEX_LOG_DEBUG("Create ObexClientSendObject mtu=%{public}d", int(mtu));

    auto &sendObject = clientSession_->CreateSendObject(req, reader, mtu);
    auto sendReq = sendObject->GetNextReqHeader(isSupportSrm_);
    if (sendReq == nullptr) {
        clientSession_->FreeSendObject();
        return -1;
    }
    int ret = SendRequest(*sendReq);
    if (ret != 0) {
        clientSession_->FreeSendObject();
    } else {
        SetBusy(true);
    }
    return ret;
}

int ObexMpClient::Get(const ObexHeader &req)
{
    return ObexClient::Get(req);
}

int ObexMpClient::Get(const ObexHeader &req, std::shared_ptr<ObexBodyObject> writer, int srmpCount)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (!CheckBeforeRequest(req.GetFieldCode())) {
        return -1;
    }
    if (req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::GET) &&
        req.GetFieldCode() != static_cast<uint8_t>(ObexOpeId::GET_FINAL)) {
        OBEX_LOG_ERROR("Error: Opcode is wrong.");
        return -1;
    }
    auto &recvObj = clientSession_->CreateReceivedObject(req, writer, isSupportSrm_, srmpCount);
    const auto &sendReq = recvObj->GetFirstReqHeader();
    int ret = SendRequest(sendReq);
    if (ret != 0) {
        clientSession_->FreeReceivedObject();
    } else {
        SetBusy(true);
    }
    return ret;
}

int ObexMpClient::SetPath(uint8_t flag, const std::u16string &path)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    return ObexClient::SetPath(flag, path);
}

int ObexMpClient::SetPath(const std::vector<std::u16string> &paths)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (paths.size() == 0) {
        OBEX_LOG_ERROR("Error: SetPath Job's size is 0.");
        return -1;
    }
    if (clientSession_->GetSetPathObject() != nullptr && !clientSession_->GetSetPathObject()->IsDone()) {
        OBEX_LOG_ERROR("Error: Another SetPath Job is being processing.");
        return -1;
    }
    clientSession_->CreateSetPathObject(paths);
    // first goto root
    return ObexClient::SetPath(OBEX_SETPATH_NOCREATE, u"");
}

void ObexMpClient::PutDataAvailable(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (isWaitingSendAbort_) {
        OBEX_LOG_DEBUG("PutDataAvailable: skip received data, send aborted request!");
        isProcessing_ = false;
        SendAbortRequest();
        clientSession_->FreeSendObject();
        return;
    }
    if (isAbortSended_) {
        OBEX_LOG_DEBUG("PutDataAvailable: skip received data.");
        clientSession_->FreeSendObject();
        return;
    }
    HandlePutData(resp);
}

void ObexMpClient::HandlePutData(const ObexHeader &resp)
{
    auto &sendObject = clientSession_->GetSendObject();
    if (sendObject != nullptr && resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        if (isSupportSrm_ && !sendObject->IsSrmSending()) {
            if (resp.HasHeader(ObexHeader::SRM)) {
                bool srmEnable = resp.GetItemSrm();
                sendObject->SetSrmEnable(srmEnable);
                OBEX_LOG_DEBUG("PutDataAvailable: srm mode enable.");
            }
            if (resp.HasHeader(ObexHeader::SRMP)) {
                bool srmWait = resp.GetItemSrmp();
                sendObject->SetSrmWait(srmWait);
                OBEX_LOG_DEBUG("PutDataAvailable: srmp waiting.");
            } else if (sendObject->IsSrmEnable()) {
                sendObject->SetSrmWait(false);
                OBEX_LOG_DEBUG("PutDataAvailable: srmp wait done.");
            }
        }
        if (sendObject->IsSrmEnable() && !sendObject->IsSrmWait()) {
            OBEX_LOG_DEBUG("PutDataAvailable: send put with srm. START");
            ProcessSendPutWithSrm();
        } else if (sendObject->IsDone()) {
            clientSession_->FreeSendObject();
            clientObserver_.OnActionCompleted(*this, resp);
        } else {
            auto nexReqHdr = sendObject->GetNextReqHeader();
            isProcessing_ = false;
            int ret = SendRequest(*nexReqHdr);
            if (ret != 0) {
                clientSession_->FreeSendObject();
                clientObserver_.OnTransportFailed(*this, ret);
            }
        }
        return;
    }
    clientSession_->FreeSendObject();
    ObexClient::PutDataAvailable(resp);
}

int ObexMpClient::ProcessSendPutWithSrm()
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (isWaitingSendAbort_) {
        OBEX_LOG_DEBUG("ProcessSendPutWithSrm: send abort request, break srm send!");
        isProcessing_ = false;
        SendAbortRequest();
        clientSession_->FreeSendObject();
        return 0;
    }
    if (isAbortSended_) {
        OBEX_LOG_DEBUG("ProcessSendPutWithSrm: break srm send");
        clientSession_->FreeSendObject();
        return 0;
    }
    auto &sendObject = clientSession_->GetSendObject();
    if (!sendObject) {
        return -1;
    }
    sendObject->SetSrmSending();
    if (sendObject->IsBusy()) {
        OBEX_LOG_DEBUG("ProcessSendPutWithSrm: Transport is busy, waiting...");
        return 0;
    }
    auto nexReqHdr = sendObject->GetNextReqHeader();
    if (nexReqHdr == nullptr) {
        if (!sendObject->IsDone()) {
            clientSession_->FreeSendObject();
            clientObserver_.OnTransportFailed(*this, -1);
        }
        return -1;
    }
    isProcessing_ = false;
    int ret = SendRequest(*nexReqHdr);
    if (ret != 0) {
        clientSession_->FreeSendObject();
        clientObserver_.OnTransportFailed(*this, ret);
        return ret;
    }
    if (sendObject->IsDone()) {
        OBEX_LOG_DEBUG("ProcessSendPutWithSrm: DONE!");
        return 0;
    }
    OBEX_LOG_DEBUG("ProcessSendPutWithSrm: CONTINUE");
    dispatcher_.PostTask(std::bind(&ObexMpClient ::ProcessSendPutWithSrm, this));
    return 0;
}

void ObexMpClient::GetDataAvailable(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (isWaitingSendAbort_) {
        OBEX_LOG_DEBUG("GetDataAvailable: skip received data, send aborted request!");
        SendAbortRequest();
        clientSession_->FreeReceivedObject();
        return;
    }
    if (isAbortSended_) {
        clientSession_->FreeReceivedObject();
        OBEX_LOG_DEBUG("GetDataAvailable: skip received data.");
        return;
    }
    const auto &recvObj = clientSession_->GetReceivedObject();

    if (recvObj != nullptr && resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        OBEX_LOG_DEBUG("GetDataAvailable: recvObj != null, Received CONTINUE Response!");
        ProcessGetContinueData(resp);
        return;
    }
    if (recvObj != nullptr && resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        OBEX_LOG_DEBUG("GetDataAvailable: recvObj != null, Received SUCCESS Response!");
        ProcessGetSuccessData(resp);
        return;
    }
    if (recvObj != nullptr) {
        OBEX_LOG_DEBUG("GetDataAvailable: recvObj != null, Received Error Response! 0x%02X", resp.GetFieldCode());
        // OBEX ERROR CODE
        clientSession_->FreeReceivedObject();
        ObexClient::GetDataAvailable(resp);
        return;
    }
    clientSession_->FreeReceivedObject();
    ObexClient::GetDataAvailable(resp);
}

void ObexMpClient::ProcessGetContinueData(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    auto &recvObj = clientSession_->GetReceivedObject();

    recvObj->CacheResp(resp);
    auto body = resp.GetItemBody();
    if (body != nullptr) {
        recvObj->AppendBody(body->GetBytes().get(), body->GetHeaderDataSize());
    }
    bool sendNextReq = true;
    if (recvObj->IsSupportSrmMode()) {
        if (!recvObj->IsSendNextReq()) {
            sendNextReq = false;
        } else {
            if (resp.HasHeader(ObexHeader::SRM)) {
                recvObj->SetSrmEnable(resp.GetItemSrm());
            }
            if (resp.HasHeader(ObexHeader::SRMP)) {
                recvObj->SetSrmWait(resp.GetItemSrmp());
            } else {
                recvObj->SetSrmWait(false);
            }
            if (!recvObj->IsSendNextReq()) {
                OBEX_LOG_DEBUG(
                    "GetDataAvailable: Srm mode is enabled. not send next request util received succeeded response!");
                sendNextReq = false;
            }
        }
    }
    if (sendNextReq) {
        OBEX_LOG_DEBUG("GetDataAvailable: send next get request");
        int ret = SendRequest(*recvObj->GetContinueReqHeader());
        if (ret != 0) {
            OBEX_LOG_ERROR("GetDataAvailable: Send Continue Next request fail!");
        }
    }
}

void ObexMpClient::ProcessGetSuccessData(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    auto &recvObj = clientSession_->GetReceivedObject();
    recvObj->CacheResp(resp);
    auto body = resp.GetItemEndBody();
    if (body != nullptr) {
        recvObj->AppendBody(body->GetBytes().get(), body->GetHeaderDataSize());
    }
    recvObj->SetDone(true);
    auto newRespHeader = recvObj->GetFirstRespHeader();
    if (newRespHeader == nullptr) {
        OBEX_LOG_ERROR("[%{public}s] Call %{public}s, newRespHeader is nullptr", GetClientId().c_str(), __PRETTY_FUNCTION__);
        return;
    }
    auto newResp = *newRespHeader;
    newResp.SetRespCode(resp.GetFieldCode());
    newResp.SetExtendBodyObject(recvObj->GetBodyWriter());
    ObexClient::GetDataAvailable(newResp);
    clientSession_->FreeReceivedObject();
}

void ObexMpClient::SetPathDataAvailable(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    if (resp.GetFieldCode() == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        auto &setPathObject = clientSession_->GetSetPathObject();
        if (setPathObject != nullptr && !setPathObject->IsDone()) {
            int ret = ObexClient::SetPath(OBEX_SETPATH_NOCREATE, setPathObject->GetNextPath());
            if (ret != 0) {
                clientSession_->FreeSetPathObject();
                clientObserver_.OnTransportFailed(*this, -1);
            }
            return;
        }
    }
    clientSession_->FreeSetPathObject();
    ObexClient::SetPathDataAvailable(resp);
}

void ObexMpClient::AbortDataAvailable(const ObexHeader &resp)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s", GetClientId().c_str(), __PRETTY_FUNCTION__);
    isWaitingSendAbort_ = false;
    isAbortSended_ = false;
    clientSession_->FreeReceivedObject();
    clientSession_->FreeSendObject();
    ObexClient::AbortDataAvailable(resp);
}

void ObexMpClient::HandleTransportDataBusy(uint8_t isBusy)
{
    OBEX_LOG_INFO("[%{public}s] Call %{public}s, isBusy %u", GetClientId().c_str(), __PRETTY_FUNCTION__, isBusy);
    auto &sendObject = clientSession_->GetSendObject();
    if (!sendObject) {
        return;
    }
    bool oldBusy = sendObject->IsBusy();
    bool newBusy = (isBusy == 0x01);
    sendObject->SetBusy(newBusy);
    // busy -> not busy , srm data is sending
    if (oldBusy && !newBusy && sendObject->IsSrmSending()) {
        OBEX_LOG_DEBUG("Transport change to not busy, continue send srm request data.");
        ProcessSendPutWithSrm();
    }
}
}  // namespace bluetooth
}  // namespace OHOS