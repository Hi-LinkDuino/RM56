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

#include "core/pipeline/base/composed_element.h"

#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/common/frontend.h"
#include "core/components/flex/flex_item_element.h"
#include "core/components/page/page_element.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

ComposedElement::ComposedElement(const ComposeId& id) : id_(id)
{
    type_ = COMPOSED_ELEMENT;
}

void ComposedElement::Detached()
{
    auto context = context_.Upgrade();
    if (addedToMap_ && context) {
        context->RemoveComposedElement(id_, AceType::Claim(this));
        addedToMap_ = false;
    }
}

void ComposedElement::Deactivate()
{
    Detached();
    UmountRender();
}

void ComposedElement::UmountRender()
{
    for (const auto& child : children_) {
        child->UmountRender();
    }
}

void ComposedElement::PerformBuild()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("Invalid pipeline stop building");
        return;
    }

    if (!addedToMap_) {
        context->AddComposedElement(id_, AceType::Claim(this));
        addedToMap_ = true;
    }

    auto component = HasRenderFunction() ? CallRenderFunction(component_) : BuildChild();
    auto child = children_.empty() ? nullptr : children_.front();
    auto composedComponent = AceType::DynamicCast<ComposedComponent>(component_);
    if (composedComponent) {
        auto composedChild = composedComponent->GetChild();
        if (HasRenderFunction() && composedComponent->GetNeedReserveChild()) {
            auto flexItem = AceType::DynamicCast<SoleChildComponent>(composedChild);
            if (flexItem) {
                flexItem->SetChild(component);
                UpdateChild(child, flexItem);
                return;
            }
        }
    }

    UpdateChild(child, component);
}

void ComposedElement::Update()
{
    const RefPtr<ComposedComponent> compose = AceType::DynamicCast<ComposedComponent>(component_);
    if (compose != nullptr) {
        name_ = compose->GetName();
        if (id_ != compose->GetId()) {
            auto context = context_.Upgrade();
            if (addedToMap_ && context != nullptr) {
                context->RemoveComposedElement(id_, AceType::Claim(this));
                context->AddComposedElement(compose->GetId(), AceType::Claim(this));
            }
            id_ = compose->GetId();
        }
        compose->ClearNeedUpdate();
    }
    if (HasPageTransitionFunction()) {
        auto pageElement = GetPageElement();
        if (pageElement) {
            pageElement->SetPageTransitionFunction(std::move(pageTransitionFunction_));
        }
    }
}

RefPtr<Component> ComposedElement::BuildChild()
{
    RefPtr<ComposedComponent> compose = AceType::DynamicCast<ComposedComponent>(component_);
    if (compose != nullptr) {
        return compose->GetChild();
    }
    return nullptr;
}

void ComposedElement::Apply(const RefPtr<Element>& child)
{
    if (!child) {
        LOGE("Element child is null");
        return;
    }

    if (!applyFunction_) {
        LOGE("No apply function");
        return;
    }

    if (child->GetType() == RENDER_ELEMENT) {
        // Directly attach the RenderNode if child is RenderElement.
        applyFunction_(AceType::DynamicCast<RenderElement>(child));
    } else if (child->GetType() == COMPOSED_ELEMENT) {
        // If child is ComposedElement, just set apply function.
        RefPtr<ComposedElement> composeChild = AceType::DynamicCast<ComposedElement>(child);
        if (composeChild) {
            composeChild->ApplyComposed(applyFunction_);
        }
    }
}

void ComposedElement::ApplyChildren()
{
    for (const auto& child : children_) {
        Apply(child);
    }
}

void ComposedElement::Dump()
{
    DumpLog::GetInstance().AddDesc("name:" + name_);
    DumpLog::GetInstance().AddDesc("id:" + id_);
}

bool ComposedElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto compose = AceType::DynamicCast<ComposedComponent>(newComponent);
    if (!compose) {
        return false;
    }
    if (compose->GetId() == id_) {
        return true;
    }

    // For declarative, IDs MUST equal
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        return false;
    }

    if (children_.empty()) {
        return true;
    }
    auto childComponent = compose->GetChild();
    if (!childComponent) {
        return true;
    }
    return children_.front()->CanUpdate(childComponent);
}

bool ComposedElement::NeedUpdateWithComponent(const RefPtr<Component>& newComponent)
{
    auto component = AceType::DynamicCast<ComposedComponent>(newComponent);
    if (component) {
        auto newId = component->GetId();
        if (newId.empty()) {
            return true;
        }

        if (component->NeedUpdate()) {
            return true;
        }

        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            return newId == id_;
        } else {
            return newId != id_;
        }
    }
    return true;
}

RefPtr<Element> ComposedElement::UpdateChild(const RefPtr<Element>& child, const RefPtr<Component>& newComponent)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("Invalid pipeline stop updating");
        return nullptr;
    }

    RefPtr<Element> newChild;
    if (context->GetIsDeclarative()) {
        newChild = UpdateChildWithSlot(child, newComponent, DEFAULT_ELEMENT_SLOT, GetRenderSlot());
    } else {
        newChild = UpdateChildWithSlot(child, newComponent, DEFAULT_ELEMENT_SLOT, DEFAULT_RENDER_SLOT);
    }
    countRenderNode_ = newChild ? newChild->CountRenderNode() : 0;
    return newChild;
}

} // namespace OHOS::Ace
