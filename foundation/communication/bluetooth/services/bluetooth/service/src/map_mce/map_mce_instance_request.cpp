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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client instance request object.
 *
 */

/**
 * @file map_mce_instance_request.cpp
 *
 * @brief map instance client instance request file .
 *
 */

#include "map_mce_instance_request.h"
#include <codecvt>
#include <locale>
#include "log.h"
#include "map_mce_instance_stm.h"
#include "map_mce_mns_server.h"

namespace OHOS {
namespace bluetooth {
MapMceInstanceRequest::MapMceInstanceRequest()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    saveFlag_ = false;
    messageType_ = MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL;
    ownerUci_ = "";
    config_.isUseSrmp = false;
    config_.srmpCount = 0;
    const int defaultMaxValue = 100;
    config_.maxOfGetUnread = defaultMaxValue;
}

MapMceInstanceRequest::~MapMceInstanceRequest()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

void MapMceInstanceRequest::SetSupportMessageType(int type)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    messageType_ = type;
}

int MapMceInstanceRequest::GetSupportMessageType() const
{
    return messageType_;
}

void MapMceInstanceRequest::SetOwnerUci(const std::string &owner)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    ownerUci_ = owner;
}

std::string MapMceInstanceRequest::GetOwnerUci() const
{
    return ownerUci_;
}

void MapMceInstanceRequest::SaveReq()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    saveFlag_ = true;
}

int MapMceInstanceRequest::MapMessageTypeToIprofileMask(MapMessageType type) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    uint8_t mask;
    switch (type) {
        case MapMessageType::SMS_GSM:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM;
            break;
        case MapMessageType::SMS_CDMA:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA;
            break;
        case MapMessageType::MMS:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS;
            break;
        case MapMessageType::EMAIL:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL;
            break;
        case MapMessageType::IM:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM;
            break;
        default:
            mask = MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL;
            break;
    }
    return int(mask);
}

MapMessageType MapMceInstanceRequest::IprofileMaskToMapMessageType(int mask) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MapMessageType type;

    switch (mask) {
        case MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM:
            type = MapMessageType::SMS_GSM;
            break;
        case MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA:
            type = MapMessageType::SMS_CDMA;
            break;
        case MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS:
            type = MapMessageType::MMS;

            break;
        case MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL:
            type = MapMessageType::EMAIL;
            break;
        case MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM:
            type = MapMessageType::IM;
            break;
        default:
            type = MapMessageType::INVALID;
            break;
    }
    return type;
}

void MapMceInstanceRequest::SetRequestConfig(MapMceRequestConfig cfg)
{
    LOG_INFO("%{public}s execute,isUseSrmp=%{public}d,srmpCount=%{public}d",
        __PRETTY_FUNCTION__, cfg.isUseSrmp, cfg.srmpCount);
    config_ = cfg;
}

MapMceRequestConfig MapMceInstanceRequest::GetRequestConfig() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return config_;
}

int MapMceInstanceRequest::MapMessageTypeToFrameworkMask(MapMessageType type) const
{
    uint8_t mask;
    switch (type) {
        case MapMessageType::SMS_GSM:
            mask = ~MAP_FILTER_MESSAGE_MASK_SMS_GSM;
            break;
        case MapMessageType::SMS_CDMA:
            mask = ~MAP_FILTER_MESSAGE_MASK_SMS_CDMA;
            break;
        case MapMessageType::MMS:
            mask = ~MAP_FILTER_MESSAGE_MASK_MMS;
            break;
        case MapMessageType::EMAIL:
            mask = ~MAP_FILTER_MESSAGE_MASK_EMAIL;
            break;
        case MapMessageType::IM:
            mask = ~MAP_FILTER_MESSAGE_MASK_IM;
            break;
        default:
            mask = MAP_FILTER_MESSAGE_MASK_NO_FILTERING;
            break;
    }
    LOG_INFO("%{public}s execute,type=%{public}d,mask=%{public}d", __PRETTY_FUNCTION__, int(type), int(mask));
    return int(mask);
}

MapMceRequestSetNotificationFilter::MapMceRequestSetNotificationFilter(const int mask)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    notificationFilterMask_ = mask;
}

MapMceRequestSetNotificationFilter::~MapMceRequestSetNotificationFilter()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    // release memory , if have
}

MceRequestType MapMceRequestSetNotificationFilter::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SET_NOTIFICATION_FILTER;
}

int MapMceRequestSetNotificationFilter::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/MAP-notification-filter"
    header->AppendItemType(MCE_HEADER_TYPE_SET_NOTIFICATION_FILTER);

    // Application Parameters
    uint32_t value = static_cast<uint32_t>(notificationFilterMask_);
    TlvTriplet paramNotif(MCE_NOTIFICATION_FILTER_MASK, value);
    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramNotif);
    header->AppendItemAppParams(appParams);

    // End body
    uint8_t emptyBody = 0x30;
    header->AppendItemEndBody(&emptyBody, 1);

    // send request
    ret = obexIns.Put(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end,FilterMask_ = 0x%x", __PRETTY_FUNCTION__, notificationFilterMask_);
    return ret;
}

int MapMceRequestSetNotificationFilter::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::SET_NOTIFICATION_FILTER;
    return MCE_RESPONSE_FINISH;
}

MapMceRequestSetNotificationRegistration::MapMceRequestSetNotificationRegistration(bool on)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    setupValue_ = on;
}

MapMceRequestSetNotificationRegistration::~MapMceRequestSetNotificationRegistration()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestSetNotificationRegistration::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SET_NOTIFICATION_REGIST;
}

int MapMceRequestSetNotificationRegistration::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/MAP-NotificationRegistration"
    header->AppendItemType(MCE_HEADER_TYPE_SET_NOTIFICATION_REGISTRATION);

    // Application Parameters
    uint8_t value = setupValue_;
    TlvTriplet paramNotificationStatus(MCE_NOTIFICATION_STATUS, value);
    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramNotificationStatus);
    header->AppendItemAppParams(appParams);

    // End body
    uint8_t emptyBody = 0x30;
    header->AppendItemEndBody(&emptyBody, 1);

    // send request
    ret = obexIns.Put(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end, value = 0x%x", __PRETTY_FUNCTION__, setupValue_);
    return ret;
}

int MapMceRequestSetNotificationRegistration::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_RESPONSE_FINISH_NO_CALLBACK;
}

MapMceRequestGetMasInstanceInformation::MapMceRequestGetMasInstanceInformation(uint8_t instId)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    instanceId_ = instId;
}

