/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "gavdp/a2dp_def.h"
#include "include/a2dp_codec_config.h"
#include "include/a2dp_codec_errors.h"
#include "include/a2dp_codec_factory.h"
#include "include/a2dp_codec_wrapper.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
static bool OrderCodecPriority(const A2dpCodecConfig *element1, const A2dpCodecConfig *element2)
{
    if (element1->GetCodecPriority() > element2->GetCodecPriority()) {
        return true;
    }

    if (element1->GetCodecPriority() < element2->GetCodecPriority()) {
        return false;
    }

    return (element1->GetCodecIndex() > element2->GetCodecIndex());
}

A2dpCodecFactory::A2dpCodecFactory(const std::map<A2dpCodecIndex, A2dpCodecPriority> &codecPriorities)
    : codecPriorities_(codecPriorities), currentCodecConfig_(nullptr), lastCodecConfig_(nullptr)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    Initialize();
}

A2dpCodecFactory::~A2dpCodecFactory()
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    for (auto iter = indexedCodecs_.begin(); iter != indexedCodecs_.end(); iter++) {
        delete iter->second;
    }
}

bool A2dpCodecFactory::Initialize()
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    for (int i = A2DP_CODEC_INDEX_MIN; i < A2DP_CODEC_INDEX_MAX; i++) {
        auto codecIndex = static_cast<A2dpCodecIndex>(i);

        // Select the codec priority if explicitly configured
        A2dpCodecPriority codecPriority = A2DP_CODEC_PRIORITY_DEFAULT;

        auto priority = codecPriorities_.find(codecIndex);
        if (priority != codecPriorities_.end()) {
            codecPriority = priority->second;
        }
        LOG_INFO("[CodecFactory] %{public}s codecIndex(%u) codecPriority(%u)\n", __func__, codecIndex, codecPriority);
        A2dpCodecConfig *codecConfig = A2dpCodecConfig::CreateCodec(codecIndex, codecPriority);
        if (codecConfig == nullptr) {
            continue;
        }

        indexedCodecs_.insert(std::make_pair(codecIndex, codecConfig));

        if (codecIndex < A2DP_SOURCE_CODEC_INDEX_MAX) {
            indexedSourceCodecs_.push_back(codecConfig);
            indexedSourceCodecs_.sort(OrderCodecPriority);
        } else {
            indexedSinkCodecs_.push_back(codecConfig);
            indexedSinkCodecs_.sort(OrderCodecPriority);
        }
    }

    if (indexedSourceCodecs_.empty()) {
        LOG_ERROR("%{public}s: no Source codecs were initialized", __func__);
    }

    if (indexedSinkCodecs_.empty()) {
        LOG_ERROR("%{public}s: no Sink codecs were initialized", __func__);
    }

    return (!indexedSourceCodecs_.empty() && !indexedSinkCodecs_.empty());
}

A2dpCodecConfig *A2dpCodecFactory::GetCodecConfig() const
{
    return currentCodecConfig_;
}

A2dpCodecConfig *A2dpCodecFactory::FindSourceCodec(const uint8_t *codeInfo) const
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    A2dpCodecIndex codecIndex = GetSourceCodecIndex(codeInfo);
    if (codecIndex == A2DP_SOURCE_CODEC_INDEX_MAX) {
        return nullptr;
    }

    auto iter = indexedCodecs_.find(static_cast<A2dpCodecIndex>(codecIndex));
    if (iter == indexedCodecs_.end()) {
        LOG_WARN("[CodecFactory] %{public}s can't find codecIndex(%u)\n", __func__, codecIndex);
        return nullptr;
    }

    return iter->second;
}

A2dpCodecConfig *A2dpCodecFactory::FindSinkCodec(const uint8_t *codeInfo) const
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    A2dpCodecIndex codecIndex = GetSinkCodecIndex(codeInfo);
    if (codecIndex == A2DP_SINK_CODEC_INDEX_MAX) {
        return nullptr;
    }

    auto iter = indexedCodecs_.find(static_cast<A2dpCodecIndex>(codecIndex));
    if (iter == indexedCodecs_.end()) {
        return nullptr;
    }

    return iter->second;
}

