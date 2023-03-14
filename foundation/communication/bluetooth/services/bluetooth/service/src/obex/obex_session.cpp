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

#include "obex_session.h"
#include <memory>
#include "log.h"
#include "obex_utils.h"
#include "obex_socket_transport.h"

namespace OHOS {
namespace bluetooth {
ObexServerSession::ObexServerSession(ObexTransport &transport, bool isSupportSrm, utility::Dispatcher &dispatcher,
    std::function<int(ObexServerSession &)> removeFun, std::function<void(ObexServerSession &, bool)> setBusyFun)
    : ObexSession(transport.GetRemoteAddress()),
      transport_(transport),
      isSupportSrm_(isSupportSrm),
      dispatcher_(dispatcher),
      removeFun_(std::move(removeFun)),
      setBusyFun_(std::move(setBusyFun))
{}

int ObexServerSession::Disconnect()
{
    if (invalid_) {
        return -1;
    }
    return removeFun_(*this);
}

int ObexServerSession::SendSimpleResponse(ObexRspCode rspCode) const
{
    if (invalid_) {
        return -1;
    }
    return SendResponse(*ObexHeader::CreateResponse(rspCode));
}

int ObexServerSession::SendResponse(ObexHeader &resp) const
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    if (invalid_) {
        return -1;
    }
    // Reset connect response's mtu
    if (resp.GetFieldMaxPacketLength() != nullptr) {
        resp.SetFieldMaxPacketLength(maxPacketLength_);
    }
    if (resp.GetFieldPacketLength() > maxPacketLength_) {
        OBEX_LOG_ERROR("Error: Response packet length[%{public}d]>mtu[%{public}d].",
            int(resp.GetFieldPacketLength()), int(maxPacketLength_));
        return -1;
    }
    bool ret = transport_.Write(resp.Build()->GetPacket());
    if (ret) {
        return 0;
    }
    OBEX_LOG_ERROR("SendResponse error: Write ret = %{public}d", ret);
    return -1;
}

int ObexServerSession::SendGetResponse(const ObexHeader &req, ObexHeader &resp, std::shared_ptr<ObexBodyObject> reader)
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    if (invalid_) {
        return -1;
    }
    if (resp.HasHeader(ObexHeader::BODY) || resp.HasHeader(ObexHeader::END_OF_BODY)) {
        OBEX_LOG_ERROR("Error: Can't include body/end-of-body header in this method.");
        return -1;
    }
    if (resp.GetFieldCode() != static_cast<uint8_t>(ObexRspCode::SUCCESS) &&
        resp.GetFieldCode() != static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        return SendResponse(resp);
    }
    if (!reader) {
        if (isSupportSrm_ && req.GetItemSrm()) {
            resp.AppendItemSrm(true);
        }
        return SendResponse(resp);
    }
    sendObject_ = std::make_unique<ObexServerSendObject>(req, resp, reader, maxPacketLength_, isSupportSrm_);

    sendObject_->SetStartBodyResp(req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::GET_FINAL));
    auto resp2 = sendObject_->GetNextRespHeader();
    if (resp2 == nullptr) {
        OBEX_LOG_ERROR("SendGetResponse: resp is null!");
        sendObject_ = nullptr;
        return -1;
    }
    int ret = SendResponse(*resp2);
    if (ret != 0) {
        sendObject_ = nullptr;
        return ret;
    }
    if (resp2->GetFieldCode() != static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        OBEX_LOG_DEBUG("Send done with first packet, reset sendObject_ to null");
        sendObject_ = nullptr;
        return ret;
    }
    dispatcher_.PostTask(std::bind(setBusyFun_, std::ref(*this), true));
    if (sendObject_->IsSupportSrmMode() && sendObject_->IsSrmEnable() && !sendObject_->IsSrmWait()) {
        ret = SendSrmResponse();
    }
    return ret;
}

