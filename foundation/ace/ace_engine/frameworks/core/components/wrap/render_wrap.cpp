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

#include "core/components/wrap/render_wrap.h"

#include <algorithm>

#include "core/components/wrap/wrap_component.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t MIN_COMPATIBLE_VERSION = 6;

}

RefPtr<RenderNode> RenderWrap::Create()
{
    return AceType::MakeRefPtr<RenderWrap>();
}

void RenderWrap::Update(const RefPtr<Component>& component)
{
    const RefPtr<WrapComponent> wrap = AceType::DynamicCast<WrapComponent>(component);
    if (!wrap) {
        LOGE("Wrap::RenderWrap update dynamicCast to nullptr error");
        return;
    }
    direction_ = wrap->GetDirection();
    // Whole alignment
    alignment_ = wrap->GetAlignment();
    // content main alignment
    mainAlignment_ = wrap->GetMainAlignment();
    // content cross alignment
    crossAlignment_ = wrap->GetCrossAlignment();
    spacing_ = wrap->GetSpacing();
    contentSpace_ = wrap->GetContentSpacing();
    dialogStretch_ = wrap->GetDialogStretch();
    horizontalMeasure_ = wrap->GetHorizontalMeasure();
    verticalMeasure_ = wrap->GetVerticalMeasure();
    SetTextDirection(wrap->GetTextDirection());
    isLeftToRight_ = (wrap->GetTextDirection() == TextDirection::LTR);
    contentList_.clear();
    MarkNeedLayout();
}

void RenderWrap::PerformLayout()
{
    if (GetChildren().empty()) {
        // no child will set current to empty and return
        SetLayoutSize(Size(0.0, 0.0));
        return;
    }

    PerformLayoutInitialize();

    // overall size including space
    totalMainLength_ = 0.0;
    totalCrossLength_ = 0.0;

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(GetLeftSize(0.0, mainLengthLimit_, crossLengthLimit_));
    if (dialogStretch_) {
        HandleDialogStretch(layoutParam);
    } else {
        auto spacing = NormalizeToPx(spacing_);
        auto contentSpace = NormalizeToPx(contentSpace_);
        // content size
        double currentMainLength = 0.0;
        // the cross length is without space
        double currentCrossLength = 0.0;
        // number of item in content
        int32_t count = 0;
        // max baseline of each line
        double baselineDistance = 0.0;
        std::list<RefPtr<RenderNode>> itemsList;
        for (auto& item : GetChildren()) {
            item->Layout(layoutParam);

            if (mainLengthLimit_ >= currentMainLength + GetMainItemLength(item)) {
                currentMainLength += GetMainItemLength(item);
                currentMainLength += spacing;
                currentCrossLength = std::max(currentCrossLength, GetCrossItemLength(item));
                if (crossAlignment_ == WrapAlignment::BASELINE) {
                    baselineDistance = std::max(baselineDistance, item->GetBaselineDistance(TextBaseline::ALPHABETIC));
                }
                itemsList.push_back(item);
                count += 1;
            } else {
                currentMainLength -= spacing;
                if ((direction_ == WrapDirection::HORIZONTAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::HORIZONTAL_REVERSE && isLeftToRight_) ||
                    direction_ == WrapDirection::VERTICAL_REVERSE) {
                    itemsList.reverse();
                }
                auto contentInfo = ContentInfo(currentMainLength, currentCrossLength, count, itemsList);
                contentInfo.maxBaselineDistance = baselineDistance;
                contentList_.emplace_back(contentInfo);
                itemsList.clear();
                totalMainLength_ = std::max(currentMainLength, totalMainLength_);
                totalCrossLength_ += currentCrossLength + contentSpace;
                currentMainLength = GetMainItemLength(item) + spacing;
                currentCrossLength = GetCrossItemLength(item);
                if (crossAlignment_ == WrapAlignment::BASELINE) {
                    baselineDistance = item->GetBaselineDistance(TextBaseline::ALPHABETIC);
                }
                itemsList.push_back(item);
                count = 1;
            }
        }
        // Add last content into list
        currentMainLength -= spacing;
        if ((direction_ == WrapDirection::HORIZONTAL && !isLeftToRight_) ||
            (direction_ == WrapDirection::HORIZONTAL_REVERSE && isLeftToRight_) ||
            (direction_ == WrapDirection::VERTICAL_REVERSE)) {
            itemsList.reverse();
        }
        auto contentInfo = ContentInfo(currentMainLength, currentCrossLength, count, itemsList);
        contentInfo.maxBaselineDistance = baselineDistance;
        contentList_.emplace_back(contentInfo);
        if ((direction_ == WrapDirection::VERTICAL || direction_ == WrapDirection::VERTICAL_REVERSE) &&
            !isLeftToRight_) {
            contentList_.reverse();
        }
        totalMainLength_ = std::max(currentMainLength, totalMainLength_);
        // n contents has n - 1 space
        totalCrossLength_ += currentCrossLength;
    }
    LayoutWholeWrap();
    SetWrapLayoutSize(mainLengthLimit_, totalCrossLength_);
    contentList_.clear();
}

