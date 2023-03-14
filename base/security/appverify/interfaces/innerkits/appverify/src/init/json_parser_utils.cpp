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

#include "init/json_parser_utils.h"

#include <fstream>
#include <sstream>

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
bool JsonParserUtils::ReadTrustedRootCAFromJson(nlohmann::json& jsonObj,
    const std::string& jsonPath, std::string& error)
{
    std::ifstream jsonFileStream;
    jsonFileStream.open(jsonPath.c_str(), std::ios::in);
    if (!jsonFileStream.is_open()) {
        error += "open file failed";
        return false;
    }
    std::ostringstream buf;
    char ch;
    while (buf && jsonFileStream.get(ch)) {
        buf.put(ch);
    }
    jsonFileStream.close();

    std::string jsonStr = buf.str();
    jsonObj = nlohmann::json::parse(jsonStr, nullptr, false);
    if (!jsonObj.is_structured()) {
        error += "parse jsonStr failed";
        return false;
    }
    return true;
}

bool JsonParserUtils::GetJsonString(const nlohmann::json& json, const std::string& key, std::string& value)
{
    if (!json.is_object()) {
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_string()) {
        value = json.at(key).get<std::string>();
    }
    return true;
}

bool JsonParserUtils::GetJsonInt(const nlohmann::json& json, const std::string& key, int& value)
{
    if (!json.is_object()) {
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_number()) {
        value = json.at(key).get<int>();
    }
    return true;
}

bool JsonParserUtils::GetJsonStringVec(const nlohmann::json& json, const std::string& key, StringVec& value)
{
    if (!json.is_object()) {
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_array()) {
        value = json.at(key).get<StringVec>();
    }
    return true;
}

bool JsonParserUtils::ParseJsonToObjVec(const nlohmann::json& json, const std::string& key, JsonObjVec& jsonObjVec)
{
    if (!json.is_object()) {
        return false;
    }
    if (json.find(key) != json.end() && json.at(key).is_array()) {
        jsonObjVec = json.at(key).get<JsonObjVec>();
    }
    return true;
}

void JsonParserUtils::ParseJsonToMap(const nlohmann::json& json, JsonMap& jsonMap)
{
    for (auto it = json.begin(); it != json.end(); it++) {
        if (it.value().is_string()) {
            jsonMap[it.key()] = it.value().get<std::string>();
        }
    }
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
