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

#include "map_mse_server.h"
#include <algorithm>
#include <atomic>
#include <codecvt>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>
#include "adapter_config.h"
#include "data_access.h"
#include "map_mse_instance.h"

namespace OHOS {
namespace bluetooth {
const uint8_t MapMseServer::MAS_TARGET[TARGET_SIZE] = {
    0xbb, 0x58, 0x2b, 0x40, 0x42, 0x0c, 0x11, 0xdb, 0xb0, 0xde, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
};
const std::string MapMseServer::HEADER_TYPE_GET_FOLDER_LISTING = "x-obex/folder-listing";
const std::string MapMseServer::HEADER_TYPE_GET_MESSAGE_LISTING = "x-bt/MAP-msg-listing";
const std::string MapMseServer::HEADER_TYPE_GET_CONVO_LISTING = "x-bt/MAP-convo-listing";
const std::string MapMseServer::HEADER_TYPE_MESSAGE = "x-bt/message";
const std::string MapMseServer::HEADER_TYPE_SET_MESSAGE_STATUS = "x-bt/messageStatus";
const std::string MapMseServer::HEADER_TYPE_SET_NOTIFICATION_REGISTRATION = "x-bt/MAP-NotificationRegistration";
const std::string MapMseServer::HEADER_TYPE_MESSAGE_UPDATE = "x-bt/MAP-messageUpdate";
const std::string MapMseServer::HEADER_TYPE_GET_MAS_INSTANCE_INFORMATION = "x-bt/MASInstanceInformation";
const std::string MapMseServer::HEADER_TYPE_OWNER_STATUS = "x-bt/ownerStatus";
const std::string MapMseServer::HEADER_TYPE_SET_NOTIFICATION_FILTER = "x-bt/MAP-notification-filter";

MapMseServer::MapMseServer(MapMseResource &content, utility::Dispatcher &dispatcher, MseObserver &observer,
    MapMseInstance &instance, uint8_t rfcommNo, uint16_t l2capPsm, std::string serviceName,
    const MapAccountItem &accountItem, bool smsSupport)
    : content_(content),
      dispatcher_(dispatcher),
      observer_(observer),
      instance_(instance),
      rfcommNo_(rfcommNo),
      goepL2capPsm_(l2capPsm),
      serviceName_(serviceName),
      accountItem_(accountItem),
      smsSupport_(smsSupport)
{
    MSE_LOG_INFO("%{public}s Create", __PRETTY_FUNCTION__);

    mseObexServer_ = std::make_unique<MseObexServer>(*this);
    CreateFolder();
    supportedFeatureMask_ = MAP_SUPPORTED_FEATURES_DEFAULT;
    connectId_ = ATOMIC_VAR_INIT();
}

void MapMseServer::CreateFolder(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    rootFolder_ = std::make_unique<MapMseFolder>("root");
    MapMseFolder *telecom = rootFolder_->SetFolder("telecom");
    MapMseFolder *msg = telecom->SetFolder("msg");
    InitDefaultFolder(*msg);
    if (smsSupport_) {
        InitSmsMmsFolder(*msg);
    }
    if (MessageType::EMAIL == accountItem_.msgType_) {
        InitEmailFolder(*msg);
    } else if (MessageType::IM == accountItem_.msgType_) {
        InitImFolder(*msg);
    }
    currentFolder_ = rootFolder_.get();
}

void MapMseServer::InitDefaultFolder(MapMseFolder &root)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    root.SetFolder(MAP_MSE_FOLDER_NAME_INBOX);
    root.SetFolder(MAP_MSE_FOLDER_NAME_SENT);
    root.SetFolder(MAP_MSE_FOLDER_NAME_OUTBOX);
    root.SetFolder(MAP_MSE_FOLDER_NAME_DELETED);
}

void MapMseServer::InitSmsMmsFolder(MapMseFolder &root)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    root.SetSmsMmsFolder(MAP_MSE_FOLDER_NAME_INBOX);
    root.SetSmsMmsFolder(MAP_MSE_FOLDER_NAME_SENT);
    root.SetSmsMmsFolder(MAP_MSE_FOLDER_NAME_DRAFT);
    root.SetSmsMmsFolder(MAP_MSE_FOLDER_NAME_OUTBOX);
    root.SetSmsMmsFolder(MAP_MSE_FOLDER_NAME_DELETED);
}

void MapMseServer::InitEmailFolder(MapMseFolder &parentFolder, int depth)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    if (!dataProvider) {
        return;
    }

    std::unordered_map<int, std::string> data = GetEmailInfo(dataProvider, parentFolder);
    for (auto &fd : data) {
        MapMseFolder *ff = parentFolder.GetSubFolder(fd.second);
        if (ff == nullptr) {
            continue;
        }
        auto secondData = GetEmailInfo(dataProvider, *ff);
        for (auto &sd : secondData) {
            MapMseFolder *sf = parentFolder.GetSubFolder(sd.second);
            if (sf == nullptr) {
                continue;
            }
            GetEmailInfo(dataProvider, *sf);
        }
    }
}

