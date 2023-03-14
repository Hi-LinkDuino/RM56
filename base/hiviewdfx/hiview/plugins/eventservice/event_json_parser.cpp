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

#include "event_json_parser.h"

#include <algorithm>
#include <fstream>

#include "cJSON.h"
#include "logger.h"
#include "sys_event_query.h"

namespace OHOS {
namespace HiviewDFX {
static const char* BASE_EVENT_PAR[] = {"domain_", "name_", "type_", "time_", "level_",
    "uid_", "tag_", "tz_", "pid_", "tid_", "traceid_", "spanid_", "pspanid_", "trace_flag_"};
static const char* EVENT_JSON_TYPE[] = {"FAULT", "STATISTIC", "SECURITY", "BEHAVIOR"};

DEFINE_LOG_TAG("Event-JsonParser");

EventJsonParser::EventJsonParser(const std::string &path) : jsonRootValid(false)
{
    std::ifstream fin(path, std::ifstream::binary);
#ifdef JSONCPP_VERSION_STRING
    Json::CharReaderBuilder jsonRBuilder;
    Json::CharReaderBuilder::strictMode(&jsonRBuilder.settings_);
    JSONCPP_STRING errs;
    if (!parseFromStream(jsonRBuilder, fin, &root_, &errs)) {
#else
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(fin, root_)) {
#endif
        HIVIEW_LOGE("parse json file failed, please check the style of json file: %{public}s", path.c_str());
    } else {
        jsonRootValid = true;
    }
}

EventJsonParser::~EventJsonParser() {}

bool EventJsonParser::HandleEventJson(std::shared_ptr<SysEvent> &event) const
{
    Json::Value eventJson;
    std::string jsonStr = event->jsonExtraInfo_;
#ifdef JSONCPP_VERSION_STRING
    Json::CharReaderBuilder jsonRBuilder;
    Json::CharReaderBuilder::strictMode(&jsonRBuilder.settings_);
    std::unique_ptr<Json::CharReader> const reader(jsonRBuilder.newCharReader());
    JSONCPP_STRING errs;
    if (!reader->parse(jsonStr.data(), jsonStr.data() + jsonStr.size(), &eventJson, &errs)) {
#else
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(jsonStr, eventJson)) {
#endif
        HIVIEW_LOGE("parse json file failed, please check the style of json file: %{public}s",
            jsonStr.c_str());
        return false;
    }

    auto eventJsonNotComplete = !eventJson.isMember("domain_") || !eventJson["domain_"].isString() ||
        !eventJson.isMember("name_") || !eventJson["name_"].isString() ||
        !eventJson.isMember("type_") || !eventJson["type_"].isInt();
    std::string domain = eventJson["domain_"].asString();
    std::string name = eventJson["name_"].asString();
    auto eventJsonHasNoDomain = !jsonRootValid || !root_.isObject() || !root_.isMember(domain) ||
        !root_[domain].isObject() || !root_[domain].isMember(name) || !root_[domain][name].isObject() ||
        !root_[domain][name].isMember("__BASE") || root_[domain][name]["__BASE"].isNull();
    auto sysEventJson = root_[domain][name];
    auto baseInfoNotValid = !CheckBaseInfo(sysEventJson["__BASE"], eventJson);
    if (eventJsonNotComplete || eventJsonHasNoDomain || baseInfoNotValid) {
        return false;
    }

    auto eventNameList = eventJson.getMemberNames();
    for (auto it = eventNameList.cbegin(); it != eventNameList.cend(); it++) {
        std::string ps = *it;
        if (std::find_if(std::cbegin(BASE_EVENT_PAR), std::cend(BASE_EVENT_PAR), [ps](const char* ele) {
            return (ps.compare(ele) == 0); }) == std::cend(BASE_EVENT_PAR)) {
            if (!CheckExtendInfo(ps, sysEventJson, eventJson)) {
                HIVIEW_LOGI("CheckExtendInfo fail, need to remove %{public}s ", ps.c_str());
                eventJson.removeMember(ps);
            }
        }
    }
    GetOrderlyJsonInfo(eventJson, jsonStr);
    event->jsonExtraInfo_ = jsonStr;
    return true;
}

bool EventJsonParser::CheckBaseInfo(const Json::Value &sysBaseJson, Json::Value &eventJson) const
{
    int eventJsonTypeValue = eventJson["type_"].asInt();
    if ((eventJsonTypeValue < 1) || (eventJsonTypeValue > 4)) {
        HIVIEW_LOGE("EventJson type is illegal parameter");
        return false;
    }
    if (sysBaseJson["type"].asString().compare(EVENT_JSON_TYPE[eventJsonTypeValue - 1]) != 0) {
        HIVIEW_LOGE("Check type error type %{public}s %{public}d",
            sysBaseJson["type"].asString().c_str(), eventJson["type_"].asInt());
        return false;
    }

    if (!sysBaseJson.isMember("level")) {
        HIVIEW_LOGE("sysBaseJson is not complete");
        return false;
    }
    eventJson["level_"] = sysBaseJson["level"].asString();
    if (sysBaseJson.isMember("tag")) {
        eventJson["tag_"] = sysBaseJson["tag"].asString();
    }
    return true;
}

bool EventJsonParser::CheckExtendInfo(const std::string &name, const Json::Value &sysEvent,
    const Json::Value &eventJson) const
{
    if (!sysEvent.isMember(name)) {
        return false;
    }
    if (sysEvent[name].isMember("arrsize")) {
        if (!eventJson[name].isArray() || eventJson[name].size() > sysEvent[name]["arrsize"].asUInt()) {
            return false;
        }
        return JudgeDataType(sysEvent[name]["type"].asString(), eventJson[name][0]);
    }
    return JudgeDataType(sysEvent[name]["type"].asString(), eventJson[name]);
}

bool EventJsonParser::JudgeDataType(const std::string &dataType, const Json::Value &eventJson) const
{
    if (dataType.compare("BOOL") == 0) {
        return eventJson.isBool();
    } else if ((dataType.compare("INT8") == 0) || (dataType.compare("INT16") == 0) ||
        (dataType.compare("INT32") == 0) || (dataType.compare("INT64") == 0)) {
        return eventJson.isInt();
    } else if ((dataType.compare("UINT8") == 0) || (dataType.compare("UINT16") == 0) ||
        (dataType.compare("UINT32") == 0) || (dataType.compare("UINT64") == 0)) {
        return eventJson.isUInt();
    } else if ((dataType.compare("FLOAT") == 0) || (dataType.compare("DOUBLE") == 0)) {
        return eventJson.isDouble();
    } else if (dataType.compare("STRING") == 0) {
        return eventJson.isString();
    } else {
        return false;
    }
}

void EventJsonParser::GetOrderlyJsonInfo(const Json::Value &eventJson, std::string &jsonStr) const
{
    // convert JsonValue to the correct order by event->jsonExtraInfo_
    cJSON *cJsonArr = cJSON_Parse(jsonStr.c_str());
    if (cJsonArr == NULL) {
        return;
    }
    int endJson = cJSON_GetArraySize(cJsonArr) - 1;
    cJSON *item = NULL;

    // cJsonArr need to delete the item that failed the check by hisysevent.def
    for (int i = endJson; i >= 0; i--) {
        item = cJSON_GetArrayItem(cJsonArr, i);
        if (!eventJson.isMember(item->string)) {
            cJSON_DeleteItemFromArray(cJsonArr, i);
        }
    }

    // cJsonArr need to add "level_" and "tag_" by hisysevent.def, "level" is must-option
    cJSON_AddStringToObject(cJsonArr, "level_", eventJson["level_"].asString().c_str());
    if (eventJson.isMember("tag_")) {
        cJSON_AddStringToObject(cJsonArr, "tag_", eventJson["tag_"].asString().c_str());
    }

    // FreezeDetector needs to add
    cJSON_AddStringToObject(cJsonArr, EventStore::EventCol::INFO.c_str(), "");
    jsonStr = cJSON_PrintUnformatted(cJsonArr);
    cJSON_Delete(cJsonArr);
    return;
}

std::string EventJsonParser::GetDefinedTagByDomainEventName(const std::string& domain,
    const std::string& eventName) const
{
    if (!jsonRootValid) {
        return "";
    }
    auto eventJsonHasNoDomain = !root_.isObject() || !root_.isMember(domain) || !root_[domain].isObject()
        || !root_[domain].isMember(eventName) || !root_[domain][eventName].isObject()
        || !root_[domain][eventName].isMember("__BASE") || root_[domain][eventName]["__BASE"].isNull();
    if (eventJsonHasNoDomain) {
        return "";
    }
    auto sysEventBaseInfo = root_[domain][eventName]["__BASE"];
    if (!sysEventBaseInfo.isMember("tag")) {
        return "";
    }
    return sysEventBaseInfo["tag"].asString();
}
} // namespace HiviewDFX
} // namespace OHOS