int ObexServerSession::ProcessSendSrmResponse()
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    if (invalid_) {
        return -1;
    }
    if (!sendObject_) {
        return -1;
    }
    sendObject_->SetSrmSending();
    if (sendObject_->IsBusy()) {
        OBEX_LOG_DEBUG("ProcessSendSrmResponse: Transport is busy, waiting...");
        return 0;
    }
    auto nextReqHdr = sendObject_->GetNextRespHeader();
    if (nextReqHdr == nullptr) {
        OBEX_LOG_ERROR("ProcessSendSrmResponse: nextReqHdr is null!");
        if (!sendObject_->IsDone()) {
            sendObject_ = nullptr;
        }
        nextReqHdr = ObexHeader::CreateResponse(ObexRspCode::INTERNAL_SERVER_ERROR);
    }
    int ret = SendResponse(*nextReqHdr);
    if (ret != 0) {
        sendObject_ = nullptr;
        return ret;
    }
    if (nextReqHdr->GetFieldCode() == static_cast<uint8_t>(ObexRspCode::CONTINUE)) {
        OBEX_LOG_DEBUG("ProcessSendSrmResponse: CONTINUE");
        dispatcher_.PostTask(std::bind(&ObexServerSession::ProcessSendSrmResponse, this));
    } else if (nextReqHdr->GetFieldCode() == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        sendObject_ = nullptr;
        OBEX_LOG_DEBUG("ProcessSendSrmResponse: Server send with SRM MODE END");
        setBusyFun_(*this, false);
    } else {
        sendObject_ = nullptr;
        OBEX_LOG_ERROR("ProcessSendSrmResponse: SRM MODE END WITH RESPCODE 0x%02X", nextReqHdr->GetFieldCode());
        setBusyFun_(*this, false);
    }
    return ret;
}

int ObexServerSession::SendSrmResponse()
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    if (invalid_) {
        return -1;
    }
    if (!isSupportSrm_) {
        return -1;
    }
    if (!sendObject_) {
        return -1;
    }
    if (!sendObject_->IsSrmEnable() || sendObject_->IsSrmWait()) {
        return -1;
    }
    OBEX_LOG_DEBUG("SendSrmResponse: Server Start send with SRM MODE START");
    return ProcessSendSrmResponse();
}

void ObexServerSession::Invalid()
{
    invalid_ = true;
    receivedObject_ = nullptr;
    sendObject_ = nullptr;
}

ObexSetPathObject::ObexSetPathObject(const std::vector<std::u16string> &paths)
    : paths_(paths), pos_(0)
{}

std::u16string ObexSetPathObject::GetNextPath()
{
    if (pos_ >= paths_.size()) {
        return nullptr;
    }
    return paths_.at(pos_++);
}

bool ObexSetPathObject::IsDone() const
{
    return pos_ >= paths_.size();
}

// ObexClientSendObject
ObexClientSendObject::ObexClientSendObject(
    const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> bodyReader, uint16_t mtu)
    : reqIndex_(0), srmEnable_(false), isDone_(false), mtu_(mtu),
      firstReq_(std::make_unique<ObexHeader>(firstReq)),
      bodyReader_(bodyReader)
{}

bool ObexClientSendObject::SetBodyToHeader(ObexHeader &header, const uint16_t &remainLength)
{
    auto buf = std::make_unique<uint8_t[]>(remainLength);
    int cnt = bodyReader_->Read(buf.get(), remainLength);
    if (cnt < 0) {
        return false;
    }
    if (cnt < remainLength) {
        isDone_ = true;
    }
    if (isDone_) {
        OBEX_LOG_DEBUG("GetNextReqHeader Add End-Body count %{public}d", cnt);
        header.SetFinalBit(true);
        header.AppendItemEndBody(buf.get(), cnt);
    } else {
        OBEX_LOG_DEBUG("GetNextReqHeader Add Body count %{public}d", cnt);
        header.SetFinalBit(false);
        header.AppendItemBody(buf.get(), cnt);
    }
    return true;
}

