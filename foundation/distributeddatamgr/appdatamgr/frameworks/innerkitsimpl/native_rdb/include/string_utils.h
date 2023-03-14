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

#ifndef NATIVE_RDB_STRINGUTILS_H
#define NATIVE_RDB_STRINGUTILS_H


#include <string>
#include <vector>

namespace OHOS {
namespace NativeRdb {
class StringUtils {
public:
    static std::string SurroundWithQuote(std::string value, std::string quote);
    static std::string SurroundWithFunction(std::string function, std::string separator,
        std::vector<std::string> array);
    static bool IsEmpty(std::string source)
    {
        return (source.empty());
    }

private:
    StringUtils();
    ~StringUtils();
};
} // namespace NativeRdb
} // namespace OHOS

#endif