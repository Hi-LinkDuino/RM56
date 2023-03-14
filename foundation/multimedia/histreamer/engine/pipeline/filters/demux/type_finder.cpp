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

#define HST_LOG_TAG "TypeFinder"

#include "type_finder.h"
#include <algorithm>
#include "foundation/log.h"
#include "osal/utils/util.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
namespace {
bool IsPluginSupportedExtension(Plugin::PluginInfo& pluginInfo, const std::string& extension)
{
    if (pluginInfo.pluginType != Plugin::PluginType::DEMUXER) {
        return false;
    }
    bool rtv = false;
    auto info = pluginInfo.extra[PLUGIN_INFO_EXTRA_EXTENSIONS];
    if (info.HasValue() && info.SameTypeWith(typeid(std::vector<std::string>))) {
        for (const auto& ext : Plugin::AnyCast<std::vector<std::string>&>(info)) {
            if (ext == extension) {
                rtv = true;
                break;
            }
        }
    }
    return rtv;
}

void ToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return std::tolower(ch); });
}

// lowercase suffix
std::vector<std::string> g_findTypeNeededForSameSuffix = {"aac"};
} // namespace

TypeFinder::TypeFinder()
    : sniffNeeded_(true),
      uriSuffix_(),
      mediaDataSize_(0),
      pluginName_(),
      plugins_(),
      pluginRegistryChanged_(true),
      task_(nullptr),
      checkRange_(),
      peekRange_(),
      typeFound_()
{
    MEDIA_LOG_D("TypeFinder ctor called...");
}

TypeFinder::~TypeFinder()
{
    MEDIA_LOG_D("TypeFinder dtor called...");
    if (task_) {
        task_->Stop();
    }
}

bool TypeFinder::IsSniffNeeded(std::string suffix)
{
    ToLower(suffix);
    bool suffixChanged = uriSuffix_ != suffix;
    if (suffixChanged || pluginRegistryChanged_) {
        return true;
    }
    return std::any_of(g_findTypeNeededForSameSuffix.begin(), g_findTypeNeededForSameSuffix.end(),
                       [&suffix](const std::string& uriSuffix) { return suffix == uriSuffix; });
}

void TypeFinder::Init(std::string uriSuffix, size_t mediaDataSize, std::function<bool(uint64_t, size_t)> checkRange,
                      std::function<bool(uint64_t, size_t, AVBufferPtr&)> peekRange)
{
    mediaDataSize_ = mediaDataSize;
    checkRange_ = std::move(checkRange);
    peekRange_ = std::move(peekRange);
    sniffNeeded_ = IsSniffNeeded(uriSuffix);
    if (sniffNeeded_) {
        uriSuffix_.swap(uriSuffix);
        pluginName_.clear();
        if (GetPlugins()) {
            SortPlugins(uriSuffix_);
        } else {
            MEDIA_LOG_E("TypeFinder Init failed due to no demuxer plugins...");
        }
    }
}

/**
 * FindMediaType for seekable source, is a sync interface.
 * @return plugin names for the found media type.
 */
std::string TypeFinder::FindMediaType()
{
    if (sniffNeeded_) {
        pluginName_ = SniffMediaType();
        if (pluginName_.empty()) {
            pluginName_ = GuessMediaType();
        }
        sniffNeeded_ = false;
    }
    return pluginName_;
}

/**
 * FindMediaTypeAsync for non-seekable source
 * @param typeFound is a callback called when media type found.
 */
void TypeFinder::FindMediaTypeAsync(std::function<void(std::string)> typeFound)
{
    typeFound_ = std::move(typeFound);
    task_ = std::make_shared<OSAL::Task>("TypeFinder");
    task_->RegisterHandler([this]() { DoTask(); });
    task_->Start();
}