MapMceRequestGetMasInstanceInformation::~MapMceRequestGetMasInstanceInformation()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetMasInstanceInformation::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_MASINSTANCE;
}

int MapMceRequestGetMasInstanceInformation::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);  // GET or GET_FINAL

    // set header
    // Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/MASInstanceInformation"
    header->AppendItemType(MCE_HEADER_TYPE_GET_MAS_INSTANCE_INFORMATION);

    TlvTriplet paramInstance(MCE_MAS_INSTANCEID, instanceId_);
    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramInstance);
    header->AppendItemAppParams(appParams);

    // make writer object
    std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();

    // send request
    ret = obexIns.Get(*header, writer);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Get error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end, instanceId_ = %{public}d", __PRETTY_FUNCTION__, instanceId_);
    return ret;
}

int MapMceRequestGetMasInstanceInformation::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    MasInstanceInformation instancInfo;
    retAction.action_ = MapActionType::GET_MAS_INSTANCE_INFO;
    // get response code
    int rescode = resp.GetFieldCode();
    if (rescode != uint8_t(ObexRspCode::SUCCESS)) {
        return MCE_RESPONSE_FINISH_NO_CALLBACK;
    }
    // init
    instancInfo.masInstanceInfoUtf8 = u8"";
    instancInfo.ownerUciUtf8 = u8"";
    // OwnerUCI, Text UTF-8
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto tlvParam = appParam->GetTlvtriplet(MCE_OWNER_UCI);  // OwnerUCI 0X27
        if (tlvParam != nullptr) {
            int ownerUciLength = tlvParam->GetLen();
            const uint8_t *val = tlvParam->GetVal();
            if ((ownerUciLength != 0) && (val != nullptr)) {
                std::string tempUci(reinterpret_cast<char *>(const_cast<uint8_t *>(val)), ownerUciLength);
                instancInfo.ownerUciUtf8 = tempUci;
            }
        }
    }
    // MASInstanceInformation is a string with the requested user-readable information of the MAS-instance.
    auto bodyObject = resp.GetExtendBodyObject();
    uint8_t masInfBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
    size_t masInfBufLen = 1;
    std::ostringstream masInfStream;
    while ((masInfBufLen != 0) && (bodyObject != 0)) {
        masInfBufLen = bodyObject->Read(masInfBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
        if ((masInfBufLen != 0) && (masInfBufLen < MCE_MAX_LENGTH_OF_RESPONSE_BUFFER)) {
            masInfBuf[masInfBufLen] = 0;
            masInfStream << (char *)masInfBuf;
        }
    }
    if (masInfStream.str().size() != 0) {
        instancInfo.masInstanceInfoUtf8 = masInfStream.str();
    }
    // Save instance information
    instancInfo.isValid = true;
    clientInst.SaveMasInstanceInformation(instancInfo);
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return MCE_RESPONSE_FINISH_NO_CALLBACK;
}

MapMceRequestGetFolderListing::MapMceRequestGetFolderListing(uint16_t listCnt, uint16_t offset)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    maxOfListCount_ = listCnt;
    startOffset_ = offset;
}

MapMceRequestGetFolderListing::~MapMceRequestGetFolderListing()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetFolderListing::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_FOLDERLISTING;
}

int MapMceRequestGetFolderListing::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);

    // set header
    // Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-obex/folder-listing"
    header->AppendItemType(MCE_HEADER_TYPE_GET_FOLDER_LISTING);

    TlvTriplet paramListCount(MCE_MAX_LIST_COUNT, maxOfListCount_);
    TlvTriplet paramStartOffset(MCE_LIST_START_OFF_SET, startOffset_);
    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramListCount);
    appParams.AppendTlvtriplet(paramStartOffset);
    header->AppendItemAppParams(appParams);

    // make writer object
    std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();

    // send request
    ret = obexIns.Get(*header, writer);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Get error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceRequestGetFolderListing::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int rescode;
    int length;
    retAction.action_ = MapActionType::GET_MAS_INSTANCE_INFO;
    // get response code
    rescode = resp.GetFieldCode();
    if (rescode != uint8_t(ObexRspCode::SUCCESS)) {
        return MCE_RESPONSE_FINISH_NO_CALLBACK;
    }
    // listSize_
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto tlvParam = appParam->GetTlvtriplet(MCE_FOLDER_LISTING_SIZE);  // OwnerUCI 0X27
        if (tlvParam != nullptr) {
            length = tlvParam->GetLen();
            const uint8_t *val = tlvParam->GetVal();
            if ((length != 0) && (val != nullptr)) {
                uint16_t tempVal = *(reinterpret_cast<uint16_t *>(const_cast<uint8_t *>(val)));
                listSize_ = ((tempVal >> MCE_8BIT) & MCE_16BIT_MASK_LEFT_LOW) |
                            ((tempVal << MCE_8BIT) & MCE_16BIT_MASK_LEFT_HIGH);
            }
        }
    } else {
        LOG_INFO("%{public}s ItemAppParams nullptr", __PRETTY_FUNCTION__);
    }
    // get body
    auto data = resp.GetExtendBodyObject();
    uint8_t folderBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
    size_t folderBufLen = 1;
    std::ostringstream folderStream;
    while ((folderBufLen != 0) && (data != 0)) {
        folderBufLen = data->Read(folderBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
        if ((folderBufLen != 0) && (folderBufLen < MCE_MAX_LENGTH_OF_RESPONSE_BUFFER)) {
            folderBuf[folderBufLen] = 0;
            folderStream << (char *)folderBuf;
        }
    }
    if (folderStream.str().size() != 0) {
        folderListingParam_.folderListingStringObject_ = folderStream.str();
        MceTypesFolderListing folderListing;
        folderListing.BuildObjectData(folderListingParam_.folderListingStringObject_);
        folderListingParam_.folderNameList_ = folderListing.GetList();
        folderListingParam_.version_ = folderListing.GetVersion();
    }
#ifdef MCE_DEBUG_RECEIVE_DATA
    LOG_INFO("%{public}s folderListing=%{public}s", __PRETTY_FUNCTION__, folderListingParam_.folderListingStringObject_.c_str());
#endif
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return MCE_RESPONSE_FINISH_NO_CALLBACK;
}

MapMceRequestSetPath::MapMceRequestSetPath(uint8_t flags, const std::u16string &paths,
    const std::vector<std::u16string> &pathsList)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    pathFlags_ = flags;
    pathsString_ = paths;
    pathsList_ = pathsList;
}

