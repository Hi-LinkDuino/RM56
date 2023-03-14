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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client service instance request object.
 *
 */

/**
 * @file map_mce_instance_request.h
 *
 * @brief map client service instance request header file .
 *
 */

#ifndef MAP_MCE_INSTANCE_REQUEST_H
#define MAP_MCE_INSTANCE_REQUEST_H

#include <cstdint>
#include <list>
#include <string>
#include <vector>
#include "../obex/obex_headers.h"
#include "../obex/obex_mp_client.h"
#include "bt_def.h"
#include "map_mce_data_analyse.h"
#include "map_mce_parameter.h"
#include "map_mce_types.h"

namespace OHOS {
namespace bluetooth {
class MapMceInstanceClient;
#define MCE_DEBUG_SEND_DATA
#define MCE_DEBUG_RECEIVE_DATA

enum MapMceSetPathEnum {
    // SetFolder Function Flags Enum
    MASSTM_SETPATH_DOWN = 0x02,                 // Go down 1 level
    MASSTM_SETPATH_ROOT = MASSTM_SETPATH_DOWN,  // Go back to root
    MASSTM_SETPATH_UP = 0x03,                   // Go up 1 level
    MASSTM_SETPATH_LIST = 0x04                  // multi path
};

enum MceClientResExcuteType {
    MCE_RESPONSE_CONTINUE_NO_CALLBACK,
    MCE_RESPONSE_CONTINUE_WITH_CALLBACK,
    MCE_RESPONSE_FINISH,  // OK or NG
    MCE_RESPONSE_FINISH_NO_CALLBACK,
    MCE_RESPONSE_FINISH_NG  // NG
};

struct MasInstanceInformation {
    bool isValid = false;
    std::string ownerUciUtf8 = "";
    std::string masInstanceInfoUtf8 = "";
};

struct MapRequestResponseAction {
    MapActionType action_ {};
    int supportedFeatures = 0;
    IProfileOwnerStatus ownerStatus_ {};
    MceTypesBMessage bmessage_ {};
    MceTypesMessagesListing messageList_ {};
    MceTypesConversationListing conversationList_ {};
};

// Tag Id
static const uint8_t MCE_MAX_LIST_COUNT = 0x01;
static const uint8_t MCE_LIST_START_OFF_SET = 0x02;
static const uint8_t MCE_FILTER_MESSAGE_TYPE = 0x03;
static const uint8_t MCE_FILTER_PERIOD_BEGIN = 0x04;
static const uint8_t MCE_FILTER_PERIOD_END = 0x05;
static const uint8_t MCE_FILTER_READ_STATUS = 0x06;
static const uint8_t MCE_FILTER_RECIPIENT = 0x07;
static const uint8_t MCE_FILTER_ORIGINATOR = 0x08;
static const uint8_t MCE_FILTER_PRIORITY = 0x09;
static const uint8_t MCE_ATTACHMENT = 0x0A;
static const uint8_t MCE_TRANSPARENT = 0x0B;
static const uint8_t MCE_RETRY = 0x0C;
static const uint8_t MCE_NEW_MESSAGE = 0x0D;
static const uint8_t MCE_NOTIFICATION_STATUS = 0x0E;
static const uint8_t MCE_MAS_INSTANCEID = 0x0F;
// Tag Length
static const uint8_t MCE_MAX_LIST_COUNT_LENGTH = 0x02;
static const uint8_t MCE_LIST_START_OFF_SET_LENGTH = 0x02;
static const uint8_t MCE_FILTER_MESSAGE_TYPE_LENGTH = 0x01;
static const uint8_t MCE_FILTER_PERIOD_BEGIN_LENGTH = 0x10;
static const uint8_t MCE_FILTER_PERIOD_END_LENGTH = 0x10;
static const uint8_t MCE_FILTER_READ_STATUS_LENGTH = 0x01;
static const uint8_t MCE_FILTER_PRIORITY_LENGTH = 0x01;
static const uint8_t MCE_ATTACHMENT_LENGTH = 0x01;
static const uint8_t MCE_TRANSPARENT_LENGTH = 0x01;
static const uint8_t MCE_RETRY_LENGTH = 0x01;
static const uint8_t MCE_NEW_MESSAGE_LENGTH = 0x01;
static const uint8_t MCE_NOTIFICATION_STATUS_LENGTH = 0x01;
static const uint8_t MCE_MAS_INSTANCEID_LENGTH = 0x01;

// Tag Id
static const uint8_t MCE_PARAMETER_MASK = 0x10;
static const uint8_t MCE_FOLDER_LISTING_SIZE = 0x11;
static const uint8_t MCE_LISTING_SIZE = 0x12;
static const uint8_t MCE_SUBJECT_LENGTH = 0x13;
static const uint8_t MCE_CHARSET = 0x14;
static const uint8_t MCE_FRACTION_REQUEST = 0x15;
static const uint8_t MCE_FRACTION_DELIVER = 0x16;
static const uint8_t MCE_STATUS_INDICATOR = 0x17;
static const uint8_t MCE_STATUS_VALUE = 0x18;
static const uint8_t MCE_MSE_TIME = 0x19;
static const uint8_t MCE_DATABASE_IDENTIFIER = 0x1A;
static const uint8_t MCE_CONVERSATION_LISTING_VERSION_COUNTER = 0x1B;
static const uint8_t MCE_PRESENCE_AVAILABILITY = 0x1C;
static const uint8_t MCE_PRESENCE_TEXT = 0x1D;
static const uint8_t MCE_LAST_ACTIVITY = 0x1E;
static const uint8_t MCE_FILTER_LAST_ACTIVITY_BEGIN = 0x1F;
static const uint8_t MCE_FILTER_LAST_ACTIVITY_END = 0x20;
// Tag Length
static const uint8_t MCE_PARAMETER_MASK_LENGTH = 0x04;
static const uint8_t MCE_FOLDER_LISTING_SIZE_LENGTH = 0x02;
static const uint8_t MCE_LISTING_SIZE_LENGTH = 0x02;
static const uint8_t MCE_SUBJECT_LENGTH_LENGTH = 0x01;
static const uint8_t MCE_CHARSET_LENGTH = 0x01;
static const uint8_t MCE_FRACTION_REQUEST_LENGTH = 0x01;
static const uint8_t MCE_FRACTION_DELIVER_LENGTH = 0x01;
static const uint8_t MCE_STATUS_INDICATOR_LENGTH = 0x01;
static const uint8_t MCE_STATUS_VALUE_LENGTH = 0x01;
static const uint8_t MCE_DATABASE_IDENTIFIER_LENGTH = 0x80;
static const uint8_t MCE_CONVERSATION_LISTING_VERSION_COUNTER_LENGTH = 0x80;
static const uint8_t MCE_PRESENCE_AVAILABILITY_LENGTH = 0x01;

// Tag Id
static const uint8_t MCE_CHAT_STATE = 0x21;
static const uint8_t MCE_CONVERSATION_ID = 0x22;
static const uint8_t MCE_FOLDER_VERSION_COUNTER = 0x23;
static const uint8_t MCE_FILTER_MESSAGE_HANDLE = 0x24;
static const uint8_t MCE_NOTIFICATION_FILTER_MASK = 0x25;
static const uint8_t MCE_CONV_PARAMETER_MASK = 0x26;
static const uint8_t MCE_OWNER_UCI = 0x27;
static const uint8_t MCE_EXTENDED_DATA = 0x28;
static const uint8_t MCE_MAP_SUPPORTED_FEATURES = 0x29;
static const uint8_t MCE_MESSAGE_HANDLE = 0x2A;
static const uint8_t MCE_MODIFY_TEXT = 0x2B;
// Tag Length
static const uint8_t MCE_CHAT_STATE_LENGTH = 0x01;
static const uint8_t MCE_CONVERSATION_ID_LENGTH = 0x80;
static const uint8_t MCE_FOLDER_VERSION_COUNTER_LENGTH = 0x80;
static const uint8_t MCE_FILTER_MESSAGE_HANDLE_LENGTH = 0x40;
static const uint8_t MCE_NOTIFICATION_FILTER_MASK_LENGTH = 0x04;
static const uint8_t MCE_CONV_PARAMETER_MASK_LENGTH = 0x04;
static const uint8_t MCE_MAP_SUPPORTED_FEATURES_LENGTH = 0x04;
static const uint8_t MCE_MESSAGE_HANDLE_LENGTH = 0x40;
static const uint8_t MCE_MODIFY_TEXT_LENGTH = 0x01;

// folder names
const std::u16string MCE_FOLDER_TELECOM = u"telecom";
const std::u16string MCE_FOLDER_MSG = u"msg";  // telecom/msg/inbox

// request type
const std::string MCE_HEADER_TYPE_SET_NOTIFICATION_FILTER = "x-bt/MAP-notification-filter";
const std::string MCE_HEADER_TYPE_SET_NOTIFICATION_REGISTRATION = "x-bt/MAP-NotificationRegistration";
const std::string MCE_HEADER_TYPE_GET_FOLDER_LISTING = "x-obex/folder-listing";
const std::string MCE_HEADER_TYPE_GET_MESSAGE_LISTING = "x-bt/MAP-msg-listing";
const std::string MCE_HEADER_TYPE_GET_CONVO_LISTING = "x-bt/MAP-convo-listing";
const std::string MCE_HEADER_TYPE_MESSAGE = "x-bt/message";
const std::string MCE_HEADER_TYPE_SET_MESSAGE_STATUS = "x-bt/messageStatus";
const std::string MCE_HEADER_TYPE_MESSAGE_UPDATE = "x-bt/MAP-messageUpdate";
const std::string MCE_HEADER_TYPE_GET_MAS_INSTANCE_INFORMATION = "x-bt/MASInstanceInformation";
const std::string MCE_HEADER_TYPE_SET_OWNER_STATUS = "x-bt/ownerStatus";

/**
 * @brief config data
 */
struct MapMceRequestConfig {
    bool isUseSrmp = false;
    int srmpCount = 0;
    int maxOfGetUnread = 100;
};

/**
 * @brief Mce InstanceClient request base
 */
class MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Instance Request object
     */
    MapMceInstanceRequest();
    /**
     * @brief Destroy the Map Mce Instance Request object
     */
    virtual ~MapMceInstanceRequest();
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    virtual MceRequestType GetRequestType() = 0;
    /**
     * @brief  Send Request
     * @param  obexIns  obex object
     * @return int
     */
    virtual int SendRequest(ObexMpClient &obexIns) = 0;
    /**
     * @brief  Process Response
     * @param  clientInst   obex object
     * @param  resp         obex header
     * @param  retAction    action
     * @return int
     */
    virtual int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) = 0;
    /**
     * @brief Set the Support Message Type object
     * @param  type   message type
     */
    void SetSupportMessageType(int type);
    /**
     * @brief Get the Support Message Type object
     * @return int
     */
    int GetSupportMessageType() const;
    /**
     * @brief Set the Owner Uci object
     * @param  owner   Owner Uci
     */
    void SetOwnerUci(const std::string &owner);
    /**
     * @brief Get the Owner Uci object
     * @return std::string Owner Uci
     */
    std::string GetOwnerUci() const;
    /**
     * @brief Save Request
     */
    void SaveReq();
    /**
     * @brief Set the Request Config object
     * @param  cfg        request config
     */
    void SetRequestConfig(MapMceRequestConfig cfg);
    /**
     * @brief Get the Request Config object
     * @return Request Config
     */
    MapMceRequestConfig GetRequestConfig() const;

protected:
    /**
     * @brief  Trans MapMessageType To IprofileMask
     * @param  type     Message Type
     * @return int
     */
    int MapMessageTypeToIprofileMask(MapMessageType type) const;
    /**
     * @brief Trans MapMessageType To FrameworkMask
     * @param  type     Message Type
     * @return int
     */
    int MapMessageTypeToFrameworkMask(MapMessageType type) const;
    /**
     * @brief  Trans IprofileMask To MapMessageType
     * @param  mask      IprofileMask
     * @return  MessageType
     */
    MapMessageType IprofileMaskToMapMessageType(int mask) const;

private:
    // saved flag
    bool saveFlag_ = false;
    // message type
    int messageType_ = 0;
    // owner uci name
    std::string ownerUci_ = "";
    // config parameter
    MapMceRequestConfig config_ {};
};

