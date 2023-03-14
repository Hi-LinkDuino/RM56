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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/menu/menu_component.h"
#include "core/components/option/option_component.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/text_field/render_text_field.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/text_overlay/text_overlay_theme.h"
#include "core/components/tween/tween_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/sole_child_component.h"
#include "frameworks/core/animation/animator.h"

namespace OHOS::Ace {

constexpr Dimension ICON_SIZE = 24.0_vp;
constexpr Dimension ICON_PADDING = 2.0_vp;

using CommonCallback = std::function<void()>;
using StartHandleMoveCallback =
    std::function<void(int32_t end, const Offset&, const std::function<void(const Offset&)>&, bool isSingleHandle)>;
using EndHandleMoveCallback =
    std::function<void(int32_t start, const Offset&, const std::function<void(const Offset&)>&)>;
using CopyAllCallback = std::function<void(const std::function<void(const Offset&, const Offset&)>&)>;

class TextOverlayComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(TextOverlayComponent, SoleChildComponent)

public:
    TextOverlayComponent(
        const RefPtr<ThemeManager>& themeManager, const RefPtr<AccessibilityManager>& accessibilityManager);
    ~TextOverlayComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void InitThemeStyle(const RefPtr<ThemeManager>& themeManager);
    RefPtr<Component> BuildChild(
        bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation = true);
    RefPtr<Component> BuildToolBar(bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation);

    void SetWeakTextField(const WeakPtr<RenderTextField>& weakTextField);
    const WeakPtr<RenderTextField>& GetWeakTextField() const;

    bool HasMoreButton() const;

    void SetIsPassword(bool isPassword);
    bool GetIsPassword() const;

    void SetIsSingleHandle(bool isSingleHandle);
    bool GetIsSingleHandle() const;

    void SetLineHeight(double lineHeight);
    double GetLineHeight() const;

    void SetClipRect(const Rect& clipRect);
    const Rect& GetClipRect() const;

    void SetHandleColor(const Color& handleColor);
    const Color& GetHandleColor() const;

    void SetHandleColorInner(const Color& handleColorInner);
    const Color& GetHandleColorInner() const;

    void SetHandleDiameter(const Dimension& handleDiameter);
    const Dimension& GetHandleDiameter() const;

    void SetHandleDiameterInner(const Dimension& handleDiameterInner);
    const Dimension& GetHandleDiameterInner() const;

    void SetMenuSpacingWithText(const Dimension& menuSpacingWithText);
    const Dimension& GetMenuSpacingWithText() const;

    void SetOnFocusChange(const std::function<void(bool, bool)>& onFocusChange);
    const std::function<void(bool, bool)>& GetOnFocusChange() const;

    void SetOnCut(const CommonCallback& onCut);
    const CommonCallback& GetOnCut() const;

    void SetOnCopy(const CommonCallback& onCopy);
    const CommonCallback& GetOnCopy() const;

    void SetOnPaste(const CommonCallback& onPaste);
    const CommonCallback& GetOnPaste() const;

    void SetOnCopyAll(const CopyAllCallback& onCopyAll);
    const CopyAllCallback& GetOnCopyAll() const;

    void SetCutButtonMarker(const EventMarker& cutButtonMarker);
    const EventMarker& GetCutButtonMarker() const;

    void SetCopyButtonMarker(const EventMarker& copyButtonMarker);
    const EventMarker& GetCopyButtonMarker() const;

    void SetPasteButtonMarker(const EventMarker& pasteButtonMarker);
    const EventMarker& GetPasteButtonMarker() const;

    void SetCopyAllButtonMarker(const EventMarker& copyAllButtonMarker);
    const EventMarker& GetCopyAllButtonMarker() const;

    void SetMoreButtonMarker(const EventMarker& moreButtonMarker);
    const EventMarker& GetMoreButtonMarker() const;

    void SetStartHandleOffset(const Offset& offset);
    const Offset& GetStartHandleOffset() const;

    void SetEndHandleOffset(const Offset& offset);
    const Offset& GetEndHandleOffset() const;

    void SetOnStartHandleMove(const StartHandleMoveCallback& onStartHandleMove);
    const StartHandleMoveCallback& GetOnStartHandleMove() const;

    void SetOnEndHandleMove(const EndHandleMoveCallback& onEndHandleMove);
    const EndHandleMoveCallback& GetOnEndHandleMove() const;

    void SetRealTextDirection(TextDirection realTextDirection);
    TextDirection GetRealTextDirection() const;

    void SetOptions(const std::vector<InputOption>& options);
    const std::vector<InputOption>& GetOptions() const;

    void SetImageFill(const std::optional<Color>& imageFill);
    void SetOptionsClickMarker(const EventMarker& onOptionsClick);
    void SetTranslateButtonMarker(const EventMarker& onTranslate);
    void SetShareButtonMarker(const EventMarker& onShare);
    void SetSearchButtonMarker(const EventMarker& onSearch);
    void SetPopOverlay(const CommonCallback& popOverlay);
    void SetContext(const WeakPtr<PipelineContext>& context);
    void SetShowOption(const TweenOption& tweenOption);
    void SetHideOption(const TweenOption& tweenOption);

    const RefPtr<SelectPopupComponent>& GetMenu() const;

private:
    RefPtr<ButtonComponent> BuildButton(const std::string& data, const EventMarker& onClick);
    RefPtr<ButtonComponent> BuildMoreIconButton(bool hasMenu);
    RefPtr<Component> BuildMenu();
    RefPtr<OptionComponent> BuildMenuOption(const std::string& imageSrc, InternalResource::ResourceId resourceId,
        const std::string& text, bool useResource);
    RefPtr<Component> BuildAnimation(const RefPtr<Component>& child, bool hasAnimation);
    void OnOptionClick(int32_t index);
    void OnToolBarButtonClick(const EventMarker& marker, const std::string& eventName);
    std::string GetSelectedText() const;

    WeakPtr<PipelineContext> context_;
    WeakPtr<RenderTextField> weakTextField_;
    RefPtr<TextOverlayTheme> theme_;
    RefPtr<SelectPopupComponent> menu_;
    RefPtr<ThemeManager> themeManager_;
    WeakPtr<AccessibilityManager> accessibilityManager_;

    bool hasMoreButton_ = false;
    bool isPassword_ = false;
    bool isSingleHandle_ = false;
    double lineHeight_ = 0.0;
    Rect clipRect_;
    Color handleColor_;
    Color handleColorInner_;
    std::optional<Color> imageFill_;
    Dimension handleDiameter_;
    Dimension handleDiameterInner_;
    Dimension menuSpacingWithText_;
    Offset startHandleOffset_;
    Offset endHandleOffset_;
    TextDirection realTextDirection_ = TextDirection::LTR;
    ComposeId innerComposeId_;
    ComposeId outerComposeId_;

    CommonCallback popOverlay_;
    CommonCallback onCut_;
    CommonCallback onCopy_;
    CommonCallback onPaste_;
    CopyAllCallback onCopyAll_;
    StartHandleMoveCallback onStartHandleMove_;
    EndHandleMoveCallback onEndHandleMove_;
    std::function<void(bool, bool)> onFocusChange_;

    EventMarker optionsClickMarker_;
    EventMarker translateButtonMarker_;
    EventMarker shareButtonMarker_;
    EventMarker searchButtonMarker_;
    EventMarker cutButtonMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker copyButtonMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker pasteButtonMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker copyAllButtonMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker moreButtonMarker_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();

    std::vector<InputOption> options_; // Options for menu.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_COMPONENT_H