std::unordered_map<int, std::string> MapMseServer::GetEmailInfo(
    std::unique_ptr<DataAccess> &dataAccess, MapMseFolder &parent)
{
    std::unordered_map<int, std::string> result {};
    std::string sql = "select id, name from Folder where parent_id = ? ";
    auto stmt = dataAccess->CreateStatement(sql);
    stmt->SetParamInt(1, parent.GetFolderId());
    auto dataResult = stmt->Query();
    while (dataResult->Next()) {
        int id = dataResult->GetInt(0);
        std::string name = dataResult->GetString(1);
        result[id] = name;
        parent.SetEmailFolder(id, name);
    }
    return result;
}

void MapMseServer::InitImFolder(MapMseFolder &root)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_OTHER, MAP_MSE_FOLDER_NAME_OTHER);
    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_INBOX, MAP_MSE_FOLDER_NAME_INBOX);
    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_SENT, MAP_MSE_FOLDER_NAME_SENT);
    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_DRAFT, MAP_MSE_FOLDER_NAME_DRAFT);
    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_OUTBOX, MAP_MSE_FOLDER_NAME_OUTBOX);
    root.SetImFolder(ImFolderId::MAP_MSE_FOLDER_ID_DELETED, MAP_MSE_FOLDER_NAME_DELETED);
}

MapMseServer::~MapMseServer()
{
    MSE_LOG_INFO("%{public}s Delete", __PRETTY_FUNCTION__);

    currentFolder_ = nullptr;
}

MapMseServer::MseObexServer::MseObexServer(MapMseServer &mseServer) : mseServer_(&mseServer)
{
    MSE_LOG_INFO("%{public}s Create", __PRETTY_FUNCTION__);
}

MapMseServer::MseObexServer::~MseObexServer()
{
    MSE_LOG_INFO("%{public}s Delete", __PRETTY_FUNCTION__);
}

void MapMseServer::MseObexServer::OnConnect(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (!req.GetItemTarget()) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
        return;
    }
    if (req.GetItemTarget()->GetHeaderDataSize() != TARGET_SIZE) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
        return;
    }
    if (req.GetItemAppParams()) {
        if (req.GetItemAppParams()->GetTlvtriplet(MapMseParams::PARAM_MAP_SUPPORTED_FEATURES)) {
            mseServer_->supportedFeatureMask_ =
                req.GetItemAppParams()->GetTlvtriplet(MapMseParams::PARAM_MAP_SUPPORTED_FEATURES)->GetUint32();
        }
    }
    std::unique_ptr<uint8_t[]> masTarget = req.GetItemTarget()->GetBytes();
    for (int i = 0; i < TARGET_SIZE; i++) {
        if (masTarget[i] != MAS_TARGET[i]) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE, true));
            return;
        }
    }
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, true);
    mseServer_->connectId_++;
    resHeader->AppendItemConnectionId(mseServer_->connectId_.load());
    resHeader->AppendItemWho(MAS_TARGET, TARGET_SIZE);
    session.SendResponse(*resHeader);
    mseServer_->observer_.OnConnected(session);
}

void MapMseServer::MseObexServer::OnTransportConnect(ObexIncomingConnect &incomingConnect)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mseServer_->observer_.OnIncomingConnect(incomingConnect);
}

void MapMseServer::MseObexServer::OnTransportDisconnected(const std::string &btAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    RawAddress remoteAddr(btAddr);
    mseServer_->observer_.OnTransportDisconnected(remoteAddr);
}

void MapMseServer::MseObexServer::OnTransportError(const std::string &btAddr, int errCd, const std::string &msg)
{
    MSE_LOG_ERROR("%{public}s Enter", __PRETTY_FUNCTION__);
}

void MapMseServer::MseObexServer::OnError(const int errCd, const std::string &msg)
{
    MSE_LOG_ERROR("%{public}s Enter", __PRETTY_FUNCTION__);
}

void MapMseServer::MseObexServer::OnDisconnect(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, false);
    session.SendResponse(*resHeader);
    mseServer_->observer_.OnDisconnect(session.GetRemoteAddr());
}

