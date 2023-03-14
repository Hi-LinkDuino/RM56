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

#include "core/components/search/render_search.h"

#include "base/json/json_util.h"
#include "base/utils/system_properties.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/divider/divider_component.h"
#include "core/components/divider/render_divider.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/components/text_field/text_field_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension SEARCH_SPACING = 2.0_vp;
constexpr Dimension DIVIDER_WIDTH = 1.0_px;
constexpr Dimension TEXT_PADDING = 14.0_vp;
constexpr Dimension SEARCH_TEXT_HEIGHT = 36.0_vp;
constexpr Dimension SEARCH_BUTTON_TEXT_FONT_SIZE = 14.0_fp;
const Color SEARCH_BUTTON_TEXT_COLOR = Color(0xFF254FF7);

} // namespace

void RenderSearch::Update(const RefPtr<Component>& component)
{
    searchComponent_ = AceType::DynamicCast<SearchComponent>(component);
    if (!searchComponent_) {
        LOGW("component is null");
        return;
    }
    needReverse_ = (searchComponent_->GetTextDirection() == TextDirection::RTL);
    closeIconSize_ = searchComponent_->GetCloseIconSize();
    placeHoldStyle_ = searchComponent_->GetPlaceHoldStyle();
    editingStyle_ = searchComponent_->GetEditingStyle();
    closeIconHotZoneHorizontal_ = searchComponent_->GetCloseIconHotZoneHorizontal();

    decoration_ = searchComponent_->GetDecoration();
    if (searchComponent_->GetTextEditController() && textEditController_ != searchComponent_->GetTextEditController()) {
        if (textEditController_) {
            textEditController_->RemoveObserver(WeakClaim(this));
        }
        textEditController_ = searchComponent_->GetTextEditController();
        textEditController_->AddObserver(WeakClaim(this));
    }

    hoverColor_ = searchComponent_->GetHoverColor();
    pressColor_ = searchComponent_->GetPressColor();
    CreateRenderImage(searchComponent_);
    CreateRenderButton(searchComponent_);

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (searchComponent_->GetOnChange()) {
            changeEvent_ = *searchComponent_->GetOnChange();
        }

        if (searchComponent_->GetOnSubmit()) {
            submitEvent_ = *searchComponent_->GetOnSubmit();
        }
    } else {
        changeEvent_ = AceAsyncEvent<void(const std::string)>::Create(searchComponent_->GetChangeEventId(), context_);
        submitEvent_ = AceAsyncEvent<void(const std::string)>::Create(searchComponent_->GetSubmitEventId(), context_);
    }

    MarkNeedLayout();
}

void RenderSearch::FireSubmitEvent(const std::string& searchKey)
{
    if (submitEvent_) {
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            submitEvent_(searchKey);
        } else {
            auto submitResult = JsonUtil::Create(true);
            submitResult->Put("text", searchKey.c_str());
            submitEvent_(std::string(R"("submit",)").append(submitResult->ToString()));
        }
    }
}

void RenderSearch::PerformLayout()
{
    const auto& renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
    if (!renderTextField) {
        return;
    }
    renderTextField->Layout(GetLayoutParam());
    SetLayoutSize(renderTextField->GetLayoutSize());
    renderTextField->SetSubmitEvent([weak = WeakClaim(this)](const std::string& searchKey) {
        auto renderSearch = weak.Upgrade();
        if (renderSearch) {
            renderSearch->FireSubmitEvent(searchKey);
        }
    });

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        renderTextField->SetOnValueChangeEvent(changeEvent_);
    } else {
        renderTextField->SetOnTextChangeEvent(changeEvent_);
    }

    Size deflateSize = Size(NormalizeToPx(SEARCH_SPACING), NormalizeToPx(SEARCH_SPACING)) * 2.0;
    if (context && decoration_) {
        deflateSize += decoration_->GetBorder().GetLayoutSize(context->GetDipScale());
    }
    LayoutParam layoutParam = LayoutParam(GetLayoutSize() - deflateSize, Size());
    if (renderCloseIcon_) {
        renderCloseIcon_->Layout(layoutParam);
    }
    if (renderSearchBox_) {
        renderSearchBox_->Layout(layoutParam);
    }

    InitRect(renderTextField);
}

