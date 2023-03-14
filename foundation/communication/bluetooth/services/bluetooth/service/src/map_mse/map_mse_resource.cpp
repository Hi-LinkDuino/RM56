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

#include "map_mse_resource.h"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace OHOS {
namespace bluetooth {
const std::string MapMseResource::XML_TAG_CONVO_LISTING = "MAP-convo-listing";
const std::string MapMseResource::XML_ATT_ENCODING = "encoding";
const std::string MapMseResource::XML_ATT_VERSION = "version";
const std::string MapMseResource::XML_TAG_CONVERSATION = "conversation";
const std::string MapMseResource::XML_ATT_LAST_ACTIVITY = "last_activity";
const std::string MapMseResource::XML_ATT_NAME = "name";
const std::string MapMseResource::XML_ATT_ID = "id";
const std::string MapMseResource::XML_ATT_READ = "read_status";
const std::string MapMseResource::XML_ATT_VERSION_COUNTER = "version_counter";
const std::string MapMseResource::XML_ATT_SUMMARY = "summary";
const std::string MapMseResource::XML_TAG_PARTICIPANT = "participant";
const std::string MapMseResource::XML_ATT_UCI = "uci";
const std::string MapMseResource::XML_ATT_DISPLAY_NAME = "display_name";
const std::string MapMseResource::XML_ATT_CHAT_STATE = "chat_state";
const std::string MapMseResource::XML_ATT_PRIORITY = "priority";
const std::string MapMseResource::XML_ATT_PRESENCE_TEXT = "presence_text";
const std::string MapMseResource::XML_ATT_PRESENCE_AVAILABILITY = "presence_availability";
const std::string MapMseResource::XML_ATT_X_BT_UID = "x_bt_uid";
const std::string MapMseResource::TYPE_NEW = "NewMessage";
const std::string MapMseResource::TYPE_DELETE = "MessageDeleted";
const std::string MapMseResource::TYPE_SHIFT = "MessageShift";
const std::string MapMseResource::TYPE_SENDING_SUCCESS = "SendingSuccess";
const std::string MapMseResource::TYPE_SENDING_FAILURE = "SendingFailure";
const std::string MapMseResource::TYPE_DELIVERY_SUCCESS = "DeliverySuccess";
const std::string MapMseResource::TYPE_DELIVERY_FAILURE = "DeliveryFailure";
const std::string MapMseResource::TYPE_MEMORY_FULL = "MemoryFull";
const std::string MapMseResource::TYPE_MEMORY_AVAILABLE = "MemoryAvailable";
const std::string MapMseResource::TYPE_READ_STATUS = "ReadStatusChanged";
const std::string MapMseResource::TYPE_CONVERSATION = "ConversationChanged";
const std::string MapMseResource::TYPE_PRESENCE = "ParticipantPresenceChanged";
const std::string MapMseResource::TYPE_CHAT_STATE = "ParticipantChatStateChanged";
const std::string MapMseResource::TYPE_EXTENDED_DATA = "MessageExtendedDataChanged";
const std::string MapMseResource::TYPE_REMOVED = "MessageRemoved";

MapMseResource::MapMseResource(MapMseMnscli &mnsClient, utility::Dispatcher &dispatcher, MapMseInstance &instance)
    : mnsClient_(&mnsClient), dispatcher_(dispatcher), instance_(instance), reportVersion_(MAP_EVENT_REPORT_V10)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    masId_ = instance_.GetMasId();
    observerMap_[masId_] = std::make_unique<EventObserver>(*this);
    for (auto iter = observerMap_.begin(); iter != observerMap_.end(); iter++) {
        stub::MapService::GetInstance()->RegisterObserver(
            (stub::MapServiceObserver *)observerMap_[masId_].get(), masId_);
    }
}

MapMseResource::~MapMseResource()
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    for (auto iter = observerMap_.begin(); iter != observerMap_.end(); iter++) {
        stub::MapService::GetInstance()->DeregisterObserver(iter->second.get(), masId_);
    }
    observerMap_.clear();
}

std::string MapMseResource::GetParticipantSql(const MapMseParams &appParameter)
{
    std::string sql = "select uci, display_name, chat_state, last_activity, x_bt_uid, name, presence_availability, "
                      "presence_text, priority from Participant where convo_id = ? ";

    if (appParameter.filterRecipient_ != "") {
        std::string recpt = appParameter.filterRecipient_;
        if (recpt.find('*') == std::string::npos) {
            sql.append(" and (convo_id like '%' || ? || '%' or name like '%' || ? || '%')");
        } else {
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                sql.append(" and (convo_id like ? || '%' or name like ? || '%')");
            } else if (right != "") {
                sql.append(" and (convo_id like '%' || ? or name like '%' || ?)");
            }
        }
    }
    return sql;
}

std::string MapMseResource::GetCvslistSql(const MapMseParams &appParameter)
{
    std::string sql = "select convo_id, name, last_activity, read_status, version_counter,"
                      "summary from Conversation where 1 = 1";

    if (appParameter.filterLastActivityBegin_ != "") {
        sql.append(" and last_activity >= ?");
    }
    if (appParameter.filterLastActivityEnd_ != "") {
        sql.append(" and last_activity <= ?");
    }
    if (appParameter.filterReadStatus_ != nullptr) {
        if (*appParameter.filterReadStatus_ == 0x1 || *appParameter.filterReadStatus_ == 0x2) {
            sql.append(" and read_status = ?");
        }
    }
    if (appParameter.conversationId_ != "") {
        sql.append(" and convo_id = ?");
    }
    sql.append(" order by id desc");
    if (appParameter.maxListCount_ != 0x0) {
        sql.append(" limit ?");
        sql.append(" offset ?");
    }
    return sql;
}

void MapMseResource::SetParticipantParam(
    IDataStatement &ids, const MapMseParams &appParameter, const std::string &convoId)
{
    ids.ClearParams();
    int index = 1;
    ids.SetParamString(index++, convoId);
    if (appParameter.filterRecipient_ != "") {
        if (appParameter.filterRecipient_.find('*') == std::string::npos) {
            ids.SetParamString(index++, appParameter.filterRecipient_);
            ids.SetParamString(index++, appParameter.filterRecipient_);
        } else {
            std::string recpt = appParameter.filterRecipient_;
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                ids.SetParamString(index++, left);
                ids.SetParamString(index++, left);
            } else if (right != "") {
                ids.SetParamString(index++, right);
                ids.SetParamString(index++, right);
            }
        }
    }
}

void MapMseResource::SetCvslistParam(IDataStatement &ids, const MapMseParams &appParameter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int index = 1;
    if (appParameter.filterLastActivityBegin_ != "") {
        ids.SetParamString(index++, appParameter.filterLastActivityBegin_);
    }
    if (appParameter.filterLastActivityEnd_ != "") {
        ids.SetParamString(index++, appParameter.filterLastActivityEnd_);
    }
    if (appParameter.filterReadStatus_ != nullptr) {
        if (*appParameter.filterReadStatus_ == 0x1) {
            ids.SetParamInt(index++, 0);
        } else if (*appParameter.filterReadStatus_ == 0x2) {
            ids.SetParamInt(index++, 1);
        }
    }
    if (appParameter.conversationId_ != "") {
        ids.SetParamString(index++, appParameter.conversationId_);
    }
    if (appParameter.maxListCount_ != 0x0) {
        ids.SetParamInt(index++, appParameter.maxListCount_);
        ids.SetParamInt(index++, appParameter.listStartOffSet_);
    }
}

int MapMseResource::GetConvoLstCount(const MapMseParams &appParameter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int count = 0;
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    if (!dataProvider) {
        return count;
    }
    std::string sql = GetCvslistSql(appParameter);
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return count;
    }
    SetCvslistParam(*stmt, appParameter);
    auto dataResult = stmt->Query();
    std::string tempId;
    while (dataResult->Next()) {
        if (tempId != dataResult->GetString(0)) {
            tempId = dataResult->GetString(0);
            count++;
        }
    }
    return count;
}

