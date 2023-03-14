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

#include "map_mse_event.h"
#include <sstream>

namespace OHOS {
namespace bluetooth {
const std::string MapMseEvent::PATH = "telecom/msg/";

MapMseEvent::MapMseEvent()
{}

MapMseEvent::MapMseEvent(const std::string &eventType, const std::string &handle, const std::string &folder,
    const std::string &oldFolder, MessageType msgType)
    : version_("1.0"), eventType_(eventType), handle_(handle), folder_(folder), oldFolder_(oldFolder), msgType_(msgType)
{
    SetFolderPath(folder, msgType);
    if ((msgType != MessageType::EMAIL) && (msgType != MessageType::IM) && (oldFolder != "")) {
        oldFolder_ = PATH + oldFolder;
    }
}

MapMseEvent::MapMseEvent(
    const std::string &eventType, const std::string &handle, const std::string &folder, MessageType msgType)
    : version_("1.0"), eventType_(eventType), handle_(handle), folder_(folder), msgType_(msgType)
{
    SetFolderPath(folder, msgType);
}

// /* extended event type 1.1 */
MapMseEvent::MapMseEvent(const MseEventType11 &event)
    : version_("1.1"),
      eventType_(event.eventType_),
      handle_(event.handle_),
      folder_(event.folder_),
      msgType_(event.msgType_),
      datetime_(event.datetime_),
      subject_(event.subject_),
      senderName_(event.senderName_),
      priority_(event.priority_)
{
    SetFolderPath(event.folder_, event.msgType_);
}

/* extended event type 1.2 message events */
MapMseEvent::MapMseEvent(const MseEventType121 &event)
    : version_("1.2"),
      eventType_(event.eventType_),
      handle_(event.handle_),
      folder_(event.folder_),
      msgType_(event.msgType_),
      datetime_(event.datetime_),
      subject_(event.subject_),
      senderName_(event.senderName_),
      priority_(event.priority_),
      conversationID_(event.conversationID_),
      conversationName_(event.conversationName_)
{
    SetFolderPath(event.folder_, event.msgType_);
}

MapMseEvent::MapMseEvent(const MseEventType122 &event)
    : version_("1.2"),
      eventType_(event.eventType_),
      handle_(event.handle_),
      folder_(event.folder_),
      msgType_(event.msgType_),
      datetime_(event.datetime_),
      subject_(event.subject_),
      senderName_(event.senderName_),
      priority_(event.priority_),
      conversationID_(event.conversationID_),
      conversationName_(event.conversationName_),
      participantUci_(event.participantUci_),
      contactUid_(event.contactUid_)
{
    SetFolderPath(event.folder_, event.msgType_);
}

MapMseEvent::MapMseEvent(const MseEventType123 &event)
    : version_("1.2"),
      eventType_(event.eventType_),
      handle_(event.handle_),
      folder_(event.folder_),
      msgType_(event.msgType_),
      datetime_(event.datetime_),
      senderName_(event.senderName_),
      conversationID_(event.conversationID_),
      conversationName_(event.conversationName_),
      extendedData_(event.extendedData_),
      participantUci_(event.participantUci_),
      contactUid_(event.contactUid_)
{}

MapMseEvent::MapMseEvent(const MseEventType124 &event)
    : version_("1.2"),
      eventType_(event.eventType_),
      senderName_(event.senderName_),
      conversationID_(event.conversationID_),
      conversationName_(event.conversationName_),
      presenceAvailability_(event.presenceState_),
      presenceText_(event.presenceStatus_),
      lastActivity_(event.lastActivity_),
      chatState_(event.chatState_),
      extendedData_(event.extendedData_),
      participantUci_(event.participantUci_),
      contactUid_(event.contactUid_)
{}

void MapMseEvent::Ver12ToXml(xmlNodePtr &node) const
{
    if (MAP_V12 == version_) {
        if (conversationName_ != "")
            xmlNewProp(node, BAD_CAST"conversation_name", BAD_CAST conversationName_.c_str());
        if (conversationID_ != "")
            xmlNewProp(node, BAD_CAST"conversation_id", BAD_CAST conversationID_.c_str());
        if (presenceAvailability_ != -1)
            xmlNewProp(node, BAD_CAST"presence_availability", BAD_CAST std::to_string(presenceAvailability_).c_str());
        if (presenceText_ != "")
            xmlNewProp(node, BAD_CAST"presence_text", BAD_CAST presenceText_.c_str());
        if (lastActivity_ != "")
            xmlNewProp(node, BAD_CAST"last_activity", BAD_CAST lastActivity_.c_str());
        if (chatState_ != -1)
            xmlNewProp(node, BAD_CAST"chat_state", BAD_CAST std::to_string(chatState_).c_str());
        if (readStatus_ != "")
            xmlNewProp(node, BAD_CAST"read_status", BAD_CAST readStatus_.c_str());
        if (participantUci_ != "")
            xmlNewProp(node, BAD_CAST"participant_uci", BAD_CAST participantUci_.c_str());
        if (extendedData_ != "")
            xmlNewProp(node, BAD_CAST"extended_data", BAD_CAST extendedData_.c_str());
        if (contactUid_ != "")
            xmlNewProp(node, BAD_CAST"contact_uid", BAD_CAST contactUid_.c_str());
    }
}

std::string MapMseEvent::ToReportXml(void) const
{
    xmlChar *xmlbuff;
    int buffersize;
    std::ostringstream oss;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST MAP_V10.c_str());
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "MAP-event-report");
    xmlDocSetRootElement(doc, root_node);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST version_.c_str());

    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"event");
    xmlAddChild(root_node, node);
    xmlNewProp(node, BAD_CAST"type", BAD_CAST eventType_.c_str());
    if (handle_ != "")
        xmlNewProp(node, BAD_CAST"handle", BAD_CAST handle_.c_str());
    if (folder_ != "")
        xmlNewProp(node, BAD_CAST"folder", BAD_CAST folder_.c_str());
    if (oldFolder_ != "")
        xmlNewProp(node, BAD_CAST"old_folder", BAD_CAST oldFolder_.c_str());
    if (MessageType::NONE != msgType_)
        xmlNewProp(node, BAD_CAST"msg_type", BAD_CAST GetMessageNameByType(msgType_).c_str());
    if (MAP_V11 == version_ || MAP_V12 == version_) {
        if (datetime_ != "")
            xmlNewProp(node, BAD_CAST"datetime", BAD_CAST datetime_.c_str());
        if (subject_ != "")
            xmlNewProp(node, BAD_CAST"subject", BAD_CAST subject_.c_str());
        if (senderName_ != "")
            xmlNewProp(node, BAD_CAST"sender_name", BAD_CAST senderName_.c_str());
        if (priority_ == 0 || priority_ == 1)
            xmlNewProp(node, BAD_CAST"priority", BAD_CAST ((priority_ == 1) ? "yes" : "no"));
    }
    Ver12ToXml(node);

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    oss << xmlbuff;
    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return oss.str();
}

void MapMseEvent::SetReadStatus(const int status)
{
    if (status == 1) {
        readStatus_ = "yes";
    } else if (status == 0) {
        readStatus_ = "no";
    }
}

MapMseEvent::~MapMseEvent()
{}

void MapMseEvent::SetFolderPath(const std::string &name, const MessageType &type)
{
    if ((type != MessageType::EMAIL) && (type != MessageType::IM) && (name != "")) {
        folder_ = PATH + name;
    }
}

std::string MapMseEvent::GetMessageNameByType(const MessageType type) const
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