MapMceRequestSetPath::~MapMceRequestSetPath()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestSetPath::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SET_PATH;
}

int MapMceRequestSetPath::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    if (pathFlags_ == MASSTM_SETPATH_LIST) {
        // path string list
        ret = obexIns.SetPath(pathsList_);
    } else {
        // normal string path and path up/down
        ret = obexIns.SetPath(pathFlags_, pathsString_);
    }

    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex setpath error", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceRequestSetPath::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_RESPONSE_FINISH_NO_CALLBACK;
}

MapMceRequestPushMessage::MapMceRequestPushMessage(const IProfileSendMessageParameters &msg)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    messageParam_ = msg;
    SetSupportMessageType(MapMessageTypeToIprofileMask(messageParam_.bmessage_.type_property));
}

MapMceRequestPushMessage::~MapMceRequestPushMessage()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestPushMessage::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SEND_MESSAGE;
}

int MapMceRequestPushMessage::SendRequest(ObexMpClient &obexIns)
{
    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);
    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/message"
    header->AppendItemType(MCE_HEADER_TYPE_MESSAGE);
    // Name of the Folder
    header->AppendItemName(messageParam_.bmessage_.folder_property);
    ObexTlvParamters appParams;
    if (messageParam_.Transparent != MapOnOffType::INVALID) {
        TlvTriplet paramTransparent(MCE_TRANSPARENT, uint8_t(messageParam_.Transparent));
        appParams.AppendTlvtriplet(paramTransparent);
    }
    if (messageParam_.Retry != MapOnOffType::INVALID) {
        TlvTriplet paramRetry(MCE_RETRY, uint8_t(messageParam_.Retry));
        appParams.AppendTlvtriplet(paramRetry);
    }
    TlvTriplet paramCharset(MCE_CHARSET, uint8_t(messageParam_.Charset));
    appParams.AppendTlvtriplet(paramCharset);
    if (messageParam_.MessageHandle.size() != 0) {
        TlvTriplet paramMessageHandle(MCE_MESSAGE_HANDLE,
            (messageParam_.MessageHandle.size() + 1),
            (const uint8_t *)messageParam_.MessageHandle.c_str());
        appParams.AppendTlvtriplet(paramMessageHandle);
        TlvTriplet paramAttachment(MCE_ATTACHMENT, uint8_t(messageParam_.Attachment));
        appParams.AppendTlvtriplet(paramAttachment);
        // This application parameter shall be included if the MessageHandle application parameter is present.
        TlvTriplet paramModifyText(MCE_MODIFY_TEXT, uint8_t(messageParam_.ModifyText));
        appParams.AppendTlvtriplet(paramModifyText);
    }
    if (messageParam_.ConversationID.size() != 0) {
        TlvTriplet paramConversationID(MCE_CONVERSATION_ID,
            messageParam_.ConversationID.size() + 1,
            (const uint8_t *)messageParam_.ConversationID.c_str());
        appParams.AppendTlvtriplet(paramConversationID);
    }
    header->AppendItemAppParams(appParams);
    // End body , body is bMessage
    MceBmessageParamMakeStringObject makeString(messageParam_.bmessage_);
    std::string msgBody = makeString.GetStringObject();
    // make writer object
    std::shared_ptr<ObexBodyObject> read = std::make_shared<ObexArrayBodyObject>();
    read->Write((const uint8_t *)msgBody.c_str(), msgBody.size());
    // send request
    int ret = obexIns.Put(*header, read);
    LOG_INFO("%{public}s execute end, obex Put ret=%{public}d ", __PRETTY_FUNCTION__, ret);
#ifdef MCE_DEBUG_SEND_DATA
    LOG_INFO("PushMessage,StringObject = %{public}s", msgBody.c_str());
#endif
    return ret;
}

int MapMceRequestPushMessage::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::SEND_MESSAGE;
    return MCE_RESPONSE_FINISH;
}

MapMceRequestGetMessage::MapMceRequestGetMessage(
    const std::u16string &msgHandle, const IProfileGetMessageParameters &para)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    msgHandle_ = msgHandle;
    msgPara_ = para;
}

MapMceRequestGetMessage::~MapMceRequestGetMessage()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetMessage::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_MESSAGE;
}

int MapMceRequestGetMessage::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);
    // set header
    // Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/MAP-msg-listing"
    header->AppendItemType(MCE_HEADER_TYPE_MESSAGE);
    header->AppendItemName(msgHandle_);

    if (GetRequestConfig().isUseSrmp) {
        LOG_ERROR("%{public}s warning: srmp only valid in pts test!", __PRETTY_FUNCTION__);
        header->AppendItemSrmp();
    }

    // Application Parameters:
    TlvTriplet paramAttachment(MCE_ATTACHMENT, uint8_t(msgPara_.Attachment));
    TlvTriplet paramCharset(MCE_CHARSET, uint8_t(msgPara_.Charset));
    TlvTriplet paramFractionRequest(MCE_FRACTION_REQUEST, uint8_t(msgPara_.FractionRequest));
    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramAttachment);
    appParams.AppendTlvtriplet(paramCharset);
    appParams.AppendTlvtriplet(paramFractionRequest);
    header->AppendItemAppParams(appParams);
#ifdef MCE_DEBUG_SEND_DATA
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::string utf8Handle = converter.to_bytes(msgHandle_);
    LOG_INFO("Send GetMessage msgHandle_=%{public}s", utf8Handle.c_str());
    LOG_INFO("Send GetMessage Attachment=%hhu", msgPara_.Attachment);
    LOG_INFO("Send GetMessage Charset=%hhu", msgPara_.Charset);
    LOG_INFO("Send GetMessage FractionRequest=%hhu", msgPara_.FractionRequest);
