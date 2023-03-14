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
#ifndef HAP_JSON_PARSER_UTILS_H
#define HAP_JSON_PARSER_UTILS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

namespace OHOS {
namespace Security {
namespace Verify {
using JsonMap = std::unordered_map<std::string, std::string>;
using JsonObjVec = std::vector<nlohmann::json>;
using StringVec = std::vector<std::string>;

class JsonParserUtils {
public:
    static bool ReadTrustedRootCAFromJson(nlohmann::json& jsonObj, const std::string& jsonPath, std::string& error);
    static bool GetJsonString(const nlohmann::json& json, const std::string& key, std::string& value);
    static bool ParseJsonToObjVec(const nlohmann::json& json, const std::string& key, JsonObjVec& jsonObjVec);
    static bool GetJsonInt(const nlohmann::json& json, const std::string& key, int& value);
    static bool GetJsonStringVec(const nlohmann::json& json, const std::string& key, StringVec& value);
    static void ParseJsonToMap(const nlohmann::json& json, JsonMap& JsonMap);
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_JSON_PARSER_UTILS_H