void RenderSearch::InitRect(const RefPtr<RenderTextField>& renderTextField)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    Border border;
    if (decoration_) {
        border = decoration_->GetBorder();
    }
    double leftBorderWidth = NormalizeToPx(border.Left().GetWidth());
    double topBorderWidth = NormalizeToPx(border.Top().GetWidth());
    double rightBorderWidth = NormalizeToPx(border.Right().GetWidth());
    double bottomBorderWidth = NormalizeToPx(border.Bottom().GetWidth());

    double searchHeight = GetLayoutSize().Height();
    double searchInnerHeight = searchHeight - topBorderWidth - bottomBorderWidth;

    // Compute rect of search text.
    double searchBoxSpacing = NormalizeToPx(SEARCH_SPACING) * 2.0 + NormalizeToPx(DIVIDER_WIDTH);
    if (renderSearchBox_) {
        double searchBoxVerticalOffset =
            topBorderWidth + (searchInnerHeight - renderSearchBox_->GetLayoutSize().Height()) / 2.0;
        double searchBoxWidth = renderSearchBox_->GetLayoutSize().Width();
        Offset searchTextOffset = Offset(
            GetLayoutSize().Width() - rightBorderWidth - searchBoxWidth - searchBoxSpacing, searchBoxVerticalOffset);
        if (needReverse_) {
            searchTextOffset = Offset(leftBorderWidth, searchBoxVerticalOffset);
        }
        searchTextRect_ = Rect(searchTextOffset, renderSearchBox_->GetLayoutSize() + Size(searchBoxSpacing, 0.0));
    } else {
        searchTextRect_ = Rect();
    }

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        double padding = searchTextRect_.Width() + rightBorderWidth +
                         NormalizeToPx(closeIconHotZoneHorizontal_) -
                         (NormalizeToPx(closeIconSize_) / 2.0);
        renderTextField->SetPaddingHorizonForSearch(padding);
    } else {
        renderTextField->SetPaddingHorizonForSearch(searchTextRect_.Width());
    }

    renderTextField->MarkNeedLayout();

    // Compute rect of close icon.
    if (renderCloseIcon_) {
        double iconVerticalOffset =
            topBorderWidth + (searchInnerHeight - renderCloseIcon_->GetLayoutSize().Height()) / 2.0;
        double iconHotzonOffset = (NormalizeToPx(closeIconHotZoneHorizontal_) - NormalizeToPx(closeIconSize_)) / 2.0;
        Offset closeIconOffset = Offset(GetLayoutSize().Width() - rightBorderWidth - searchTextRect_.Width() -
                                            renderCloseIcon_->GetLayoutSize().Width() - iconHotzonOffset,
            iconVerticalOffset);
        if (needReverse_) {
            closeIconOffset = Offset(leftBorderWidth + searchTextRect_.Width() + iconHotzonOffset, iconVerticalOffset);
        }
        closeIconRect_ = Rect(closeIconOffset, renderCloseIcon_->GetLayoutSize());

        // Compute rect of hot zone of close icon.
        double searchSpacing = NormalizeToPx(SEARCH_SPACING);
        double horizonSpacing = iconHotzonOffset - searchSpacing;
        double verticalSpacing =
            std::min(iconHotzonOffset, (searchInnerHeight - renderCloseIcon_->GetLayoutSize().Height()) / 2.0) -
            searchSpacing;
        closeIconHotZoneRect_ =
            closeIconRect_ - Offset(horizonSpacing, verticalSpacing) + Size(horizonSpacing, verticalSpacing) * 2.0;
    } else {
        closeIconRect_ = Rect();
        closeIconHotZoneRect_ = Rect();
    }
}

