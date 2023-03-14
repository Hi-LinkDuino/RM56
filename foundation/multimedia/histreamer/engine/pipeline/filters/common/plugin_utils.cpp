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

#define HST_LOG_TAG "Pip_Plugin_Utils"

#include "plugin_utils.h"

#include <cstdarg>
#include <sstream>

#include "foundation/log.h"
#include "pipeline/core/plugin_attr_desc.h"

namespace {
constexpr int32_t MAX_BUF_LEN = 512; // 512 buffer size
#define RETURN_IF_FAILED(exec, errVal, retVal) \
do { \
    auto res = exec; \
    if (res == (errVal)) { \
        return errVal; \
    } else { \
        (retVal) = res; \
    } \
} while (0)

#define RETURN_IF_SNPRI_FAILED(exec, snPrintRet, retVal) \
do { \
    snPrintRet = exec; \
    if ((snPrintRet) == -1) { \
        MEDIA_LOG_W("stringiness failed due to " PUBLIC_LOG_S " or truncated.", strerror(errno)); \
        return retVal; \
    } \
} while (0)

using namespace OHOS::Media;

template <typename T>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const T& val)
{
    MEDIA_LOG_I("no cap trans function for " PUBLIC_LOG_S " may be update?", name);
    return 0;
}

template <typename T>
int32_t FixedCapKeyStringiness(char* buf, size_t maxLen, const char* name, const char* typeName,
                               const Plugin::ValueType& val)
{
    int pos = 0;
    auto ret = snprintf_truncated_s(buf, maxLen, "%s:(%s)", name, typeName);
    if (ret == -1) {
        return ret;
    }
    pos += ret;
    auto item = Plugin::AnyCast<T>(&val);
    RETURN_IF_FAILED(Stringiness(buf + pos, maxLen - pos, name, *item), -1, ret);
    return pos + ret;
}

template<typename T>
int32_t IntervalCapKeyStringiness(char* buf, size_t maxLen, const char* name, const char* typeName,
                                  const Plugin::ValueType& val)
{
    int pos = 0;
    auto ret = snprintf_truncated_s(buf, maxLen, "%s:(%s)[", name, typeName);
    if (ret == -1) {
        return ret;
    }
    pos += ret;
    auto item = Plugin::AnyCast<Plugin::IntervalCapability<T>>(&val);
    RETURN_IF_FAILED(Stringiness(buf + pos, maxLen - pos, name, item->first), -1, ret);
    pos += ret;
    RETURN_IF_FAILED(snprintf_truncated_s(buf + pos, maxLen - pos, ", "), -1, ret);
    pos += ret;
    RETURN_IF_FAILED(Stringiness(buf + pos, maxLen - pos, name, item->second), -1, ret);
    pos += ret;
    RETURN_IF_FAILED(snprintf_truncated_s(buf + pos, maxLen - pos, "]"), -1, ret);
    return pos + ret;
}

template<typename T>
int32_t DiscreteCapKeyStringiness(char* buf, size_t maxLen, const char* name, const char* typeName,
                                  const Plugin::ValueType& val)
{
    int pos = 0;
    auto ret = snprintf_truncated_s(buf, maxLen, "%s:(%s){", name, typeName);
    if (ret == -1) {
        return ret;
    }
    pos += ret;
    auto item = Plugin::AnyCast<Plugin::DiscreteCapability<T>>(&val);
    int32_t i = 0;
    int32_t length = item->size();
    for (; i < length - 1; i++) {
        RETURN_IF_FAILED(Stringiness<T>(buf + pos, maxLen - pos, name, item->at(i)), -1, ret);
        pos += ret;
        RETURN_IF_FAILED(snprintf_truncated_s(buf + pos, maxLen - pos, ", "), -1, ret);
        pos += ret;
    }
    if (i == length -1) {
        RETURN_IF_FAILED(Stringiness<T>(buf + pos, maxLen - pos, name, item->at(i)), -1, ret);
        pos += ret;
    }
    RETURN_IF_FAILED(snprintf_truncated_s(buf + pos, maxLen - pos, "}"), -1, ret);
    return pos + ret;
}

