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

#include "core/components/text_overlay/text_overlay_component.h"

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/clip/clip_component.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/flex/flex_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components/text_overlay/render_text_overlay.h"
#include "core/components/text_overlay/text_overlay_element.h"
#include "core/components/theme/theme_manager.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {
namespace {

constexpr char BUTTON_COPY_ALL[] = "textoverlay.select_all";
constexpr char BUTTON_CUT[] = "textoverlay.cut";
constexpr char BUTTON_COPY[] = "textoverlay.copy";
constexpr char BUTTON_PASTE[] = "textoverlay.paste";
constexpr char BUTTON_TRANSLATE[] = "textoverlay.translate";
constexpr char BUTTON_SHARE[] = "textoverlay.share";
constexpr char BUTTON_SEARCH[] = "textoverlay.search";
constexpr char OVERLAY_TRANSLATE[] = "translate";
constexpr char OVERLAY_SHARE[] = "share";
constexpr char OVERLAY_SEARCH[] = "search";

constexpr Dimension OVERLAY_MARGIN_BOTTOM = 8.0_vp;
constexpr Dimension TOOL_BAR_HEIGHT = 40.0_vp;

} // namespace

TextOverlayComponent::TextOverlayComponent(
    const RefPtr<ThemeManager>& themeManager, const RefPtr<AccessibilityManager>& accessibilityManager)
{
    themeManager_ = themeManager;
    accessibilityManager_ = accessibilityManager;
    InitThemeStyle(themeManager);
}

RefPtr<Element> TextOverlayComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextOverlayElement>();
}

RefPtr<RenderNode> TextOverlayComponent::CreateRenderNode()
{
    return RenderTextOverlay::Create();
}

void TextOverlayComponent::InitThemeStyle(const RefPtr<ThemeManager>& themeManager)
{
    theme_ = themeManager->GetTheme<TextOverlayTheme>();
    if (!theme_) {
        return;
    }
    handleColor_ = theme_->GetHandleColor();
    handleColorInner_ = theme_->GetHandleColorInner();
    handleDiameter_ = theme_->GetHandleDiameter();
    handleDiameterInner_ = theme_->GetHandleDiameterInner();
    menuSpacingWithText_ = theme_->GetMenuSpacingWithText();
}

RefPtr<Component> TextOverlayComponent::BuildChild(
    bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation)
{
    // if type of input is password, don't show tool menu.
    if (isPassword_) {
        return nullptr;
    }
    if (!hasToolBar && !hasMenu && !hasIcon) {
        return nullptr;
    }
    std::list<RefPtr<Component>> columnChildren;
    auto column = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::SPACE_AROUND, FlexAlign::FLEX_END, columnChildren);
    if (GetTextDirection() == TextDirection::RTL) {
        column->SetCrossAxisAlign(FlexAlign::FLEX_START);
    }
    column->SetMainAxisSize(MainAxisSize::MIN);

    // Add toolbar.
    column->AppendChild(BuildToolBar(isSingleHandle, hasToolBar, hasMenu, hasIcon, hasAnimation));
    if (hasMenu) {
        // Add menu.
        column->AppendChild(BuildMenu());
    } else {
        menu_.Reset();
    }

    // Add focus collaboration to show focus animation.
    auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
    focusCollaboration->InsertChild(0, column);
    return focusCollaboration;
}

