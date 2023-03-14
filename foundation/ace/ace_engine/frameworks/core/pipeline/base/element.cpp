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

#include "core/pipeline/base/element.h"

#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "core/common/frontend.h"
#include "core/components/focus_animation/focus_animation_element.h"
#include "core/components/page/page_element.h"
#include "core/components/shadow/shadow_element.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

Element::~Element()
{
    for (const auto& child : children_) {
        DetachChild(child);
    }
}

void Element::AddChild(const RefPtr<Element>& child, int32_t slot)
{
    if (!child) {
        return;
    }

    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        LOGW("Child element is already existed");
        return;
    }

    it = children_.begin();
    std::advance(it, slot);
    children_.insert(it, child);

    child->SetSlot(slot);
    Apply(child);
}

void Element::RemoveChild(const RefPtr<Element>& child)
{
    if (child) {
        DetachChild(child);
        children_.remove(child);
    }
}

RefPtr<Element> Element::GetChildBySlot(int32_t slot)
{
    for (auto iter = children_.begin(); iter != children_.end(); iter++) {
        if (slot == (*iter)->GetSlot()) {
            return (*iter);
        }
    }
    return nullptr;
}

void Element::ChangeChildSlot(const RefPtr<Element>& child, int32_t slot)
{
    child->SetSlot(slot);

    if (slot < 0) {
        return;
    }

    auto it = children_.end();
    if (static_cast<size_t>(slot) < children_.size()) {
        it = children_.begin();
        std::advance(it, slot);
        if (*it == child) {
            // Already at the right place
            return;
        }

        auto itChild = std::find(it, children_.end(), child);
        if (itChild != children_.end()) {
            children_.erase(itChild);
        } else {
            LOGW("Should NOT be here");
            children_.remove(child);
            ++it;
        }
    } else {
        children_.remove(child);
    }
    children_.insert(it, child);
}

void Element::ChangeChildRenderSlot(const RefPtr<Element>& child, int32_t renderSlot, bool effectDescendant)
{
    child->SetRenderSlot(renderSlot);

    if (renderSlot < 0) {
        return;
    }

    if (child->GetType() == RENDER_ELEMENT) {
        auto renderNode = child->GetRenderNode();
        renderNode->MovePosition(renderSlot);
    } else if (child->GetType() == COMPOSED_ELEMENT && effectDescendant) {
        int32_t newRenderSlot = renderSlot;
        for (const auto& grandChild : child->children_) {
            child->ChangeChildRenderSlot(grandChild, newRenderSlot, effectDescendant);
            newRenderSlot += grandChild->CountRenderNode();
        }
    }
}

void Element::DeactivateChild(RefPtr<Element> child)
{
    if (child && !child->parent_.Invalid()) {
        child->parent_ = nullptr;
        RefPtr<PipelineContext> context = context_.Upgrade();
        if (context) {
            context->AddDeactivateElement(child->GetRetakeId(), child);
        }
        auto focusNode = AceType::DynamicCast<FocusNode>(child);
        if (focusNode) {
            focusNode->RemoveSelf();
        }
        child->Deactivate();
        child->MarkActive(false);
        children_.remove(child);
    }
}

void Element::DetachChild(const RefPtr<Element>& child)
{
    auto focusNode = AceType::DynamicCast<FocusNode>(child);
    if (focusNode) {
        focusNode->RemoveSelf();
    }
    child->Detached();
}

void Element::Rebuild()
{
    if (!needRebuild_) {
        return;
    }

    needRebuild_ = false;

    // When rebuild comes, newComponent_ should not be null, and will go to these 3 steps:
    // 1. Update self using new component
    // 2. PerformBuild will build and update child recursively
    // 3. Finish update and release the new component
    Update();
    PerformBuild();
    SetNewComponent(nullptr);
}

void Element::DumpTree(int32_t depth)
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        Dump();
        DumpLog::GetInstance().AddDesc(std::string("retakeID: ").append(std::to_string(GetRetakeId())));
        DumpLog::GetInstance().AddDesc(std::string("Active: ").append(IsActive() ? "Y" : "N"));
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children_.size());
    }

    for (const auto& item : children_) {
        item->DumpTree(depth + 1);
    }
}

void Element::Dump() {}

bool Element::CanUpdate(const RefPtr<Component>& newComponent)
{
    // The raw ptr is persistent during app process.
    return componentTypeId_ == AceType::TypeId(newComponent);
}