std::vector<MseParticipant> MapMseResource::GetParticipantContent(
    IDataStatement &ids, const MapMseParams &appParameter, const std::string &convoId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    SetParticipantParam(ids, appParameter, convoId);
    auto dataResult = ids.Query();
    std::vector<MseParticipant> contacts;
    while (dataResult->Next()) {
        MseParticipant contact;
        contact.uci_ = dataResult->GetString(0);
        contact.displayName_ = dataResult->GetString(0x1);
        if (dataResult->IsNotNull(0x2)) {
            contact.chatState_ = dataResult->GetInt(0x2);
        }
        contact.lastActy_ = dataResult->GetString(0x3);
        contact.btUid_ = dataResult->GetString(0x4);
        contact.name_ = dataResult->GetString(0x5);
        if (dataResult->IsNotNull(0x6)) {
            contact.presenceAvailability_ = dataResult->GetInt(0x6);
        }
        contact.presenceText_ = dataResult->GetString(0x7);
        if (dataResult->IsNotNull(0x8)) {
            contact.priority_ = dataResult->GetInt(0x8);
        }
        contacts.push_back(contact);
        MSE_LOG_INFO("contacts : %{public}zu Enter", contacts.size());
    }
    return contacts;
}

std::vector<std::unique_ptr<MseConvoLstElement>> MapMseResource::GetConvoLstContent(const MapMseParams &appParameter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::vector<std::unique_ptr<MseConvoLstElement>> convoList;
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    if (!dataProvider) {
        return convoList;
    }
    std::string sql = GetCvslistSql(appParameter);
    auto stmt = dataProvider->CreateStatement(sql);
    SetCvslistParam(*stmt, appParameter);
    auto dataResult = stmt->Query();
    std::unordered_map<std::string, std::unique_ptr<MseConvoLstElement>> convoElementMap;
    std::vector<std::string> convoIds;
    while (dataResult->Next()) {
        std::string convId = dataResult->GetString(0);
        auto convoElement = std::make_unique<MseConvoLstElement>();
        convoElement->convoId_ = convId;
        convoElement->name_ = dataResult->GetString(0x1);
        convoElement->lastActivity_ = dataResult->GetString(0x2);
        if (dataResult->IsNotNull(0x3))
            convoElement->readStatus_ = dataResult->GetInt(0x3);
        convoElement->versionCounter_ = dataResult->GetString(0x4);
        convoElement->summary_ = dataResult->GetString(0x5);
        convoElementMap[convId] = std::move(convoElement);
        convoIds.push_back(convId);
    }

    std::string sql2 = GetParticipantSql(appParameter);
    auto stmt2 = dataProvider->CreateStatement(sql2);
    for (auto &iter : convoIds) {
        auto contacts = GetParticipantContent(*stmt2, appParameter, iter);
        MSE_LOG_INFO("contacts : %{public}zu Enter", contacts.size());
        convoElementMap[iter]->contacts_ = contacts;
        convoList.push_back(std::move(convoElementMap[iter]));
    }
    return convoList;
}

void MapMseResource::ParticipantsToXml(xmlNodePtr &node, std::vector<MseParticipant> &contacts, long paraMask)
{
    for (auto &contact : contacts) {
        xmlNodePtr subNode = xmlNewNode(NULL, BAD_CAST XML_TAG_PARTICIPANT.c_str());
        xmlAddChild(node, subNode);
        if (CheckParameterMask(PARTI_UCI_MASK, contact.uci_, paraMask, true)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_UCI.c_str(), BAD_CAST contact.uci_.c_str());
        }
        if (CheckParameterMask(PARTI_DISPLAY_NAME_MASK, contact.displayName_, paraMask, true)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_DISPLAY_NAME.c_str(), BAD_CAST contact.displayName_.c_str());
        }
        if (CheckParameterMask(PARTI_CHAT_STATE_MASK, std::to_string(contact.chatState_), paraMask, true)) {
            xmlNewProp(subNode,
                BAD_CAST XML_ATT_CHAT_STATE.c_str(),
                BAD_CAST std::to_string(contact.chatState_).c_str());
        }
        if (CheckParameterMask(PARTI_LAST_ACTIVITY_MASK, contact.lastActy_, paraMask, true)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_LAST_ACTIVITY.c_str(), BAD_CAST contact.lastActy_.c_str());
        }
        if (CheckParameterMask(PARTI_X_BT_UID_MASK, contact.btUid_, paraMask)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_X_BT_UID.c_str(), BAD_CAST contact.btUid_.c_str());
        }
        if (CheckParameterMask(PARTI_NAME_MASK, contact.name_, paraMask)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_NAME.c_str(), BAD_CAST contact.name_.c_str());
        }
        if (CheckParameterMask(PARTI_PRESENCE_AVAIL_MASK, std::to_string(contact.presenceAvailability_), paraMask)) {
            xmlNewProp(subNode,
                BAD_CAST XML_ATT_PRESENCE_AVAILABILITY.c_str(),
                BAD_CAST std::to_string(contact.presenceAvailability_).c_str());
        }
        if (CheckParameterMask(PARTI_PRESENCE_TEXT_MASK, contact.presenceText_, paraMask)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_PRESENCE_TEXT.c_str(), BAD_CAST contact.presenceText_.c_str());
        }
        if (CheckParameterMask(PARTI_PRIORITY_MASK, std::to_string(contact.priority_), paraMask)) {
            xmlNewProp(subNode, BAD_CAST XML_ATT_PRIORITY.c_str(), BAD_CAST std::to_string(contact.priority_).c_str());
        }
    }
}

std::string MapMseResource::GetCvslistBodyXml(const MapMseParams &appParameter, uint16_t &size)
{
    long paraMask = appParameter.convParameterMask_;
    auto convoList = GetConvoLstContent(appParameter);
    size = convoList.size();

    xmlChar *xmlbuff;
    int buffersize;
    std::ostringstream oss;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST XML_TAG_CONVO_LISTING.c_str());
    xmlDocSetRootElement(doc, root_node);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST MAP_V10.c_str());

    for (auto &iter : convoList) {
        xmlNodePtr node = xmlNewNode(NULL, BAD_CAST XML_TAG_CONVERSATION.c_str());
        xmlAddChild(root_node, node);
        xmlNewProp(node, BAD_CAST XML_ATT_ID.c_str(), BAD_CAST iter->convoId_.c_str());

        if (CheckParameterMask(CONVO_LISTING_NAME_MASK, iter->name_, paraMask, true)) {
            xmlNewProp(node, BAD_CAST XML_ATT_NAME.c_str(), BAD_CAST iter->name_.c_str());
        }
        if (CheckParameterMask(CONVO_LAST_ACTIVITY_MASK, iter->lastActivity_, paraMask, true)) {
            xmlNewProp(node, BAD_CAST XML_ATT_LAST_ACTIVITY.c_str(), BAD_CAST iter->lastActivity_.c_str());
        }
        std::string read = (iter->readStatus_ == 1) ? "yes" : "no";
        if (CheckParameterMask(CONVO_READ_STATUS_MASK, read, paraMask, true)) {
            xmlNewProp(node, BAD_CAST XML_ATT_READ.c_str(), BAD_CAST read.c_str());
        }
        if (CheckParameterMask(CONVO_VERSION_COUNTER_MASK, iter->versionCounter_, paraMask, true)) {
            xmlNewProp(node, BAD_CAST XML_ATT_VERSION_COUNTER.c_str(), BAD_CAST iter->versionCounter_.c_str());
        }
        if (CheckParameterMask(CONVO_SUMMARY_MASK, iter->summary_, paraMask)) {
            xmlNewProp(node, BAD_CAST XML_ATT_SUMMARY.c_str(), BAD_CAST iter->summary_.c_str());
        }
        if (CheckParameterMask(PARTI_MASK, "Participants", paraMask) || paraMask == -1) {
            ParticipantsToXml(node, iter->contacts_, paraMask);
        }
    }
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    oss << xmlbuff;
    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return oss.str();
}

