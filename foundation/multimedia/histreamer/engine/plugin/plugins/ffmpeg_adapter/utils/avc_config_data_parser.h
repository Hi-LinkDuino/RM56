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

#ifndef HISTREAMER_AVC_CONFIG_DATA_PARSER_H
#define HISTREAMER_AVC_CONFIG_DATA_PARSER_H

#ifdef VIDEO_SUPPORT

#include <cstdint>
#include <string>
#include <memory>
#include "bit_reader.h"
#include "common/plugin_audio_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
#define AVC_MAX_CONFIG_ITEM 256

class AVCConfigDataParser final {
public:
    AVCConfigDataParser(const uint8_t* cfgData, const size_t cfgDataSize);
    ~AVCConfigDataParser();

    bool ParseConfigData();
    bool IsNeedAddFrameHeader();
    bool GetNewConfigData(std::shared_ptr<uint8_t>& newCfgData, size_t& newCfgDataSize);

private:
    // sps or pps
    struct NalConfigItem {
        uint32_t len;
        uint8_t SpsOrPps[0];
    };

    struct NalConfigList {
        uint32_t count;
        NalConfigItem *items[AVC_MAX_CONFIG_ITEM];
    };

    NalConfigList cfgSet;

    BitReader bitReader_;
    const uint8_t* cfgData_;
    const size_t cfgDataSize_;

    uint8_t nalUnitLen_;
    uint8_t version_;
    uint8_t profile_;
    uint8_t profile_compat_;
    uint8_t level_;
    bool needAddFrameHeader_;

    std::shared_ptr<uint8_t> newCfgData_;
    size_t newCfgDataSize_;

    void ClearConfigSet();
    bool ParseNalUnitSizeLen();
    bool GetSpsOrPpsLen(uint32_t& len);
    bool ParseNalHeader();
    bool CreateConfigSetItem(const uint32_t len);
    bool ParseSpsOrPps(const uint32_t mask);
    bool ParseNal();
};
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
#endif // HISTREAMER_AVC_CONFIG_DATA_PARSER_H