void RenderWrap::HandleDialogStretch(const LayoutParam& layoutParam)
{
    int32_t dialogButtonNum = 0;
    double totalLength = 0.0;
    auto spacing = NormalizeToPx(spacing_);
    auto contentSpace = NormalizeToPx(contentSpace_);
    // whether the btn in the wrap needs wrap
    for (const auto& item : GetChildren()) {
        dialogButtonNum += 1;
        item->Layout(layoutParam);
        totalLength += GetMainItemLength(item) + spacing;
        if (totalLength - spacing > mainLengthLimit_) {
            dialogDirection_ = WrapDirection::VERTICAL;
        }
    }
    if (dialogButtonNum == 0) {
        LOGW("dialog button number is 0");
        return;
    }

    double buttonSize = (mainLengthLimit_ - spacing * (dialogButtonNum - 1)) / dialogButtonNum;
    std::list<RefPtr<RenderNode>> itemsList;
    for (const auto& item : GetChildren()) {
        LayoutParam newParam;
        // if dialog is vertical, stretch each button equally to fill max length, otherwise stretch equally in same line
        double stretchSize = dialogDirection_ == WrapDirection::VERTICAL ? mainLengthLimit_ : buttonSize;
        newParam.SetFixedSize(
            (direction_ == WrapDirection::HORIZONTAL ? Size(stretchSize, item->GetLayoutSize().Height())
                                                     : Size(item->GetLayoutSize().Width(), stretchSize)));
        item->Layout(newParam);
        itemsList.push_back(item);
        totalMainLength_ = mainLengthLimit_;

        if (dialogDirection_ == WrapDirection::VERTICAL) {
            // stretch each button equally to fill max length

            totalCrossLength_ += direction_ == WrapDirection::HORIZONTAL ? item->GetLayoutSize().Height()
                                                                         : item->GetLayoutSize().Width();
            totalCrossLength_ += contentSpace;
            contentList_.emplace_back(
                ContentInfo(newParam.GetMaxSize().Width(), newParam.GetMaxSize().Height(), 1, itemsList));
            itemsList.clear();
        } else {
            // stretch each button equally in same line
            totalCrossLength_ = std::max(direction_ == WrapDirection::HORIZONTAL ? item->GetLayoutSize().Height()
                                                                                 : item->GetLayoutSize().Width(),
                totalCrossLength_);
        }
    }
    // if wrap direction is vertical, item has already added into content list
    if (dialogDirection_ == WrapDirection::VERTICAL) {
        totalCrossLength_ -= contentSpace;
        return;
    }
    if (!isLeftToRight_) {
        itemsList.reverse();
    }
    if (direction_ == WrapDirection::HORIZONTAL) {
        contentList_.emplace_back(ContentInfo(mainLengthLimit_, totalCrossLength_, dialogButtonNum, itemsList));
    } else {
        contentList_.emplace_back(ContentInfo(totalCrossLength_, mainLengthLimit_, dialogButtonNum, itemsList));
    }
}