inline RefPtr<Element> Element::DoUpdateChildWithNewComponent(
    const RefPtr<Element>& child, const RefPtr<Component>& newComponent, int32_t slot, int32_t renderSlot)
{
    ChangeChildSlot(child, slot);
    // Only change render slot for itself, the descendant node will update by 'Rebuild'
    ChangeChildRenderSlot(child, renderSlot, false);
    child->SetNewComponent(newComponent);
    child->Rebuild();
    return child;
}

RefPtr<Element> Element::UpdateChildWithSlot(
    const RefPtr<Element>& child, const RefPtr<Component>& newComponent, int32_t slot, int32_t renderSlot)
{
    LOGD("%{public}p->%{public}s::UpdateChildWithSlot(%{public}s, %{public}s, %{public}d, %{public}d)",
        this, AceType::TypeName(this), AceType::TypeName(child), AceType::TypeName(newComponent), slot, renderSlot);

    // Considering 4 cases:
    // 1. child == null && newComponent == null  -->  do nothing
    // 2. child == null && newComponent != null  -->  create new child configured with newComponent
    if (!child) {
        return !newComponent ? nullptr : InflateComponent(newComponent, slot, renderSlot);
    }

    // 3. child != null && newComponent == null  -->  remove old child
    if (!newComponent) {
        DeactivateChild(child);
        return nullptr;
    }

    // 4. child != null && newComponent != null  -->  update old child with new configuration if possible(determined by
    //    [Element::CanUpdate]), or remove the old child and create new one configured with newComponent.
    auto context = context_.Upgrade();
    if (!child->CanUpdate(newComponent)) {
        // Can not update

        auto needRebuildFocusElement = AceType::DynamicCast<Element>(GetFocusScope());
        if (context && needRebuildFocusElement) {
            context->AddNeedRebuildFocusElement(needRebuildFocusElement);
        }
        DeactivateChild(child);
        return InflateComponent(newComponent, slot, renderSlot);
    }

    if (!context || !context->GetIsDeclarative()) {
        // Non-declarative code path
        return DoUpdateChildWithNewComponent(child, newComponent, slot, renderSlot);
    }

    // Declarative path
    if (newComponent->IsStatic()) {
        // Declarative && Component marked as static
        ChangeChildSlot(child, slot);
        // Not need to rebuild child, but should change render slot for all descendant
        ChangeChildRenderSlot(child, renderSlot, true);
        return child;
    }

    // Non-static component
    if (newComponent->HasElementFunction()) {
        newComponent->CallElementFunction(child);
    }
    return DoUpdateChildWithNewComponent(child, newComponent, slot, renderSlot);
}

void Element::Mount(const RefPtr<Element>& parent, int32_t slot, int32_t renderSlot)
{
    MarkActive(true);
    Activate();
    SetParent(parent);
    SetDepth(parent != nullptr ? parent->GetDepth() + 1 : 1);
    SetPipelineContext(parent != nullptr ? parent->context_ : context_);
    Prepare(parent);
    SetRenderSlot(renderSlot);
    if (parent) {
        parent->AddChild(AceType::Claim(this), slot);
        AddToFocus();
    }
    Rebuild();
    OnMount();
}

void Element::AddToFocus()
{
    auto parent = parent_.Upgrade();
    if (!parent) {
        return;
    }
    auto focusNode = AceType::DynamicCast<FocusNode>(this);
    if (focusNode) {
        auto scope = parent->GetFocusScope();
        if (scope) {
            auto brothers = parent->GetChildren();
            auto iter = std::find(brothers.begin(), brothers.end(), AceType::Claim(this));
            if (iter == brothers.end()) {
                return;
            }
            ++iter;
            while (iter != brothers.end()) {
                auto nextFocusNode = AceType::DynamicCast<FocusNode>(*iter);
                if (nextFocusNode) {
                    break;
                }
                ++iter;
            }
            if (iter != brothers.end()) {
                scope->AddChild(AceType::Claim(focusNode), AceType::DynamicCast<FocusNode>(*iter));
            } else {
                scope->AddChild(AceType::Claim(focusNode));
            }
            focusNode->SetParentFocusable(scope->FocusNode::IsFocusable());
        }
    }

    auto focusAnimation = AceType::DynamicCast<FocusAnimationElement>(this);
    if (focusAnimation) {
        auto context = context_.Upgrade();
        if (context) {
            context->PushFocusAnimation(AceType::Claim(this));
        }
    }

    auto shadow = AceType::DynamicCast<ShadowElement>(this);
    if (shadow) {
        auto context = context_.Upgrade();
        if (context) {
            context->PushShadow(AceType::Claim(this));
        }
    }
}

