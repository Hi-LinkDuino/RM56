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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_XCOMPONENT_XCOMPONENT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_XCOMPONENT_XCOMPONENT_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {
struct XComponentAttribute : Attribute {
    std::string name;
    std::string id;
    std::string type;
    std::string libraryName;
};

struct XComponentEvent : Event {
    EventMarker xComponentInitEventId;
    EventMarker xComponentDestroyEventId;
};

class ACE_EXPORT XComponentDeclaration : public Declaration {
    DECLARE_ACE_TYPE(XComponentDeclaration, Declaration);

public:
    XComponentDeclaration() = default;
    ~XComponentDeclaration() override = default;

    void SetXComponentName(const std::string& name)
    {
        auto& attribute = MaybeResetAttribute<XComponentAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.name = name;
    }

    const std::string& GetXComponentName() const
    {
        auto& attribute = static_cast<XComponentAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.name;
    }

    void SetXComponentId(const std::string& id)
    {
        auto& attribute = MaybeResetAttribute<XComponentAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.id = id;
    }

    const std::string& GetXComponentId() const
    {
        auto& attribute = static_cast<XComponentAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.id;
    }

    void SetXComponentType(const std::string& type)
    {
        auto& attribute = MaybeResetAttribute<XComponentAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.type = type;
    }

    const std::string& GetXComponentType() const
    {
        auto& attribute = static_cast<XComponentAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.type;
    }

    void SetXComponentLibraryName(const std::string& libraryName)
    {
        auto& attribute = MaybeResetAttribute<XComponentAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.libraryName = libraryName;
    }

    const std::string& GetXComponentLibraryName() const
    {
        auto& attribute = static_cast<XComponentAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.libraryName;
    }

    void SetXComponentInitEventId(const EventMarker& xComponentInitEventId)
    {
        auto& event = MaybeResetEvent<XComponentEvent>(EventTag::SPECIALIZED_EVENT);
        event.xComponentInitEventId = xComponentInitEventId;
    }

    const EventMarker& GetXComponentInitEventId() const
    {
        auto& event = static_cast<XComponentEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.xComponentInitEventId;
    }

    void SetXComponentDestroyEventId(const EventMarker& xComponentDestroyEventId)
    {
        auto& event = MaybeResetEvent<XComponentEvent>(EventTag::SPECIALIZED_EVENT);
        event.xComponentDestroyEventId = xComponentDestroyEventId;
    }

    const EventMarker& GetXComponentDestroyEventId() const
    {
        auto& event = static_cast<XComponentEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.xComponentDestroyEventId;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_XCOMPONENT_XCOMPONENT_DECLARATION_H