void RenderSearch::OnValueChanged(bool needFireChangeEvent, bool needFireSelectChangeEvent)
{
    if (textEditController_) {
        const auto& currentText = textEditController_->GetValue().text;
        showCloseIcon_ = !currentText.empty();
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
            if (showCloseIcon_) {
                renderTextField->SetTextStyle(editingStyle_);
            } else {
                renderTextField->SetTextStyle(placeHoldStyle_);
            }
        }
    }
}

void RenderSearch::CreateRenderImage(const RefPtr<SearchComponent>& searchComponent)
{
    if (!searchComponent) {
        return;
    }
    if (!renderCloseIcon_ && (SystemProperties::GetDeviceType() != DeviceType::TV)) {
        const auto& closeIconSrc = searchComponent->GetCloseIconSrc();
        RefPtr<ImageComponent> imageComponent;
        if (closeIconSrc.empty()) {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::CLOSE_SVG);
        } else {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(closeIconSrc);
        }
        imageComponent->SetWidth(searchComponent->GetCloseIconSize());
        imageComponent->SetHeight(searchComponent->GetCloseIconSize());

        renderCloseIcon_ = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
        if (!renderCloseIcon_) {
            return;
        }
        renderCloseIcon_->Attach(GetContext());
        renderCloseIcon_->Update(imageComponent);
    }
}

void RenderSearch::CreateRenderButton(const RefPtr<SearchComponent>& searchComponent)
{
    if (!searchComponent) {
        return;
    }
    if (searchComponent->GetSearchText().empty()) {
        searchText_ = "";
        renderSearchBox_.Reset();
        return;
    }
    if (searchText_ != searchComponent->GetSearchText()) {
        searchText_ = searchComponent->GetSearchText();
        renderSearchBox_.Reset();
    }
    if (!renderSearchBox_ && (SystemProperties::GetDeviceType() != DeviceType::TV)) {
        const auto& searchText = searchComponent->GetSearchText();
        const auto& textComponent = AceType::MakeRefPtr<TextComponent>(searchText);
        TextStyle textStyle;
        textStyle.SetMaxLines(1);
        textStyle.SetTextColor(SEARCH_BUTTON_TEXT_COLOR);
        textStyle.SetFontSize(SEARCH_BUTTON_TEXT_FONT_SIZE);
        textStyle.SetFontWeight(FontWeight::W500);
        textComponent->SetTextStyle(textStyle);
        auto renderText = AceType::DynamicCast<RenderText>(textComponent->CreateRenderNode());
        if (!renderText) {
            return;
        }
        renderText->Attach(GetContext());
        renderText->Update(textComponent);

        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        Edge edge = Edge(TEXT_PADDING, 0.0_vp, TEXT_PADDING, 0.0_vp);
        boxComponent->SetPadding(edge);
        boxComponent->SetHeight(SEARCH_TEXT_HEIGHT.Value(), SEARCH_TEXT_HEIGHT.Unit());
        renderSearchBox_ = AceType::DynamicCast<RenderBox>(boxComponent->CreateRenderNode());
        if (!renderSearchBox_) {
            return;
        }
        renderSearchBox_->Attach(GetContext());
        renderSearchBox_->Update(boxComponent);
        renderSearchBox_->AddChild(renderText);
        renderSearchBox_->SyncRSNode(nullptr);
        renderText->SyncRSNode(nullptr);
    }
}

bool RenderSearch::HandleEnterEvent()
{
    auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
    if (!renderTextField || !textEditController_) {
        return false;
    }

    if (focusRender_ == SearchNodeType::IMAGE) {
        renderTextField->SetNeedNotifyChangeEvent(true);
        textEditController_->Clear();
        focusRender_ = SearchNodeType::NONE;
        MarkNeedRender();
        return true;
    } else if (focusRender_ == SearchNodeType::BUTTON) {
        std::string searchKey = textEditController_->GetText();
        FireSubmitEvent(searchKey);
        focusRender_ = SearchNodeType::NONE;
        MarkNeedRender();
        return true;
    }
    return false;
}