#endif
    // make writer object
    std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();
    // send request
    if (GetRequestConfig().isUseSrmp) {
        LOG_ERROR("%{public}s warning: srmp only valid in pts test!", __PRETTY_FUNCTION__);
        ret = obexIns.Get(*header, writer, GetRequestConfig().srmpCount);
    } else {
        ret = obexIns.Get(*header, writer);
    }
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex put error", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceRequestGetMessage::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    retAction.action_ = MapActionType::GET_MESSAGE;
    MceTypesBMessage *bmessageData = &retAction.bmessage_;
    if (resp.GetFieldCode() != uint8_t(ObexRspCode::SUCCESS)) {
        return MCE_RESPONSE_FINISH;
    }
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto tlvParam1 = appParam->GetTlvtriplet(MCE_FRACTION_DELIVER);
        if (tlvParam1 != nullptr) {
            int length = tlvParam1->GetLen();
            const uint8_t *val1 = tlvParam1->GetVal();
            uint8_t deliveryType = int(MapFractionDeliverType::INVALID);
            if ((length != 0) && (val1 != nullptr)) {
                deliveryType = *val1;
            }
            if (deliveryType == 0) {
                bmessageData->SetFractionDeliver(MapFractionDeliverType::MORE);
            } else if (deliveryType == 1) {
                bmessageData->SetFractionDeliver(MapFractionDeliverType::LAST);
            } else {
                bmessageData->SetFractionDeliver(MapFractionDeliverType::INVALID);
            }
        }
    }
    auto msgObject = resp.GetExtendBodyObject();
    uint8_t msgBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
    size_t bufLen = 1;
    stream_.clear();
    while ((bufLen != 0) && (msgObject != 0)) {
        bufLen = msgObject->Read(msgBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
        if ((bufLen != 0) && (bufLen < MCE_MAX_LENGTH_OF_RESPONSE_BUFFER)) {
            msgBuf[bufLen] = 0;
            stream_ << (char *)msgBuf;
        }
    }
    if (stream_.str().size() != 0) {
        bmessageData->BuildObjectData(stream_.str());
    }
#ifdef MCE_DEBUG_RECEIVE_DATA
    LOG_INFO("GetMessage msgObject=%{public}s", bmessageData->GetBmessageObject().c_str());
#endif
    if (bmessageData->GetBMessageData().type_property != IprofileMaskToMapMessageType(GetSupportMessageType())) {
        return MCE_RESPONSE_FINISH_NG;
    } else {
        return MCE_RESPONSE_FINISH;
    }
}

MapMceRequestGetUreadMessages::MapMceRequestGetUreadMessages(
    MapMessageType type, const IProfileGetMessagesListingParameters &para)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    requestPtr_ = nullptr;
    obexInsBackup_ = nullptr;
    msgHandleList_.clear();
    listParam_ = para;
    listParam_.FilterMessageType = MapMessageTypeToFrameworkMask(type);
    SetSupportMessageType(MapMessageTypeToIprofileMask(type));
}

MapMceRequestGetUreadMessages::~MapMceRequestGetUreadMessages()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetUreadMessages::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_UNREAD_MESSAGES;
}

int MapMceRequestGetUreadMessages::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    MapMceRequestConfig cfg = GetRequestConfig();

    obexInsBackup_ = &obexIns;
    if (listParam_.MaxListCount > cfg.maxOfGetUnread) {
        listParam_.MaxListCount = cfg.maxOfGetUnread;
    }
    requestPtr_ = std::make_unique<MapMceRequestGetMessagesListing>(listParam_);
    ret = requestPtr_->SendRequest(obexIns);
    return ret;
}

int MapMceRequestGetUreadMessages::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    int ret = MCE_RESPONSE_FINISH_NG;
    retAction.action_ = MapActionType::GET_UNREAD_MESSAGES;
    if (requestPtr_ == nullptr) {
        return ret;
    }
    LOG_INFO("%{public}s request type = %{public}d", __PRETTY_FUNCTION__, requestPtr_->GetRequestType());
    if (requestPtr_->GetRequestType() == MCE_REQUEST_TYPE_GET_MESSAGELISTING) {
        MapRequestResponseAction ureadAction;
        requestPtr_->ProcessResponse(clientInst, resp, ureadAction);
        auto msgOutlineList = ureadAction.messageList_.GetList();
        for (auto it = msgOutlineList.begin(); it != msgOutlineList.end(); it++) {
            IProfileMessageOutline msgOutline = *it;
            if (msgOutline.handle != "") {
                msgHandleList_.push_back(msgOutline.handle);
                ret = MCE_RESPONSE_CONTINUE_NO_CALLBACK;
            }
        }
    } else if (requestPtr_->GetRequestType() == MCE_REQUEST_TYPE_GET_MESSAGE) {
        requestPtr_->ProcessResponse(clientInst, resp, retAction);
        ret = MCE_RESPONSE_CONTINUE_WITH_CALLBACK;
        if (msgHandleList_.size() == 0) {
            ret = MCE_RESPONSE_FINISH;  // normal finish
        }
    }
    if (msgHandleList_.size() != 0) {
        std::string tempStr = msgHandleList_.front();
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        std::u16string msgHandle = converter.from_bytes(tempStr);
        msgHandleList_.pop_front();
        IProfileGetMessageParameters para;
        para.Attachment = MapAttachmentType::NO;               // no Attachment
        para.Charset = MapCharsetType::UTF_8;                  // 0 is "native" , 1 is "UTF-8"
        para.FractionRequest = MapFractionRequestType::FIRST;  // 0 is "first" , 1 is "next"
        requestPtr_ = std::make_unique<MapMceRequestGetMessage>(msgHandle, para);
        requestPtr_->SetSupportMessageType(GetSupportMessageType());
        int sendRet = requestPtr_->SendRequest(*obexInsBackup_);
        if (sendRet != RET_NO_ERROR) {
            msgHandleList_.clear();
            ret = MCE_RESPONSE_FINISH_NG;
        }
    }
    return ret;
}

MapMceRequestUpdateInbox::MapMceRequestUpdateInbox()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MapMceRequestUpdateInbox::~MapMceRequestUpdateInbox()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestUpdateInbox::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_UPDATE_INBOX;
}

int MapMceRequestUpdateInbox::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;
    uint8_t emptyBody = 0x30;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is “x-bt/MAP-messageUpdate”
    header->AppendItemType(MCE_HEADER_TYPE_MESSAGE_UPDATE);
    // End body
    header->AppendItemEndBody(&emptyBody, 1);
    // send request
    ret = obexIns.Put(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceRequestUpdateInbox::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::UPDATE_INBOX;
    return MCE_RESPONSE_FINISH;
}

MapMceRequestSetOwnerStatus::MapMceRequestSetOwnerStatus(const IProfileSetOwnerStatusParameters &para)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    ownerPara_ = para;
}

MapMceRequestSetOwnerStatus::~MapMceRequestSetOwnerStatus()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestSetOwnerStatus::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SET_OWNER_STATUS;
}

