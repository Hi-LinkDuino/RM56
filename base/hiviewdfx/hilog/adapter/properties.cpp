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

#include "properties.h"

#include <array>
#include <atomic>
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <pthread.h>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <strstream>
#include <sys/uio.h>
#include <unistd.h>
#include <unordered_map>

#include <hilog/log.h>
#include <parameter.h>
#include <sysparam_errno.h>

using namespace std;

using ReadLock = shared_lock<shared_timed_mutex>;
using InsertLock = unique_lock<shared_timed_mutex>;

static pthread_mutex_t g_globalLevelLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_tagLevelLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_domainLevelLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_debugLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_persistDebugLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_privateLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_processFlowLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_domainFlowLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_kmsgLock = PTHREAD_MUTEX_INITIALIZER;

static int LockByProp(uint32_t propType);
static void UnlockByProp(uint32_t propType);

class PropertyTypeLocker {
public:
    explicit PropertyTypeLocker(uint32_t propType)
        : m_propType(propType)
        , m_isLocked(false)
    {
        m_isLocked = !LockByProp(m_propType);
    }

    ~PropertyTypeLocker()
    {
        if (m_isLocked) {
            UnlockByProp(m_propType);
        }
    }

    bool isLocked() const
    {
        return m_isLocked;
    }

private:
    uint32_t m_propType;
    bool m_isLocked;
};

using RawPropertyData = std::array<char, HILOG_PROP_VALUE_MAX>;

template<typename T>
class CacheData {
public:
    using DataConverter = std::function<T(const RawPropertyData&, const T& defaultVal)>;

    CacheData(DataConverter converter, const T& defaultValue, uint32_t propType, const std::string& suffix = "")
        : m_value(defaultValue), m_defaultValue(defaultValue), m_propType(propType), m_converter(converter)
    {
        m_key = GetPropertyName(m_propType) + suffix;
    }

    T getValue()
    {
        if (m_handle == -1) {
            if (m_commit == 0) { // temparary solution, after sysparam supply get_global_commitid, FIXME
                return m_defaultValue;
            }
            int handle = static_cast<int>(FindParameter(m_key.c_str()));
            if (handle == -1) {
                m_commit = 0; // temparary solution, after sysparam supply get_global_commitid, FIXME
                return m_defaultValue;
            }
            m_handle = handle;
        }
        int currentCommit = static_cast<int>(GetParameterCommitId(m_handle));
        PropertyTypeLocker locker(m_propType);
        if (locker.isLocked()) {
            if (currentCommit != m_commit) {
                updateValue();
                m_commit = currentCommit;
            }
            return m_value;
        } else {
            return getDirectValue();
        }
    }

private:
    bool getRawValue(char *value, unsigned int len)
    {
        auto res = GetParameterValue(m_handle, value, len);
        if (res < 0) {
            std::cerr << "CacheData -> GetParameterValue -> Can't get value for key: " << m_key;
            std::cerr << " handle: " << m_handle << " Result: " << res << "\n";
            return false;
        }
        return true;
    }

    T getDirectValue()
    {
        RawPropertyData tempData;
        if (!getRawValue(tempData.data(), tempData.size())) {
            return m_defaultValue;
        }
        return m_converter(tempData, m_defaultValue);
    }

    void updateValue()
    {
        if (!getRawValue(m_rawData.data(), m_rawData.size())) {
            m_value = m_defaultValue;
            return;
        }
        m_value = m_converter(m_rawData, m_defaultValue);
    }

    RawPropertyData m_rawData = {0};
    int m_handle = -1;
    int m_commit = -1;
    T m_value;
    const T m_defaultValue;
    const uint32_t m_propType;
    std::string m_key;
    DataConverter m_converter;
};

using SwitchCache = CacheData<bool>;
using LogLevelCache = CacheData<uint16_t>;


void PropertyGet(const string &key, char *value, int len)
{
    if (len > HILOG_PROP_VALUE_MAX) {
        std::cerr << "PropertyGet(): len exceed maximum.\n";
        return;
    }

    int handle = static_cast<int>(FindParameter(key.c_str()));
    if (handle == -1) {
        return;
    }

    auto res = GetParameterValue(handle, value, len);
    if (res < 0) {
        std::cerr << "PropertyGet() -> GetParameterValue -> Can't get value for key: " << key;
        std::cerr << " handle: " << handle << " Result: " << res << "\n";
    }
}

void PropertySet(const string &key, const char* value)
{
    auto len = value ? strlen(value) : 0;
    if (len > HILOG_PROP_VALUE_MAX) {
        std::cerr << "PropertySet(): len exceed maximum.\n";
        return;
    }

    auto result = SetParameter(key.c_str(), value);
    if (result < 0) {
        if (result == EC_INVALID) {
            std::cerr << "PropertySet(): Invalid arguments.\n";
        } else {
            std::cerr << "PropertySet(): key: " << key.c_str() << "value: " << value << ",  error: " << result << "\n";
        }
    }
}

