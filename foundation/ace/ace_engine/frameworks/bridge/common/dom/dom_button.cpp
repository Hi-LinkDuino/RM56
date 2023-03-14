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

#include "frameworks/bridge/common/dom/dom_button.h"

#include "base/log/event_report.h"
#include "core/common/ace_application_info.h"
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

// Button types definition
const char BUTTON_TYPE_CAPSULE[] = "capsule";
const char BUTTON_TYPE_TEXT[] = "text";
const char BUTTON_TYPE_CIRCLE[] = "circle";
const char BUTTON_TYPE_DOWNLOAD[] = "download";
const char BUTTON_TYPE_ICON[] = "icon";
const char BUTTON_TYPE_ARC[] = "arc"; // for watch

// Button children placement definition
const char PLACEMENT_START[] = "start";
const char PLACEMENT_TOP[] = "top";
const char PLACEMENT_BOTTOM[] = "bottom";

// Watch button definitions
constexpr Dimension ARC_FONT_SIZE = 19.0_fp;
constexpr Dimension ARC_FONT_MIN_SIZE = 16.0_fp;
constexpr Dimension ARC_PADDING_TOP = 8.0_vp;
constexpr Dimension ARC_PADDING_BOTTOM = 0.0_vp;
constexpr Dimension ARC_PADDING_HORIZONTAL = 30.0_vp;
constexpr Dimension WATCH_TEXT_PADDING = 2.0_vp;
constexpr Dimension WATCH_TEXT_RADIUS = 4.0_vp;
constexpr uint32_t MAX_LINES = 2;

// TV button definitions
constexpr Dimension TEXT_PADDING_HORIZONTAL = 8.0_vp;
constexpr Dimension TEXT_PADDING_VERTICAL = 0.0_vp;
constexpr Dimension TEXT_FONT_MIN_SIZE = 12.0_fp;
constexpr double TEXT_FOCUS_HEIGHT = 24.0;

constexpr uint32_t TRANSPARENT_COLOR = 0x00000000;
constexpr double FLEX_ITEM_SHRINK = 1.0;
constexpr double INIT_HEIGHT = -1.0;
constexpr Dimension DOWNLOAD_BORDER_WIDTH = Dimension(1.0, DimensionUnit::VP);
constexpr Dimension ICON_BUTTON_PADDING = 0.0_vp;
constexpr Dimension ICON_BUTTON_RADIUS = 0.0_vp;
constexpr Dimension INNER_PADDING = 4.0_vp;

} // namespace

DOMButton::DOMButton(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    std::list<RefPtr<Component>> buttonChildren;
    buttonChild_ = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    textChild_ = AceType::MakeRefPtr<TextComponent>("");
    imageChild_ = AceType::MakeRefPtr<ImageComponent>("");
    paddingChild_ = AceType::MakeRefPtr<PaddingComponent>();
    buttonChild_->AppendChild(paddingChild_);
    isWatch_ = (SystemProperties::GetDeviceType() == DeviceType::WATCH);
    isTv_ = (SystemProperties::GetDeviceType() == DeviceType::TV);
}

void DOMButton::ResetInitializedStyle()
{
    if (declaration_) {
        declaration_->InitializeStyle();
    }
}