double RenderWrap::GetMainItemLength(const RefPtr<RenderNode>& item) const
{
    return direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE
               ? item->GetLayoutSize().Width()
               : item->GetLayoutSize().Height();
}

double RenderWrap::GetCrossItemLength(const RefPtr<RenderNode>& item) const
{
    return direction_ == WrapDirection::VERTICAL || direction_ == WrapDirection::VERTICAL_REVERSE
               ? item->GetLayoutSize().Width()
               : item->GetLayoutSize().Height();
}

void RenderWrap::PerformLayoutInitialize()
{
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        mainLengthLimit_ = GetLayoutParam().GetMaxSize().Width();
        crossLengthLimit_ = GetLayoutParam().GetMaxSize().Height();
    } else {
        mainLengthLimit_ =
            GetLayoutParam().GetMaxSize().IsInfinite() ? viewPort_.Height() : GetLayoutParam().GetMaxSize().Height();
        crossLengthLimit_ = GetLayoutParam().GetMaxSize().Width();
    }
}

Size RenderWrap::GetLeftSize(double crossLength, double mainLeftLength, double crossLeftLength) const
{
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        return Size(mainLeftLength, crossLeftLength - crossLength);
    } else {
        return Size(crossLeftLength - crossLength, mainLeftLength);
    }
}

void RenderWrap::LayoutWholeWrap()
{
    int32_t contentNum = static_cast<int32_t>(contentList_.size());
    if (contentNum == 0) {
        LOGW("no content in wrap");
        return;
    }
    Offset startPosition;
    Offset betweenPosition;
    bool isHorizontal = direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE;

    switch (alignment_) {
        case WrapAlignment::START: {
            startPosition = Offset(0.0, 0.0);
            betweenPosition = Offset();
            break;
        }
        case WrapAlignment::END: {
            startPosition = GetContentOffset(totalCrossLength_);
            betweenPosition = Offset();
            break;
        }
        case WrapAlignment::CENTER: {
            // divided the space by two
            startPosition = GetContentOffset(totalCrossLength_) / 2;
            betweenPosition = Offset();
            break;
        }
        case WrapAlignment::SPACE_BETWEEN: {
            startPosition = Offset(0.0, 0.0);
            double crossSpace =
                contentNum > 1 ? (crossLengthLimit_ - totalCrossLength_) / static_cast<double>(contentNum - 1) : 0.0;
            betweenPosition = isHorizontal ? Offset(0.0, crossSpace) : Offset(crossSpace, 0.0);
            break;
        }
        case WrapAlignment::SPACE_EVENLY: {
            double leftSpace = crossLengthLimit_ - totalCrossLength_;
            double crossSpace = leftSpace / static_cast<double>(contentNum + 1);
            startPosition = isHorizontal ? Offset(0.0, crossSpace) : Offset(crossSpace, 0.0);
            betweenPosition = isHorizontal ? Offset(0.0, crossSpace) : Offset(crossSpace, 0.0);
            break;
        }
        case WrapAlignment::SPACE_AROUND: {
            double leftSpace = crossLengthLimit_ - totalCrossLength_;
            double crossSpace = leftSpace / static_cast<double>(contentNum);
            startPosition = isHorizontal ? Offset(0.0, crossSpace / 2) : Offset(crossSpace / 2, 0.0);
            betweenPosition = isHorizontal ? Offset(0.0, crossSpace) : Offset(crossSpace, 0.0);
            break;
        }
        default: {
            LOGE("Wrap::alignment setting error.");
            startPosition = Offset(0.0, 0.0);
            betweenPosition = Offset();
            break;
        }
    }
    auto context = context_.Upgrade();
    bool applyNewOffset = context ? context->GetMinPlatformVersion() >= MIN_COMPATIBLE_VERSION : false;
    if (applyNewOffset) {
        // In content type, wrap is as large as children, no need to set alignment_.
        if ((!isHorizontal && horizontalMeasure_ == MeasureType::CONTENT) ||
            (isHorizontal && verticalMeasure_ == MeasureType::CONTENT)) {
            startPosition = Offset();
            betweenPosition = Offset();
        }
    }
    TraverseContent(startPosition, betweenPosition);
}