string GetProgName()
{
#ifdef HILOG_USE_MUSL
    return program_invocation_short_name;
#else
    return getprogname();
#endif
}

string GetPropertyName(uint32_t propType)
{
    string key;
    switch (propType) {
        case PROP_PRIVATE:
            key = "hilog.private.on";
            break;
        case PROP_PROCESS_FLOWCTRL:
            key = "hilog.flowctrl.pid.on";
            break;
        case PROP_DOMAIN_FLOWCTRL:
            key = "hilog.flowctrl.domain.on";
            break;
        case PROP_GLOBAL_LOG_LEVEL:
            key = "hilog.loggable.global";
            break;
        case PROP_DOMAIN_LOG_LEVEL:
            key = "hilog.loggable.domain.";
            break;
        case PROP_TAG_LOG_LEVEL:
            key = "hilog.loggable.tag.";
            break;
        case PROP_SINGLE_DEBUG:
            key = "hilog.debug.on";
            break;
        case PROP_KMSG:
            key = "persist.sys.hilog.kmsg.on";
            break;
        case PROP_PERSIST_DEBUG:
            key = "persist.sys.hilog.debug.on";
            break;
        case PROP_BUFFER_SIZE:
            key = "hilog.buffersize.";
        default:
            break;
    }
    return key;
}

static int LockByProp(uint32_t propType)
{
    switch (propType) {
        case PROP_PRIVATE:
            return pthread_mutex_trylock(&g_privateLock);
        case PROP_PROCESS_FLOWCTRL:
            return pthread_mutex_trylock(&g_processFlowLock);
        case PROP_DOMAIN_FLOWCTRL:
            return pthread_mutex_trylock(&g_domainFlowLock);
        case PROP_GLOBAL_LOG_LEVEL:
            return pthread_mutex_trylock(&g_globalLevelLock);
        case PROP_DOMAIN_LOG_LEVEL:
            return pthread_mutex_trylock(&g_domainLevelLock);
        case PROP_TAG_LOG_LEVEL:
            return pthread_mutex_trylock(&g_tagLevelLock);
        case PROP_SINGLE_DEBUG:
            return pthread_mutex_trylock(&g_debugLock);
        case PROP_KMSG:
            return pthread_mutex_trylock(&g_kmsgLock);
        case PROP_PERSIST_DEBUG:
            return pthread_mutex_trylock(&g_persistDebugLock);
        default:
            return -1;
    }
}

static void UnlockByProp(uint32_t propType)
{
    switch (propType) {
        case PROP_PRIVATE:
            pthread_mutex_unlock(&g_privateLock);
            break;
        case PROP_PROCESS_FLOWCTRL:
            pthread_mutex_unlock(&g_processFlowLock);
            break;
        case PROP_DOMAIN_FLOWCTRL:
            pthread_mutex_unlock(&g_domainFlowLock);
            break;
        case PROP_GLOBAL_LOG_LEVEL:
            pthread_mutex_unlock(&g_globalLevelLock);
            break;
        case PROP_DOMAIN_LOG_LEVEL:
            pthread_mutex_unlock(&g_domainLevelLock);
            break;
        case PROP_TAG_LOG_LEVEL:
            pthread_mutex_unlock(&g_tagLevelLock);
            break;
        case PROP_SINGLE_DEBUG:
            pthread_mutex_unlock(&g_debugLock);
            break;
        case PROP_KMSG:
            pthread_mutex_unlock(&g_kmsgLock);
            break;
        case PROP_PERSIST_DEBUG:
            pthread_mutex_unlock(&g_persistDebugLock);
            break;
        default:
            break;
    }
}

static bool textToBool(const RawPropertyData& data, bool defaultVal)
{
    if (!strcmp(data.data(), "true")) {
        return true;
    } else if (!strcmp(data.data(), "false")) {
        return false;
    }
    return defaultVal;
}

bool IsDebugOn()
{
    return IsSingleDebugOn() || IsPersistDebugOn();
}