void MapMseResource::AddWhereSql(const MapMseParams &appParameter, std::string &sql)
{
    if (appParameter.filterOriginator_ != "" && appParameter.filterOriginator_ != "*") {
        std::string recpt = appParameter.filterOriginator_;
        sql.append(" and m.handle in ( select v.handle from Vcard v where v.is_originator = 1");
        if (recpt.find('*') == std::string::npos) {
            sql.append(" and (v.name like '%' || ? || '%' or v.phone_numbers like '%' || ? || '%' or v.email_addresses "
                       "like '%' || ? || '%'))");
        } else {
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                sql.append(" and (v.name like ? || '%' or v.phone_numbers like ? || '%' or v.email_addresses like "
                           "? || '%'))");
            } else if (right != "") {
                sql.append(" and (v.name like '%' || ? or v.phone_numbers like '%' || ? or v.email_addresses like "
                           "'%' || ?))");
            }
        }
    }
    if (appParameter.filterRecipient_ != "" && appParameter.filterRecipient_ != "*") {
        std::string recpt = appParameter.filterRecipient_;
        sql.append(" and m.handle in ( select v2.handle from Vcard v2 where v2.is_originator = 0");
        if (recpt.find('*') == std::string::npos) {
            sql.append(
                " and (v2.name like '%' || ? || '%' or v2.phone_numbers like '%' || ? || '%' or v2.email_addresses "
                "like '%' || ? || '%'))");
        } else {
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                sql.append(" and (v2.name like ? || '%' or v2.phone_numbers like ? || '%' or v2.email_addresses like "
                           "? || '%'))");
            } else if (right != "") {
                sql.append(" and (v2.name like '%' || ? or v2.phone_numbers like '%' || ? or v2.email_addresses like "
                           "'%' || ?))");
            }
        }
    }
    sql.append(" order by m.datetime desc");
    if (appParameter.maxListCount_ != 0x0) {
        sql.append(" limit ?");
        sql.append(" offset ?");
    }
}

std::string MapMseResource::GetMsglistSql(const MapMseParams &appParameter, const std::string &folderName)
{
    std::string sql = "select m.handle,m.subject,m.datetime,m.sender_name,m.sender_addressing,m.replyto_addressing,"
          "m.recipient_name,m.recipient_addressing,m.type,m.size,m.text,m.reception_status,m.attachment_size,"
          "m.priority,m.read,m.sent,m.protected,m.delivery_status,m.conversation_id,m.conversation_name,"
          "m.direction,m.attachment_mime_types from MessageList m where 1 = 1";
    if (folderName != "") {
        sql.append(" and m.folder = ?");
    }
    if (appParameter.filterMessageHandle_ != "") {
        sql.append(" and m.handle = ?");
    }
    if (appParameter.conversationId_ != "") {
        sql.append(" and m.conversation_id = ?");
    }
    if (appParameter.filterMessageType_ != nullptr) {
        int count = GetMsgTypeParam(*appParameter.filterMessageType_).size();
        for (int i = 0; i < count; i++) {
            if (i == 0) {
                sql.append(" and m.type not in (");
            }
            sql.append(" ?");
            if (i == count - 1) {
                sql.append(" )");
            } else {
                sql.append(" ,");
            }
        }
    }
    if (appParameter.filterPeriodBegin_ != "") {
        sql.append(" and ? <= m.datetime");
    }
    if (appParameter.filterPeriodEnd_ != "") {
        sql.append(" and m.datetime <= ?");
    }
    if (appParameter.filterReadStatus_ != nullptr) {
        if (*appParameter.filterReadStatus_ == 1 || *appParameter.filterReadStatus_ == 0x2) {
            sql.append(" and m.read = ?");
        }
    }
    if (appParameter.filterPriority_ != nullptr) {
        if (*appParameter.filterPriority_ == 1 || *appParameter.filterPriority_ == 0x2) {
            sql.append(" and m.priority = ?");
        }
    }
    AddWhereSql(appParameter, sql);
    return sql;
}

void MapMseResource::SetContacts(IDataStatement &ids, const MapMseParams &appParameter, int &index)
{
    if (appParameter.filterOriginator_ != "") {
        if (appParameter.filterOriginator_.find('*') == std::string::npos) {
            ids.SetParamString(index++, appParameter.filterOriginator_);
            ids.SetParamString(index++, appParameter.filterOriginator_);
            ids.SetParamString(index++, appParameter.filterOriginator_);
        } else {
            std::string recpt = appParameter.filterOriginator_;
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                ids.SetParamString(index++, left);
                ids.SetParamString(index++, left);
                ids.SetParamString(index++, left);
            } else if (right != "") {
                ids.SetParamString(index++, right);
                ids.SetParamString(index++, right);
                ids.SetParamString(index++, right);
            }
        }
    }
    if (appParameter.filterRecipient_ != "") {
        if (appParameter.filterRecipient_.find('*') == std::string::npos) {
            ids.SetParamString(index++, appParameter.filterRecipient_);
            ids.SetParamString(index++, appParameter.filterRecipient_);
            ids.SetParamString(index++, appParameter.filterRecipient_);
        } else {
            std::string recpt = appParameter.filterRecipient_;
            std::size_t i = recpt.find('*');
            std::string left = recpt.substr(0, i);
            std::string right = recpt.substr(i + 1);
            if (left != "") {
                ids.SetParamString(index++, left);
                ids.SetParamString(index++, left);
                ids.SetParamString(index++, left);
            } else if (right != "") {
                ids.SetParamString(index++, right);
                ids.SetParamString(index++, right);
                ids.SetParamString(index++, right);
            }
        }
    }
}

std::vector<int> MapMseResource::GetMsgTypeParam(uint8_t mask)
{
    std::vector<int> ret {};
    if (mask == 0) {
        return ret;
    }
    if ((MASK_MSG_SMS_GSM & mask) > 0) {
        ret.push_back(0x2);
    }
    if ((MASK_MSG_SMS_CDMA & mask) > 0) {
        ret.push_back(0x3);
    }
    if ((MASK_MSG_MMS & mask) > 0) {
        ret.push_back(0x4);
    }
    if ((MASK_MSG_IM & mask) > 0) {
        ret.push_back(0x5);
    }
    if ((MASK_MSG_EMAIL & mask) > 0) {
        ret.push_back(0x1);
    }
    return ret;
}

void MapMseResource::SetMsglistParam(
    IDataStatement &ids, const MapMseParams &appParameter, const std::string &folderName)
{
    int index = 1;
    ids.ClearParams();
    if (folderName != "") {
        ids.SetParamString(index++, folderName);
    }
    if (appParameter.filterMessageHandle_ != "") {
        ids.SetParamString(index++, appParameter.filterMessageHandle_);
    }
    if (appParameter.conversationId_ != "") {
        ids.SetParamString(index++, appParameter.conversationId_);
    }
    if (appParameter.filterMessageType_ != nullptr) {
        for (auto &iter : GetMsgTypeParam(*appParameter.filterMessageType_)) {
            ids.SetParamInt(index++, iter);
        }
    }
    if (appParameter.filterPeriodBegin_ != "") {
        ids.SetParamString(index++, appParameter.filterPeriodBegin_);
    }
    if (appParameter.filterPeriodEnd_ != "") {
        ids.SetParamString(index++, appParameter.filterPeriodEnd_);
    }
    if (appParameter.filterReadStatus_ != nullptr) {
        if (*appParameter.filterReadStatus_ == 0x1) {
            ids.SetParamInt(index++, 0);
        } else if (*appParameter.filterReadStatus_ == 0x2) {
            ids.SetParamInt(index++, 1);
        }
    }
    if (appParameter.filterPriority_ != nullptr) {
        if (*appParameter.filterPriority_ == 0x1) {
            ids.SetParamInt(index++, 1);
        } else if (*appParameter.filterPriority_ == 0x2) {
            ids.SetParamInt(index++, 0);
        }
    }
    SetContacts(ids, appParameter, index);
    if (appParameter.maxListCount_ != 0x0) {
        ids.SetParamInt(index++, appParameter.maxListCount_);
        ids.SetParamInt(index++, appParameter.listStartOffSet_);
    }
}

