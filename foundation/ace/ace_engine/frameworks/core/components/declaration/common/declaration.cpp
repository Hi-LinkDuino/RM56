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

#include "core/components/declaration/common/declaration.h"

#include "base/geometry/calc_dimension.h"
#include "base/geometry/dimension.h"
#include "base/log/ace_trace.h"
#include "base/utils/string_utils.h"
#include "core/common/ace_application_info.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

namespace {

constexpr uint32_t TRANSFORM_SINGLE = 1;
constexpr uint32_t TRANSFORM_DUAL = 2;
constexpr int32_t DIRECTION_ANGLE = 1;
constexpr int32_t DIRECTION_SIDE = 2;
constexpr int32_t DIRECTION_CORNER = 3;
constexpr int32_t MS_TO_S = 1000;
constexpr uint32_t COMMON_METHOD_FOCUS_ARGS_SIZE = 1;
constexpr Dimension TRANSFORM_ORIGIN_DEFAULT = 0.5_pct;
const char COMMON_METHOD_FOCUS[] = "focus";

// Shared Transition Effect Type String
constexpr char SHARED_TRANSITION_EFFECT_STATIC[] = "static";
constexpr char SHARED_TRANSITION_EFFECT_EXCHANGE[] = "exchange";

template<class T>
T ParseFunctionValue(const std::string& line, const std::string& key, std::function<T(const std::string&)> parser)
{
    std::vector<std::string> strs;
    StringUtils::SplitStr(line, " ", strs, true);
    for (const auto& str : strs) {
        if (str.empty()) {
            continue;
        }
        auto leftIndex = str.find('(');
        auto rightIndex = str.find(')');
        if (leftIndex == std::string::npos || rightIndex == std::string::npos) {
            continue;
        }
        if (leftIndex + 1 >= rightIndex) {
            continue;
        }
        if (str.substr(0, leftIndex) != key) {
            continue;
        }

        auto valueStr = str.substr(leftIndex + 1, rightIndex - leftIndex - 1);
        return parser(valueStr);
    }
    return T {};
}

RefPtr<SharedTransitionEffect> ParseSharedEffect(const std::string& effect, Declaration& declaration)
{
    std::string effectTrim = effect;
    RemoveHeadTailSpace(effectTrim);
    if (effectTrim == SHARED_TRANSITION_EFFECT_STATIC) {
        return SharedTransitionEffect::GetSharedTransitionEffect(
            SharedTransitionEffectType::SHARED_EFFECT_STATIC, declaration.GetShareId());
    } else if (effectTrim == SHARED_TRANSITION_EFFECT_EXCHANGE) {
        return SharedTransitionEffect::GetSharedTransitionEffect(
            SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE, declaration.GetShareId());
    } else {
        LOGE("Parse shared effect failed. unknown effect: %{public}s, share id: %{public}s", effect.c_str(),
            declaration.GetShareId().c_str());
        return nullptr;
    }
}

TransitionEffect ParseTransitionEffect(const std::string& option)
{
    static std::unordered_map<std::string, TransitionEffect> types = {
        { "unfold", TransitionEffect::UNFOLD },
        { "none", TransitionEffect::NONE },
    };
    auto pos = types.find(option);
    if (pos != types.end()) {
        return pos->second;
    }
    return TransitionEffect::NONE;
}

ClickSpringEffectType ParseClickEffect(const std::string& effect)
{
    static std::unordered_map<std::string, ClickSpringEffectType> types = {
        { "spring-small", ClickSpringEffectType::SMALL },
        { "spring-medium", ClickSpringEffectType::MEDIUM },
        { "spring-large", ClickSpringEffectType::LARGE },
    };
    auto pos = types.find(effect);
    if (pos != types.end()) {
        return pos->second;
    }
    return ClickSpringEffectType::NONE;
}

inline WindowBlurStyle StrToWindowBlurStyle(const std::string& value)
{
    static std::unordered_map<std::string, WindowBlurStyle> types = {
        { "small_light", WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT },
        { "medium_light", WindowBlurStyle::STYLE_BACKGROUND_MEDIUM_LIGHT },
        { "large_light", WindowBlurStyle::STYLE_BACKGROUND_LARGE_LIGHT },
        { "xlarge_light", WindowBlurStyle::STYLE_BACKGROUND_XLARGE_LIGHT },
        { "small_dark", WindowBlurStyle::STYLE_BACKGROUND_SMALL_DARK },
        { "medium_dark", WindowBlurStyle::STYLE_BACKGROUND_MEDIUM_DARK },
        { "large_dark", WindowBlurStyle::STYLE_BACKGROUND_LARGE_DARK },
        { "xlarge_dark", WindowBlurStyle::STYLE_BACKGROUND_XLARGE_DARK },
    };
    auto pos = types.find(value);
    if (pos != types.end()) {
        return pos->second;
    }
    return WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT;
}

} // namespace

Declaration::Declaration()
{
    backDecoration_ = AceType::MakeRefPtr<Decoration>();
}

Declaration::~Declaration() = default;

void Declaration::Init()
{
    InitCommonAttribute();
    InitCommonStyle();
    InitCommonEvent();
    InitCommonMethod();
    InitSpecialized();
    auto& commonAttr = MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
    if (commonAttr.IsValid()) {
        commonAttr.isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
    }

    auto& backgroundStyle = MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (backgroundStyle.IsValid()) {
        backgroundStyle.backgroundImage = AceType::MakeRefPtr<BackgroundImage>();
        backgroundStyle.borderImage = AceType::MakeRefPtr<BorderImage>();
    }
}

void Declaration::InitCommonAttribute()
{
    for (auto attribute : DeclarationConstants::DEFAULT_ATTRS) {
        AddCommonAttribute(attribute);
    }
}

void Declaration::InitCommonStyle()
{
    for (auto style : DeclarationConstants::DEFAULT_STYLES) {
        AddCommonStyle(style);
    }
}

void Declaration::InitCommonEvent()
{
    for (auto event : DeclarationConstants::DEFAULT_EVENTS) {
        AddCommonEvent(event);
    }
}

void Declaration::InitCommonMethod()
{
    for (auto method : DeclarationConstants::DEFAULT_METHODS) {
        AddCommonMethod(method);
    }
}

