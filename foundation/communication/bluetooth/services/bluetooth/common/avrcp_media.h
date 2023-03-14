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

#ifndef AVRCP_MEDIA_H
#define AVRCP_MEDIA_H

#include <stdint.h>
#include <string>
#include <vector>
#include "cstdint"
#include "iosfwd"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of attributes for saving the packet of the <b>Media player item</b>.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.1 Media player item.
 *
 * @since 6.0
 */
class AvrcMpItem {
public:
    AvrcMpItem(uint8_t itemType, uint16_t playerId, uint8_t majorType, uint32_t subType, uint8_t playStatus,
        const std::vector<uint8_t> &features, const std::string &name);

    AvrcMpItem() = default;
    AvrcMpItem(const AvrcMpItem& other) = default;
    ~AvrcMpItem() = default;

    uint8_t itemType_;               // The value of the "Item Type ".
    uint16_t playerId_;              // The value of the "Player Id".
    uint8_t majorType_;              // The value of the "Major Player Type". Refer to <b>AvrcMediaMajorPlayerType</b>.
    uint32_t subType_;               // The value of the "Player Sub Type". Refer to <b>AvrcMediaPlayerSubType</b>.
    uint8_t playStatus_;             // The value of the "Play Status". Refer to <b>AvrcPlayStatus</b>.
    std::vector<uint8_t> features_;  // The value of the "Feature Bit Mask".
    std::string name_;               // The value of the "Displayable Name".
};

/**
 * @brief This class provides a set of attributes for saving the packet of the <b>Media player item</b>.
 * @see Audio/Video Remote Control 1.6.2 Section 6.10.2.2 Folder item and Section 6.10.2.3 Media element item.
 *
 * @since 6.0
 */
class AvrcMeItem {
public:
    AvrcMeItem(uint8_t itemType, uint64_t uid, uint8_t type, uint8_t playable, const std::string &name,
        const std::vector<uint32_t> &attributes, const std::vector<std::string> &values);
    AvrcMeItem() = default;
    AvrcMeItem(const AvrcMeItem& other) = default;
    ~AvrcMeItem() = default;

    uint8_t itemType_;  // The value of the "Item Type ".
    // The value of the "Folder UID" and the "Media Element UID".
    uint64_t uid_;
    // The value of the "Folder Type" and the "Media Type". Refer to <b>AvrcMediaFolderType</b> and
    // <b>AvrcMediaElementType</b>.
    uint8_t type_;
    // The value of the "Is Playable". Refer to <b>AvrcMediaFolderPlayable</b>.
    uint8_t playable_;
    // The value of the "Displayable Name".
    std::string name_;
    // The list of the "Attribute ID".  Refer to <b>AvrcMediaAttribute</b>.
    std::vector<uint32_t> attributes_;
    // The list of the "Attribute Value".
    std::vector<std::string> values_;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // AVRCP_MEDIA_H