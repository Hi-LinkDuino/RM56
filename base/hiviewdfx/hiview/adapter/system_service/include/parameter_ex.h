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
#ifndef HIVIEW_OAL_SYSTEM_PROPERTY_H
#define HIVIEW_OAL_SYSTEM_PROPERTY_H
#include <cstdint>
#include <string>
namespace OHOS {
namespace HiviewDFX {
constexpr char KEY_BUILD_CHARACTER[] = "ro.build.characteristics";
constexpr char KEY_HIVIEW_USER_TYPE[] = "ro.logsystem.usertype";
namespace Parameter {
enum UserType {
    UNKNOWN_TYPE = -1,
    COMMERCIAL = 1,
    FANS,
    BETA,
    TEST,
    OVERSEAS_BETA,
    OVERSEAS_COMMERCIAL,
};

enum DeviceType {
    UNKNOWN = 0,
    PHONE,
    WATCH,
    TV,
    IVI,
};

std::string GetString(const std::string& key, const std::string& defaultValue);
int64_t GetInteger(const std::string& key, const int64_t defaultValue);
uint64_t GetUnsignedInteger(const std::string& key, const uint64_t defaultValue);
bool GetBoolean(const std::string& key, const bool defaultValue);
bool SetProperty(const std::string& key, const std::string& defaultValue);
bool IsBetaVersion();
DeviceType GetDeviceType();
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_OAL_SYSTEM_PROPERTY_H
