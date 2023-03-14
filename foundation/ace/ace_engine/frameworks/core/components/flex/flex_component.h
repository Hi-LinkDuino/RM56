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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_COMPONENT_H

#include "base/utils/macros.h"
#include "core/components/common/layout/align_declaration.h"
#include "core/components/common/layout/constants.h"
#include "core/components/flex/flex_element.h"
#include "core/components/flex/render_flex.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ACE_EXPORT FlexComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(FlexComponent, ComponentGroup);

public:
    FlexComponent(FlexDirection direction, FlexAlign mainAxisAlign, FlexAlign crossAxisAlign,
        const std::list<RefPtr<Component>>& children)
        : ComponentGroup(children), direction_(direction), mainAxisAlign_(mainAxisAlign),
          crossAxisAlign_(crossAxisAlign)
    {}
    ~FlexComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderFlex::Create();
    }

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<FlexElement>();
    }

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    void SetDirection(FlexDirection direction)
    {
        direction_ = direction;
    }

    FlexAlign GetCrossAxisAlign() const
    {
        return crossAxisAlign_;
    }

    void SetCrossAxisAlign(FlexAlign align)
    {
        crossAxisAlign_ = align;
    }

    FlexAlign GetMainAxisAlign() const
    {
        return mainAxisAlign_;
    }

    void SetMainAxisAlign(FlexAlign align)
    {
        mainAxisAlign_ = align;
    }

    MainAxisSize GetMainAxisSize() const
    {
        return mainAxisSize_;
    }

    void SetMainAxisSize(MainAxisSize mainAxisSize)
    {
        mainAxisSize_ = mainAxisSize;
    }

    CrossAxisSize GetCrossAxisSize() const
    {
        return crossAxisSize_;
    }

    void SetCrossAxisSize(CrossAxisSize crossAxisSize)
    {
        crossAxisSize_ = crossAxisSize;
    }

    TextBaseline GetBaseline() const
    {
        return baseline_;
    }

    void SetBaseline(TextBaseline baseline)
    {
        baseline_ = baseline;
    }

    bool IsStretchToParent() const
    {
        return stretchToParent_;
    }

    void SetStretchToParent(bool flag)
    {
        stretchToParent_ = flag;
    }

    bool GetUseViewPortFlag() const
    {
        return useViewPort_;
    }

    void SetUseViewPortFlag(bool flag)
    {
        useViewPort_ = flag;
    }

    bool ContainsNavigation() const
    {
        return containsNavigation_;
    }

    void SetContainsNavigation(bool containsNavigation)
    {
        containsNavigation_ = containsNavigation;
    }

    Overflow GetOverflow() const
    {
        return overflow_;
    }

    void SetOverflow(Overflow overflow)
    {
        overflow_ = overflow;
    }

    void SetSpace(const Dimension& space)
    {
        space_ = space;
    }

    const Dimension& GetSpace() const
    {
        return space_;
    }

    void SetAlignDeclarationPtr(AlignDeclaration* alignPtr)
    {
        alignPtr_ = alignPtr;
    }

    AlignDeclarationPtr GetAlignDeclarationPtr() const
    {
        return alignPtr_;
    }

private:
    FlexDirection direction_ = FlexDirection::ROW;
    FlexAlign mainAxisAlign_ = FlexAlign::FLEX_START;
    FlexAlign crossAxisAlign_ = FlexAlign::FLEX_START;
    MainAxisSize mainAxisSize_ = MainAxisSize::MAX;
    CrossAxisSize crossAxisSize_ = CrossAxisSize::MIN;
    TextBaseline baseline_ = TextBaseline::ALPHABETIC;
    Overflow overflow_ = Overflow::CLIP;
    Dimension space_;

    AlignDeclarationPtr alignPtr_ = nullptr;

    bool stretchToParent_ = false;
    bool useViewPort_ = false;
    bool containsNavigation_ = false;
};

class RowComponent : public FlexComponent {
    DECLARE_ACE_TYPE(RowComponent, FlexComponent);

public:
    RowComponent(FlexAlign mainAxisAlign, FlexAlign crossAxisAlign, const std::list<RefPtr<Component>>& children)
        : FlexComponent(FlexDirection::ROW, mainAxisAlign, crossAxisAlign, children)
    {}
    ~RowComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<RowElement>();
    }
};

class ColumnComponent : public FlexComponent {
    DECLARE_ACE_TYPE(ColumnComponent, FlexComponent);

public:
    ColumnComponent(FlexAlign mainAxisAlign, FlexAlign crossAxisAlign, const std::list<RefPtr<Component>>& children)
        : FlexComponent(FlexDirection::COLUMN, mainAxisAlign, crossAxisAlign, children)
    {}
    ~ColumnComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<ColumnElement>();
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FLEX_FLEX_COMPONENT_H