bool IsSingleDebugOn()
{
    static auto *switchCache = new SwitchCache(textToBool, false, PROP_SINGLE_DEBUG);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

bool IsPersistDebugOn()
{
    static auto *switchCache = new SwitchCache(textToBool, false, PROP_PERSIST_DEBUG);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

bool IsPrivateSwitchOn()
{
    static auto *switchCache = new SwitchCache(textToBool, true, PROP_PRIVATE);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

bool IsProcessSwitchOn()
{
    static auto *switchCache = new SwitchCache(textToBool, false, PROP_PROCESS_FLOWCTRL);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

bool IsDomainSwitchOn()
{
    static auto *switchCache = new SwitchCache(textToBool, false, PROP_DOMAIN_FLOWCTRL);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

bool IsKmsgSwitchOn()
{
    static auto *switchCache = new SwitchCache(textToBool, false, PROP_KMSG);
    if (switchCache == nullptr) {
        return false;
    }
    return switchCache->getValue();
}

static uint16_t textToLogLevel(const RawPropertyData& data, uint16_t defaultVal)
{
    static const std::unordered_map<char, uint16_t> logLevels = {
        { 'd', LOG_DEBUG }, { 'D', LOG_DEBUG },
        { 'i', LOG_INFO }, { 'I', LOG_INFO },
        { 'w', LOG_WARN }, { 'W', LOG_WARN },
        { 'e', LOG_ERROR }, { 'E', LOG_ERROR },
        { 'f', LOG_FATAL }, { 'F', LOG_FATAL },
    };
    auto it = logLevels.find(data[0]);
    if (it != logLevels.end()) {
        return it->second;
    }
    return LOG_LEVEL_MIN;
}

uint16_t GetGlobalLevel()
{
    static auto *logLevelCache = new LogLevelCache(textToLogLevel, LOG_LEVEL_MIN, PROP_GLOBAL_LOG_LEVEL);
    return logLevelCache->getValue();
}

uint16_t GetDomainLevel(uint32_t domain)
{
    static auto *domainMap = new std::unordered_map<uint32_t, LogLevelCache*>();
    static shared_timed_mutex* mtx = new shared_timed_mutex;
    std::decay<decltype(*domainMap)>::type::iterator it;
    {
        ReadLock lock(*mtx);
        it = domainMap->find(domain);
    }
    if (it == domainMap->end()) { // new domain
        InsertLock lock(*mtx);
        it = domainMap->find(domain); // secured for two thread went across above condition
        if (it == domainMap->end()) {
            LogLevelCache* levelCache = new LogLevelCache(textToLogLevel, LOG_LEVEL_MIN, PROP_DOMAIN_LOG_LEVEL,
                to_string(domain));
            auto result = domainMap->insert({ domain, levelCache });
            if (!result.second) {
                std::cerr << "Can't insert new LogLevelCache for domain: " << domain << "\n";
                return LOG_LEVEL_MIN;
            }
            it = result.first;
        }
    }
    LogLevelCache* levelCache = it->second;
    return levelCache->getValue();
}

uint16_t GetTagLevel(const string& tag)
{
    static auto *tagMap = new std::unordered_map<std::string, LogLevelCache*>();
    static shared_timed_mutex* mtx = new shared_timed_mutex;
    std::decay<decltype(*tagMap)>::type::iterator it;
    {
        ReadLock lock(*mtx);
        it = tagMap->find(tag);
    }
    if (it == tagMap->end()) { // new tag
        InsertLock lock(*mtx);
        it = tagMap->find(tag); // secured for two thread went across above condition
        if (it == tagMap->end()) {
            LogLevelCache* levelCache = new LogLevelCache(textToLogLevel, LOG_LEVEL_MIN, PROP_TAG_LOG_LEVEL, tag);
            auto result = tagMap->insert({ tag, levelCache });
            if (!result.second) {
                std::cerr << "Can't insert new LogLevelCache for tag: " << tag << "\n";
                return LOG_LEVEL_MIN;
            }
            it = result.first;
        }
    }
    LogLevelCache* levelCache = it->second;
    return levelCache->getValue();
}

static string GetBufferSizePropName(uint16_t type, bool persist)
{
    string name = persist ? "persist.sys." : "";

    static const string logTypeStr[LOG_TYPE_MAX + 1] = {
        "app", "init", "", "core", "kmsg", "global"
    };
    name += (GetPropertyName(PROP_BUFFER_SIZE) + logTypeStr[type]);
    return name;
}

size_t GetBufferSize(uint16_t type, bool persist)
{
    char value[HILOG_PROP_VALUE_MAX] = {0};

    if (type > LOG_TYPE_MAX || type < LOG_TYPE_MIN) {
        return 0;
    }

    PropertyGet(GetBufferSizePropName(type, persist), value, HILOG_PROP_VALUE_MAX);
    if (value[0] == 0) {
        return 0;
    }

    return std::stoi(value);
}

void SetBufferSize(uint16_t type, bool persist, size_t size)
{
    if (type > LOG_TYPE_MAX || type < LOG_TYPE_MIN) {
        return;
    }

    PropertySet(GetBufferSizePropName(type, persist), to_string(size).c_str());
    return;
}