Offset RenderWrap::GetContentOffset(double totalCrossLength) const
{
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        return Offset(0.0, crossLengthLimit_ - totalCrossLength);
    } else {
        return Offset(crossLengthLimit_ - totalCrossLength, 0.0);
    }
}

void RenderWrap::TraverseContent(const Offset& startPosition, const Offset& betweenPosition) const
{
    // determine the content start position by main axis
    Offset accumulateOffset = startPosition;
    int32_t startItemIndex = 0;
    double currentMainSpaceLength = 0.0;
    for (const auto& content : contentList_) {
        // dfs positioned item in each content
        currentMainSpaceLength = mainLengthLimit_ - content.mainLength_;
        int32_t itemNum = content.count_;
        if (itemNum == 0) {
            LOGE("fail to TraverseContent due to item num is zero");
            return;
        }

        switch (mainAlignment_) {
            case WrapAlignment::START: {
                if ((direction_ == WrapDirection::HORIZONTAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::HORIZONTAL_REVERSE && isLeftToRight_) ||
                    (direction_ == WrapDirection::VERTICAL_REVERSE)) {
                    PositionedItem(0.0, content, accumulateOffset + GetItemMainOffset(currentMainSpaceLength),
                        content.crossLength_);
                } else {
                    PositionedItem(0.0, content, accumulateOffset, content.crossLength_);
                }
                break;
            }
            case WrapAlignment::END: {
                if ((direction_ == WrapDirection::HORIZONTAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::HORIZONTAL_REVERSE && isLeftToRight_) ||
                    direction_ == WrapDirection::VERTICAL_REVERSE) {
                    PositionedItem(0.0, content, accumulateOffset, content.crossLength_);
                } else {
                    PositionedItem(0.0, content, accumulateOffset + GetItemMainOffset(currentMainSpaceLength),
                        content.crossLength_);
                }
                break;
            }
            case WrapAlignment::CENTER: {
                // divided the space by two
                PositionedItem(0.0, content, accumulateOffset + GetItemMainOffset(currentMainSpaceLength / 2),
                    content.crossLength_);
                break;
            }
            case WrapAlignment::SPACE_BETWEEN: {
                double betweenSpace = (itemNum - 1 == 0) ? 0.0 : currentMainSpaceLength / (itemNum - 1);
                PositionedItem(betweenSpace, content, accumulateOffset, content.crossLength_);
                break;
            }
            case WrapAlignment::SPACE_AROUND: {
                double itemMainSpace = currentMainSpaceLength / itemNum;
                PositionedItem(itemMainSpace, content, accumulateOffset + GetItemMainOffset(itemMainSpace / 2),
                    content.crossLength_);
                break;
            }
            case WrapAlignment::SPACE_EVENLY: {
                double itemMainSpace = currentMainSpaceLength / (itemNum + 1);
                PositionedItem(
                    itemMainSpace, content, accumulateOffset + GetItemMainOffset(itemMainSpace), content.crossLength_);
                break;
            }
            default: {
                LOGE("Wrap::mainAlignment setting error. Now using START");
                PositionedItem(0.0, content, accumulateOffset, content.crossLength_);
                break;
            }
        }
        auto contentSpace = NormalizeToPx(contentSpace_);
        startItemIndex += itemNum;
        accumulateOffset += betweenPosition;
        accumulateOffset += (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE)
                                ? Offset(0.0, content.crossLength_ + contentSpace)
                                : Offset(content.crossLength_ + contentSpace, 0.0);
    }
}

