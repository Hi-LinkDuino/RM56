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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_MASK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_MASK_H

#include "base/memory/ace_type.h"

#include "frameworks/core/animation/animator.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/components/svg/render_svg_base.h"

class SkCanvas;
namespace OHOS::Ace {

class RenderSvgMask : public RenderSvgBase {
    DECLARE_ACE_TYPE(RenderSvgMask, RenderSvgBase)

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate) override;
    virtual void PaintMaskLayer(SkCanvas* canvas, const Offset& offset, const Rect& paintRect) {}
    virtual void PaintMaskLayer(RenderContext& context, const Offset& offset, const Rect& paintRect) {}

    bool IsDefalutMaskUnits() const
    {
        return maskUnits_ == "objectBoundingBox";
    }

    bool IsDefalutMaskContentUnits() const
    {
        // return true if content relative to svg
        return maskContentUnits_ == "userSpaceOnUse";
    }

    void SetElementBounds(const Rect& bounds)
    {
        elementBounds_ = bounds;
    }

protected:
    double ParseUnitsAttr(const Dimension& attr, double value);

    Dimension x_ = Dimension(-0.1, DimensionUnit::PERCENT); // x-axis default value
    Dimension y_ = Dimension(-0.1, DimensionUnit::PERCENT); // y-axis default value
    Dimension height_ = Dimension(1.2, DimensionUnit::PERCENT); // masking area width default value
    Dimension width_ = Dimension(1.2, DimensionUnit::PERCENT); // masking area height default  value
    std::string maskUnits_ = "objectBoundingBox";
    std::string maskContentUnits_ = "userSpaceOnUse";
    Rect elementBounds_;

private:
    void PrepareAnimations(const RefPtr<Component>& component);
    bool SetProperty(const std::string& attributeName, double value);
    bool GetProperty(const std::string& attrName, double& value) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_MASK_H