RefPtr<Component> TextOverlayComponent::BuildToolBar(
    bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation)
{
    if (!hasToolBar && !hasMenu && !hasIcon) {
        return nullptr;
    }

    std::list<RefPtr<Component>> operations;
    auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::SPACE_AROUND, FlexAlign::FLEX_START, operations);
    row->SetMainAxisSize(MainAxisSize::MIN);
    row->SetStretchToParent(true);
    if (hasToolBar) {
        if (!isSingleHandle) {
            if (onCut_) {
                row->AppendChild(
                    BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_CUT), cutButtonMarker_));
            }
            if (onCopy_) {
                row->AppendChild(
                    BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY), copyButtonMarker_));
            }
        }
        if (onPaste_) {
            row->AppendChild(
                BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_PASTE), pasteButtonMarker_));
        }
        if (onCopyAll_) {
            row->AppendChild(
                BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY_ALL), copyAllButtonMarker_));
        }
        if (!translateButtonMarker_.IsEmpty()) {
            const auto& translateButtonMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
            BackEndEventManager<void()>::GetInstance().BindBackendEvent(
                translateButtonMarker, [weak = WeakClaim(this)]() {
                    auto overlay = weak.Upgrade();
                    if (overlay) {
                        overlay->OnToolBarButtonClick(overlay->translateButtonMarker_, OVERLAY_TRANSLATE);
                    }
                });
            row->AppendChild(
                BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_TRANSLATE), translateButtonMarker));
        }
    }
    if (hasIcon && (!options_.empty() || !shareButtonMarker_.IsEmpty() || !searchButtonMarker_.IsEmpty())) {
        hasMoreButton_ = true;
        row->AppendChild(BuildMoreIconButton(hasMenu));
    } else {
        hasMoreButton_ = false;
    }
    auto firstButton = DynamicCast<ButtonComponent>(row->GetChildren().front());
    if (firstButton) {
        firstButton->SetAutoFocusState(true);
    }
    auto box = AceType::MakeRefPtr<BoxComponent>();
    if (theme_) {
        auto backDecoration = AceType::MakeRefPtr<Decoration>();
        backDecoration->SetBackgroundColor(theme_->GetMenuBackgroundColor());
        backDecoration->SetBorder(theme_->GetMenuBorder());
        backDecoration->SetBorderRadius(Radius(theme_->GetMenuButtonHeight()));
        box->SetBackDecoration(backDecoration);
        box->SetPadding(theme_->GetMenuPadding());
    }

    if (hasAnimation) {
        if (innerComposeId_.empty()) {
            innerComposeId_ = TweenComponent::AllocTweenComponentId();
        }
        auto innerTween = AceType::MakeRefPtr<TweenComponent>(innerComposeId_, innerComposeId_, row);
        innerTween->SetIsFirstFrameShow(true);
        box->SetChild(innerTween);
    } else {
        box->SetChild(row);
    }
    box->SetTextDirection(GetTextDirection());
    return BuildAnimation(box, hasAnimation);
}

RefPtr<ButtonComponent> TextOverlayComponent::BuildButton(const std::string& data, const EventMarker& onClick)
{
    if (!theme_) {
        return nullptr;
    }
    auto text = AceType::MakeRefPtr<TextComponent>(data);
    text->SetTextStyle(theme_->GetMenuButtonTextStyle());
    text->SetFocusColor(theme_->GetMenuButtonTextStyle().GetTextColor());

    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetPadding(theme_->GetMenuButtonPadding());
    padding->SetChild(text);

    std::list<RefPtr<Component>> children;
    children.emplace_back(padding);
    auto button = AceType::MakeRefPtr<ButtonComponent>(children);
    button->SetIsInnerBorder(true);
    button->SetClickedEventId(onClick);
    button->SetHeight(theme_->GetMenuButtonHeight());
    button->SetRectRadius(theme_->GetMenuButtonHeight() / 2.0);
    button->SetBackgroundColor(theme_->GetMenuBackgroundColor());
    button->SetHoverColor(theme_->GetButtonHoverColor());
    button->SetClickedColor(theme_->GetButtonClickedColor());
    button->SetFocusColor(theme_->GetMenuBackgroundColor());
    return button;
}

RefPtr<ButtonComponent> TextOverlayComponent::BuildMoreIconButton(bool hasMenu)
{
    if (!theme_) {
        return nullptr;
    }

    std::list<RefPtr<Component>> children;
    auto button = AceType::MakeRefPtr<ButtonComponent>(children);
    button->SetIsInnerBorder(true);
    button->SetClickedEventId(moreButtonMarker_);
    button->SetHeight(theme_->GetMenuButtonHeight());
    button->SetWidth(theme_->GetMenuButtonHeight());
    button->SetRectRadius(theme_->GetMenuButtonHeight() / 2.0);
    button->SetBackgroundColor(Color::TRANSPARENT);
    button->SetHoverColor(theme_->GetButtonHoverColor());
    button->SetClickedColor(theme_->GetButtonClickedColor());
    button->SetFocusColor(theme_->GetMenuBackgroundColor());
    return button;
}

