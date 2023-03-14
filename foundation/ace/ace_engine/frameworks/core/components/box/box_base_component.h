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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_BASE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_BASE_COMPONENT_H

#include <string>

#include "base/geometry/animatable_dimension.h"
#include "base/image/pixel_map.h"
#include "core/components/box/mask.h"
#include "core/components/common/layout/align_declaration.h"
#include "core/components/common/layout/grid_column_info.h"
#include "core/components/common/layout/grid_layout_info.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/clip_path.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/measurable.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

constexpr uint32_t PERCENT_FLAG_USE_VIEW_PORT = 1;

// A component can box other components.
class BoxBaseComponent : public SoleChildComponent, public Measurable {
    DECLARE_ACE_TYPE(BoxBaseComponent, SoleChildComponent, Measurable);

public:
    const Alignment& GetAlignment() const
    {
        return align_;
    }

    const LayoutParam& GetConstraints() const
    {
        return constraints_;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    const Edge& GetAdditionalPadding() const
    {
        return additionalPadding_;
    }

    const AnimatableDimension& GetWidthDimension() const
    {
        return GetWidth();
    }

    const AnimatableDimension& GetHeightDimension() const
    {
        return GetHeight();
    }

    BoxFlex GetFlex() const
    {
        return flex_;
    }

    bool GetDeliverMinToChild() const
    {
        return deliverMinToChild_;
    }

    bool GetScrollPage() const
    {
        return scrollPage_;
    }

    uint32_t GetPercentFlag() const
    {
        return percentFlag_;
    }

    bool GetLayoutInBoxFlag() const
    {
        return layoutInBox_;
    }

    void SetAlignment(const Alignment& alignment)
    {
        align_ = alignment;
    }

    void SetConstraints(const LayoutParam& constraints)
    {
        if (!constraints.IsWidthValid() || !constraints.IsHeightValid()) {
            return;
        }
        constraints_ = constraints;
    }

    void SetMargin(const Edge& margin)
    {
        // Margin need to support negative number.
        margin_ = margin;
    }

    void SetPadding(const Edge& padding, const Edge& additionalPadding = Edge())
    {
        if (padding.IsValid()) {
            // Padding not support negative number.
            padding_ = padding;
        }
        if (additionalPadding.IsValid()) {
            additionalPadding_ = additionalPadding;
        }
    }

    void SetFlex(BoxFlex flex)
    {
        flex_ = flex;
    }

    void SetDeliverMinToChild(bool deliverMinToChild)
    {
        deliverMinToChild_ = deliverMinToChild;
    }

    void SetScrollPage(bool scrollPage)
    {
        scrollPage_ = scrollPage;
    }

    void SetPercentFlag(uint32_t flag)
    {
        percentFlag_ = flag;
    }

    void SetLayoutInBoxFlag(bool layoutInBox)
    {
        layoutInBox_ = layoutInBox;
    }

    void SetAspectRatio(double aspectRatio)
    {
        aspectRatio_ = AnimatableDimension(aspectRatio);
    }

    void SetAspectRatio(double aspectRatio, AnimationOption option)
    {
        aspectRatio_ = AnimatableDimension(aspectRatio, DimensionUnit::PX, option);
    }

    AnimatableDimension GetAspectRatio() const
    {
        return aspectRatio_;
    }

    void SetMinWidth(const CalcDimension& minWidth)
    {
        minWidth_ = minWidth;
    }

    const CalcDimension& GetMinWidth() const
    {
        return minWidth_;
    }

    void SetMinHeight(const CalcDimension& minHeight)
    {
        minHeight_ = minHeight;
    }

    const CalcDimension& GetMinHeight() const
    {
        return minHeight_;
    }

    void SetMaxWidth(const CalcDimension& maxWidth)
    {
        maxWidth_ = maxWidth;
    }

    const CalcDimension& GetMaxWidth() const
    {
        return maxWidth_;
    }

    void SetMaxHeight(const CalcDimension& maxHeight)
    {
        maxHeight_ = maxHeight;
    }

    const CalcDimension& GetMaxHeight() const
    {
        return maxHeight_;
    }

    const RefPtr<GridLayoutInfo>& GetGridLayoutInfo() const
    {
        return gridLayoutInfo_;
    }

    void SetGridLayoutInfo(const RefPtr<GridLayoutInfo>& gridLayoutInfo)
    {
        gridLayoutInfo_ = gridLayoutInfo;
    }

    RefPtr<GridColumnInfo::Builder> GetGridColumnInfoBuilder()
    {
        if (!gridColumnInfoBuilder_) {
            gridColumnInfoBuilder_ = AceType::MakeRefPtr<GridColumnInfo::Builder>();
        }
        return gridColumnInfoBuilder_;
    }

    void OnWrap() override
    {
        if (gridColumnInfoBuilder_) {
            SetGridLayoutInfo(gridColumnInfoBuilder_->Build());
            gridColumnInfoBuilder_ = nullptr;
        }
    }

    void SetUseLiteStyle(bool flag)
    {
        useLiteStyle_ = flag;
    }

    bool UseLiteStyle() const
    {
        return useLiteStyle_;
    }

    void SetOverflow(Overflow overflow)
    {
        overflow_ = overflow;
    }

    Overflow GetOverflow() const
    {
        return overflow_;
    }

    const RefPtr<ClipPath>& GetClipPath() const
    {
        return clipPath_;
    }

    void SetClipPath(const RefPtr<ClipPath>& clipPath)
    {
        clipPath_ = clipPath;
    }
    const RefPtr<Mask>& GetMask() const
    {
        return mask_;
    }

    void SetMask(const std::string& image, const BackgroundImageSize& size, const BackgroundImagePosition& position)
    {
        if (!image.empty()) {
            if (!mask_) {
                mask_ = Mask::Create();
            }
            if (mask_) {
                mask_->SetMaskImage(image);
                mask_->SetMaskSize(size);
                mask_->SetMaskPosition(position);
            }
        }
    }

    void SetMask(const RefPtr<MaskPath>& maskPath)
    {
        if (!mask_) {
            mask_ = Mask::Create();
        }
        if (mask_) {
            mask_->SetMask(maskPath);
        }
    }

    BoxSizing GetBoxSizing() const
    {
        return boxSizing_;
    }

    void SetBoxSizing(BoxSizing boxSizing)
    {
        boxSizing_ = boxSizing;
    }

    void SetAlignDeclarationPtr(AlignDeclarationPtr alignPtr)
    {
        alignPtr_ = alignPtr;
    }

    AlignDeclarationPtr GetAlignDeclarationPtr() const
    {
        return alignPtr_;
    }

    void SetUseAlignSide(AlignDeclaration::Edge side)
    {
        alignSide_ = side;
    }

    AlignDeclaration::Edge GetUseAlignSide() const
    {
        return alignSide_;
    }

    void SetUseAlignOffset(const Dimension& offset)
    {
        alignOffset_ = offset;
    }

    const Dimension& GetUseAlignOffset() const
    {
        return alignOffset_;
    }

    bool IsUseAlign() const
    {
        return alignPtr_ != nullptr && alignSide_ != AlignDeclaration::Edge::AUTO;
    }

    void SetBoxClipFlag(bool boxClipFlag)
    {
        boxClipFlag_ = boxClipFlag;
    }

    bool GetBoxClipFlag() const
    {
        return boxClipFlag_;
    }

    void SetPixelMap(const RefPtr<PixelMap>& pixelMap)
    {
        pixelMap_ = pixelMap;
    }

    const RefPtr<PixelMap>& GetPixelMap() const
    {
        return pixelMap_;
    }

private:
    Alignment align_;
    LayoutParam constraints_ = LayoutParam(Size(), Size()); // no constraints when init
    Edge padding_;
    Edge margin_;
    Edge additionalPadding_;
    BoxFlex flex_ { BoxFlex::FLEX_NO };
    bool deliverMinToChild_ = true;
    bool scrollPage_ = false;
    uint32_t percentFlag_ = 1;
    bool layoutInBox_ = false;
    AnimatableDimension aspectRatio_ = AnimatableDimension();
    CalcDimension minWidth_ = Dimension();
    CalcDimension minHeight_ = Dimension();
    CalcDimension maxWidth_ = Dimension();
    CalcDimension maxHeight_ = Dimension();
    RefPtr<GridLayoutInfo> gridLayoutInfo_;
    RefPtr<GridColumnInfo::Builder> gridColumnInfoBuilder_;
    bool useLiteStyle_ = false;
    Overflow overflow_ = Overflow::OBSERVABLE;
    RefPtr<ClipPath> clipPath_;
    RefPtr<Mask> mask_;
    BoxSizing boxSizing_ = BoxSizing::BORDER_BOX;
    bool boxClipFlag_ = false;
    RefPtr<PixelMap> pixelMap_ = nullptr;

    AlignDeclarationPtr alignPtr_ = nullptr;
    AlignDeclaration::Edge alignSide_ { AlignDeclaration::Edge::AUTO };
    Dimension alignOffset_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_BOX_BASE_COMPONENT_H
