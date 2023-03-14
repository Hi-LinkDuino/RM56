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

#ifdef VIDEO_SUPPORT

#define HST_LOG_TAG "AVCConfigDataParser"

#include "avc_config_data_parser.h"
#include <memory>
#include <cstring>
#include "bit_reader.h"
#include "foundation/log.h"
#include "securec.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
#define AVC_MIN_CONFIG_DATA_SIZE 7
#define AVC_NAL_HEADER_LEN 4

// refer to ISO/IEC 14496-15: AVC NAL size bytes should be 1, 2, 4
enum AVC_NAL_SIZE_LEN : int32_t {
    AVC_NAL_SIZE_LEN_1 = 1,
    AVC_NAL_SIZE_LEN_2 = 2,
    AVC_NAL_SIZE_LEN_4 = 4,
};

AVCConfigDataParser::AVCConfigDataParser(const uint8_t* cfgData, const size_t cfgDataSize)
    : bitReader_(cfgData, cfgDataSize),
      cfgData_(cfgData),
      cfgDataSize_(cfgDataSize),
      version_(1),
      profile_(0),
      profile_compat_(0),
      level_(0),
      needAddFrameHeader_(false)
{
    cfgSet.count = 0;
    newCfgData_ = nullptr;
    newCfgDataSize_ = 0;
    nalUnitLen_ = AVC_NAL_SIZE_LEN_4;
}

AVCConfigDataParser::~AVCConfigDataParser()
{
    ClearConfigSet();
}

bool AVCConfigDataParser::ParseConfigData()
{
    if (!ParseNal()) {
        if ((cfgDataSize_ < 3) || !((version_ == 0) && (profile_ == 0) && (profile_compat_ <= 1))) {  // 3
            needAddFrameHeader_ = true;
        }
        if (cfgDataSize_ == 0) {
            return false;
        }
        newCfgDataSize_ = cfgDataSize_;
        newCfgData_ = std::shared_ptr<uint8_t>(new uint8_t[newCfgDataSize_], std::default_delete<uint8_t[]>());
        if (newCfgData_ == nullptr) {
            MEDIA_LOG_E("Alloc new config data memory fail");
            return false;
        }
        (void)memcpy_s(newCfgData_.get(), cfgDataSize_, cfgData_, cfgDataSize_);
        return true;
    }

    needAddFrameHeader_ = true;
    if (cfgSet.count == 0) {
        return false;
    }
    for (uint32_t i = 0; i < cfgSet.count; ++i) {
        newCfgDataSize_ += (cfgSet.items[i]->len & 0xFFF);
    }
    newCfgData_ = std::shared_ptr<uint8_t>(new uint8_t[newCfgDataSize_], std::default_delete<uint8_t[]>());
    if (newCfgData_ == nullptr) {
        MEDIA_LOG_E("Alloc new config data memory fail");
        return false;
    }
    uint32_t usedLen = 0;
    for (uint32_t i = 0; i < cfgSet.count; ++i) {
        (void)memcpy_s(newCfgData_.get() + usedLen, cfgSet.items[i]->len, cfgSet.items[i]->SpsOrPps,
                       cfgSet.items[i]->len);
        usedLen += cfgSet.items[i]->len;
    }
    return true;
}

bool AVCConfigDataParser::IsNeedAddFrameHeader()
{
    return needAddFrameHeader_; // We can add this flag as specific header in newCfgData_, do it later
}

bool AVCConfigDataParser::GetNewConfigData(std::shared_ptr<uint8_t>& newCfgData, size_t& newCfgDataSize)
{
    if (newCfgData_ != nullptr) {
        newCfgData = newCfgData_;
        newCfgDataSize = newCfgDataSize_;
        return true;
    }

    return false;
}

void AVCConfigDataParser::ClearConfigSet()
{
    for (uint32_t i = 0; i < cfgSet.count; i++) {
        if (cfgSet.items[i]) {
            delete[] reinterpret_cast<uint8_t*>(cfgSet.items[i]);
        }
    }
}

bool AVCConfigDataParser::ParseNalUnitSizeLen()
{
    uint8_t sizeLen_ = 0;
    if (!bitReader_.ReadBits(1, sizeLen_)) {
        return false;
    }

    nalUnitLen_ = (sizeLen_ & 0x3) + 1; // lengthSize Minus One
    if ((nalUnitLen_ != AVC_NAL_SIZE_LEN_1) && (nalUnitLen_ != AVC_NAL_SIZE_LEN_2) &&
        (nalUnitLen_ != AVC_NAL_SIZE_LEN_4)) {
        MEDIA_LOG_I("Unsupported config data, nalUnitLen_: " PUBLIC_LOG_U8, nalUnitLen_);
        return false;
    }

    return true;
}