void Declaration::AddCommonAttribute(AttributeTag tag)
{
    static const LinearEnumMapNode<AttributeTag, void (*)(Declaration&)> operators[] = {
        { AttributeTag::COMMON_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_ATTR, DeclarationConstants::DEFAULT_COMMON_ATTR);
            } },
        { AttributeTag::COMMON_DISABLED_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_DISABLED_ATTR, DeclarationConstants::DEFAULT_DISABLED_ATTR);
            } },
        { AttributeTag::COMMON_FOCUSABLE_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_FOCUSABLE_ATTR, DeclarationConstants::DEFAULT_FOCUSABLE_ATTR);
            } },
        { AttributeTag::COMMON_TOUCHABLE_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_TOUCHABLE_ATTR, DeclarationConstants::DEFAULT_TOUCHABLE_ATTR);
            } },
        { AttributeTag::COMMON_DATA_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_DATA_ATTR, DeclarationConstants::DEFAULT_DATA_ATTR);
            } },
        { AttributeTag::COMMON_CLICK_EFFECT_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_CLICK_EFFECT_ATTR, DeclarationConstants::DEFAULT_CLICK_EFFECT_ATTR);
            } },
        { AttributeTag::COMMON_RENDER_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_RENDER_ATTR, DeclarationConstants::DEFAULT_RENDER_ATTR);
            } },
        { AttributeTag::COMMON_MULTIMODAL_ATTR,
            [](Declaration& declaration) {
                declaration.attributes_.try_emplace(
                    AttributeTag::COMMON_MULTIMODAL_ATTR, DeclarationConstants::DEFAULT_MULTI_MODAL_ATTR);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(operators, ArraySize(operators), tag);
    if (operatorIter != -1) {
        operators[operatorIter].value(*this);
    } else {
        LOGW("attribute tag %{public}d is invalid", tag);
    }
}

void Declaration::AddCommonStyle(StyleTag tag)
{
    static const LinearEnumMapNode<StyleTag, void (*)(Declaration&)> operators[] = {
        { StyleTag::COMMON_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(StyleTag::COMMON_STYLE, DeclarationConstants::DEFAULT_COMMON_STYLE);
            } },
        { StyleTag::COMMON_SIZE_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(StyleTag::COMMON_SIZE_STYLE, DeclarationConstants::DEFAULT_SIZE_STYLE);
            } },
        { StyleTag::COMMON_MARGIN_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_MARGIN_STYLE, DeclarationConstants::DEFAULT_MARGIN_STYLE);
            } },
        { StyleTag::COMMON_PADDING_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_PADDING_STYLE, DeclarationConstants::DEFAULT_PADDING_STYLE);
            } },
        { StyleTag::COMMON_BORDER_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_BORDER_STYLE, DeclarationConstants::DEFAULT_BORDER_STYLE);
            } },
        { StyleTag::COMMON_BACKGROUND_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_BACKGROUND_STYLE, DeclarationConstants::DEFAULT_BACKGROUND_STYLE);
            } },
        { StyleTag::COMMON_FLEX_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(StyleTag::COMMON_FLEX_STYLE, DeclarationConstants::DEFAULT_FLEX_STYLE);
            } },
        { StyleTag::COMMON_POSITION_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_POSITION_STYLE, DeclarationConstants::DEFAULT_POSITION_STYLE);
            } },
        { StyleTag::COMMON_OPACITY_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_OPACITY_STYLE, DeclarationConstants::DEFAULT_OPACITY_STYLE);
            } },
        { StyleTag::COMMON_VISIBILITY_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_VISIBILITY_STYLE, DeclarationConstants::DEFAULT_VISIBILITY_STYLE);
            } },
        { StyleTag::COMMON_DISPLAY_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_DISPLAY_STYLE, DeclarationConstants::DEFAULT_DISPLAY_STYLE);
            } },
        { StyleTag::COMMON_SHADOW_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_SHADOW_STYLE, DeclarationConstants::DEFAULT_SHADOW_STYLE);
            } },
        { StyleTag::COMMON_OVERFLOW_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_OVERFLOW_STYLE, DeclarationConstants::DEFAULT_OVERFLOW_STYLE);
            } },
        { StyleTag::COMMON_FILTER_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_FILTER_STYLE, DeclarationConstants::DEFAULT_FILTER_STYLE);
            } },
        { StyleTag::COMMON_ANIMATION_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_ANIMATION_STYLE, DeclarationConstants::DEFAULT_ANIMATION_STYLE);
            } },
        { StyleTag::COMMON_SHARE_TRANSITION_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_SHARE_TRANSITION_STYLE, DeclarationConstants::DEFAULT_SHARE_TRANSITION_STYLE);
            } },
        { StyleTag::COMMON_CARD_TRANSITION_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_CARD_TRANSITION_STYLE, DeclarationConstants::DEFAULT_CARD_TRANSITION_STYLE);
            } },
        { StyleTag::COMMON_PAGE_TRANSITION_STYLE,
            [](Declaration& declaration) {
                declaration.styles_.try_emplace(
                    StyleTag::COMMON_PAGE_TRANSITION_STYLE, DeclarationConstants::DEFAULT_PAGE_TRANSITION_STYLE);
            } },
        { StyleTag::COMMON_CLIP_PATH_STYLE,
            [](Declaration& declaration) {
              declaration.styles_.try_emplace(
                  StyleTag::COMMON_CLIP_PATH_STYLE, DeclarationConstants::DEFAULT_CLIP_PATH_STYLE);
            } },
        { StyleTag::COMMON_MASK_STYLE,
            [](Declaration& declaration) {
              declaration.styles_.try_emplace(
                  StyleTag::COMMON_MASK_STYLE, DeclarationConstants::DEFAULT_MASK_STYLE);
            } },
        { StyleTag::COMMON_IMAGE_STYLE,
            [](Declaration& declaration) {
              declaration.styles_.try_emplace(
                  StyleTag::COMMON_IMAGE_STYLE, DeclarationConstants::DEFAULT_IMAGE_STYLE);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(operators, ArraySize(operators), tag);
    if (operatorIter != -1) {
        operators[operatorIter].value(*this);
    } else {
        LOGW("style tag %{public}d is invalid", tag);
    }
}

void Declaration::AddCommonEvent(EventTag tag)
{
    static const LinearEnumMapNode<EventTag, void (*)(Declaration&)> operators[] = {
        { EventTag::COMMON_RAW_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(EventTag::COMMON_RAW_EVENT, DeclarationConstants::DEFAULT_RAW_EVENT);
            } },
        { EventTag::COMMON_GESTURE_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_GESTURE_EVENT, DeclarationConstants::DEFAULT_GESTURE_EVENT);
            } },
        { EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT, DeclarationConstants::DEFAULT_GESTURE_EVENT);
            } },
        { EventTag::COMMON_FOCUS_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_FOCUS_EVENT, DeclarationConstants::DEFAULT_FOCUS_EVENT);
            } },
        { EventTag::COMMON_KEY_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(EventTag::COMMON_KEY_EVENT, DeclarationConstants::DEFAULT_KEY_EVENT);
            } },
        { EventTag::COMMON_MOUSE_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_MOUSE_EVENT, DeclarationConstants::DEFAULT_MOUSE_EVENT);
            } },
        { EventTag::COMMON_SWIPE_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_SWIPE_EVENT, DeclarationConstants::DEFAULT_SWIPE_EVENT);
            } },
        { EventTag::COMMON_ATTACH_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_ATTACH_EVENT, DeclarationConstants::DEFAULT_ATTACH_EVENT);
            } },
        { EventTag::COMMON_CROWN_EVENT,
            [](Declaration& declaration) {
                declaration.events_.try_emplace(
                    EventTag::COMMON_CROWN_EVENT, DeclarationConstants::DEFAULT_CROWN_EVENT);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(operators, ArraySize(operators), tag);
    if (operatorIter != -1) {
        operators[operatorIter].value(*this);
    } else {
        LOGW("event tag %{public}d is invalid", tag);
    }
}

void Declaration::AddCommonMethod(MethodTag tag)
{
    static const LinearEnumMapNode<MethodTag, void (*)(Declaration&)> operators[] = {
        { MethodTag::COMMON_METHOD,
            [](Declaration& declaration) {
                declaration.methods_.try_emplace(MethodTag::COMMON_METHOD, DeclarationConstants::DEFAULT_METHOD);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(operators, ArraySize(operators), tag);
    if (operatorIter != -1) {
        operators[operatorIter].value(*this);
    } else {
        LOGW("method tag %{public}d is invalid", tag);
    }
}

void Declaration::AddSpecializedAttribute(std::shared_ptr<Attribute>&& specializedAttribute)
{
    attributes_.try_emplace(AttributeTag::SPECIALIZED_ATTR, std::move(specializedAttribute));
}

void Declaration::AddSpecializedStyle(std::shared_ptr<Style>&& specializedStyle)
{
    styles_.try_emplace(StyleTag::SPECIALIZED_STYLE, std::move(specializedStyle));
}

void Declaration::AddSpecializedEvent(std::shared_ptr<Event>&& specializedEvent)
{
    events_.try_emplace(EventTag::SPECIALIZED_EVENT, std::move(specializedEvent));
}

void Declaration::AddSpecializedRemoteMessageEvent(std::shared_ptr<Event>&& specializedEvent)
{
    events_.try_emplace(EventTag::SPECIALIZED_REMOTE_MESSAGE_EVENT, std::move(specializedEvent));
}

void Declaration::AddSpecializedMethod(std::shared_ptr<Method>&& specializedMethod)
{
    methods_.try_emplace(MethodTag::SPECIALIZED_METHOD, std::move(specializedMethod));
}

Attribute& Declaration::GetAttribute(AttributeTag tag) const
{
    auto it = attributes_.find(tag);
    if (it != attributes_.end()) {
        return *(it->second);
    } else {
        static Attribute errAttribute {
            .tag = AttributeTag::UNKNOWN
        };
        return errAttribute;
    }
}

Style& Declaration::GetStyle(StyleTag tag) const
{
    auto it = styles_.find(tag);
    if (it != styles_.end()) {
        return *(it->second);
    } else {
        static Style errStyle { .tag = StyleTag::UNKNOWN };
        return errStyle;
    }
}

Event& Declaration::GetEvent(EventTag tag) const
{
    auto it = events_.find(tag);
    if (it != events_.end()) {
        return *(it->second);
    } else {
        static Event errEvent { .tag = EventTag::UNKNOWN };
        return errEvent;
    }
}

Method& Declaration::GetMethod(MethodTag tag) const
{
    auto it = methods_.find(tag);
    if (it != methods_.end()) {
        return *(it->second);
    } else {
        static Method errMethod { .tag = MethodTag::UNKNOWN };
        return errMethod;
    }
}

void Declaration::SetShowAttr(const std::string& showValue)
{
    auto& renderAttr = MaybeResetAttribute<CommonRenderAttribute>(AttributeTag::COMMON_RENDER_ATTR);
    if (renderAttr.IsValid()) {
        renderAttr.show = showValue;
    }
    auto& displayStyle = MaybeResetStyle<CommonDisplayStyle>(StyleTag::COMMON_DISPLAY_STYLE);
    if (displayStyle.IsValid()) {
        displayStyle.display = (showValue == "false") ? DisplayType::NONE : DisplayType::NO_SETTING;
    }
}

void Declaration::SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs)
{
    ACE_SCOPED_TRACE("Declaration::SetAttr");
    if (onSetAttribute_) {
        onSetAttribute_();
    }
    static const std::string flagOn = "on";
    static const std::string flagOff = "off";
    static const std::string flagAuto = "auto";
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, Declaration&)> attrSetters[] = {
        { DOM_CLICK_EFFECT,
            [](const std::string& value, Declaration& declaration) {
                auto& clickEffectAttr =
                    declaration.MaybeResetAttribute<CommonClickEffectAttribute>(AttributeTag::COMMON_CLICK_EFFECT_ATTR);
                if (clickEffectAttr.IsValid()) {
                    declaration.hasTransformStyle_ = true;
                    declaration.hasClickEffect_ = true;
                    clickEffectAttr.clickEffect = ParseClickEffect(value);
                }
            } },
        { DOM_DIR,
            [](const std::string& value, Declaration& declaration) {
                auto& commonAttr = declaration.MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
                if (commonAttr.IsValid()) {
                    if (value == "rtl") {
                        commonAttr.isRightToLeft = true;
                        commonAttr.direction = TextDirection::RTL;
                    } else if (value == "ltr") {
                        commonAttr.isRightToLeft = false;
                        commonAttr.direction = TextDirection::LTR;
                    } else {
                        commonAttr.isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
                        commonAttr.direction = TextDirection::AUTO;
                    }
                }
            } },
        { DOM_FOCUSABLE,
            [](const std::string& value, Declaration& declaration) {
                auto& focusableAttr =
                    declaration.MaybeResetAttribute<CommonFocusableAttribute>(AttributeTag::COMMON_FOCUSABLE_ATTR);
                if (focusableAttr.IsValid()) {
                    focusableAttr.focusable.first = StringToBool(value);
                    focusableAttr.focusable.second = true; // Tag whether user defined focusable.
                }
            } },
        { DOM_ID,
            [](const std::string& value, Declaration& declaration) {
                auto& commonAttr = declaration.MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
                if (commonAttr.IsValid()) {
                    commonAttr.id = value;
                    declaration.hasIdAttr_ = true;
                }
            } },
#ifndef WEARABLE_PRODUCT
        { DOM_SCENE_LABEL,
            [](const std::string& value, Declaration& declaration) {
                auto& multimodalAttr =
                    declaration.MaybeResetAttribute<CommonMultimodalAttribute>(AttributeTag::COMMON_MULTIMODAL_ATTR);
                if (!multimodalAttr.IsValid()) {
                    return;
                }
                static const LinearMapNode<SceneLabel> multimodalSceneMap[] = {
                    { "audio", SceneLabel::AUDIO },
                    { "common", SceneLabel::COMMON },
                    { "page", SceneLabel::PAGE },
                    { "switch", SceneLabel::SWITCH },
                    { "video", SceneLabel::VIDEO },
                };
                auto iter = BinarySearchFindIndex(multimodalSceneMap, ArraySize(multimodalSceneMap), value.c_str());
                if (iter != -1) {
                    multimodalAttr.scene = multimodalSceneMap[iter].value;
                }
            } },
#endif
        { DOM_SHOW,
            [](const std::string& value, Declaration& declaration) {
                declaration.hasDisplayStyle_ = true;
                if (declaration.useLiteStyle_) {
                    auto& visibilityStyle =
                        declaration.MaybeResetStyle<CommonVisibilityStyle>(StyleTag::COMMON_VISIBILITY_STYLE);
                    if (visibilityStyle.IsValid()) {
                        visibilityStyle.visibility =
                            (value == "true") ? VisibilityType::VISIBLE : VisibilityType::HIDDEN;
                    }
                } else {
                    declaration.SetShowAttr(value);
                }
            } },
#ifndef WEARABLE_PRODUCT
        { DOM_SPRING_EFFECT,
            [](const std::string& value, Declaration& declaration) {
                auto& clickEffectAttr =
                    declaration.MaybeResetAttribute<CommonClickEffectAttribute>(AttributeTag::COMMON_CLICK_EFFECT_ATTR);
                if (clickEffectAttr.IsValid()) {
                    declaration.hasTransformStyle_ = true;
                    declaration.hasClickEffect_ = true;
                    clickEffectAttr.clickEffect = ParseClickEffect(value);
                }
            } },
        { DOM_SUBSCRIPT_FLAG,
            [](const std::string& value, Declaration& declaration) {
                auto& multimodalAttr =
                    declaration.MaybeResetAttribute<CommonMultimodalAttribute>(AttributeTag::COMMON_MULTIMODAL_ATTR);
                if (multimodalAttr.IsValid()) {
                    multimodalAttr.useSubscript =
                        (value == flagOn) || (value == flagAuto && declaration.IsSubscriptEnable());
                }
            } },
        { DOM_SUBSCRIPT_LABEL,
            [](const std::string& value, Declaration& declaration) {
                auto& multimodalAttr =
                    declaration.MaybeResetAttribute<CommonMultimodalAttribute>(AttributeTag::COMMON_MULTIMODAL_ATTR);
                if (multimodalAttr.IsValid()) {
                    multimodalAttr.subscriptLabel = value;
                }
            } },
        { DOM_TOUCHABLE,
            [](const std::string& value, Declaration& declaration) {
                auto& touchableAttr =
                    declaration.MaybeResetAttribute<CommonTouchableAttribute>(AttributeTag::COMMON_TOUCHABLE_ATTR);
                if (touchableAttr.IsValid()) {
                    touchableAttr.touchable = StringToBool(value);
                }
            } },
        { DOM_VOICE_LABEL,
            [](const std::string& value, Declaration& declaration) {
                auto& multimodalAttr =
                    declaration.MaybeResetAttribute<CommonMultimodalAttribute>(AttributeTag::COMMON_MULTIMODAL_ATTR);
                if (multimodalAttr.IsValid()) {
                    multimodalAttr.voiceLabel = value;
                }
            } },
#endif
    };

    for (const auto& attr : attrs) {
        if (attr.first == DOM_DISABLED) {
            isDisabled_ = StringToBool(attr.second);
        }

        if (attr.first == DOM_BUTTON_WAITING) {
            isWaiting_ = StringToBool(attr.second);
        }

        if (attr.first == DOM_CHECKED) {
            isChecked_ = StringToBool(attr.second);
        }

        if (SetSpecializedAttr(attr)) {
            continue;
        }
        auto operatorIter = BinarySearchFindIndex(attrSetters, ArraySize(attrSetters), attr.first.c_str());
        if (operatorIter != -1) {
            attrSetters[operatorIter].value(attr.second, *this);
        }
    }
}

void Declaration::SetStyle(const std::vector<std::pair<std::string, std::string>>& styles)
{
    ACE_SCOPED_TRACE("Declaration::SetStyle");
    for (const auto& style : styles) {
        if (style.first.find(DOM_PSEUDO_CLASS_SYMBOL) == std::string::npos) {
            SetCurrentStyle(style);
        }
    }
}

void Declaration::SetCurrentStyle(const std::pair<std::string, std::string>& style)
{
    if (SetSpecializedStyle(style)) {
        // If the subclass consumes this property, it will no longer look in the general property.
        return;
    }

    // Operator map for styles
    static const LinearMapNode<void (*)(const std::string&, Declaration&)> styleSetter[] = {
        { DOM_ALIGN_SELF,
            [](const std::string& value, Declaration& declaration) {
                auto& commonStyle = declaration.MaybeResetStyle<CommonStyle>(StyleTag::COMMON_STYLE);
                if (commonStyle.IsValid()) {
                    commonStyle.alignSelf = value;
                }
            } },
        { DOM_ANIMATION_DELAY,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    if (value.find("ms") != std::string::npos) {
                        animationStyle.animationDelay = StringUtils::StringToInt(value);
                        animationStyle.tweenOption.SetDelay(animationStyle.animationDelay);
                    } else {
                        animationStyle.animationDelay = StringUtils::StringToInt(value) * MS_TO_S;
                        animationStyle.tweenOption.SetDelay(animationStyle.animationDelay);
                    }
                }
            } },
        { DOM_ANIMATION_DIRECTION,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    animationStyle.tweenOption.SetAnimationDirection(StringToAnimationDirection(value));
                }
            } },
        { DOM_ANIMATION_DURATION,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    if (value.find("ms") != std::string::npos) {
                        animationStyle.animationDuration = StringUtils::StringToInt(value);
                        animationStyle.tweenOption.SetDuration(animationStyle.animationDuration);
                    } else {
                        animationStyle.animationDuration = StringUtils::StringToInt(value) * MS_TO_S;
                        animationStyle.tweenOption.SetDuration(animationStyle.animationDuration);
                    }
                }
            } },
        { DOM_ANIMATION_FILL_MODE,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    animationStyle.fillMode = StringToFillMode(value);
                    animationStyle.tweenOption.SetFillMode(animationStyle.fillMode);
                }
            } },
        { DOM_ANIMATION_ITERATION_COUNT,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    animationStyle.iteration = StringUtils::StringToInt(value);
                    animationStyle.tweenOption.SetIteration(animationStyle.iteration);
                }
            } },
        { DOM_ANIMATION_PLAY_STATE,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    animationStyle.animationOperation = StringToAnimationOperation(value);
                }
            } },
        { DOM_ANIMATION_TIMING_FUNCTION,
            [](const std::string& value, Declaration& declaration) {
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (animationStyle.IsValid()) {
                    animationStyle.curve = CreateCurve(value);
                    animationStyle.tweenOption.SetCurve(animationStyle.curve);
                }
            } },
        { DOM_APPEARING_DURATION,
            [](const std::string& value, Declaration& declaration) {
                auto& opacityStyle = declaration.MaybeResetStyle<CommonOpacityStyle>(StyleTag::COMMON_OPACITY_STYLE);
                if (opacityStyle.IsValid()) {
                    opacityStyle.appearingDuration = StringUtils::StringToInt(value);
                    declaration.hasDisplayStyle_ = true;
                }
            } },
        { DOM_ASPECT_RATIO,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.aspectRatio = StringToDouble(value);
                }
            } },
        { DOM_BACKDROP_FILTER,
            [](const std::string& value, Declaration& declaration) {
                declaration.hasDecorationStyle_ = true;
                auto radius = ParseFunctionValue<Dimension>(value, DOM_BLUR, StringToDimension);
                if (radius.IsValid()) {
                    declaration.backDecoration_->SetBlurRadius(radius);
                } else {
                    declaration.backDecoration_->SetBlurRadius(Dimension {});
                }
            } },
        { DOM_BACKGROUND, &Declaration::SetBackground },
        { DOM_BACKGROUND_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& backgroundStyle =
                    declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
                if (backgroundStyle.IsValid()) {
                    auto backgroundColor = declaration.ParseColor(value);
                    backgroundStyle.backgroundColor = backgroundColor;
                    declaration.backDecoration_->SetBackgroundColor(backgroundColor);
                    declaration.hasBackGroundColor_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BACKGROUND_IMAGE,
            [](const std::string& value, Declaration& declaration) {
                auto& backgroundStyle =
                    declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
                if (backgroundStyle.IsValid()) {
                    backgroundStyle.backgroundImage->SetSrc(value, declaration.GetThemeConstants());
                    declaration.backDecoration_->SetImage(backgroundStyle.backgroundImage);
                    declaration.backDecoration_->SetBackgroundColor(Color::TRANSPARENT);
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BACKGROUND_IMAGE_POSITION, &Declaration::SetBackgroundImagePosition },
        { DOM_BACKGROUND_IMAGE_REPEAT,
            [](const std::string& value, Declaration& declaration) {
                auto& backgroundStyle =
                    declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
                if (backgroundStyle.IsValid()) {
                    backgroundStyle.backgroundRepeat = ConvertStrToImageRepeat(value);
                    backgroundStyle.backgroundImage->SetImageRepeat(backgroundStyle.backgroundRepeat);
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BACKGROUND_IMAGE_SIZE, &Declaration::SetBackgroundImageSize },
        { DOM_BORDER, &Declaration::SetBorderOverall },
        { DOM_BORDER_BOTTOM_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBottomColor(declaration.ParseColor(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_BOTTOM_LEFT_RADIUS,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBottomLeftRadius(Radius(declaration.ParseDimension(value)));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasBorderRadiusStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_BOTTOM_RIGHT_RADIUS,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBottomRightRadius(Radius(declaration.ParseDimension(value)));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasBorderRadiusStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_BOTTOM_STYLE,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBottomStyle(ConvertStrToBorderStyle(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_BOTTOM_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBottomWidth(declaration.ParseDimension(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_COLOR, &Declaration::SetBorderColorForFourEdges },

        { DOM_BORDER_IMAGE, &Declaration::SetBorderImage},
        { DOM_BORDER_IMAGE_OUTSET, &Declaration::SetBorderImageOutSetForFourEdges},
        { DOM_BORDER_IMAGE_REPEAT, &Declaration::SetBorderImageRepeatForFourEdges},
        { DOM_BORDER_IMAGE_SLICE, &Declaration::SetBorderImageSliceForFourEdges},
        { DOM_BORDER_IMAGE_SOURCE, &Declaration::SetBorderImageFindUrl},
        { DOM_BORDER_IMAGE_WIDTH, &Declaration::SetBorderImageWidthForFourEdges},

        { DOM_BORDER_LEFT_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetLeftColor(declaration.ParseColor(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_LEFT_STYLE,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetLeftStyle(ConvertStrToBorderStyle(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_LEFT_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetLeftWidth(declaration.ParseDimension(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_RADIUS,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetBorderRadius(Radius(declaration.ParseDimension(value)));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasBorderRadiusStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_RIGHT_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetRightColor(declaration.ParseColor(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_RIGHT_STYLE,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetRightStyle(ConvertStrToBorderStyle(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_RIGHT_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetRightWidth(declaration.ParseDimension(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_STYLE, &Declaration::SetBorderStyleForFourEdges },
        { DOM_BORDER_TOP_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetTopColor(declaration.ParseColor(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_TOP_LEFT_RADIUS,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetTopLeftRadius(Radius(declaration.ParseDimension(value)));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasBorderRadiusStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_TOP_RIGHT_RADIUS,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetTopRightRadius(Radius(declaration.ParseDimension(value)));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasBorderRadiusStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_TOP_STYLE,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetTopStyle(ConvertStrToBorderStyle(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_TOP_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
                if (borderStyle.IsValid()) {
                    borderStyle.border.SetTopWidth(declaration.ParseDimension(value));
                    declaration.hasBorderStyle_ = true;
                    declaration.hasDecorationStyle_ = true;
                }
            } },
        { DOM_BORDER_WIDTH, &Declaration::SetBorderWidthForFourEdges },
        { DOM_POSITION_BOTTOM,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    positionStyle.bottom = declaration.ParseDimension(value);
                    declaration.hasPositionStyle_ = true;
                    declaration.hasBottom_ = true;
                }
            } },
        { DOM_BOX_SHADOW_BLUR,
            [](const std::string& value, Declaration& declaration) {
                auto& shadowStyle = declaration.MaybeResetStyle<CommonShadowStyle>(StyleTag::COMMON_SHADOW_STYLE);
                if (shadowStyle.IsValid()) {
                    shadowStyle.shadow.SetBlurRadius(StringToDouble(value));
                    declaration.hasDecorationStyle_ = true;
                    declaration.hasShadowStyle_ = true;
                }
            } },
        { DOM_BOX_SHADOW_COLOR,
            [](const std::string& value, Declaration& declaration) {
                auto& shadowStyle = declaration.MaybeResetStyle<CommonShadowStyle>(StyleTag::COMMON_SHADOW_STYLE);
                if (shadowStyle.IsValid()) {
                    if (value.empty()) {
                        shadowStyle.shadow.SetColor(Color::BLACK);
                        return;
                    }
                    shadowStyle.shadow.SetColor(declaration.ParseColor(value));
                    declaration.hasDecorationStyle_ = true;
                    declaration.hasShadowStyle_ = true;
                }
            } },
        { DOM_BOX_SHADOW_H,
            [](const std::string& value, Declaration& declaration) {
                auto& shadowStyle = declaration.MaybeResetStyle<CommonShadowStyle>(StyleTag::COMMON_SHADOW_STYLE);
                if (shadowStyle.IsValid()) {
                    shadowStyle.shadow.SetOffsetX(StringToDouble(value));
                    declaration.hasDecorationStyle_ = true;
                    declaration.hasShadowStyle_ = true;
                }
            } },
        { DOM_BOX_SHADOW_SPREAD,
            [](const std::string& value, Declaration& declaration) {
                auto& shadowStyle = declaration.MaybeResetStyle<CommonShadowStyle>(StyleTag::COMMON_SHADOW_STYLE);
                if (shadowStyle.IsValid()) {
                    shadowStyle.shadow.SetSpreadRadius(StringToDouble(value));
                    declaration.hasDecorationStyle_ = true;
                    declaration.hasShadowStyle_ = true;
                }
            } },
        { DOM_BOX_SHADOW_V,
            [](const std::string& value, Declaration& declaration) {
                auto& shadowStyle = declaration.MaybeResetStyle<CommonShadowStyle>(StyleTag::COMMON_SHADOW_STYLE);
                if (shadowStyle.IsValid()) {
                    shadowStyle.shadow.SetOffsetY(StringToDouble(value));
                    declaration.hasDecorationStyle_ = true;
                    declaration.hasShadowStyle_ = true;
                }
            } },
        { DOM_BOX_SIZING,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.boxSizing = ConvertStrToBoxSizing(value);
                }
            } },
        { DOM_CLIP_PATH,
            [](const std::string& value, Declaration& declaration) {
                auto& clipPathStyle =
                    declaration.MaybeResetStyle<CommonClipPathStyle>(StyleTag::COMMON_CLIP_PATH_STYLE);
                if (clipPathStyle.IsValid()) {
                    clipPathStyle.clipPath = CreateClipPath(value);
                }
            } },
        { DOM_DISPLAY,
            [](const std::string& value, Declaration& declaration) {
                auto& displayStyle = declaration.MaybeResetStyle<CommonDisplayStyle>(StyleTag::COMMON_DISPLAY_STYLE);
                if (displayStyle.IsValid()) {
                    displayStyle.display = (value == DOM_DISPLAY_NONE)
                                               ? DisplayType::NONE
                                               : (value == DOM_DISPLAY_GRID) ? DisplayType::GRID : DisplayType::FLEX;
                    declaration.hasDisplayStyle_ = true;
                }
            } },
        { DOM_DISPLAY_INDEX,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.displayIndex = StringToInt(value);
                }
            } },
        { DOM_POSITION_END,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    if (declaration.IsRightToLeft()) {
                        positionStyle.left = declaration.ParseDimension(value);
                        declaration.hasLeft_ = true;
                    } else {
                        positionStyle.right = declaration.ParseDimension(value);
                        declaration.hasRight_ = true;
                    }
                    declaration.hasPositionStyle_ = true;
                }
            } },
        { DOM_FILTER,
            [](const std::string& value, Declaration& declaration) {
                declaration.hasFrontDecorationStyle_ = true;
                if (!declaration.frontDecoration_) {
                    declaration.frontDecoration_ = AceType::MakeRefPtr<Decoration>();
                }
                auto radius = ParseFunctionValue<Dimension>(value, DOM_BLUR, StringToDimension);
                if (radius.IsValid()) {
                    declaration.frontDecoration_->SetBlurRadius(radius);
                } else {
                    declaration.frontDecoration_->SetBlurRadius(Dimension {});
                }
            } },
        { DOM_FLEX,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.flexGrow = StringToDouble(value);
                }
            } },
        { DOM_FLEX_BASIS,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.flexBasis = StringToDimension(value);
                }
            } },
        { DOM_FLEX_GROW,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.flexGrow = StringToDouble(value);
                }
            } },
        { DOM_FLEX_SHRINK,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.flexShrink = StringToDouble(value);
                }
            } },
        { DOM_FLEX_WEIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& flexStyle = declaration.MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
                if (flexStyle.IsValid()) {
                    flexStyle.flexWeight = StringToDouble(value);
                }
            } },
        { DOM_HEIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.height = declaration.ParseCalcDimension(value);
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_IMAGE_FILL,
            [](const std::string& value, Declaration& declaration) {
                auto& imageStyle = declaration.MaybeResetStyle<CommonImageStyle>(StyleTag::COMMON_IMAGE_STYLE);
                if (imageStyle.IsValid()) {
                    imageStyle.imageFill = declaration.ParseColor(value);
                }
            } },
        { DOM_LAYOUT_IN_BOX,
            [](const std::string& value, Declaration& declaration) {
                auto& commonStyle = declaration.MaybeResetStyle<CommonStyle>(StyleTag::COMMON_STYLE);
                if (commonStyle.IsValid()) {
                    commonStyle.layoutInBox = StringToBool(value);
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_POSITION_LEFT,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    positionStyle.left = declaration.ParseDimension(value);
                    declaration.hasPositionStyle_ = true;
                    declaration.hasLeft_ = true;
                }
            } },
        { DOM_MARGIN, &Declaration::SetMarginOverall },
        { DOM_MARGIN_BOTTOM,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    marginStyle.margin.SetBottom(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MARGIN_END,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    if (declaration.IsRightToLeft()) {
                        marginStyle.margin.SetLeft(declaration.ParseCalcDimension(value));
                    } else {
                        marginStyle.margin.SetRight(declaration.ParseCalcDimension(value));
                    }
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MARGIN_LEFT,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    marginStyle.margin.SetLeft(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MARGIN_RIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    marginStyle.margin.SetRight(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MARGIN_START,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    if (declaration.IsRightToLeft()) {
                        marginStyle.margin.SetRight(declaration.ParseCalcDimension(value));
                    } else {
                        marginStyle.margin.SetLeft(declaration.ParseCalcDimension(value));
                    }
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MARGIN_TOP,
            [](const std::string& value, Declaration& declaration) {
                auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
                if (marginStyle.IsValid()) {
                    marginStyle.margin.SetTop(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_MASK_IMAGE,
            [](const std::string& value, Declaration& declaration) {
                auto& maskStyle = declaration.MaybeResetStyle<CommonMaskStyle>(StyleTag::COMMON_MASK_STYLE);
                if (maskStyle.IsValid()) {
                    maskStyle.maskImage = value;
                }
            } },
        { DOM_MASK_POSITION,
            [](const std::string& value, Declaration& declaration) {
                auto& maskStyle = declaration.MaybeResetStyle<CommonMaskStyle>(StyleTag::COMMON_MASK_STYLE);
                if (maskStyle.IsValid()) {
                    maskStyle.maskPosition = value;
                }
            } },
        { DOM_MASK_SIZE,
            [](const std::string& value, Declaration& declaration) {
                auto& maskStyle = declaration.MaybeResetStyle<CommonMaskStyle>(StyleTag::COMMON_MASK_STYLE);
                if (maskStyle.IsValid()) {
                    maskStyle.maskSize = value;
                }
            } },
        { DOM_MAX_HEIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.maxHeight = declaration.ParseCalcDimension(value);
                }
            } },
        { DOM_MAX_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.maxWidth = declaration.ParseCalcDimension(value);
                }
            } },
        { DOM_MIN_HEIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.minHeight = declaration.ParseCalcDimension(value);
                }
            } },
        { DOM_MIN_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.minWidth = declaration.ParseCalcDimension(value);
                }
            } },
        { DOM_OPACITY,
            [](const std::string& value, Declaration& declaration) {
                auto& opacityStyle = declaration.MaybeResetStyle<CommonOpacityStyle>(StyleTag::COMMON_OPACITY_STYLE);
                if (opacityStyle.IsValid()) {
                    opacityStyle.opacity = declaration.ParseDouble(value);
                    declaration.hasDisplayStyle_ = true;
                }
            } },
        { DOM_OVERFLOW_STYLE,
            [](const std::string& value, Declaration& declaration) {
                auto& overflowStyle = declaration.MaybeResetStyle<CommonOverflowStyle>(StyleTag::COMMON_OVERFLOW_STYLE);
                if (overflowStyle.IsValid()) {
                    overflowStyle.overflow = ConvertStrToOverflow(value);
                    declaration.hasOverflowStyle_ = true;
                }
            } },
        { DOM_SCROLL_OVER_SCROLL_EFFECT,
            [](const std::string& val, Declaration& declaration) {
                auto& overflowStyle = declaration.MaybeResetStyle<CommonOverflowStyle>(StyleTag::COMMON_OVERFLOW_STYLE);
                if (!overflowStyle.IsValid()) {
                    return;
                }
                if (val == DOM_SCROLL_EFFECT_SPRING) {
                    overflowStyle.edgeEffect = EdgeEffect::SPRING;
                } else if (val == DOM_SCROLL_EFFECT_FADE) {
                    overflowStyle.edgeEffect = EdgeEffect::FADE;
                } else {
                    overflowStyle.edgeEffect = EdgeEffect::NONE;
                }
            } },
        { DOM_PADDING, &Declaration::SetPaddingOverall },
        { DOM_PADDING_BOTTOM,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    paddingStyle.padding.SetBottom(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_PADDING_END,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    if (declaration.IsRightToLeft()) {
                        paddingStyle.padding.SetLeft(declaration.ParseCalcDimension(value));
                    } else {
                        paddingStyle.padding.SetRight(declaration.ParseCalcDimension(value));
                    }
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_PADDING_LEFT,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    paddingStyle.padding.SetLeft(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_PADDING_RIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    paddingStyle.padding.SetRight(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_PADDING_START,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    if (declaration.IsRightToLeft()) {
                        paddingStyle.padding.SetRight(declaration.ParseCalcDimension(value));
                    } else {
                        paddingStyle.padding.SetLeft(declaration.ParseCalcDimension(value));
                    }
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_PADDING_TOP,
            [](const std::string& value, Declaration& declaration) {
                auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
                if (paddingStyle.IsValid()) {
                    paddingStyle.padding.SetTop(declaration.ParseCalcDimension(value));
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_POSITION,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    positionStyle.position =
                        value == DOM_POSITION_FIXED
                            ? PositionType::FIXED
                            : value == DOM_POSITION_ABSOLUTE ? PositionType::ABSOLUTE : PositionType::RELATIVE;
                    declaration.hasPositionStyle_ = true;
                }
            } },
        { DOM_POSITION_RIGHT,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    positionStyle.right = declaration.ParseDimension(value);
                    declaration.hasPositionStyle_ = true;
                    declaration.hasRight_ = true;
                }
            } },
        { DOM_SCROLL_SCROLLBAR_COLOR,
            [](const std::string& val, Declaration& declaration) {
                auto& overflowStyle = declaration.MaybeResetStyle<CommonOverflowStyle>(StyleTag::COMMON_OVERFLOW_STYLE);
                if (overflowStyle.IsValid()) {
                    overflowStyle.scrollBarColor.first = true;
                    overflowStyle.scrollBarColor.second = declaration.ParseColor(val);
                }
            } },
        { DOM_SCROLL_SCROLLBAR_WIDTH,
            [](const std::string& val, Declaration& declaration) {
                auto& overflowStyle = declaration.MaybeResetStyle<CommonOverflowStyle>(StyleTag::COMMON_OVERFLOW_STYLE);
                if (overflowStyle.IsValid()) {
                    overflowStyle.scrollBarWidth.first = true;
                    auto width = declaration.ParseDimension(val);
                    overflowStyle.scrollBarWidth.second = width.IsValid() ? width : Dimension();
                }
            } },
        { DOM_SHARED_TRANSITION_EFFECT,
            [](const std::string& value, Declaration& declaration) {
                auto& shareTransitionStyle =
                    declaration.MaybeResetStyle<CommonShareTransitionStyle>(StyleTag::COMMON_SHARE_TRANSITION_STYLE);
                if (shareTransitionStyle.IsValid()) {
                    shareTransitionStyle.sharedEffect = ParseSharedEffect(value, declaration);
                }
            } },
        { DOM_SHARED_TRANSITION_TIMING_FUNCTION,
            [](const std::string& value, Declaration& declaration) {
                auto& shareTransitionStyle =
                    declaration.MaybeResetStyle<CommonShareTransitionStyle>(StyleTag::COMMON_SHARE_TRANSITION_STYLE);
                if (shareTransitionStyle.IsValid()) {
                    shareTransitionStyle.curve = CreateCurve(value);
                    shareTransitionStyle.sharedTransitionOption.SetCurve(shareTransitionStyle.curve);
                }
            } },
        { DOM_POSITION_START,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    if (declaration.IsRightToLeft()) {
                        positionStyle.right = declaration.ParseDimension(value);
                        declaration.hasRight_ = true;
                    } else {
                        positionStyle.left = declaration.ParseDimension(value);
                        declaration.hasLeft_ = true;
                    }
                    declaration.hasPositionStyle_ = true;
                }
            } },
        { DOM_POSITION_TOP,
            [](const std::string& value, Declaration& declaration) {
                auto& positionStyle = declaration.MaybeResetStyle<CommonPositionStyle>(StyleTag::COMMON_POSITION_STYLE);
                if (positionStyle.IsValid() && !value.empty()) {
                    positionStyle.top = declaration.ParseDimension(value);
                    declaration.hasPositionStyle_ = true;
                    declaration.hasTop_ = true;
                }
            } },
        { DOM_TRANSFORM_ORIGIN,
            [](const std::string& val, Declaration& declaration) {
                declaration.hasTransformStyle_ = true;
                auto& animationStyle =
                    declaration.MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
                if (!animationStyle.IsValid()) {
                    LOGD("don't support animation style");
                    return;
                }

                std::vector<std::string> offsets;
                StringUtils::StringSpliter(val, ' ', offsets);
                if (offsets.size() == TRANSFORM_SINGLE) {
                    Dimension originDimensionX = TRANSFORM_ORIGIN_DEFAULT;
                    Dimension originDimensionY = TRANSFORM_ORIGIN_DEFAULT;
                    // for Enum
                    if (CheckTransformEnum(val)) {
                        auto resultX = ConvertStrToTransformOrigin(val, Axis::HORIZONTAL);
                        if (resultX.first) {
                            originDimensionX = resultX.second;
                        }
                        auto resultY = ConvertStrToTransformOrigin(val, Axis::VERTICAL);
                        if (resultY.first) {
                            originDimensionY = resultY.second;
                        }
                    } else {
                        // for Dimension
                        originDimensionX = declaration.ParseDimension(val);
                    }
                    animationStyle.tweenOption.SetTransformOrigin(originDimensionX, originDimensionY);
                    animationStyle.transformOriginX = originDimensionX;
                    animationStyle.transformOriginY = originDimensionY;
                } else if (offsets.size() == TRANSFORM_DUAL) {
                    Dimension originDimensionX = TRANSFORM_ORIGIN_DEFAULT;
                    Dimension originDimensionY = TRANSFORM_ORIGIN_DEFAULT;
                    if (CheckTransformEnum(offsets[0])) {
                        auto result = ConvertStrToTransformOrigin(offsets[0], Axis::HORIZONTAL);
                        if (result.first) {
                            originDimensionX = result.second;
                        }
                    } else {
                        originDimensionX = declaration.ParseDimension(offsets[0]);
                    }

                    if (CheckTransformEnum(offsets[1])) {
                        auto result = ConvertStrToTransformOrigin(offsets[1], Axis::VERTICAL);
                        if (result.first) {
                            originDimensionY = result.second;
                        }
                    } else {
                        originDimensionY = declaration.ParseDimension(offsets[1]);
                    }
                    animationStyle.tweenOption.SetTransformOrigin(originDimensionX, originDimensionY);
                    animationStyle.transformOriginX = originDimensionX;
                    animationStyle.transformOriginY = originDimensionY;
                }
                declaration.hasTransformOriginStyle_ = true;
            } },
        { DOM_TRANSITION_DURATION,
            [](const std::string& value, Declaration& declaration) {
                auto& pageTransitionStyle =
                    declaration.MaybeResetStyle<CommonPageTransitionStyle>(StyleTag::COMMON_PAGE_TRANSITION_STYLE);
                if (pageTransitionStyle.IsValid()) {
                    if (value.find("ms") != std::string::npos) {
                        pageTransitionStyle.transitionDuration = StringUtils::StringToInt(value);
                    } else if (value.find('s') != std::string::npos) {
                        pageTransitionStyle.transitionDuration = StringUtils::StringToInt(value) * MS_TO_S;
                    } else {
                        // default unit is ms
                        pageTransitionStyle.transitionDuration = StringUtils::StringToInt(value);
                    }
                    pageTransitionStyle.transitionEnterOption.SetDuration(pageTransitionStyle.transitionDuration);
                    pageTransitionStyle.transitionExitOption.SetDuration(pageTransitionStyle.transitionDuration);
                }
            } },
        // card transition
        { DOM_TRANSITION_EFFECT,
            [](const std::string& value, Declaration& declaration) {
                declaration.hasTransitionAnimation_ = true;
                auto& cardTransitionStyle =
                    declaration.MaybeResetStyle<CommonCardTransitionStyle>(StyleTag::COMMON_CARD_TRANSITION_STYLE);
                if (cardTransitionStyle.IsValid()) {
                    declaration.hasTransformStyle_ = true;
                    cardTransitionStyle.transitionEffect = ParseTransitionEffect(value);
                }
            } },
        { DOM_TRANSITION_TIMING_FUNCTION,
            [](const std::string& value, Declaration& declaration) {
                auto& pageTransitionStyle =
                    declaration.MaybeResetStyle<CommonPageTransitionStyle>(StyleTag::COMMON_PAGE_TRANSITION_STYLE);
                if (pageTransitionStyle.IsValid()) {
                    pageTransitionStyle.curve = CreateCurve(value);
                    pageTransitionStyle.transitionEnterOption.SetCurve(pageTransitionStyle.curve);
                    pageTransitionStyle.transitionExitOption.SetCurve(pageTransitionStyle.curve);
                }
            } },
        { DOM_VISIBILITY,
            [](const std::string& value, Declaration& declaration) {
                auto& visibilityStyle =
                    declaration.MaybeResetStyle<CommonVisibilityStyle>(StyleTag::COMMON_VISIBILITY_STYLE);
                if (visibilityStyle.IsValid()) {
                    visibilityStyle.visibility =
                        (value == DOM_VISIBILITY_HIDDEN) ? VisibilityType::HIDDEN : VisibilityType::VISIBLE;
                    declaration.hasDisplayStyle_ = true;
                }
            } },
        { DOM_WIDTH,
            [](const std::string& value, Declaration& declaration) {
                auto& sizeStyle = declaration.MaybeResetStyle<CommonSizeStyle>(StyleTag::COMMON_SIZE_STYLE);
                if (sizeStyle.IsValid()) {
                    sizeStyle.width = declaration.ParseCalcDimension(value);
                    declaration.hasBoxStyle_ = true;
                }
            } },
        { DOM_WINDOW_FILTER,
            [](const std::string& value, Declaration& declaration) {
                declaration.hasDecorationStyle_ = true;
                std::vector<std::string> offsets;
                StringUtils::StringSpliter(value, ' ', offsets);
                // progress
                if (offsets.size() >= 1) {
                    auto parseValue = ParseFunctionValue<Dimension>(offsets[0], DOM_BLUR, StringToDimension);
                    if (parseValue.Unit() == DimensionUnit::PERCENT) {
                        auto progress = parseValue.Value();
                        if (GreatNotEqual(progress, 0.0) && LessOrEqual(progress, 1.0)) {
                            declaration.backDecoration_->SetWindowBlurProgress(static_cast<float>(progress));
                        }
                    } else {
                        declaration.backDecoration_->SetWindowBlurProgress(static_cast<float>(0.0f));
                    }
                }
                // style
                if (offsets.size() >= 2) {
                    auto windowBlurStyle = StrToWindowBlurStyle(offsets[1]);
                    declaration.backDecoration_->SetWindowBlurStyle(windowBlurStyle);
                }
            } },
        { DOM_ZINDEX,
            [](const std::string& value, Declaration& declaration) {
                auto& commonStyle = declaration.MaybeResetStyle<CommonStyle>(StyleTag::COMMON_STYLE);
                if (commonStyle.IsValid()) {
                    commonStyle.zIndex = StringToInt(value);
                }
            } },
    };

    auto operatorIter = BinarySearchFindIndex(styleSetter, ArraySize(styleSetter), style.first.c_str());
    if (operatorIter != -1) {
        styleSetter[operatorIter].value(style.second, *this);
    }

    auto& renderAttr = static_cast<CommonRenderAttribute&>(GetAttribute(AttributeTag::COMMON_RENDER_ATTR));
    static const std::unordered_set<std::string> displayStyleSet = { DOM_OPACITY, DOM_DISPLAY, DOM_VISIBILITY };
    if (displayStyleSet.find(style.first) != displayStyleSet.end() &&
        AceApplicationInfo::GetInstance().GetIsCardType() && renderAttr.show == "false") {
        SetShowAttr(renderAttr.show);
    }
}

void Declaration::AddEvent(int32_t pageId, const std::string& eventId, const std::vector<std::string>& events)
{
    ACE_SCOPED_TRACE("Declaration::AddEvent");
    static const LinearMapNode<void (*)(int32_t, const std::string&, Declaration&)> eventSetters[] = {
        { DOM_BLUR,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& focusableEvent = declaration.MaybeResetEvent<CommonFocusEvent>(EventTag::COMMON_FOCUS_EVENT);
                if (focusableEvent.IsValid()) {
                    focusableEvent.blur.eventMarker = EventMarker(eventId, DOM_BLUR, pageId);
                    focusableEvent.blur.isRefreshed = true;
                }
            } },
        { DOM_CAPTURE_TOUCH_CANCEL,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.captureTouchCancel.eventMarker = EventMarker(eventId, DOM_CAPTURE_TOUCH_CANCEL, pageId);
                    rawEvent.captureTouchCancel.isRefreshed = true;
                }
            } },
        { DOM_CAPTURE_TOUCH_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.captureTouchEnd.eventMarker = EventMarker(eventId, DOM_CAPTURE_TOUCH_END, pageId);
                    rawEvent.captureTouchEnd.isRefreshed = true;
                }
            } },
        { DOM_CAPTURE_TOUCH_MOVE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.captureTouchMove.eventMarker = EventMarker(eventId, DOM_CAPTURE_TOUCH_MOVE, pageId);
                    rawEvent.captureTouchMove.isRefreshed = true;
                }
            } },
        { DOM_CAPTURE_TOUCH_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.captureTouchStart.eventMarker = EventMarker(eventId, DOM_CAPTURE_TOUCH_START, pageId);
                    rawEvent.captureTouchStart.isRefreshed = true;
                }
            } },
        { DOM_CATCH_BUBBLE_CLICK,
          [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
            auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
            if (gestureEvent.IsValid()) {
                gestureEvent.click.eventMarker = EventMarker(eventId, DOM_CATCH_BUBBLE_CLICK, pageId);
                gestureEvent.click.eventMarker.SetCatchMode(true);
                gestureEvent.click.isRefreshed = true;
            }
          } },
        { DOM_CATCH_BUBBLE_TOUCH_CANCEL,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchBubbleTouchCancel.eventMarker =
                        EventMarker(eventId, DOM_CATCH_BUBBLE_TOUCH_CANCEL, pageId);
                    rawEvent.catchBubbleTouchCancel.isRefreshed = true;
                }
            } },
        { DOM_CATCH_BUBBLE_TOUCH_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchBubbleTouchEnd.eventMarker = EventMarker(eventId, DOM_CATCH_BUBBLE_TOUCH_END, pageId);
                    rawEvent.catchBubbleTouchEnd.isRefreshed = true;
                }
            } },
        { DOM_CATCH_BUBBLE_TOUCH_MOVE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchBubbleTouchMove.eventMarker =
                        EventMarker(eventId, DOM_CATCH_BUBBLE_TOUCH_MOVE, pageId);
                    rawEvent.catchBubbleTouchMove.isRefreshed = true;
                }
            } },
        { DOM_CATCH_BUBBLE_TOUCH_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchBubbleTouchStart.eventMarker =
                        EventMarker(eventId, DOM_CATCH_BUBBLE_TOUCH_START, pageId);
                    rawEvent.catchBubbleTouchStart.isRefreshed = true;
                }
            } },
        { DOM_CATCH_CAPTURE_TOUCH_CANCEL,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchCaptureTouchCancel.eventMarker =
                        EventMarker(eventId, DOM_CATCH_CAPTURE_TOUCH_CANCEL, pageId);
                    rawEvent.catchCaptureTouchCancel.isRefreshed = true;
                }
            } },
        { DOM_CATCH_CAPTURE_TOUCH_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchCaptureTouchEnd.eventMarker =
                        EventMarker(eventId, DOM_CATCH_CAPTURE_TOUCH_END, pageId);
                    rawEvent.catchCaptureTouchEnd.isRefreshed = true;
                }
            } },
        { DOM_CATCH_CAPTURE_TOUCH_MOVE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchCaptureTouchMove.eventMarker =
                        EventMarker(eventId, DOM_CATCH_CAPTURE_TOUCH_MOVE, pageId);
                    rawEvent.catchCaptureTouchMove.isRefreshed = true;
                }
            } },
        { DOM_CATCH_CAPTURE_TOUCH_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.catchCaptureTouchStart.eventMarker =
                        EventMarker(eventId, DOM_CATCH_CAPTURE_TOUCH_START, pageId);
                    rawEvent.catchCaptureTouchStart.isRefreshed = true;
                }
            } },
        { DOM_CLICK,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.click.eventMarker = EventMarker(eventId, DOM_CLICK, pageId);
                    gestureEvent.click.eventMarker.SetCatchMode(false);
                    gestureEvent.click.isRefreshed = true;
                }
            } },
        { DOM_DOUBLE_CLICK,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.doubleClick.eventMarker = EventMarker(eventId, DOM_DOUBLE_CLICK, pageId);
                    gestureEvent.doubleClick.isRefreshed = true;
                }
            } },
        { DOM_DRAG,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.drag.eventMarker = EventMarker(eventId, DOM_DRAG, pageId);
                    gestureEvent.drag.isRefreshed = true;
                }
            } },
        { DOM_DRAG_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragEnd.eventMarker = EventMarker(eventId, DOM_DRAG_END, pageId);
                    gestureEvent.dragEnd.isRefreshed = true;
                }
            } },
        { DOM_DRAG_ENTER,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragEnter.eventMarker = EventMarker(eventId, DOM_DRAG_ENTER, pageId);
                    gestureEvent.dragEnter.isRefreshed = true;
                }
            } },
        { DOM_DRAG_LEAVE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragLeave.eventMarker = EventMarker(eventId, DOM_DRAG_LEAVE, pageId);
                    gestureEvent.dragLeave.isRefreshed = true;
                }
            } },
        { DOM_DRAG_OVER,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragOver.eventMarker = EventMarker(eventId, DOM_DRAG_OVER, pageId);
                    gestureEvent.dragOver.isRefreshed = true;
                }
            } },
        { DOM_DRAG_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragStart.eventMarker = EventMarker(eventId, DOM_DRAG_START, pageId);
                    gestureEvent.dragStart.isRefreshed = true;
                }
            } },
        { DOM_DRAG_DROP,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.dragDrop.eventMarker = EventMarker(eventId, DOM_DRAG_DROP, pageId);
                    gestureEvent.dragDrop.isRefreshed = true;
                }
            } },
        { DOM_FOCUS,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& focusEvent = declaration.MaybeResetEvent<CommonFocusEvent>(EventTag::COMMON_FOCUS_EVENT);
                if (focusEvent.IsValid()) {
                    focusEvent.focus.eventMarker = EventMarker(eventId, DOM_FOCUS, pageId);
                    focusEvent.focus.isRefreshed = true;
                }
            } },
        { DOM_HOVER,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& mouseEvent = declaration.MaybeResetEvent<CommonMouseEvent>(EventTag::COMMON_MOUSE_EVENT);
                if (mouseEvent.IsValid()) {
                    mouseEvent.mouseHover.eventMarker = EventMarker(eventId, DOM_HOVER, pageId);
                    mouseEvent.mouseHover.isRefreshed = true;
                }
            } },
        { DOM_KEY,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& keyEvent = declaration.MaybeResetEvent<CommonKeyEvent>(EventTag::COMMON_KEY_EVENT);
                if (keyEvent.IsValid()) {
                    keyEvent.key.eventMarker = EventMarker(eventId, DOM_KEY, pageId);
                    keyEvent.key.isRefreshed = true;
                }
            } },
        { DOM_LONG_PRESS,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.longPress.eventMarker = EventMarker(eventId, DOM_LONG_PRESS, pageId);
                    gestureEvent.longPress.isRefreshed = true;
                }
            } },
        { DOM_MOUSE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& mouseEvent = declaration.MaybeResetEvent<CommonMouseEvent>(EventTag::COMMON_MOUSE_EVENT);
                if (mouseEvent.IsValid()) {
                    mouseEvent.mouse.eventMarker = EventMarker(eventId, DOM_MOUSE, pageId);
                    mouseEvent.mouse.isRefreshed = true;
                }
            } },
        { DOM_PINCH_CANCEL,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.pinchCancel.eventMarker = EventMarker(eventId, DOM_PINCH_CANCEL, pageId);
                    gestureEvent.pinchCancel.isRefreshed = true;
                }
            } },
        { DOM_PINCH_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.pinchEnd.eventMarker = EventMarker(eventId, DOM_PINCH_END, pageId);
                    gestureEvent.pinchEnd.isRefreshed = true;
                }
            } },
        { DOM_PINCH_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.pinchStart.eventMarker = EventMarker(eventId, DOM_PINCH_START, pageId);
                    gestureEvent.pinchStart.isRefreshed = true;
                }
            } },
        { DOM_PINCH_UPDATE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& gestureEvent = declaration.MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
                if (gestureEvent.IsValid()) {
                    gestureEvent.pinchUpdate.eventMarker = EventMarker(eventId, DOM_PINCH_UPDATE, pageId);
                    gestureEvent.pinchUpdate.isRefreshed = true;
                }
            } },
        { DOM_CROWN_ROTATE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& crownEvent = declaration.MaybeResetEvent<CommonCrownEvent>(EventTag::COMMON_CROWN_EVENT);
                if (crownEvent.IsValid()) {
                    crownEvent.rotate.eventMarker = EventMarker(eventId, DOM_CROWN_ROTATE, pageId);
                    crownEvent.rotate.isRefreshed = true;
                }
            } },
        { DOM_SWIPE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& swipeEvent = declaration.MaybeResetEvent<CommonSwipeEvent>(EventTag::COMMON_SWIPE_EVENT);
                if (swipeEvent.IsValid()) {
                    swipeEvent.swipe.eventMarker = EventMarker(eventId, DOM_SWIPE, pageId);
                    swipeEvent.swipe.isRefreshed = true;
                }
            } },
        { DOM_TOUCH_CANCEL,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.touchCancel.eventMarker = EventMarker(eventId, DOM_TOUCH_CANCEL, pageId);
                    rawEvent.touchCancel.isRefreshed = true;
                }
            } },
        { DOM_TOUCH_END,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.touchEnd.eventMarker = EventMarker(eventId, DOM_TOUCH_END, pageId);
                    rawEvent.touchEnd.isRefreshed = true;
                }
            } },
        { DOM_TOUCH_MOVE,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.touchMove.eventMarker = EventMarker(eventId, DOM_TOUCH_MOVE, pageId);
                    rawEvent.touchMove.isRefreshed = true;
                }
            } },
        { DOM_TOUCH_START,
            [](int32_t pageId, const std::string& eventId, Declaration& declaration) {
                auto& rawEvent = declaration.MaybeResetEvent<CommonRawEvent>(EventTag::COMMON_RAW_EVENT);
                if (rawEvent.IsValid()) {
                    rawEvent.touchStart.eventMarker = EventMarker(eventId, DOM_TOUCH_START, pageId);
                    rawEvent.touchStart.isRefreshed = true;
                }
            } },
    };
    for (const auto& event : events) {
        if (SetSpecializedEvent(pageId, eventId, event)) {
            continue;
        }
        auto setterIter = BinarySearchFindIndex(eventSetters, ArraySize(eventSetters), event.c_str());
        if (setterIter != -1) {
            eventSetters[setterIter].value(pageId, eventId, *this);
        }
    }
}