void MapMseServer::MseObexServer::OnPut(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    if (!req.GetItemType()) {
        MSE_LOG_ERROR("Item Type of Obex header is error.");
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    OnBusy(session, true);
    auto msgType = req.GetItemType()->GetString();
    if (HEADER_TYPE_MESSAGE == msgType) {
        mseServer_->PushMessageResponse(session, req);
    } else if (HEADER_TYPE_SET_NOTIFICATION_REGISTRATION == msgType) {
        mseServer_->SetNotificationRegistration(session, req);
    } else if (HEADER_TYPE_SET_NOTIFICATION_FILTER == msgType) {
        mseServer_->SetNotificationFilter(session, req);
    } else if (HEADER_TYPE_SET_MESSAGE_STATUS == msgType) {
        if (!req.GetItemName()) {
            MSE_LOG_ERROR("Item Name of Obex header is error.");
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
            return;
        }
        mseServer_->SetMessageStatus(session, req);
    } else if (HEADER_TYPE_MESSAGE_UPDATE == msgType) {
        mseServer_->UpdateInbox(session, req);
    } else if (HEADER_TYPE_OWNER_STATUS == msgType) {
        mseServer_->SetOwnerStatus(session, req);
    }
    OnBusy(session, false);
}

void MapMseServer::MseObexServer::OnGet(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (!req.GetItemType()) {
        MSE_LOG_ERROR("Item Type of Obex header is error.");
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    OnBusy(session, true);
    auto msgType = req.GetItemType()->GetString();
    if (HEADER_TYPE_MESSAGE == msgType) {
        mseServer_->GetMessageResponse(session, req);
    } else if (HEADER_TYPE_GET_MESSAGE_LISTING == msgType) {
        if (!req.GetItemName()) {
            MSE_LOG_ERROR("Item Name of Obex header is error.");
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
            OnBusy(session, false);
            return;
        }
        mseServer_->GetMessageListing(session, req);
    } else if (HEADER_TYPE_GET_CONVO_LISTING == msgType) {
        mseServer_->GetConversationListing(session, req);
    } else if (HEADER_TYPE_GET_FOLDER_LISTING == msgType) {
        mseServer_->GetFolderListing(session, req);
    } else if (HEADER_TYPE_OWNER_STATUS == msgType) {
        mseServer_->GetOwnerStatus(session, req);
    } else if (HEADER_TYPE_GET_MAS_INSTANCE_INFORMATION == msgType) {
        mseServer_->GetMASInstanceInformation(session, req);
    }
    OnBusy(session, false);
}

void MapMseServer::MseObexServer::OnAbort(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
}

void MapMseServer::MseObexServer::OnBusy(ObexServerSession &session, bool isBusy) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    const RawAddress &remoteAddr = session.GetRemoteAddr();
    mseServer_->observer_.OnBusy(remoteAddr, isBusy);
}

void MapMseServer::MseObexServer::OnSetPath(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string folderName;
    if (req.GetItemName() != nullptr) {
        if (req.GetItemName()->GetUnicodeText().size() != 0) {
            folderName = mseServer_->ToUtf8String(req.GetItemName()->GetUnicodeText());
            std::transform(folderName.begin(), folderName.end(), folderName.begin(), ::tolower);
        }
    }
    uint8_t flages = *req.GetFieldFlags();
    if ((flages & 0x02) != 0x02) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
        return;
    }
    MSE_LOG_INFO("MapMseServer::MseObexServer::OnSetPath folderName : %{public}s flages : %u ", folderName.c_str(), flages);
    MapMseFolder *tempFolder = nullptr;
    if (folderName.size() == 0) {
        if (flages == 0x03) {
            tempFolder = mseServer_->currentFolder_->GetParent();
        } else {
            tempFolder = mseServer_->rootFolder_.get();
        }
    } else {
        if (flages == 0x03) {
            tempFolder = mseServer_->currentFolder_->GetParent();
            if (tempFolder == nullptr) {
                session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
                return;
            }
        }
        tempFolder = mseServer_->currentFolder_->GetSubFolder(folderName);
    }
    if (tempFolder == nullptr) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
        return;
    }
    mseServer_->currentFolder_ = tempFolder;
    MSE_LOG_INFO("Current Folder : %{public}s ", mseServer_->currentFolder_->GetName().c_str());
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    session.SendResponse(*resHeader);
}

int MapMseServer::Enable(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    LoadAdapterConfig();
    ObexServerConfig obexSrvConfig = {
        .useRfcomm_ = true,
        .rfcommScn_ = rfcommNo_,
        .rfcommMtu_ = static_cast<uint16_t>(rfcommMtu_),
#ifdef MAP_MSE_L2CAP_ENABLE
        .useL2cap_ = true,
#else
        .useL2cap_ = false,
#endif
        .l2capPsm_ = goepL2capPsm_,
        .l2capMtu_ = static_cast<uint16_t>(l2capMtu_),
        .isSupportSrm_ = srmEnable_,
        .isSupportReliableSession_ = false
    };
    obexServer_ = std::make_unique<ObexMpServer>(serviceName_, obexSrvConfig, *mseObexServer_, dispatcher_);
    return !obexServer_->Startup() ? Result::SUCCEED : Result::FAIL;
}