void DOMButton::PrepareSpecializedComponent()
{
    buttonTheme_ = GetTheme<ButtonTheme>();
    buttonDeclaration_ = AceType::DynamicCast<ButtonDeclaration>(declaration_);
    if (!buttonDeclaration_ || !buttonTheme_) {
        return;
    }
    textChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    imageChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    buttonChild_->SetLayoutFlag(LAYOUT_FLAG_EXTEND_TO_PARENT);
    buttonType_ = buttonDeclaration_->GetType();
    textStyle_ = buttonDeclaration_->GetTextStyle();
    edge_ = buttonDeclaration_->GetBorderEdge();
    blendOpacity_ = buttonDeclaration_->GetBlendOpacity();
    diameter_ = buttonDeclaration_->GetProgressDiameter();
    imageChild_->SetSrc(buttonDeclaration_->GetIconSrc());
    imageChild_->SetImageFill(GetImageFill());
    imageChild_->SetWidth(buttonDeclaration_->GetIconWidth());
    imageChild_->SetHeight(buttonDeclaration_->GetIconHeight());
    imageChild_->SetFitMaxSize(false);
    paddingChild_->SetPadding(buttonDeclaration_->GetPadding());
    textChild_->SetData(buttonDeclaration_->GetTextData());
    bool isCard = AceApplicationInfo::GetInstance().GetIsCardType();
    if (isCard) {
        textStyle_.SetAllowScale(false);
        if (textStyle_.GetFontSize().Unit() == DimensionUnit::FP) {
            textStyle_.SetAllowScale(true);
        }
    }
    if (!focusColorChanged_) {
        focusColor_ = buttonDeclaration_->GetFocusColor();
    }
    paddingChild_->SetChild(textChild_);
    PrepareBoxSize();
    PrepareUniversalButton();
    PrepareBorderStyle();
    PrepareBackDecorationStyle();
    PrepareButtonState();
    PreparePseudoStyle();
    if (!isTv_) {
        textChild_->SetFocusColor(textStyle_.GetTextColor());
        if (!isWatch_) {
            PrepareClickedColor();
        }
    }
    if (buttonDeclaration_->GetFontSizeState()) {
        textStyle_.SetAdaptTextSize(textStyle_.GetFontSize(), textStyle_.GetFontSize());
    }
    textChild_->SetTextStyle(textStyle_);
    buttonDeclaration_->SetBorderEdge(edge_);
    buttonChild_->SetDeclaration(buttonDeclaration_);
    AddPadding();
}

void DOMButton::PrepareBoxSize()
{
    if (!boxComponent_) {
        return;
    }
    boxComponent_->SetDeliverMinToChild(true);
    backDecoration_ = boxComponent_->GetBackDecoration();
    if (buttonType_ == BUTTON_TYPE_ARC) {
        return;
    }
    buttonChild_->SetWidth(buttonDeclaration_->GetWidth());
    buttonChild_->SetHeight(buttonDeclaration_->GetHeight());
    if (GreatOrEqual(buttonChild_->GetWidth().Value(), 0.0)) {
        boxComponent_->SetWidth(buttonChild_->GetWidth().Value(), buttonChild_->GetWidth().Unit());
    }
    if (GreatOrEqual(buttonChild_->GetHeight().Value(), 0.0)) {
        boxComponent_->SetHeight(buttonChild_->GetHeight().Value(), buttonChild_->GetHeight().Unit());
    }
    // Use theme height if user not define. Circle button will calculate height when rendering.
    if ((buttonType_ != BUTTON_TYPE_CIRCLE) && (LessNotEqual(buttonChild_->GetHeight().Value(), 0.0))) {
        if ((buttonType_ == BUTTON_TYPE_DOWNLOAD) && (SystemProperties::GetDeviceType() == DeviceType::PHONE)) {
            boxComponent_->SetHeight(buttonTheme_->GetDownloadHeight().Value(),
                buttonTheme_->GetDownloadHeight().Unit());
            return;
        }
        boxComponent_->SetHeight(buttonTheme_->GetHeight().Value(), buttonTheme_->GetHeight().Unit());
    }
}

void DOMButton::PreparePseudoStyle()
{
    if (!HasPseudo()) {
        return;
    }
    if (HasActivePseudo()) {
        buttonDeclaration_->SetClickedColor(buttonDeclaration_->GetBackgroundColor());
    }
    if (HasFocusPseudo()) {
        buttonDeclaration_->SetFocusColor(buttonDeclaration_->GetBackgroundColor());
    }
}

void DOMButton::PrepareUniversalButton()
{
    UpdateCustomizedColorFlag();
    if (buttonType_ == BUTTON_TYPE_ICON) {
        PrepareIconButton();
    } else if (buttonType_ == BUTTON_TYPE_CAPSULE) {
        PrepareCapsuleButton();
    } else if (buttonType_ == BUTTON_TYPE_TEXT) {
        PrepareTextButton();
    } else if (buttonType_ == BUTTON_TYPE_CIRCLE) {
        PrepareCircleButton();
    } else if (buttonType_ == BUTTON_TYPE_DOWNLOAD) {
        PrepareDownloadButton();
    } else if (buttonType_ == BUTTON_TYPE_ARC) {
        PrepareArcButton();
    } else {
        PrepareDefaultButton();
    }
}

