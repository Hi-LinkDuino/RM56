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

#include "opp_obex_server.h"

#include <codecvt>
#include <dirent.h>
#include "../obex/obex_server.h"
#include "bt_def.h"
#include "log.h"
#include "opp_defines.h"
#include "opp_service.h"

namespace OHOS {
namespace bluetooth {
OppReceiveFileBodyObject::OppReceiveFileBodyObject(const std::string &file, const std::string address)
{
    address_ = address;
    file_ = file;
    OpenFile(file);
}

OppReceiveFileBodyObject::~OppReceiveFileBodyObject()
{
    try {
        Close();
    } catch(std::exception &e) {
        LOG_ERROR("[OPP OBEX SERVER]%{public}s():Catch exception %{public}s", __FUNCTION__, e.what());
    }
}

void OppReceiveFileBodyObject::OpenFile(const std::string &file)
{
    ofs_.open(file, std::ios::out);
    if (!ofs_.is_open()) {
        HILOGE("[OPP OBEX SERVER] file open failed");
        return;
    } else {
        HILOGI("[OPP OBEX SERVER] file=%{public}s  opened.", file.c_str());
    }
}

size_t OppReceiveFileBodyObject::Read(uint8_t *buf, size_t bufLen)
{
    return bufLen;
}

size_t OppReceiveFileBodyObject::Write(const uint8_t *buf, size_t bufLen)
{
    size_t writeSize = 0;
    if (ofs_.is_open()) {
        auto buffer = reinterpret_cast<const char *>(buf);
        ofs_.write(buffer, bufLen);
        fileReceiveSize_ += bufLen;
        writeSize = bufLen;
        OppService::GetService()->OnTransferPositionChange(address_, fileReceiveSize_);
        HILOGI("[OPP OBEX SERVER] write file bufLen=%zu", bufLen);
    } else {
        OppService::GetService()->CancelTransfer(address_);
        HILOGE("[OPP OBEX SERVER] file open failed");
    }
    return writeSize;
}

void OppReceiveFileBodyObject::SetFileReceiveSuccess()
{
    fileReceiveSuccess_ = true;
}

int OppReceiveFileBodyObject::Close()
{
    ofs_.close();
    if (!fileReceiveSuccess_) {
        remove(file_.c_str());
    }
    return RET_NO_ERROR;
}

OppObexServer::OppObexServer(const ObexServerConfig &config, utility::Dispatcher &dispatcher)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    oppObexObserver_ = std::make_unique<OppObexObserver>();
    obexServer_ = std::make_unique<ObexServer>(OPP_SERVICE_NAME, config, *oppObexObserver_, dispatcher);
}

int OppObexServer::StartUp() const
{
    HILOGI("[OPP OBEX SERVER] Enter");
    int ret = obexServer_->Startup();
    if (ret != RET_NO_ERROR) {
        HILOGE("[OPP OBEX SERVER]:Obex Startup Error ret = %{public}d", ret);
    }
    return ret;
}

int OppObexServer::ShutDown() const
{
    HILOGI("[OPP OBEX SERVER] Enter");
    obexServer_->Shutdown();
    return RET_NO_ERROR;
}

void OppObexServer::OppObexObserver::SendOppDisconnected(const std::string &btAddr) const
{
    OppMessage event(OPP_DISCONNECTED_EVT);
    event.dev_ = btAddr;
    OppService::GetService()->PostEvent(event);
}

void OppObexServer::OppObexObserver::OnTransportDisconnected(const std::string &btAddr)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    SendOppDisconnected(btAddr);
}

void OppObexServer::OppObexObserver::OnTransportError(const std::string &btAddr, int errCd, const std::string &msg)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    SendOppDisconnected(btAddr);
}

void OppObexServer::OppObexObserver::OnTransportConnect(ObexIncomingConnect &incomingConnect)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    incomingConnect.AcceptConnection();
}

void OppObexServer::OppObexObserver::OnConnect(ObexServerSession &session, const ObexHeader &req)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    OppService::GetService()->OnReceiveIncomingConnect(session, connectionId_);
}

