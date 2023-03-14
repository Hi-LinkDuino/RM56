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

#include "opp_transfer.h"

#include <fstream>
#include "interface_adapter_manager.h"
#include "interface_adapter_classic.h"
#include "log.h"
#include "opp_message.h"
#include "opp_service.h"

namespace OHOS {
namespace bluetooth {
int OppTransfer::currentTransferId_ = 0;

OppTransfer::OppTransfer(const std::string &address, const std::vector<std::string> filePaths,
    const std::vector<std::string> mimeTypes, int direction)
{
    HILOGI("[OPP TRANSFER] start");
    incomingConnectTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::OppTransfer::IncomingConnectTimeout, this));
    incomingFileTimer_ = std::make_unique<utility::Timer>(
        std::bind(&bluetooth::OppTransfer::IncomingFileTimeout, this));
    time(&timeStamp_);
    IAdapterClassic *adapterClassic = (IAdapterClassic *)(IAdapterManager::GetInstance()->
        GetAdapter(ADAPTER_BREDR));
    if (adapterClassic != nullptr) {
        deviceName_ = adapterClassic->GetDeviceName(address);
    }
    auto filePathItr = filePaths.begin();
    auto mimeTypeItr = mimeTypes.begin();
    for (; (filePathItr != filePaths.end() && (mimeTypeItr != mimeTypes.end()));
        ++filePathItr, ++mimeTypeItr) {
        IOppTransferInformation transferInfo;
        transferInfo.SetId(currentTransferId_++);
        if (currentTransferId_ >= INT_MAX) {
            currentTransferId_ = 0;
        }
        transferInfo.SetFileName(GetFileNameFromPath(*filePathItr));
        transferInfo.SetFilePath(*filePathItr);
        transferInfo.SetFileType(*mimeTypeItr);
        transferInfo.SetDeviceName(deviceName_);
        transferInfo.SetDeviceAddress(address);
        transferInfo.SetDirection(direction);
        transferInfo.SetStatus(OPP_TRANSFER_STATUS_PENDING);
        transferInfo.SetTimeStamp(static_cast<uint64_t>(timeStamp_));
        transferInfo.SetTotalBytes(static_cast<uint64_t>(GetFileLength(*filePathItr)));
        if (transferInfo.GetTotalBytes() > 0) {
            fileList_.push(transferInfo);
        }
    }
    direction_ = direction;
    address_ = address;
}

OppTransfer::~OppTransfer()
{
    isConnected_ = false;
    OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
}

int OppTransfer::GetFileNumber() const
{
    return fileList_.size();
}

std::string OppTransfer::GetFileNameFromPath(std::string filePath) const
{
    std::string fileName;
    size_t pos = filePath.find_last_of(u'/');
    if (pos != std::string::npos) {
        fileName = filePath.substr(pos + 1);
    }
    return fileName;
}

size_t OppTransfer::GetFileLength(std::string filePath) const
{
    std::ifstream ifs;
    ifs.open(filePath, std::ios::in);
    if (!ifs.is_open()) {
        HILOGE("[OPP TRANSFER] file open failed");
        return 0;
    }
    ifs.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.close();
    HILOGI("[OPP TRANSFER] File size is %{public}zu", fileSize);
    return fileSize;
}

int OppTransfer::ConnectObex(const ObexClientConfig &config, utility::Dispatcher &dispatcher)
{
    HILOGI("[OPP TRANSFER] Enter");

    if (direction_ == OPP_TRANSFER_DIRECTION_INBOND) {
        HILOGE("[OPP TRANSFER] is inbond,no need send connect request");
        return RET_BAD_STATUS;
    }
    if (fileList_.empty()) {
        HILOGE("[OPP TRANSFER] file list is empty");
        return RET_BAD_STATUS;
    }

    obexClient_ = std::make_unique<OppObexClient>(config, dispatcher);
    return obexClient_->Connect(fileList_.size());
}

