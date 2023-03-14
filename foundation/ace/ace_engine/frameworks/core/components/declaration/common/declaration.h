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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "core/components/declaration/common/attribute.h"
#include "core/components/declaration/common/event.h"
#include "core/components/declaration/common/method.h"
#include "core/components/declaration/common/style.h"
#include "core/components/scroll/scroll_position_controller.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/core/components/transform/click_spring_effect.h"
#include "base/geometry/calc_dimension.h"

namespace OHOS::Ace {

using OnSetAttributeFunc = std::function<void()>;
using OnSetStyleFinishedFunc = std::function<void()>;
using CachePseudoClassStyleFunc = std::function<void(std::pair<std::string, std::string>)>;

class ACE_EXPORT Declaration : public virtual AceType {
    DECLARE_ACE_TYPE(Declaration, AceType);

public:
    Declaration();
    ~Declaration() override;

    virtual void SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs);
    virtual void SetStyle(const std::vector<std::pair<std::string, std::string>>& styles);
    virtual void AddEvent(int32_t pageId, const std::string& eventId, const std::vector<std::string>& events);
    virtual void CallMethod(const std::string& method, const std::string& args);
    virtual void OnRequestFocus(bool shouldFocus);
    virtual void OnScrollBy(double dx, double dy, bool isSmooth);
    virtual void SetShowAttr(const std::string& showValue);
    // Initialize declaration theme style when created.
    virtual void InitializeStyle() {}
    virtual void Clear() {}

    Attribute& GetAttribute(AttributeTag tag) const;
    Style& GetStyle(StyleTag tag) const;
    Event& GetEvent(EventTag tag) const;
    Method& GetMethod(MethodTag tag) const;

    template<class T>
    T& MaybeResetAttribute(AttributeTag tag)
    {
        auto& attr = static_cast<T&>(GetAttribute(tag));
        if (!attr.IsValid() || !attr.IsShared()) {
            return attr;
        }
        auto newAttr = std::make_shared<T>(attr);
        newAttr->isShared = false;
        attributes_[tag] = newAttr;
        return *newAttr;
    }

    template<class T>
    T& MaybeResetStyle(StyleTag tag)
    {
        auto& style = static_cast<T&>(GetStyle(tag));
        if (!style.IsValid() || !style.IsShared()) {
            return style;
        }
        auto newStyle = std::make_shared<T>(style);
        newStyle->isShared = false;
        styles_[tag] = newStyle;
        return *newStyle;
    }

    template<class T>
    T& MaybeResetEvent(EventTag tag)
    {
        auto& event = static_cast<T&>(GetEvent(tag));
        if (!event.IsValid() || !event.IsShared()) {
            return event;
        }
        auto newEvent = std::make_shared<T>(event);
        newEvent->isShared = false;
        events_[tag] = newEvent;
        return *newEvent;
    }

    template<class T>
    T& MaybeResetMethod(MethodTag tag)
    {
        auto& method = static_cast<T&>(GetMethod(tag));
        if (!method.IsValid() || !method.IsShared()) {
            return method;
        }
        auto newMethod = std::make_shared<T>(method);
        newMethod->isShared = false;
        methods_[tag] = newMethod;
        return *newMethod;
    }

    void Init();
    void SetCurrentStyle(const std::pair<std::string, std::string>& style);
    void BindPipelineContext(const WeakPtr<PipelineContext>& pipelineContext);
    void ResetDefaultStyles();
    void SetClickEvent(const EventMarker& onClick);
    void SetRemoteMessageEvent(const EventMarker& remoteMessage);

    void SetOnSetAttribute(const OnSetAttributeFunc& onSetAttribute)
    {
        onSetAttribute_ = onSetAttribute;
    }

    void SetOnSetStyleFinished(const OnSetStyleFinishedFunc& onSetStyleFinished)
    {
        onSetStyleFinished_ = onSetStyleFinished;
    }