std::vector<std::unique_ptr<MseMessageElement>> MapMseResource::GetMessageListContent(
    const std::string &folderName, const MapMseParams &appParameter)
{
    std::vector<std::unique_ptr<MseMessageElement>> messageList;
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    if (!dataProvider) {
        return messageList;
    }
    std::string sql = GetMsglistSql(appParameter, folderName);
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return messageList;
    }
    SetMsglistParam(*stmt, appParameter, folderName);
    auto dataResult = stmt->Query();
    while (dataResult->Next()) {
        auto messageEle = std::make_unique<MseMessageElement>();
        int index = 0;
        messageEle->handle_ = dataResult->GetString(index++);
        std::string subject = dataResult->GetString(index++);
        if (appParameter.subjectLength_ != nullptr) {
            std::size_t end = *appParameter.subjectLength_;
            subject = subject.substr(0, end);
        }
        messageEle->subject_ = subject;
        messageEle->dateTime_ = dataResult->GetString(index++);
        messageEle->senderName_ = dataResult->GetString(index++);
        messageEle->senderAddressing_ = dataResult->GetString(index++);
        messageEle->replytoAddressing_ = dataResult->GetString(index++);
        messageEle->recipientName_ = dataResult->GetString(index++);
        messageEle->recipientAddressing_ = dataResult->GetString(index++);
        messageEle->type = MessageType(dataResult->GetInt(index++));
        messageEle->size_ = dataResult->GetInt(index++);
        messageEle->text_ = (dataResult->GetInt(index++) == 1) ? "yes" : "no";
        messageEle->receptionStatus_ = dataResult->GetString(index++);
        messageEle->attachmentSize_ = dataResult->GetInt(index++);
        messageEle->priority_ = (dataResult->GetInt(index++) == 1) ? "yes" : "no";
        messageEle->read_ = (dataResult->GetInt(index++) == 1) ? true : false;
        messageEle->sent_ = (dataResult->GetInt(index++) == 1) ? "yes" : "no";
        messageEle->protected_ = (dataResult->GetInt(index++) == 1) ? "yes" : "no";
        messageEle->deliveryStatus_ = dataResult->GetString(index++);  // Messages-Listing Format Version 1.1
        messageEle->conversationId_ = dataResult->GetString(index++);
        messageEle->conversationName_ = dataResult->GetString(index++);
        messageEle->direction_ = dataResult->GetString(index++);
        messageEle->attachmentMimeTypes_ = dataResult->GetString(index++);
        messageList.push_back(std::move(messageEle));
    }
    return messageList;
}

std::string MapMseResource::GetBodyXml(const std::string &folderName, const MapMseParams &appParameter,
    const std::string &version, uint16_t &listSize, uint8_t &unRead)
{
    auto messageList = GetMessageListContent(folderName, appParameter);
    listSize = messageList.size();
    if (listSize == 0) {
        return "";
    }
    return GetPrivateBodyXml(messageList, version, unRead, appParameter.parameterMask_);
}

void MapMseResource::AddVer10Node(xmlNodePtr &node, MseMessageElement &msg, uint8_t &unRead, long paraMask)
{
    xmlNewProp(node, BAD_CAST "handle", BAD_CAST msg.handle_.c_str());
    if (CheckParameterMask(SUBJECT_MASK, msg.subject_, paraMask, true))
        xmlNewProp(node, BAD_CAST "subject", BAD_CAST msg.subject_.c_str());
    if (CheckParameterMask(DATETIME_MASK, msg.dateTime_, paraMask, true))
        xmlNewProp(node, BAD_CAST "datetime", BAD_CAST msg.dateTime_.c_str());
    if (CheckParameterMask(SENDER_NAME_MASK, msg.senderName_, paraMask))
        xmlNewProp(node, BAD_CAST "sender_name", BAD_CAST msg.senderName_.c_str());
    if (CheckParameterMask(SENDER_ADDRESSING_MASK, msg.senderAddressing_, paraMask))
        xmlNewProp(node, BAD_CAST "sender_addressing", BAD_CAST msg.senderAddressing_.c_str());
    if (CheckParameterMask(REPLYTO_ADDRESSING, msg.replytoAddressing_, paraMask))
        xmlNewProp(node, BAD_CAST "replyto_addressing", BAD_CAST msg.replytoAddressing_.c_str());
    if (CheckParameterMask(RECIPIENT_NAME_MASK, msg.recipientName_, paraMask))
        xmlNewProp(node, BAD_CAST "recipient_name", BAD_CAST msg.recipientName_.c_str());
    if (CheckParameterMask(RECIPIENT_ADDRESSING_MASK, msg.recipientAddressing_, paraMask, true))
        xmlNewProp(node, BAD_CAST "recipient_addressing", BAD_CAST msg.recipientAddressing_.c_str());
    if (static_cast<int>(msg.type) > 0) {
        if (CheckParameterMask(TYPE_MASK, std::to_string(static_cast<int>(msg.type)), paraMask, true))
            xmlNewProp(node, BAD_CAST "type", BAD_CAST ChangeType(msg.type).c_str());
    }
    if (CheckParameterMask(SIZE_MASK, std::to_string(msg.size_), paraMask, true))
        xmlNewProp(node, BAD_CAST "size", BAD_CAST std::to_string(msg.size_).c_str());
    if (CheckParameterMask(TEXT_MASK, msg.text_, paraMask))
        xmlNewProp(node, BAD_CAST "text", BAD_CAST msg.text_.c_str());
    if (CheckParameterMask(RECEPTION_STATUS_MASK, msg.receptionStatus_, paraMask, true))
        xmlNewProp(node, BAD_CAST "reception_status", BAD_CAST msg.receptionStatus_.c_str());
    if (CheckParameterMask(ATTACHMENT_SIZE_MASK, std::to_string(msg.attachmentSize_), paraMask, true))
        xmlNewProp(node, BAD_CAST "attachment_size", BAD_CAST std::to_string(msg.attachmentSize_).c_str());
    if (CheckParameterMask(PRIORITY_MASK, msg.priority_, paraMask))
        xmlNewProp(node, BAD_CAST "priority", BAD_CAST msg.priority_.c_str());
    std::string read = msg.read_ ? "yes" : "no";
    unRead = msg.read_ ? 0 : 1;
    if (CheckParameterMask(READ_MASK, read, paraMask))
        xmlNewProp(node, BAD_CAST "read", BAD_CAST read.c_str());
    if (CheckParameterMask(SENT_MASK, msg.sent_, paraMask))
        xmlNewProp(node, BAD_CAST "sent", BAD_CAST msg.sent_.c_str());
    if (CheckParameterMask(PROTECTED_MASK, msg.protected_, paraMask))
        xmlNewProp(node, BAD_CAST "protected", BAD_CAST msg.protected_.c_str());
}

std::string MapMseResource::ChangeType(MessageType type)
{
    if (type == MessageType::EMAIL) {
        return "EMAIL";
    } else if (type == MessageType::IM) {
        return "IM";
    } else if (type == MessageType::MMS) {
        return "MMS";
    } else if (type == MessageType::SMS_GSM) {
        return "SMS_GSM";
    } else if (type == MessageType::SMS_CDMA) {
        return "SMS_CDMA";
    } else {
        return "NONE";
    }
}

std::string MapMseResource::GetPrivateBodyXml(std::vector<std::unique_ptr<MseMessageElement>> &messageList,
    const std::string &version, uint8_t &unRead, long paraMask)
{
    xmlChar *xmlbuff;
    int buffersize;
    std::ostringstream oss;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST MAP_V10.c_str());
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "MAP-msg-listing");
    xmlDocSetRootElement(doc, root_node);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST version.c_str());

    for (auto &iter : messageList) {
        xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"msg");
        xmlAddChild(root_node, node);

        AddVer10Node(node, *iter, unRead, paraMask);
        if (MAP_V11 == version) {
            if (CheckParameterMask(DELIVERY_STATUS_MASK, iter->deliveryStatus_, paraMask))
                xmlNewProp(node, BAD_CAST"delivery_status", BAD_CAST iter->deliveryStatus_.c_str());
            if (CheckParameterMask(CONVERSATION_ID_MASK, iter->conversationId_, paraMask, true))
                xmlNewProp(node, BAD_CAST"conversation_id", BAD_CAST iter->conversationId_.c_str());
            if (CheckParameterMask(CONVERSATION_NAME_MASK, iter->conversationName_, paraMask))
                xmlNewProp(node, BAD_CAST"conversation_name", BAD_CAST iter->conversationName_.c_str());
            if (CheckParameterMask(DIRECTION_MASK, iter->direction_, paraMask, true))
                xmlNewProp(node, BAD_CAST"direction", BAD_CAST iter->direction_.c_str());
            if (CheckParameterMask(ATTACHMENT_NIME_MASK, iter->attachmentMimeTypes_, paraMask))
                xmlNewProp(node, BAD_CAST"attachment_mime_types", BAD_CAST iter->attachmentMimeTypes_.c_str());
        }
    }
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    oss << xmlbuff;
    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return oss.str();
}

bool MapMseResource::CheckParameterMask(uint32_t mask, std::string para, long paraMask, bool required)
{
    if (para.size() > 0) {
        if (required) {
            return true;
        }
        if (paraMask != -1) {
            if (paraMask == 0 || ((static_cast<uint32_t>(paraMask) & mask) > 0)) {
                return true;
            }
        }
    }
    return false;
}