int MapMceRequestSetOwnerStatus::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/ownerStatus"
    header->AppendItemType(MCE_HEADER_TYPE_SET_OWNER_STATUS);

    // Application Parameters:
    ObexTlvParamters appParams;

    TlvTriplet paramApplication(MCE_PRESENCE_AVAILABILITY, ownerPara_.ownerStatus_.PresenceAvailability);
    if (ownerPara_.ownerStatus_.PresenceText != "") {
        TlvTriplet paramPresenceText(MCE_PRESENCE_TEXT,
            ownerPara_.ownerStatus_.PresenceText.size() + 1,
            (const uint8_t *)ownerPara_.ownerStatus_.PresenceText.c_str());
        appParams.AppendTlvtriplet(paramPresenceText);
    }
    if (ownerPara_.ownerStatus_.LastActivity != "") {
        TlvTriplet paramLastActivity(MCE_LAST_ACTIVITY,
            ownerPara_.ownerStatus_.LastActivity.size() + 1,
            (const uint8_t *)ownerPara_.ownerStatus_.LastActivity.c_str());
        appParams.AppendTlvtriplet(paramLastActivity);
    }
    if (ownerPara_.ConversationID != "") {
        TlvTriplet paramConversationID(MCE_CONVERSATION_ID,
            ownerPara_.ConversationID.size() + 1,
            (const uint8_t *)ownerPara_.ConversationID.c_str());
        appParams.AppendTlvtriplet(paramConversationID);
    }
    TlvTriplet paramChatState(MCE_CHAT_STATE, ownerPara_.ownerStatus_.ChatState);

    appParams.AppendTlvtriplet(paramApplication);
    appParams.AppendTlvtriplet(paramChatState);
    header->AppendItemAppParams(appParams);

    // End body
    uint8_t emptyBody = 0x30;
    header->AppendItemEndBody(&emptyBody, 1);

    // send request
    ret = obexIns.Put(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int MapMceRequestSetOwnerStatus::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::SET_OWNER_STATUS;
    return MCE_RESPONSE_FINISH;
}

MapMceRequestGetOwnerStatus::MapMceRequestGetOwnerStatus(const std::string &conversationId)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    ownerConversationId_ = conversationId;
}

MapMceRequestGetOwnerStatus::~MapMceRequestGetOwnerStatus()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetOwnerStatus::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_OWNER_STATUS;
}

int MapMceRequestGetOwnerStatus::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/ownerStatus", the string is same as set owner status
    header->AppendItemType(MCE_HEADER_TYPE_SET_OWNER_STATUS);

    // Application Parameters:
    if (ownerConversationId_.size() != 0) {
        TlvTriplet paramConversationID(
            MCE_CONVERSATION_ID, ownerConversationId_.size(), (const uint8_t *)ownerConversationId_.c_str());
        ObexTlvParamters appParams;
        appParams.AppendTlvtriplet(paramConversationID);
        header->AppendItemAppParams(appParams);
    }

    // send request
    ret = obexIns.Get(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end, ownerConversationId_ = %{public}s", __PRETTY_FUNCTION__, ownerConversationId_.c_str());
    return ret;
}

int MapMceRequestGetOwnerStatus::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    retAction.action_ = MapActionType::GET_OWNER_STATUS;
    // get response code
    int rescode = resp.GetFieldCode();
    if (rescode != uint8_t(ObexRspCode::SUCCESS)) {
        return MCE_RESPONSE_FINISH;
    }
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto tlvParam1 = appParam->GetTlvtriplet(MCE_PRESENCE_AVAILABILITY);
        if (tlvParam1 != nullptr) {
            int length1 = tlvParam1->GetLen();
            const uint8_t *val1 = tlvParam1->GetVal();
            if ((length1 != 0) && (val1 != nullptr)) {
                retAction.ownerStatus_.PresenceAvailability = *val1;
            }
        }
        auto tlvParam2 = appParam->GetTlvtriplet(MCE_PRESENCE_TEXT);
        if (tlvParam2 != nullptr) {
            int length2 = tlvParam2->GetLen();
            const uint8_t *val2 = tlvParam2->GetVal();
            if ((length2 != 0) && (val2 != nullptr)) {
                std::string tempPresence(reinterpret_cast<char *>(const_cast<uint8_t *>(val2)), length2);
                retAction.ownerStatus_.PresenceText = tempPresence;
            }
        }
        auto tlvParam3 = appParam->GetTlvtriplet(MCE_LAST_ACTIVITY);
        if (tlvParam3 != nullptr) {
            int length3 = tlvParam3->GetLen();
            const uint8_t *val3 = tlvParam3->GetVal();
            if ((length3 != 0) && (val3 != nullptr)) {
                std::string tempLastAct(reinterpret_cast<char *>(const_cast<uint8_t *>(val3)), length3);
                retAction.ownerStatus_.LastActivity = tempLastAct;
            }
        }
        auto tlvParam4 = appParam->GetTlvtriplet(MCE_CHAT_STATE);
        if (tlvParam4 != nullptr) {
            int length4 = tlvParam4->GetLen();
            const uint8_t *val4 = tlvParam4->GetVal();
            if ((length4 != 0) && (val4 != nullptr)) {
                retAction.ownerStatus_.ChatState = *val4;
            }
        }
    }
    return MCE_RESPONSE_FINISH;
}

MapMceRequestSetMessageStatus::MapMceRequestSetMessageStatus(const std::u16string &msgHandle,
    MapStatusIndicatorType statusIndicator, MapStatusValueType statusValue, const std::string &exData)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    msgHandle_ = msgHandle;
    msgStatusIndicator_ = uint8_t(statusIndicator);
    msgstatusValue_ = uint8_t(statusValue);
    extendedData_ = exData;
}

MapMceRequestSetMessageStatus::~MapMceRequestSetMessageStatus()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestSetMessageStatus::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_SET_MESSAGE_STATUS;
}

int MapMceRequestSetMessageStatus::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::PUT_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Message Handle
    header->AppendItemName(msgHandle_);
    // Type is "x-bt/messageStatus"
    header->AppendItemType(MCE_HEADER_TYPE_SET_MESSAGE_STATUS);

    // Application Parameters:
    TlvTriplet paramStatusIndicator(MCE_STATUS_INDICATOR, msgStatusIndicator_);
    TlvTriplet paramStatusValue(MCE_STATUS_VALUE, msgstatusValue_);

    ObexTlvParamters appParams;
    appParams.AppendTlvtriplet(paramStatusIndicator);
    appParams.AppendTlvtriplet(paramStatusValue);

    // extendedData_ is optional
    if (extendedData_ != "") {
        TlvTriplet extendedDataParam(
            MCE_EXTENDED_DATA, extendedData_.size() + 1, (const uint8_t *)extendedData_.c_str());
        appParams.AppendTlvtriplet(extendedDataParam);
    }
    header->AppendItemAppParams(appParams);
    // End body
    uint8_t emptyBody = 0x30;
    header->AppendItemEndBody(&emptyBody, 1);

    // send request
    ret = obexIns.Put(*header);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    return ret;
}