/**
 * @brief Request SetNotificationFilter
 */
class MapMceRequestSetNotificationFilter : public MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Request Set Notification Filter object
     * @param  mask             mask value
     */
    explicit MapMceRequestSetNotificationFilter(const int mask);
    /**
     * @brief Destroy the Map Mce Request Set Notification Filter object
     */
    ~MapMceRequestSetNotificationFilter() override;
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    MceRequestType GetRequestType() override;
    /**
     * @brief  Send Request process
     * @param  obexIns          obex client
     * @return int
     */
    int SendRequest(ObexMpClient &obexIns) override;
    /**
     * @brief  Process Response
     * @param  clientInst       obex client object
     * @param  resp             ObexHeader
     * @param  retAction        action observer data
     * @return int
     */
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    int notificationFilterMask_ = 0;
};

/**
 * @brief Request SetNotificationRegistration
 */
class MapMceRequestSetNotificationRegistration : public MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Request Set Notification Registration object
     * @param  on               on / off
     */
    explicit MapMceRequestSetNotificationRegistration(bool on);
    /**
     * @brief Destroy the Map Mce Request Set Notification Registration object
     */
    ~MapMceRequestSetNotificationRegistration() override;
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    MceRequestType GetRequestType() override;
    /**
     * @brief  Send Request process
     * @param  obexIns          obex client
     * @return int
     */
    int SendRequest(ObexMpClient &obexIns) override;
    /**
     * @brief  Process Response
     * @param  clientInst       obex client object
     * @param  resp             ObexHeader
     * @param  retAction        action observer data
     * @return int
     */
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    bool setupValue_ = false;
};