void MapMseResource::GetListingSize(
    const std::string &folderName, const MapMseParams &appParameter, uint16_t &listSize, uint8_t &unRead)
{
    auto messagesList = GetMessageListContent(folderName, appParameter);
    listSize = messagesList.size();
    bool isExist = std::any_of(messagesList.begin(), messagesList.end(),
        [](const std::unique_ptr<bluetooth::MseMessageElement>& item) {return !item->read_;});
    if (isExist) {
        unRead = 1;
    }
}

std::string MapMseResource::GetMessage(
    const std::string handle, const MapMseParams &appParameter, const std::string &version)
{
    MapMseBmsg data;
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE);
    if (!dataProvider) {
        return "";
    }
    std::string sql = "select body, status, type, folder_path, extended_data, part_id, encoding, "
                      "conent_charset, language, content_length from Message where handle = ?";
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return "";
    }
    stmt->SetParamString(1, handle);
    auto dataResult = stmt->Query();
    while (dataResult->Next()) {
        int index = 0;
        data.SetMsgBody(dataResult->GetString(index++));
        data.SetVersion(version);
        data.SetStatus(dataResult->GetString(index++));
        data.SetMsgTypeName(dataResult->GetString(index++));
        data.SetFolder(dataResult->GetString(index++));
        data.SetExtendedData(dataResult->GetString(index++));
        data.SetPartId(std::atoi(dataResult->GetString(index++).data()));
        data.SetEncoding(dataResult->GetString(index++));
        data.SetCharset(dataResult->GetString(index++));
        data.SetLanguage(dataResult->GetString(index++));
        data.SetBmsgLength(std::atoi(dataResult->GetString(index++).data()));
        data.SetOriginators(GetVCardData(dataProvider, handle, 1));
        data.SetRecipients(GetVCardData(dataProvider, handle));
    }
    return data.ToBmessageString();
}

std::vector<MapMseVcard> MapMseResource::GetVCardData(
    std::unique_ptr<DataAccess> &dataAccess, const std::string handle, const int isOriginator)
{
    std::vector<MapMseVcard> result;
    std::string sql = "select version, name, formatted_name, phone_numbers, email_addresses, x_bt_uid, "
                      "x_bt_ucis, env_level from Vcard where handle = ? and is_originator = ?";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return result;
    }
    stmt->SetParamString(1, handle);
    stmt->SetParamInt(0x2, isOriginator);
    auto dataResult = stmt->Query();
    while (dataResult->Next()) {
        int index = 0;
        MseVcard3 vcard3;
        vcard3.version_ = dataResult->GetString(index++);
        vcard3.name_ = dataResult->GetString(index++);
        vcard3.formattedName_ = dataResult->GetString(index++);
        vcard3.phoneNumbers_.push_back(dataResult->GetString(index++));
        vcard3.emailAddresses_.push_back(dataResult->GetString(index++));
        vcard3.btUids_.push_back(dataResult->GetString(index++));
        vcard3.btUcis_.push_back(dataResult->GetString(index++));
        vcard3.envLevel_ = dataResult->GetInt(index++);
        if (vcard3.version_ != "3.0") {
            vcard3.version_ = "2.1";
        }
        MapMseVcard vcd(vcard3);
        result.push_back(vcd);
    }
    return result;
}

void MapMseResource::SetNotificationRegistration(const RawAddress &remoteAddr, const int status)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (NotificationStatus(status) == NotificationStatus::ON) {
        mnsClient_->Connect(remoteAddr, masId_);
    } else {
        mnsClient_->DisConnect(remoteAddr, masId_);
    }
}

void MapMseResource::SetNotificationFilter(const RawAddress &remoteAddr, uint32_t filter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    MSE_LOG_INFO("Notification Filter value : %u", filter);
    std::string addr = remoteAddr.GetAddress();
    eventFilterMaskMap_[addr] = filter;
}

void MapMseResource::SetRemoteFeatureMask(uint32_t features)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    remoteFeatures_ = features;
}

void MapMseResource::SendEventReport(const std::string &addr, const std::string &event, const std::string &type)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool has = false;
    if (type == TYPE_NEW) {
        has = CheckEventFilterMask(addr, FILTER_MASK_NEW_MESSAGE);
    } else if (type == TYPE_DELETE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_MESSAGE_DELETED);
    } else if (type == TYPE_SHIFT) {
        has = CheckEventFilterMask(addr, FILTER_MASK_MESSAGE_SHIFT);
    } else if (type == TYPE_SENDING_SUCCESS) {
        has = CheckEventFilterMask(addr, FILTER_MASK_SENDING_SUCCESS);
    } else if (type == TYPE_SENDING_FAILURE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_SENDING_FAILURE);
    } else if (type == TYPE_DELIVERY_SUCCESS) {
        has = CheckEventFilterMask(addr, FILTER_MASK_DELIVERY_SUCCESS);
    } else if (type == TYPE_DELIVERY_FAILURE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_DELIVERY_FAILURE);
    } else if (type == TYPE_MEMORY_FULL) {
        has = CheckEventFilterMask(addr, FILTER_MASK_MEMORY_FULL);
    } else if (type == TYPE_MEMORY_AVAILABLE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_MEMORY_AVAILABLE);
    } else if (type == TYPE_READ_STATUS) {
        has = CheckEventFilterMask(addr, FILTER_MASK_READ_STATUS_CHANGED);
    } else if (type == TYPE_CONVERSATION) {
        has = CheckEventFilterMask(addr, FILTER_MASK_CONVERSATION_CHANGED);
    } else if (type == TYPE_PRESENCE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_PARTICIPANT_PRESENCE_CHANGED);
    } else if (type == TYPE_CHAT_STATE) {
        has = CheckEventFilterMask(addr, FILTER_MASK_PARTICIPANT_CHATSTATE_CHANGED);
    } else if (type == TYPE_EXTENDED_DATA) {
        has = CheckEventFilterMask(addr, FILTER_MASK_EXTENDED_DATA_CHANGED);
    } else if (type == TYPE_REMOVED) {
        has = CheckEventFilterMask(addr, FILTER_MASK_MESSAGE_REMOVED);
    }
    if (!has) {
        return;
    }
    MSE_LOG_INFO("Event body: %{public}s ", event.c_str());
    mnsClient_->SendEvent(RawAddress(addr), event, event.size(), masId_);
}

bool MapMseResource::CheckEventFilterMask(const std::string &addr, const uint32_t bit)
{
    bool ret = false;
    if (eventFilterMaskMap_.find(addr) == eventFilterMaskMap_.end()) {
        MSE_LOG_INFO("set notification filter function is not used, then all event will be sent to the MCE.");
        ret = true;
    } else {
        if ((eventFilterMaskMap_[addr] & bit) > 0) {
            ret = true;
        }
    }
    return ret;
}

MapMseResource::EventObserver::EventObserver(MapMseResource &observer) : observer_(observer)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
}

void MapMseResource::EventObserver::OnNotifyingEvent(const std::string &addr, stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    observer_.SendEvent(addr, event);
}

void MapMseResource::EventObserver::OnUpdateDbIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    observer_.UpdateDbIdCounter();
}

void MapMseResource::EventObserver::OnUpdateVerIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    observer_.UpdateVerIdCounter();
}

void MapMseResource::EventObserver::OnUpdateConvoVerIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    observer_.UpdateConvoVerIdCounter();
}

void MapMseResource::UpdateDbIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    instance_.UpdateDatabaseIdentifier();
}

void MapMseResource::UpdateVerIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    instance_.UpdateFolderVersionCounter();
}

void MapMseResource::UpdateConvoVerIdCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    instance_.UpdateConversationVersionCounter();
}

void MapMseResource::SendEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    uint32_t feature = GetRemoteFeatureMask(addr);
    if ((MAP_FEATURE_EVENT_REPORT_V12_BIT & feature) != 0) {
        reportVersion_ = MAP_EVENT_REPORT_V12;
    } else if ((MAP_FEATURE_EXTENDED_EVENT_REPORT_11_BIT & feature) != 0) {
        reportVersion_ = MAP_EVENT_REPORT_V11;
    }
    dispatcher_.PostTask(std::bind(&MapMseResource::SendEventNative, this, addr, event));
}

