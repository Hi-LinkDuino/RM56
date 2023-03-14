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
#include "parameter_ex.h"
#include <cstdint>

namespace OHOS {
namespace HiviewDFX {
namespace Parameter {
std::string GetString(const std::string& key, const std::string& defaultValue)
{
    return defaultValue;
}

int64_t GetInteger(const std::string& key, const int64_t defaultValue)
{
    return defaultValue;
}

uint64_t GetUnsignedInteger(const std::string& key, const uint64_t defaultValue)
{
    return defaultValue;
}

bool GetBoolean(const std::string& key, const bool defaultValue)
{
    return defaultValue;
}

bool SetProperty(const std::string& key, const std::string& defaultValue)
{
    return true;
}
} // namespace Parameter
} // namespace HiviewDFX
} // namespace OHOS