void MapMseServer::Disable(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    return obexServer_->Shutdown();
}

void MapMseServer::SetRemoteFeatureMask(uint32_t features)
{
    if (supportedFeatureMask_ == MAP_SUPPORTED_FEATURES_DEFAULT) {
        supportedFeatureMask_ = features;
    } else {
        supportedFeatureMask_ = supportedFeatureMask_ & features;
    }
    content_.SetRemoteFeatureMask(supportedFeatureMask_);
    messageVersion_ = MAP_V10;
    messageListingVersion_ = MAP_V10;
    if ((supportedFeatureMask_ & MAP_FEATURE_MESSAGE_FORMAT_V11_BIT) > 0) {
        messageVersion_ = MAP_V11;
    }
    if ((supportedFeatureMask_ & MAP_FEATURE_MESSAGE_LISTING_FORMAT_V11_BIT) > 0) {
        messageListingVersion_ = MAP_V11;
    }
}

int MapMseServer::CreateBMessage(
    ObexServerSession &session, const ObexHeader &req, const uint8_t charset, std::string &folderName)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (currentFolder_ == nullptr || req.GetItemName() == nullptr) {
        return RET_BAD_PARAM;
    } else {
        if (req.GetItemName()->GetUnicodeText().size() != 0) {
            std::string subFolderName = ToUtf8String(req.GetItemName()->GetUnicodeText());
            std::transform(subFolderName.begin(), subFolderName.end(), subFolderName.begin(), ::tolower);
            folderName = currentFolder_->GetSubFolder(subFolderName)->GetName();
        } else {
            folderName = currentFolder_->GetName();
        }
    }
    if (MAP_MSE_FOLDER_NAME_OUTBOX != folderName && MAP_MSE_FOLDER_NAME_DRAFT != folderName) {
        return RET_BAD_STATUS;
    }
    auto body = req.GetExtendBodyObject();
    uint8_t buf[READ_BODY_BUFF_SIZE];
    size_t cnt = 0;
    std::vector<uint8_t> results;
    while ((cnt = body->Read(buf, READ_BODY_BUFF_SIZE)) > 0) {
        results.insert(results.end(), buf, buf + cnt);
    }
    std::string data(results.begin(), results.end());
    MSE_LOG_INFO("body data: %{public}s", data.c_str());
    bMessage_ = std::make_unique<MapMseBmsg>();
    bMessage_->SetVersion(messageVersion_);
    if (bMessage_->Init(data, charset) != RET_NO_ERROR) {
        return RET_BAD_STATUS;
    }
    if (MessageType::SMS_GSM == bMessage_->GetType() || MessageType::SMS_CDMA == bMessage_->GetType()) {
        if (stub::PhoneType::SMS_GSM == stub::MapService::GetInstance()->GetSystemTelephonyService().phoneType) {
            bMessage_->SetType(MessageType::SMS_GSM);
        } else if (stub::PhoneType::SMS_CDMA ==
                   stub::MapService::GetInstance()->GetSystemTelephonyService().phoneType) {
            bMessage_->SetType(MessageType::SMS_CDMA);
        }
    }
    return RET_NO_ERROR;
}

void MapMseServer::PushMessageResponse(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (!appParams->charSet_) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    std::string folderName = "";
    auto ret = CreateBMessage(session, req, *appParams->charSet_, folderName);
    if (ret == RET_BAD_PARAM) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    } else if (ret == RET_BAD_STATUS) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::BAD_REQUEST));
        return;
    }
    std::string handle;
    if (appParams->messageHandle_ != "") {
        if (((supportedFeatureMask_ & MAP_FEATURE_MESSAGE_FORWARDING_BIT) == 0) || !appParams->attachment_ ||
            !appParams->modifyText_) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
            return;
        }
        content_.SentOrForwardedMessage(*bMessage_, *appParams, appParams->messageHandle_);
        handle = appParams->messageHandle_;
    } else if (appParams->conversationId_ != "") {
        handle = content_.SentConversationMessage(*bMessage_, *appParams, folderName);
    } else {
        handle = content_.SentNewMessage(*bMessage_, folderName, *appParams);
    }
    if (handle == "") {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
        return;
    }
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    resHeader->AppendItemName(ToU16String(handle));
    session.SendResponse(*resHeader);
}