    void SetCachePseudoClassStyle(const CachePseudoClassStyleFunc& cachePseudoClassStyle)
    {
        cachePseudoClassStyle_ = cachePseudoClassStyle;
    }

    void SetFocusableController(const RefPtr<FocusableController>& focusableController)
    {
        focusableController_ = focusableController;
    }

    void SetPositionController(const RefPtr<ScrollPositionController>& positionController)
    {
        positionController_ = positionController;
    }

    const RefPtr<Decoration>& GetBackDecoration() const
    {
        return backDecoration_;
    }

    void SetBackDecoration(const RefPtr<Decoration>& backDecoration)
    {
        backDecoration_ = backDecoration;
    }

    const RefPtr<Decoration>& GetFrontDecoration() const
    {
        return frontDecoration_;
    }

    void SetShareId(const std::string& shareId)
    {
        shareId_ = shareId;
    }

    const std::string& GetShareId() const
    {
        return shareId_;
    }

    void SetIsSubscriptEnable(bool isSubscriptEnable)
    {
        isSubscriptEnable_ = isSubscriptEnable;
    }

    bool IsDisabled() const
    {
        return isDisabled_;
    }

    bool IsWaiting() const
    {
        return isWaiting_;
    }

    bool IsChecked() const
    {
        return isChecked_;
    }

    void SetIsChecked(bool isChecked)
    {
        isChecked_ = isChecked;
    }

    bool IsHover() const
    {
        return isHover_;
    }

    void SetIsHover(bool isHover)
    {
        isHover_ = isHover;
    }

    bool HasTransformStyle() const
    {
        return hasTransformStyle_;
    }

    bool HasBackGroundColor() const
    {
        return hasBackGroundColor_;
    }
    void SetHasBackGroundColor(bool hasBackGroundColor)
    {
        hasBackGroundColor_ = hasBackGroundColor;
    }

    bool HasDecorationStyle() const
    {
        return hasDecorationStyle_;
    }
    void SetHasDecorationStyle(bool hasDecorationStyle)
    {
        hasDecorationStyle_ = hasDecorationStyle;
    }

    bool HasPositionProcessed() const
    {
        return hasPositionProcessed_;
    }
    void SetHasPositionProcessed(bool hasPositionProcessed)
    {
        hasPositionProcessed_ = hasPositionProcessed;
    }

    bool HasBoxStyle() const
    {
        return hasBoxStyle_;
    }
    void SetHasBoxStyle(bool hasBoxStyle)
    {
        hasBoxStyle_ = hasBoxStyle;
    }

    bool HasShadowStyle() const
    {
        return hasShadowStyle_;
    }
    void SetHasShadowStyle(bool hasShadowStyle)
    {
        hasShadowStyle_ = hasShadowStyle;
    }

    bool HasFrontDecorationStyle() const
    {
        return hasFrontDecorationStyle_;
    }
    void SetHasFrontDecorationStyle(bool hasFrontDecorationStyle)
    {
        hasFrontDecorationStyle_ = hasFrontDecorationStyle;
    }

    bool HasBorderStyle() const
    {
        return hasBorderStyle_;
    }
    void SetHasBorderStyle(bool hasBorderStyle)
    {
        hasBorderStyle_ = hasBorderStyle;
    }

    bool HasBorderRadiusStyle() const
    {
        return hasBorderRadiusStyle_;
    }
    void SetHasBorderRadiusStyle(bool hasBorderRadiusStyle)
    {
        hasBorderRadiusStyle_ = hasBorderRadiusStyle;
    }

    bool HasClickEffect() const
    {
        return hasClickEffect_;
    }
    void SetHasClickEffect(bool hasClickEffect)
    {
        hasClickEffect_ = hasClickEffect;
    }

    bool HasTransitionAnimation() const
    {
        return hasTransitionAnimation_;
    }
    void SetHasTransitionAnimation(bool hasTransitionAnimation)
    {
        hasTransitionAnimation_ = hasTransitionAnimation;
    }

