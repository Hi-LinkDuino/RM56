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

#ifndef HIVIEW_BASE_SYS_EVENT_H
#define HIVIEW_BASE_SYS_EVENT_H

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "pipeline.h"

namespace OHOS {
namespace HiviewDFX {
class SysEventCreator;
enum ParseStatus {
    STATE_PARSING_DOMAIN,
    STATE_PARSING_NAME,
    STATE_PARSING_TYPE,
    STATE_PARSING_TIME,
    STATE_PARSING_TZONE,
    STATE_PARSING_PID,
    STATE_PARSING_TID,
    STATE_PARSING_UID,
    STATE_PARSING_TRACE_ID,
    STATE_PARSING_SPAN_ID,
    STATE_PARSING_PARENT_SPAN_ID,
    STATE_PARSING_TRACE_FLAG,
};
struct ParseItem {
    const char* keyString;
    const char* valueStart;
    const char* valueEnd1;
    const char* valueEnd2;
    ParseStatus status;
    bool parseContine;
};

class SysEvent : public PipelineEvent {
public:
    SysEvent(const std::string& sender, PipelineEventProducer* handler, const std::string& jsonStr);
    SysEvent(const std::string& sender, PipelineEventProducer* handler, SysEventCreator& sysEventCreator);
    ~SysEvent();
public:
    int PaserJson();
    int32_t GetPid() const;
    int32_t GetTid() const;
    int32_t GetUid() const;
    int16_t GetTz() const;
    void SetSeq(int64_t);
    int64_t GetSeq() const;
    std::string GetEventValue(const std::string& key);
    uint64_t GetEventIntValue(const std::string& key);
    void SetEventValue(const std::string& key, int64_t value);
    void SetEventValue(const std::string& key, const std::string& value, bool append = false);

private:
    int64_t seq_;
    int32_t pid_;
    int32_t tid_;
    int32_t uid_;
    int16_t tz_;
    void InitialMember(ParseStatus status, const std::string &content);
};

class SysEventCreator {
public:
    enum EventType {
        FAULT     = 1,    // system fault event
        STATISTIC = 2,    // system statistic event
        SECURITY  = 3,    // system security event
        BEHAVIOR  = 4     // system behavior event
    };
public:
    SysEventCreator(const std::string &domain, const std::string &eventName, EventType type);

    template <typename T, typename U, typename... Rest>
    struct is_one_of : std::conditional_t<std::is_same_v<typename std::decay_t<T>, typename std::decay_t<U>>,
        std::true_type, is_one_of<T, Rest...>> {};

    template <typename T, typename U>
    struct is_one_of<T, U> : std::conditional_t<std::is_same_v<typename std::decay_t<T>, typename std::decay_t<U>>,
        std::true_type, std::false_type> {};

    template <typename Inst, template <typename...> typename Tmpl>
    struct is_instantiation_of : std::false_type {};

    template <template <typename...> typename Tmpl, typename... Args>
    struct is_instantiation_of<Tmpl<Args...>, Tmpl> : std::true_type {};

    // supported types of the key
    template <typename T>
    struct is_type_key : is_one_of<T, char *, char const *, std::string>::type {};

    template <typename T>
    inline static constexpr bool is_type_key_v = is_type_key<T>::value;

    // supported base types of the value
    template <typename T>
    struct is_type_value_base : is_one_of<T, bool, char, signed char, unsigned char, short, unsigned short, int,
        unsigned int, long, unsigned long, long long, unsigned long long, float, double, char *, char const *,
        std::string>::type {};

    template <typename T>
    inline static constexpr bool is_type_value_base_v = is_type_value_base<T>::value;

    // supported vector types of the value
    template <typename T>
    inline static constexpr bool is_type_value_vector_v = []() {
        if constexpr(is_instantiation_of<typename std::decay_t<T>, std::vector>::value) {
            return is_type_value_base_v<typename std::decay_t<T>::value_type>;
        }
        return false;
    }();

    template<typename T>
    static decltype(auto) GetItem(T&& item)
    {
        if constexpr(is_one_of<T, char, signed char, unsigned char>::value) {
            return static_cast<short>(item);
        } else if constexpr(is_one_of<T, char *, char const *, std::string>::value) {
            return std::quoted(EscapeStringValue(item));
        } else {
            return std::forward<T>(item);
        }
    }

    template<typename K, typename V>
    SysEventCreator& SetKeyValue(K&& key, V&& value)
    {
        jsonStr_ << GetItem(std::forward<K>(key)) << ":";
        if constexpr(is_type_value_base_v<V>) {
            jsonStr_ << GetItem(std::forward<V>(value)) << ",";
        } else if constexpr(is_type_value_vector_v<V>) {
            jsonStr_ << "[";
            for (const auto &it : value) {
                jsonStr_ << GetItem(it) << ",";
            }
            if (!value.empty()) {
                jsonStr_.seekp(-1, std::ios_base::end);
            }
            jsonStr_ << "],";
        }
        return *this;
    }

private:
    friend class SysEvent;
    std::string BuildSysEventJson();
    static std::string EscapeStringValue(const std::string &value);
    static std::string EscapeStringValue(const char* value);
    static std::string EscapeStringValue(char* value);
private:
    std::stringstream jsonStr_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_SYS_EVENT_H
