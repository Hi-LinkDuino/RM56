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

#include "avrcp_media.h"
#include "string"
#include "vector"

namespace OHOS {
namespace bluetooth {
AvrcMpItem::AvrcMpItem(uint8_t itemType, uint16_t playerId, uint8_t majorType, uint32_t subType, uint8_t playStatus,
    const std::vector<uint8_t> &features, const std::string &name)
    : itemType_(itemType),
      playerId_(playerId),
      majorType_(majorType),
      subType_(subType),
      playStatus_(playStatus),
      features_(features),
      name_(name)
{}
AvrcMeItem::AvrcMeItem(uint8_t itemType, uint64_t uid, uint8_t type, uint8_t playable, const std::string &name,
    const std::vector<uint32_t> &attributes, const std::vector<std::string> &values)
    : itemType_(itemType),
      uid_(uid),
      type_(type),
      playable_(playable),
      name_(name),
      attributes_(attributes),
      values_(values)
{}
}  // namespace bluetooth
}  // namespace OHOS