bool AVCConfigDataParser::GetSpsOrPpsLen(uint32_t& len)
{
    uint8_t tmp1, tmp2;
    if (!bitReader_.ReadBits(1, tmp1) || !bitReader_.ReadBits(1, tmp2)) {
        return false;
    }
    len = ((tmp1 << 8) | tmp2) & 0xFFFF;  // 8
    if (len > bitReader_.GetAvailableBits()) {
        MEDIA_LOG_E("len: " PUBLIC_LOG_U32 " is too large", len);
        return false;
    }
    return true;
}

bool AVCConfigDataParser::ParseNalHeader()
{
    if (bitReader_.GetAvailableBits() < AVC_MIN_CONFIG_DATA_SIZE) {
        MEDIA_LOG_E("Config data size is smaller than MIN: " PUBLIC_LOG_D32,
                    static_cast<int32_t>(AVC_MIN_CONFIG_DATA_SIZE));
        return false;
    }
    auto ret = bitReader_.ReadBits(1, version_); // configurationVersion = 1
    if ((ret == false) || (version_ != 1)) {
        // Some parser has parser config data, so just return
        MEDIA_LOG_I("Unsupported config data, version: " PUBLIC_LOG_U8, version_);
        return false;
    }
    if (!bitReader_.ReadBits(1, profile_)) { // AVCProfileIndication
        return false;
    }
    if (!bitReader_.ReadBits(1, profile_compat_)) { // profile_compatibility
        return false;
    }
    if (!bitReader_.ReadBits(1, level_)) { // AVCLevelIndication
        return false;
    }
    if (!ParseNalUnitSizeLen()) {
        return false;
    }
    return true;
}

bool AVCConfigDataParser::CreateConfigSetItem(const uint32_t len)
{
    uint32_t itemLen = static_cast<uint32_t>(sizeof(NalConfigItem)) + AVC_NAL_HEADER_LEN + len;
    cfgSet.items[cfgSet.count] = reinterpret_cast<NalConfigItem*>(new (std::nothrow) uint8_t[itemLen]);
    if (cfgSet.items[cfgSet.count] == nullptr) {
        MEDIA_LOG_E("Alloc config set item memory fail");
        return false;
    }
    auto& item = cfgSet.items[cfgSet.count];
    // Add start code: 0x00000001
    item->SpsOrPps[0] = 0;
    item->SpsOrPps[1] = 0;
    item->SpsOrPps[2] = 0;  // 2
    item->SpsOrPps[3] = 1;  // 3
    item->len = AVC_NAL_HEADER_LEN + len;
    return true;
}

bool AVCConfigDataParser::ParseSpsOrPps(const uint32_t mask)
{
    uint32_t setCount = 0;
    if (!bitReader_.ReadBits(1, setCount)) {
        return false;
    }
    setCount &= mask;
    for (uint32_t idx = 0; idx < setCount; ++idx) {
        if (bitReader_.GetAvailableBits() < 2) { // 2
            MEDIA_LOG_E("Sps data err");
            return false;
        }
        uint32_t len = 0;
        if (!GetSpsOrPpsLen(len)) {
            MEDIA_LOG_E("Get sps/pps len fail");
            return false;
        }
        if (cfgSet.count >= AVC_MAX_CONFIG_ITEM) {
            MEDIA_LOG_E("config set count is larger than: " PUBLIC_LOG_D32,
                        static_cast<int32_t>(AVC_MAX_CONFIG_ITEM));
            return false;
        }
        if (!CreateConfigSetItem(len)) {
            MEDIA_LOG_E("Create config set item fail");
            return false;
        }
        (void)memcpy_s(&cfgSet.items[cfgSet.count]->SpsOrPps[AVC_NAL_HEADER_LEN], static_cast<size_t>(len),
                       bitReader_.GetCurrentPtr(), static_cast<size_t>(len));
        cfgSet.count++;
        bitReader_.SkipBits(len);
    }
    return true;
}

bool AVCConfigDataParser::ParseNal()
{
    if (!ParseNalHeader()) {
        MEDIA_LOG_D("Parse NAL header fail");
        return false;
    }
    // Parse SPS
    if (!ParseSpsOrPps(0x1F)) {
        MEDIA_LOG_E("Parse SPS fail");
        return false;
    }
    if (bitReader_.GetAvailableBits() < 1) {
        return false;
    }
    // Parse PPS
    if (!ParseSpsOrPps(0xFF)) {
        MEDIA_LOG_E("Parse PPS fail");
        return false;
    }
    return true;
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