bool RenderSearch::HandleFocusEvent(bool vertical, bool reverse)
{
    if (vertical) {
        focusRender_ = SearchNodeType::NONE;
        return false;
    }
    if (!(showCloseIcon_ && renderCloseIcon_) && !renderSearchBox_) {
        focusRender_ = SearchNodeType::NONE;
        return false;
    }

    auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
    if (!renderTextField) {
        focusRender_ = SearchNodeType::NONE;
        return false;
    }
    bool result = false;
    if ((showCloseIcon_ && renderCloseIcon_) && renderSearchBox_) {
        if (focusRender_ == SearchNodeType::NONE) {
            if (!reverse) {
                focusRender_ = SearchNodeType::IMAGE;
                result = true;
            }
        } else if (focusRender_ == SearchNodeType::IMAGE) {
            if (!reverse) {
                focusRender_ = SearchNodeType::BUTTON;
                result = true;
            } else {
                focusRender_ = SearchNodeType::NONE;
                renderTextField->UpdateFocusAnimation();
                result = true;
            }
        } else if (focusRender_ == SearchNodeType::BUTTON) {
            if (reverse) {
                focusRender_ = SearchNodeType::IMAGE;
                result = true;
            }
        }
    } else {
        if (focusRender_ == SearchNodeType::NONE) {
            if (!reverse) {
                focusRender_ = renderSearchBox_ ? SearchNodeType::BUTTON : SearchNodeType::IMAGE;
                result = true;
            }
        } else {
            if (reverse) {
                focusRender_ = SearchNodeType::NONE;
                renderTextField->UpdateFocusAnimation();
                result = true;
            }
        }
    }
    if (result) {
        MarkNeedRender();
    }
    return result;
}

bool RenderSearch::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGD("OnTouchTest: type is %{public}s, the region is %{public}lf, %{public}lf, %{public}lf, %{public}lf",
        GetTypeName(), GetTouchRect().Left(), GetTouchRect().Top(), GetTouchRect().Width(), GetTouchRect().Height());
    LOGD("OnTouchTest: the local point refer to parent is %{public}lf, %{public}lf, ", parentLocalPoint.GetX(),
        parentLocalPoint.GetY());
    if (GetDisableTouchEvent() || disabled_) {
        return false;
    }
    if (showCloseIcon_ && closeIconHotZoneRect_.IsInRegion(parentLocalPoint)) {
        hoverOrPressRender_ = SearchNodeType::IMAGE;
    } else if (searchTextRect_.IsInRegion(parentLocalPoint)) {
        hoverOrPressRender_ = SearchNodeType::BUTTON;
    } else {
        hoverOrPressRender_ = SearchNodeType::NONE;
    }
    // Since the paintRect is relative to parent, use parent local point to perform touch test.
    if (closeIconHotZoneRect_.IsInRegion(parentLocalPoint) || searchTextRect_.IsInRegion(parentLocalPoint)) {
        // Calculates the coordinate offset in this node.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return true;
    }
    if (GetPaintRect().IsInRegion(parentLocalPoint)) {
        // Calculates the local point location in this node.
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        bool dispatchSuccess = false;
        for (auto iter = GetChildren().rbegin(); iter != GetChildren().rend(); ++iter) {
            auto& child = *iter;
            if (child->TouchTest(globalPoint, localPoint, touchRestrict, result)) {
                dispatchSuccess = true;
                break;
            }
            if (child->InterceptTouchEvent()) {
                break;
            }
        }
        return dispatchSuccess;
    }
    return false;
}

