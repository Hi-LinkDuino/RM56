/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HIVIEW_PLUGINS_EVENT_SERVICE_INCLUDE_EVENT_JSON_PARSER_H
#define HIVIEW_PLUGINS_EVENT_SERVICE_INCLUDE_EVENT_JSON_PARSER_H

#include <cstdint>
#include <string>

#include "json/json.h"
#include "sys_event.h"

namespace OHOS {
namespace HiviewDFX {
class EventJsonParser {
public:
    EventJsonParser(const std::string &path);
    ~EventJsonParser();

    bool HandleEventJson(std::shared_ptr<SysEvent> &event) const;
    bool CheckBaseInfo(const Json::Value &baseJson, Json::Value &eventJson) const;
    bool CheckExtendInfo(const std::string &name, const Json::Value &sysEvent, const Json::Value &eventJson) const;
    bool JudgeDataType(const std::string &dataType, const Json::Value &eventJson) const;
    void GetOrderlyJsonInfo(const Json::Value &eventJson, std::string &jsonStr) const;
    std::string GetDefinedTagByDomainEventName(const std::string &domain, const std::string &eventName) const;
private:
    Json::Value root_;
    bool jsonRootValid;
}; // SysEventDbMgr
} // namespace HiviewDFX
} // namespace OHOS
#endif
