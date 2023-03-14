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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_RENDER_BOX_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_RENDER_BOX_BASE_H

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "core/animation/property_animatable.h"
#include "core/animation/property_animatable_helper.h"
#include "core/components/box/box_base_component.h"
#include "core/components/box/box_component.h"
#include "core/components/box/mask.h"
#include "core/components/common/layout/grid_column_info.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderBoxBase : public RenderNode {
    DECLARE_ACE_TYPE(RenderBoxBase, RenderNode);

public:
    using LayoutCallback = std::function<void()>;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void Dump() override;

    Offset GetPaintPosition() const;
    const Size& GetPaintSize() const;
    void SetPaintSize(const Size& paintSize);
    virtual Size GetBorderSize() const;
    double CalculateHeightPercent(double percent) const; // add for text filed
    virtual void DrawOnPixelMap() {}

    FloatPropertyAnimatable::SetterMap GetFloatPropertySetterMap() override;
    FloatPropertyAnimatable::GetterMap GetFloatPropertyGetterMap() override;

    void OnAttachContext() override
    {
        width_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderBox = weak.Upgrade();
            if (renderBox) {
                renderBox->OnAnimationCallback();
            }
        });
        height_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderBox = weak.Upgrade();
            if (renderBox) {
                renderBox->OnAnimationCallback();
            }
        });
        marginOrigin_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderBox = weak.Upgrade();
            if (renderBox) {
                renderBox->OnAnimationCallback();
            }
        });
        paddingOrigin_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderBox = weak.Upgrade();
            if (renderBox) {
                renderBox->OnAnimationCallback();
            }
        });
        aspectRatio_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderBox = weak.Upgrade();
            if (renderBox) {
                renderBox->OnAnimationCallback();
            }
        });
    }

    virtual const Color& GetColor() const
    {
        return Color::TRANSPARENT;
    }

    virtual void UpdateStyleFromRenderNode(PropertyAnimatableType type) {}

    AnimatableDimension GetMargin(const DimensionHelper& helper) const
    {
        return helper.Get(marginOrigin_);
    }

    void SetMargin(const AnimatableDimension& value, const DimensionHelper& helper) // add for animation
    {
        if (helper.Set(value, &marginOrigin_)) {
            MarkNeedLayout();
        }
    }

    AnimatableDimension GetPadding(const DimensionHelper& helper) const
    {
        return helper.Get(paddingOrigin_);
    }

    void SetPadding(const AnimatableDimension& value, const DimensionHelper& helper) // add for animation
    {
        if (helper.Set(value, &paddingOrigin_)) {
            MarkNeedLayout();
        }
    }

    void SetPadding(const Edge& padding)
    {
        paddingOrigin_ = padding;
        MarkNeedLayout();
    }

    double GetWidth() const // add for animation
    {
        return width_.Value();
    }

    double GetHeight() const // add for animation
    {
        return height_.Value();
    }

    virtual void SetWidth(double width) // add for animation
    {
        if (GreatOrEqual(width, 0.0) && !NearEqual(width_.Value(), width)) {
            width_.SetValue(width);
            MarkNeedLayout();
        }
    }

    virtual void SetHeight(double height) // add for animation
    {
        if (GreatOrEqual(height, 0.0) && !NearEqual(height_.Value(), height)) {
            height_.SetValue(height);
            MarkNeedLayout();
        }
    }

    virtual void SetWidth(const Dimension& width) // add for animation
    {
        if (width_ != width) {
            width_ = width;
            MarkNeedLayout();
        }
    }

    Dimension GetWidthDimension() const
    {
        return width_;
    }

    virtual void SetHeight(const Dimension& height) // add for animation
    {
        if (height_ != height) {
            height_ = height;
            MarkNeedLayout();
        }
    }

    Dimension GetHeightDimension() const
    {
        return height_;
    }

    EdgePx GetMargin() const
    {
        return margin_;
    }

    EdgePx GetPadding() const
    {
        return padding_;
    }

    Size GetMarginSize() const
    {
        return margin_.GetLayoutSize();
    }

    Size GetPaddingSize() const
    {
        return padding_.GetLayoutSize();
    }

    void SetLayoutCallback(LayoutCallback&& layoutCallback)
    {
        layoutCallback_ = layoutCallback;
    }

    const Rect& GetTouchArea() const
    {
        return touchArea_;
    }

    void SetConstraints(const LayoutParam& constraints)
    {
        constraints_ = constraints;
        MarkNeedLayout();
    }
    const LayoutParam& GetConstraints()
    {
        return constraints_;
    }

    void SetBoxFlex(BoxFlex flex)
    {
        flex_ = flex;
    }

    AlignDeclarationPtr GetAlignDeclarationPtr() const
    {
        return alignPtr_;
    }

    AlignDeclaration::Edge GetUseAlignSide() const
    {
        return alignSide_;
    }

    const Dimension& GetUseAlignOffset() const
    {
        return alignItemOffset_;
    }

    virtual void CalculateAlignDeclaration();

    const Alignment& GetAlign() const
    {
        return align_;
    }

    double GetAspectRatio() const
    {
        return aspectRatio_.Value();
    }

    void SetAspectRatio(const Dimension& aspectRatio)
    {
        aspectRatio_ = aspectRatio;
    }

    const RefPtr<ClipPath>& GetClipPath() const
    {
        return clipPath_;
    }

    bool GetBoxClipFlag() const
    {
        return boxClipFlag_;
    }

    const RefPtr<Mask>& GetMask() const
    {
        return mask_;
    }

    const RefPtr<GridColumnInfo>& GetGridColumnInfo() const
    {
        return gridColumnInfo_;
    }

    const RefPtr<GridContainerInfo>& GetGridContainerInfo() const
    {
        return gridContainerInfo_;
    }

    const RefPtr<PixelMap>& GetPixelMap() const
    {
        return pixelMap_;
    }

    Rect GetPaintRectExcludeMargin() const
    {
        Rect rect;
        rect.SetSize(paintSize_);
        rect.SetOffset(GetPaintRect().GetOffset() + Offset(margin_.LeftPx(), margin_.TopPx()));
        return rect;
    }