int OppTransfer::DisconnectObex() const
{
    HILOGI("[OPP TRANSFER] Enter");
    if (direction_ == OPP_TRANSFER_DIRECTION_INBOND) {
        HILOGE("[OPP TRANSFER] is inbond,send disconnect request in OppService");
        return RET_BAD_STATUS;
    }

    if (obexClient_ == nullptr) {
        return RET_BAD_STATUS;
    }
    return obexClient_->Disconnect(true);
}

int OppTransfer::CancelTransfer()
{
    HILOGI("[OPP TRANSFER] Enter");
    if (direction_ == OPP_TRANSFER_DIRECTION_INBOND) {
        if (obexSession_ != nullptr) {
            int ret = obexSession_->SendSimpleResponse(ObexRspCode::BAD_REQUEST);
            OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
            return ret;
        } else {
            HILOGE("[OPP TRANSFER]obexSession is null");
            return RET_BAD_STATUS;
        }
    } else if (direction_ == OPP_TRANSFER_DIRECTION_OUTBOND) {
        if (obexClient_ == nullptr) {
            return RET_BAD_STATUS;
        }
        return obexClient_->CancelSendFile();
    }
    HILOGE("[OPP TRANSFER] unknow direction");
    return RET_BAD_STATUS;
}

void OppTransfer::OnReceiveIncomingConnect(ObexServerSession &session, uint32_t connectId)
{
    HILOGI("[OPP TRANSFER] Enter");
    if (direction_ != OPP_TRANSFER_DIRECTION_INBOND) {
        HILOGE("[OPP TRANSFER] is outbond");
        return;
    }
    obexSession_ = &session;
    connectId_ = connectId;
    incomingConnectTimer_->Start(INCOMING_CONNECT_TIMEOUT_MS);
}

void OppTransfer::OnReceiveIncomingFile(IOppTransferInformation info)
{
    HILOGI("[OPP TRANSFER] Enter");
    if (direction_ != OPP_TRANSFER_DIRECTION_INBOND) {
        HILOGE("[OPP TRANSFER] is outbond");
        return;
    }
    if (!fileList_.empty()) {
        HILOGE("[OPP TRANSFER] file list not empty");
        SetIncomingFileConfirmation(false);
        return;
    }

    info.SetId(currentTransferId_++);
    if (currentTransferId_ >= INT_MAX) {
        currentTransferId_ = 0;
    }
    info.SetTimeStamp(static_cast<uint64_t>(timeStamp_));
    info.SetDeviceName(deviceName_);
    info.SetDeviceAddress(address_);
    info.SetDirection(direction_);
    info.SetStatus(OPP_TRANSFER_STATUS_PENDING);
    fileList_.push(info);

    if (confirm_ == OPP_TRANSFER_CONFIRM_PENDING) {
        incomingFileTimer_->Start(INCOMING_FILE_TIMEOUT_MS);
        OppService::GetService()->NotifyReceiveIncomingFile(fileList_.front());
    } else if (confirm_ == OPP_TRANSFER_CONFIRM_ACCEPT) {
        SetIncomingFileConfirmation(true);
    } else if (confirm_ == OPP_TRANSFER_CONFIRM_REJECT) {
        SetIncomingFileConfirmation(false);
    } else {
        HILOGE("[OPP TRANSFER] unknow confirm");
        SetIncomingFileConfirmation(false);
    }
}

int OppTransfer::AcceptConnect()
{
    if (direction_ != OPP_TRANSFER_DIRECTION_INBOND) {
        HILOGE("[OPP TRANSFER] is outbond");
        return RET_BAD_STATUS;
    }
    int ret = RET_BAD_STATUS;
    incomingConnectTimer_->Stop();
    if (obexSession_ != nullptr) {
        OppMessage event(OPP_CONNECTED_EVT);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);

        auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, true);
        header->AppendItemConnectionId(connectId_);
        ret = obexSession_->SendResponse(*header);
    } else {
        HILOGE("[OPP TRANSFER] obexSession is null");
    }
    if (ret != RET_NO_ERROR) {
        OppService::GetService()->OnObexDisconnected(address_);
    }
    return ret;
}