void OppObexServer::OppObexObserver::OnDisconnect(ObexServerSession &session, const ObexHeader &req)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, false);
    auto connectId = req.GetItemConnectionId();
    if (connectId != nullptr) {
        header->AppendItemConnectionId(connectId->GetWord());
    }
    session.SendResponse(*header);
    session.Disconnect();
}

void OppObexServer::OppObexObserver::OnPut(ObexServerSession &session, const ObexHeader &req)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    bool isHeader = false;
    int ret = RET_NO_ERROR;
    if (session.GetReceivedObject() == nullptr) {
        ret = ReceiveFileHeader(session, req);
        isHeader = true;
    }
    if (ret == RET_NO_ERROR) {
        ReceiveFileBody(session, req, isHeader);
    }
}

void OppObexServer::OppObexObserver::OnAbort(ObexServerSession &session, const ObexHeader &req)
{
    HILOGI("[OPP OBEX SERVER] Enter");
    auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, false);
    auto connectId = req.GetItemConnectionId();
    if (connectId != nullptr) {
        header->AppendItemConnectionId(connectId->GetWord());
    }
    session.SendResponse(*header);
    OppService::GetService()->OnTransferStateChange(
        session.GetRemoteAddr().GetAddress(), OPP_TRANSFER_STATUS_FAILD, OPP_TRANSFER_FAILED_CANCELED);
}

void OppObexServer::OppObexObserver::OnBusy(ObexServerSession &session, bool isBusy) const
{
    HILOGI("[OPP OBEX SERVER] Enter");
}

int OppObexServer::OppObexObserver::ReceiveFileHeader(ObexServerSession &session, const ObexHeader &req)
{
    auto nameItem = req.GetItemName();
    std::u16string name;
    if (nameItem != nullptr) {
        name = nameItem->GetUnicodeText();
    }
    if (name.empty()) {
        HILOGE("[OPP OBEX SERVER]:name is empty");
        session.SendSimpleResponse(ObexRspCode::BAD_REQUEST);
        return RET_BAD_STATUS;
    }
    std::string fileName = U16stringToString(name);
    if (NeedRejectFileForPts(fileName)) {
        HILOGE("[OPP OBEX SERVER]:name is pts reject");
        session.SendSimpleResponse(ObexRspCode::UNSUPPORTED_MEDIA_TYPE);
        return RET_BAD_STATUS;
    }

    auto typeItem = req.GetItemType();
    std::string type;
    if (typeItem != nullptr) {
        type = typeItem->GetString();
    }

    auto lengthItem = req.GetItemLength();
    size_t length = 0;
    if (lengthItem != nullptr) {
        length = lengthItem->GetWord();
    }
    if (length <= 0) {
        HILOGE("[OPP OBEX SERVER]:length is 0");
        session.SendSimpleResponse(ObexRspCode::LENGTH_REQUIRED);
        return RET_BAD_STATUS;
    }

    std::string filePath = OPP_RECEIVE_FILE_PATH + RenameFile(fileName);

    std::shared_ptr<ObexBodyObject> writer =
        std::make_shared<OppReceiveFileBodyObject>(filePath, session.GetRemoteAddr().GetAddress());
    std::unique_ptr<ObexServerReceivedObject> &receivedObject = session.CreateReceivedObject(req, writer);
    if ((receivedObject != nullptr) && session.IsSupportSrmMode() && req.GetItemSrm()) {
        receivedObject->SetSrmEnable(true);
    }

    IOppTransferInformation info;
    info.SetDeviceAddress(session.GetRemoteAddr().GetAddress());
    info.SetFileName(fileName);
    info.SetFilePath(filePath);
    info.SetFileType(type);
    info.SetTotalBytes(static_cast<uint64_t>(length));
    OppService::GetService()->OnReceiveIncomingFile(info);
    return RET_NO_ERROR;
}

// For PTS OPP/SR/OPH/BV-xxx.
bool OppObexServer::OppObexObserver::NeedRejectFileForPts(std::string fileName) const
{
    // regect OPHBV10,OPHBV14,OPHBV18.
    std::string ptsRejectName = "OPHBV";
    std::string::size_type idx = fileName.find(ptsRejectName);
    if (idx == fileName.npos) {
        return false;
    }
    return true;
}