void Declaration::CallMethod(const std::string& method, const std::string& args)
{
    if (method == COMMON_METHOD_FOCUS) {
        if (!focusableController_) {
            LOGE("CallMethod: call focus method failed, focusableController_ is null");
            return;
        }

        bool shouldFocus = true;
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (argsValue && argsValue->IsArray() && argsValue->GetArraySize() == COMMON_METHOD_FOCUS_ARGS_SIZE) {
            std::unique_ptr<JsonValue> focusValue = argsValue->GetArrayItem(0)->GetValue(COMMON_METHOD_FOCUS);
            if (focusValue && focusValue->IsBool()) {
                shouldFocus = focusValue->GetBool();
            }
        }
        OnRequestFocus(shouldFocus);
    } else if (method == DOM_LIST_METHOD_SCROLL_BY) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
            LOGE("parse args error");
            return;
        }
        std::unique_ptr<JsonValue> scrollByPara = argsValue->GetArrayItem(0);
        double x = scrollByPara->GetDouble("dx", 0.0);
        double y = scrollByPara->GetDouble("dy", 0.0);
        bool isSmooth = scrollByPara->GetBool("smooth", true);
        OnScrollBy(x, y, isSmooth);
    } else {
        CallSpecializedMethod(method, args);
    }
}

void Declaration::OnRequestFocus(bool shouldFocus)
{
    auto& commonMethod = MaybeResetMethod<CommonMethod>(MethodTag::COMMON_METHOD);
    if (commonMethod.IsValid()) {
        commonMethod.Focus(focusableController_, shouldFocus);
    }
}

