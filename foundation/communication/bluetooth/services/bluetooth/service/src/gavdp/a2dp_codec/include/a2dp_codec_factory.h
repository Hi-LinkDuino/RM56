/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef A2DP_CODEC_FACTORY_H
#define A2DP_CODEC_FACTORY_H

#include <cstdint>
#include <list>
#include <map>

#include "a2dp_codec_config.h"
#include "a2dp_codec_constant.h"
#include "a2dp_codec_wrapper.h"
#include "base_def.h"

namespace OHOS {
namespace bluetooth {
// A2dpCodecFactory manage all codec
class A2dpCodecFactory {
public:
    explicit A2dpCodecFactory(const std::map<A2dpCodecIndex, A2dpCodecPriority> &codecPriorities);
    ~A2dpCodecFactory();

    bool Initialize();
    A2dpCodecConfig *GetCodecConfig() const;
    A2dpCodecConfig *FindSourceCodec(const uint8_t *codeInfo) const;
    A2dpCodecConfig *FindSinkCodec(const uint8_t *codeInfo) const;
    std::list<A2dpCodecConfig *> GetIndexedSourceCodecs() const;
    std::list<A2dpCodecConfig *> GetIndexedSinkCodecs() const;

    bool SetCodecUserConfig(const A2dpCodecCapability &userConfig, const uint8_t *peerSinkCapabilities,
        uint8_t *resultCodecInfo, bool *restartConfig);
    bool SetSinkCodecConfig(const uint8_t *peerCodecInfo, uint8_t *resultCodecInfo);
    bool SetCodecAudioConfig(const A2dpCodecCapability &audioConfig, const uint8_t *peerSinkCapabilities,
        uint8_t *resultCodecInfo, bool *restartConfig);
    bool SetCodecConfig(const uint8_t *peerCodecInfo, uint8_t *resultCodecInfo);
    bool SetPeerSinkCodecCapabilities(const uint8_t *capabilities);
    bool SetPeerSourceCodecCapabilities(const uint8_t *capabilities);

private:
    BT_DISALLOW_COPY_AND_ASSIGN(A2dpCodecFactory);
    bool ResetCodecUserConfig(void);
    void UpdateCodecPriority(
        const A2dpCodecCapability &userConfig, A2dpCodecConfig *a2dpCodecConfig, bool *restartConfig);
    // A2DP Source codecs ordered by codec index
    std::map<A2dpCodecIndex, A2dpCodecConfig *> indexedCodecs_ {};
    std::map<A2dpCodecIndex, A2dpCodecConfig *> disabledCodecs_ {};
    std::map<A2dpCodecIndex, A2dpCodecPriority> codecPriorities_;
    A2dpCodecConfig *currentCodecConfig_;  // Currently selected codec
    A2dpCodecConfig *lastCodecConfig_;     // last selected codec
    // A2DP Source codecs ordered by priority
    std::list<A2dpCodecConfig *> indexedSourceCodecs_ {};
    // A2DP Sink codecs ordered by priority
    std::list<A2dpCodecConfig *> indexedSinkCodecs_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_CODEC_FACTORY_H
