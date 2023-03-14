/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "Compatible_Check"

#include "compatible_check.h"

#include <algorithm>
#include <functional>
#include <map>

#include "pipeline/core/plugin_attr_desc.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
static constexpr uint8_t ALLOW_FIXED = 1 << 0;
static constexpr uint8_t ALLOW_INTERVAL = 1 << 1;
static constexpr uint8_t ALLOW_DISCRETE = 1 << 2;

static inline bool IsFixedAllowed(uint8_t flags)
{
    return ALLOW_FIXED & flags;
}

static inline bool IsIntervalAllowed(uint8_t flags)
{
    return ALLOW_INTERVAL & flags;
}

static inline bool IsDiscreteAllowed(uint8_t flags)
{
    return ALLOW_DISCRETE & flags;
}

template<typename T>
bool CapabilityValueCheck(CapabilityID key, std::pair<const Plugin::ValueType&, const Plugin::ValueType&> inVals,
                          uint8_t flags, std::function<int(T,T)> cmpFunc, Plugin::ValueType& outValue);

template <typename T>
bool FixInvalDiscCapValCheck(CapabilityID key, const Plugin::ValueType& val1, const Plugin::ValueType& val2,
                             Plugin::ValueType& outValue)
{
    return CapabilityValueCheck<T>(key, {val1, val2}, ALLOW_FIXED | ALLOW_INTERVAL | ALLOW_DISCRETE,
        [](T a, T b) {
        return a - b;
        }, outValue);
}

template <typename T, typename U>
bool FixDiscCapValCheck(CapabilityID key, const Plugin::ValueType& val1, const Plugin::ValueType& val2,
                        Plugin::ValueType& outValue)
{
    return CapabilityValueCheck<T>(key, {val1, val2}, ALLOW_FIXED | ALLOW_DISCRETE, [](T a, T b) {
        return static_cast<U>(a) - static_cast<U>(b);
    }, outValue);
}

static std::vector<CapabilityID> g_allCapabilityId = {
    CapabilityID::AUDIO_SAMPLE_RATE, // 0
    CapabilityID::AUDIO_CHANNELS, // 1
    CapabilityID::AUDIO_CHANNEL_LAYOUT, // 2
    CapabilityID::AUDIO_SAMPLE_FORMAT, // 3
    CapabilityID::AUDIO_MPEG_VERSION, // 4
    CapabilityID::AUDIO_MPEG_LAYER, // 5
    CapabilityID::AUDIO_AAC_PROFILE, // 6
    CapabilityID::AUDIO_AAC_LEVEL, // 7
    CapabilityID::AUDIO_AAC_STREAM_FORMAT, // 8
    CapabilityID::VIDEO_PIXEL_FORMAT, // 9
    CapabilityID::MEDIA_BITRATE, // 10
};

template <typename T>
bool ExtractFixedCap(const Plugin::ValueType& value, Plugin::ValueType& fixedValue);
static std::map<CapabilityID, std::function<bool(const Plugin::ValueType&, Plugin::ValueType&)>> g_capExtrMap = {
    {g_allCapabilityId[0], ExtractFixedCap<uint32_t>}, // 0
    {g_allCapabilityId[1], ExtractFixedCap<uint32_t>}, // 1
    {g_allCapabilityId[2], ExtractFixedCap<Plugin::AudioChannelLayout>}, // 2
    {g_allCapabilityId[3], ExtractFixedCap<Plugin::AudioSampleFormat>}, // 3
    {g_allCapabilityId[4], ExtractFixedCap<uint32_t>}, // 4
    {g_allCapabilityId[5], ExtractFixedCap<uint32_t>}, // 5
    {g_allCapabilityId[6], ExtractFixedCap<Plugin::AudioAacProfile>}, // 6
    {g_allCapabilityId[7], ExtractFixedCap<uint32_t>}, // 7
    {g_allCapabilityId[8], ExtractFixedCap<Plugin::AudioAacStreamFormat>}, // 8
    {g_allCapabilityId[9], ExtractFixedCap<Plugin::VideoPixelFormat>}, // 9
    {g_allCapabilityId[10], ExtractFixedCap<int64_t>}, // 10
};
using CheckFunc = std::function<bool(Capability::Key key, const Plugin::ValueType& val1, const Plugin::ValueType& val2,
        Plugin::ValueType& outValue)>;