int OppTransfer::StartTransfer()
{
    HILOGI("[OPP TRANSFER] Enter");

    if (direction_ == OPP_TRANSFER_DIRECTION_OUTBOND) {
        if (fileList_.empty()) {
            HILOGE("[OPP TRANSFER] file list is null");
            return RET_BAD_STATUS;
        }
        isConnected_ = true;
        if (obexClient_ == nullptr) {
            return RET_BAD_STATUS;
        }
        OnTransferStateChangeRunning();
        return obexClient_->SendFile(fileList_.front());
    } else if (direction_ == OPP_TRANSFER_DIRECTION_INBOND) {
        isConnected_ = true;
        return RET_NO_ERROR;
    } else {
        HILOGE("[OPP TRANSFER] unknow direction");
    }

    return RET_BAD_STATUS;
}

int OppTransfer::SetIncomingFileConfirmation(bool accept)
{
    HILOGI("[OPP TRANSFER] Enter");
    int ret = RET_BAD_STATUS;
    if (direction_ != OPP_TRANSFER_DIRECTION_INBOND) {
        return RET_BAD_STATUS;
    }
    if (accept) {
        confirm_ = OPP_TRANSFER_CONFIRM_ACCEPT;
    } else {
        confirm_ = OPP_TRANSFER_CONFIRM_REJECT;
    }
    incomingFileTimer_->Stop();
    if (obexSession_ != nullptr) {
        ObexRspCode rspCode = ObexRspCode::FORBIDDEN;
        if (accept) {
            rspCode = ObexRspCode::CONTINUE;
            OnTransferStateChangeRunning();
        } else {
            OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
        }
        auto respHeader = ObexHeader::CreateResponse(rspCode);
        respHeader->AppendItemConnectionId(connectId_);
        std::unique_ptr<ObexServerReceivedObject> &receivedObject = obexSession_->GetReceivedObject();
        if ((receivedObject != nullptr) && receivedObject->IsSrmEnable()) {
            respHeader->AppendItemSrm(true);
        }
        ret = obexSession_->SendResponse(*respHeader);
    }
    if (ret != RET_NO_ERROR) {
        OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
    }
    return ret;
}

IOppTransferInformation OppTransfer::GetCurrentTransferInformation()
{
    if (curretTransferInfo_ == nullptr) {
        IOppTransferInformation ret;
        return ret;
    }
    return *curretTransferInfo_;
}

void OppTransfer::IncomingConnectTimeout()
{
    if (obexSession_ == nullptr) {
        HILOGE("[OPP TRANSFER] obexSession_ is null");
        return;
    }
    auto header = ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE, true);
    obexSession_->SendResponse(*header);
    obexSession_->Disconnect();
    OppService::GetService()->OnObexDisconnected(address_);
}

void OppTransfer::IncomingFileTimeout()
{
    if (obexSession_ == nullptr) {
        HILOGE("[OPP TRANSFER] obexSession_ is null");
        return;
    }
    auto respHeader = ObexHeader::CreateResponse(ObexRspCode::FORBIDDEN);
    respHeader->AppendItemConnectionId(connectId_);
    std::unique_ptr<ObexServerReceivedObject> &receivedObject = obexSession_->GetReceivedObject();
    if ((receivedObject != nullptr) && receivedObject->IsSrmEnable()) {
        respHeader->AppendItemSrm(true);
    }
    obexSession_->SendResponse(*respHeader);
    OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CANCELED);
}

std::string OppTransfer::GetDeviceAddress()
{
    return address_;
}

int OppTransfer::GetDirection() const
{
    return direction_;
}

void OppTransfer::OnObexDisconnected()
{
    isConnected_ = false;
    obexSession_ = nullptr;
    connectId_ = 0;
    OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_CONNECTION_FAILED);
}