void Declaration::OnScrollBy(double dx, double dy, bool isSmooth)
{
    auto& commonMethod = MaybeResetMethod<CommonMethod>(MethodTag::COMMON_METHOD);
    if (commonMethod.IsValid()) {
        commonMethod.ScrollBy(positionController_, dx, dy, isSmooth);
    }
}

void Declaration::SetPaddingOverall(const std::string& value, Declaration& declaration)
{
    auto& paddingStyle = declaration.MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
    if (!paddingStyle.IsValid()) {
        LOGD("don't support padding");
        return;
    }

    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            paddingStyle.padding.SetLeft(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetRight(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetTop(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetBottom(declaration.ParseDimension(offsets[0]));
            break;
        case 2:
            paddingStyle.padding.SetLeft(declaration.ParseDimension(offsets[1]));
            paddingStyle.padding.SetRight(declaration.ParseDimension(offsets[1]));
            paddingStyle.padding.SetTop(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetBottom(declaration.ParseDimension(offsets[0]));
            break;
        case 3:
            paddingStyle.padding.SetLeft(declaration.ParseDimension(offsets[1]));
            paddingStyle.padding.SetRight(declaration.ParseDimension(offsets[1]));
            paddingStyle.padding.SetTop(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetBottom(declaration.ParseDimension(offsets[2]));
            break;
        case 4:
            paddingStyle.padding.SetLeft(declaration.ParseDimension(offsets[3]));
            paddingStyle.padding.SetRight(declaration.ParseDimension(offsets[1]));
            paddingStyle.padding.SetTop(declaration.ParseDimension(offsets[0]));
            paddingStyle.padding.SetBottom(declaration.ParseDimension(offsets[2]));
            break;
        default:
            break;
    }
    declaration.hasBoxStyle_ = true;
}

void Declaration::SetMarginOverall(const std::string& value, Declaration& declaration)
{
    auto& marginStyle = declaration.MaybeResetStyle<CommonMarginStyle>(StyleTag::COMMON_MARGIN_STYLE);
    if (!marginStyle.IsValid()) {
        LOGD("don't support margin");
        return;
    }

    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            marginStyle.margin.SetLeft(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetRight(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetTop(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetBottom(declaration.ParseDimension(offsets[0]));
            break;
        case 2:
            marginStyle.margin.SetLeft(declaration.ParseDimension(offsets[1]));
            marginStyle.margin.SetRight(declaration.ParseDimension(offsets[1]));
            marginStyle.margin.SetTop(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetBottom(declaration.ParseDimension(offsets[0]));
            break;
        case 3:
            marginStyle.margin.SetLeft(declaration.ParseDimension(offsets[1]));
            marginStyle.margin.SetRight(declaration.ParseDimension(offsets[1]));
            marginStyle.margin.SetTop(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetBottom(declaration.ParseDimension(offsets[2]));
            break;
        case 4:
            marginStyle.margin.SetLeft(declaration.ParseDimension(offsets[3]));
            marginStyle.margin.SetRight(declaration.ParseDimension(offsets[1]));
            marginStyle.margin.SetTop(declaration.ParseDimension(offsets[0]));
            marginStyle.margin.SetBottom(declaration.ParseDimension(offsets[2]));
            break;
        default:
            break;
    }
    declaration.hasBoxStyle_ = true;
}

void Declaration::SetBorderImageWidthForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]),
                BorderImageDirection::BOTTOM);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::RIGHT);
            declaration.backDecoration_->SetHasBorderImageWidth(true);
            break;
        case 2:
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]),
                BorderImageDirection::BOTTOM);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            declaration.backDecoration_->SetHasBorderImageWidth(true);
            break;
        case 3:
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[2]),
                BorderImageDirection::BOTTOM);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            declaration.backDecoration_->SetHasBorderImageWidth(true);
            break;
        case 4:
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[2]),
                BorderImageDirection::BOTTOM);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(offsets[3]), BorderImageDirection::RIGHT);
            declaration.backDecoration_->SetHasBorderImageWidth(true);
            break;
        default:
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(value), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(value),
                BorderImageDirection::BOTTOM);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(value), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageWidth(declaration.ParseDimension(value), BorderImageDirection::RIGHT);
            declaration.backDecoration_->SetHasBorderImageWidth(false);
            break;
    }
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImageSliceForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::RIGHT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]),
                BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageSlice(true);
            break;
        case 2:
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]),
                BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageSlice(true);
            break;
        case 3:
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[2]),
                BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageSlice(true);
            break;
        case 4:
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[3]), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(offsets[2]),
                BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageSlice(true);
            break;
        default:
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(value), BorderImageDirection::LEFT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(value), BorderImageDirection::TOP);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(value), BorderImageDirection::RIGHT);
            borderStyle.border.SetBorderImageSlice(declaration.ParseDimension(value),
                BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageSlice(false);
            break;
    }
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImageOutSetForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& bs = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::LEFT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::RIGHT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageOutset(true);
            break;
        case 2:
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageOutset(true);
            break;
        case 3:
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[1]), BorderImageDirection::LEFT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[2]), BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageOutset(true);
            break;
        case 4:
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[3]), BorderImageDirection::LEFT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[0]), BorderImageDirection::TOP);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[1]), BorderImageDirection::RIGHT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(offsets[2]), BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageOutset(true);
            break;
        default:
            bs.border.SetBorderImageOutset(declaration.ParseDimension(value), BorderImageDirection::LEFT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(value), BorderImageDirection::TOP);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(value), BorderImageDirection::RIGHT);
            bs.border.SetBorderImageOutset(declaration.ParseDimension(value), BorderImageDirection::BOTTOM);
            declaration.backDecoration_->SetHasBorderImageOutset(false);
            break;
    }
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImageRepeatForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    borderStyle.border.SetBorderImageRepeat(ConvertStrToBorderImageRepeat(value));
    borderStyle.border.SetBorderImageRepeat(ConvertStrToBorderImageRepeat(value));
    borderStyle.border.SetBorderImageRepeat(ConvertStrToBorderImageRepeat(value));
    borderStyle.border.SetBorderImageRepeat(ConvertStrToBorderImageRepeat(value));
    declaration.backDecoration_->SetHasBorderImageRepeat(true);
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImage(const std::string& value, Declaration& declaration)
{
    declaration.backDecoration_->SetHasBorderImageSource(false);
    declaration.backDecoration_->SetHasBorderImageGradient(false);
    SetBorderImageSliceForFourEdges("", declaration);
    SetBorderImageWidthForFourEdges("", declaration);
    SetBorderImageOutSetForFourEdges("", declaration);
    SetBorderImageRepeatForFourEdges("", declaration);

    auto borderImageJson = JsonUtil::ParseJsonString(value);
    if (!borderImageJson->IsObject()) {
        LOGE("borderImageJson json is not Object");
        return;
    }
    if (borderImageJson->Contains(DOM_VALUES) && borderImageJson->GetValue(DOM_VALUES)->IsArray() &&
        borderImageJson->GetValue(DOM_VALUES)->GetArraySize() > 0) {

        auto values = borderImageJson->GetValue(DOM_VALUES)->GetArrayItem(0);

        if (values->Contains("url")) {
            SetBorderImageUrl(values, declaration);
        } else {
            SetBorderImageGradient(values, declaration);
        }
    }
}