    bool HasOverflowStyle() const
    {
        return hasOverflowStyle_;
    }
    void SetHasOverflowStyle(bool hasOverflowStyle)
    {
        hasOverflowStyle_ = hasOverflowStyle;
    }

    bool HasTransformOriginStyle() const
    {
        return hasTransformOriginStyle_;
    }
    void SetHasTransformOriginStyle(bool hasTransformOriginStyle)
    {
        hasTransformOriginStyle_ = hasTransformOriginStyle;
    }

    bool HasDisplayStyle() const
    {
        return hasDisplayStyle_;
    }
    void SetHasDisplayStyle(bool hasDisplayStyle)
    {
        hasDisplayStyle_ = hasDisplayStyle;
    }

    bool HasIdAttr() const
    {
        return hasIdAttr_;
    }
    void SetHasIdAttr(bool hasIdAttr)
    {
        hasIdAttr_ = hasIdAttr;
    }

    bool HasLeft() const
    {
        return hasLeft_;
    }
    void SetHasLeft(bool hasLeft)
    {
        hasLeft_ = hasLeft;
    }

    bool HasTop() const
    {
        return hasTop_;
    }
    void SetHasTop(bool hasTop)
    {
        hasTop_ = hasTop;
    }

    bool HasRight() const
    {
        return hasRight_;
    }
    void SetHasRight(bool hasRight)
    {
        hasRight_ = hasRight;
    }

    bool HasBottom() const
    {
        return hasBottom_;
    }
    void SetHasBottom(bool hasBottom)
    {
        hasBottom_ = hasBottom;
    }

    bool HasPositionStyle() const
    {
        return hasPositionStyle_;
    }
    void SetHasPositionStyle(bool hasPositionStyle)
    {
        hasPositionStyle_ = hasPositionStyle;
    }

    bool GetUseLiteStyle() const
    {
        return useLiteStyle_;
    }
    void SetUseLiteStyle(bool useLiteStyle)
    {
        useLiteStyle_ = useLiteStyle;
    }

    void SetFlexShrink(double flexShrink)
    {
        auto& flexStyle = MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
        flexStyle.flexShrink = flexShrink;
    }

    bool IsRightToLeft() const;

    static void SetMaskGradient(const std::string& value, Declaration& declaration);

protected:
    virtual void InitCommonAttribute();
    virtual void InitCommonStyle();
    virtual void InitCommonEvent();
    virtual void InitCommonMethod();
    virtual void InitSpecialized() {}

    // Each subclass needs to override this function to obtain the properties. If it returns true, it means that the
    // property has been consumed. If it returns false, it means it is handed over to the parent class.
    virtual bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
    {
        return false;
    }

    // Each subclass needs to override this function to obtain the style. If it returns true, it means that the
    // style has been consumed. If it returns false, it means it is handed over to the parent class.
    virtual bool SetSpecializedStyle(const std::pair<std::string, std::string>& style)
    {
        return false;
    }

    // Each subclass needs to override this function to obtain the event. If it returns true, it means that the
    // event has been consumed. If it returns false, it means it is handed over to the parent class.
    virtual bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
    {
        return false;
    }

    virtual void CallSpecializedMethod(const std::string& method, const std::string& args) {}

    // When the multi-mode input subscript is set to auto, need to determine whether the current component has the
    // ability to support the subscript.
    virtual bool IsSubscriptEnable() const
    {
        return isSubscriptEnable_;
    }

    void AddCommonAttribute(AttributeTag tag);
    void AddCommonStyle(StyleTag tag);
    void AddCommonEvent(EventTag tag);
    void AddCommonMethod(MethodTag tag);

    void AddSpecializedAttribute(std::shared_ptr<Attribute>&& specializedAttribute);
    void AddSpecializedStyle(std::shared_ptr<Style>&& specializedStyle);
    void AddSpecializedEvent(std::shared_ptr<Event>&& specializedEvent);
    void AddSpecializedRemoteMessageEvent(std::shared_ptr<Event>&& specializedEvent);
    void AddSpecializedMethod(std::shared_ptr<Method>&& specializedMethod);