void OppTransfer::OnTransferStateChange(int state, int reason)
{
    HILOGI("[OPP TRANSFER] Enter");

    if (state == OPP_TRANSFER_STATUS_RUNNING) {
        OnTransferStateChangeRunning();
    } else if (state == OPP_TRANSFER_STATUS_SUCCESS) {
        OnTransferStateChangeSuccess();
    } else if (state == OPP_TRANSFER_STATUS_FAILD) {
        OnTransferStateChangeFaild(reason);
    } else {
        HILOGE("[OPP TRANSFER] error state=%{public}d", state);
    }
}

void OppTransfer::OnTransferStateChangeRunning()
{
    if (!isConnected_) {
        HILOGE("[OPP TRANSFER]obex is not connected");
        return;
    }
    if (fileList_.empty()) {
        HILOGE("[OPP TRANSFER] file list is empty");
        return;
    }
    curretTransferInfo_ = std::make_unique<IOppTransferInformation>(fileList_.front());
    curretTransferInfo_->SetStatus(OPP_TRANSFER_STATUS_RUNNING);
    OppService::GetService()->NotifyTransferStateChanged(*curretTransferInfo_);
}

void OppTransfer::OnTransferStateChangeSuccess()
{
    if (!isConnected_) {
        HILOGE("[OPP TRANSFER] obex is not connected");
        return;
    }
    if (curretTransferInfo_ == nullptr) {
        HILOGE("[OPP TRANSFER] curretTransferInfo_ is null");
        return;
    }
    if (fileList_.empty()) {
        HILOGE("[OPP TRANSFER] file list is empty");
        return;
    }
    curretTransferInfo_->SetStatus(OPP_TRANSFER_STATUS_SUCCESS);
    OppService::GetService()->NotifyTransferStateChanged(*curretTransferInfo_);
    curretTransferInfo_ = nullptr;
    fileList_.pop();
    if (direction_ == OPP_TRANSFER_DIRECTION_OUTBOND) {
        if (fileList_.empty()) {
            OppMessage event(OPP_DISCONNECT_REQ_EVT);
            event.dev_ = address_;
            OppService::GetService()->PostEvent(event);
        } else {
            if (obexClient_ == nullptr) {
                HILOGE("[OPP TRANSFER] obexClient_ is null");
                OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
                return;
            }
            OnTransferStateChangeRunning();
            if (obexClient_->SendFile(fileList_.front()) != RET_NO_ERROR) {
                HILOGE("[OPP TRANSFER] send file error");
                OnTransferStateChangeFaild(OPP_TRANSFER_FAILED_PROTOCOL);
                return;
            }
        }
    }
}

void OppTransfer::OnTransferStateChangeFaild(int reason)
{
    if (curretTransferInfo_ != nullptr) {
        curretTransferInfo_->SetStatus(OPP_TRANSFER_STATUS_FAILD);
        curretTransferInfo_->SetFailedReason(reason);
        OppService::GetService()->NotifyTransferStateChanged(*curretTransferInfo_);
        curretTransferInfo_ = nullptr;
        if (!fileList_.empty()) {
            fileList_.pop();
        }
    }
    while (!fileList_.empty()) {
        IOppTransferInformation transferInfo = fileList_.front();
        transferInfo.SetStatus(OPP_TRANSFER_STATUS_FAILD);
        transferInfo.SetFailedReason(reason);
        OppService::GetService()->NotifyTransferStateChanged(transferInfo);
        fileList_.pop();
    }
    if ((direction_ == OPP_TRANSFER_DIRECTION_OUTBOND) && isConnected_) {
        OppMessage event(OPP_DISCONNECT_REQ_EVT);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
    }
}

void OppTransfer::OnTransferPositionChange(size_t position) const
{
    HILOGI("[OPP TRANSFER] Enter");

    if (curretTransferInfo_ == nullptr) {
        HILOGE("[OPP TRANSFER] curretTransferInfo_ is null");
        return;
    }
    if (fileList_.empty()) {
        HILOGE("[OPP TRANSFER] file list is empty");
        return;
    }
    curretTransferInfo_->SetCurrentBytes(static_cast<uint64_t>(position));
    OppService::GetService()->NotifyTransferStateChanged(*curretTransferInfo_);
}
}  // namespace bluetooth
}  // namespace OHOS