void Declaration::SetBorderImageGradient(const std::unique_ptr<JsonValue>& values, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    backgroundStyle.gradientBorderImage = Gradient();
    if (values->Contains(DOM_GRADIENT_TYPE) && values->GetValue(DOM_GRADIENT_TYPE)->IsString()) {
        SetBorderImageGradientType(values->GetValue(DOM_GRADIENT_TYPE)->GetString(), declaration);
    }
    if (values->Contains(DOM_GRADIENT_DIRECTIONS) && values->GetValue(DOM_GRADIENT_DIRECTIONS)->IsArray()) {
        SetBorderImageGradientDirections(values->GetValue(DOM_GRADIENT_DIRECTIONS), declaration);
    }
    if (values->Contains(DOM_GRADIENT_VALUES) && values->GetValue(DOM_GRADIENT_VALUES)->IsArray()) {
        SetBorderImageGradientColor(values->GetValue(DOM_GRADIENT_VALUES), declaration);
    }
    if (values->Contains("slice") && values->GetValue("slice")->IsArray()) {
        std::unique_ptr<JsonValue> sliceItem = values->GetValue("slice");
        std::string sliceStr;
        for (int32_t i = 0; i < sliceItem->GetArraySize(); i++) {
            sliceStr += sliceItem->GetArrayItem(i)->GetString() + " ";
        }
        SetBorderImageSliceForFourEdges(sliceStr, declaration);
    }
    declaration.backDecoration_->SetHasBorderImageGradient(true);
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImageUrl(const std::unique_ptr<JsonValue>& values, Declaration& declaration)
{
    if (values->Contains("url") && values->GetValue("url")->IsString()) {
        SetBorderImageFindUrl(values->GetValue("url")->GetString(), declaration);
    }
    if (values->Contains("slice") && values->GetValue("slice")->IsArray()) {
        std::unique_ptr<JsonValue> sliceItem = values->GetValue("slice");
        std::string sliceStr;
        for (int32_t i = 0; i < sliceItem->GetArraySize(); i++) {
            sliceStr += sliceItem->GetArrayItem(i)->GetString() + " ";
        }
        SetBorderImageSliceForFourEdges(sliceStr, declaration);
    }
    if (values->Contains("width") && values->GetValue("width")->IsArray()) {
        std::unique_ptr<JsonValue> widthItem = values->GetValue("width");

        std::string widthStr;
        for (int32_t i = 0; i < widthItem->GetArraySize(); i++) {
            widthStr += widthItem->GetArrayItem(i)->GetString() + " ";
        }
        SetBorderImageWidthForFourEdges(widthStr, declaration);
    }
    if (values->Contains("outset") && values->GetValue("outset")->IsArray()) {
        std::unique_ptr<JsonValue> outsetItem = values->GetValue("outset");

        std::string outsetStr;
        for (int32_t i = 0; i < outsetItem->GetArraySize(); i++) {
            outsetStr += outsetItem->GetArrayItem(i)->GetString() + " ";
        }
        SetBorderImageOutSetForFourEdges(outsetStr, declaration);
    }
    if (values->Contains("repeat") && values->GetValue("repeat")->IsString()) {
        SetBorderImageRepeatForFourEdges(values->GetValue("repeat")->GetString(), declaration);
    }
    declaration.hasDecorationStyle_ = true;
    declaration.hasBorderStyle_ = true;
}

void Declaration::SetBorderImageFindUrl(const std::string& value, Declaration& declaration)
{
    auto& backgroundStyle =
                    declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (backgroundStyle.IsValid()) {
        backgroundStyle.borderImage->SetSrc(value);
        declaration.backDecoration_->SetBorderImage(backgroundStyle.borderImage);
        declaration.backDecoration_->SetHasBorderImageSource(true);
        declaration.hasDecorationStyle_ = true;
    }
}

void Declaration::SetBorderImageGradientType(const std::string& gradientType, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    // default: LINEAR
    backgroundStyle.gradientBorderImage.SetType(GradientType::LINEAR);
    if (gradientType == DOM_RADIAL_GRADIENT || gradientType == DOM_REPEATING_RADIAL_GRADIENT) {
        backgroundStyle.gradientBorderImage.SetType(GradientType::RADIAL);
    } else if (gradientType == DOM_SWEEP_GRADIENT || gradientType == DOM_REPEATING_SWEEP_GRADIENT) {
        backgroundStyle.gradientBorderImage.SetType(GradientType::SWEEP);
    }

    if (gradientType == DOM_REPEATING_LINEAR_GRADIENT || gradientType == DOM_REPEATING_RADIAL_GRADIENT ||
        gradientType == DOM_REPEATING_SWEEP_GRADIENT) {
        backgroundStyle.gradientBorderImage.SetRepeat(true);
    }
    declaration.hasDecorationStyle_ = true;
}

void Declaration::SetBorderImageGradientDirections(const std::unique_ptr<JsonValue>& gradientDirections,
    Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }

    std::unique_ptr<JsonValue> angleItem;
    std::unique_ptr<JsonValue> sideItem;
    std::unique_ptr<JsonValue> cornerItem;
    GradientDirection direction;
    switch (gradientDirections->GetArraySize()) {
        case DIRECTION_ANGLE:
            angleItem = gradientDirections->GetArrayItem(0);
            if (angleItem->IsString()) {
                LinearGradient linearGradient;
                linearGradient.angle = AnimatableDimension(StringToDouble(angleItem->GetString()));
                backgroundStyle.gradientBorderImage.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        case DIRECTION_SIDE:
            sideItem = gradientDirections->GetArrayItem(1);
            if (sideItem->IsString()) {
                direction = StrToGradientDirection(sideItem->GetString());
                LinearGradient linearGradient;
                if (LinearGradient::IsXAxis(direction)) {
                    linearGradient.linearX = direction;
                } else {
                    linearGradient.linearY = direction;
                }
                backgroundStyle.gradientBorderImage.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        case DIRECTION_CORNER:
            sideItem = gradientDirections->GetArrayItem(1);
            cornerItem = gradientDirections->GetArrayItem(2);
            if (sideItem->IsString() && cornerItem->IsString()) {
                LinearGradient linearGradient;
                auto direction1 = StrToGradientDirection(sideItem->GetString());
                auto direction2 = StrToGradientDirection(cornerItem->GetString());
                if ((LinearGradient::IsXAxis(direction1) && LinearGradient::IsXAxis(direction2)) ||
                    (!LinearGradient::IsXAxis(direction1) && !LinearGradient::IsXAxis(direction2))) {
                    linearGradient.linearY = GradientDirection::BOTTOM;
                    break;
                } else {
                    if (LinearGradient::IsXAxis(direction1)) {
                        linearGradient.linearX = direction1;
                        linearGradient.linearY = direction2;
                    } else {
                        linearGradient.linearY = direction1;
                        linearGradient.linearX = direction2;
                    }
                }
                backgroundStyle.gradientBorderImage.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        default:
            LOGE("gradientDirectionsLength error");
            break;
    }
}

void Declaration::SetBorderImageGradientColor(const std::unique_ptr<JsonValue>& gradientColorValues,
    Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    backgroundStyle.gradientBorderImage.ClearColors();
    int32_t gradientColorValuesLength = gradientColorValues->GetArraySize();
    for (int32_t i = 0; i < gradientColorValuesLength; i++) {
        std::string gradientColorValue = gradientColorValues->GetArrayItem(i)->GetString();
        GradientColor gradientColor;
        RemoveHeadTailSpace(gradientColorValue);
        auto index = gradientColorValue.find(' ');
        if (index != std::string::npos && index != 0) {
            std::string color = gradientColorValue.substr(0, index);
            std::string area = gradientColorValue.substr(index + 1, gradientColorValue.size() - index - 1);
            gradientColor.SetColor(declaration.ParseColor(color));
            gradientColor.SetHasValue(true);
            if (area.find("px") != std::string::npos) {
                gradientColor.SetDimension(StringToDouble(area), DimensionUnit::PX);
            } else if (area.find('%') != std::string::npos) {
                gradientColor.SetDimension(StringToDouble(area), DimensionUnit::PERCENT);
            } else {
                LOGW("gradientColor DimensionUnit is incorrect)");
                gradientColor.SetHasValue(false);
            }
        } else {
            gradientColor.SetHasValue(false);
            gradientColor.SetColor(declaration.ParseColor(gradientColorValue));
        }
        backgroundStyle.gradientBorderImage.AddColor(gradientColor);
        declaration.hasDecorationStyle_ = true;
    }
}

void Declaration::SetBorderOverall(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    if (!borderStyle.IsValid()) {
        LOGD("don't support border");
        return;
    }

    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1:
            if (offsets[0].find("px") != std::string::npos) {
                SetBorderWidthForFourEdges(offsets[0], declaration);
            } else if (offsets[0] == "solid" || offsets[0] == "dotted" || offsets[0] == "dashed") {
                SetBorderStyleForFourEdges(offsets[0], declaration);
            } else {
                SetBorderColorForFourEdges(offsets[0], declaration);
            }
            break;
        case 2:
            SetBorderWidthForFourEdges(offsets[0], declaration);
            SetBorderStyleForFourEdges(offsets[1], declaration);
            break;
        case 3:
            SetBorderWidthForFourEdges(offsets[0], declaration);
            SetBorderStyleForFourEdges(offsets[1], declaration);
            SetBorderColorForFourEdges(offsets[2], declaration);
            break;
        default:
            break;
    }
}

void Declaration::SetBorderWidthForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    if (borderStyle.IsValid()) {
        borderStyle.border.SetWidth(declaration.ParseDimension(value));
        declaration.hasDecorationStyle_ = true;
        declaration.hasBorderStyle_ = true;
    }
}

void Declaration::SetBorderColorForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    if (borderStyle.IsValid()) {
        borderStyle.border.SetColor(declaration.ParseColor(value));
        declaration.hasDecorationStyle_ = true;
        declaration.hasBorderStyle_ = true;
    }
}