/**
 * @brief Request GetMasInstanceInformation
 */
class MapMceRequestGetMasInstanceInformation : public MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Request Get Mas Instance Information object
     * @param  instId           instance id
     */
    explicit MapMceRequestGetMasInstanceInformation(uint8_t instId);
    /**
     * @brief Destroy the Map Mce Request Get Mas Instance Information object
     */
    ~MapMceRequestGetMasInstanceInformation() override;
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    MceRequestType GetRequestType() override;
    /**
     * @brief  Send Request process
     * @param  obexIns          obex client
     * @return int
     */
    int SendRequest(ObexMpClient &obexIns) override;
    /**
     * @brief  Process Response
     * @param  clientInst       obex client object
     * @param  resp             ObexHeader
     * @param  retAction        action observer data
     * @return int
     */
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    // instance id
    uint8_t instanceId_ = 0;
};

/**
 * @brief Request GetGetFolderListing
 */
class MapMceRequestGetFolderListing : public MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Request Get Folder Listing object
     * @param  listCnt          list count
     * @param  offset           offset
     */
    MapMceRequestGetFolderListing(uint16_t listCnt, uint16_t offset);
    /**
     * @brief Destroy the Map Mce Request Get Folder Listing object
     */
    ~MapMceRequestGetFolderListing() override;
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    MceRequestType GetRequestType() override;
    /**
     * @brief  Send Request process
     * @param  obexIns          obex client
     * @return int
     */
    int SendRequest(ObexMpClient &obexIns) override;
    /**
     * @brief  Process Response
     * @param  clientInst       obex client object
     * @param  resp             ObexHeader
     * @param  retAction        action observer data
     * @return int
     */
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    // max of list count
    uint16_t maxOfListCount_ = 0;
    // start offset
    uint16_t startOffset_ = 0;
    // list size
    uint16_t listSize_ = 0;
    // folder list parameter
    IProfileFolderListing folderListingParam_ {};
};