void MapMseResource::SendEventNative(const std::string &addr, const stub::EventInfo &event)
{
    switch (event.eventType) {
        case stub::EventType::NEW_MESSAGE:
            SendNewMessageEvent(addr, event);
            break;
        case stub::EventType::MESSAGE_DELETED:
            SendMessageEvent(TYPE_DELETE, addr, event);
            break;
        case stub::EventType::MESSAGE_REMOVED:
            SendMessageRemovedEvent(addr, event);
            break;
        case stub::EventType::MESSAGE_SHIFT:
            SendMessageShiftEvent(addr, event);
            break;
        case stub::EventType::DELIVERY_SUCCESS:
            SendMessageEvent(TYPE_DELIVERY_SUCCESS, addr, event);
            break;
        case stub::EventType::SENDING_SUCCESS:
            SendMessageEvent(TYPE_SENDING_SUCCESS, addr, event);
            break;
        case stub::EventType::SENDING_FAILURE:
            SendingFailureEvent(addr, event);
            break;
        case stub::EventType::DELIVERY_FAILURE:
            SendMessageEvent(TYPE_DELIVERY_FAILURE, addr, event);
            break;
        case stub::EventType::READ_STATUS_CHANGED:
            SendReadStatusChangedEvent(addr, event);
            break;
        case stub::EventType::CONVERSATION_CHANGED:
            SendConversationChangedEvent(addr, event);
            break;
        case stub::EventType::PARTICIPANT_PRESENCE_CHANGED:
            SendParticipantPresenceChangedEvent(addr, event);
            break;
        case stub::EventType::PARTICIPANT_CHAT_STATE_CHANGED:
            SendParticipantChatStateChangedEvent(addr, event);
            break;
        case stub::EventType::MESSAGE_EXTENDED_DATA_CHANGED:
            SendMessageExtendedDataChangedEvent(addr, event);
            break;
        case stub::EventType::MEMORY_FULL:
            SendMemoryFullEvent(addr, event);
            break;
        case stub::EventType::MEMORY_AVAILABLE:
            SendMemoryAvailableEvent(addr, event);
            break;
        default:
            break;
    }
}

uint32_t MapMseResource::GetRemoteFeatureMask(const std::string &addr)
{
    MSE_LOG_INFO("remoteFeatures_: %{public}d ", remoteFeatures_);

    return remoteFeatures_ & mnsClient_->GetRemoteFeatures(addr);
}

void MapMseResource::SendNewMessageEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ == MAP_EVENT_REPORT_V10) {
        MapMseEvent report(TYPE_NEW, event.handle, event.folder, MessageType(event.msgType));
        body = report.ToReportXml();
    } else if (reportVersion_ == MAP_EVENT_REPORT_V11) {
        MseEventType11 eventTmp;
        eventTmp.eventType_ = TYPE_NEW;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = event.datetime;
        eventTmp.subject_ = event.subject;
        eventTmp.senderName_ = event.senderName;
        eventTmp.priority_ = event.priority;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    } else {
        MseEventType122 eventTmp;
        eventTmp.eventType_ = TYPE_NEW;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = event.datetime;
        eventTmp.subject_ = event.subject;
        eventTmp.senderName_ = event.senderName;
        eventTmp.priority_ = event.priority;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        eventTmp.participantUci_ = event.participantUci;
        eventTmp.contactUid_ = event.contactUid;
        MapMseEvent report(eventTmp);
        report.SetReadStatus(event.readStatus);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_NEW);
}

void MapMseResource::SendMessageEvent(const std::string type, const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ == MAP_EVENT_REPORT_V10 || reportVersion_ == MAP_EVENT_REPORT_V11) {
        MapMseEvent report(type, event.handle, event.folder, MessageType(event.msgType));
        body = report.ToReportXml();
    } else {
        MseEventType121 eventTmp;
        eventTmp.eventType_ = type;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = "";
        eventTmp.subject_ = "";
        eventTmp.senderName_ = "";
        eventTmp.priority_ = -1;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, type);
}

void MapMseResource::SendMessageShiftEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ == MAP_EVENT_REPORT_V10 || reportVersion_ == MAP_EVENT_REPORT_V11) {
        MapMseEvent report(TYPE_SHIFT, event.handle, event.folder, event.oldFolder, MessageType(event.msgType));
        body = report.ToReportXml();
    } else {
        MseEventType121 eventTmp;
        eventTmp.eventType_ = TYPE_SHIFT;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = "";
        eventTmp.subject_ = "";
        eventTmp.senderName_ = "";
        eventTmp.priority_ = -1;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_SHIFT);
}

void MapMseResource::SendMessageRemovedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ != MAP_EVENT_REPORT_V12) {
        MSE_LOG_ERROR("MessageRemoved event is not exist.");
        return;
    } else {
        MseEventType121 eventTmp;
        eventTmp.eventType_ = TYPE_REMOVED;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = "";
        eventTmp.subject_ = "";
        eventTmp.senderName_ = "";
        eventTmp.priority_ = -1;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_REMOVED);
}

void MapMseResource::SendingFailureEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ == MAP_EVENT_REPORT_V10 || reportVersion_ == MAP_EVENT_REPORT_V11) {
        MapMseEvent report(TYPE_SENDING_FAILURE, event.handle, event.folder, MessageType(event.msgType));
        body = report.ToReportXml();
    } else {
        MseEventType121 eventTmp;
        eventTmp.eventType_ = TYPE_SENDING_FAILURE;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = "";
        eventTmp.subject_ = "";
        eventTmp.senderName_ = "";
        eventTmp.priority_ = -1;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = "";
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_SENDING_FAILURE);
}

void MapMseResource::SendReadStatusChangedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ == MAP_EVENT_REPORT_V10) {
        MSE_LOG_ERROR("MessageRemoved event is not exist.");
        return;
    } else if (reportVersion_ == MAP_EVENT_REPORT_V11) {
        MapMseEvent report(TYPE_READ_STATUS, event.handle, event.folder, MessageType(event.msgType));
        body = report.ToReportXml();
    } else {
        MseEventType121 eventTmp;
        eventTmp.eventType_ = TYPE_READ_STATUS;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = "";
        eventTmp.subject_ = "";
        eventTmp.senderName_ = "";
        eventTmp.priority_ = -1;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        MapMseEvent report(eventTmp);
        report.SetReadStatus(event.readStatus);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_READ_STATUS);
}

void MapMseResource::SendConversationChangedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ != MAP_EVENT_REPORT_V12) {
        MSE_LOG_ERROR("ConversationChanged event is not exist.");
        return;
    } else {
        MseEventType124 eventTmp;
        eventTmp.eventType_ = TYPE_CONVERSATION;
        eventTmp.senderName_ = "";
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        eventTmp.presenceState_ = event.presenceAvailability;
        eventTmp.presenceStatus_ = event.presenceText;
        eventTmp.lastActivity_ = event.lastActivity;
        eventTmp.chatState_ = event.chatState;
        eventTmp.extendedData_ = event.extendedData;
        eventTmp.participantUci_ = event.participantUci;
        eventTmp.contactUid_ = event.contactUid;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_CONVERSATION);
}

void MapMseResource::SendParticipantPresenceChangedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ != MAP_EVENT_REPORT_V12) {
        MSE_LOG_ERROR("ConversationChanged event is not exist.");
        return;
    } else {
        MseEventType124 eventTmp;
        eventTmp.eventType_ = TYPE_PRESENCE;
        eventTmp.senderName_ = event.senderName;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        eventTmp.presenceState_ = event.presenceAvailability;
        eventTmp.presenceStatus_ = event.presenceText;
        eventTmp.lastActivity_ = event.lastActivity;
        eventTmp.chatState_ = -1;
        eventTmp.extendedData_ = "";
        eventTmp.participantUci_ = event.participantUci;
        eventTmp.contactUid_ = event.contactUid;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_PRESENCE);
}

void MapMseResource::SendParticipantChatStateChangedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ != MAP_EVENT_REPORT_V12) {
        MSE_LOG_ERROR("ConversationChanged event is not exist.");
        return;
    } else {
        MseEventType124 eventTmp;
        eventTmp.eventType_ = TYPE_CHAT_STATE;
        eventTmp.senderName_ = event.senderName;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        eventTmp.presenceState_ = -1;
        eventTmp.presenceStatus_ = "";
        eventTmp.lastActivity_ = event.lastActivity;
        eventTmp.chatState_ = event.chatState;
        eventTmp.extendedData_ = "";
        eventTmp.participantUci_ = event.participantUci;
        eventTmp.contactUid_ = event.contactUid;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_CHAT_STATE);
}