template<typename T>
int32_t CapKeyStringiness(char* buf, size_t maxLen, const char* name, const char* typeName,
                          const Plugin::ValueType& val)
{
    if (val.SameTypeWith(typeid(Plugin::FixedCapability<T>))) {
        return FixedCapKeyStringiness<T>(buf, maxLen, name, typeName, val);
    } else if (val.SameTypeWith(typeid(Plugin::IntervalCapability<T>))) {
        return IntervalCapKeyStringiness<T>(buf, maxLen, name, typeName, val);
    } else if (val.SameTypeWith(typeid(Plugin::DiscreteCapability<T>))) {
        return DiscreteCapKeyStringiness<T>(buf, maxLen, name, typeName, val);
    } else {
        MEDIA_LOG_W("cap " PUBLIC_LOG_S "type mismatches when cast to string, which should be " PUBLIC_LOG_S,
                    name, typeName);
    }
    return -1;
}
template <>
MEDIA_UNUSED int32_t Stringiness(char* buf, size_t maxLen, const char* name, const int32_t& val)
{
    return snprintf_truncated_s(buf, maxLen, "%" PRIi32, val);
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const uint32_t& val)
{
    return snprintf_truncated_s(buf, maxLen, "%" PRIu32, val);
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const std::string& val)
{
    return snprintf_truncated_s(buf, maxLen, "%s", val.c_str());
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const int64_t& val)
{
    return snprintf_truncated_s(buf, maxLen, "%" PRId64, val);
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const uint64_t& val)
{
    return snprintf_truncated_s(buf, maxLen, "%" PRIu64, val);
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::AudioSampleFormat& val)
{
    if (Pipeline::g_auSampleFmtStrMap.count(val) == 0) {
        MEDIA_LOG_W("audio sample format " PUBLIC_LOG_D32 " is unknown", static_cast<int32_t>(val));
        return 0;
    }
    return snprintf_truncated_s(buf, maxLen, "%s", Pipeline::g_auSampleFmtStrMap.at(val));
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::AudioChannelLayout& val)
{
    if (Pipeline::g_auChannelLayoutStrMap.count(val) == 0) {
        MEDIA_LOG_W("audio channel layout " PUBLIC_LOG_U64 " is unknown", static_cast<uint64_t>(val));
        return 0;
    }
    return snprintf_truncated_s(buf, maxLen, "%s", Pipeline::g_auChannelLayoutStrMap.at(val));
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::VideoPixelFormat& val)
{
    if (Pipeline::g_videoPixelFormatStrMap.count(val) == 0) {
        MEDIA_LOG_W("video pixel format " PUBLIC_LOG_U32 " is unknown", static_cast<uint32_t>(val));
        return 0;
    }
    return snprintf_truncated_s(buf, maxLen, "%s", Pipeline::g_videoPixelFormatStrMap.at(val));
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::AudioAacProfile& val)
{
    if (Pipeline::g_auAacProfileNameStrMap.count(val) == 0) {
        MEDIA_LOG_W("audio aac profile name " PUBLIC_LOG_U8 " is unknown", static_cast<uint8_t>(val));
        return 0;
    }
    return snprintf_truncated_s(buf, maxLen, "%s", Pipeline::g_auAacProfileNameStrMap.at(val));
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::AudioAacStreamFormat& val)
{
    if (Pipeline::g_auAacStreamFormatNameStrMap.count(val) == 0) {
        MEDIA_LOG_W("audio aac stream format name " PUBLIC_LOG_U8 " is unknown", static_cast<uint8_t>(val));
        return 0;
    }
    return snprintf_truncated_s(buf, maxLen, "%s", Pipeline::g_auAacStreamFormatNameStrMap.at(val));
}

template<>
int32_t Stringiness(char* buf, size_t maxLen, const char* name, const Plugin::CodecConfig& val)
{
    return snprintf_truncated_s(buf, maxLen, "...");
}