/**
 * @brief Request SetPath
 */
class MapMceRequestSetPath : public MapMceInstanceRequest {
public:
    /**
     * @brief Construct a new Map Mce Request Set Path object
     * @param  flags            setpath type
     * @param  paths            path string
     * @param  pathsList        path string list
     */
    MapMceRequestSetPath(uint8_t flags, const std::u16string &paths, const std::vector<std::u16string> &pathsList);
    /**
     * @brief Destroy the Map Mce Request Set Path object
     */
    ~MapMceRequestSetPath() override;
    /**
     * @brief Get the Request Type object
     * @return MceRequestType
     */
    MceRequestType GetRequestType() override;
    /**
     * @brief  Send Request process
     * @param  obexIns          obex client
     * @return int
     */
    int SendRequest(ObexMpClient &obexIns) override;
    /**
     * @brief  Process Response
     * @param  clientInst       obex client object
     * @param  resp             ObexHeader
     * @param  retAction        action observer data
     * @return int
     */
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    uint8_t pathFlags_ = 0;
    std::u16string pathsString_ = u"";
    std::vector<std::u16string> pathsList_ {};
};

/**
 * @brief Request Push Message
 */
class MapMceRequestPushMessage : public MapMceInstanceRequest {
public:
    explicit MapMceRequestPushMessage(const IProfileSendMessageParameters &msg);
    ~MapMceRequestPushMessage() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    IProfileSendMessageParameters messageParam_ {};
};