void DOMButton::PrepareDefaultButton()
{
    paddingChild_->SetPadding(Edge());
    if (!buttonDeclaration_->GetHeightState()) {
        ResetBoxHeight(INIT_HEIGHT);
    }
    if (!imageChild_->GetSrc().empty()) {
        if (textChild_->GetData().empty()) {
            paddingChild_->SetChild(imageChild_);
            return;
        }
        textStyle_.DisableAdaptTextSize();
        PrepareChildren();
    }
}

void DOMButton::PrepareIconButton()
{
    buttonChild_->SetType(ButtonType::ICON);
    buttonDeclaration_->SetRectRadius(ICON_BUTTON_RADIUS);
    paddingChild_->SetChild(imageChild_);
    paddingChild_->SetPadding(Edge(ICON_BUTTON_PADDING));
    ResetBoxHeight(INIT_HEIGHT);
}

void DOMButton::PrepareCapsuleButton()
{
    buttonChild_->SetType(ButtonType::CAPSULE);
    if (!buttonDeclaration_->GetRadiusState()) {
        if (!NearZero(buttonChild_->GetHeight().Value())) {
            buttonDeclaration_->SetRectRadius(buttonChild_->GetHeight() / 2.0);
        }
    } else {
        ResetBoxHeight(buttonDeclaration_->GetRectRadius().Value() * 2.0, buttonDeclaration_->GetRectRadius().Unit());
    }
    if (isCustomizedColor_ && isTv_) {
        buttonDeclaration_->SetFocusColor(buttonDeclaration_->GetBackgroundColor());
        textChild_->SetFocusColor(textStyle_.GetTextColor());
    }
}

void DOMButton::PrepareTextButton()
{
    buttonChild_->SetType(ButtonType::TEXT);
    if (!isCustomizedColor_) {
        buttonDeclaration_->SetBackgroundColor(Color(TRANSPARENT_COLOR));
    }
    textStyle_.SetTextAlign(TextAlign::CENTER);
    if (isTv_) {
        textStyle_.SetAdaptTextSize(textStyle_.GetFontSize(), TEXT_FONT_MIN_SIZE);
        paddingChild_->SetPadding(Edge(TEXT_PADDING_HORIZONTAL, TEXT_PADDING_VERTICAL,
            TEXT_PADDING_HORIZONTAL, TEXT_PADDING_VERTICAL));
        if (!buttonDeclaration_->GetFontSizeState()) {
            ResetBoxHeight(TEXT_FOCUS_HEIGHT, DimensionUnit::VP);
            buttonDeclaration_->SetRectRadius(Dimension(TEXT_FOCUS_HEIGHT / 2.0, DimensionUnit::VP));
        }
        return;
    }
    if (isWatch_) {
        if (!buttonDeclaration_->GetFontSizeState()) {
            std::vector<TextSizeGroup> preferTextSizeGroups;
            preferTextSizeGroups.push_back({ buttonTheme_->GetTextStyle().GetFontSize(), 1 });
            preferTextSizeGroups.push_back({ buttonTheme_->GetMinFontSize(), MAX_LINES, TextOverflow::ELLIPSIS });
            textStyle_.SetPreferTextSizeGroups(preferTextSizeGroups);
        }
        ResetBoxHeight(INIT_HEIGHT);
        paddingChild_->SetPadding(Edge(WATCH_TEXT_PADDING));
        buttonDeclaration_->SetRectRadius(WATCH_TEXT_RADIUS);
        return;
    }
    if (!buttonDeclaration_->GetTextColorState()) {
        textStyle_.SetTextColor(buttonTheme_->GetNormalTextColor());
    }
}

void DOMButton::PrepareCircleButton()
{
    buttonChild_->SetType(ButtonType::CIRCLE);
    paddingChild_->SetPadding(Edge());
    if (!imageChild_->GetSrc().empty()) {
        paddingChild_->SetChild(imageChild_);
    }
    if (isCustomizedColor_ && isTv_) {
        buttonDeclaration_->SetFocusColor(buttonDeclaration_->GetBackgroundColor());
    }
}

