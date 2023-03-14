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

#ifndef MAP_MSE_EVENT_H
#define MAP_MSE_EVENT_H

#include <string>
#include "bt_def.h"
#include "libxml/parser.h"
#include "map_mse_types.h"

namespace OHOS {
namespace bluetooth {
struct MseEventType11 {
    std::string eventType_ = "";
    std::string handle_ = "";
    std::string folder_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string datetime_ = "";
    std::string subject_ = "";
    std::string senderName_ = "";
    int priority_ = -1;
};

struct MseEventType121 {
    std::string eventType_ = "";
    std::string handle_ = "";
    std::string folder_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string datetime_ = "";
    std::string subject_ = "";
    std::string senderName_ = "";
    int priority_ = -1;
    std::string conversationID_ = "";
    std::string conversationName_ = "";
};

struct MseEventType122 {
    std::string eventType_ = "";
    std::string handle_ = "";
    std::string folder_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string datetime_ = "";
    std::string subject_ = "";
    std::string senderName_ = "";
    int priority_ = -1;
    std::string conversationID_ = "";
    std::string conversationName_ = "";
    std::string participantUci_ = "";
    std::string contactUid_ = "";
};

struct MseEventType123 {
    std::string eventType_ = "";
    std::string handle_ = "";
    std::string folder_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string datetime_ = "";
    std::string senderName_ = "";
    std::string conversationID_ = "";
    std::string conversationName_ = "";
    std::string extendedData_ = "";
    std::string participantUci_ = "";
    std::string contactUid_ = "";
};

struct MseEventType124 {
    std::string eventType_ = "";
    std::string senderName_ = "";
    std::string conversationID_ = "";
    std::string conversationName_ = "";
    int presenceState_ = -1;
    std::string presenceStatus_ = "";
    std::string lastActivity_ = "";
    int chatState_ = -1;
    std::string extendedData_ = "";
    std::string participantUci_ = "";
    std::string contactUid_ = "";
};


class MapMseEvent {
public:
    explicit MapMseEvent();

    virtual ~MapMseEvent();

    MapMseEvent(const std::string &eventType, const std::string &handle, const std::string &folder,
        const std::string &oldFolder, MessageType msgType);

    MapMseEvent(
        const std::string &eventType, const std::string &handle, const std::string &folder, MessageType msgType);

    /* extended event type 1.1 */
    explicit MapMseEvent(const MseEventType11 &event);

    /* extended event type 1.2 message events */
    explicit MapMseEvent(const MseEventType121 &event);

    explicit MapMseEvent(const MseEventType122 &event);

    explicit MapMseEvent(const MseEventType123 &event);

    explicit MapMseEvent(const MseEventType124 &event);

    std::string ToReportXml(void) const;

    void SetReadStatus(const int status);

private:
    static const std::string PATH;
    void SetFolderPath(const std::string &name, const MessageType &type);
    std::string GetMessageNameByType(const MessageType type) const;
    void Ver12ToXml(xmlNodePtr &node) const;
    std::string version_ = "";
    std::string eventType_ = "";
    std::string handle_ = "";
    std::string folder_ = "";
    std::string oldFolder_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string name_ = "";
    /* Extended event parameters in MAP Event version 1.1 */
    std::string datetime_ = "";
    std::string uci_ = "";
    std::string subject_ = "";
    std::string senderName_ = "";
    int priority_ = -1;
    /* Event parameters in MAP Event version 1.2 */
    std::string conversationID_ = "";
    std::string conversationName_ = "";
    int presenceAvailability_ = -1;
    std::string presenceText_ = "";
    std::string lastActivity_ = "";
    int chatState_ = -1;
    std::string readStatus_ = "";
    std::string extendedData_ = "";
    std::string participantUci_ = "";
    std::string contactUid_ = "";
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_EVENT_H