void Element::MarkDirty()
{
    RefPtr<PipelineContext> context = context_.Upgrade();
    if (context) {
        context->AddDirtyElement(AceType::Claim(this));
        MarkNeedRebuild();
    }
}

void Element::SetUpdateComponent(const RefPtr<Component>& newComponent)
{
    SetNewComponent(newComponent);
    RefPtr<PipelineContext> context = context_.Upgrade();
    if (context) {
        context->AddDirtyElement(AceType::Claim(this));
    }
}

const std::list<RefPtr<Element>>& Element::GetChildren() const
{
    return children_;
}

RefPtr<Element> Element::GetFirstChild() const
{
    if (children_.empty()) {
        return nullptr;
    }
    return children_.front();
}

RefPtr<Element> Element::GetLastChild() const
{
    if (children_.empty()) {
        return nullptr;
    }
    return children_.back();
}

void Element::SetPipelineContext(const WeakPtr<PipelineContext>& context)
{
    context_ = context;
    OnContextAttached();
}

RefPtr<Element> Element::InflateComponent(const RefPtr<Component>& newComponent, int32_t slot, int32_t renderSlot)
{
    // confirm whether there is a reuseable element.
    auto retakeElement = RetakeDeactivateElement(newComponent);
    if (retakeElement) {
        retakeElement->SetNewComponent(newComponent);
        retakeElement->Mount(AceType::Claim(this), slot, renderSlot);
        if (auto node = retakeElement->GetRenderNode()) {
            node->SyncRSNode(node->GetRSNode());
        }
        return retakeElement;
    }

    RefPtr<Element> newChild = newComponent->CreateElement();
    if (newChild) {
        newChild->SetNewComponent(newComponent);
        newChild->Mount(AceType::Claim(this), slot, renderSlot);
    }
    return newChild;
}

RefPtr<FocusGroup> Element::GetFocusScope()
{
    auto rawPtrFocusGroup = AceType::DynamicCast<FocusGroup>(this);
    if (rawPtrFocusGroup) {
        return AceType::Claim(rawPtrFocusGroup);
    }

    auto rawPtrFocusNode = AceType::DynamicCast<FocusNode>(this);
    if (rawPtrFocusNode) {
        return nullptr;
    }

    auto parent = parent_.Upgrade();
    if (parent) {
        return parent->GetFocusScope();
    }

    return nullptr;
}

RefPtr<PageElement> Element::GetPageElement()
{
    auto pageElement = AceType::DynamicCast<PageElement>(this);
    if (pageElement != nullptr) {
        return AceType::Claim(pageElement);
    }

    auto parent = parent_.Upgrade();

    if (!parent) {
        return nullptr;
    }

    return parent->GetPageElement();
}

RefPtr<Element> Element::RetakeDeactivateElement(const RefPtr<Component>& newComponent)
{
    RefPtr<PipelineContext> context = context_.Upgrade();
    if (context != nullptr) {
        return context->GetDeactivateElement(newComponent->GetRetakeId());
    }
    return nullptr;
}

void Element::RebuildFocusTree()
{
    auto focusScope = AceType::DynamicCast<FocusGroup>(this);
    if (!focusScope) {
        return;
    }

    std::list<RefPtr<FocusNode>> rebuildFocusNodes;
    for (auto& item : children_) {
        auto tmp = item->RebuildFocusChild();
        if (!tmp.empty()) {
            rebuildFocusNodes.merge(std::move(tmp));
        }
    }
    focusScope->RebuildChild(std::move(rebuildFocusNodes));
}

std::list<RefPtr<FocusNode>> Element::RebuildFocusChild()
{
    std::list<RefPtr<FocusNode>> list;
    auto focusNode = AceType::DynamicCast<FocusNode>(this);
    if (focusNode) {
        RebuildFocusTree();
        list.emplace_back(AceType::Claim(focusNode));
        return list;
    }
    for (auto& item : children_) {
        auto focusTmp = item->RebuildFocusChild();
        if (!focusTmp.empty()) {
            list.merge(std::move(focusTmp));
        }
    }
    return list;
}

void Element::MarkActive(bool active)
{
    if (active_ == active) {
        return;
    }
    active_ = active;
    if (active_) {
        OnActive();
    } else {
        OnInactive();
    }
    for (auto& item : children_) {
        item->MarkActive(active);
    }
}

} // namespace OHOS::Ace
