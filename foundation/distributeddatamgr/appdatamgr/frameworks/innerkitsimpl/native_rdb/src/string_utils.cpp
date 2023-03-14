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

#include "string_utils.h"

namespace OHOS {
namespace NativeRdb {
std::string StringUtils::SurroundWithQuote(std::string value, std::string quote)
{
    if (value.empty()) {
        return value;
    }
    std::string str = quote + value + quote;
    return str;
}

// Join array members as parameters of a function call.
std::string StringUtils::SurroundWithFunction(std::string function, std::string separator,
    std::vector<std::string> array)
{
    std::string builder(function);
    builder += "(";
    bool isFirst = true;
    for (auto text : array) {
        if (!isFirst) {
            builder = builder + " " + separator + " ";
        } else {
            isFirst = false;
        }
        builder += text;
    }
    builder += ")";
    return builder;
}

StringUtils::StringUtils() {}
StringUtils::~StringUtils() {}
} // namespace NativeRdb
} // namespace OHOS