static std::map<CapabilityID, CheckFunc> g_capabilityValueCheckMap = {
    {g_allCapabilityId[0], FixInvalDiscCapValCheck<uint32_t>}, // 0
    {g_allCapabilityId[1], FixInvalDiscCapValCheck<uint32_t>}, // 1
    {g_allCapabilityId[2], FixDiscCapValCheck<Plugin::AudioChannelLayout, uint64_t>}, // 2
    {g_allCapabilityId[3], FixDiscCapValCheck<Plugin::AudioSampleFormat, uint8_t>}, // 3
    {g_allCapabilityId[4], FixInvalDiscCapValCheck<uint32_t>}, // 4
    {g_allCapabilityId[5], FixInvalDiscCapValCheck<uint32_t>}, // 5
    {g_allCapabilityId[6], FixDiscCapValCheck<Plugin::AudioAacProfile, uint8_t>}, // 6
    {g_allCapabilityId[7], FixInvalDiscCapValCheck<uint32_t>}, // 7
    {g_allCapabilityId[8], FixDiscCapValCheck<Plugin::AudioAacStreamFormat, uint8_t>}, // 8
    {g_allCapabilityId[9], FixDiscCapValCheck<Plugin::VideoPixelFormat, uint32_t>}, // 9
    {g_allCapabilityId[10], FixInvalDiscCapValCheck<int64_t>}, // 10
};