Offset RenderWrap::GetItemMainOffset(double mainSpace) const
{
    // calculate the offset of each item in content
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        return Offset(mainSpace, 0.0);
    } else {
        return Offset(0.0, mainSpace);
    }
}

void RenderWrap::PositionedItem(
    double betweenSpace, const ContentInfo& content, const Offset& position, double totalCrossSpace) const
{
    Offset itemPositionOffset;
    // iterate every item in content
    for (const auto& item : content.itemList_) {
        switch (crossAlignment_) {
            case WrapAlignment::START: {
                if ((direction_ == WrapDirection::VERTICAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::VERTICAL_REVERSE && !isLeftToRight_)) {
                    HandleEndAlignment(totalCrossSpace, item, position, betweenSpace, itemPositionOffset);
                } else {
                    HandleStartAlignment(item, position, betweenSpace, itemPositionOffset);
                }
                break;
            }
            case WrapAlignment::STRETCH: {
                PlaceItemAndLog(item, position + itemPositionOffset, "STRETCH");
                // stretch the component in wrap
                LayoutParam layoutParam;
                auto spacing = NormalizeToPx(spacing_);
                if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
                    itemPositionOffset += Offset(item->GetLayoutSize().Width() + betweenSpace + spacing, 0.0);
                    layoutParam.SetFixedSize(Size(item->GetLayoutSize().Width(), totalCrossSpace));
                } else {
                    itemPositionOffset += Offset(0.0, item->GetLayoutSize().Height() + betweenSpace + spacing);
                    layoutParam.SetFixedSize(Size(totalCrossSpace, item->GetLayoutSize().Height()));
                }
                item->Layout(layoutParam);
                break;
            }
            case WrapAlignment::END: {
                if ((direction_ == WrapDirection::VERTICAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::VERTICAL_REVERSE && !isLeftToRight_)) {
                    HandleStartAlignment(item, position, betweenSpace, itemPositionOffset);
                } else {
                    HandleEndAlignment(totalCrossSpace, item, position, betweenSpace, itemPositionOffset);
                }
                break;
            }
            case WrapAlignment::CENTER: {
                // divide the space by two
                HandleCenterAlignment(totalCrossSpace, item, position, betweenSpace, itemPositionOffset);
                break;
            }
            case WrapAlignment::BASELINE: {
                if (direction_ == WrapDirection::VERTICAL || direction_ == WrapDirection::VERTICAL_REVERSE) {
                    if (isLeftToRight_) {
                        HandleStartAlignment(item, position, betweenSpace, itemPositionOffset);
                    } else {
                        HandleEndAlignment(totalCrossSpace, item, position, betweenSpace, itemPositionOffset);
                    }
                } else {
                    HandleBaselineAlignment(
                        content.maxBaselineDistance, item, position, betweenSpace, itemPositionOffset);
                }
                break;
            }
            default: {
                LOGW("Wrap::crossAlignment setting error. Now using START");
                if ((direction_ == WrapDirection::VERTICAL && !isLeftToRight_) ||
                    (direction_ == WrapDirection::VERTICAL_REVERSE && !isLeftToRight_)) {
                    HandleEndAlignment(totalCrossSpace, item, position, betweenSpace, itemPositionOffset);
                } else {
                    HandleStartAlignment(item, position, betweenSpace, itemPositionOffset);
                }
                break;
            }
        }
    }
}

void RenderWrap::PlaceItemAndLog(const RefPtr<RenderNode>& node, const Offset& position, const std::string& align) const
{
    node->SetPosition(position);
    LOGD("Wrap::PositionedItem %s item position x:%lf, y:%lf", align.c_str(), position.GetX(), position.GetY());
}