int MapMceRequestSetMessageStatus::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::SET_MESSAGE_STATUS;
    return MCE_RESPONSE_FINISH;
}

MapMceRequestGetConversationListing::MapMceRequestGetConversationListing(
    const IProfileGetConversationListingParameters &para)

{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    converPara_ = para;
}

MapMceRequestGetConversationListing::~MapMceRequestGetConversationListing()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetConversationListing::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_CONVERSATION_LISTING;
}

void MapMceRequestGetConversationListing::SetAppendParam(ObexTlvParamters &appParams)
{
    TlvTriplet paramMaxListCount(MCE_MAX_LIST_COUNT, converPara_.MaxListCount);
    appParams.AppendTlvtriplet(paramMaxListCount);

    TlvTriplet paramListStartOffset(MCE_LIST_START_OFF_SET, converPara_.ListStartOffset);
    appParams.AppendTlvtriplet(paramListStartOffset);

    if (converPara_.FilterLastActivityBegin.size() != 0) {
        TlvTriplet paramFilterBegin(MCE_FILTER_LAST_ACTIVITY_BEGIN,
            converPara_.FilterLastActivityBegin.size() + 1,
            (const uint8_t *)converPara_.FilterLastActivityBegin.c_str());
        appParams.AppendTlvtriplet(paramFilterBegin);
    }

    if (converPara_.FilterLastActivityEnd.size() != 0) {
        TlvTriplet paramFilterEnd(MCE_FILTER_LAST_ACTIVITY_END,
            converPara_.FilterLastActivityEnd.size() + 1,
            (const uint8_t *)converPara_.FilterLastActivityEnd.c_str());
        appParams.AppendTlvtriplet(paramFilterEnd);
    }

    TlvTriplet paramFilterRead(MCE_FILTER_READ_STATUS, converPara_.FilterReadStatus);
    appParams.AppendTlvtriplet(paramFilterRead);

    if (converPara_.FilterRecipient.size()) {
        TlvTriplet paramFilterRecipient(MCE_FILTER_RECIPIENT,
            converPara_.FilterRecipient.size(),
            (const uint8_t *)converPara_.FilterRecipient.c_str());
        appParams.AppendTlvtriplet(paramFilterRecipient);
    }

    if (converPara_.ConversationID.size()) {
        TlvTriplet paramConversationID(MCE_CONVERSATION_ID,
            converPara_.ConversationID.size(),
            (const uint8_t *)converPara_.ConversationID.c_str());
        appParams.AppendTlvtriplet(paramConversationID);
    }

    TlvTriplet paramConvMask(MCE_CONV_PARAMETER_MASK, converPara_.ConvParameterMask);
    appParams.AppendTlvtriplet(paramConvMask);
}

int MapMceRequestGetConversationListing::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);

    // set header
    // set Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // type is "x-bt/MAP-convo-listing"
    header->AppendItemType(MCE_HEADER_TYPE_GET_CONVO_LISTING);

    // Application Parameters:
    ObexTlvParamters appParams;
    SetAppendParam(appParams);

    header->AppendItemAppParams(appParams);

    // make writer object
    std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();

    // send request
    ret = obexIns.Get(*header, writer);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex Put error,ret=%{public}d ", __PRETTY_FUNCTION__, ret);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void MapMceRequestGetConversationListing::MapMceGetItemAppParams(
    IProfileConversationListingParamStruct &data, const ObexHeader &resp)
{
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto convListVerCnt =
            appParam->GetTlvtriplet(MCE_CONVERSATION_LISTING_VERSION_COUNTER);  // 128-bit value in hex string format
        if (convListVerCnt != nullptr) {
            int convLenth = convListVerCnt->GetLen();
            const uint8_t *val1 = convListVerCnt->GetVal();
            if ((convLenth != 0) && (val1 != nullptr)) {
                std::string tempCounter(reinterpret_cast<char *>(const_cast<uint8_t *>(val1)), convLenth);
                data.ConversationListingVersionCounter = tempCounter;
            }
        }
        auto convMseTim = appParam->GetTlvtriplet(MCE_MSE_TIME);
        if (convMseTim != nullptr) {
            int convMseLen = convMseTim->GetLen();
            const uint8_t *val2 = convMseTim->GetVal();
            if ((convMseLen != 0) && (val2 != nullptr)) {
                std::string tempTime(reinterpret_cast<char *>(const_cast<uint8_t *>(val2)), convMseLen);
                data.MseTime = tempTime;
            }
        }
        auto convListSize = appParam->GetTlvtriplet(MCE_LISTING_SIZE);
        if (convListSize != nullptr) {
            int convListLength = convListSize->GetLen();
            const uint8_t *val3 = convListSize->GetVal();
            if ((convListLength != 0) && (val3 != nullptr)) {
                uint16_t tempVal = *(reinterpret_cast<uint16_t *>(const_cast<uint8_t *>(val3)));
                data.ListingSize = ((tempVal >> MCE_8BIT) & MCE_16BIT_MASK_LEFT_LOW) |
                                   ((tempVal << MCE_8BIT) & MCE_16BIT_MASK_LEFT_HIGH);
            }
        }
        auto convDbId = appParam->GetTlvtriplet(MCE_DATABASE_IDENTIFIER);
        if (convDbId != nullptr) {
            int convDbIdLenth = convDbId->GetLen();
            const uint8_t *val4 = convDbId->GetVal();
            if ((convDbIdLenth != 0) && (val4 != nullptr)) {
                std::string tempDatabase(reinterpret_cast<char *>(const_cast<uint8_t *>(val4)), convDbIdLenth);
                data.DatabaseIdentifier = tempDatabase;
            }
        }
    }
}