template<typename T>
int32_t MetaIDStringiness(char* buf, size_t maxLen, const char* name, const char* typeName,
                          const Plugin::ValueType& val)
{
    if (val.SameTypeWith(typeid(T))) {
        return FixedCapKeyStringiness<T>(buf, maxLen, name, typeName, val);
    } else {
        MEDIA_LOG_W("meta " PUBLIC_LOG_S " type mismatches when cast to string", name);
    }
    return -1;
}
using CapStrnessFunc = std::function<int32_t(char*, size_t, const char*, const char*, const Plugin::ValueType&)>;

std::map<Plugin::MetaID, CapStrnessFunc> g_metaStrnessMap = {
    {Plugin::MetaID::MIME, MetaIDStringiness<std::string>},
    {Plugin::MetaID::TRACK_ID, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::MEDIA_CODEC_CONFIG,MetaIDStringiness<std::vector<uint8_t>>},
    {Plugin::MetaID::AUDIO_CHANNELS, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_SAMPLE_RATE, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_SAMPLE_FORMAT, MetaIDStringiness<Plugin::AudioSampleFormat>},
    {Plugin::MetaID::AUDIO_SAMPLE_PER_FRAME, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_CHANNEL_LAYOUT, MetaIDStringiness<Plugin::AudioChannelLayout>},
    {Plugin::MetaID::AUDIO_OUTPUT_CHANNELS, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_OUTPUT_CHANNEL_LAYOUT, MetaIDStringiness<Plugin::AudioChannelLayout>},
    {Plugin::MetaID::MEDIA_TITLE, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_ARTIST, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_LYRICIST, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_ALBUM, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_ALBUM_ARTIST, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_DATE, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_COMMENT, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_GENRE, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_DESCRIPTION, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_COPYRIGHT, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_LANGUAGE, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_LYRICS, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_DURATION, MetaIDStringiness<int64_t>},
    {Plugin::MetaID::MEDIA_BITRATE, MetaIDStringiness<int64_t>},
    {Plugin::MetaID::MEDIA_FILE_EXTENSION, MetaIDStringiness<std::string>},
    {Plugin::MetaID::MEDIA_FILE_SIZE, MetaIDStringiness<uint64_t>},
    {Plugin::MetaID::MEDIA_SEEKABLE, MetaIDStringiness<Plugin::Seekable>},
    {Plugin::MetaID::AUDIO_MPEG_VERSION, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_MPEG_LAYER ,MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_AAC_PROFILE, MetaIDStringiness<Plugin::AudioAacProfile>},
    {Plugin::MetaID::AUDIO_AAC_LEVEL, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::AUDIO_AAC_STREAM_FORMAT, MetaIDStringiness<Plugin::AudioAacStreamFormat>},
    {Plugin::MetaID::VIDEO_WIDTH, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::VIDEO_HEIGHT, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::VIDEO_FRAME_RATE, MetaIDStringiness<uint32_t>},
    {Plugin::MetaID::VIDEO_PIXEL_FORMAT, MetaIDStringiness<Plugin::VideoPixelFormat>},
    {Plugin::MetaID::BITS_PER_CODED_SAMPLE, MetaIDStringiness<uint32_t>},
};
}