static bool StringEqIgnoreCase(const std::string& s1, const std::string& s2)
{
    if (s1.length() == s2.length()) {
        return std::equal(s1.begin(), s1.end(), s2.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
    }
    return false;
}

bool IsSubsetMime(const std::string& subset, const std::string& universe)
{
    size_t devLinePosInMeta = subset.find_first_of('/');
    if (devLinePosInMeta == 0 || devLinePosInMeta == std::string::npos) {
        MEDIA_LOG_E("wrong format of subset mime, must be xx/xxx");
        return false;
    }

    if (universe == "*") {
        return true;
    }

    size_t devLinePosInCap = universe.find_first_of('/');
    if (devLinePosInCap == 0 || devLinePosInCap == std::string::npos) {
        MEDIA_LOG_E("wrong format of universe mime, must be * or xx/* or xx/xxx");
        return false;
    }

    // if media type is not the same, return false
    if (!StringEqIgnoreCase(subset.substr(0, devLinePosInMeta), universe.substr(0, devLinePosInCap))) {
        return false;
    }
    // if media type of capability is like audio/* video/* image/* etc. always return true
    if (universe.substr(devLinePosInCap + 1) == "*") {
        return true;
    }
    // left mime string compare
    if (!StringEqIgnoreCase(universe.substr(devLinePosInCap + 1), subset.substr(devLinePosInMeta + 1))) {
        return false;
    }
    return true;
}

template <typename T>
T Max(T val1, T val2, std::function<int(T, T)> compareFunc)
{
    if (compareFunc(val1, val2) >= 0) {
        return val1;
    }
    return val2;
}

template <typename T>
T Min(T val1, T val2, std::function<int(T, T)> compareFunc)
{
    if (compareFunc(val1, val2) <= 0) {
        return val1;
    }
    return val2;
}

template <typename T>
bool FFCapabilityCheck(const Plugin::FixedCapability<T>& v1, const Plugin::FixedCapability<T>& v2,
                       std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    if (cmpFunc(v1, v2) == 0) {
        outValue = v1;
        return true;
    }
    return false;
}

template <typename T>
bool FICapabilityCheck(const Plugin::FixedCapability<T>& v1, const Plugin::IntervalCapability<T>& v2,
                       const std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    T max = Max(v2.first, v2.second, cmpFunc);
    T min = Min(v2.first, v2.second, cmpFunc);
    if (cmpFunc(v1, min) >= 0 && cmpFunc(v1, max) <= 0) {
        outValue = v1;
        return true;
    }
    return false;
}

template <typename T>
bool FDCapabilityCheck(const Plugin::FixedCapability<T>& v1, const Plugin::DiscreteCapability<T>& v2,
                       const std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    if (std::any_of(v2.begin(), v2.end(), [&v1, &cmpFunc](const T& tmp){return cmpFunc(tmp, v1) == 0;})) {
        outValue = v1;
        return true;
    }
    return false;
}

template <typename T>
bool IICapabilityCheck(const Plugin::IntervalCapability<T>& v1, const Plugin::IntervalCapability<T>& v2,
                       const std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    T max1 = Max(v1.first, v1.second, cmpFunc);
    T min1 = Min(v1.first, v1.second, cmpFunc);
    T max2 = Max(v2.first, v2.second, cmpFunc);
    T min2 = Min(v2.first, v2.second, cmpFunc);
    T tmpMin = Max(min1, min2, cmpFunc);
    T tmpMax = Min(max1, max2, cmpFunc);
    auto compRes = cmpFunc(tmpMin, tmpMax);
    if (compRes > 0) {
        return false;
    } else if (compRes == 0) {
        outValue = Plugin::FixedCapability<T>(tmpMin);
    } else {
        outValue = Plugin::IntervalCapability<T>(tmpMin, tmpMax);
    }
    return true;
}

template <typename T>
bool IDCapabilityCheck(const Plugin::IntervalCapability<T>& v1, const Plugin::DiscreteCapability<T>& v2,
                       const std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    Plugin::DiscreteCapability<T> tmpOut;
    for (const auto& oneValue : v2) {
        if (cmpFunc(oneValue, v1.first) >= 0 && cmpFunc(oneValue, v1.second) <= 0) {
            tmpOut.emplace_back(oneValue);
        }
    }
    if (tmpOut.empty()) {
        return false;
    }
    if (tmpOut.size() == 1) {
        outValue = Plugin::FixedCapability<T>(tmpOut[0]);
    } else {
        outValue = Plugin::DiscreteCapability<T>(tmpOut);
    }
    return true;
}

template <typename T>
bool DDCapabilityCheck(const Plugin::DiscreteCapability<T>& v1, const Plugin::DiscreteCapability<T>& v2,
                       const std::function<int(T,T)>& cmpFunc, Plugin::ValueType& outValue)
{
    Plugin::DiscreteCapability<T> tmpOut;
    for (const auto& cap1 : v1) {
        if (std::any_of(v2.begin(), v2.end(), [&cap1, &cmpFunc](const T& tmp){return cmpFunc(cap1, tmp) == 0;})) {
            tmpOut.emplace_back(cap1);
        }
    }
    if (tmpOut.empty()) {
        return false;
    }
    if (tmpOut.size() == 1) {
        outValue = Plugin::FixedCapability<T>(tmpOut[0]);
    } else {
        outValue = Plugin::DiscreteCapability<T>(tmpOut);
    }
    return true;
}

void LogOutIncorrectType(CapabilityID key, uint8_t flags)
{
    if (HasTagInfo(static_cast<Tag>(key))) {
        const auto& tuple = g_tagInfoMap.at(static_cast<Tag>(key));
        const auto& typeName = std::get<2>(tuple); // secondary
        MEDIA_LOG_E("type of " PUBLIC_LOG_S " should be"
        " " PUBLIC_LOG_S "(" PUBLIC_LOG_C ")"
        " or Interval<" PUBLIC_LOG_S ">(" PUBLIC_LOG_C ")"
        " or Discrete<" PUBLIC_LOG_S ">(" PUBLIC_LOG_C ")",
        std::get<0>(tuple),
        typeName, IsFixedAllowed(flags)? 'o': 'x',
        typeName, IsFixedAllowed(flags)? 'o': 'x',
        typeName, IsFixedAllowed(flags)? 'o': 'x');
    } else {
        MEDIA_LOG_E("capability " PUBLIC_LOG_D32 "is not in the map, may be update the map?", key);
    }
}

template <typename T>
bool FixedNumericalCapabilityCheck(CapabilityID key, const T& value2, const Plugin::ValueType& value1,
                                   uint8_t flags, std::function<int(T, T)> cmpFunc, Plugin::ValueType& outValue)
{
    if (value1.SameTypeWith(typeid(T))) {
        return FFCapabilityCheck(value2, Plugin::AnyCast<T>(value1), cmpFunc, outValue);
    }
    if (IsIntervalAllowed(flags) && value1.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        return FICapabilityCheck(value2, Plugin::AnyCast<Plugin::IntervalCapability<T>>(value1), cmpFunc, outValue);
    }
    if (IsDiscreteAllowed(flags) && value1.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        return FDCapabilityCheck(value2, Plugin::AnyCast<Plugin::DiscreteCapability<T>>(value1), cmpFunc, outValue);
    }
    LogOutIncorrectType(key, flags);
    return false;
}

template <typename T>
bool IntervalNumericalCapabilityCheck(CapabilityID key, const Plugin::IntervalCapability<T>& value2,
    const Plugin::ValueType& value1, uint8_t flags, std::function<int(T, T)> cmpFunc, Plugin::ValueType& outValue)
{
    if (IsFixedAllowed(flags) && value1.SameTypeWith(typeid(T))) {
        return FICapabilityCheck(Plugin::AnyCast<T>(value1), value2, cmpFunc, outValue);
    }
    if (value1.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        return IICapabilityCheck(Plugin::AnyCast<Plugin::IntervalCapability<T>>(value1), value2, cmpFunc, outValue);
    }
    if (IsDiscreteAllowed(flags) && value1.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        return IDCapabilityCheck(value2, Plugin::AnyCast<Plugin::DiscreteCapability<T>>(value1), cmpFunc, outValue);
    }
    LogOutIncorrectType(key, flags);
    return false;
}

template <typename T>
bool DiscreteNumericalCapabilityCheck(CapabilityID key, const Plugin::DiscreteCapability<T>& value2,
    const Plugin::ValueType& value1, uint8_t flags, std::function<int(T, T)> cmpFunc, Plugin::ValueType& outValue)
{
    if (IsFixedAllowed(flags) && value1.SameTypeWith(typeid(T))) {
        return FDCapabilityCheck(Plugin::AnyCast<T>(value1), value2, cmpFunc, outValue);
    }
    if (IsIntervalAllowed(flags) && value1.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        return IDCapabilityCheck(Plugin::AnyCast<Plugin::IntervalCapability<T>>(value1), value2, cmpFunc, outValue);
    }
    if (value1.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        return DDCapabilityCheck(Plugin::AnyCast<Plugin::DiscreteCapability<T>>(value1), value2, cmpFunc, outValue);
    }
    LogOutIncorrectType(key, flags);
    return false;
}

template<typename T>
bool CapabilityValueCheck(CapabilityID key, std::pair<const Plugin::ValueType&, const Plugin::ValueType&> inVals,
                          uint8_t flags, std::function<int(T,T)> cmpFunc, Plugin::ValueType& outValue)
{
    if (IsFixedAllowed(flags) && inVals.first.SameTypeWith(typeid(Plugin::FixedCapability<T>))) {
        return FixedNumericalCapabilityCheck<T>(key,
            Plugin::AnyCast<Plugin::FixedCapability<T>>(inVals.first), inVals.second, flags, cmpFunc,outValue);
    }
    if (IsIntervalAllowed(flags) && inVals.first.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        return IntervalNumericalCapabilityCheck(key,
            Plugin::AnyCast<Plugin::IntervalCapability<T>>(inVals.first), inVals.second, flags, cmpFunc, outValue);
    }
    if (IsDiscreteAllowed(flags) && inVals.first.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        return DiscreteNumericalCapabilityCheck(key,
            Plugin::AnyCast<Plugin::DiscreteCapability<T>>(inVals.first), inVals.second, flags, cmpFunc, outValue);
    }
    LogOutIncorrectType(key, flags);
    return false;
}

bool MergeCapabilityKeys(const Capability& originCap, const Capability& otherCap, Capability& resCap)
{
    resCap.keys.clear();
    for (const auto& pairKey : originCap.keys) {
        auto oIte = otherCap.keys.find(pairKey.first);
        if (oIte == otherCap.keys.end()) {
            // if key is not in otherCap, then put into resCap
            resCap.keys.insert(pairKey);
            continue;
        }
        // if key is in otherCap, calculate the intersections
        if (g_capabilityValueCheckMap.count(pairKey.first) == 0) {
            MEDIA_LOG_W("capability " PUBLIC_LOG_D32 " cannot be applied, may be update the check map?",
                        static_cast<int32_t>(pairKey.first));
            continue;
        }
        Plugin::ValueType tmp;
        if (g_capabilityValueCheckMap.at(pairKey.first)(pairKey.first, pairKey.second, oIte->second, tmp)) {
            resCap.keys[pairKey.first] = tmp;
        } else {
            //  if no intersections return false
            resCap.keys.clear();
            return false;
        }
    }
    // if key is otherCap but not in originCap, put into resCap
    for (const auto& pairKey : otherCap.keys) {
        if (resCap.keys.count(pairKey.first) == 0) {
            resCap.keys.insert(pairKey);
        }
    }
    return true;
}

bool MergeCapability(const Capability& originCap, const Capability& otherCap, Capability& resCap)
{
    resCap.mime.clear();
    resCap.keys.clear();
    if (!IsSubsetMime(originCap.mime, otherCap.mime)) {
        return false;
    }
    if (!MergeCapabilityKeys(originCap, otherCap, resCap)) {
        return false;
    }
    resCap.mime = originCap.mime;
    return true;
}

bool ApplyCapabilitySet(const Capability& originCap, const CapabilitySet& capabilitySet, Capability& resCap)
{
    Capability tmp;
    for (const auto& cap : capabilitySet) {
        if (MergeCapability(originCap, cap, resCap)) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool ExtractFixedCap(const Plugin::ValueType& value, Plugin::ValueType& fixedValue)
{
    if (value.SameTypeWith(typeid(Plugin::FixedCapability<T>))) {
        fixedValue = Plugin::AnyCast<Plugin::FixedCapability<T>>(value);
        return true;
    } else if (value.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        auto tmp = Plugin::AnyCast<Plugin::IntervalCapability<T>>(value);
        fixedValue = tmp.first;
        return true;
    } else if (value.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        auto tmp = Plugin::AnyCast<Plugin::DiscreteCapability<T>>(value);
        if (!tmp.empty()) {
            fixedValue = tmp[0];
            return true;
        }
    }
    return false;
}

std::shared_ptr<Capability> MetaToCapability(const Plugin::Meta& meta)
{
    auto ret = std::make_shared<Capability>();
    std::string mime;
    if (meta.GetString(Plugin::MetaID::MIME, mime)) {
        ret->mime = mime;
    }
    for (const auto& key : g_allCapabilityId) {
        Plugin::ValueType tmp;
        if (meta.GetData(static_cast<Plugin::MetaID>(key), tmp)) {
            ret->keys[key] = tmp;
        }
    }
    return ret;
}

bool MergeMetaWithCapability(const Plugin::Meta& meta, const Capability& cap,  Plugin::Meta& resMeta)
{
    resMeta.Clear();
    // change meta into capability firstly
    Capability metaCap;
    metaCap.mime = cap.mime;
    for (const auto& key : g_allCapabilityId) {
        Plugin::ValueType tmp;
        if (meta.GetData(static_cast<Plugin::MetaID>(key), tmp)) {
            metaCap.keys[key] = tmp;
        }
    }
    Capability resCap;
    if (!MergeCapability(metaCap, cap, resCap)) {
        return false;
    }
    // merge capability
    resMeta.Update(meta);
    resMeta.SetString(Plugin::MetaID::MIME, cap.mime);
    for (const auto& oneCap : resCap.keys) {
        if (g_capExtrMap.count(oneCap.first) == 0) {
            continue;
        }
        auto func = g_capExtrMap[oneCap.first];
        Plugin::ValueType tmp;
        if (func(oneCap.second, tmp)) {
            resMeta.SetData(static_cast<Plugin::MetaID>(oneCap.first), tmp);
        }
    }
    return true;
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