void OppObexServer::OppObexObserver::ReceiveFileBody(ObexServerSession &session,
    const ObexHeader &req, bool isHead) const
{
    std::unique_ptr<ObexServerReceivedObject> &receivedObject = session.GetReceivedObject();
    if (receivedObject == nullptr) {
        session.SendSimpleResponse(ObexRspCode::BAD_REQUEST);
        OppService::GetService()->OnTransferStateChange(
            session.GetRemoteAddr().GetAddress(), OPP_TRANSFER_STATUS_FAILD, OPP_TRANSFER_FAILED_PROTOCOL);
        return;
    }
    auto body = req.GetItemBody();
    if (body != nullptr) {
        HILOGI("[OPP OBEX SERVER]:Server Received Body From Request!");
        receivedObject->AppendBody(body->GetBytes().get(), body->GetHeaderDataSize());
    }
    if (req.GetFieldCode() == static_cast<uint8_t>(ObexOpeId::PUT_FINAL)) {
        auto fbody = req.GetItemEndBody();
        if (fbody != nullptr) {
            HILOGI("[OPP OBEX SERVER]:Server Received End-Body From Request!");
            receivedObject->AppendBody(fbody->GetBytes().get(), fbody->GetHeaderDataSize());
        }
        auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, false);
        auto connectId = req.GetItemConnectionId();
        if (connectId != nullptr) {
            header->AppendItemConnectionId(connectId->GetWord());
        }
        header->AppendItemEndBody(nullptr, 0);
        session.SendResponse(*header);

        OppReceiveFileBodyObject *bodyObject = static_cast<OppReceiveFileBodyObject *>(
            receivedObject->GetWriter().get());
        if (bodyObject != nullptr) {
            bodyObject->SetFileReceiveSuccess();
            session.FreeReceivedObject();
            OppService::GetService()->OnTransferStateChange(
                session.GetRemoteAddr().GetAddress(), OPP_TRANSFER_STATUS_SUCCESS, 0);
            return;
        }
        session.FreeReceivedObject();
        OppService::GetService()->OnTransferStateChange(
            session.GetRemoteAddr().GetAddress(), OPP_TRANSFER_STATUS_FAILD, OPP_TRANSFER_FAILED_PROTOCOL);
    } else if (!isHead) {
        if (!session.IsSupportSrmMode() || req.GetItemSrm()) {
            auto respHeader = ObexHeader::CreateResponse(ObexRspCode::CONTINUE);
            auto connectId = req.GetItemConnectionId();
            if (connectId != nullptr) {
                respHeader->AppendItemConnectionId(connectId->GetWord());
            }
            if (session.IsSupportSrmMode() && req.GetItemSrm()) {
                respHeader->AppendItemSrm(true);
            }
            session.SendResponse(*respHeader);
        }
    }
}

std::string OppObexServer::OppObexObserver::U16stringToString(const std::u16string &u16str) const
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(u16str);
}

std::string OppObexServer::OppObexObserver::RenameFile(std::string fileName) const
{
    if (!HasSameName(OPP_RECEIVE_FILE_PATH, fileName)) {
        return fileName;
    }

    time_t timeStamp = 0;
    (void)time(&timeStamp);
    char *name = strchr(&fileName[0], '.');
    if (name == nullptr) {
        HILOGI("[OPP OBEX SERVER] opendir error");
        return fileName;
    }
    size_t namelen = name - &fileName[0];
    std::string rename = fileName.substr(0, namelen);
    std::string type = fileName.erase(0, fileName.find_last_of('.') + 1);
    std::string fileNewName = rename + "_" + std::to_string(timeStamp) + "." + type;
    return fileNewName;
}

bool OppObexServer::OppObexObserver::HasSameName(std::string path, std::string name) const
{
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        HILOGI("[OPP OBEX SERVER] opendir error");
        return false;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        if (strcmp(ent->d_name, name.c_str()) == 0) {
            HILOGI("[OPP OBEX SERVER] has same name");
            return true;
        }
    }
    return false;
}
}  // namespace bluetooth
}  // namespace OHOS