std::list<A2dpCodecConfig *> A2dpCodecFactory::GetIndexedSourceCodecs() const
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    return indexedSourceCodecs_;
}

std::list<A2dpCodecConfig *> A2dpCodecFactory::GetIndexedSinkCodecs() const
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    return indexedSinkCodecs_;
}

bool A2dpCodecFactory::SetCodecConfig(const uint8_t *peerCodecInfo, uint8_t *resultCodecInfo)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    A2dpCodecConfig *a2dpCodecConfig = FindSourceCodec(peerCodecInfo);
    if (a2dpCodecConfig == nullptr) {
        return false;
    }
    if (!a2dpCodecConfig->SetCodecConfig(peerCodecInfo, resultCodecInfo)) {
        return false;
    }

    currentCodecConfig_ = a2dpCodecConfig;
    return true;
}

bool A2dpCodecFactory::SetSinkCodecConfig(const uint8_t *peerCodecInfo, uint8_t *resultCodecInfo)
{
    bool ret = true;
    LOG_INFO("[CodecFactory] %{public}s isCapability\n", __func__);
    A2dpCodecConfig *a2dpCodecConfig = FindSinkCodec(peerCodecInfo);
    if (a2dpCodecConfig == nullptr) {
        return false;
    }

    if (!a2dpCodecConfig->SetCodecConfig(peerCodecInfo, resultCodecInfo)) {
        ret = false;
    }

    currentCodecConfig_ = a2dpCodecConfig;
    return ret;
}

bool A2dpCodecFactory::ResetCodecUserConfig(void)
{
    currentCodecConfig_ = lastCodecConfig_;
    return false;
}

void A2dpCodecFactory::UpdateCodecPriority(
    const A2dpCodecCapability &userConfig, A2dpCodecConfig *a2dpCodecConfig, bool *restartConfig)
{
    A2dpCodecPriority lastPriority = a2dpCodecConfig->GetCodecPriority();
    A2dpCodecPriority newPriority = userConfig.codecPriority_;
    LOG_INFO("[CodecFactory] %{public}s newpriority(%u) lastPriority(%u)\n", __func__, newPriority, lastPriority);
    a2dpCodecConfig->SetCodecPriority(newPriority);
    newPriority = a2dpCodecConfig->GetCodecPriority();
    LOG_INFO("[CodecFactory] %{public}s newpriority(%u) \n", __func__, newPriority);
    do {
        if (lastCodecConfig_ == nullptr) {  // have no previous codec, so update the current codec
            LOG_INFO("[CodecFactory] %{public}s update current codec config\n", __func__);
            currentCodecConfig_ = a2dpCodecConfig;
            *restartConfig = true;
            break;
        }

        if (lastCodecConfig_ == a2dpCodecConfig) {
            LOG_INFO("[CodecFactory] %{public}s codec not changed\n", __func__);
            if (newPriority == lastPriority) {
                LOG_INFO("[CodecFactory] %{public}s the priority not changed\n", __func__);
                break;
            }
            if (newPriority < lastPriority) {
                LOG_INFO("[CodecFactory] %{public}s the current priority is lower than before, "
                         "need select new codec\n",
                    __func__);
                *restartConfig = true;
                break;
            }
        }

        if (newPriority <= lastPriority) {
            LOG_INFO("[CodecFactory] %{public}s the new priority is lower than before, but not the current codec", __func__);
            break;
        }
        if (newPriority >= lastCodecConfig_->GetCodecPriority()) {
            LOG_INFO("[CodecFactory] %{public}s the new priority is higher than before, update current codec", __func__);
            currentCodecConfig_ = a2dpCodecConfig;
            lastCodecConfig_->SetDefaultCodecPriority();
            *restartConfig = true;
        }
    } while (false);
    LOG_INFO("[CodecFactory] %{public}s [index:%u\n", __func__, currentCodecConfig_->GetCodecIndex());
    if (currentCodecConfig_->GetCodecIndex() < A2DP_SOURCE_CODEC_INDEX_MAX) {
        indexedSourceCodecs_.sort(OrderCodecPriority);
    } else {
        indexedSinkCodecs_.sort(OrderCodecPriority);
    }
}