void MapMseServer::GetMessageResponse(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (!req.GetItemName()) {
        MSE_LOG_ERROR("Item Name of Obex header is error.");
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    std::string name;
    if (req.GetItemName()->GetUnicodeText().size() != 0) {
        name = ToUtf8String(req.GetItemName()->GetUnicodeText());
    } else {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
        return;
    }
    std::string body = content_.GetMessage(name, *appParams, messageVersion_);
    auto bodyObj = std::make_shared<ObexArrayBodyObject>();
    bodyObj->Write((uint8_t *)body.data(), body.size());
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    uint64_t handle = std::atol(name.c_str());
    if ((MessageType::EMAIL == ParseHandle(handle) || MessageType::IM == ParseHandle(handle)) &&
        !appParams->fractionRequest_) {
        ObexTlvParamters obexAppPrarams;
        uint8_t fractionDeliver = static_cast<uint8_t>(FractionDeliver::LAST);
        TlvTriplet tlvItem(MapMseParams::PARAM_FRACTION_DELIVER, fractionDeliver);
        obexAppPrarams.AppendTlvtriplet(tlvItem);
        resHeader->AppendItemAppParams(obexAppPrarams);
    }
    session.SendGetResponse(req, *resHeader, bodyObj);
}

void MapMseServer::GetMessageListing(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    MapMseFolder *folder = currentFolder_;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
        if (appParams->filterMessageHandle_.size() != 0 || appParams->conversationId_.size() != 0) {
            folder = rootFolder_.get();
            folder->SetIgnore(true);
        } else {
            std::string folderName;
            if (req.GetItemName() && req.GetItemName()->GetUnicodeText().size() != 0) {
                folderName = ToUtf8String(req.GetItemName()->GetUnicodeText());
                MSE_LOG_INFO("folderName : %{public}s Enter", folderName.c_str());
                std::transform(folderName.begin(), folderName.end(), folderName.begin(), ::tolower);
                folder = currentFolder_->GetSubFolder(folderName);
            } else {
                folder = currentFolder_;
            }
            if (folder == nullptr) {
                MSE_LOG_INFO("currentFolder_ is nullptr.");
                session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
                return;
            }
        }
    } else {
        if (appParams == nullptr)
            appParams = std::make_unique<MapMseParams>();
    }
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    std::shared_ptr<ObexBodyObject> bodyObj = nullptr;
    uint8_t unMessage = 0;
    uint16_t listingSize = 0;
    auto name = folder->IsIgnore() ? "" : folder->GetName();
    if (appParams->maxListCount_ == 0x0) {
        content_.GetListingSize(name, *appParams, listingSize, unMessage);
    } else {
        auto body = content_.GetBodyXml(name, *appParams, messageListingVersion_, listingSize, unMessage);
        MSE_LOG_INFO("body : %{public}s ", body.c_str());
        if (body != "") {
            bodyObj = std::make_shared<ObexArrayBodyObject>();
            bodyObj->Write((uint8_t *)body.data(), body.size());
        }
    }
    ObexTlvParamters obexAppPrarams;
    CreateOutputAppPrarams(obexAppPrarams, listingSize, unMessage);
    resHeader->AppendItemAppParams(obexAppPrarams);
    session.SendGetResponse(req, *resHeader, bodyObj);
}

void MapMseServer::CreateOutputAppPrarams(
    ObexTlvParamters &obexAppPrarams, const uint16_t &listSize, const uint8_t &unRead)
{
    obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_LISTING_SIZE, listSize));
    obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_NEW_MESSAGE, unRead));
    if ((supportedFeatureMask_ & MAP_FEATURE_DATABASE_INDENTIFIER_BIT) == MAP_FEATURE_DATABASE_INDENTIFIER_BIT) {
        auto value = std::to_string(instance_.GetDatabaseIdentifier());
        obexAppPrarams.AppendTlvtriplet(
            TlvTriplet(MapMseParams::PARAM_DATABASE_IDENTIFIER, static_cast<uint8_t>(value.size() + 1),
            reinterpret_cast<uint8_t *>(value.data())));
    }
    if ((supportedFeatureMask_ & MAP_FEATURE_FOLDER_VERSION_COUNTER_BIT) == MAP_FEATURE_FOLDER_VERSION_COUNTER_BIT) {
        auto value = std::to_string(instance_.GetFolderVersionCounter());
        obexAppPrarams.AppendTlvtriplet(TlvTriplet(
            MapMseParams::PARAM_FOLDER_VERSION_COUNTER, static_cast<uint8_t>(value.size() + 1),
            reinterpret_cast<uint8_t *>(value.data())));
    }
    std::string mseTime = GetMseTime();
    obexAppPrarams.AppendTlvtriplet(
        TlvTriplet(MapMseParams::PARAM_MSE_TIME, mseTime.size(),
            reinterpret_cast<uint8_t *>(const_cast<char *>(mseTime.c_str()))));
}