void MapMseResource::SendMessageExtendedDataChangedEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string body;
    if (reportVersion_ != MAP_EVENT_REPORT_V12) {
        MSE_LOG_ERROR("ConversationChanged event is not exist.");
        return;
    } else {
        MseEventType123 eventTmp;
        eventTmp.eventType_ = TYPE_EXTENDED_DATA;
        eventTmp.handle_ = event.handle;
        eventTmp.folder_ = event.folder;
        eventTmp.msgType_ = MessageType(event.msgType);
        eventTmp.datetime_ = event.senderName;
        eventTmp.senderName_ = event.senderName;
        eventTmp.conversationID_ = event.conversationID;
        eventTmp.conversationName_ = event.conversationName;
        eventTmp.extendedData_ = event.extendedData;
        eventTmp.participantUci_ = event.participantUci;
        eventTmp.contactUid_ = event.contactUid;
        MapMseEvent report(eventTmp);
        body = report.ToReportXml();
    }
    SendEventReport(addr, body, TYPE_EXTENDED_DATA);
}

void MapMseResource::SendMemoryFullEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    MapMseEvent report(TYPE_MEMORY_FULL, "", "", MessageType::NONE);
    std::string body = report.ToReportXml();
    SendEventReport(addr, body, TYPE_MEMORY_FULL);
}

void MapMseResource::SendMemoryAvailableEvent(const std::string &addr, const stub::EventInfo &event)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    MapMseEvent report(TYPE_MEMORY_AVAILABLE, "", "", MessageType::NONE);
    std::string body = report.ToReportXml();
    SendEventReport(addr, body, TYPE_MEMORY_AVAILABLE);
}

bool MapMseResource::SetMessageStatusDeleted(
    std::string handle, MessageType type, std::string path, uint8_t status)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return false;
    }
    std::string sql = "update MessageList set folder = ? where handle = ? and folder = ?";
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return false;
    }
    stmt->SetParamString(0x1, MAP_MSE_FOLDER_NAME_DELETED);
    stmt->SetParamString(0x2, handle);
    std::string folderName = MAP_MSE_FOLDER_NAME_INBOX;
    if (type == MessageType::EMAIL) {
        folderName = path;
    }
    stmt->SetParamString(0x3, folderName);
    auto ret = stmt->Update();
    if (ret != 1) {
        return false;
    }
    return true;
}

bool MapMseResource::SetMessageStatusRead(
    std::string handle, MessageType type, std::string path, uint8_t status)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return false;
    }
    std::string sql = "update MessageList set read = ? where handle = ? and folder = ?";
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return false;
    }
    stmt->SetParamInt(0x1, status);
    stmt->SetParamString(0x2, handle);
    std::string folderName = MAP_MSE_FOLDER_NAME_INBOX;
    if (type == MessageType::EMAIL) {
        folderName = path;
    }
    stmt->SetParamString(0x3, folderName);
    auto ret = stmt->Update();
    if (ret != 1) {
        return false;
    }
    return true;
}

bool MapMseResource::SetExtendedData(std::string handle, std::string data)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return false;
    }
    std::string sql = "update Message set extended_data = ? where handle = ?";
    auto stmt = dataProvider->CreateStatement(sql);
    if (!stmt) {
        return false;
    }
    stmt->SetParamString(0x1, data);
    stmt->SetParamString(0x2, handle);
    auto ret = stmt->Update();
    if (ret != 1) {
        return false;
    }
    return true;
}

void MapMseResource::SentOrForwardedMessage(
    MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    UpdateData(bmessage, appParameter, handle);
    NotifySendMessage(bmessage, appParameter);
}