void DOMButton::PrepareDownloadButton()
{
    buttonChild_->SetType(ButtonType::DOWNLOAD);
    if (!isWatch_) {
        edge_.SetWidth(DOWNLOAD_BORDER_WIDTH);
        buttonDeclaration_->SetProgressColor(buttonTheme_->GetDownloadProgressColor());
        if (!isTv_) {
            if (!buttonDeclaration_->GetRadiusState()) {
                buttonDeclaration_->SetRectRadius(buttonTheme_->GetDownloadHeight() / 2.0);
            }
            if (!buttonDeclaration_->GetBgColorState()) {
                buttonDeclaration_->SetBackgroundColor(buttonTheme_->GetDownloadBackgroundColor());
            }
            if (!buttonDeclaration_->GetTextColorState()) {
                textStyle_.SetTextColor(buttonTheme_->GetDownloadTextColor());
            }
            if (!buttonDeclaration_->GetFontSizeState()) {
                textStyle_.SetFontSize(buttonTheme_->GetDownloadFontSize());
            }
        }
        return;
    }
    if (!isCustomizedColor_) {
        buttonDeclaration_->SetBackgroundColor(Color(TRANSPARENT_COLOR));
    }
    if (!imageChild_->GetSrc().empty()) {
        paddingChild_->SetChild(imageChild_);
    }
    paddingChild_->SetPadding(Edge());
    buttonDeclaration_->SetProgressDiameter(diameter_);
    buttonDeclaration_->SetProgressColor(buttonDeclaration_->GetProgressColor());
    ResetBoxHeight(INIT_HEIGHT);
}

void DOMButton::PrepareArcButton()
{
    buttonChild_->SetType(ButtonType::ARC);
    textStyle_.SetAdaptTextSize(ARC_FONT_SIZE, ARC_FONT_MIN_SIZE);
    paddingChild_->SetPadding(Edge(ARC_PADDING_HORIZONTAL, ARC_PADDING_TOP,
        ARC_PADDING_HORIZONTAL, ARC_PADDING_BOTTOM));
}

void DOMButton::PrepareButtonState()
{
    UpdateCustomizedColorFlag();
    if (buttonDeclaration_->GetWaitingState()) {
        PrepareWaiting();
    } else {
        if (!textColorChanged_) {
            textColor_ = textStyle_.GetTextColor();
        }
        textStyle_.SetTextColor(textColor_);
        if (!disabledColorEffected_) {
            edgeColor_ = edge_.GetColor();
            disabledColor_ = buttonDeclaration_->GetDisabledColor();
        }
        edge_.SetColor(edgeColor_);
        if (focusColorChanged_) {
            buttonDeclaration_->SetFocusColor(focusColor_);
        }
    }
    if (buttonDeclaration_->GetDisabledState()) {
        if (HasDisabledPseudo()) {
            buttonDeclaration_->SetDisabledColor(buttonDeclaration_->GetBackgroundColor());
        } else {
            PrepareDisabledBackgroundColor();
            PrepareDisabledChildStyle();
        }
    }
}

void DOMButton::PrepareDisabledBackgroundColor()
{
    edge_.SetColor(edge_.GetColor().BlendOpacity(blendOpacity_));
    if ((SystemProperties::GetDeviceType() == DeviceType::PHONE) && (buttonType_ == BUTTON_TYPE_DOWNLOAD)) {
        buttonDeclaration_->SetProgressColor(buttonDeclaration_->GetProgressColor().BlendOpacity(blendOpacity_));
    }

    // Disabled background color not defined by user.
    if (disabledColor_ == Color()) {
        Color bgColor = buttonDeclaration_->GetBackgroundColor();
        Color customizedColor = (isWatch_ ? bgColor : bgColor.BlendOpacity(blendOpacity_));
        buttonDeclaration_->SetDisabledColor(isCustomizedColor_ ? customizedColor : buttonTheme_->GetDisabledColor());
    } else {
        buttonDeclaration_->SetDisabledColor(disabledColor_);
    }
    disabledColorEffected_ = true;
}

void DOMButton::PrepareDisabledChildStyle()
{
    bool isWatchDownload = isWatch_ && (buttonType_ == BUTTON_TYPE_DOWNLOAD);
    if ((buttonType_ == BUTTON_TYPE_CIRCLE) || isWatchDownload || buttonDeclaration_->GetWaitingState()) {
        auto displayChild = AceType::MakeRefPtr<DisplayComponent>(paddingChild_->GetChild());
        displayChild->SetOpacity(blendOpacity_);
        paddingChild_->SetChild(displayChild);
        return;
    }

    // Disabled text color not defined by user.
    Color disabledTextColor = buttonDeclaration_->GetDisabledTextColor();
    if (disabledTextColor == Color()) {
        Color textColor = textStyle_.GetTextColor().BlendOpacity(blendOpacity_);
        textStyle_.SetTextColor(isCustomizedColor_ ? textColor : buttonTheme_->GetTextDisabledColor());
    } else {
        textStyle_.SetTextColor(disabledTextColor);
    }
    textColorChanged_ = true;
}