/**
 * @brief Request Push Message
 */
class MapMceRequestGetMessage : public MapMceInstanceRequest {
public:
    MapMceRequestGetMessage(const std::u16string &msgHandle, const IProfileGetMessageParameters &para);
    ~MapMceRequestGetMessage() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    std::u16string msgHandle_ = u"";
    IProfileGetMessageParameters msgPara_ {};
    std::ostringstream stream_ {};
};

/**
 * @brief Request Get Update Inbox
 */
class MapMceRequestUpdateInbox : public MapMceInstanceRequest {
public:
    MapMceRequestUpdateInbox();
    ~MapMceRequestUpdateInbox() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
};

/**
 * @brief Request Set Owner Status
 */
class MapMceRequestSetOwnerStatus : public MapMceInstanceRequest {
public:
    explicit MapMceRequestSetOwnerStatus(const IProfileSetOwnerStatusParameters &para);
    ~MapMceRequestSetOwnerStatus() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    IProfileSetOwnerStatusParameters ownerPara_ {};
};

/**
 * @brief Request Get Owner Status
 */
class MapMceRequestGetOwnerStatus : public MapMceInstanceRequest {
public:
    explicit MapMceRequestGetOwnerStatus(const std::string &conversationId);
    ~MapMceRequestGetOwnerStatus() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    std::string ownerConversationId_ = "";
};

/**
 * @brief Request Get Owner Status
 */
class MapMceRequestSetMessageStatus : public MapMceInstanceRequest {
public:
    MapMceRequestSetMessageStatus(const std::u16string &msgHandle, MapStatusIndicatorType statusIndicator,
        MapStatusValueType statusValue, const std::string &exData);
    ~MapMceRequestSetMessageStatus() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    std::u16string msgHandle_ = u"";
    uint8_t msgStatusIndicator_ = 0;
    uint8_t msgstatusValue_ = 0;
    std::string extendedData_ = "";
};

/**
 * @brief Request Set Owner Status
 */
class MapMceRequestGetConversationListing : public MapMceInstanceRequest {
public:
    explicit MapMceRequestGetConversationListing(const IProfileGetConversationListingParameters &para);
    ~MapMceRequestGetConversationListing() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    void SetAppendParam(ObexTlvParamters &appParams);
    IProfileGetConversationListingParameters converPara_ {};
    static void MapMceGetItemAppParams(IProfileConversationListingParamStruct &data, const ObexHeader &resp);
};

/**
 * @brief Request Get Message listing
 */
class MapMceRequestGetMessagesListing : public MapMceInstanceRequest {
public:
    explicit MapMceRequestGetMessagesListing(const IProfileGetMessagesListingParameters &para);
    ~MapMceRequestGetMessagesListing() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    static void MapMceGetItemAppParams(IProfileMessagesListingParamStruct &data, const ObexHeader &resp);
    void SendRequestSetAppParamsStep1(ObexTlvParamters &appParams);
    void SendRequestSetAppParamsStep2(ObexTlvParamters &appParams);
    IProfileGetMessagesListingParameters msgPara_ {};
    std::ostringstream stream_ {};
};

/**
 * @brief Request Get Uread Messages
 */
class MapMceRequestGetUreadMessages : public MapMceInstanceRequest {
public:
    BT_DISALLOW_COPY_AND_ASSIGN(MapMceRequestGetUreadMessages);
    MapMceRequestGetUreadMessages(MapMessageType type, const IProfileGetMessagesListingParameters &para);
    ~MapMceRequestGetUreadMessages() override;
    MceRequestType GetRequestType() override;
    int SendRequest(ObexMpClient &obexIns) override;
    int ProcessResponse(
        MapMceInstanceClient &clientInst, const ObexHeader &resp, MapRequestResponseAction &retAction) override;

private:
    std::list<std::string> msgHandleList_ {};
    std::unique_ptr<MapMceInstanceRequest> requestPtr_ = nullptr;
    ObexMpClient *obexInsBackup_ = nullptr;
    IProfileGetMessagesListingParameters listParam_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_INSTANCE_REQUEST_H