RefPtr<Component> TextOverlayComponent::BuildAnimation(const RefPtr<Component>& child, bool hasAnimation)
{
    auto box = AceType::MakeRefPtr<BoxComponent>();
    if (hasAnimation) {
        auto clip = AceType::MakeRefPtr<ClipComponent>(child);
        clip->SetTopLeftRadius(Radius(theme_->GetMenuButtonHeight()));
        clip->SetTopRightRadius(Radius(theme_->GetMenuButtonHeight()));
        clip->SetBottomLeftRadius(Radius(theme_->GetMenuButtonHeight()));
        clip->SetBottomRightRadius(Radius(theme_->GetMenuButtonHeight()));
        clip->SetHeight(TOOL_BAR_HEIGHT);
        clip->SetClipWithShadow(true);

        if (outerComposeId_.empty()) {
            outerComposeId_ = TweenComponent::AllocTweenComponentId();
        }
        auto outerTween = AceType::MakeRefPtr<TweenComponent>(outerComposeId_, outerComposeId_, clip);
        outerTween->SetIsFirstFrameShow(true);
        box->SetChild(outerTween);
    } else {
        box->SetChild(child);
    }

    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->AddShadow(ShadowConfig::DefaultShadowM);
    backDecoration->SetBackgroundColor(theme_->GetMenuBackgroundColor());
    backDecoration->SetBorder(theme_->GetMenuBorder());
    backDecoration->SetBorderRadius(Radius(theme_->GetMenuButtonHeight()));
    box->SetBackDecoration(backDecoration);
    return box;
}

RefPtr<Component> TextOverlayComponent::BuildMenu()
{
    if (!menu_) {
        menu_ = AceType::MakeRefPtr<SelectPopupComponent>();
        menu_->ClearAllOptions();

        if (!shareButtonMarker_.IsEmpty()) {
            auto optionComponent = BuildMenuOption("", InternalResource::ResourceId::SHARE_SVG,
                Localization::GetInstance()->GetEntryLetters(BUTTON_SHARE), true);
            const auto& shareButtonMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
            optionComponent->SetClickEvent(shareButtonMarker);
            BackEndEventManager<void()>::GetInstance().BindBackendEvent(shareButtonMarker, [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->OnToolBarButtonClick(overlay->shareButtonMarker_, OVERLAY_SHARE);
                }
            });
            menu_->AppendSelectOption(optionComponent);
        }
        if (!searchButtonMarker_.IsEmpty()) {
            auto optionComponent = BuildMenuOption("", InternalResource::ResourceId::SEARCH_SVG,
                Localization::GetInstance()->GetEntryLetters(BUTTON_SEARCH), true);
            const auto& searchButtonMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
            optionComponent->SetClickEvent(searchButtonMarker);
            BackEndEventManager<void()>::GetInstance().BindBackendEvent(searchButtonMarker, [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->OnToolBarButtonClick(overlay->searchButtonMarker_, OVERLAY_SEARCH);
                }
            });
            menu_->AppendSelectOption(optionComponent);
        }

        int32_t index = 0;
        for (const auto& option : options_) {
            auto optionComponent =
                BuildMenuOption(option.image, InternalResource::ResourceId::NO_ID, option.text, false);
            EventMarker clickEvent = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
            optionComponent->SetClickEvent(clickEvent);
            BackEndEventManager<void()>::GetInstance().BindBackendEvent(clickEvent, [weak = WeakClaim(this), index]() {
                auto overlay = weak.Upgrade();
                overlay->OnOptionClick(index);
            });
            menu_->AppendSelectOption(optionComponent);
            ++index;
        }
        menu_->SetIsFullScreen(false);
        menu_->InitTheme(themeManager_);
        menu_->Initialize(accessibilityManager_.Upgrade());
    }

    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(menu_);
    box->SetMargin(Edge(0.0_vp, OVERLAY_MARGIN_BOTTOM, 0.0_vp, 0.0_vp));
    return box;
}

RefPtr<OptionComponent> TextOverlayComponent::BuildMenuOption(
    const std::string& imageSrc, InternalResource::ResourceId resourceId, const std::string& text, bool useResource)
{
    auto optionComponent = AceType::MakeRefPtr<OptionComponent>();
    RefPtr<ImageComponent> image;
    if (useResource) {
        image = AceType::MakeRefPtr<ImageComponent>(resourceId);
        if (theme_) {
            // fill color only effect svg image color
            image->SetImageFill(theme_->GetMenuIconColor());
        }
    } else {
        image = AceType::MakeRefPtr<ImageComponent>(imageSrc);
        if (image) {
            // fill color only effect svg image color
            image->SetImageFill(imageFill_);
        }
    }
    optionComponent->SetIcon(image);
    auto textComponent = AceType::MakeRefPtr<TextComponent>(text);
    optionComponent->SetText(textComponent);
    optionComponent->SetValue(text);
    optionComponent->InitTheme(themeManager_);
    optionComponent->Initialize(accessibilityManager_.Upgrade());
    return optionComponent;
}

void TextOverlayComponent::OnOptionClick(int32_t index)
{
    const auto& event = AceAsyncEvent<void(const std::string&)>::Create(optionsClickMarker_, context_);
    if (event) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("index", index);
        jsonResult->Put("value", GetSelectedText().c_str());
        event(std::string(R"("optionselect",)").append(jsonResult->ToString()));
    }
    if (popOverlay_) {
        popOverlay_();
    }
}

