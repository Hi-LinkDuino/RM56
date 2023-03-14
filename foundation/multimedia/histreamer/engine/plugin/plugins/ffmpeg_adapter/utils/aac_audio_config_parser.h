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

#ifndef HISTREAMER_AAC_AUDIO_CONFIG_PARSER_H
#define HISTREAMER_AAC_AUDIO_CONFIG_PARSER_H

#include <cstdint>
#include <string>
#include "bit_reader.h"
#include "common/plugin_audio_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class AACAudioConfigParser final {
public:
    AACAudioConfigParser(const uint8_t* audioConfig, size_t len);

    ~AACAudioConfigParser() = default;

    bool ParseConfigs();

    uint32_t GetLevel() const;

    AudioAacProfile GetProfile() const;

private:
    struct AudioSpecificConfig {
        uint8_t audioObjectType {0xFF};
        uint8_t channelConfig {0xFF};
        uint32_t sampleRate {0};
        uint32_t level {0};
        AudioAacProfile profile {AudioAacProfile::NONE};
    };

    void Reset();

    bool ParseObjectType();

    bool ParseSampleRate();

    bool ParseChannel();

    bool ParseObjectTypeFull();

    bool ExtractChannelElements(int& sceCnt, int& cpeCnt, int& lfeCnt, int& indepCce, int& depCce);

    bool ExtractReferencePRCU(int& refPcu, int& refRcu);

    bool CalculateProfile(int channelCnt, int pcu, int rcu);

    bool ParseLevel();

    bool ParseProfile();

    BitReader bitReader_;
    bool isConfigValid_;
    AudioSpecificConfig audioConfig_;
};
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_AAC_AUDIO_CONFIG_PARSER_H