std::string MapMseResource::UpdateData(
    MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return "";
    }
    try {
        if (!dataProvider->BeginTransaction()) {
            return "";
        }
        if (UpdateMessage(dataProvider, bmessage, handle) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        if (UpdateMessageList(dataProvider, handle) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        dataProvider->Commit();
    } catch (const std::exception &e) {
        dataProvider->Rollback();
    }
    return handle;
}

int MapMseResource::UpdateMessage(
    std::unique_ptr<DataAccess> &dataAccess, const MapMseBmsg &bmessage, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "update Message set body = ? where handle = ? ";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    stmt->SetParamString(0x1, bmessage.GetMsgBody());
    stmt->SetParamString(0x2, handle);
    auto ret = stmt->Update();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

int MapMseResource::UpdateMessageList(std::unique_ptr<DataAccess> &dataAccess, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "update MessageList set datetime = ?, folder = ? where handle = ? ";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    stmt->SetParamString(0x1, GetDateTime());
    stmt->SetParamString(0x2, "outbox");
    stmt->SetParamString(0x3, handle);
    auto ret = stmt->Insert();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

std::string MapMseResource::SentConversationMessage(
    MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto handle = InsertConvData(bmessage, appParameter, folderName);
    if (handle == "") {
        return "";
    }
    NotifySendMessage(bmessage, appParameter);
    return handle;
}

std::string MapMseResource::InsertConvData(
    MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName)
{
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return "";
    }
    uint64_t tempHandle = GetMessageHandle(bmessage.GetType());
    std::stringstream ss;
    int index = 0x10;
    ss << std::uppercase << std::hex << std::setw(index) << std::setfill('0') << tempHandle;
    std::string handle = ss.str();
    try {
        if (!dataProvider->BeginTransaction()) {
            dataProvider->Rollback();
            return "";
        }
        if (InsertMessage(dataProvider, appParameter, bmessage, handle) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        if (InsertMessageList(dataProvider, appParameter, bmessage, handle, folderName) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        for (auto &iter : bmessage.GetOriginators()) {
            if (InsertVCard(dataProvider, iter, true, handle) != RET_NO_ERROR) {
                dataProvider->Rollback();
                return "";
            }
        }
        for (auto &iter : bmessage.GetRecipients()) {
            if (InsertVCard(dataProvider, iter, false, handle) != RET_NO_ERROR) {
                dataProvider->Rollback();
                return "";
            }
        }
        if (UpdateConvMessage(dataProvider, appParameter.conversationId_, handle) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        if (UpdateConvMessageList(dataProvider, appParameter.conversationId_, handle) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        dataProvider->Commit();
    } catch (const std::exception &e) {
        dataProvider->Rollback();
    }
    return handle;
}

int MapMseResource::UpdateConvMessage(
    std::unique_ptr<DataAccess> &dataAccess, const std::string &conversationId, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "update Message set conversation_id = ? where handle = ? ";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    stmt->SetParamString(0x1, conversationId);
    stmt->SetParamString(0x2, handle);
    auto ret = stmt->Update();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

int MapMseResource::UpdateConvMessageList(
    std::unique_ptr<DataAccess> &dataAccess, const std::string &conversationId, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "update MessageList set conversation_id = ? where handle = ? ";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    stmt->SetParamString(0x1, conversationId);
    stmt->SetParamString(0x2, handle);
    auto ret = stmt->Insert();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

void MapMseResource::NotifySendMessage(const MapMseBmsg &bmessage, const MapMseParams &appParameter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::vector<MapMseVcard> recipients = bmessage.GetRecipients();
    std::string body = bmessage.GetMsgBody();
    for (auto &recipient : recipients) {
        if (recipient.GetEnvLevel() == 0) {
            stub::MsgInfo msgInfo;
            msgInfo.phoneNumber = recipient.GetFirstPhoneNumber();
            msgInfo.emailAddress = recipient.GetFirstEmail();
            if (appParameter.retry_) {
                msgInfo.retry = *appParameter.retry_;
            }
            if (appParameter.transparent_) {
                msgInfo.transparent = *appParameter.transparent_;
            }
            msgInfo.msgType = static_cast<int>(bmessage.GetType());

            stub::MapService::GetInstance()->SendMessage(msgInfo, body, masId_);
        }
    }
}

std::string MapMseResource::SentNewMessage(
    MapMseBmsg &bmessage, std::string &folderName, MapMseParams &appParameter)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto handle = DataInsert(bmessage, appParameter, folderName);
    if (handle == "") {
        return "";
    }
    NotifySendMessage(bmessage, appParameter);
    return handle;
}

std::string MapMseResource::DataInsert(
    MapMseBmsg &bmessage, MapMseParams &appParameter, const std::string &folderName)
{
    auto dataProvider = DataAccess::GetConnection(DEFAULT_MAP_MSE_DB_FILE, false);
    if (!dataProvider) {
        return "";
    }
    uint64_t tempHandle = GetMessageHandle(bmessage.GetType());
    std::stringstream ss;
    int index = 0x10;
    ss << std::uppercase << std::hex << std::setw(index) << std::setfill('0') << tempHandle;
    std::string handle = ss.str();
    try {
        if (!dataProvider->BeginTransaction()) {
            return "";
        }
        if (InsertMessage(dataProvider, appParameter, bmessage, handle) != RET_NO_ERROR) {
            return "";
        }
        if (InsertMessageList(dataProvider, appParameter, bmessage, handle, folderName) != RET_NO_ERROR) {
            dataProvider->Rollback();
            return "";
        }
        for (auto &iter : bmessage.GetOriginators()) {
            if (InsertVCard(dataProvider, iter, true, handle) != RET_NO_ERROR) {
                dataProvider->Rollback();
                return "";
            }
        }
        for (auto &iter : bmessage.GetRecipients()) {
            if (InsertVCard(dataProvider, iter, false, handle) != RET_NO_ERROR) {
                dataProvider->Rollback();
                return "";
            }
        }
        dataProvider->Commit();
    } catch (const std::exception &e) {
        dataProvider->Rollback();
    }
    return handle;
}

int MapMseResource::InsertMessageList(std::unique_ptr<DataAccess> &dataAccess, MapMseParams &appParameter,
    const MapMseBmsg &bmessage, const std::string &handle, const std::string &folderName)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql =
        "insert into MessageList(handle, subject, datetime, sender_name, sender_addressing, recipient_name, "
        "recipient_addressing, type, size, text, reception_status, attachment_size, priority, read, sent, protected, "
        "folder, delivery_status, direction) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    int index = 1;
    stmt->SetParamString(index++, handle);
    stmt->SetParamString(index++, "");
    stmt->SetParamString(index++, GetDateTime());
    std::string name = "";
    std::string addr = "";
    GetNameAddress(bmessage.GetType(), bmessage.GetOriginators(), name, addr);
    stmt->SetParamString(index++, name);
    stmt->SetParamString(index++, addr);
    GetNameAddress(bmessage.GetType(), bmessage.GetRecipients(), name, addr);
    stmt->SetParamString(index++, name);
    stmt->SetParamString(index++, addr);
    stmt->SetParamInt(index++, (int)bmessage.GetType());
    stmt->SetParamInt(index++, bmessage.GetMsgBody().size());
    stmt->SetParamInt(index++, 1);
    stmt->SetParamString(index++, "complete");
    stmt->SetParamInt(index++, 0);
    stmt->SetParamInt(index++, 0);
    int read = (bmessage.GetStatus() == "READ") ? 1 : 0;
    stmt->SetParamInt(index++, read);
    stmt->SetParamInt(index++, 1);
    stmt->SetParamInt(index++, 0);
    stmt->SetParamString(index++, folderName);
    stmt->SetParamString(index++, "sent");
    stmt->SetParamString(index++, "outgoing");
    auto ret = stmt->Insert();
    if (ret != 1)
        return RET_BAD_STATUS;
    return RET_NO_ERROR;
}

void MapMseResource::GetNameAddress(
    MessageType type, std::vector<MapMseVcard> vcard, std::string &name, std::string &sender)
{
    name = "";
    sender = "";
    if (vcard.size() > 0) {
        name = vcard.front().GetName();
        if (type == MessageType::EMAIL) {
            sender = vcard.front().GetFirstEmail();
        } else {
            sender = vcard.front().GetFirstPhoneNumber();
        }
    }
}

int MapMseResource::InsertMessage(std::unique_ptr<DataAccess> &dataAccess, MapMseParams &appParameter,
    const MapMseBmsg &bmessage, const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "insert into Message(transparent, retry, char_set, handle, body, version, status, type, "
                      "folder_path, extended_data, part_id, encoding, conent_charset, language, content_length"
                      ") values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    int transparent = 0;
    if (appParameter.transparent_) {
        transparent = *appParameter.transparent_;
    }
    int retry = 1;
    if (appParameter.retry_) {
        retry = *appParameter.retry_;
    }
    int index = 1;
    stmt->SetParamInt(index++, transparent);
    stmt->SetParamInt(index++, retry);
    stmt->SetParamInt(index++, (int)*appParameter.charSet_);
    stmt->SetParamString(index++, handle);
    stmt->SetParamString(index++, bmessage.GetMsgBody());
    stmt->SetParamString(index++, bmessage.GetVersion());
    stmt->SetParamString(index++, bmessage.GetStatus());
    stmt->SetParamString(index++, GetMessageNameByType(bmessage.GetType()));
    stmt->SetParamString(index++, bmessage.GetFolder());
    stmt->SetParamString(index++, bmessage.GetExtendedData());
    stmt->SetParamString(index++, bmessage.GetPartId());
    stmt->SetParamString(index++, bmessage.GetEncoding());
    stmt->SetParamString(index++, bmessage.GetCharset());
    stmt->SetParamString(index++, bmessage.GetLanguage());
    stmt->SetParamString(index++, bmessage.GetBmsgLength());
    auto ret = stmt->Insert();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

int MapMseResource::InsertVCard(
    std::unique_ptr<DataAccess> &dataAccess, const MapMseVcard &vcard, const int isOriginator,
    const std::string &handle)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string sql = "insert into Vcard(name, version, formatted_name, phone_numbers, email_addresses, x_bt_uid, "
                      "x_bt_ucis, env_level, is_originator, handle) values (?,?,?,?,?,?,?,?,?,?)";
    auto stmt = dataAccess->CreateStatement(sql);
    if (!stmt) {
        return RET_BAD_STATUS;
    }
    int index = 1;
    stmt->SetParamString(index++, vcard.GetName());
    stmt->SetParamString(index++, vcard.GetVersion());
    stmt->SetParamString(index++, vcard.GetFormattedName());
    stmt->SetParamString(index++, vcard.GetPhoneNumbersText());
    stmt->SetParamString(index++, vcard.GetEmailAddressesText());
    stmt->SetParamString(index++, vcard.GetBtUidsText());
    stmt->SetParamString(index++, vcard.GetBtUcisText());
    stmt->SetParamInt(index++, vcard.GetEnvLevel());
    stmt->SetParamInt(index++, isOriginator);
    stmt->SetParamString(index++, handle);
    auto ret = stmt->Insert();
    if (ret != 1) {
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

std::string MapMseResource::GetDateTime()
{
    time_t currTime;
    time(&currTime);
    struct tm utctm;
    gmtime_r(&currTime, &utctm);
    char buff[16] = "";
    strftime(buff, sizeof(buff), "%Y%m%{public}d%H%M%{public}s%Z", &utctm);
    return std::string(buff);
}

uint64_t MapMseResource::GetMessageHandle(MessageType type)
{
    uint64_t handle = std::strtoul(GetDateTime().c_str(), nullptr, 0x10);
    if (type == MessageType::EMAIL) {
        return handle | MESSAGE_HANDLE_EMAIL_MASK;
    } else if (type == MessageType::IM) {
        return handle | MESSAGE_HANDLE_IM_MASK;
    } else if (type == MessageType::MMS) {
        return handle | MESSAGE_HANDLE_MMS_MASK;
    } else if (type == MessageType::SMS_GSM) {
        return handle | MESSAGE_HANDLE_SMS_GSM_MASK;
    } else if (type == MessageType::SMS_CDMA) {
        return handle | MESSAGE_HANDLE_SMS_CDMA_MASK;
    } else {
        MSE_LOG_INFO("Message type is not supported.");
        return 0x0;
    }
}

std::string MapMseResource::GetMessageNameByType(MessageType type)
{
    if (type == MessageType::EMAIL) {
        return "EMAIL";
    } else if (type == MessageType::IM) {
        return "IM";
    } else if (type == MessageType::MMS) {
        return "MMS";
    } else if (type == MessageType::SMS_GSM) {
        return "SMS_GSM";
    } else if (type == MessageType::SMS_CDMA) {
        return "SMS_CDMA";
    } else {
        return "NONE";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