void TextOverlayComponent::OnToolBarButtonClick(const EventMarker& marker, const std::string& eventName)
{
    const auto& event = AceAsyncEvent<void(const std::string&)>::Create(marker, context_);
    if (event) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("value", GetSelectedText().c_str());
        if (eventName == OVERLAY_TRANSLATE) {
            event(std::string(R"("translate",)").append(jsonResult->ToString()));
        } else if (eventName == OVERLAY_SHARE) {
            event(std::string(R"("share",)").append(jsonResult->ToString()));
        } else if (eventName == OVERLAY_SEARCH) {
            event(std::string(R"("search",)").append(jsonResult->ToString()));
        }
    }
    if (popOverlay_) {
        popOverlay_();
    }
}

std::string TextOverlayComponent::GetSelectedText() const
{
    const auto& textField = weakTextField_.Upgrade();
    if (textField) {
        return textField->GetEditingValue().GetSelectedText();
    }
    return "";
}

bool TextOverlayComponent::HasMoreButton() const
{
    return hasMoreButton_;
}

void TextOverlayComponent::SetIsPassword(bool isPassword)
{
    isPassword_ = isPassword;
}

bool TextOverlayComponent::GetIsPassword() const
{
    return isPassword_;
}

void TextOverlayComponent::SetIsSingleHandle(bool isSingleHandle)
{
    isSingleHandle_ = isSingleHandle;
}

bool TextOverlayComponent::GetIsSingleHandle() const
{
    return isSingleHandle_;
}

void TextOverlayComponent::SetLineHeight(double lineHeight)
{
    lineHeight_ = lineHeight;
}

double TextOverlayComponent::GetLineHeight() const
{
    return lineHeight_;
}

void TextOverlayComponent::SetClipRect(const Rect& clipRect)
{
    clipRect_ = clipRect;
}

const Rect& TextOverlayComponent::GetClipRect() const
{
    return clipRect_;
}

void TextOverlayComponent::SetHandleColor(const Color& handleColor)
{
    handleColor_ = handleColor;
}

const Color& TextOverlayComponent::GetHandleColor() const
{
    return handleColor_;
}

void TextOverlayComponent::SetHandleColorInner(const Color& handleColorInner)
{
    handleColorInner_ = handleColorInner;
}

const Color& TextOverlayComponent::GetHandleColorInner() const
{
    return handleColorInner_;
}

void TextOverlayComponent::SetHandleDiameter(const Dimension& handleDiameter)
{
    handleDiameter_ = handleDiameter;
}

const Dimension& TextOverlayComponent::GetHandleDiameter() const
{
    return handleDiameter_;
}

void TextOverlayComponent::SetHandleDiameterInner(const Dimension& handleDiameterInner)
{
    handleDiameterInner_ = handleDiameterInner;
}

const Dimension& TextOverlayComponent::GetHandleDiameterInner() const
{
    return handleDiameterInner_;
}

void TextOverlayComponent::SetMenuSpacingWithText(const Dimension& menuSpacingWithText)
{
    menuSpacingWithText_ = menuSpacingWithText;
}

const Dimension& TextOverlayComponent::GetMenuSpacingWithText() const
{
    return menuSpacingWithText_;
}

void TextOverlayComponent::SetOnFocusChange(const std::function<void(bool, bool)>& onFocusChange)
{
    onFocusChange_ = onFocusChange;
}

const std::function<void(bool, bool)>& TextOverlayComponent::GetOnFocusChange() const
{
    return onFocusChange_;
}

void TextOverlayComponent::SetOnCut(const CommonCallback& onCut)
{
    onCut_ = onCut;
}

const CommonCallback& TextOverlayComponent::GetOnCut() const
{
    return onCut_;
}

void TextOverlayComponent::SetOnCopy(const CommonCallback& onCopy)
{
    onCopy_ = onCopy;
}

const CommonCallback& TextOverlayComponent::GetOnCopy() const
{
    return onCopy_;
}

void TextOverlayComponent::SetOnPaste(const CommonCallback& onPaste)
{
    onPaste_ = onPaste;
}

const CommonCallback& TextOverlayComponent::GetOnPaste() const
{
    return onPaste_;
}

void TextOverlayComponent::SetOnCopyAll(const CopyAllCallback& onCopyAll)
{
    onCopyAll_ = onCopyAll;
}