std::unique_ptr<ObexHeader> ObexClientSendObject::GetNextReqHeader(bool useSrm)
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    if (isDone_) {
        return nullptr;
    }
    std::unique_ptr<ObexHeader> header = nullptr;
    if (reqIndex_ == 0) {
        header = std::make_unique<ObexHeader>(*firstReq_);
    } else {
        header = std::make_unique<ObexHeader>(*firstReq_, true);
        auto connectId = firstReq_->GetItemConnectionId();
        if (connectId != nullptr) {
            header->AppendItemConnectionId(connectId->GetWord());
        }
    }
    if (useSrm) {
        if (!header->HasHeader(ObexHeader::SRM)) {
            header->AppendItemSrm(useSrm);
        }
    }
    uint16_t pktLen = header->GetFieldPacketLength();
    if (reqIndex_ == 0 && pktLen == mtu_ - ObexHeader::MIN_PACKET_LENGTH) {
        reqIndex_++;
        return header;
    }
    if (pktLen > mtu_) {
        OBEX_LOG_ERROR("ObexClientSendObject:pktLen[%{public}d] can't large then mtu_[%{public}d]", pktLen, mtu_);
        return nullptr;
    }

    if ((reqIndex_ != 0) && (pktLen >= mtu_ - ObexHeader::MIN_PACKET_LENGTH)) {
        OBEX_LOG_ERROR("ObexClientSendObject:can't send next packet, reqIndex_[%{public}d]!=0 && pktLen[%{public}d]>=mtu_[%{public}d] - 3",
            reqIndex_,
            int(pktLen),
            int(mtu_));
        return nullptr;
    }
    uint16_t remainLength = mtu_ - pktLen - ObexHeader::HDR_BYTES_PREFIX_LENGTH;  // 3: body header+len
    if (!SetBodyToHeader(*header, remainLength)) {
        return nullptr;
    }
    reqIndex_++;
    return header;
}

bool ObexClientSendObject::IsDone() const
{
    return isDone_;
}

void ObexClientSendObject::SetSrmEnable(bool srmEnable)
{
    srmEnable_ = srmEnable;
}

bool ObexClientSendObject::IsSrmEnable() const
{
    return srmEnable_;
}

void ObexClientSendObject::SetSrmWait(bool wait)
{
    srmWait_ = wait;
}

bool ObexClientSendObject::IsSrmWait() const
{
    return srmWait_;
}

void ObexClientSendObject::SetSrmSending()
{
    isSrmSending_ = true;
}

bool ObexClientSendObject::IsSrmSending() const
{
    return isDone_ ? false : isSrmSending_;
}

void ObexClientSendObject::SetBusy(bool isBusy)
{
    isBusy_ = isBusy;
}

bool ObexClientSendObject::IsBusy() const
{
    return isBusy_;
}

// ObexClientReceivedObject
ObexClientReceivedObject::ObexClientReceivedObject(
    const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> writer, bool supportSrm, int srmpCount)
    : firstReq_(std::make_unique<ObexHeader>(firstReq)), bodyWriter_(writer)
{
    supportSrmMode_ = supportSrm;
    srmpCount_ = srmpCount;
    srmEnable_ = false;
    isDone_ = false;
    if (supportSrmMode_) {
        if (!firstReq_->HasHeader(ObexHeader::SRM)) {
            firstReq_->AppendItemSrm(supportSrmMode_);
        }
        if (srmpCount_ < 1) {
            firstReq_->RemoveItem(ObexHeader::SRMP);
        } else {
            firstReq_->AppendItemSrmp();
            srmpCount_--;
            srmpSended_ = true;
        }
    } else {
        firstReq_->RemoveItem(ObexHeader::SRM);
        firstReq_->RemoveItem(ObexHeader::SRMP);
    }
}

ObexHeader &ObexClientReceivedObject::GetFirstReqHeader() const
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    return *firstReq_;
}

const ObexHeader *ObexClientReceivedObject::GetFirstRespHeader() const
{
    if (firstResp_) {
        return firstResp_.get();
    }
    return nullptr;
}

void ObexClientReceivedObject::CacheResp(const ObexHeader &resp)
{
    if (!firstResp_) {
        firstResp_ = std::make_unique<ObexHeader>(resp);
        firstResp_->RemoveItem(ObexHeader::SRM);
        firstResp_->RemoveItem(ObexHeader::SRMP);
        firstResp_->RemoveItem(ObexHeader::BODY);
        firstResp_->SetExtendBodyObject(nullptr);
    }
}

std::unique_ptr<ObexHeader> ObexClientReceivedObject::GetContinueReqHeader()
{
    OBEX_LOG_DEBUG("Call %{public}s", __PRETTY_FUNCTION__);
    auto header = std::make_unique<ObexHeader>(*firstReq_, true);
    auto connectId = firstReq_->GetItemConnectionId();
    if (connectId != nullptr) {
        header->AppendItemConnectionId(connectId->GetWord());
    }
    if (srmpCount_ > 0) {
        header->AppendItemSrmp();
        srmpCount_--;
        srmpSended_ = true;
    } else {
        srmpSended_ = false;
    }
    header->SetFinalBit(true);
    return header;
}

std::shared_ptr<ObexBodyObject> &ObexClientReceivedObject::GetBodyWriter()
{
    return bodyWriter_;
}