void Declaration::SetBorderStyleForFourEdges(const std::string& value, Declaration& declaration)
{
    auto& borderStyle = declaration.MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    if (borderStyle.IsValid()) {
        borderStyle.border.SetStyle(ConvertStrToBorderStyle(value));
        declaration.hasDecorationStyle_ = true;
        declaration.hasBorderStyle_ = true;
    }
}

void Declaration::SetMaskGradient(const std::string& value, Declaration& declaration)
{
    Declaration::SetBackground(value, declaration);
}

void Declaration::SetBackground(const std::string& value, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        LOGD("don't support background style");
        return;
    }

    LOGD("Declaration::SetBackground value:%{private}s", value.c_str());
    auto backgroundJson = JsonUtil::ParseJsonString(value);
    if (!backgroundJson->IsObject()) {
        LOGE("background json is not Object");
        return;
    }
    if (backgroundJson->Contains(DOM_VALUES) && backgroundJson->GetValue(DOM_VALUES)->IsArray() &&
        backgroundJson->GetValue(DOM_VALUES)->GetArraySize() > 0) {
        backgroundStyle.gradient = Gradient();
        auto values = backgroundJson->GetValue(DOM_VALUES)->GetArrayItem(0);
        // gradient type and repeating
        if (values->Contains(DOM_GRADIENT_TYPE) && values->GetValue(DOM_GRADIENT_TYPE)->IsString()) {
            SetGradientType(values->GetValue(DOM_GRADIENT_TYPE)->GetString(), declaration);
        }
        // linearGradient direction
        if (values->Contains(DOM_GRADIENT_DIRECTIONS) && values->GetValue(DOM_GRADIENT_DIRECTIONS)->IsArray()) {
            SetGradientDirections(values->GetValue(DOM_GRADIENT_DIRECTIONS), declaration);
        }
        // radialGradient shape
        if (values->Contains(DOM_GRADIENT_SHAPE) && values->GetValue(DOM_GRADIENT_SHAPE)->IsString()) {
            SetGradientShape(values->GetValue(DOM_GRADIENT_SHAPE)->GetString(), declaration);
        }
        // radialGradient size
        if (values->Contains(DOM_GRADIENT_SIZE) && values->GetValue(DOM_GRADIENT_SIZE)->IsString()) {
            SetGradientSize(values->GetValue(DOM_GRADIENT_SIZE)->GetString(), declaration);
        }
        // radialGradient or sweepGradient position
        if (values->Contains(DOM_GRADIENT_POSITION) && values->GetValue(DOM_GRADIENT_POSITION)->IsString()) {
            SetGradientPosition(values->GetValue(DOM_GRADIENT_POSITION)->GetString(), declaration);
        }
        // sweepGradient startAngle and endAngle
        if (values->Contains(DOM_GRADIENT_ANGLE) && values->GetValue(DOM_GRADIENT_ANGLE)->IsString()) {
            SetGradientAngle(values->GetValue(DOM_GRADIENT_ANGLE)->GetString(), declaration);
        }
        // sweepGradient rotation
        if (values->Contains(DOM_GRADIENT_ROTATION) && values->GetValue(DOM_GRADIENT_ROTATION)->IsString()) {
            SetGradientRotation(values->GetValue(DOM_GRADIENT_ROTATION)->GetString(), declaration);
        }
        // gradient color stops
        if (values->Contains(DOM_GRADIENT_VALUES) && values->GetValue(DOM_GRADIENT_VALUES)->IsArray()) {
            SetGradientColor(values->GetValue(DOM_GRADIENT_VALUES), declaration);
        }
    }
    declaration.hasDecorationStyle_ = true;
    declaration.hasBackGroundColor_ = true;
}

void Declaration::SetGradientType(const std::string& gradientType, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    // default: LINEAR
    backgroundStyle.gradient.SetType(GradientType::LINEAR);
    if (gradientType == DOM_RADIAL_GRADIENT || gradientType == DOM_REPEATING_RADIAL_GRADIENT) {
        backgroundStyle.gradient.SetType(GradientType::RADIAL);
    } else if (gradientType == DOM_SWEEP_GRADIENT || gradientType == DOM_REPEATING_SWEEP_GRADIENT) {
        backgroundStyle.gradient.SetType(GradientType::SWEEP);
    }

    if (gradientType == DOM_REPEATING_LINEAR_GRADIENT || gradientType == DOM_REPEATING_RADIAL_GRADIENT ||
        gradientType == DOM_REPEATING_SWEEP_GRADIENT) {
        backgroundStyle.gradient.SetRepeat(true);
    }
    declaration.hasDecorationStyle_ = true;
}

