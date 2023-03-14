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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_COMPONENT_H

#include "core/components/common/layout/constants.h"
#include "core/components/wrap/render_wrap.h"
#include "core/components/wrap/wrap_element.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class WrapComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(WrapComponent, ComponentGroup);

public:
    WrapComponent(double spacing, double contentSpacing, const std::list<RefPtr<Component>>& children)
        : ComponentGroup(children), spacing_(Dimension(spacing, DimensionUnit::PX)),
          contentSpacing_(Dimension(contentSpacing, DimensionUnit::PX))
    {}
    explicit WrapComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children) {}
    ~WrapComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderWrap::Create();
    }

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<WrapElement>();
    }

    const Dimension& GetSpacing() const
    {
        return spacing_;
    }

    void SetSpacing(const Dimension& spacing)
    {
        spacing_ = spacing;
    }

    const Dimension& GetContentSpacing() const
    {
        return contentSpacing_;
    }

    void SetContentSpacing(const Dimension& contentSpacing)
    {
        contentSpacing_ = contentSpacing;
    }

    // get/set enum type below
    WrapDirection GetDirection() const
    {
        return direction_;
    }

    void SetDirection(WrapDirection direction)
    {
        direction_ = direction;
    }

    WrapAlignment GetAlignment() const
    {
        return alignment_;
    }

    void SetAlignment(WrapAlignment alignment)
    {
        alignment_ = alignment;
    }

    WrapAlignment GetCrossAlignment() const
    {
        return crossAlignment_;
    }

    void SetCrossAlignment(WrapAlignment crossAlignment)
    {
        crossAlignment_ = crossAlignment;
    }

    WrapAlignment GetMainAlignment() const
    {
        return mainAlignment_;
    }

    void SetMainAlignment(WrapAlignment mainAlignment)
    {
        mainAlignment_ = mainAlignment;
    }

    bool GetDialogStretch() const
    {
        return dialogStretch_;
    }

    void SetDialogStretch(bool dialogStretch)
    {
        dialogStretch_ = dialogStretch;
    }

    MeasureType GetHorizontalMeasure() const
    {
        return horizontalMeasure_;
    }

    void SetHorizontalMeasure(MeasureType horizontalMeasure)
    {
        horizontalMeasure_ = horizontalMeasure;
    }

    MeasureType GetVerticalMeasure() const
    {
        return verticalMeasure_;
    }

    void SetVerticalMeasure(MeasureType verticalMeasure)
    {
        verticalMeasure_ = verticalMeasure;
    }

private:
    // The direction of main axis
    WrapDirection direction_ = WrapDirection::HORIZONTAL;
    // The overall alignment of all children in the wrap component
    WrapAlignment alignment_ = WrapAlignment::START;
    // The alignment of all components in current direction
    WrapAlignment mainAlignment_ = WrapAlignment::SPACE_AROUND;
    // The alignment in cross direction of all component in one direction group.
    WrapAlignment crossAlignment_ = WrapAlignment::CENTER;
    // The space between item in one direction content
    Dimension spacing_;
    // The space between each direction content
    Dimension contentSpacing_;

    MeasureType horizontalMeasure_ = MeasureType::CONTENT;
    MeasureType verticalMeasure_ = MeasureType::CONTENT;

    bool dialogStretch_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WRAP_WRAP_COMPONENT_H