void ObexClientReceivedObject::AppendBody(const uint8_t *body, const uint16_t bodyLen)
{
    bodyWriter_->Write(body, bodyLen);
    receivedSize_ += bodyLen;
    OBEX_LOG_DEBUG("ObexClientReceivedObject:ReceivedSize + %{public}d = %zu", bodyLen, receivedSize_);
}

bool ObexClientReceivedObject::IsDone() const
{
    return isDone_;
}

void ObexClientReceivedObject::SetDone(bool done)
{
    isDone_ = done;
}

void ObexClientReceivedObject::SetSrmEnable(bool srmEnable)
{
    srmEnable_ = srmEnable;
}

bool ObexClientReceivedObject::IsSrmEnable() const
{
    return srmEnable_;
}

void ObexClientReceivedObject::SetSrmWait(bool wait)
{
    srmWait_ = wait;
}

bool ObexClientReceivedObject::IsSrmWait() const
{
    return srmWait_;
}

bool ObexClientReceivedObject::IsSupportSrmMode() const
{
    return supportSrmMode_;
}

bool ObexClientReceivedObject::IsSendNextReq() const
{
    if (IsSrmEnable() && !IsSrmWait() && !srmpSended_) {
        return false;
    }
    return true;
}

// ObexServerReceivedObject
ObexServerReceivedObject::ObexServerReceivedObject(const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> writer)
    : baseReq_(std::make_unique<ObexHeader>(firstReq))
{
    baseReq_->RemoveItem(ObexHeader::SRM);
    baseReq_->RemoveItem(ObexHeader::SRMP);
    baseReq_->RemoveItem(ObexHeader::BODY);
    baseReq_->SetExtendBodyObject(nullptr);
    baseReq_->SetFinalBit(true);
    writer_ = writer;
}

void ObexServerReceivedObject::AppendBody(const uint8_t *body, const uint16_t bodyLen)
{
    writer_->Write(body, bodyLen);
    receivedSize_ += bodyLen;
    OBEX_LOG_DEBUG("ObexServerReceivedObject:ReceivedSize + %{public}d = %zu", bodyLen, receivedSize_);
}

std::shared_ptr<ObexBodyObject> &ObexServerReceivedObject::GetWriter()
{
    return writer_;
}

std::unique_ptr<ObexHeader> ObexServerReceivedObject::CreateReceivedHeader()
{
    auto header = std::make_unique<ObexHeader>(*baseReq_);
    header->SetExtendBodyObject(writer_);
    return header;
}

void ObexServerReceivedObject::SetSrmEnable(bool srmEnable)
{
    srmEnable_ = srmEnable;
}

bool ObexServerReceivedObject::IsSrmEnable() const
{
    return srmEnable_;
}

void ObexServerReceivedObject::SetSrmWait(bool wait)
{
    srmWait_ = wait;
}

bool ObexServerReceivedObject::IsSrmWait() const
{
    return srmWait_;
}

void ObexServerReceivedObject::SetSrmReceiving()
{
    isSrmReceiving_ = true;
}

bool ObexServerReceivedObject::IsSrmReceiving() const
{
    return isSrmReceiving_;
}

ObexServerSendObject::ObexServerSendObject(const ObexHeader &firstReq, const ObexHeader &firstResp,
    std::shared_ptr<ObexBodyObject> bodyReader, uint16_t mtu, bool supportSrm)
    : firstReq_(std::make_unique<ObexHeader>(firstReq)), firstResp_(std::make_unique<ObexHeader>(firstResp))
{
    firstResp_->RemoveItem(ObexHeader::SRM);
    firstResp_->RemoveItem(ObexHeader::SRMP);
    bodyReader_ = bodyReader;
    mtu_ = mtu;
    reqIndex_ = 0;
    srmEnable_ = false;
    isDone_ = false;
    supportSrmMode_ = supportSrm;
}

void ObexServerSendObject::SetSrmParam(ObexHeader &header)
{
    if (supportSrmMode_) {
        if (reqIndex_ == 0 && firstReq_->HasHeader(ObexHeader::SRM)) {
            OBEX_LOG_DEBUG("ObexServerSendObject send response with SRM set to true");
            SetSrmEnable(firstReq_->GetItemSrm());
            if (IsSrmEnable()) {
                header.AppendItemSrm(true);
            }
        }
        if (reqIndex_ == 0 && firstReq_->HasHeader(ObexHeader::SRMP)) {
            OBEX_LOG_DEBUG("ObexServerSendObject srmp_wait Set to true");
            SetSrmWait(firstReq_->GetItemSrmp());
        }
    }
}

