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

#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include <memory>
#include <vector>

namespace stub {
static const std::string STUB_MAP_MSE_DB_FILE = "map_mse.s3db";

enum AccountType { EMAIL, IM, OTHER };

enum PhoneType { NONE, SMS_GSM, SMS_CDMA };

enum class Presence : uint8_t { UNKNOWN, OFFLINE, ONLINE, AWAY, DO_NOT_DISTURB, BUSY, IN_A_MEETING };

enum class ChatState : uint8_t { UNKNOWN, INACTIVE, ACTIVE, COMPOSING, PAUSED_COMPOSING, GONE };

enum EventType {
    NEW_MESSAGE,
    MESSAGE_DELETED,
    MESSAGE_REMOVED,
    MESSAGE_SHIFT,
    DELIVERY_SUCCESS,
    SENDING_SUCCESS,
    SENDING_FAILURE,
    DELIVERY_FAILURE,
    READ_STATUS_CHANGED,
    CONVERSATION_CHANGED,
    PARTICIPANT_PRESENCE_CHANGED,
    PARTICIPANT_CHAT_STATE_CHANGED,
    MESSAGE_EXTENDED_DATA_CHANGED,
    MEMORY_FULL,
    MEMORY_AVAILABLE
};

enum class MsgType { NONE, EMAIL, SMS_GSM, SMS_CDMA, MMS, IM };

class EmailImAccount {
public:
    explicit EmailImAccount(std::string &accountId, AccountType accountType, std::string &name,
        std::string &packageName, std::string &providerAuthority, std::string &uci, std::string &uciPrefix)
    {}
    virtual ~EmailImAccount()
    {}
    AccountType GetType(void)
    {
        return OTHER;
    }
    std::string GetAccountId(void)
    {
        return "";
    }
    std::string GetName(void)
    {
        return "";
    }
    std::string GetPackageName(void)
    {
        return "";
    }
    std::string GetProviderAuthority(void)
    {
        return "";
    }
    std::string GetBaseUri(void)
    {
        return "";
    }
    std::string GetBaseUriNoAccount(void)
    {
        return "";
    }
    std::string GetUci(void)
    {
        return "";
    }
    std::string GetUciPrefix(void)
    {
        return "";
    }
};

struct FolderColumns {
    std::string id = "";
    std::string name = "";
    std::string accountId = "";
    std::string parentId = "";
};

struct OwnerStatusParam {
    std::string conversationId = "";
    uint8_t presenceAvailability = 0;
    std::string presenceText = "";
    std::string lastActivity = "";
    uint8_t chatState = 0;
};
struct TelContent {
    bool isSmsCapable = false;
    PhoneType phoneType = PhoneType::NONE;
    std::string phoneNumber = "";
};

struct MsgInfo {
    uint8_t transparent = 0;
    uint8_t retry = 0;
    std::string phoneNumber = "";
    std::string emailAddress = "";
    int msgType = 0;
};

struct EventInfo {
    EventType eventType {};
    std::string handle = "";
    std::string folder = "";
    std::string oldFolder = "";
    MsgType msgType = MsgType::NONE;
    std::string datetime = "";
    std::string subject = "";
    std::string senderName = "";
    int priority = -1;
    std::string conversationName = "";
    std::string conversationID = "";
    int presenceAvailability = -1;
    std::string presenceText = "";
    std::string lastActivity = "";
    int chatState = 0;
    int readStatus = -1;
    std::string extendedData = "";
    std::string participantUci = "";
    std::string contactUid = "";
};

class MapServiceObserver {
public:
    virtual ~MapServiceObserver() = default;
    virtual void OnNotifyingEvent(const std::string &addr, EventInfo &event)
    {}
    virtual void OnUpdateDbIdCounter(void)
    {}
    virtual void OnUpdateVerIdCounter(void)
    {}
    virtual void OnUpdateConvoVerIdCounter(void)
    {}
};

class StubMapObserver {
public:
    virtual ~StubMapObserver() = default;
    virtual void OnSetOwnerStatusComplete(const OwnerStatusParam &ownerStatus)
    {}
    virtual void OnUpdateFolder(std::string &accountId, std::string &folderId)
    {}
    virtual void OnSendMessage(MsgInfo &msgInfo, std::string &msgBody)
    {}
    virtual void OnEventConnected(const std::string &addr)
    {}
    virtual void OnEventDisConnected(const std::string &addr)
    {}
    virtual void OnEventSendComplete(const std::string &addr)
    {}
};

class MapService {
public:
    static MapService *GetInstance();
    void RegisterObserver(StubMapObserver &observer)
    {}
    void DeregisterObserver(StubMapObserver &observer)
    {}
    void RegisterObserver(MapServiceObserver *observer, int masId)
    {}
    void DeregisterObserver(MapServiceObserver *observer, int masId)
    {}
    TelContent GetSystemTelephonyService(void)
    {
        TelContent tmp = {0};
        return tmp;
    }
    std::vector<std::unique_ptr<EmailImAccount>> &GetEmailImAccounts(void)
    {
        return accounts_;
    }
    void SetMseInstance(int id = 0)
    {}
    std::vector<FolderColumns> QueryFolderInfo(std::string &parentId)
    {
        std::vector<FolderColumns> tmp;
        return tmp;
    }
    bool UpdateFolder(std::string &accountId, std::string folderId, int masId)
    {
        return false;
    }
    bool SetOwnerStatus(OwnerStatusParam &ownerParam, int masId)
    {
        return false;
    }
    const OwnerStatusParam &GetOwnerStatus(std::string &conversationId)
    {
        return ownerStatus_;
    }
    void SendMessage(MsgInfo &msgInfo, std::string &msgBody, int masId)
    {}
    void SendEvent(const std::string &addr, int type, int msgType = 2)
    {}
    int ReceiveMessage(int type, std::string id = "")
    {
        return 0;
    }
    void UpdateMessageStatus()
    {}
    void EventConnected(const std::string &addr)
    {}
    void EventDisConnected(const std::string &addr)
    {}
    void EventSendComplete(const std::string &addr)
    {}

private:
    MapService() = default;
    ~MapService() = default;

    std::vector<std::unique_ptr<EmailImAccount>> accounts_ {};
    OwnerStatusParam ownerStatus_ {};

    MapService(const MapService &) = delete;
    MapService &operator=(const MapService &) = delete;
};
}  // namespace stub
#endif  // MAP_SERVICE_H