void Declaration::SetGradientDirections(const std::unique_ptr<JsonValue>& gradientDirections, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }

    std::unique_ptr<JsonValue> angleItem;
    std::unique_ptr<JsonValue> sideItem;
    std::unique_ptr<JsonValue> cornerItem;
    GradientDirection direction;
    switch (gradientDirections->GetArraySize()) {
        case DIRECTION_ANGLE:
            angleItem = gradientDirections->GetArrayItem(0);
            if (angleItem->IsString()) {
                LinearGradient linearGradient;
                linearGradient.angle = AnimatableDimension(StringToDouble(angleItem->GetString()));
                backgroundStyle.gradient.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        case DIRECTION_SIDE:
            sideItem = gradientDirections->GetArrayItem(1);
            if (sideItem->IsString()) {
                direction = StrToGradientDirection(sideItem->GetString());
                LinearGradient linearGradient;
                if (LinearGradient::IsXAxis(direction)) {
                    linearGradient.linearX = direction;
                } else {
                    linearGradient.linearY = direction;
                }
                backgroundStyle.gradient.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        case DIRECTION_CORNER:
            sideItem = gradientDirections->GetArrayItem(1);
            cornerItem = gradientDirections->GetArrayItem(2);
            if (sideItem->IsString() && cornerItem->IsString()) {
                LinearGradient linearGradient;
                auto direction1 = StrToGradientDirection(sideItem->GetString());
                auto direction2 = StrToGradientDirection(cornerItem->GetString());
                if ((LinearGradient::IsXAxis(direction1) && LinearGradient::IsXAxis(direction2)) ||
                    (!LinearGradient::IsXAxis(direction1) && !LinearGradient::IsXAxis(direction2))) {
                    linearGradient.linearY = GradientDirection::BOTTOM;
                    break;
                } else {
                    if (LinearGradient::IsXAxis(direction1)) {
                        linearGradient.linearX = direction1;
                        linearGradient.linearY = direction2;
                    } else {
                        linearGradient.linearY = direction1;
                        linearGradient.linearX = direction2;
                    }
                }
                backgroundStyle.gradient.SetLinearGradient(linearGradient);
                declaration.hasDecorationStyle_ = true;
            }
            break;
        default:
            LOGE("gradientDirectionsLength error");
            break;
    }
}

void Declaration::SetGradientColor(const std::unique_ptr<JsonValue>& gradientColorValues, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }

    backgroundStyle.gradient.ClearColors();
    int32_t gradientColorValuesLength = gradientColorValues->GetArraySize();
    for (int32_t i = 0; i < gradientColorValuesLength; i++) {
        std::string gradientColorValue = gradientColorValues->GetArrayItem(i)->GetString();
        GradientColor gradientColor;
        RemoveHeadTailSpace(gradientColorValue);
        auto index = gradientColorValue.find(' ');
        if (index != std::string::npos && index != 0) {
            std::string color = gradientColorValue.substr(0, index);
            std::string area = gradientColorValue.substr(index + 1, gradientColorValue.size() - index - 1);
            gradientColor.SetColor(declaration.ParseColor(color));
            gradientColor.SetHasValue(true);
            if (area.find("px") != std::string::npos) {
                gradientColor.SetDimension(StringToDouble(area), DimensionUnit::PX);
            } else if (area.find('%') != std::string::npos) {
                gradientColor.SetDimension(StringToDouble(area), DimensionUnit::PERCENT);
            } else {
                LOGW("gradientColor DimensionUnit is incorrect)");
                gradientColor.SetHasValue(false);
            }
        } else {
            gradientColor.SetHasValue(false);
            gradientColor.SetColor(declaration.ParseColor(gradientColorValue));
        }
        backgroundStyle.gradient.AddColor(gradientColor);
        declaration.hasDecorationStyle_ = true;
    }
}

void Declaration::SetGradientShape(const std::string& gradientShape, Declaration& declaration)
{
    // if empty do nothing, If shape is omitted, the ending shape defaults to a circle if the <size> is a single
    // <length>, and to an ellipse otherwise.
    if (gradientShape.empty()) {
        return;
    }
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }

    if (gradientShape == DOM_GRADIENT_SHAPE_ELLIPSE) {
        backgroundStyle.gradient.GetRadialGradient().radialShape = RadialShapeType::ELLIPSE;
    } else {
        backgroundStyle.gradient.GetRadialGradient().radialShape = RadialShapeType::CIRCLE;
    }
    declaration.hasDecorationStyle_ = true;
}

void Declaration::SetGradientSize(const std::string& gradientSize, Declaration& declaration)
{
    if (gradientSize.empty()) {
        return;
    }
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    // 1. closest-corner | closest-side | farthest-corner | farthest-side
    auto extent = ParseRadialGradientSize(gradientSize);
    if (extent) {
        backgroundStyle.gradient.GetRadialGradient().radialSizeType = extent;
        declaration.hasDecorationStyle_ = true;
        return;
    }

    std::vector<std::string> offsets;
    StringUtils::StringSpliter(gradientSize, ' ', offsets);
    if (offsets.size() == 1) {
        // 2. if circle: <length>
        auto circleSize = StringToDimension(offsets[0]);

        if (circleSize.Unit() != DimensionUnit::PX) {
            LOGE("circle only support length");
            return;
        }
        if (backgroundStyle.gradient.GetRadialGradient().radialShape &&
            backgroundStyle.gradient.GetRadialGradient().radialShape != RadialShapeType::CIRCLE) {
            LOGE("only circle support one size");
            return;
        }
        backgroundStyle.gradient.GetRadialGradient().radialVerticalSize = circleSize;
        backgroundStyle.gradient.GetRadialGradient().radialShape = RadialShapeType::CIRCLE;
        declaration.hasDecorationStyle_ = true;
    } else if (offsets.size() == 2) {
        // 3. if ellipse: <length-percentage>{2}
        auto horizontalSize = StringToDimension(offsets[0]);
        auto verticalSize = StringToDimension(offsets[1]);

        if (backgroundStyle.gradient.GetRadialGradient().radialShape &&
            backgroundStyle.gradient.GetRadialGradient().radialShape != RadialShapeType::ELLIPSE) {
            LOGE("only ellipse support two size");
            return;
        }
        backgroundStyle.gradient.GetRadialGradient().radialHorizontalSize = horizontalSize;
        backgroundStyle.gradient.GetRadialGradient().radialVerticalSize = verticalSize;
        backgroundStyle.gradient.GetRadialGradient().radialShape = RadialShapeType::ELLIPSE;
        declaration.hasDecorationStyle_ = true;
    } else {
        LOGE("unsupported offset size");
    }
}

void Declaration::SetGradientPosition(const std::string& gradientPosition, Declaration& declaration)
{
    if (gradientPosition.empty()) {
        return;
    }
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    // position determines the center of gradient default is center
    BackgroundImagePosition position;
    if (ParseBackgroundImagePosition(gradientPosition, position)) {
        auto xAxisPosition = Dimension(position.GetSizeValueX(),
            position.GetSizeTypeX() == BackgroundImagePositionType::PX ? DimensionUnit::PX : DimensionUnit::PERCENT);
        auto yAxisPosition = Dimension(position.GetSizeValueY(),
            position.GetSizeTypeY() == BackgroundImagePositionType::PX ? DimensionUnit::PX : DimensionUnit::PERCENT);
        if (backgroundStyle.gradient.GetType() == GradientType::RADIAL) {
            backgroundStyle.gradient.GetRadialGradient().radialCenterX = xAxisPosition;
            backgroundStyle.gradient.GetRadialGradient().radialCenterY = yAxisPosition;
            declaration.hasDecorationStyle_ = true;
        } else if (backgroundStyle.gradient.GetType() == GradientType::SWEEP) {
            backgroundStyle.gradient.GetSweepGradient().centerX = xAxisPosition;
            backgroundStyle.gradient.GetSweepGradient().centerY = yAxisPosition;
            declaration.hasDecorationStyle_ = true;
        }
    } else {
        LOGE("ParseBackgroundImagePosition failed");
    }
}

void Declaration::SetGradientAngle(const std::string& gradientAngle, Declaration& declaration)
{
    if (gradientAngle.empty()) {
        return;
    }
    auto backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        LOGE("backgroundStyle is invalid");
        return;
    }
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(gradientAngle, ' ', offsets);
    if (!offsets.empty()) {
        auto startAngle = StringUtils::StringToDegree(offsets[0]);
        backgroundStyle.gradient.GetSweepGradient().startAngle = AnimatableDimension(startAngle);
        if (offsets.size() > 1) {
            auto endAngle = StringUtils::StringToDegree(offsets[1]);
            backgroundStyle.gradient.GetSweepGradient().endAngle = AnimatableDimension(endAngle);
        }
        declaration.hasDecorationStyle_ = true;
    }
}

void Declaration::SetGradientRotation(const std::string& gradientRotation, Declaration& declaration)
{
    if (gradientRotation.empty()) {
        return;
    }
    auto backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        return;
    }
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(gradientRotation, ' ', offsets);
    if (!offsets.empty()) {
        auto rotationAngle = StringUtils::StringToDegree(offsets[0]);
        backgroundStyle.gradient.GetSweepGradient().rotation = AnimatableDimension(rotationAngle);
        declaration.hasDecorationStyle_ = true;
    }
}

void SetBgImgSizeX(const BackgroundImageSizeType type, const double value, BackgroundImageSize& bgImgSize)
{
    bgImgSize.SetSizeTypeX(type);
    bgImgSize.SetSizeValueX(value);
}

void SetBgImgSizeY(const BackgroundImageSizeType type, const double value, BackgroundImageSize& bgImgSize)
{
    bgImgSize.SetSizeTypeY(type);
    bgImgSize.SetSizeValueY(value);
}

void Declaration::SetBackgroundImageSize(const std::string& value, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        LOGD("don't support background");
        return;
    }

    static const LinearMapNode<BackgroundImageSizeType> bgImageSizeType[] = {
        { DOM_BACKGROUND_IMAGE_SIZE_AUTO, BackgroundImageSizeType::AUTO },
        { DOM_BACKGROUND_IMAGE_SIZE_CONTAIN, BackgroundImageSizeType::CONTAIN },
        { DOM_BACKGROUND_IMAGE_SIZE_COVER, BackgroundImageSizeType::COVER },
    };
    BackgroundImageSize bgImgSize;
    auto spaceIndex = value.find(' ', 0);
    if (spaceIndex != std::string::npos) {
        std::string valueX = value.substr(0, spaceIndex);
        std::string valueY = value.substr(spaceIndex + 1, value.size() - spaceIndex - 1);
        if (valueX.find("px") != std::string::npos) {
            SetBgImgSizeX(BackgroundImageSizeType::LENGTH, StringToDouble(valueX), bgImgSize);
        } else if (valueX.find('%') != std::string::npos) {
            SetBgImgSizeX(BackgroundImageSizeType::PERCENT, StringToDouble(valueX), bgImgSize);
        } else {
            bgImgSize.SetSizeTypeX(BackgroundImageSizeType::AUTO);
        }
        if (valueY.find("px") != std::string::npos) {
            SetBgImgSizeY(BackgroundImageSizeType::LENGTH, StringToDouble(valueY), bgImgSize);
        } else if (valueY.find('%') != std::string::npos) {
            SetBgImgSizeY(BackgroundImageSizeType::PERCENT, StringToDouble(valueY), bgImgSize);
        } else {
            bgImgSize.SetSizeTypeY(BackgroundImageSizeType::AUTO);
        }
    } else {
        auto sizeTypeIter = BinarySearchFindIndex(bgImageSizeType, ArraySize(bgImageSizeType), value.c_str());
        if (sizeTypeIter != -1) {
            bgImgSize.SetSizeTypeX(bgImageSizeType[sizeTypeIter].value);
            bgImgSize.SetSizeTypeY(bgImageSizeType[sizeTypeIter].value);
        } else if (value.find("px") != std::string::npos) {
            SetBgImgSizeX(BackgroundImageSizeType::LENGTH, StringToDouble(value), bgImgSize);
            bgImgSize.SetSizeTypeY(BackgroundImageSizeType::AUTO);
        } else if (value.find('%') != std::string::npos) {
            SetBgImgSizeX(BackgroundImageSizeType::PERCENT, StringToDouble(value), bgImgSize);
            bgImgSize.SetSizeTypeY(BackgroundImageSizeType::AUTO);
        } else {
            bgImgSize.SetSizeTypeX(BackgroundImageSizeType::AUTO);
            bgImgSize.SetSizeTypeY(BackgroundImageSizeType::AUTO);
        }
    }
    backgroundStyle.backgroundImage->SetImageSize(
        bgImgSize.GetSizeTypeX(), bgImgSize.GetSizeValueX(), bgImgSize.GetSizeTypeY(), bgImgSize.GetSizeValueY());
    declaration.hasDecorationStyle_ = true;
}

void SetBgImgPositionX(
    const BackgroundImagePositionType type, const double value, BackgroundImagePosition& bgImgPosition)
{
    bgImgPosition.SetSizeTypeX(type);
    bgImgPosition.SetSizeValueX(value);
}

void SetBgImgPositionY(
    const BackgroundImagePositionType type, const double value, BackgroundImagePosition& bgImgPosition)
{
    bgImgPosition.SetSizeTypeY(type);
    bgImgPosition.SetSizeValueY(value);
}

void SetBgImgPosition(
    const BackgroundImagePositionType type, const double value, BackgroundImagePosition& bgImgPosition)
{
    SetBgImgPositionX(type, value, bgImgPosition);
    SetBgImgPositionY(type, value, bgImgPosition);
}

bool BgImgPositionIsValid(const std::string& posX, const std::string& posY)
{
    if ((std::strcmp(posX.c_str(), DOM_BACKGROUND_IMAGE_POSITION_CENTER) == 0) ||
        (std::strcmp(posY.c_str(), DOM_BACKGROUND_IMAGE_POSITION_CENTER) == 0)) {
        return true;
    }

    static const std::unordered_set<std::string> horizonSet = {
        DOM_BACKGROUND_IMAGE_POSITION_LEFT,
        DOM_BACKGROUND_IMAGE_POSITION_RIGHT,
    };
    static const std::unordered_set<std::string> verticalSet = {
        DOM_BACKGROUND_IMAGE_POSITION_TOP,
        DOM_BACKGROUND_IMAGE_POSITION_BOTTOM,
    };

    // posX and posY are not strictly corresponding to horizontal or vertical, but they must not conflict,
    // for example both of them are "top" is invalid.
    if (posX.find("px") != std::string::npos || posX.find('%') != std::string::npos ||
        horizonSet.find(posX) != horizonSet.end()) {
        if (posY.find("px") != std::string::npos || posY.find('%') != std::string::npos ||
            verticalSet.find(posY) != verticalSet.end()) {
            return true;
        }
    }

    return verticalSet.find(posX) != verticalSet.end() && horizonSet.find(posY) != horizonSet.end();
}