bool ObexServerSendObject::SetBodyToHeader(ObexHeader &header, const uint16_t &remainLength)
{
    auto buf = std::make_unique<uint8_t[]>(remainLength);
    int cnt = bodyReader_->Read(buf.get(), remainLength);
    if (cnt < 0) {
        return false;
    }
    if (cnt < remainLength) {
        isDone_ = true;
    }
    if (isDone_) {
        OBEX_LOG_DEBUG("GetNextRespHeader Add End-Body count %{public}d", cnt);
        header.SetRespCode(static_cast<uint8_t>(ObexRspCode::SUCCESS));
        header.AppendItemEndBody(buf.get(), cnt);
    } else {
        OBEX_LOG_DEBUG("GetNextRespHeader Add Body count %{public}d", cnt);
        header.SetRespCode(static_cast<uint8_t>(ObexRspCode::CONTINUE));
        header.AppendItemBody(buf.get(), cnt);
    }
    return true;
}

std::unique_ptr<ObexHeader> ObexServerSendObject::GetNextRespHeader()
{
    if (isDone_) {
        return nullptr;
    }
    std::unique_ptr<ObexHeader> header = nullptr;
    if (reqIndex_ == 0) {
        header = std::make_unique<ObexHeader>(*firstResp_);
    } else {
        header = std::make_unique<ObexHeader>(*firstResp_, true);
    }
    // Set SRM
    SetSrmParam(*header);

    uint16_t pktLen = header->GetFieldPacketLength();
    if (reqIndex_ == 0 && pktLen == mtu_ - ObexHeader::MIN_PACKET_LENGTH) {
        reqIndex_++;
        return header;
    }
    if (pktLen > mtu_) {
        OBEX_LOG_ERROR("ObexServerSendObject:pktLen[%{public}d] can't large then mtu[%{public}d]", pktLen, mtu_);
        return nullptr;
    }

    if (reqIndex_ != 0 && pktLen >= mtu_ - ObexHeader::MIN_PACKET_LENGTH) {
        OBEX_LOG_ERROR("ObexServerSendObject:can't send next packet, reqIndex[%{public}d] != 0 && pktLen[%{public}d] >= mtu[%{public}d] - 3",
            reqIndex_,
            pktLen,
            mtu_);
        return nullptr;
    }
    if (!startBodyResp_) {
        header->SetRespCode(static_cast<uint8_t>(ObexRspCode::CONTINUE));
    } else {
        uint16_t remainLength = mtu_ - pktLen - ObexHeader::HDR_BYTES_PREFIX_LENGTH;  // 3: body header+len
        if (!SetBodyToHeader(*header, remainLength)) {
            return nullptr;
        }
    }
    reqIndex_++;
    return header;
}

bool ObexServerSendObject::IsDone() const
{
    return isDone_;
}

void ObexServerSendObject::SetSrmEnable(bool srmEnable)
{
    srmEnable_ = srmEnable;
}

bool ObexServerSendObject::IsSrmEnable() const
{
    return srmEnable_;
}

void ObexServerSendObject::SetSrmWait(bool wait)
{
    srmWait_ = wait;
}

bool ObexServerSendObject::IsSrmWait() const
{
    return srmWait_;
}

void ObexServerSendObject::SetStartBodyResp(bool start)
{
    startBodyResp_ = start;
}

void ObexServerSendObject::SetSrmSending()
{
    isSrmSending_ = true;
}

bool ObexServerSendObject::IsSrmSending() const
{
    return isDone_ ? false : isSrmSending_;
}

void ObexServerSendObject::SetBusy(bool isBusy)
{
    isBusy_ = isBusy;
}

bool ObexServerSendObject::IsBusy() const
{
    return isBusy_;
}

bool ObexServerSendObject::IsSupportSrmMode() const
{
    return supportSrmMode_;
}

ObexSession::ObexSession(const RawAddress &remoteAddr) : remoteAddr_(remoteAddr)
{}

ObexClientSession::ObexClientSession(const RawAddress &remoteAddr) : ObexSession(remoteAddr)
{}
}  // namespace bluetooth
}  // namespace OHOS