namespace OHOS {
namespace Media {
namespace Pipeline {
template<typename T>
bool AssignParameterIfMatch(Tag tag, T& ret, const Plugin::ValueType& val)
{
    if (HasTagInfo(tag)) {
        if (val.SameTypeWith(*GetTagDefValue(tag)) && val.SameTypeWith(typeid(T))) {
            ret = Plugin::AnyCast<T>(val);
            return true;
        } else {
            MEDIA_LOG_I("type of " PUBLIC_LOG_S " mismatch, should be " PUBLIC_LOG_S,
                        GetTagStrName(tag), GetTagTypeStrName(tag));
        }
    } else {
        MEDIA_LOG_I("tag " PUBLIC_LOG_D32 " is not in map, may be update it?", tag);
    }
    return false;
}

template bool AssignParameterIfMatch(Tag tag, Plugin::SrcInputType& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, uint32_t& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, int32_t& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, int64_t& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, double& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, Plugin::AudioSampleFormat& ret, const Plugin::ValueType& val);
template bool AssignParameterIfMatch(Tag tag, Plugin::AudioChannelLayout& ret, const Plugin::ValueType& val);

/**
 * translate plugin error into pipeline error code
 * @param pluginError
 * @return
 */
OHOS::Media::ErrorCode TranslatePluginStatus(Plugin::Status pluginError)
{
    const static std::map<Plugin::Status, ErrorCode> g_transTable = {
        {Plugin::Status::END_OF_STREAM, ErrorCode::END_OF_STREAM},
        {Plugin::Status::OK, ErrorCode::SUCCESS},
        {Plugin::Status::NO_ERROR, ErrorCode::SUCCESS},
        {Plugin::Status::ERROR_UNKNOWN, ErrorCode::ERROR_UNKNOWN},
        {Plugin::Status::ERROR_PLUGIN_ALREADY_EXISTS, ErrorCode::ERROR_UNKNOWN},
        {Plugin::Status::ERROR_INCOMPATIBLE_VERSION, ErrorCode::ERROR_UNKNOWN},
        {Plugin::Status::ERROR_NO_MEMORY, ErrorCode::ERROR_NO_MEMORY},
        {Plugin::Status::ERROR_WRONG_STATE, ErrorCode::ERROR_INVALID_OPERATION},
        {Plugin::Status::ERROR_UNIMPLEMENTED, ErrorCode::ERROR_UNIMPLEMENTED},
        {Plugin::Status::ERROR_INVALID_PARAMETER, ErrorCode::ERROR_INVALID_PARAMETER_VALUE},
        {Plugin::Status::ERROR_INVALID_DATA, ErrorCode::ERROR_UNKNOWN},
        {Plugin::Status::ERROR_MISMATCHED_TYPE, ErrorCode::ERROR_INVALID_PARAMETER_TYPE},
        {Plugin::Status::ERROR_TIMED_OUT, ErrorCode::ERROR_TIMED_OUT},
        {Plugin::Status::ERROR_UNSUPPORTED_FORMAT, ErrorCode::ERROR_UNSUPPORTED_FORMAT},
        {Plugin::Status::ERROR_NOT_ENOUGH_DATA, ErrorCode::ERROR_UNKNOWN},
        {Plugin::Status::ERROR_NOT_EXISTED, ErrorCode::ERROR_NOT_EXISTED},
        {Plugin::Status::ERROR_AGAIN, ErrorCode::ERROR_AGAIN},
        {Plugin::Status::ERROR_PERMISSION_DENIED, ErrorCode::ERROR_PERMISSION_DENIED},
        {Plugin::Status::ERROR_DELAY_READY, ErrorCode::SUCCESS},
    };
    auto ite = g_transTable.find(pluginError);
    if (ite == g_transTable.end()) {
        return ErrorCode::ERROR_UNKNOWN;
    }
    return ite->second;
}

bool TranslateIntoParameter(const int& key, OHOS::Media::Plugin::Tag& tag)
{
    if (key < static_cast<int32_t>(OHOS::Media::Plugin::Tag::INVALID)) {
        return false;
    }
    tag = static_cast<OHOS::Media::Plugin::Tag>(key);
    return true;
}

std::vector<std::pair<std::shared_ptr<Plugin::PluginInfo>, Plugin::Capability>>
    FindAvailablePlugins(const Plugin::Capability& upStreamCaps, Plugin::PluginType pluginType)
{
    auto pluginNames = Plugin::PluginManager::Instance().ListPlugins(pluginType);
    std::vector<std::pair<std::shared_ptr<Plugin::PluginInfo>, Plugin::Capability>> infos;
    for (const auto& name : pluginNames) {
        auto tmpInfo = Plugin::PluginManager::Instance().GetPluginInfo(pluginType, name);
        Capability cap;
        if (ApplyCapabilitySet(upStreamCaps, tmpInfo->inCaps, cap)) {
            infos.emplace_back(tmpInfo, cap);
        }
    }
    return infos;
}
std::vector<std::shared_ptr<Plugin::PluginInfo>> FindAvailablePluginsByOutputMime(const std::string& outputMime,
                                                                                  Plugin::PluginType pluginType)
{
    auto pluginNames = Plugin::PluginManager::Instance().ListPlugins(pluginType);
    std::vector<std::shared_ptr<Plugin::PluginInfo>> rets;
    for (const auto& name : pluginNames) {
        auto tmpInfo = Plugin::PluginManager::Instance().GetPluginInfo(pluginType, name);
        for (const auto& cap : tmpInfo->outCaps) {
            if (cap.mime == outputMime) {
                rets.emplace_back(tmpInfo);
                break;
            }
        }
    }
    return rets;
}
uint8_t GetBytesPerSample(Plugin::AudioSampleFormat fmt)
{
    uint8_t bytesPerSample = 0;
    switch (fmt) {
        case Plugin::AudioSampleFormat::S64:
        case Plugin::AudioSampleFormat::S64P:
        case Plugin::AudioSampleFormat::U64:
        case Plugin::AudioSampleFormat::U64P:
        case Plugin::AudioSampleFormat::F64:
        case Plugin::AudioSampleFormat::F64P:
            bytesPerSample = 8; // 8 bytes
            break;
        case Plugin::AudioSampleFormat::F32:
        case Plugin::AudioSampleFormat::F32P:
        case Plugin::AudioSampleFormat::S32:
        case Plugin::AudioSampleFormat::S32P:
        case Plugin::AudioSampleFormat::U32:
        case Plugin::AudioSampleFormat::U32P:
            bytesPerSample = 4; // 4 bytes
            break;
        case Plugin::AudioSampleFormat::S24:
        case Plugin::AudioSampleFormat::S24P:
        case Plugin::AudioSampleFormat::U24:
        case Plugin::AudioSampleFormat::U24P:
            bytesPerSample = 3; // 3 bytes
            break;
        case Plugin::AudioSampleFormat::S16:
        case Plugin::AudioSampleFormat::S16P:
        case Plugin::AudioSampleFormat::U16:
        case Plugin::AudioSampleFormat::U16P:
            bytesPerSample = 2; // 2 bytes
            break;
        case Plugin::AudioSampleFormat::S8:
        case Plugin::AudioSampleFormat::S8P:
        case Plugin::AudioSampleFormat::U8:
        case Plugin::AudioSampleFormat::U8P:
            bytesPerSample = 1; // 1 bytes
            break;
        default:
            bytesPerSample = 0;
            break;
    }
    return bytesPerSample;
}

std::string Capability2String(const Capability& capability)
{
    const static std::map<Capability::Key, CapStrnessFunc> capStrnessMap = {
        {Capability::Key::MEDIA_BITRATE, CapKeyStringiness<int64_t>},
        {Capability::Key::AUDIO_SAMPLE_RATE, CapKeyStringiness<uint32_t>},
        {Capability::Key::AUDIO_CHANNELS, CapKeyStringiness<uint32_t>},
        {Capability::Key::AUDIO_CHANNEL_LAYOUT, CapKeyStringiness<Plugin::AudioChannelLayout>},
        {Capability::Key::AUDIO_SAMPLE_FORMAT, CapKeyStringiness<Plugin::AudioSampleFormat>},
        {Capability::Key::AUDIO_MPEG_VERSION, CapKeyStringiness<uint32_t>},
        {Capability::Key::AUDIO_MPEG_LAYER, CapKeyStringiness<uint32_t>},
        {Capability::Key::AUDIO_AAC_PROFILE, CapKeyStringiness<Plugin::AudioAacProfile>},
        {Capability::Key::AUDIO_AAC_LEVEL, CapKeyStringiness<uint32_t>},
        {Capability::Key::AUDIO_AAC_STREAM_FORMAT, CapKeyStringiness<Plugin::AudioAacStreamFormat>},
        {Capability::Key::VIDEO_PIXEL_FORMAT, CapKeyStringiness<Plugin::VideoPixelFormat>},
    };
    char buffer[MAX_BUF_LEN] = {0};
    int pos = 0;
    int32_t ret = 0;
    RETURN_IF_SNPRI_FAILED(
        snprintf_truncated_s(buffer, sizeof(buffer), "Capability{mime:%s, ", capability.mime.c_str()), ret, {});
    pos += ret;
    bool needEtc = false;
    for (const auto& cap : capability.keys) {
        if (pos >= MAX_BUF_LEN - 1) { // reserve for "}"
            needEtc = true;
            break;
        }
        if (capStrnessMap.count(cap.first) == 0 || !HasTagInfo(static_cast<Tag>(cap.first))) {
            MEDIA_LOG_W(PUBLIC_LOG_D32 " is not in map, may be new key which is not contained?", cap.first);
            continue;
        }
        const auto& info = g_tagInfoMap.at(static_cast<Tag>(cap.first));
        RETURN_IF_SNPRI_FAILED(capStrnessMap.at(cap.first)(buffer + pos, MAX_BUF_LEN - pos, std::get<0>(info),
            std::get<2>(info), cap.second), ret, buffer); // secondary parameter
        pos += ret;
        RETURN_IF_SNPRI_FAILED(snprintf_truncated_s(buffer + pos, MAX_BUF_LEN - pos, ", "), ret, buffer);
        pos += ret;
    }
    if (needEtc) {
        pos = MAX_BUF_LEN - 5; // 5 is length of " ...}"
        snprintf_truncated_s(buffer + pos, MAX_BUF_LEN + 1 - pos, " ...}");
    } else {
        pos -= 2; // 2 is length of ", "
        snprintf_truncated_s(buffer + pos, MAX_BUF_LEN + 1 - pos, "}");
    }
    return buffer;
}

std::string Meta2String(const Plugin::Meta& meta)
{
    char buffer[MAX_BUF_LEN + 1] = {0}; // one more is for \0
    int pos = 0;
    int32_t ret = 0;
    std::string mime;
    RETURN_IF_SNPRI_FAILED(snprintf_truncated_s(buffer + pos, MAX_BUF_LEN - pos, "Meta{"), ret, {});
    pos += ret;
    bool needEtc = false;
    for (const auto & item : meta.GetMetaIDs()) {
        if (pos >= MAX_BUF_LEN - 2) { // reserve for "}\0"
            needEtc = true;
            break;
        }
        if (!HasTagInfo(static_cast<Tag>(item)) || g_metaStrnessMap.count(item) == 0) {
            MEDIA_LOG_W("meta id " PUBLIC_LOG_D32 "is not is map, may be update the info map?", item);
            continue;
        }
        const Plugin::ValueType* tmp = meta.GetData(item);
        const auto& tuple = g_tagInfoMap.at(static_cast<Tag>(item));
        if (tmp) {
            RETURN_IF_SNPRI_FAILED(g_metaStrnessMap.at(item)(buffer + pos, MAX_BUF_LEN - pos,
                std::get<0>(tuple), std::get<2>(tuple), *tmp), ret, buffer); // secondary parameter
            pos += ret;
            RETURN_IF_SNPRI_FAILED(snprintf_truncated_s(buffer + pos, MAX_BUF_LEN - pos, ", "), ret, buffer);
            pos += ret;
        }
    }
    if (needEtc) {
        pos = MAX_BUF_LEN - 5; // 5 is length of " ...}\0"
        snprintf_truncated_s(buffer + pos, MAX_BUF_LEN + 1 - pos, " ...}");
    } else {
        pos -= 2; // 2 is length of ", "
        snprintf_truncated_s(buffer + pos, MAX_BUF_LEN + 1 - pos, "}");
    }
    return buffer;
}
#undef RETURN_IF_SNPRI_FAILED
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