void RenderWrap::HandleCenterAlignment(double totalCrossSpace, const RefPtr<RenderNode>& item, const Offset& position,
    double betweenSpace, Offset& itemPositionOffset) const
{
    // itemPositionOffset will change in this function
    Offset crossOffset;
    auto spacing = NormalizeToPx(spacing_);
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        crossOffset = Offset(0.0, (totalCrossSpace - item->GetLayoutSize().Height()) / 2.0);
        PlaceItemAndLog(item, position + itemPositionOffset + crossOffset, "CENTER");
        itemPositionOffset += Offset(item->GetLayoutSize().Width() + betweenSpace + spacing, 0.0);
    } else {
        crossOffset = Offset((totalCrossSpace - item->GetLayoutSize().Width()) / 2, 0.0);
        PlaceItemAndLog(item, position + itemPositionOffset + crossOffset, "CENTER");
        itemPositionOffset += Offset(0.0, item->GetLayoutSize().Height() + betweenSpace + spacing);
    }
}

void RenderWrap::HandleEndAlignment(double totalCrossSpace, const RefPtr<RenderNode>& item, const Offset& position,
    double betweenSpace, Offset& itemPositionOffset) const
{
    // itemPositionOffset will change in this function
    Offset crossOffset;
    auto spacing = NormalizeToPx(spacing_);
    if (direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE) {
        crossOffset = Offset(0.0, totalCrossSpace - item->GetLayoutSize().Height());
        PlaceItemAndLog(item, position + itemPositionOffset + crossOffset, "END");
        itemPositionOffset += Offset(item->GetLayoutSize().Width() + betweenSpace + spacing, 0.0);
    } else {
        crossOffset = Offset(totalCrossSpace - item->GetLayoutSize().Width(), 0.0);
        PlaceItemAndLog(item, position + itemPositionOffset + crossOffset, "END");
        itemPositionOffset += Offset(0.0, item->GetLayoutSize().Height() + betweenSpace + spacing);
    }
}

void RenderWrap::HandleBaselineAlignment(double totalCrossSpace, const RefPtr<RenderNode>& item, const Offset& position,
    double betweenSpace, Offset& itemPositionOffset) const
{
    Offset crossOffset;
    auto spacing = NormalizeToPx(spacing_);
    crossOffset = Offset(0.0, totalCrossSpace - item->GetBaselineDistance(TextBaseline::ALPHABETIC));
    PlaceItemAndLog(item, position + itemPositionOffset + crossOffset, "Baseline");
    itemPositionOffset += Offset(item->GetLayoutSize().Width() + betweenSpace + spacing, 0.0);
}

void RenderWrap::HandleStartAlignment(
    const RefPtr<RenderNode>& item, const Offset& position, double betweenSpace, Offset& itemPositionOffset) const
{
    PlaceItemAndLog(item, position + itemPositionOffset, "START");
    // Decide content offset position
    auto spacing = NormalizeToPx(spacing_);
    bool isHorizontal = direction_ == WrapDirection::HORIZONTAL || direction_ == WrapDirection::HORIZONTAL_REVERSE;
    itemPositionOffset += Offset(isHorizontal ? item->GetLayoutSize().Width() + betweenSpace + spacing : 0.0,
        isHorizontal ? 0.0 : item->GetLayoutSize().Height() + betweenSpace + spacing);
}

void RenderWrap::ClearRenderObject()
{
    RenderNode::ClearRenderObject();
    direction_ = WrapDirection::VERTICAL;
    alignment_ = WrapAlignment::START;
    mainAlignment_ = WrapAlignment::START;
    crossAlignment_ = WrapAlignment::START;
    spacing_ = Dimension();
    contentSpace_ = Dimension();
    mainLengthLimit_ = 0.0;
    crossLengthLimit_ = 0.0;
    totalMainLength_ = 0.0;
    totalCrossLength_ = 0.0;

    dialogDirection_ = WrapDirection::HORIZONTAL;
    dialogStretch_ = false;
    isLeftToRight_ = true;
}

bool RenderWrap::MaybeRelease()
{
    auto context = GetContext().Upgrade();
    if (context && context->GetRenderFactory() && context->GetRenderFactory()->GetRenderWrapFactory()->Recycle(this)) {
        ClearRenderObject();
        return false;
    }
    return true;
}

} // namespace OHOS::Ace