void Declaration::SetBackgroundImagePosition(const std::string& value, Declaration& declaration)
{
    auto& backgroundStyle = declaration.MaybeResetStyle<CommonBackgroundStyle>(StyleTag::COMMON_BACKGROUND_STYLE);
    if (!backgroundStyle.IsValid()) {
        LOGD("don't support background");
        return;
    }

    static const LinearMapNode<void (*)(BackgroundImagePosition&)> backGroundPositionOperators[] = {
        { DOM_BACKGROUND_IMAGE_POSITION_BOTTOM,
            [](BackgroundImagePosition& backgroundImagePosition) {
                SetBgImgPositionY(BackgroundImagePositionType::PERCENT, 100.0, backgroundImagePosition);
            } },
        { DOM_BACKGROUND_IMAGE_POSITION_LEFT,
            [](BackgroundImagePosition& backgroundImagePosition) {
                SetBgImgPositionX(BackgroundImagePositionType::PERCENT, 0.0, backgroundImagePosition);
            } },
        { DOM_BACKGROUND_IMAGE_POSITION_RIGHT,
            [](BackgroundImagePosition& backgroundImagePosition) {
                SetBgImgPositionX(BackgroundImagePositionType::PERCENT, 100.0, backgroundImagePosition);
            } },
        { DOM_BACKGROUND_IMAGE_POSITION_TOP,
            [](BackgroundImagePosition& backgroundImagePosition) {
                SetBgImgPositionY(BackgroundImagePositionType::PERCENT, 0.0, backgroundImagePosition);
            } },
    };
    BackgroundImagePosition backgroundImagePosition;

    auto index = value.find(' ', 0);
    if (index != std::string::npos) {
        std::string valueX = value.substr(0, index);
        std::string valueY = value.substr(index + 1, value.size() - index - 1);
        if (!BgImgPositionIsValid(valueX, valueY)) {
            return;
        }
        // The input is valid,so set the default is (center,center),
        // if the value is different, the default value is overwritten.
        SetBgImgPosition(BackgroundImagePositionType::PERCENT, 50.0, backgroundImagePosition);
        if (valueX.find("px") != std::string::npos) {
            SetBgImgPositionX(BackgroundImagePositionType::PX, StringToDouble(valueX), backgroundImagePosition);
        } else if (valueX.find('%') != std::string::npos) {
            SetBgImgPositionX(BackgroundImagePositionType::PERCENT, StringToDouble(valueX), backgroundImagePosition);
        } else {
            auto operatorIterX = BinarySearchFindIndex(
                backGroundPositionOperators, ArraySize(backGroundPositionOperators), valueX.c_str());
            if (operatorIterX != -1) {
                backGroundPositionOperators[operatorIterX].value(backgroundImagePosition);
            }
        }
        if (valueY.find("px") != std::string::npos) {
            SetBgImgPositionY(BackgroundImagePositionType::PX, StringToDouble(valueY), backgroundImagePosition);
        } else if (valueY.find('%') != std::string::npos) {
            SetBgImgPositionY(BackgroundImagePositionType::PERCENT, StringToDouble(valueY), backgroundImagePosition);
        } else {
            auto operatorIterY = BinarySearchFindIndex(
                backGroundPositionOperators, ArraySize(backGroundPositionOperators), valueY.c_str());
            if (operatorIterY != -1) {
                backGroundPositionOperators[operatorIterY].value(backgroundImagePosition);
            }
        }
    } else {
        SetBgImgPosition(BackgroundImagePositionType::PERCENT, 50.0, backgroundImagePosition);
        if (value.find("px") != std::string::npos) {
            SetBgImgPositionX(BackgroundImagePositionType::PX, StringToDouble(value), backgroundImagePosition);
        } else if (value.find('%') != std::string::npos) {
            SetBgImgPositionX(BackgroundImagePositionType::PERCENT, StringToDouble(value), backgroundImagePosition);
        } else {
            auto operatorIter = BinarySearchFindIndex(
                backGroundPositionOperators, ArraySize(backGroundPositionOperators), value.c_str());
            if (operatorIter != -1) {
                backGroundPositionOperators[operatorIter].value(backgroundImagePosition);
            }
        }
    }
    backgroundStyle.backgroundImage->SetImagePosition(backgroundImagePosition.GetSizeTypeX(),
        backgroundImagePosition.GetSizeValueX(), backgroundImagePosition.GetSizeTypeY(),
        backgroundImagePosition.GetSizeValueY());
    declaration.hasDecorationStyle_ = true;
}

void Declaration::BindPipelineContext(const WeakPtr<PipelineContext>& pipelineContext)
{
    pipelineContext_ = pipelineContext;
}

void Declaration::ResetDefaultStyles()
{
    auto& sizeStyle = static_cast<CommonSizeStyle&>(GetStyle(StyleTag::COMMON_SIZE_STYLE));
    if (sizeStyle.IsValid() && !sizeStyle.IsShared()) {
        sizeStyle.width = Dimension(-1.0, DimensionUnit::PX);
        sizeStyle.height = Dimension(-1.0, DimensionUnit::PX);
        sizeStyle.minWidth = Dimension(0.0);
        sizeStyle.minHeight = Dimension(0.0);
        sizeStyle.maxWidth = Dimension(Size::INFINITE_SIZE);
        sizeStyle.maxHeight = Dimension(Size::INFINITE_SIZE);
        sizeStyle.aspectRatio = -1.0;
    }

    auto& paddingStyle = static_cast<CommonPaddingStyle&>(GetStyle(StyleTag::COMMON_PADDING_STYLE));
    if (paddingStyle.IsValid() && !paddingStyle.IsShared()) {
        paddingStyle.padding = Edge(Dimension(0.0));
    }

    auto& marginStyle = static_cast<CommonMarginStyle&>(GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    if (marginStyle.IsValid() && !marginStyle.IsShared()) {
        marginStyle.margin = Edge(Dimension(0.0));
    }

    auto& flexStyle = static_cast<CommonFlexStyle&>(GetStyle(StyleTag::COMMON_FLEX_STYLE));
    if (flexStyle.IsValid() && !flexStyle.IsShared()) {
        flexStyle.flexGrow = 0.0;
        flexStyle.flexShrink = 1.0;
        flexStyle.flexBasis = 0.0_px;
        flexStyle.flexWeight = 0.0;
        flexStyle.displayIndex = 1;
    }

    auto& opacityStyle = static_cast<CommonOpacityStyle&>(GetStyle(StyleTag::COMMON_OPACITY_STYLE));
    if (opacityStyle.IsValid() && !opacityStyle.IsShared()) {
        opacityStyle.opacity = 1.0;
    }

    auto& displayStyle = static_cast<CommonDisplayStyle&>(GetStyle(StyleTag::COMMON_DISPLAY_STYLE));
    if (displayStyle.IsValid() && !displayStyle.IsShared()) {
        displayStyle.display = DisplayType::NO_SETTING;
    }
    hasDisplayStyle_ = false;

    auto& visibilityStyle = static_cast<CommonVisibilityStyle&>(GetStyle(StyleTag::COMMON_VISIBILITY_STYLE));
    if (visibilityStyle.IsValid() && !visibilityStyle.IsShared()) {
        visibilityStyle.visibility = VisibilityType::NO_SETTING;
    }

    auto& borderStyle = static_cast<CommonBorderStyle&>(GetStyle(StyleTag::COMMON_BORDER_STYLE));
    if (borderStyle.IsValid() && !borderStyle.IsShared()) {
        borderStyle.border.SetBorderEdge(BorderEdge(Color::BLACK, Dimension(), BorderStyle::SOLID));
    }

    auto& borderImageStyle = static_cast<CommonBorderStyle&>(GetStyle(StyleTag::COMMON_BORDER_STYLE));
    if (borderImageStyle.IsValid() && !borderImageStyle.IsShared()) {
        borderImageStyle.border.SetBorderImageEdge(
            BorderImageEdge(Dimension(), Dimension(), Dimension(), BorderImageRepeat::STRETCH));
    }

    auto& background = static_cast<CommonBackgroundStyle&>(GetStyle(StyleTag::COMMON_BACKGROUND_STYLE));
    if (background.IsValid() && !background.IsShared()) {
        background.gradient = Gradient();
        background.gradientBorderImage = Gradient();
        background.backgroundImage = AceType::MakeRefPtr<BackgroundImage>();
        background.borderImage = AceType::MakeRefPtr<BorderImage>();
    }

    auto& renderAttr = static_cast<CommonRenderAttribute&>(GetAttribute(AttributeTag::COMMON_RENDER_ATTR));
    if (renderAttr.IsValid() && !renderAttr.IsShared() && !renderAttr.show.empty()) {
        hasDisplayStyle_ = true;
        SetShowAttr(renderAttr.show);
    }

    backDecoration_ = AceType::MakeRefPtr<Decoration>();
    frontDecoration_ = AceType::MakeRefPtr<Decoration>();
}

// Convert transform style to json format, such as rotate(50deg) to {"ratate":"50deg"}
std::string Declaration::GetTransformJsonValue(const std::string& value)
{
    auto rightIndex = value.find('(');
    auto leftIndex = value.find(')');
    std::string jsonValue = value;

    if (rightIndex != std::string::npos && leftIndex != std::string::npos && (leftIndex - 1 - rightIndex > 0)) {
        std::string transformType = value.substr(0, rightIndex);
        std::string transformValue = value.substr(rightIndex + 1, leftIndex - 1 - rightIndex);
        jsonValue = "{\"" + transformType + "\":\"" + transformValue + "\"}";
    }

    return jsonValue;
}

std::string Declaration::GetTransformType(const std::unique_ptr<JsonValue>& transformJson)
{
    if (transformJson->IsNull()) {
        LOGE("transformJson is null");
        return "";
    }
    return transformJson->GetKey();
}

std::string Declaration::GetTransformTypeValue(const std::unique_ptr<JsonValue>& transformJson)
{
    if (transformJson->IsNull()) {
        LOGE("transformJson is null");
        return "";
    }
    std::string jsonValue = transformJson->GetString();
    if (jsonValue.empty()) {
        double jsonDouble = transformJson->GetDouble();
        return std::to_string(jsonDouble);
    }
    return jsonValue;
}

RefPtr<ThemeManager> Declaration::GetThemeManager() const
{
    auto context = pipelineContext_.Upgrade();
    if (!context) {
        return nullptr;
    }
    return context->GetThemeManager();
}

RefPtr<ThemeConstants> Declaration::GetThemeConstants() const
{
    auto themeManager = GetThemeManager();
    if (!themeManager) {
        return nullptr;
    }
    return themeManager->GetThemeConstants();
}

Color Declaration::ParseColor(const std::string& value, uint32_t maskAlpha) const
{
    auto themeConstants = GetThemeConstants();
    auto&& noRefFunc = [&value, maskAlpha = maskAlpha]() { return Color::FromString(value, maskAlpha); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetColor(refId); };
    return ParseThemeReference<Color>(value, noRefFunc, idRefFunc, Color::TRANSPARENT);
}

double Declaration::ParseDouble(const std::string& value) const
{
    auto themeConstants = GetThemeConstants();
    auto&& noRefFunc = [&value]() { return StringUtils::StringToDouble(value); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetDouble(refId); };
    return ParseThemeReference<double>(value, noRefFunc, idRefFunc, 0.0);
}

Dimension Declaration::ParseDimension(const std::string& value, bool useVp) const
{
    auto themeConstants = GetThemeConstants();
    auto&& noRefFunc = [&value, useVp]() { return StringUtils::StringToDimension(value, useVp); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetDimension(refId); };
    return ParseThemeReference<Dimension>(value, noRefFunc, idRefFunc, Dimension());
}

CalcDimension Declaration::ParseCalcDimension(const std::string& value, bool useVp) const
{
    if (value.find("calc") != std::string::npos) {
        return StringUtils::StringToCalcDimension(value, useVp);
    } else {
        return ParseDimension(value, useVp);
    }
}

Dimension Declaration::ParseLineHeight(const std::string& value) const
{
    auto themeConstants = GetThemeConstants();
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(value, themeConstants);
    if (!parseResult.parseSuccess) {
        return StringUtils::StringToDimension(value);
    }
    auto&& noRefFunc = [&value]() { return StringUtils::StringToDouble(value); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetDouble(refId); };
    auto lineHeightScale = ParseThemeReference<double>(value, noRefFunc, idRefFunc, 1.0);
    // If got 0.0 from ThemeConstants, use default 1.0
    lineHeightScale = NearZero(lineHeightScale) ? 1.0 : lineHeightScale;
    return Dimension(lineHeightScale, DimensionUnit::PERCENT);
}

std::vector<std::string> Declaration::ParseFontFamilies(const std::string& value) const
{
    std::vector<std::string> fontFamilies;
    std::stringstream stream(value);
    std::string fontFamily;

    auto themeConstants = GetThemeConstants();
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetString(refId); };

    while (getline(stream, fontFamily, ',')) {
        auto&& noRefFunc = [&fontFamily]() { return fontFamily; };
        fontFamilies.emplace_back(ParseThemeReference<std::string>(fontFamily, noRefFunc, idRefFunc, fontFamily));
    }
    return fontFamilies;
}

std::vector<Dimension> Declaration::ParsePreferFontSizes(const std::string& value) const
{
    std::vector<Dimension> prefers;
    std::stringstream stream(value);
    std::string fontSize;
    while (getline(stream, fontSize, ',')) {
        prefers.emplace_back(ParseDimension(fontSize));
    }
    std::sort(prefers.begin(), prefers.end(),
        [](const Dimension& left, const Dimension& right) { return left.Value() > right.Value(); });
    return prefers;
}

std::string Declaration::ParseImageSrc(const std::string& imgSrc) const
{
    return ThemeUtils::ProcessImageSource(imgSrc, GetThemeConstants());
}

bool Declaration::IsRightToLeft() const
{
    bool isRightToLeft = false;
    auto& commonAttr = static_cast<CommonAttribute&>(GetAttribute(AttributeTag::COMMON_ATTR));
    if (commonAttr.IsValid()) {
        isRightToLeft = commonAttr.isRightToLeft;
    }
    return isRightToLeft;
}

void Declaration::SetClickEvent(const EventMarker& onClick)
{
    auto& gestureEvent = MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_GESTURE_EVENT);
    if (gestureEvent.IsValid()) {
        gestureEvent.click.eventMarker = onClick;
        gestureEvent.click.eventMarker.SetCatchMode(false);
        gestureEvent.click.isRefreshed = true;
    }
}

void Declaration::SetRemoteMessageEvent(const EventMarker& remoteMessage)
{
    LOGI("Declaration::SetRemoteMessageEvent");
    auto& gestureEvent = MaybeResetEvent<CommonGestureEvent>(EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT);
    if (gestureEvent.IsValid()) {
        LOGI("Declaration::SetRemoteMessageEvent IsValid");
        gestureEvent.click.eventMarker = remoteMessage;
        gestureEvent.click.eventMarker.SetCatchMode(false);
        gestureEvent.click.isRefreshed = true;
    }
}

} // namespace OHOS::Ace
