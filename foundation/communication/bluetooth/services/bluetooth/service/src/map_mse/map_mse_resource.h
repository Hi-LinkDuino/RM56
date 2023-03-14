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

#ifndef MAP_MSE_RESOURCE_H
#define MAP_MSE_RESOURCE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "data_access.h"
#include "dispatcher.h"
#include "log.h"
#include "map_mse_bmsg.h"
#include "map_mse_event.h"
#include "map_mse_folder.h"
#include "map_mse_instance.h"
#include "map_mse_params.h"
#include "stub/map_service.h"

namespace OHOS {
namespace bluetooth {
struct MseMessageElement {
    // Messages-Listing Format Version 1.0
    std::string handle_ = "";
    std::string subject_ = "";
    std::string dateTime_ = "";
    std::string senderName_ = "";
    std::string senderAddressing_ = "";
    std::string replytoAddressing_ = "";
    std::string recipientName_ = "";
    std::string recipientAddressing_ = "";
    MessageType type = MessageType::NONE;
    int size_ = -1;
    std::string text_ = "";
    std::string receptionStatus_ = "";
    int attachmentSize_ = -1;
    std::string priority_ = "";
    bool read_ = false;
    std::string sent_ = "";
    std::string protected_ = "";
    // Messages-Listing Format Version 1.1
    std::string deliveryStatus_ = "";
    std::string conversationId_ = "";
    std::string conversationName_ = "";
    std::string direction_ = "";
    std::string attachmentMimeTypes_ = "";
    bool msgTypeAppParamSet_ = false;
};
struct MseParticipant {
    std::string uci_ = "";
    std::string name_ = "";
    std::string displayName_ = "";
    int presenceAvailability_ = -1;
    std::string presenceText_ = "";
    int priority_ = -1;
    std::string lastActy_ = "";
    int chatState_ = -1;
    std::string btUid_ = "";
};

struct MseConvoLstElement {
    std::string convoId_ = "";
    std::string name_ = "";
    std::string lastActivity_ = "";
    int readStatus_ = -1;
    std::string versionCounter_ = "";
    std::string summary_ = "";
    std::vector<MseParticipant> contacts_ {};
};
class MapMseResource {
public:
    explicit MapMseResource(MapMseMnscli &mnsClient, utility::Dispatcher &dispatcher, MapMseInstance &instance);
    virtual ~MapMseResource();
    std::string GetBodyXml(const std::string &folderName, const MapMseParams &appParameter,
        const std::string &version, uint16_t &listSize, uint8_t &unRead);
    void GetListingSize(
        const std::string &folderName, const MapMseParams &appParameter, uint16_t &listSize, uint8_t &unRead);
    std::string GetMessage(const std::string handle, const MapMseParams &appParameter, const std::string &version);
    std::string GetCvslistBodyXml(const MapMseParams &appParameter, uint16_t &size);
    int GetConvoLstCount(const MapMseParams &appParameter);
    void SetNotificationRegistration(const RawAddress &remoteAddr, const int status);
    void SetNotificationFilter(const RawAddress &remoteAddr, uint32_t filter);
    void SetRemoteFeatureMask(uint32_t features);
    static bool SetMessageStatusDeleted(
        std::string handle, MessageType type, std::string path, uint8_t status);
    static bool SetMessageStatusRead(std::string handle, MessageType type, std::string path, uint8_t status);
    static bool SetExtendedData(std::string handle, std::string data);
    void SentOrForwardedMessage(MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &handle);
    std::string SentConversationMessage(
        MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName);
    std::string SentNewMessage(MapMseBmsg &bmessage, std::string &folderName, MapMseParams &appParameter);

private:
    static const std::string XML_TAG_CONVO_LISTING;
    static const std::string XML_ATT_ENCODING;
    static const std::string XML_ATT_VERSION;
    static const std::string XML_TAG_CONVERSATION;
    static const std::string XML_ATT_LAST_ACTIVITY;
    static const std::string XML_ATT_NAME;
    static const std::string XML_ATT_ID;
    static const std::string XML_ATT_READ;
    static const std::string XML_ATT_VERSION_COUNTER;
    static const std::string XML_ATT_SUMMARY;
    static const std::string XML_TAG_PARTICIPANT;
    static const std::string XML_ATT_UCI;
    static const std::string XML_ATT_DISPLAY_NAME;
    static const std::string XML_ATT_CHAT_STATE;
    static const std::string XML_ATT_PRIORITY;
    static const std::string XML_ATT_PRESENCE_TEXT;
    static const std::string XML_ATT_PRESENCE_AVAILABILITY;
    static const std::string XML_ATT_X_BT_UID;
    static const uint32_t SUBJECT_MASK = 1;
    static const uint32_t DATETIME_MASK = 1 << 1;
    static const uint32_t SENDER_NAME_MASK = 1 << 2;
    static const uint32_t SENDER_ADDRESSING_MASK = 1 << 3;
    static const uint32_t RECIPIENT_NAME_MASK = 1 << 4;
    static const uint32_t RECIPIENT_ADDRESSING_MASK = 1 << 5;
    static const uint32_t TYPE_MASK = 1 << 6;
    static const uint32_t SIZE_MASK = 1 << 7;
    static const uint32_t RECEPTION_STATUS_MASK = 1 << 8;
    static const uint32_t TEXT_MASK = 1 << 9;
    static const uint32_t ATTACHMENT_SIZE_MASK = 1 << 10;
    static const uint32_t PRIORITY_MASK = 1 << 11;
    static const uint32_t READ_MASK = 1 << 12;
    static const uint32_t SENT_MASK = 1 << 13;
    static const uint32_t PROTECTED_MASK = 1 << 14;
    static const uint32_t REPLYTO_ADDRESSING = 1 << 15;
    static const uint32_t DELIVERY_STATUS_MASK = 1 << 16;
    static const uint32_t CONVERSATION_ID_MASK = 1 << 17;
    static const uint32_t CONVERSATION_NAME_MASK = 1 << 18;
    static const uint32_t DIRECTION_MASK = 1 << 19;
    static const uint32_t ATTACHMENT_NIME_MASK = 1 << 20;
    static const uint8_t MASK_MSG_SMS_GSM = 0x01;
    static const uint8_t MASK_MSG_SMS_CDMA = 0x02;
    static const uint8_t MASK_MSG_EMAIL = 0x04;
    static const uint8_t MASK_MSG_MMS = 0x08;
    static const uint8_t MASK_MSG_IM = 0x10;
    static const uint32_t CONVO_LISTING_NAME_MASK = 1;
    static const uint32_t CONVO_LAST_ACTIVITY_MASK = 1 << 1;
    static const uint32_t CONVO_READ_STATUS_MASK = 1 << 2;
    static const uint32_t CONVO_VERSION_COUNTER_MASK = 1 << 3;
    static const uint32_t CONVO_SUMMARY_MASK = 1 << 4;
    static const uint32_t PARTI_MASK = 1 << 5;
    static const uint32_t PARTI_UCI_MASK = 1 << 6;
    static const uint32_t PARTI_DISPLAY_NAME_MASK = 1 << 7;
    static const uint32_t PARTI_CHAT_STATE_MASK = 1 << 8;
    static const uint32_t PARTI_LAST_ACTIVITY_MASK = 1 << 9;
    static const uint32_t PARTI_X_BT_UID_MASK = 1 << 10;
    static const uint32_t PARTI_NAME_MASK = 1 << 11;
    static const uint32_t PARTI_PRESENCE_AVAIL_MASK = 1 << 12;
    static const uint32_t PARTI_PRESENCE_TEXT_MASK = 1 << 13;
    static const uint32_t PARTI_PRIORITY_MASK = 1 << 14;
    static const std::string TYPE_NEW;
    static const std::string TYPE_DELETE;
    static const std::string TYPE_SHIFT;
    static const std::string TYPE_SENDING_SUCCESS;
    static const std::string TYPE_SENDING_FAILURE;
    static const std::string TYPE_DELIVERY_SUCCESS;
    static const std::string TYPE_DELIVERY_FAILURE;
    static const std::string TYPE_MEMORY_FULL;
    static const std::string TYPE_MEMORY_AVAILABLE;
    static const std::string TYPE_READ_STATUS;
    static const std::string TYPE_CONVERSATION;
    static const std::string TYPE_PRESENCE;
    static const std::string TYPE_CHAT_STATE;
    static const std::string TYPE_EXTENDED_DATA;
    static const std::string TYPE_REMOVED;
    static const uint32_t FILTER_MASK_NEW_MESSAGE = 1;
    static const uint32_t FILTER_MASK_MESSAGE_DELETED = 1 << 1;
    static const uint32_t FILTER_MASK_MESSAGE_SHIFT = 1 << 2;
    static const uint32_t FILTER_MASK_SENDING_SUCCESS = 1 << 3;
    static const uint32_t FILTER_MASK_SENDING_FAILURE = 1 << 4;
    static const uint32_t FILTER_MASK_DELIVERY_SUCCESS = 1 << 5;
    static const uint32_t FILTER_MASK_DELIVERY_FAILURE = 1 << 6;
    static const uint32_t FILTER_MASK_MEMORY_FULL = 1 << 7;
    static const uint32_t FILTER_MASK_MEMORY_AVAILABLE = 1 << 8;
    static const uint32_t FILTER_MASK_READ_STATUS_CHANGED = 1 << 9;
    static const uint32_t FILTER_MASK_CONVERSATION_CHANGED = 1 << 10;
    static const uint32_t FILTER_MASK_PARTICIPANT_PRESENCE_CHANGED = 1 << 11;
    static const uint32_t FILTER_MASK_PARTICIPANT_CHATSTATE_CHANGED = 1 << 12;
    static const uint32_t FILTER_MASK_EXTENDED_DATA_CHANGED = 1 << 13;
    static const uint32_t FILTER_MASK_MESSAGE_REMOVED = 1 << 14;
    std::string GetMsglistSql(const MapMseParams &appParameter, const std::string &folderName);
    static void AddWhereSql(const MapMseParams &appParameter, std::string &sql);
    static void SetContacts(IDataStatement &ids, const MapMseParams &appParameter, int &index);
    static std::vector<int> GetMsgTypeParam(uint8_t mask);
    void SetMsglistParam(IDataStatement &ids, const MapMseParams &appParameter, const std::string &folderName);
    std::vector<std::unique_ptr<MseMessageElement>> GetMessageListContent(
        const std::string &folderName, const MapMseParams &appParameter);
    static std::vector<MapMseVcard> GetVCardData(
        std::unique_ptr<DataAccess> &dataAccess, const std::string handle, const int isOriginator = 0);
    static std::string GetCvslistSql(const MapMseParams &appParameter);
    static void SetCvslistParam(IDataStatement &ids, const MapMseParams &appParameter);
    static std::string GetParticipantSql(const MapMseParams &appParameter);
    static void SetParticipantParam(IDataStatement &ids, const MapMseParams &appParameter,
        const std::string &convoId);
    std::vector<MseParticipant> GetParticipantContent(
        IDataStatement &ids, const MapMseParams &appParameter, const std::string &convoId);
    std::vector<std::unique_ptr<MseConvoLstElement>> GetConvoLstContent(const MapMseParams &appParameter);
    void AddVer10Node(xmlNodePtr &node, MseMessageElement &msg, uint8_t &unRead, long paraMask);
    static std::string ChangeType(MessageType type);
    std::string GetPrivateBodyXml(std::vector<std::unique_ptr<MseMessageElement>> &messageList,
        const std::string &version, uint8_t &unRead, long paraMask);
    static bool CheckParameterMask(uint32_t mask, std::string para, long paraMask, bool required = false);
    void ParticipantsToXml(xmlNodePtr &node, std::vector<MseParticipant> &contacts, long paraMask);
    void SendEventReport(const std::string &addr, const std::string &event, const std::string &type);
    bool CheckEventFilterMask(const std::string &addr, const uint32_t bit);
    MapMseMnscli *mnsClient_ = nullptr;
    utility::Dispatcher &dispatcher_;
    MapMseInstance &instance_;
    uint8_t masId_ = 0;
    std::atomic_uint8_t reportVersion_ = 0x0;
    std::unordered_map<std::string, uint32_t> eventFilterMaskMap_ {};
    static std::string GetDateTime();
    std::string UpdateData(MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &handle);
    static int UpdateMessage(
        std::unique_ptr<DataAccess> &dataAccess, const MapMseBmsg &bmessage, const std::string &handle);
    int UpdateMessageList(std::unique_ptr<DataAccess> &dataAccess, const std::string &handle);
    std::string DataInsert(MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName);
    int InsertMessageList(std::unique_ptr<DataAccess> &dataAccess, MapMseParams &appParameter,
        const MapMseBmsg &bmessage, const std::string &handle, const std::string &folderName);
    static void GetNameAddress(MessageType type, std::vector<MapMseVcard> vcard,
        std::string &name, std::string &sender);
    int InsertMessage(std::unique_ptr<DataAccess> &dataAccess, MapMseParams &appParameter,
        const MapMseBmsg &bmessage, const std::string &handle);
    uint64_t GetMessageHandle(MessageType type);
    static std::string GetMessageNameByType(MessageType type);
    void NotifySendMessage(const MapMseBmsg &bmessage, const MapMseParams &appParameter);
    std::string InsertConvData(MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName);
    static int UpdateConvMessage(
        std::unique_ptr<DataAccess> &dataAccess, const std::string &conversationId, const std::string &handle);
    static int UpdateConvMessageList(
        std::unique_ptr<DataAccess> &dataAccess, const std::string &conversationId, const std::string &handle);
    static int InsertVCard(
        std::unique_ptr<DataAccess> &dataAccess, const MapMseVcard &vcard, const int isOriginator,
        const std::string &handle);
    uint32_t remoteFeatures_ = 0x0;
    void SendEvent(const std::string &addr, const stub::EventInfo &event);
    void SendEventNative(const std::string &addr, const stub::EventInfo &event);
    uint32_t GetRemoteFeatureMask(const std::string &addr);
    void SendNewMessageEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMessageShiftEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMessageRemovedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendingFailureEvent(const std::string &addr, const stub::EventInfo &event);
    void SendReadStatusChangedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendConversationChangedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendParticipantPresenceChangedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendParticipantChatStateChangedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMessageExtendedDataChangedEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMemoryFullEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMemoryAvailableEvent(const std::string &addr, const stub::EventInfo &event);
    void SendMessageEvent(const std::string type, const std::string &addr, const stub::EventInfo &event);
    void UpdateDbIdCounter(void);
    void UpdateVerIdCounter(void);
    void UpdateConvoVerIdCounter(void);
    class EventObserver : public stub::MapServiceObserver {
    public:
        explicit EventObserver(MapMseResource &observer);
        virtual ~EventObserver() = default;

    private:
        void OnNotifyingEvent(const std::string &addr, stub::EventInfo &event) override;
        void OnUpdateDbIdCounter(void) override;
        void OnUpdateVerIdCounter(void) override;
        void OnUpdateConvoVerIdCounter(void) override;
        MapMseResource &observer_;
    };
    std::unordered_map<int, std::unique_ptr<EventObserver>> observerMap_ {};
    BT_DISALLOW_COPY_AND_ASSIGN(MapMseResource);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_RESOURCE_H