int MapMceRequestGetConversationListing::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::GET_CONVERSATION_LISTING;
    IProfileConversationListingParamStruct paramData;
    int rescode;

    // get response code
    rescode = resp.GetFieldCode();
    if (rescode == uint8_t(ObexRspCode::SUCCESS)) {
        MapMceGetItemAppParams(paramData, resp);
        // get body
        auto convObject = resp.GetExtendBodyObject();
        uint8_t convBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
        size_t bufLen = 1;
        std::ostringstream convStream;
        while ((bufLen != 0) && (convObject != 0)) {
            bufLen = convObject->Read(convBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
            if ((bufLen != 0) && (bufLen < MCE_MAX_LENGTH_OF_RESPONSE_BUFFER)) {
                convBuf[bufLen] = 0;
                convStream << (char *)convBuf;
            }
        }
        if (convStream.str().size() != 0) {
            retAction.conversationList_.BuildObjectData(paramData, convStream.str());
        }
#ifdef MCE_DEBUG_RECEIVE_DATA
        LOG_INFO("GetConversationListing list size=%{public}d, strlength=%{public}d, convStream size=%{public}d",
            int(retAction.conversationList_.GetList().size()),
            int(retAction.conversationList_.GetStringObject().size()),
            int(convStream.str().size()));
        LOG_INFO("GetConversationListing stringObject=%{public}s", retAction.conversationList_.GetStringObject().c_str());
#endif
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return MCE_RESPONSE_FINISH;
}

MapMceRequestGetMessagesListing::MapMceRequestGetMessagesListing(const IProfileGetMessagesListingParameters &para)

{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int messagType = MAP_MCE_SUPPORTED_MESSAGE_TYPE_ALL;
    msgPara_ = para;

    if ((msgPara_.FilterMessageType & MAP_FILTER_MESSAGE_MASK_SMS_GSM) != 0) {  // filter out gsm
        messagType &= ~MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_GSM;
    }
    if ((msgPara_.FilterMessageType & MAP_FILTER_MESSAGE_MASK_SMS_CDMA) != 0) {  // filter out cdma
        messagType &= ~MAP_MCE_SUPPORTED_MESSAGE_TYPE_SMS_CDMA;
    }
    if ((msgPara_.FilterMessageType & MAP_FILTER_MESSAGE_MASK_EMAIL) != 0) {  // filter out email
        messagType &= ~MAP_MCE_SUPPORTED_MESSAGE_TYPE_EMAIL;
    }
    if ((msgPara_.FilterMessageType & MAP_FILTER_MESSAGE_MASK_MMS) != 0) {  // filter out mms
        messagType &= ~MAP_MCE_SUPPORTED_MESSAGE_TYPE_MMS;
    }
    if ((msgPara_.FilterMessageType & MAP_FILTER_MESSAGE_MASK_IM) != 0) {  // filter out im
        messagType &= ~MAP_MCE_SUPPORTED_MESSAGE_TYPE_IM;
    }
    SetSupportMessageType(messagType);
}

MapMceRequestGetMessagesListing::~MapMceRequestGetMessagesListing()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceRequestType MapMceRequestGetMessagesListing::GetRequestType()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MCE_REQUEST_TYPE_GET_MESSAGELISTING;
}

void MapMceRequestGetMessagesListing::SendRequestSetAppParamsStep1(ObexTlvParamters &appParams)
{
    TlvTriplet paramListCount(MCE_MAX_LIST_COUNT, msgPara_.MaxListCount);
    appParams.AppendTlvtriplet(paramListCount);

    TlvTriplet paramStartOffset(MCE_LIST_START_OFF_SET, msgPara_.ListStartOffset);
    appParams.AppendTlvtriplet(paramStartOffset);

    TlvTriplet paramSubjectLength(MCE_SUBJECT_LENGTH, msgPara_.SubjectLength);
    appParams.AppendTlvtriplet(paramSubjectLength);

    TlvTriplet paramParameterMask(MCE_PARAMETER_MASK, msgPara_.ParameterMask);
    appParams.AppendTlvtriplet(paramParameterMask);

    TlvTriplet paramFilMsgType(MCE_FILTER_MESSAGE_TYPE, msgPara_.FilterMessageType);
    appParams.AppendTlvtriplet(paramFilMsgType);

    TlvTriplet paramFilterPriority(MCE_FILTER_PRIORITY, msgPara_.FilterPriority);
    appParams.AppendTlvtriplet(paramFilterPriority);

    TlvTriplet paramFilterRead(MCE_FILTER_READ_STATUS, msgPara_.FilterReadStatus);
    appParams.AppendTlvtriplet(paramFilterRead);
}

void MapMceRequestGetMessagesListing::SendRequestSetAppParamsStep2(ObexTlvParamters &appParams)
{
    if (msgPara_.FilterPeriodBegin.size() != 0) {
        TlvTriplet paramFilBegin(MCE_FILTER_PERIOD_BEGIN,
            msgPara_.FilterPeriodBegin.size() + 1,
            (const uint8_t *)msgPara_.FilterPeriodBegin.c_str());
        appParams.AppendTlvtriplet(paramFilBegin);
    }
    if (msgPara_.FilterPeriodEnd.size() != 0) {
        TlvTriplet paramFilEnd(MCE_FILTER_PERIOD_END,
            msgPara_.FilterPeriodEnd.size() + 1,
            (const uint8_t *)msgPara_.FilterPeriodEnd.c_str());
        appParams.AppendTlvtriplet(paramFilEnd);
    }
    if (msgPara_.FilterRecipient.size() != 0) {
        TlvTriplet paramFilterRecipient(MCE_FILTER_RECIPIENT,
            msgPara_.FilterRecipient.size() + 1,
            (const uint8_t *)msgPara_.FilterRecipient.c_str());
        appParams.AppendTlvtriplet(paramFilterRecipient);
    }
    if (msgPara_.FilterOriginator.size() != 0) {
        TlvTriplet paramOriginat(MCE_FILTER_ORIGINATOR,
            msgPara_.FilterOriginator.size() + 1,
            (const uint8_t *)msgPara_.FilterOriginator.c_str());
        appParams.AppendTlvtriplet(paramOriginat);
    }
    if (msgPara_.ConversationID.size() != 0) {
        TlvTriplet paramConversationID(
            MCE_CONVERSATION_ID, msgPara_.ConversationID.size() + 1, (const uint8_t *)msgPara_.ConversationID.c_str());
        appParams.AppendTlvtriplet(paramConversationID);
    }
    if (msgPara_.FilterMessageHandle.size() != 0) {
        TlvTriplet paramMsgHandle(MCE_FILTER_MESSAGE_HANDLE,
            msgPara_.FilterMessageHandle.size() + 1,
            (const uint8_t *)msgPara_.FilterMessageHandle.c_str());
        appParams.AppendTlvtriplet(paramMsgHandle);
    }
}