    static void SetBackgroundImagePosition(const std::string& value, Declaration& declaration);
    static void SetBackgroundImageSize(const std::string& value, Declaration& declaration);
    static void SetPaddingOverall(const std::string& value, Declaration& declaration);
    static void SetMarginOverall(const std::string& value, Declaration& declaration);
    static void SetBorderOverall(const std::string& value, Declaration& declaration);
    static void SetBorderWidthForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderColorForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderStyleForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBackground(const std::string& value, Declaration& declaration);
    static void SetGradientType(const std::string& gradientType, Declaration& declaration);
    static void SetGradientDirections(const std::unique_ptr<JsonValue>& gradientDirections, Declaration& declaration);
    static void SetGradientColor(const std::unique_ptr<JsonValue>& gradientColorValues, Declaration& declaration);
    static void SetGradientShape(const std::string& gradientShape, Declaration& declaration);
    static void SetGradientSize(const std::string& gradientSize, Declaration& declaration);
    static void SetGradientPosition(const std::string& gradientPosition, Declaration& declaration);
    static void SetGradientAngle(const std::string& gradientPosition, Declaration& declaration);
    static void SetGradientRotation(const std::string& gradientRotation, Declaration& declaration);
    static void SetTransform(const std::string& value, Declaration& declaration);
    static void SetBorderImage(const std::string& value, Declaration& declaration);
    static void SetBorderImageUrl(const std::unique_ptr<JsonValue>& values, Declaration& declaration);
    static void SetBorderImageGradient(const std::unique_ptr<JsonValue>& values, Declaration& declaration);

    static void SetBorderImageWidthForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderImageSliceForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderImageOutSetForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderImageRepeatForFourEdges(const std::string& value, Declaration& declaration);
    static void SetBorderImageGradientType(const std::string& gradientType, Declaration& declaration);
    static void SetBorderImageGradientDirections(
        const std::unique_ptr<JsonValue>& gradientDirections, Declaration& declaration);
    static void SetBorderImageGradientColor(const std::unique_ptr<JsonValue>& gradientColorValues,
        Declaration& declaration);
    static void SetBorderImageFindUrl(const std::string& value, Declaration& declaration);

    std::string GetTransformJsonValue(const std::string& value);
    std::string GetTransformType(const std::unique_ptr<JsonValue>& transformJson);
    std::string GetTransformTypeValue(const std::unique_ptr<JsonValue>& transformJson);

    RefPtr<ThemeManager> GetThemeManager() const;
    RefPtr<ThemeConstants> GetThemeConstants() const;

    template<typename T>
    RefPtr<T> GetTheme() const
    {
        auto context = pipelineContext_.Upgrade();
        if (!context) {
            return nullptr;
        }
        auto themeManager = context->GetThemeManager();
        if (!themeManager) {
            return nullptr;
        }
        return themeManager->GetTheme<T>();
    }

    template<typename T>
    T ParseThemeReference(const std::string& value, std::function<T()>&& noRefFunc,
        std::function<T(uint32_t refId)>&& idRefFunc, const T& errorValue) const
    {
        const auto& parseResult = ThemeUtils::ParseThemeIdReference(value, GetThemeConstants());
        if (!parseResult.parseSuccess) {
            return noRefFunc();
        }
        auto themeConstants = GetThemeConstants();
        if (!themeConstants) {
            return errorValue;
        }
        // Refer to a theme id resource.
        if (parseResult.isIdRef) {
            return idRefFunc(parseResult.id);
        }
        // Refer to a theme attribute.
        auto themeStyle = themeConstants->GetThemeStyle();
        if (!themeStyle) {
            return errorValue;
        }
        return themeStyle->GetAttr<T>(parseResult.refAttr, errorValue);
    }