void RenderSearch::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!rawRecognizer_) {
        rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        auto weak = WeakClaim(this);
        rawRecognizer_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto search = weak.Upgrade();
            if (search) {
                search->overlayColor_ = search->pressColor_;
                search->MarkNeedRender();
            }
        });

        rawRecognizer_->SetOnTouchUp([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto search = weak.Upgrade();
            if (search) {
                search->overlayColor_ = Color::TRANSPARENT;
                search->MarkNeedRender();
            }
        });

        rawRecognizer_->SetOnTouchCancel([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto search = weak.Upgrade();
            if (search) {
                search->overlayColor_ = Color::TRANSPARENT;
                search->MarkNeedRender();
            }
        });
    }
    rawRecognizer_->SetTouchRestrict(touchRestrict);
    rawRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawRecognizer_);

    if (!clickRecognizer_) {
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
            auto search = weak.Upgrade();
            if (search) {
                search->HandleClick();
            }
        });
    }
    clickRecognizer_->SetTouchRestrict(touchRestrict);
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);
}

void RenderSearch::HandleClick()
{
    auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
    if (!renderTextField || !textEditController_) {
        return;
    }
    std::string accessibilityEventType = "click";
    SendAccessibilityEvent(accessibilityEventType);
    if (hoverOrPressRender_ == SearchNodeType::IMAGE) {
        renderTextField->SetNeedNotifyChangeEvent(true);
        textEditController_->Clear();
        auto onValueChange = renderTextField->GetOnValueChange();
        if (onValueChange) {
            onValueChange();
        }
    } else if (hoverOrPressRender_ == SearchNodeType::BUTTON) {
        std::string searchKey = textEditController_->GetText();
        renderTextField->CloseKeyboard(true);
        FireSubmitEvent(searchKey);
    }
}

bool RenderSearch::MouseHoverTest(const Point& parentLocalPoint)
{
    auto context = context_.Upgrade();
    if (!context) {
        hoverOrPressRender_ = SearchNodeType::NONE;
        return false;
    }
    if (!InTouchRectList(parentLocalPoint, GetTouchRectList())) {
        if (mouseState_ == MouseState::HOVER) {
            mouseState_ = MouseState::NONE;
            hoverOrPressRender_ = SearchNodeType::NONE;
        }
        CancelTextFieldHover();
        return false;
    }
    context->AddToHoverList(AceType::WeakClaim(this).Upgrade());

    if (showCloseIcon_ && closeIconHotZoneRect_.IsInRegion(parentLocalPoint)) {
        if (mouseState_ == MouseState::NONE || hoverOrPressRender_ == SearchNodeType::BUTTON) {
            mouseState_ = MouseState::HOVER;
            hoverOrPressRender_ = SearchNodeType::IMAGE;
            CancelTextFieldHover();
            return true;
        }
    }

    if (searchTextRect_.IsInRegion(parentLocalPoint)) {
        if (mouseState_ == MouseState::NONE || hoverOrPressRender_ == SearchNodeType::IMAGE) {
            mouseState_ = MouseState::HOVER;
            hoverOrPressRender_ = SearchNodeType::BUTTON;
            CancelTextFieldHover();
            return true;
        }
    }

    bool hoverInSearch = !searchTextRect_.IsInRegion(parentLocalPoint);
    if (showCloseIcon_ && hoverInSearch) {
        hoverInSearch = !closeIconHotZoneRect_.IsInRegion(parentLocalPoint);
    }
    if (hoverInSearch) {
        hoverOrPressRender_ = SearchNodeType::NONE;
        mouseState_ = MouseState::NONE;

        // Change textfield to hover.
        auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
        if (renderTextField) {
            renderTextField->SetOverlayColor(hoverColor_);
            renderTextField->MarkNeedRender();
        }
    }
    return true;
}

void RenderSearch::CancelTextFieldHover()
{
    auto renderTextField = AceType::DynamicCast<RenderTextField>(GetChildren().front());
    if (renderTextField) {
        renderTextField->SetOverlayColor(Color::TRANSPARENT);
        renderTextField->MarkNeedRender();
    }
}

} // namespace OHOS::Ace