bool A2dpCodecFactory::SetCodecUserConfig(const A2dpCodecCapability &userConfig, const uint8_t *peerSinkCapabilities,
    uint8_t *resultCodecInfo, bool *restartConfig)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    A2dpCodecConfig *a2dpCodecConfig = nullptr;
    lastCodecConfig_ = currentCodecConfig_;

    if (userConfig.codecIndex_ < A2DP_CODEC_INDEX_MAX) {
        auto iter = indexedCodecs_.find(userConfig.codecIndex_);
        if (iter == indexedCodecs_.end()) {
            return ResetCodecUserConfig();
        }
        a2dpCodecConfig = iter->second;
    } else {
        a2dpCodecConfig = currentCodecConfig_;
    }

    if (a2dpCodecConfig == nullptr) {
        LOG_INFO("[CodecFactory] %{public}s a2dpCodecConfig is null\n", __func__);
        return ResetCodecUserConfig();
    }
    A2dpCodecCapability codecAudioConfig = a2dpCodecConfig->GetAudioCodecConfig();
    if (!a2dpCodecConfig->SetCodecUserConfig(
        userConfig, codecAudioConfig, peerSinkCapabilities, resultCodecInfo, restartConfig)) {
        LOG_INFO("[CodecFactory] %{public}s\n", __func__);
        return ResetCodecUserConfig();
    }

    UpdateCodecPriority(userConfig, a2dpCodecConfig, restartConfig);
    return true;
}

bool A2dpCodecFactory::SetCodecAudioConfig(const A2dpCodecCapability &audioConfig, const uint8_t *peerSinkCapabilities,
    uint8_t *resultCodecInfo, bool *restartConfig)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    A2dpCodecConfig *a2dpCodecConfig = currentCodecConfig_;
    *restartConfig = false;
    if (a2dpCodecConfig == nullptr) {
        return false;
    }
    A2dpCodecCapability codecUserConfig = a2dpCodecConfig->GetUserConfig();
    if (!a2dpCodecConfig->SetCodecUserConfig(
        codecUserConfig, audioConfig, peerSinkCapabilities, resultCodecInfo, restartConfig)) {
        LOG_INFO("[CodecFactory] %{public}s\n", __func__);
        return ResetCodecUserConfig();
    }
    return true;
}

bool A2dpCodecFactory::SetPeerSinkCodecCapabilities(const uint8_t *capabilities)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    bool isPeerSinkCodecValid = IsPeerSinkCodecValid(capabilities);
    if (!isPeerSinkCodecValid) {
        return false;
    }
    A2dpCodecConfig *a2dpCodecConfig = FindSinkCodec(capabilities);
    if (a2dpCodecConfig == nullptr) {
        return false;
    }

    return a2dpCodecConfig->SetPeerCodecCapabilities(capabilities);
}

bool A2dpCodecFactory::SetPeerSourceCodecCapabilities(const uint8_t *capabilities)
{
    LOG_INFO("[CodecFactory] %{public}s\n", __func__);
    bool isCodecValid = IsPeerSourceCodecValid(capabilities);
    if (!isCodecValid) {
        return false;
    }

    A2dpCodecConfig *a2dpCodecConfig = FindSourceCodec(capabilities);
    if (a2dpCodecConfig == nullptr) {
        return false;
    }

    return a2dpCodecConfig->SetPeerCodecCapabilities(capabilities);
}
}  // namespace bluetooth
}  // namespace OHOS