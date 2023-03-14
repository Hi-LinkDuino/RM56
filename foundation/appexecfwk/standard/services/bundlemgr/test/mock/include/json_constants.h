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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_JSON_CONSTANTS_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_JSON_CONSTANTS_H

#include "nlohmann/json.hpp"

namespace OHOS {
namespace AppExecFwk {
namespace JsonConstants {
const nlohmann::json NOT_STRING_TYPE = R"(
        {
            "number": 234,
            "bool": true,
            "object": {},
            "array": []
        }
    )"_json;

const nlohmann::json NOT_NUMBER_TYPE = R"(
        {
            "string": "info",
            "object": {},
            "array": []
        }
    )"_json;

const nlohmann::json NOT_BOOL_TYPE = R"(
        {
            "number": 234,
            "string": "info",
            "object": {},
            "array": []
        }
    )"_json;

const nlohmann::json NOT_OBJECT_TYPE = R"(
        {
            "bool": true,
            "number": 234,
            "string": "info",
            "array": []
        }
    )"_json;

const nlohmann::json NOT_ARRAY_TYPE = R"(
        {
            "bool": true,
            "number": 234,
            "string": "info",
            "object": {}
        }
    )"_json;
}  // namespace JsonConstants
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_JSON_CONSTANTS_H