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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_H

#include <array>
#include <list>
#include <map>
#include <memory>
#include <utility>

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/animation/property_animation.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/extensions/events/event_extensions.h"
#include "core/event/ace_event_handler.h"

namespace OHOS::Ace {

class Element;
class ComposedElement;
class SingleChild;

using ElementFunction = std::function<void(const RefPtr<ComposedElement>&)>;

enum class UpdateType {
    STYLE,
    ATTR,
    EVENT,
    METHOD,
    REBUILD,
    ALL,
};

enum class UpdateRenderType : uint32_t {
    NONE = 0,
    LAYOUT = 1,
    PAINT = 1 << 1,
    EVENT = 1 << 2
};

enum class HoverAnimationType : int32_t {
    NONE,
    OPACITY,
    SCALE,
    BOARD,
    AUTO,
};

// Component is a read-only structure, represent the basic information how to display it.
class ACE_EXPORT Component : public virtual AceType {
    DECLARE_ACE_TYPE(Component, AceType);

public:
    Component();
    ~Component() override;

    virtual RefPtr<Element> CreateElement() = 0;

    TextDirection GetTextDirection() const
    {
        return direction_;
    }

    virtual void SetTextDirection(TextDirection direction)
    {
        direction_ = direction;
    }

    virtual void SetInspectorTag(std::string inspectorTag)
    {
        inspectorTag_ = inspectorTag;
    }

    virtual std::string GetInspectorTag()
    {
        return inspectorTag_;
    }

    UpdateType GetUpdateType() const
    {
        return updateType_;
    }

    virtual void SetUpdateType(UpdateType updateType)
    {
        updateType_ = updateType;
    }

    void SetParent(const WeakPtr<Component>& parent)
    {
        parent_ = parent;
    }

    const WeakPtr<Component>& GetParent() const
    {
        return parent_;
    }

    bool IsDisabledStatus() const
    {
        return disabledStatus_;
    }
    virtual void SetDisabledStatus(bool disabledStatus)
    {
        disabledStatus_ = disabledStatus;
    }
    bool IsTouchable() const
    {
        return touchable_;
    }
    void SetTouchable(bool touchable)
    {
        touchable_ = touchable;
    }

    void SetRetakeId(int32_t retakeId);
    int32_t GetRetakeId() const;

    virtual bool HasElementFunction()
    {
        return false;
    }

    virtual void SetElementFunction(ElementFunction&& func) {}
    virtual void CallElementFunction(const RefPtr<Element>& element) {}

    bool IsStatic()
    {
        return static_;
    }

    void SetStatic()
    {
        static_ = true;
    }

    void AddAnimatable(AnimatableType type, const RefPtr<PropertyAnimation> animation)
    {
        propAnimations_[type] = animation;
    }

    void ClearAnimatables()
    {
        propAnimations_.clear();
    }

    const PropAnimationMap& GetAnimatables() const
    {
        return propAnimations_;
    }

    void SetOnAppearEventId(const EventMarker& appearEventId)
    {
        appearEventId_ = appearEventId;
    }

    const EventMarker& GetAppearEventMarker() const
    {
        return appearEventId_;
    }

    void SetOnDisappearEventId(const EventMarker& disappearEventId)
    {
        disappearEventId_ = disappearEventId;
    }

    const EventMarker& GetDisappearEventMarker() const
    {
        return disappearEventId_;
    }

    virtual void OnWrap() {}

    bool IsHeadComponent() const { return isHeadComponent_; }
    bool IsTailComponent() const { return isTailComponent_; }
    static void MergeRSNode(const std::vector<RefPtr<Component>>& components);
    static void MergeRSNode(const std::vector<RefPtr<SingleChild>>& components);
    static void MergeRSNode(const std::vector<RefPtr<SingleChild>>& components, const RefPtr<Component>& mainComponent);
    static void MergeRSNode(const RefPtr<Component>& head, const RefPtr<Component>& tail);
    static void MergeRSNode(const RefPtr<Component>& standaloneNode);
    static void ExtendRSNode(const RefPtr<Component>& newHead, const RefPtr<Component>& prevHead);

    virtual uint32_t Compare(const RefPtr<Component>& component) const
    {
        return static_cast<uint32_t>(UpdateRenderType::LAYOUT);
    }

    void SetIgnoreInspector(bool ignoreInspector)
    {
        ignoreInspector_ = ignoreInspector;
    }

    bool IsIgnoreInspector() const
    {
        return ignoreInspector_;
    }

    RefPtr<V2::EventExtensions> GetEventExtensions()
    {
        if (!eventExtensions_) {
            eventExtensions_ = MakeRefPtr<V2::EventExtensions>();
        }
        return eventExtensions_;
    }

    bool HasEventExtensions() const
    {
        return eventExtensions_;
    }

    const std::string& GetInspectorKey() const
    {
        return inspectorKey_;
    }
    void SetInspectorKey(const std::string& inspectorKey)
    {
        inspectorKey_ = inspectorKey;
    }

    int32_t GetRestoreId() const
    {
        return restoreId_;
    }

    void SetRestoreId(int32_t restoreId)
    {
        restoreId_ = restoreId;
    }

protected:
    TextDirection direction_ = TextDirection::LTR;

private:
    bool ignoreInspector_ = false;
    PropAnimationMap propAnimations_;
    UpdateType updateType_ = UpdateType::ALL;
    WeakPtr<Component> parent_;
    bool disabledStatus_ = false;
    bool touchable_ = true;
    static std::atomic<int32_t> key_;
    // Set the id for the component to identify the unique component.
    int32_t retakeId_ = 0;
    bool static_ = false;
    std::string inspectorKey_;
    // eventMarker used to handle component detach and attach to the render tree.
    EventMarker appearEventId_;
    EventMarker disappearEventId_;
    RefPtr<V2::EventExtensions> eventExtensions_;
    bool isHeadComponent_ = false;
    bool isTailComponent_ = false;
    std::string inspectorTag_;
    int32_t restoreId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_H
