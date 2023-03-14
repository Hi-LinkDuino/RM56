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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPOSED_ELEMENT_H

#include <string>

#include "base/utils/macros.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_element.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

// ComposedElement just maintain a child element may have render node.
class ACE_EXPORT ComposedElement : public Element {
    DECLARE_ACE_TYPE(ComposedElement, Element);

public:
    using RenderFunction = std::function<RefPtr<Component>(const RefPtr<Component>&)>;
    using PageTransitionFunction = std::function<RefPtr<Component>()>;
    using ApplyFunction = std::function<void(const RefPtr<RenderElement>&)>;
    explicit ComposedElement(const ComposeId& id);
    ~ComposedElement() override = default;

    void Detached() override;
    void Deactivate() override;
    void PerformBuild() override;
    void Update() override;
    RefPtr<Element> UpdateChild(const RefPtr<Element>& child, const RefPtr<Component>& newComponent) final;

    void Dump() override;

    bool CanUpdate(const RefPtr<Component>& newComponent) override;
    bool NeedUpdateWithComponent(const RefPtr<Component>& newComponent) override;

    void ApplyComposed(ApplyFunction&& applyFunction)
    {
        applyFunction_ = std::move(applyFunction);
        ApplyChildren();
    }

    void ApplyComposed(const ApplyFunction& applyFunction)
    {
        applyFunction_ = applyFunction;
        ApplyChildren();
    }

    const ComposeId& GetId() const
    {
        return id_;
    }

    RefPtr<Component> CallRenderFunction(const RefPtr<Component>& component)
    {
        if (renderFunction_) {
            return renderFunction_(component);
        }
        return nullptr;
    }

    void SetRenderFunction(RenderFunction&& func)
    {
        renderFunction_ = std::move(func);
    }

    bool HasRenderFunction()
    {
        if (renderFunction_) {
            return true;
        }
        return false;
    }

    RefPtr<Component> CallPageTransitionFunction()
    {
        if (!pageTransitionFunction_) {
            return nullptr;
        }
        return pageTransitionFunction_();
    }

    void SetPageTransitionFunction(PageTransitionFunction&& func)
    {
        pageTransitionFunction_ = std::move(func);
    }

    bool HasPageTransitionFunction()
    {
        return !!pageTransitionFunction_;
    }

protected:
    virtual RefPtr<Component> BuildChild();
    void Apply(const RefPtr<Element>& child) override;
    void UmountRender() override;
    void ApplyChildren();
    int32_t CountRenderNode() const override
    {
        return countRenderNode_;
    }

    ComposeId id_;
    std::string name_;
    bool addedToMap_ = false;
    int32_t countRenderNode_ = -1;
    RenderFunction renderFunction_;
    PageTransitionFunction pageTransitionFunction_;

    ApplyFunction applyFunction_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPOSED_ELEMENT_H
