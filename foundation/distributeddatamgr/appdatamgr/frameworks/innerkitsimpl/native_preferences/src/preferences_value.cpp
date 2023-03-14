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

#include "preferences_value.h"

namespace OHOS {
namespace NativePreferences {
PreferencesValue::PreferencesValue(int value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(int64_t value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(float value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(double value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(bool value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(std::string value)
{
    data_ = value;
}

PreferencesValue::PreferencesValue(std::set<std::string> value)
{
    data_ = std::move(value);
}

bool PreferencesValue::IsInt() const
{
    auto pVal = std::get_if<int>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsLong() const
{
    auto pVal = std::get_if<int64_t>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsFloat() const
{
    auto pVal = std::get_if<float>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsDouble() const
{
    auto pVal = std::get_if<double>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsBool() const
{
    auto pVal = std::get_if<bool>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsString() const
{
    auto pVal = std::get_if<std::string>(&data_);
    return (pVal != nullptr);
}

bool PreferencesValue::IsSet() const
{
    auto pVal = std::get_if<std::set<std::string>>(&data_);
    return (pVal != nullptr);
}

PreferencesValue::operator int()
{
    return std::get<int>(data_);
}

PreferencesValue::operator int64_t()
{
    return std::get<int64_t>(data_);
}

PreferencesValue::operator float()
{
    return std::get<float>(data_);
}

PreferencesValue::operator double()
{
    return std::get<double>(data_);
}

PreferencesValue::operator bool()
{
    return std::get<bool>(data_);
}

PreferencesValue::operator std::string()
{
    return std::get<std::string>(data_);
}

PreferencesValue::operator std::set<std::string>()
{
    return std::get<std::set<std::string>>(data_);
}

bool PreferencesValue::operator == (const PreferencesValue &value)
{
    return (this->data_ == value.data_);
}
} // End of namespace NativePreferences
} // End of namespace OHOS