const CopyAllCallback& TextOverlayComponent::GetOnCopyAll() const
{
    return onCopyAll_;
}

void TextOverlayComponent::SetCutButtonMarker(const EventMarker& cutButtonMarker)
{
    cutButtonMarker_ = cutButtonMarker;
}

const EventMarker& TextOverlayComponent::GetCutButtonMarker() const
{
    return cutButtonMarker_;
}

void TextOverlayComponent::SetCopyButtonMarker(const EventMarker& copyButtonMarker)
{
    copyButtonMarker_ = copyButtonMarker;
}

const EventMarker& TextOverlayComponent::GetCopyButtonMarker() const
{
    return copyButtonMarker_;
}

void TextOverlayComponent::SetPasteButtonMarker(const EventMarker& pasteButtonMarker)
{
    pasteButtonMarker_ = pasteButtonMarker;
}

const EventMarker& TextOverlayComponent::GetPasteButtonMarker() const
{
    return pasteButtonMarker_;
}

void TextOverlayComponent::SetCopyAllButtonMarker(const EventMarker& copyAllButtonMarker)
{
    copyAllButtonMarker_ = copyAllButtonMarker;
}

const EventMarker& TextOverlayComponent::GetCopyAllButtonMarker() const
{
    return copyAllButtonMarker_;
}

void TextOverlayComponent::SetMoreButtonMarker(const EventMarker& moreButtonMarker)
{
    moreButtonMarker_ = moreButtonMarker;
}

const EventMarker& TextOverlayComponent::GetMoreButtonMarker() const
{
    return moreButtonMarker_;
}

void TextOverlayComponent::SetStartHandleOffset(const Offset& offset)
{
    startHandleOffset_ = offset;
}

const Offset& TextOverlayComponent::GetStartHandleOffset() const
{
    return startHandleOffset_;
}

void TextOverlayComponent::SetEndHandleOffset(const Offset& offset)
{
    endHandleOffset_ = offset;
}

const Offset& TextOverlayComponent::GetEndHandleOffset() const
{
    return endHandleOffset_;
}

void TextOverlayComponent::SetOnStartHandleMove(const StartHandleMoveCallback& onStartHandleMove)
{
    onStartHandleMove_ = onStartHandleMove;
}

const StartHandleMoveCallback& TextOverlayComponent::GetOnStartHandleMove() const
{
    return onStartHandleMove_;
}

void TextOverlayComponent::SetOnEndHandleMove(const EndHandleMoveCallback& onEndHandleMove)
{
    onEndHandleMove_ = onEndHandleMove;
}

const EndHandleMoveCallback& TextOverlayComponent::GetOnEndHandleMove() const
{
    return onEndHandleMove_;
}

void TextOverlayComponent::SetWeakTextField(const WeakPtr<RenderTextField>& weakTextField)
{
    weakTextField_ = weakTextField;
}

const WeakPtr<RenderTextField>& TextOverlayComponent::GetWeakTextField() const
{
    return weakTextField_;
}

void TextOverlayComponent::SetRealTextDirection(TextDirection realTextDirection)
{
    realTextDirection_ = realTextDirection;
}

TextDirection TextOverlayComponent::GetRealTextDirection() const
{
    return realTextDirection_;
}

void TextOverlayComponent::SetOptions(const std::vector<InputOption>& options)
{
    options_ = options;
}

const std::vector<InputOption>& TextOverlayComponent::GetOptions() const
{
    return options_;
}

void TextOverlayComponent::SetImageFill(const std::optional<Color>& imageFill)
{
    imageFill_ = imageFill;
}

void TextOverlayComponent::SetOptionsClickMarker(const EventMarker& onOptionsClick)
{
    optionsClickMarker_ = onOptionsClick;
}

void TextOverlayComponent::SetTranslateButtonMarker(const EventMarker& onTranslate)
{
    translateButtonMarker_ = onTranslate;
}

void TextOverlayComponent::SetShareButtonMarker(const EventMarker& onShare)
{
    shareButtonMarker_ = onShare;
}

void TextOverlayComponent::SetSearchButtonMarker(const EventMarker& onSearch)
{
    searchButtonMarker_ = onSearch;
}

void TextOverlayComponent::SetPopOverlay(const CommonCallback& popOverlay)
{
    popOverlay_ = popOverlay;
}

void TextOverlayComponent::SetContext(const WeakPtr<PipelineContext>& context)
{
    context_ = context;
}

const RefPtr<SelectPopupComponent>& TextOverlayComponent::GetMenu() const
{
    return menu_;
}

} // namespace OHOS::Ace
