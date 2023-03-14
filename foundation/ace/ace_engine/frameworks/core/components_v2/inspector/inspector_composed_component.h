/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_INSPECTOR_COMPOSED_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_INSPECTOR_COMPOSED_COMPONENT_H

#include <functional>
#include <string>

#include "core/accessibility/accessibility_manager.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT InspectorFunctionImpl : public AceType {
    DECLARE_ACE_TYPE(InspectorFunctionImpl, AceType);

public:
    InspectorFunctionImpl() = default;
    ~InspectorFunctionImpl() override = default;

    void UpdateEventInfo(BaseEventInfo& info)
    {
        if (updateEventInfoimpl_) {
            updateEventInfoimpl_(info);
        }
    }

    void SetUpdateEventInfoImpl(const std::function<void(BaseEventInfo&)>& impl)
    {
        updateEventInfoimpl_ = impl;
    }

private:
    std::function<void(BaseEventInfo&)> updateEventInfoimpl_;
};

class ACE_EXPORT InspectorComposedComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(InspectorComposedComponent, ComposedComponent);

public:
    using ComposedComponent::ComposedComponent;
    ~InspectorComposedComponent() override = default;

    RefPtr<Element> CreateElement() override;
    bool IsInspector() override
    {
        return true;
    }
    void AddElementToAccessibilityManager(const RefPtr<ComposedElement>& composedElement);

    void SetAccessibilityGroup(bool accessibilitygroup)
    {
        accessibilitygroup_ = accessibilitygroup;
    }

    void SetAccessibilitytext(const std::string& accessibilitytext)
    {
        accessibilitytext_ = accessibilitytext;
    }

    void SetAccessibilityDescription(const std::string& accessibilitydescription)
    {
        accessibilitydescription_ = accessibilitydescription;
    }

    void SetAccessibilityImportance(const std::string& accessibilityimportance)
    {
        accessibilityimportance_ = accessibilityimportance;
    }

    void SetAccessibilityEvent(const EventMarker& accessibilityEvent)
    {
        accessibilityEvent_ = accessibilityEvent;
    }

    bool IsAccessibilityGroup() const
    {
        return accessibilitygroup_;
    }

    const std::string& GetAccessibilitytext() const
    {
        return accessibilitytext_;
    }

    const std::string& GetAccessibilityDescription() const
    {
        return accessibilitydescription_;
    }

    const std::string& GetAccessibilityImportance() const
    {
        return accessibilityimportance_;
    }

    const EventMarker& GetAccessibilityEvent() const
    {
        return accessibilityEvent_;
    }

    const RefPtr<InspectorFunctionImpl>& GetInspectorFunctionImpl() const
    {
        return inspectorFunctionImpl_;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void SetDebugLine(std::string debugLine)
    {
        debugLine_ = debugLine;
    }

    std::string GetDebugLine()
    {
        return debugLine_;
    }
#endif

    static bool HasInspectorFinished(std::string tag);
    static std::string GetEtsTag(const std::string& tag);
    static RefPtr<AccessibilityManager> GetAccessibilityManager();
    static RefPtr<AccessibilityNode> CreateAccessibilityNode(
        const std::string& tag, int32_t nodeId, int32_t parentNodeId, int32_t itemIndex);

private:
    bool accessibilitygroup_ = false;
    std::string accessibilitytext_ = "";
    std::string accessibilitydescription_ = "";
    std::string accessibilityimportance_ = "";
    EventMarker accessibilityEvent_;
    RefPtr<InspectorFunctionImpl> inspectorFunctionImpl_ = MakeRefPtr<InspectorFunctionImpl>();

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string debugLine_;
#endif
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_INSPECTOR_COMPOSED_COMPONENT_H