    /*
     * Parse color from string content and reference for id/attr, including format:
     * #rrggbb, #aarrggbb, "@id001", "@attr_sys_color".
     */
    Color ParseColor(const std::string& value, uint32_t maskAlpha = COLOR_ALPHA_MASK) const;

    /*
     * Parse double from string content and reference for id/attr, including format:
     * 100.01, "@id001", "@attr_sys_alpha".
     */
    double ParseDouble(const std::string& value) const;

    /*
     * Parse dimension from string content and reference for id/attr, including format:
     * 10px, "@id001", "@attr_sys_dimension".
     */
    Dimension ParseDimension(const std::string& value, bool useVp = false) const;

    /*
     * Parse calcdimension from string content and reference for id/attr, including format:
     * calc(100% - 10px), 10px, "@id001", "@attr_sys_dimension".
     */
    CalcDimension ParseCalcDimension(const std::string& value, bool useVp = false) const;

    /*
     * Parse line height from string content and reference for id/attr, including format:
     * 1.5, "@id001", "@attr_sys_line_height".
     */
    Dimension ParseLineHeight(const std::string& value) const;

    /*
     * Parse font family list from string content and reference for id/attr, including format:
     * sans-serif, "@id001", "@attr_sys_font_family".
     */
    std::vector<std::string> ParseFontFamilies(const std::string& value) const;

    /*
     * Parse dimension list from string content and reference for id/attr, including format:
     * 10px, "@id001", "@attr_sys_dimension".
     */
    std::vector<Dimension> ParsePreferFontSizes(const std::string& value) const;

    /*
     * Parse image src from string content and reference for id/attr, including format:
     * "@app.media.customized_image", "@sys.media.123".
     */
    std::string ParseImageSrc(const std::string& imgSrc) const;

    WeakPtr<PipelineContext> pipelineContext_;

private:
    std::unordered_map<AttributeTag, std::shared_ptr<Attribute>> attributes_;
    std::unordered_map<StyleTag, std::shared_ptr<Style>> styles_;
    std::unordered_map<EventTag, std::shared_ptr<Event>> events_;
    std::unordered_map<MethodTag, std::shared_ptr<Method>> methods_;

    RefPtr<Decoration> backDecoration_;
    RefPtr<Decoration> frontDecoration_;
    RefPtr<FocusableController> focusableController_;
    RefPtr<ScrollPositionController> positionController_;

    OnSetAttributeFunc onSetAttribute_;
    OnSetStyleFinishedFunc onSetStyleFinished_;
    CachePseudoClassStyleFunc cachePseudoClassStyle_;

    std::string shareId_;

    bool isSubscriptEnable_ = false;
    bool isDisabled_ = false;
    bool isWaiting_ = false;
    bool isChecked_ = false;
    bool isHover_ = false;
    bool hasTransformStyle_ = false;
    bool hasBackGroundColor_ = false;
    bool hasPositionProcessed_ = false;
    bool hasBoxStyle_ = false;
    bool hasShadowStyle_ = false;
    bool hasDecorationStyle_ = false;
    bool hasFrontDecorationStyle_ = false;
    bool hasBorderStyle_ = false;
    bool hasBorderRadiusStyle_ = false;
    bool hasClickEffect_ = false;
    bool hasTransitionAnimation_ = false;
    bool hasOverflowStyle_ = false;
    bool hasTransformOriginStyle_ = false;
    bool hasDisplayStyle_ = false;
    bool hasPositionStyle_ = false;
    bool hasLeft_ = false;
    bool hasTop_ = false;
    bool hasRight_ = false;
    bool hasBottom_ = false;
    bool useLiteStyle_ = false;
    // The target node (with id attribute) for popup should be added gesture event handler,
    // it's ok to take 'id' as a flag, even not all dom nodes with id attribute should do this.
    bool hasIdAttr_ = false;
};

}; // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_H