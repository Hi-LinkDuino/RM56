/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_DEFINE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_DEFINE_H

#include <cstdint>
#include <string>

namespace OHOS::Ace {

enum ModuleMask {
    VOICE_MASK = 0x0,
    BODY_LANGUAGE_MASK = 0x1,
    LOCATION_MASK = 0x2,
    TRADITIONAL_KEY_MASK = 0x4,
    TRADITIONAL_TP_MASK = 0x8,
    MASK_SHIFT = 3,
};

enum class MultimodalSourceType {
    SOURCE_INVALID = 0,
    SOURCE_VOICE,
    SOURCE_CV,
    SOURCE_SENSOR,
    SOURCE_KEY,
    SOURCE_TP,
    SOURCE_MOTION,
    SOURCE_TYPE_MAX,
};

struct VoiceLabel {
    std::string hotWord;
    std::string slot;
    int32_t action;
    std::string badge;
};

enum class MultimodalGeneralEvent : uint32_t {
    HOME = VOICE_MASK << MASK_SHIFT,
    BACK,
    OPEN_SETTING,
    CLOSE_SETTING,
    SWITCH_NIGHT_MODE,
    QUIT_NIGHT_MODE,
    SCREEN_CAPTURE,
    NEXT,
    PREV,
    NO_INTENT
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_DEFINE_H