Plugin::Status TypeFinder::ReadAt(int64_t offset, std::shared_ptr<Plugin::Buffer>& buffer, size_t expectedLen)
{
    if (!buffer || expectedLen == 0 || !IsOffsetValid(offset)) {
        MEDIA_LOG_E("ReadAt failed, buffer empty: " PUBLIC_LOG_D32 ", expectedLen: " PUBLIC_LOG_ZU ", offset: "
                    PUBLIC_LOG_D64, !buffer, expectedLen, offset);
        return Plugin::Status::ERROR_INVALID_PARAMETER;
    }
    const int maxTryTimes = 3;
    int i = 0;
    while (!checkRange_(offset, expectedLen) && (i++ < maxTryTimes)) {
        OSAL::SleepFor(5); // 5 ms
    }
    if (i == maxTryTimes) {
        MEDIA_LOG_E("ReadAt exceed maximum allowed try times and failed.");
        return Plugin::Status::ERROR_NOT_ENOUGH_DATA;
    }
    FALSE_LOG_MSG(peekRange_(static_cast<uint64_t>(offset), expectedLen, buffer), "peekRange failed.");
    return Plugin::Status::OK;
}

Plugin::Status TypeFinder::GetSize(size_t& size)
{
    size = mediaDataSize_;
    return (mediaDataSize_ > 0) ? Plugin::Status::OK : Plugin::Status::ERROR_UNKNOWN;
}

Plugin::Seekable TypeFinder::GetSeekable()
{
    return Plugin::Seekable::INVALID;
}

void TypeFinder::DoTask()
{
    if (sniffNeeded_) {
        pluginName_ = SniffMediaType();
        if (pluginName_.empty()) {
            pluginName_ = GuessMediaType();
        }
        sniffNeeded_ = false;
    }
    task_->StopAsync();
    typeFound_(pluginName_);
}

std::string TypeFinder::SniffMediaType()
{
    PROFILE_BEGIN("SniffMediaType begin.");
    constexpr int probThresh = 50; // valid range [0, 100]
    std::string pluginName;
    int maxProb = 0;
    auto dataSource = shared_from_this();
    int cnt = 0;
    for (const auto& plugin : plugins_) {
        auto prob = Plugin::PluginManager::Instance().Sniffer(plugin->name, dataSource);
        ++cnt;
        if (prob > probThresh) {
            pluginName = plugin->name;
            break;
        }
        if (prob > maxProb) {
            maxProb = prob;
            pluginName = plugin->name;
        }
    }
    PROFILE_END("SniffMediaType end, sniffed plugin num = " PUBLIC_LOG_D32, cnt);
    return pluginName;
}

std::string TypeFinder::GuessMediaType() const
{
    std::string pluginName;
    for (const auto& pluginInfo : plugins_) {
        if (IsPluginSupportedExtension(*pluginInfo, uriSuffix_)) {
            pluginName = pluginInfo->name;
            break;
        }
    }
    return pluginName;
}

bool TypeFinder::IsOffsetValid(int64_t offset) const
{
    return (mediaDataSize_ == 0) || offset < mediaDataSize_;
}

bool TypeFinder::GetPlugins()
{
    MEDIA_LOG_I("TypeFinder::GetPlugins : " PUBLIC_LOG_D32 ", empty: " PUBLIC_LOG_D32,
                (pluginRegistryChanged_ == true), plugins_.empty());
    if (pluginRegistryChanged_) {
        pluginRegistryChanged_ = false;
        auto pluginNames = Plugin::PluginManager::Instance().ListPlugins(Plugin::PluginType::DEMUXER);
        for (auto& pluginName : pluginNames) {
            auto pluginInfo = Plugin::PluginManager::Instance().GetPluginInfo(Plugin::PluginType::DEMUXER, pluginName);
            if (!pluginInfo) {
                MEDIA_LOG_E("GetPlugins failed for plugin: " PUBLIC_LOG_S, pluginName.c_str());
                continue;
            }
            plugins_.emplace_back(std::move(pluginInfo));
        }
    }
    return !plugins_.empty();
}

void TypeFinder::SortPlugins(const std::string& uriSuffix)
{
    if (uriSuffix.empty()) {
        return;
    }
    std::stable_sort(
        plugins_.begin(), plugins_.end(),
        [&uriSuffix](const std::shared_ptr<Plugin::PluginInfo>& lhs, const std::shared_ptr<Plugin::PluginInfo>& rhs) {
            if (IsPluginSupportedExtension(*lhs, uriSuffix)) {
                return (lhs->rank >= rhs->rank) || !IsPluginSupportedExtension(*rhs, uriSuffix);
            } else {
                return (lhs->rank >= rhs->rank) && !IsPluginSupportedExtension(*rhs, uriSuffix);
            }
        });
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