void MapMseServer::GetConversationListing(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    ObexTlvParamters obexAppPrarams;
    uint16_t listingSize = 0;
    std::shared_ptr<ObexBodyObject> bodyObj = nullptr;
    if (appParams->maxListCount_ == 0x0) {
        listingSize = content_.GetConvoLstCount(*appParams);
        if ((supportedFeatureMask_ & MAP_FEATURE_CONVERSATION_VERSION_COUNTER_BIT) > 0) {
            auto value = std::to_string(instance_.GetConversationVersionCounter());
            obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_CONVERSATION_LISTING_VERSION_COUNTER,
                static_cast<uint8_t>(value.size() + 1),
                reinterpret_cast<uint8_t *>(value.data())));
        }
    } else {
        auto bodyData = content_.GetCvslistBodyXml(*appParams, listingSize);
        MSE_LOG_INFO("Body : %{public}s ", bodyData.c_str());
        bodyObj = std::make_shared<ObexArrayBodyObject>();
        bodyObj->Write((uint8_t *)bodyData.data(), bodyData.size());
    }
    obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_LISTING_SIZE, listingSize));
    if ((supportedFeatureMask_ & MAP_FEATURE_DATABASE_INDENTIFIER_BIT) > 0) {
        auto value = std::to_string(instance_.GetDatabaseIdentifier());
        obexAppPrarams.AppendTlvtriplet(
            TlvTriplet(MapMseParams::PARAM_DATABASE_IDENTIFIER, static_cast<uint8_t>(value.size() + 1),
            reinterpret_cast<uint8_t *>(value.data())));
    }
    std::string mseTime = GetMseTime();
    obexAppPrarams.AppendTlvtriplet(
        TlvTriplet(MapMseParams::PARAM_MSE_TIME, static_cast<uint8_t>(mseTime.size()),
        reinterpret_cast<uint8_t *>(const_cast<char *>(mseTime.c_str()))));
    resHeader->AppendItemAppParams(obexAppPrarams);
    session.SendGetResponse(req, *resHeader, bodyObj);
}

void MapMseServer::GetFolderListing(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    ObexTlvParamters obexAppPrarams;
    std::shared_ptr<ObexBodyObject> bodyObj = nullptr;
    if (appParams->maxListCount_ == 0x00) {
        uint16_t listingSize = currentFolder_->GetSubFolderSize();
        obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_LISTING_SIZE, listingSize));
        // no body to seting
    } else {
        auto folderData = currentFolder_->GetData(appParams->listStartOffSet_, appParams->maxListCount_);
        if (folderData.size() == 0) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
            return;
        }
        bodyObj = std::make_shared<ObexArrayBodyObject>();
        bodyObj->Write((uint8_t *)folderData.data(), folderData.size());
        MSE_LOG_INFO("Body : %{public}s ", folderData.c_str());
    }
    resHeader->AppendItemAppParams(obexAppPrarams);
    session.SendGetResponse(req, *resHeader, bodyObj);
}

void MapMseServer::GetOwnerStatus(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (accountItem_.msgType_ == MessageType::IM) {
        if (!CheckConversationId(appParams->conversationId_)) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
            return;
        }
        auto result = stub::MapService::GetInstance()->GetOwnerStatus(appParams->conversationId_);
        ObexTlvParamters obexAppPrarams;
        uint8_t presenceAvlt = result.presenceAvailability;
        std::string presenceText = result.presenceText;
        obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_PRESENCE_AVAILABILITY, presenceAvlt));
        obexAppPrarams.AppendTlvtriplet(
            TlvTriplet(MapMseParams::PARAM_PRESENCE_TEXT, presenceText.size(),
            reinterpret_cast<uint8_t *>(const_cast<char *>(presenceText.c_str()))));
        if (appParams->conversationId_.size() > 0) {
            std::string lastActivity = result.lastActivity;
            uint8_t chatState = result.chatState;
            obexAppPrarams.AppendTlvtriplet(TlvTriplet(
                MapMseParams::PARAM_LAST_ACTIVITY, static_cast<uint8_t>(lastActivity.size()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(lastActivity.c_str()))));
            obexAppPrarams.AppendTlvtriplet(TlvTriplet(MapMseParams::PARAM_CHAT_STATE, chatState));
        }
        auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
        resHeader->AppendItemAppParams(obexAppPrarams);
        session.SendResponse(*resHeader);
        return;
    }
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
}