int MapMceRequestGetMessagesListing::SendRequest(ObexMpClient &obexIns)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret;

    std::unique_ptr<ObexHeader> header = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);

    // set header
    // Connection ID
    header->AppendItemConnectionId(obexIns.GetClientSession().GetConnectId());
    // Type is "x-bt/MAP-msg-listing"
    header->AppendItemType(MCE_HEADER_TYPE_GET_MESSAGE_LISTING);
    // folder name
    header->AppendItemName(msgPara_.folder);
    // Application Parameters:
    ObexTlvParamters appParams;

    SendRequestSetAppParamsStep1(appParams);
    SendRequestSetAppParamsStep2(appParams);

    header->AppendItemAppParams(appParams);

#ifdef MCE_DEBUG_SEND_DATA
    LOG_INFO("Send "
             "GetMessagesListing:MaxListCount=0x%x,ListStartOffset=0x%x,ParameterMask=0x%x,FilterMessageType=0x%x,"
             "FilterReadStatus=0x%x",
        msgPara_.MaxListCount,
        msgPara_.ListStartOffset,
        msgPara_.ParameterMask,
        msgPara_.FilterMessageType,
        msgPara_.FilterReadStatus);
#endif

    // make writer object
    std::shared_ptr<ObexBodyObject> writer = std::make_shared<ObexArrayBodyObject>();

    // send request
    ret = obexIns.Get(*header, writer);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s obex put error", __PRETTY_FUNCTION__);
    }
    LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void MapMceRequestGetMessagesListing::MapMceGetItemAppParams(
    IProfileMessagesListingParamStruct &data, const ObexHeader &resp)
{
    auto appParam = resp.GetItemAppParams();
    if (appParam != nullptr) {
        auto tlvParam1 = appParam->GetTlvtriplet(MCE_NEW_MESSAGE);
        if (tlvParam1 != nullptr) {
            int length1 = tlvParam1->GetLen();
            const uint8_t *val1 = tlvParam1->GetVal();
            if ((length1 != 0) && (val1 != nullptr)) {
                data.NewMessage = MapOnOffType(*val1);
            }
        }
        auto msgListTime = appParam->GetTlvtriplet(MCE_MSE_TIME);
        if (msgListTime != nullptr) {
            int timeLength = msgListTime->GetLen();
            const uint8_t *val2 = msgListTime->GetVal();
            if ((timeLength != 0) && (val2 != nullptr)) {
                std::string tempTime(reinterpret_cast<char *>(const_cast<uint8_t *>(val2)), timeLength);
                data.MseTime = tempTime;
            }
        }
        auto msgListSize = appParam->GetTlvtriplet(MCE_LISTING_SIZE);
        if (msgListSize != nullptr) {
            int length3 = msgListSize->GetLen();
            const uint8_t *val3 = msgListSize->GetVal();
            if ((length3 != 0) && (val3 != nullptr)) {
                uint16_t tempVal = *(reinterpret_cast<uint16_t *>(const_cast<uint8_t *>(val3)));
                data.ListingSize = ((tempVal >> MCE_8BIT) & MCE_16BIT_MASK_LEFT_LOW) |
                                   ((tempVal << MCE_8BIT) & MCE_16BIT_MASK_LEFT_HIGH);
            }
        }
        auto msgListDb = appParam->GetTlvtriplet(MCE_DATABASE_IDENTIFIER);
        if (msgListDb != nullptr) {
            int databaseLength = msgListDb->GetLen();
            const uint8_t *val4 = msgListDb->GetVal();
            if ((databaseLength != 0) && (val4 != nullptr)) {
                std::string tempDatabase(reinterpret_cast<char *>(const_cast<uint8_t *>(val4)), databaseLength);
                data.DatabaseIdentifier = tempDatabase;
            }
        }
        auto msgListVerCnt = appParam->GetTlvtriplet(MCE_FOLDER_VERSION_COUNTER);
        if (msgListVerCnt != nullptr) {
            int folderLength = msgListVerCnt->GetLen();
            const uint8_t *val5 = msgListVerCnt->GetVal();
            if ((folderLength != 0) && (val5 != nullptr)) {
                std::string tempFolderCn(reinterpret_cast<char *>(const_cast<uint8_t *>(val5)), folderLength);
                data.FolderVersionCounter = tempFolderCn;
            }
        }
    }
}

int MapMceRequestGetMessagesListing::ProcessResponse(
    MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    retAction.action_ = MapActionType::GET_MESSAGES_LISTING;
    IProfileMessagesListingParamStruct rcvData;
    // get response code
    int rescode = resp.GetFieldCode();
    if (rescode == uint8_t(ObexRspCode::SUCCESS)) {
        MapMceGetItemAppParams(rcvData, resp);
        // get body
        auto msgListObject = resp.GetExtendBodyObject();
        uint8_t msgListBuf[MCE_MAX_LENGTH_OF_RESPONSE_BUFFER];
        size_t msgLBufLen = 1;
        stream_.clear();
        while ((msgLBufLen != 0) && (msgListObject != 0)) {
            msgLBufLen = msgListObject->Read(msgListBuf, MCE_MAX_LENGTH_OF_RESPONSE_BUFFER - 1);
            if ((msgLBufLen != 0) && (msgLBufLen < MCE_MAX_LENGTH_OF_RESPONSE_BUFFER)) {
                msgListBuf[msgLBufLen] = 0;
                stream_ << (char *)msgListBuf;
            }
        }
        if (stream_.str().size() != 0) {
            retAction.messageList_.BuildObjectData(rcvData, stream_.str());
        }
#ifdef MCE_DEBUG_RECEIVE_DATA
        LOG_INFO("GetMessagesListing ListingSize=%{public}d", rcvData.ListingSize);
        LOG_INFO("GetMessagesListing stringObject stream length=%{public}d,str obj length=%{public}d, msglist size=%{public}d",
            int(stream_.str().size()),
            int(retAction.messageList_.GetStringObject().size()),
            int(retAction.messageList_.GetList().size()));
        LOG_INFO("GetMessagesListing stringObject=%{public}s", retAction.messageList_.GetStringObject().c_str());
#endif
    }
    if (retAction.messageList_.GetList().size() == 0) {
        return MCE_RESPONSE_FINISH_NG;
    } else {
        return MCE_RESPONSE_FINISH;
    }
}
}  // namespace bluetooth
}  // namespace OHOS
