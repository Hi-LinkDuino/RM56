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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_RICHTEXT_RICHTEXT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_RICHTEXT_RICHTEXT_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct RichTextAttribute : Attribute {
    std::string data;
};

struct RichTextEvent : Event {
    EventMarker pageStartEventId;
    EventMarker pageFinishEventId;
    EventMarker pageErrorEventId;
};

class RichTextDeclaration : public Declaration {
    DECLARE_ACE_TYPE(RichTextDeclaration, Declaration);

public:
    RichTextDeclaration() = default;
    ~RichTextDeclaration() override = default;

    void SetData(const std::string& data)
    {
        auto& attribute = MaybeResetAttribute<RichTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.data = data;
    }

    const std::string& GetData() const
    {
        auto& attribute = static_cast<RichTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.data;
    }

    void SetPageStartedEventId(const EventMarker& pageStartedEventId)
    {
        auto& event = MaybeResetEvent<RichTextEvent>(EventTag::SPECIALIZED_EVENT);
        event.pageStartEventId = pageStartedEventId;
    }

    const EventMarker& GetPageStartedEventId() const
    {
        auto& event = static_cast<RichTextEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.pageStartEventId;
    }

    void SetPageFinishedEventId(const EventMarker& pageFinishedEventId)
    {
        auto& event = MaybeResetEvent<RichTextEvent>(EventTag::SPECIALIZED_EVENT);
        event.pageFinishEventId = pageFinishedEventId;
    }

    const EventMarker& GetPageFinishedEventId() const
    {
        auto& event = static_cast<RichTextEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.pageFinishEventId;
    }

    void SetPageErrorEventId(const EventMarker& pageErrorEventId)
    {
        auto& event = MaybeResetEvent<RichTextEvent>(EventTag::SPECIALIZED_EVENT);
        event.pageErrorEventId = pageErrorEventId;
    }

    const EventMarker& GetPageErrorEventId() const
    {
        auto& event = static_cast<RichTextEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.pageErrorEventId;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_RICHTEXT_RICHTEXT_DECLARATION_H
