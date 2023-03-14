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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_COVERAGE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_COVERAGE_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/utils/label_target.h"
#include "base/utils/macros.h"
#include "core/components/align/align_component.h"
#include "core/components/box/box_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/text/text_component_v2.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ACE_EXPORT CoverageComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(CoverageComponent, ComponentGroup);

public:
    explicit CoverageComponent(const std::list<RefPtr<Component>>& children);
    ~CoverageComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    void SetTextVal(const std::string& val)
    {
        textComponent_->SetData(val);
    }

    const std::string GetTextVal() const
    {
        return textComponent_->GetData();
    }

    void SetAlignment(Alignment alignment)
    {
        alignComponent_->SetAlignment(alignment);
    }

    void SetX(const Dimension& x)
    {
        textComponent_->SetHasLeft(true);
        textComponent_->SetLeft(x);
        textComponent_->SetPositionType(PositionType::RELATIVE);
    }

    void SetY(const Dimension& y)
    {
        textComponent_->SetHasTop(true);
        textComponent_->SetTop(y);
        textComponent_->SetPositionType(PositionType::RELATIVE);
    }

    const Dimension& GetX() const
    {
        return textComponent_->GetLeft();
    }

    const Alignment& GetAlignment() const
    {
        return alignComponent_->GetAlignment();
    }

    const Dimension& GetY() const
    {
        return textComponent_->GetTop();
    }

    bool IsOverLay() const
    {
        return isOverLay_;
    }

    void SetIsOverLay(bool isOverLay)
    {
        isOverLay_ = isOverLay;
    }

    void Initialization();

private:
    RefPtr<TextComponent> textComponent_;
    RefPtr<BoxComponent> textBoxComponent_;
    RefPtr<AlignComponent> alignComponent_;
    bool isOverLay_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COVERAGE_COVERAGE_COMPONENT_H
