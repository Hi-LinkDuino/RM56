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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_COMPONENT_H

#include "core/pipeline/base/component_group.h"
#ifdef WEARABLE_PRODUCT
#include "core/components/flex/flex_component.h"
#endif
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/stack/render_stack.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

class ACE_EXPORT StackComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(StackComponent, ComponentGroup);

public:
    StackComponent(
        const Alignment& align, StackFit fit, Overflow overflow, const std::list<RefPtr<Component>>& children)
        : ComponentGroup(children), align_(align), fit_(fit), overflow_(overflow)
    {}
    ~StackComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<StackElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderStack::Create();
    }

    const Alignment& GetAlignment() const
    {
        return align_;
    }

    void SetAlignment(const Alignment& align)
    {
        align_ = align;
    }

    StackFit GetStackFit() const
    {
        return fit_;
    }

    void SetStackFit(StackFit fit)
    {
        fit_ = fit;
    }

    Overflow GetOverflow() const
    {
        return overflow_;
    }

    void SetOverflow(Overflow overflow)
    {
        overflow_ = overflow;
    }

    MainStackSize GetMainStackSize() const
    {
        return mainStackSize_;
    }

    void SetMainStackSize(MainStackSize mainStackSize)
    {
        mainStackSize_ = mainStackSize;
    }

    bool HasNavigationBar() const
    {
        return navigationBar_ != nullptr;
    }

    void SetNavigationBar(const RefPtr<Component>& navigationBar);

private:
    Alignment align_;
    StackFit fit_ { StackFit::KEEP };
    Overflow overflow_ { Overflow::CLIP };
    MainStackSize mainStackSize_ { MainStackSize::NORMAL };
    RefPtr<Component> navigationBar_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_COMPONENT_H
