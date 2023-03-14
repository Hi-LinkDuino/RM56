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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_RENDER_STACK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_RENDER_STACK_H

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/positioned/render_positioned.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class ACE_EXPORT RenderStack : public RenderNode {
    DECLARE_ACE_TYPE(RenderStack, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    Overflow GetOverflowFlag() const
    {
        return overflow_;
    }

    bool IsChildOverflow() const
    {
        return isChildOverflow_;
    }

    void SetStackFit(StackFit fit)
    {
        fit_ = fit;
    }

    void SetMainStackSize(MainStackSize mainStackSize)
    {
        mainStackSize_ = mainStackSize;
    }

    const Alignment& GetAlignment() const
    {
        return align_;
    }

    StackFit GetStackFit() const
    {
        return fit_;
    }

    Overflow GetOverflow() const
    {
        return overflow_;
    }

    void OnAttachContext() override;

protected:
    LayoutParam MakeNonPositionedInnerLayoutParam(const RefPtr<RenderNode>& firstChild) const;
    LayoutParam MakePositionedInnerLayoutParam(
        const RefPtr<RenderPositioned>& item, const RefPtr<RenderNode>& firstChild) const;

    Offset GetNonPositionedChildOffset(const Size& childSize);
    Offset GetPositionedChildOffset(const RefPtr<RenderPositioned>& item);

    virtual void DetermineStackSize(bool hasNonPositioned);
    virtual void SetChildrenStatus() {}
private:
    Alignment align_;
    StackFit fit_ = StackFit::KEEP;
    Overflow overflow_ = Overflow::OBSERVABLE;
    bool isChildOverflow_ = false;
    MainStackSize mainStackSize_ = MainStackSize::NORMAL;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_RENDER_STACK_H