void DOMButton::PrepareClickedColor()
{
    if (buttonDeclaration_->GetClickedColor() != buttonTheme_->GetClickedColor()) {
        return;
    }
    Color defaultClickedColor = buttonDeclaration_->GetBackgroundColor().BlendColor(buttonTheme_->GetClickedColor());
    buttonDeclaration_->SetClickedColor(defaultClickedColor);
}

void DOMButton::PrepareWaiting()
{
    if ((!buttonTheme_) || isWatch_ || (buttonType_ == BUTTON_TYPE_DOWNLOAD)) {
        return;
    }
    buttonDeclaration_->SetFocusColor(focusColor_.BlendOpacity(blendOpacity_));
    buttonDeclaration_->SetFocusAnimationColor(buttonTheme_->GetBgFocusColor().BlendOpacity(blendOpacity_));
    focusColorChanged_ = true;
    if (buttonType_ == BUTTON_TYPE_CIRCLE) {
        diameter_ = LessNotEqual(buttonChild_->GetWidth().Value(), 0.0)
                    ? buttonDeclaration_->GetRectRadius()
                    : std::min(buttonChild_->GetHeight(), buttonChild_->GetWidth()) / 2.0;
    }
    auto progressComponent = AceType::MakeRefPtr<LoadingProgressComponent>();
    progressComponent->SetDiameter(diameter_);
    progressComponent->SetProgressColor(buttonDeclaration_->GetProgressColor());
    if ((buttonType_ == BUTTON_TYPE_CIRCLE) || (buttonType_ == BUTTON_TYPE_TEXT) || textChild_->GetData().empty()) {
        paddingChild_->SetChild(progressComponent);
        return;
    }
    PrepareWaitingWithText(progressComponent);
}

void DOMButton::PrepareWaitingWithText(const RefPtr<LoadingProgressComponent>& progressComponent)
{
    if (!progressComponent) {
        return;
    }
    if (!isCustomizedColor_) {
        textStyle_.SetTextColor(buttonTheme_->GetTextWaitingColor());
        textColorChanged_ = true;
    }
    textStyle_.DisableAdaptTextSize();
    auto innerPadding = AceType::MakeRefPtr<PaddingComponent>();
    Edge edge;
    edge.SetLeft(buttonDeclaration_->GetInnerPadding());
    innerPadding->SetChild(textChild_);
    innerPadding->SetPadding(edge);
    auto flexItemProgress = AceType::MakeRefPtr<FlexItemComponent>(0.0, 0.0, 0.0, progressComponent);
    auto flexItemText = AceType::MakeRefPtr<FlexItemComponent>(0.0, 0.0, 0.0, innerPadding);
    flexItemText->SetFlexShrink(FLEX_ITEM_SHRINK);
    std::list<RefPtr<Component>> children;
    children.emplace_back(flexItemProgress);
    children.emplace_back(flexItemText);
    auto rowComponent = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, children);
    paddingChild_->SetChild(rowComponent);
}

void DOMButton::PrepareBorderStyle()
{
    if (!isCustomizedColor_) {
        return;
    }
    if ((buttonType_ == BUTTON_TYPE_CAPSULE) || (buttonType_ == BUTTON_TYPE_CIRCLE)) {
        edge_.SetColor(buttonTheme_->GetBorderColor());
        edge_.SetWidth(buttonTheme_->GetBorderWidth());
    }
}

void DOMButton::PrepareBackDecorationStyle()
{
    if (!backDecoration_) {
        return;
    }
    if (backDecoration_->GetImage() || backDecoration_->GetGradient().IsValid()) {
        buttonDeclaration_->SetBackgroundColor(Color(TRANSPARENT_COLOR));
    }
    if (buttonChild_->GetType() == ButtonType::CIRCLE) {
        return;
    }
    auto border = backDecoration_->GetBorder();
    if (!HasBorderRadiusStyle() || buttonDeclaration_->GetRadiusState()) {
        if (buttonDeclaration_->GetRectRadius().Unit() == border.Top().GetWidth().Unit()) {
            backDecoration_->SetBorderRadius(Radius(buttonDeclaration_->GetRectRadius() + border.Top().GetWidth()));
        }
    } else {
        if (border.TopLeftRadius().GetX().Unit() == border.Top().GetWidth().Unit()) {
            buttonDeclaration_->SetRectRadius(border.TopLeftRadius().GetX() - border.Top().GetWidth());
        }
    }
}