void MapMseServer::GetMASInstanceInformation(ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (instance_.GetMasId() == *appParams->masInstanceId_) {
        std::string masStr;
        std::string ownerUci;
        GetImEmailMasInfo(masStr, ownerUci);
        auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
        if (masStr.size() > MAP_MAS_INSTANCE_INFORMATION_LENGTH) {
            masStr = masStr.substr(0, MAP_MAS_INSTANCE_INFORMATION_LENGTH);
        }
        auto bodyObj = std::make_shared<ObexArrayBodyObject>();
        bodyObj->Write((uint8_t *)masStr.data(), masStr.size());
        ObexTlvParamters obexAppPrarams;
        obexAppPrarams.AppendTlvtriplet(
            TlvTriplet(MapMseParams::PARAM_OWNER_UCI, static_cast<uint8_t>(ownerUci.size()),
            reinterpret_cast<uint8_t *>(ownerUci.data())));
        resHeader->AppendItemAppParams(obexAppPrarams);
        session.SendGetResponse(req, *resHeader, bodyObj);
        return;
    } else {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::BAD_REQUEST));
        return;
    }
}

void MapMseServer::GetImEmailMasInfo(std::string &masStr, std::string &ownerUci)
{
    if (accountItem_.id_ != "") {
        if (MessageType::EMAIL == accountItem_.msgType_) {
            if (accountItem_.name_ != "") {
                masStr = accountItem_.name_;
            } else {
                masStr = EMAIL_NAME;
            }
        } else if (MessageType::IM == accountItem_.msgType_) {
            masStr = accountItem_.GetUciFullName();
            if (masStr == "") {
                std::stringstream ss;
                ss << std::setw(0x3) << static_cast<int>(instance_.GetMasId());
                masStr = "un" + ss.str() + accountItem_.uci_;
            }
        }
        ownerUci = accountItem_.GetUciFullName();
    } else {
        masStr.append(SMS_MMS_NAME + "_TEL_");
        masStr.append(stub::MapService::GetInstance()->GetSystemTelephonyService().phoneNumber);
        ownerUci = stub::MapService::GetInstance()->GetSystemTelephonyService().phoneNumber;
    }
}

void MapMseServer::SetNotificationRegistration(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (!appParams->notificationStatus_) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
        return;
    }
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SUCCESS));
    content_.SetNotificationRegistration(session.GetRemoteAddr(), *appParams->notificationStatus_);
}

void MapMseServer::SetNotificationFilter(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (appParams->notificationFilterMask_ == -1) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
        return;
    }
    auto const &remoteAddr = session.GetRemoteAddr();
    content_.SetNotificationFilter(remoteAddr, appParams->notificationFilterMask_);
    auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    session.SendResponse(*resHeader);
}

void MapMseServer::SetMessageStatus(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string name;
    if (req.GetItemName()->GetUnicodeText().size() != 0) {
        name = ToUtf8String(req.GetItemName()->GetUnicodeText());
    } else {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_ACCEPTABLE));
        return;
    }
    std::unique_ptr<MapMseParams> appParams = nullptr;
    if (req.GetItemAppParams()) {
        appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
    } else {
        appParams = std::make_unique<MapMseParams>();
    }
    if (appParams->extendedData_ != "") {
        if (!content_.SetExtendedData(name, appParams->extendedData_)) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
            return;
        }
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SUCCESS));
        return;
    }
    if (!appParams->statusIndicator_ || !appParams->statusValue_) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    if (static_cast<uint8_t>(StatusIndicator::SET_EXTENDED_DATA) == *appParams->statusIndicator_ &&
        appParams->extendedData_.size() == 0) {
        session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
        return;
    }
    MessageType messageType = ParseHandle(std::atol(name.c_str()));
    if (static_cast<uint8_t>(StatusIndicator::READ_STATUS) == *appParams->statusIndicator_) {
        if (!content_.SetMessageStatusRead(
            name, messageType, currentFolder_->GetFullPath(), *appParams->statusValue_)) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
            return;
        }
    } else if (static_cast<uint8_t>(StatusIndicator::DELETED_STATUS) == *appParams->statusIndicator_) {
        if (!content_.SetMessageStatusDeleted(
            name, messageType, currentFolder_->GetFullPath(), *appParams->statusValue_)) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
            return;
        }
    }
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SUCCESS));
}

void MapMseServer::UpdateInbox(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (accountItem_.id_.size() != 0) {
        std::string name = MAP_MSE_FOLDER_NAME_INBOX;
        if (req.GetItemName() != nullptr) {
            if (req.GetItemName()->GetUnicodeText().size() != 0) {
                name = ToUtf8String(req.GetItemName()->GetUnicodeText());
            }
        }
        MapMseFolder *inboxFolder = currentFolder_->GetFolderByName(name);
        if (inboxFolder == nullptr) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED));
            return;
        }
        if (accountItem_.pAuthority_.size() != 0) {
            if (stub::MapService::GetInstance()->UpdateFolder(
                accountItem_.id_, std::to_string(inboxFolder->GetFolderId()), instance_.GetMasId())) {
                session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SUCCESS));
                return;
            }
        }
    }
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED));
}

