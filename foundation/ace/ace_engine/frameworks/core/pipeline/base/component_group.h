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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_GROUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_GROUP_H

#include <list>

#include "core/pipeline/base/component_group_element.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class ComponentGroup : public RenderComponent {
    DECLARE_ACE_TYPE(ComponentGroup, RenderComponent);

public:
    ComponentGroup() = default;
    explicit ComponentGroup(const std::list<RefPtr<Component>>& children) : children_(children)
    {
        InitChildren();
    }
    ~ComponentGroup() override = default;

    virtual void OnChildInserted(const RefPtr<Component>& child, int32_t position) {}
    virtual void OnChildAppended(const RefPtr<Component>& child) {}

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<ComponentGroupElement>();
    }

    void InitChildren()
    {
        for (const auto& child : children_) {
            child->SetParent(WeakClaim(this));
        }
    }

    const std::list<RefPtr<Component>>& GetChildren() const
    {
        return children_;
    }

    void ClearChildren()
    {
        for (const auto& child : children_) {
            child->SetParent(nullptr);
        }
        children_.clear();
    }

    virtual void InsertChild(int32_t position, const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }
        child->SetParent(WeakClaim(this));
        auto insertIter = children_.begin();
        std::advance(insertIter, position);
        children_.insert(insertIter, child);
        OnChildInserted(child, position);
    }

    virtual void AppendChild(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }
        child->SetParent(WeakClaim(this));
        children_.emplace_back(child);
        OnChildAppended(child);
    }

    void AppendChildDirectly(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }
        child->SetParent(WeakClaim(this));
        children_.emplace_back(child);
    }

    virtual void RemoveChildDirectly(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }

        child->SetParent(nullptr);
        children_.pop_back();
        return;
    }

    virtual void RemoveChildByComposedId(const ComposeId& composeId)
    {
        for (const auto& item : children_) {
            auto compose = AceType::DynamicCast<ComposedComponent>(item);
            if (compose && composeId == compose->GetId()) {
                auto child = compose->GetChild();
                if (child) {
                    child->SetParent(nullptr);
                }
                children_.remove(item);
                return;
            }
        }
    }

    virtual void RemoveChild(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }
        auto composedChild = AceType::DynamicCast<ComposedComponent>(child);
        if (!composedChild) {
            LOGW("get composed component failed");
            return;
        }
        auto composeId = composedChild->GetId();
        for (const auto& item : children_) {
            auto compose = AceType::DynamicCast<ComposedComponent>(item);
            if (compose && composeId == compose->GetId()) {
                child->SetParent(nullptr);
                children_.remove(item);
                return;
            }
        }
    }

    void SetUpdateType(UpdateType updateType) override
    {
        if (GetUpdateType() == updateType) {
            return;
        }
        RenderComponent::SetUpdateType(updateType);
        for (const auto& child : children_) {
            child->SetUpdateType(updateType);
        }
    }

    void SetDisabledStatus(bool disabledStatus) override
    {
        if (IsDisabledStatus() == disabledStatus) {
            return;
        }
        RenderComponent::SetDisabledStatus(disabledStatus);
        for (const auto& child : children_) {
            child->SetDisabledStatus(disabledStatus);
        }
    }

    void SetTextDirection(TextDirection direction) override
    {
        RenderComponent::SetTextDirection(direction);
        for (const auto& child : children_) {
            child->SetTextDirection(direction);
        }
    }

    void ReverseChildren()
    {
        children_.reverse();
    }

private:
    std::list<RefPtr<Component>> children_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPONENT_GROUP_H
