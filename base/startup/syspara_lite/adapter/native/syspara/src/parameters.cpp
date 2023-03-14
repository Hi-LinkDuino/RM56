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

#include "parameters.h"

#include <cerrno>
#include <unordered_map>
#include <vector>

#include "parameters_abstractor.h"
#include "sys_param.h"

namespace OHOS {
namespace system {
namespace {
class NullAbstractor : public ParametersAbstractor {
public:
    std::string GetParameter(const std::string& key, const std::string& def) override
    {
        unsigned int len = 0;
        int ret = SystemGetParameter(key.c_str(), nullptr, &len);
        if (ret == 0 && len > 0) {
            std::vector<char> value(len + 1);
            ret = SystemGetParameter(key.c_str(), value.data(), &len);
            if (ret == 0) {
                return std::string(value.data());
            }
        }
        return def;
    }
    bool SetParameter(const std::string& key, const std::string& value) override
    {
        return SystemSetParameter(key.c_str(), value.c_str()) == 0;
    }

    int WaitParameter(const std::string& key, const std::string& value, int timeout) override
    {
        return SystemWaitParameter(key.c_str(), value.c_str(), timeout);
    }

    unsigned int FindParameter(const std::string& key) override
    {
        unsigned int handle = 0;
        int ret = SystemFindParameter(key.c_str(), &handle);
        if (ret != 0) {
            return static_cast<unsigned int>(-1);
        }
        return handle;
    }

    unsigned int GetParameterCommitId(unsigned int handle) override
    {
        unsigned int commitId = 0;
        int ret = SystemGetParameterCommitId(handle, &commitId);
        if (ret != 0) {
            return static_cast<unsigned int>(-1);
        }
        return commitId;
    }

    std::string GetParameterName(unsigned int handle) override
    {
        std::vector<char> value(PARAM_NAME_LEN_MAX);
        int ret = SystemGetParameterName(handle, value.data(), PARAM_NAME_LEN_MAX);
        if (ret == 0) {
            return std::string(value.data());
        }
        return std::string();
    }

    std::string GetParameterValue(unsigned int handle) override
    {
        unsigned int len = 0;
        int ret = SystemGetParameterValue(handle, nullptr, &len);
        if (ret == 0 && len > 0) {
            std::vector<char> value(len + 1);
            ret = SystemGetParameterValue(handle, value.data(), &len);
            if (ret == 0) {
                return std::string(value.data());
            }
        }
        return std::string();
    }
} g_abstractor;

ParametersAbstractor& g_abstractorRef = g_abstractor;

constexpr unsigned int DECIMAL = 10;
constexpr unsigned int HEX = 16;

template<typename T>
bool StringToInt(const std::string& str, T min, T max, T& out)
{
    const char* s = str.c_str();
    while (isspace(*s)) {
        s++;
    }

    bool positiveHex = (str.size() > 1 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
    bool negativeHex = (str.size() > 2 && s[0] == '-' && s[1] == '0' && (s[2] == 'x' || s[2] == 'X')); // 2: shorttest
    int base = (positiveHex || negativeHex) ? HEX : DECIMAL;
    char* end = nullptr;
    errno = 0;
    long long int result = strtoll(s, &end, base);
    if (errno != 0) {
        return false;
    }
    if (s == end || *end != '\0') {
        return false;
    }
    if (result < min || max < result) {
        return false;
    }
    out = static_cast<T>(result);
    return true;
}

template<typename T>
bool StringToUint(const std::string& str, T max, T& out)
{
    const char* s = str.c_str();
    while (isspace(*s)) {
        s++;
    }

    if (s[0] == '-') {
        return false;
    }

    int base = (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? HEX : DECIMAL;
    char* end = nullptr;
    errno = 0;
    unsigned long long int result = strtoull(s, &end, base);
    if (errno != 0) {
        return false;
    }
    if (end == s) {
        return false;
    }
    if (*end != '\0') {
        return false;
    }
    if (max < result) {
        return false;
    }
    out = static_cast<T>(result);
    return true;
}
}  // namespace

std::string GetParameter(const std::string& key, const std::string& def)
{
    return g_abstractorRef.GetParameter(key, def);
}

bool GetBoolParameter(const std::string& key, bool def)
{
    std::string value = GetParameter(key, "");
    if ((value == "1") || (value == "y") || (value == "yes") || (value == "on") || (value == "true")) {
        return true;
    } else if ((value == "0") || (value == "n") || (value == "no") || (value == "off") || (value == "false")) {
        return false;
    }
    return def;
}

template<typename T>
T GetIntParameter(const std::string& key, T def, T min, T max)
{
    if (!std::is_signed<T>::value) {
        return def;
    }
    T result;
    std::string value = GetParameter(key, "");
    if (!value.empty() && StringToInt(value, min, max, result)) {
        return result;
    }
    return def;
}

template int8_t GetIntParameter(const std::string&, int8_t, int8_t, int8_t);
template int16_t GetIntParameter(const std::string&, int16_t, int16_t, int16_t);
template int32_t GetIntParameter(const std::string&, int32_t, int32_t, int32_t);
template int64_t GetIntParameter(const std::string&, int64_t, int64_t, int64_t);

template<typename T>
T GetUintParameter(const std::string& key, T def, T max)
{
    if (!std::is_unsigned<T>::value) {
        return def;
    }
    T result;
    std::string value = GetParameter(key, "");
    if (!value.empty() && StringToUint(value, max, result)) {
        return result;
    }
    return def;
}

template uint8_t GetUintParameter(const std::string&, uint8_t, uint8_t);
template uint16_t GetUintParameter(const std::string&, uint16_t, uint16_t);
template uint32_t GetUintParameter(const std::string&, uint32_t, uint32_t);
template uint64_t GetUintParameter(const std::string&, uint64_t, uint64_t);

bool SetParameter(const std::string& key, const std::string& value)
{
    return g_abstractorRef.SetParameter(key, value);
}

int WaitParameter(const std::string& key, const std::string& value, int timeout)
{
    return g_abstractorRef.WaitParameter(key, value, timeout);
}

unsigned int FindParameter(const std::string& key)
{
    return g_abstractorRef.FindParameter(key);
}

unsigned int GetParameterCommitId(unsigned int handle)
{
    return g_abstractorRef.GetParameterCommitId(handle);
}

std::string GetParameterName(unsigned int handle)
{
    return g_abstractorRef.GetParameterName(handle);
}

std::string GetParameterValue(unsigned int handle)
{
    return g_abstractorRef.GetParameterValue(handle);
}

void SetAbstractor(const ParametersAbstractor& abstractor)
{
    g_abstractorRef = abstractor;
}

std::string GetDeviceType(void)
{
    std::unordered_map<std::string, std::string> deviceTypeMap = {
        { "watch", "wearable" },
        { "fitnessWatch", "liteWearable" },
    };
    std::string deviceType = GetParameter("const.build.characteristics", "");
    if (deviceTypeMap.count(deviceType) != 0) {
        return deviceTypeMap[deviceType];
    }
    return deviceType;
}
}  // namespace system
}  // namespace OHOS