int MapMseServer::SetStatusParam(const MapMseParams &appParams, stub::OwnerStatusParam &statusParam)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (appParams.conversationId_.size() == 0 && appParams.presenceAvailability_ == nullptr &&
        appParams.presenceText_.size() == 0 && appParams.lastActivity_.size() == 0 && appParams.chatState_ == nullptr) {
        return RET_BAD_STATUS;
    } else {
        if (appParams.conversationId_ != "") {
            if (!CheckConversationId(appParams.conversationId_)) {
                return RET_NO_SUPPORT;
            }
            statusParam.conversationId = appParams.conversationId_;
        }
        if (appParams.presenceAvailability_) {
            statusParam.presenceAvailability = *appParams.presenceAvailability_;
        }
        if (appParams.lastActivity_ != "") {
            statusParam.lastActivity = appParams.lastActivity_;
        }
        if (appParams.chatState_) {
            statusParam.chatState = *appParams.chatState_;
        }
    }
    return RET_NO_ERROR;
}

void MapMseServer::SetOwnerStatus(const ObexServerSession &session, const ObexHeader &req)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (accountItem_.msgType_ == MessageType::IM) {
        std::unique_ptr<MapMseParams> appParams = nullptr;
        if (req.GetItemAppParams()) {
            appParams = std::make_unique<MapMseParams>(*req.GetItemAppParams()->GetTlvParamters());
        } else {
            appParams = std::make_unique<MapMseParams>();
        }
        stub::OwnerStatusParam statusParam {};
        auto ret = SetStatusParam(*appParams, statusParam);
        if (ret == RET_BAD_STATUS) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::PRECONDITION_FAILED));
            return;
        } else if (ret == RET_NO_SUPPORT) {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
            return;
        }

        if (stub::MapService::GetInstance()->SetOwnerStatus(statusParam, instance_.GetMasId())) {
            auto resHeader = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
            session.SendResponse(*resHeader);
            return;
        } else {
            session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_IMPLEMENTED));
            return;
        }
    }
    session.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SERVICE_UNAVAILABLE));
}

std::string MapMseServer::ToUtf8String(std::u16string u16str)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(u16str);
}

std::u16string MapMseServer::ToU16String(std::string str)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(str);
}

std::string MapMseServer::GetMseTime(void)
{
    time_t currTime;
    time(&currTime);
    struct tm utctm;
    gmtime_r(&currTime, &utctm);
    char buff[16];
    strftime(buff, sizeof(buff), "%Y%m%{public}dT%H%M%{public}s", &utctm);
    return std::string(buff);
}

MessageType MapMseServer::ParseHandle(uint64_t handle)
{
    if ((handle & MESSAGE_HANDLE_MMS_MASK) != 0) {
        return MessageType::MMS;
    }
    if ((handle & MESSAGE_HANDLE_EMAIL_MASK) != 0) {
        return MessageType::EMAIL;
    }
    if ((handle & MESSAGE_HANDLE_SMS_GSM_MASK) != 0) {
        return MessageType::SMS_GSM;
    }
    if ((handle & MESSAGE_HANDLE_SMS_CDMA_MASK) != 0) {
        return MessageType::SMS_CDMA;
    }
    if ((handle & MESSAGE_HANDLE_IM_MASK) != 0) {
        return MessageType::IM;
    }
    return MessageType::NONE;
}

bool MapMseServer::CheckConversationId(std::string id)
{
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    std::string sql = "select id from Conversation where convo_id = ? ";
    auto stmt = dataProvider->CreateStatement(sql);
    stmt->SetParamString(1, id);
    auto dataResult = stmt->Query();
    return dataResult->Next();
}

void MapMseServer::LoadAdapterConfig()
{
    IAdapterConfig *adpterConfig = AdapterConfig::GetInstance();
    if (!adpterConfig->GetValue(SECTION_MAP_MSE_SERVICE, PROPERTY_SRM_ENABLE, srmEnable_)) {
        MSE_LOG_INFO("Load config %{public}s failure", PROPERTY_SRM_ENABLE.c_str());
        srmEnable_ = true;
    }
    if (!adpterConfig->GetValue(SECTION_MAP_MSE_SERVICE, PROPERTY_RFCOMM_MTU, rfcommMtu_)) {
        MSE_LOG_INFO("Load config %{public}s failure", PROPERTY_RFCOMM_MTU.c_str());
        rfcommMtu_ = OBEX_DEFAULT_MTU;
    }
    if (!adpterConfig->GetValue(SECTION_MAP_MSE_SERVICE, PROPERTY_L2CAP_MTU, l2capMtu_)) {
        MSE_LOG_INFO("Load config %{public}s failure", PROPERTY_L2CAP_MTU.c_str());
        l2capMtu_ = OBEX_DEFAULT_MTU;
    }
}
}  // namespace bluetooth
}  // namespace OHOS