protected:
    void ClearRenderObject() override;
    virtual Offset GetBorderOffset() const;
    virtual Radius GetBorderRadius() const;

    EdgePx ConvertEdgeToPx(const Edge& edge, bool additional);
    double ConvertMarginToPx(CalcDimension dimension, bool vertical, bool additional) const;
    double ConvertDimensionToPx(CalcDimension dimension, bool vertical, bool defaultZero = false) const;
    double ConvertHorizontalDimensionToPx(CalcDimension dimension, bool defaultZero = false) const;
    double ConvertVerticalDimensionToPx(CalcDimension dimension, bool defaultZero = false) const;
    void CalculateWidth();
    void CalculateHeight();
    void CalculateAutoMargin();
    void ConvertMarginPaddingToPx();
    void ConvertConstraintsToPx();
    void CalculateGridLayoutSize();
    void CalculateSelfLayoutParam();
    void SetChildLayoutParam();
    void ConvertPaddingForLayoutInBox();
    void CalculateSelfLayoutSize();
    void CalculateChildPosition();
    void AdjustSizeByAspectRatio();
    virtual void OnAnimationCallback();

    AnimatableDimension width_ { AnimatableDimension(-1.0, DimensionUnit::PX) };  // exclude margin
    AnimatableDimension height_ { AnimatableDimension(-1.0, DimensionUnit::PX) }; // exclude margin
    AnimatableDimension aspectRatio_ = AnimatableDimension();

    BoxFlex flex_ = BoxFlex::FLEX_NO;
    LayoutParam constraints_ = LayoutParam(Size(), Size()); // exclude margin
    EdgePx padding_;
    EdgePx margin_;
    Alignment align_;
    Size paintSize_; // exclude margin
    Rect touchArea_; // exclude margin
    bool deliverMinToChild_ = true;
    bool scrollPage_ = false;
    uint32_t percentFlag_ = 0;
    bool layoutInBox_ = false;
    Edge paddingOrigin_;
    Edge marginOrigin_;
    LayoutCallback layoutCallback_;
    bool useLiteStyle_ = false;
    Overflow overflow_ = Overflow::OBSERVABLE;
    bool isChildOverflow_ = false;
    RefPtr<ClipPath> clipPath_;
    RefPtr<Mask> mask_;
    BoxSizing boxSizing_ = BoxSizing::BORDER_BOX;
    WeakPtr<BoxComponent> boxComponent_;

    bool isUseAlign_ = false;
    AlignDeclarationPtr alignPtr_ = nullptr;
    AlignDeclaration::Edge alignSide_ { AlignDeclaration::Edge::AUTO };
    Dimension alignItemOffset_;
    Offset alignOffset_;
    bool boxClipFlag_ = false;
    RefPtr<PixelMap> pixelMap_ = nullptr;

private:
    bool IsSizeValid(const Dimension& value, double maxLimit);

    Edge additionalPadding_;
    bool useFlexWidth_ = false;
    bool useFlexHeight_ = false;
    bool selfDefineWidth_ = false;
    bool selfDefineHeight_ = false;
    double selfMaxWidth_ = Size::INFINITE_SIZE;  // exclude margin
    double selfMinWidth_ = 0.0;                  // exclude margin
    double selfMaxHeight_ = Size::INFINITE_SIZE; // exclude margin
    double selfMinHeight_ = 0.0;                 // exclude margin
    CalcDimension minWidth_ = Dimension();
    CalcDimension minHeight_ = Dimension();
    CalcDimension maxWidth_ = Dimension();
    CalcDimension maxHeight_ = Dimension();

    // result for layout
    LayoutParam selfLayoutParam_; // include margin
    Size selfLayoutSize_;         // include margin
    LayoutParam childLayoutParam_;
    Size childSize_;
    Offset childPosition_;

    // grid layout
    RefPtr<GridColumnInfo> gridColumnInfo_;
    RefPtr<GridContainerInfo> gridContainerInfo_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_RENDER_BOX_BASE_H