void DOMButton::PrepareChildren()
{
    Edge edge;
    placement_ = buttonDeclaration_->GetPlacement();
    if (placement_ == PLACEMENT_BOTTOM) {
        edge.SetBottom(INNER_PADDING);
    } else if (placement_ == PLACEMENT_TOP) {
        edge.SetTop(INNER_PADDING);
    } else if (placement_ == PLACEMENT_START) {
        edge.SetLeft(INNER_PADDING);
    } else {
        edge.SetRight(INNER_PADDING);
        edge.SetBottom(Dimension(1.0, DimensionUnit::PX));
    }
    innerPaddingChild_ = AceType::MakeRefPtr<PaddingComponent>();
    innerPaddingChild_->SetChild(textChild_);
    innerPaddingChild_->SetPadding(edge);
    PrepareChildrenLayout();

}

void DOMButton::PrepareChildrenLayout()
{
    auto flexItemText = AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0, innerPaddingChild_);
    auto flexItemIcon = AceType::MakeRefPtr<FlexItemComponent>(0.0, 0.0, 0.0, imageChild_);
    std::list<RefPtr<Component>> children;
    if ((placement_ == PLACEMENT_START) || (placement_ == PLACEMENT_TOP)) {
        children.emplace_back(flexItemIcon);
        children.emplace_back(flexItemText);
    } else {
        children.emplace_back(flexItemText);
        children.emplace_back(flexItemIcon);
    }
    auto flexComponent = AceType::MakeRefPtr<FlexComponent>(FlexDirection::ROW, FlexAlign::CENTER,
        FlexAlign::CENTER, children);
    if ((placement_ == PLACEMENT_TOP) || (placement_ == PLACEMENT_BOTTOM)) {
        flexComponent->SetDirection(FlexDirection::COLUMN);
    }
    flexComponent->SetMainAxisSize(MainAxisSize::MIN);
    paddingChild_->SetChild(flexComponent);
}

void DOMButton::AddPadding()
{
    if (!boxComponent_) {
        return;
    }
    auto edge = boxComponent_->GetPadding();
    if (edge == Edge::NONE) {
        return;
    }
    paddingChild_->SetPadding(edge);
    boxComponent_->SetPadding(Edge());
}

void DOMButton::ResetBoxHeight(double height, DimensionUnit unit)
{
    if (!boxComponent_) {
        return;
    }
    boxComponent_->SetHeight(height, unit);
}

void DOMButton::UpdateCustomizedColorFlag()
{
    isCustomizedColor_ = buttonDeclaration_->GetBackgroundColor() != buttonTheme_->GetBgColor();
}

Dimension DOMButton::GetHeight() const
{
    Dimension height = Dimension(-1.0, DimensionUnit::PX);
    auto buttonDeclaration = buttonDeclaration_;
    if (IsPlatformFive()) {
        // less api 5 should set height of box component in UpdateBoxSize, buttonDeclaration_ != nullptr after
        // PrepareSpecializedComponent
        buttonDeclaration = AceType::DynamicCast<ButtonDeclaration>(declaration_);
    }
    if (buttonDeclaration) {
        height = buttonDeclaration->GetHeight();
    }
    return height;
}

Dimension DOMButton::GetWidth() const
{
    Dimension width = Dimension(-1.0, DimensionUnit::PX);
    auto buttonDeclaration = buttonDeclaration_;
    if (IsPlatformFive()) {
        // less api 5 should set width of box component in UpdateBoxSize, buttonDeclaration_ != nullptr after
        // PrepareSpecializedComponent
        buttonDeclaration = AceType::DynamicCast<ButtonDeclaration>(declaration_);
    }
    if (buttonDeclaration) {
        width = buttonDeclaration->GetWidth();
    }

    return width;
}

bool DOMButton::IsPlatformFive() const
{
    const static int32_t PLATFORM_VERSION_FIVE = 5;
    auto context = GetPipelineContext().Upgrade();
    return context && context->GetMinPlatformVersion() <= PLATFORM_VERSION_FIVE;
}

} // namespace OHOS::Ace::Framework
