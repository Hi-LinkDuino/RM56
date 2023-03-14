/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "opp_obex_client.h"

#include <codecvt>
#include "bt_def.h"
#include "log.h"
#include "log_util.h"
#include "opp_service.h"

namespace OHOS {
namespace bluetooth {
OppSendFileBodyObject::OppSendFileBodyObject(const std::string &file)
{
    OpenFile(file);
}

OppSendFileBodyObject::~OppSendFileBodyObject()
{
    try {
        Close();
    } catch(std::exception &e) {
        LOG_ERROR("[OPP OBEX SERVER]%{public}s():Catch exception %{public}s", __FUNCTION__, e.what());
    }
}

void OppSendFileBodyObject::OpenFile(const std::string &file)
{
    ifs_.open(file, std::ios::in);
    if (!ifs_.is_open()) {
        HILOGE("[OPP OBEX CLIENT] file open failed");
        return;
    } else {
        HILOGI("[OPP OBEX CLIENT] file=%{public}s  opened.", file.c_str());
    }
    ifs_.seekg(0, std::ios::end);
    fileSize_ = static_cast<size_t>(ifs_.tellg());
    ifs_.seekg(0, std::ios::beg);
}

size_t OppSendFileBodyObject::Read(uint8_t *buf, size_t bufLen)
{
    size_t readSize = bufLen;
    size_t remainSize = fileSize_ - fileSendSize_;
    if (remainSize < readSize) {
        readSize = remainSize;
    }
    ifs_.read(reinterpret_cast<char*>(buf), readSize);
    fileSendSize_ += readSize;
    return readSize;
}

size_t OppSendFileBodyObject::Write(const uint8_t *buf, size_t bufLen)
{
    return bufLen;
}

size_t OppSendFileBodyObject::GetFileSize() const
{
    return fileSize_;
}

size_t OppSendFileBodyObject::GetFileSendSize() const
{
    return fileSendSize_;
}

int OppSendFileBodyObject::Close()
{
    ifs_.close();
    return RET_NO_ERROR;
}

OppObexClient::OppObexClient(const ObexClientConfig &config, utility::Dispatcher &dispatcher)
{
    observer_ = std::make_unique<OppObexObserver>(this);
    client_ = std::make_unique<ObexClient>(config, *observer_, dispatcher);
    if (config.isGoepL2capPSM_ && (config.scn_ != OPP_GOEP_L2CAP_PSM)) {
        ObexClient::RegisterL2capLPsm(config.scn_);
        lpsm_ = config.scn_;
        isRegisterL2capLPsm_ = true;
    }
    if (config.isGoepL2capPSM_) {
        isSupportSrm_ = true;
    } else {
        isSupportSrm_ = false;
    }
    address_ = RawAddress::ConvertToString(config.addr_.addr).GetAddress();
    SendFileThread_ = std::make_unique<utility::Dispatcher>(OPP_SEND_FILE_THREAD_NAME);
    SendFileThread_->Initialize();
}

OppObexClient::~OppObexClient()
{
    if (isBusy_) {
        isBusy_ = false;
        std::lock_guard<std::mutex> lock(mutexBusyChanged_);
        cvWaitBusyChanged_.notify_all();
    }
    if (isRegisterL2capLPsm_) {
        ObexClient::DeregisterL2capLPsm(lpsm_);
        lpsm_ = 0;
    }
    if (SendFileThread_ != nullptr) {
        SendFileThread_->Uninitialize();
    }
}

int OppObexClient::Connect(uint32_t fileCount) const
{
    HILOGI("[OPP OBEX CLIENT] start");
    int ret = RET_BAD_STATUS;
    if (observer_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, observer_ is null");
        return ret;
    }
    ObexConnectParams param = {.count = &fileCount};
    ret = client_->Connect(param);

    HILOGI("[OPP OBEX CLIENT] end");
    return ret;
}

int OppObexClient::Disconnect(bool withObexReq) const
{
    HILOGI("[OPP OBEX CLIENT] start");
    int ret = RET_BAD_STATUS;
    if (client_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, client_ is null");
        return ret;
    }
    ret = client_->Disconnect(withObexReq);
    HILOGI("[OPP OBEX CLIENT] end");
    return ret;
}

int OppObexClient::CancelSendFile()
{
    HILOGI("[OPP OBEX CLIENT] start");
    sendAbort_ = true;
    HILOGI("[OPP OBEX CLIENT] end");
    return RET_NO_ERROR;
}

int OppObexClient::SendFile(IOppTransferInformation fileInfo)
{
    HILOGI("[OPP OBEX CLIENT] start");
    int ret = RET_BAD_STATUS;
    if (client_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, client_ is null");
        return ret;
    }
    if (fileInfo.GetTotalBytes() <= 0) {
        return RET_BAD_STATUS;
    }
    auto reqHeader = ObexHeader::CreateRequest(ObexOpeId::PUT);
    if (connectionId_ != 0) {
        reqHeader->AppendItemConnectionId(connectionId_);
    }
    reqHeader->AppendItemName(StringToU16string(fileInfo.GetFileName()));
    reqHeader->AppendItemType(fileInfo.GetFileType());
    reqHeader->AppendItemLength(fileInfo.GetTotalBytes());
    if (isSupportSrm_) {
        reqHeader->AppendItemSrm(true);
    }
    ret = client_->Put(*reqHeader);
    status_ = OPP_OBEX_STATUS_WAITING_CONTINUE;

    fileObject_ = std::make_shared<OppSendFileBodyObject>(fileInfo.GetFilePath());
    auto bodyHeader = ObexHeader::CreateRequest(ObexOpeId::PUT);
    if (connectionId_ != 0) {
        bodyHeader->AppendItemConnectionId(connectionId_);
    }
    uint16_t mtu = client_->GetClientSession().GetMaxPacketLength();
    client_->GetClientSession().CreateSendObject(*bodyHeader, fileObject_, mtu);

    HILOGI("[OPP OBEX CLIENT] end");
    return ret;
}

void OppObexClient::SendFileBody()
{
    int ret = RET_BAD_STATUS;
    if (client_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, client_ is null");
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
        return;
    }
    if (fileObject_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, fileObject_ is null");
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
        return;
    }
    auto &sendObject = client_->GetClientSession().GetSendObject();
    if (sendObject == nullptr) {
        HILOGE("[OPP OBEX CLIENT] end, sendObject is null");
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
        return;
    }
    while (!sendObject->IsDone()) {
        if (!startSendFile_) {
            return;
        }
        if (isBusy_) {
            std::unique_lock<std::mutex> lock(mutexBusyChanged_);
            cvWaitBusyChanged_.wait(lock, [this] { return !isBusy_; });
        }
        if (sendAbort_) {
            client_->Abort();
            client_->GetClientSession().FreeSendObject();
            sendAbort_ = false;
            OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
            return;
        }
        auto sendReq = sendObject->GetNextReqHeader(false);
        if (sendReq == nullptr) {
            client_->GetClientSession().FreeSendObject();
            OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
            return;
        }
        ret = client_->Put(*sendReq);
        if (ret != RET_NO_ERROR) {
            client_->GetClientSession().FreeSendObject();
            OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
            return;
        }
        OppService::GetService()->OnTransferPositionChange(
            address_, static_cast<OppSendFileBodyObject *>(fileObject_.get())->GetFileSendSize());
        if (!sendObject->IsDone() && !isSupportSrm_) {
            status_ = OPP_OBEX_STATUS_WAITING_CONTINUE;
            return;
        }
    }
    status_ = OPP_OBEX_STATUS_WAITING_END;
    client_->GetClientSession().FreeSendObject();
    fileObject_ = nullptr;
}

void OppObexClient::SendOppDisconnected()
{
    OppMessage event(OPP_DISCONNECTED_EVT);
    event.dev_ = address_;
    OppService::GetService()->PostEvent(event);
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::SendOppConnected()
{
    OppMessage event(OPP_CONNECTED_EVT);
    event.dev_ = address_;
    OppService::GetService()->PostEvent(event);
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OnTransferStateChangeFaild(int reason)
{
    startSendFile_ = false;
    OppService::GetService()->OnTransferStateChange(address_, OPP_TRANSFER_STATUS_FAILD, reason);
}

void OppObexClient::OppObexObserver::OnTransportFailed(ObexClient &client, int errCd)
{
    HILOGI("[OPP OBEX CLIENT] start");
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnTransportFailed(client, errCd);
}

void OppObexClient::OnTransportFailed(const ObexClient &client, int errCd)
{
    HILOGI("[OPP OBEX CLIENT] start");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    if (address_ != address) {
        HILOGE("[OPP OBEX CLIENT] end, not current device");
        return;
    }
    HILOGE("[OPP OBEX CLIENT]Transport failed with error %{public}d", errCd);
    connectionId_ = 0;
    SendOppDisconnected();
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OppObexObserver::OnConnected(ObexClient &client, const ObexHeader &resp)
{
    HILOGI("[OPP OBEX CLIENT] start");
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnConnected(client, resp);
}

void OppObexClient::OnConnected(const ObexClient &client, const ObexHeader &resp)
{
    HILOGI("[OPP OBEX CLIENT] start");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    if (address_ != address) {
        HILOGE("[OPP OBEX CLIENT] end, not current device");
        return;
    }

    auto connectId = resp.GetItemConnectionId();
    if (connectId != nullptr) {
        connectionId_ = connectId->GetWord();
    } else {
        connectionId_ = 0;
    }
    SendOppConnected();
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OppObexObserver::OnConnectFailed(ObexClient &client, const ObexHeader &resp)
{
    HILOGI("[OPP OBEX CLIENT] start");
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnConnectFailed(client);
}

void OppObexClient::OnConnectFailed(const ObexClient &client)
{
    HILOGI("[OPP OBEX CLIENT] start");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    if (address_ != address) {
        HILOGE("[OPP OBEX CLIENT] end, not current device");
        return;
    }
    connectionId_ = 0;
    SendOppDisconnected();
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OppObexObserver::OnDisconnected(ObexClient &client)
{
    HILOGI("[OPP OBEX CLIENT] start");
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnDisconnected(client);
}

void OppObexClient::OnDisconnected(const ObexClient &client)
{
    HILOGI("[OPP OBEX CLIENT] start");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    if (address_ != address) {
        HILOGE("[OPP OBEX CLIENT] end, not current device");
        return;
    }
    connectionId_ = 0;
    SendOppDisconnected();
    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OppObexObserver::OnActionCompleted(ObexClient &client, const ObexHeader &resp)
{
    HILOGI("[OPP OBEX CLIENT] start");
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnActionCompleted(client, resp);
}

void OppObexClient::OnActionCompleted(const ObexClient &client, const ObexHeader &resp)
{
    HILOGI("[OPP OBEX CLIENT] start");
    std::string address = client.GetClientSession().GetRemoteAddr().GetAddress();
    if (address_ != address) {
        HILOGE("[OPP OBEX CLIENT] end, not current device");
        return;
    }

    uint8_t respCode = resp.GetFieldCode();
    if ((respCode == static_cast<uint8_t>(ObexRspCode::CONTINUE)) ||
        (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS))) {
        if (status_ == OPP_OBEX_STATUS_WAITING_END) {
            status_ = OPP_OBEX_STATUS_IDLE;
            OppService::GetService()->OnTransferStateChange(
                address_, OPP_TRANSFER_STATUS_SUCCESS, 0);
        } else if (status_ == OPP_OBEX_STATUS_WAITING_CONTINUE) {
            startSendFile_ = true;
            status_ = OPP_OBEX_STATUS_IDLE;
            if (isSupportSrm_ && !resp.HasHeader(ObexHeader::SRM)) {
                isSupportSrm_ = false;
            }
            SendFileThread_->PostTask(std::bind(&OppObexClient::SendFileBody, this));
        }
    } else if (respCode == static_cast<uint8_t>(ObexRspCode::FORBIDDEN)) {
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_FORBIDDEN);
    } else if (respCode == static_cast<uint8_t>(ObexRspCode::NOT_ACCEPTABLE)) {
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_NOT_ACCEPTABLE);
    } else if (respCode == static_cast<uint8_t>(ObexRspCode::UNSUPPORTED_MEDIA_TYPE)) {
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_UNSUPPORTED_TYPE);
    } else {
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
    }

    HILOGI("[OPP OBEX CLIENT] end");
}

void OppObexClient::OppObexObserver::OnBusy(ObexClient &client, bool isBusy)
{
    if (oppObexClient_ == nullptr) {
        HILOGE("[OPP OBEX CLIENT] oppObexClient_ is null");
        return;
    }
    oppObexClient_->OnBusy(client, isBusy);
}

void OppObexClient::OnBusy(ObexClient &client, bool isBusy)
{
    auto &device = client.GetClientSession().GetRemoteAddr();
    HILOGI("[OPP OBEX CLIENT] start, device=%{public}s", GET_ENCRYPT_ADDR(device));
    isBusy_ = isBusy;
    if (!isBusy_) {
        std::lock_guard<std::mutex> lock(mutexBusyChanged_);
        cvWaitBusyChanged_.notify_all();
    }
    HILOGI("[OPP OBEX CLIENT] end, isBusy=%{public}d", isBusy ? 1 : 0);
}

std::u16string OppObexClient::StringToU16string(const std::string &str) const
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(str);
}
}  // namespace bluetooth
}  // namespace OHOS