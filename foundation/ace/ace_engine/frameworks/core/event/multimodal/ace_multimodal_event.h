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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_EVENT_H

#include "core/event/multimodal/ace_multimodal_define.h"

namespace OHOS::Ace {

class AceMultimodalEvent final {
public:
    MultimodalSourceType GetSourceType() const
    {
        return sourceType_;
    }

    void SetSourceType(MultimodalSourceType sourceType)
    {
        sourceType_ = sourceType;
    }

    const VoiceLabel& GetVoice() const
    {
        return voice_;
    }

    void SetVoice(const VoiceLabel& voice)
    {
        voice_ = voice;
    }

    MultimodalGeneralEvent GetMultimodalGeneralEvent() const
    {
        return multimodalGeneralEvent_;
    }

    void SetMultimodalGeneralEvent(MultimodalGeneralEvent multimodalGeneralEvent)
    {
        multimodalGeneralEvent_ = multimodalGeneralEvent;
    }

private:
    VoiceLabel voice_;
    MultimodalSourceType sourceType_ = MultimodalSourceType::SOURCE_VOICE;
    MultimodalGeneralEvent multimodalGeneralEvent_ = MultimodalGeneralEvent::NO_INTENT;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_ACE_MULTIMODAL_EVENT_H
