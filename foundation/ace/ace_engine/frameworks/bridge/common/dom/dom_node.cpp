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

#include "frameworks/bridge/common/dom/dom_node.h"

#include <sstream>
#include <unordered_set>

#include "base/log/ace_trace.h"
#include "core/animation/animatable_data.h"
#include "core/common/ace_application_info.h"
#include "core/common/frontend.h"
#include "core/components/declaration/common/declaration_creator_manager.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/video/texture_component.h"
#include "core/components/web/web_component.h"
#include "core/components/xcomponent/xcomponent_component.h"
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t TRANSFORM_SINGLE = 1;
constexpr uint32_t TRANSFORM_DUAL = 2;
constexpr uint32_t TRANSFORM_SIX = 6;
constexpr uint32_t TRANSFORM_SIXTEEN = 16;
constexpr uint32_t COMMON_METHOD_FOCUS_ARGS_SIZE = 1;
constexpr uint32_t THREE_VALUES = 3;
constexpr uint32_t FOUR_VALUES = 4;
constexpr int32_t MS_TO_S = 1000;
const char COMMON_METHOD_FOCUS[] = "focus";

// focusable support list, this list should be updated after the other nodes(checkbox/radio/switch/
// grid/dialog/button/input) implemented in frontend.
std::unordered_set<std::string> g_focusableNode;

// unsupported focusable list.
const std::unordered_set<std::string> UNFOCUSABLED_NODE = {
    DOM_NODE_TAG_SPAN,
};

static const std::unordered_set<std::string> TRANSITION_PROPERTIES = {
    DOM_WIDTH,
    DOM_HEIGHT,
    DOM_PADDING_TOP,
    DOM_PADDING_RIGHT,
    DOM_PADDING_BOTTOM,
    DOM_PADDING_LEFT,
    DOM_MARGIN_TOP,
    DOM_MARGIN_RIGHT,
    DOM_MARGIN_BOTTOM,
    DOM_MARGIN_LEFT,
    DOM_BACKGROUND_COLOR,
    DOM_OPACITY,
    DOM_BORDER_LEFT_WIDTH,
    DOM_BORDER_TOP_WIDTH,
    DOM_BORDER_RIGHT_WIDTH,
    DOM_BORDER_BOTTOM_WIDTH,
    DOM_BORDER_LEFT_COLOR,
    DOM_BORDER_TOP_COLOR,
    DOM_BORDER_RIGHT_COLOR,
    DOM_BORDER_BOTTOM_COLOR
};

// default flex value
constexpr double DEFAULT_FLEX_GROW = 0.0;
constexpr double DEFAULT_FLEX_SHRINK = 1.0;
constexpr double DEFAULT_FLEX_BASIS = -1.0;
constexpr int32_t TRANSITION_NAME_LENGTH = 4;

// prefix id of TweenComponent, for differentiation from id of ComposedComponent
constexpr char COMPONENT_PREFIX[] = "FrontendTween";
constexpr char TRANSITION_COMPONENT_PREFIX[] = "FrontendTransition";

} // namespace

DOMNode::DOMNode(NodeId nodeId, const std::string& nodeName) : nodeId_(nodeId), tag_(nodeName)
{
    rootComponent_ = AceType::MakeRefPtr<ComposedComponent>(std::to_string(nodeId), nodeName);
    boxComponent_ = AceType::MakeRefPtr<BoxComponent>();
    CreateDeclaration(nodeName);
}

DOMNode::~DOMNode()
{
    if (declaration_) {
        auto& focusEvent = static_cast<CommonFocusEvent&>(declaration_->GetEvent(EventTag::COMMON_FOCUS_EVENT));
        if (focusEvent.IsValid()) {
            if (!focusEvent.focus.eventMarker.IsEmpty()) {
                BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(focusEvent.focus.eventMarker);
            }
            if (!focusEvent.blur.eventMarker.IsEmpty()) {
                BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(focusEvent.blur.eventMarker);
            }
        }
        auto& mouseEvent = static_cast<CommonMouseEvent&>(declaration_->GetEvent(EventTag::COMMON_MOUSE_EVENT));
        if (mouseEvent.IsValid()) {
            if (!mouseEvent.mouseHover.eventMarker.IsEmpty()) {
                BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(mouseEvent.mouseHover.eventMarker);
            }
            if (!mouseEvent.mouseHoverExit.eventMarker.IsEmpty()) {
                BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(mouseEvent.mouseHoverExit.eventMarker);
            }
        }

        for (uint32_t eventAction = 0; eventAction < EventAction::SIZE; eventAction++) {
            for (uint32_t eventStage = 0; eventStage < EventStage::SIZE; eventStage++) {
                for (uint32_t touchEventType = 0; touchEventType < EventType::SIZE; touchEventType++) {
                    const auto& eventMarker = GetTouchId(eventAction, eventStage, touchEventType);
                    if (!eventMarker.IsEmpty()) {
                        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(eventMarker);
                    }
                }
            }
        }
    }
}

void DOMNode::Mount(int32_t slot)
{
    auto parentNode = parentNode_.Upgrade();
    if (!parentNode) {
        return;
    }
    // flex-item could be used in div, list-item, tabs
    static const std::unordered_set<std::string> flexItemParents = {
        DOM_NODE_TAG_DIV,
        DOM_NODE_TAG_GRID_COLUMN,
        DOM_NODE_TAG_LIST_ITEM,
        DOM_NODE_TAG_LIST_ITEM_GROUP,
        DOM_NODE_TAG_TABS,
        DOM_NODE_TAG_REFRESH,
        DOM_NODE_TAG_STEPPER_ITEM,
    };
    if (flexItemParents.count(parentTag_) > 0) {
        flexItemComponent_ =
            AceType::MakeRefPtr<FlexItemComponent>(DEFAULT_FLEX_GROW, DEFAULT_FLEX_SHRINK, DEFAULT_FLEX_BASIS);
        if (boxComponent_) {
            // span has no box component.
            boxComponent_->SetDeliverMinToChild(false);
        }
    }
    GenerateComponentNode();
    if (parentNode->IsRootNode()) {
        // When parent is root node, parent contains scroll component,
        // box could handle percent with viewport size.
        if (boxComponent_) {
            // span has no box component.
            boxComponent_->SetPercentFlag(PERCENT_FLAG_USE_VIEW_PORT);
        }
    }
    parentNode->AddNode(AceType::Claim(this), slot);
    OnMounted(parentNode);
}

void DOMNode::AddEvent(int32_t pageId, const std::vector<std::string>& events)
{
    std::vector<std::string> tempEvents;
    for (const auto& event : events) {
        if (!AddSpecializedEvent(pageId, event)) {
            tempEvents.emplace_back(event);
        }
    }
    if (declaration_) {
        declaration_->AddEvent(pageId, GetNodeIdForEvent(), tempEvents);
    }
}

void DOMNode::SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs)
{
    std::vector<std::pair<std::string, std::string>> tempAttrs;
    for (const auto& attr : attrs) {
        if (SetSpecializedAttr(attr)) {
            continue;
        } else {
            tempAttrs.emplace_back(attr);
        }
    }
    if (declaration_) {
        IsSubscriptEnable();
        declaration_->SetAttr(tempAttrs);
    }
}

void DOMNode::CallMethod(const std::string& method, const std::string& args)
{
    if (method == COMMON_METHOD_FOCUS) {
        LOGD("CallMethod: node tag: %{public}s call focus method.", tag_.c_str());
        if (!focusableEventComponent_) {
            LOGE("CallMethod: call focus method failed, focusableEventComponent is null");
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

void DOMNode::OnRequestFocus(bool shouldFocus)
{
    auto controller = focusableEventComponent_->GetFocusableController();
    if (!controller) {
        return;
    }
    controller->RequestFocus(shouldFocus);
}

void DOMNode::OnScrollBy(double dx, double dy, bool isSmooth)
{
    auto scrollComponent = GetScrollComponent();
    if (!scrollComponent) {
        return;
    }
    auto positionController = scrollComponent->GetScrollPositionController();
    if (!positionController) {
        return;
    }
    positionController->ScrollBy(dx, dy, isSmooth);
}

void DOMNode::SetShowAttr(const std::string& showValue)
{
    if (!declaration_) {
        return;
    }

    auto& renderAttr = declaration_->MaybeResetAttribute<CommonRenderAttribute>(AttributeTag::COMMON_RENDER_ATTR);
    if (renderAttr.IsValid()) {
        renderAttr.show = showValue;
    }
    if (showValue == "false") {
        SetDisplay(DisplayType::NONE);
    } else {
        SetDisplay(DisplayType::NO_SETTING);
    }
}

bool DOMNode::ParseTransitionPropertyStyle(const std::string& transitionProperty)
{
    if (TRANSITION_PROPERTIES.find(transitionProperty) == TRANSITION_PROPERTIES.end()) {
        return false;
    }
    transitionPropertyName_ = transitionProperty;
    CreatePropertyAnimation(transitionPropertyName_);
    return true;
}

bool DOMNode::ParseTransitionNameStyle(const std::string& transitionName)
{
    std::vector<std::string> transitions;
    StringUtils::StringSpliter(transitionName, ' ', transitions);
    if (transitions.size() != TRANSITION_NAME_LENGTH) {
        LOGE("transition length is invalid");
        return false;
    }
    transitionPropertyName_ = transitions[0];
    transitionDuration_ = StringUtils::StringToDouble(transitions[1]) * MS_TO_S;
    transitionTimeFunction_ = transitions[2];
    transitionDelay_ = StringUtils::StringToDouble(transitions[3]) * MS_TO_S;

    CreatePropertyAnimation(transitionPropertyName_);
    return true;
}

void DOMNode::CreatePropertyAnimation(const std::string& property)
{
    propertyWidthAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    propertyHeightAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    propertyColorAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    static const std::unordered_map<std::string, void (*)(DOMNode&)> propertyAnimationMap = {
        { DOM_WIDTH, [](DOMNode& node) {
            float widthBegin = 0.0f;
            float widthEnd = node.GetWidth().Value();
            node.propertyWidthAnimation_ = node.SetPropertyFloatAnimationKeyframe(widthBegin, widthEnd);
        } },
        { DOM_HEIGHT, [](DOMNode& node) {
            float heightBegin = 0.0f;
            float heightEnd = node.GetHeight().Value();
            node.propertyHeightAnimation_ = node.SetPropertyFloatAnimationKeyframe(heightBegin, heightEnd);
        } },
        { DOM_TRANSITION_BACKGROUND_COLOR, [](DOMNode& node) {
            Color backgroundColorBegin = Color::TRANSPARENT;
            Color backgroundColorEnd = node.GetBackGroundColor();
            node.propertyColorAnimation_ =
                    node.SetPropertyColorAnimationKeyframe(backgroundColorBegin, backgroundColorEnd);
        } },
    };

    auto animationIter = propertyAnimationMap.find(property);
    if (animationIter != propertyAnimationMap.end()) {
        animationIter->second(*this);
    }
    if (property == "all") {
        for (auto propertyAnimation : propertyAnimationMap) {
            propertyAnimation.second(*this);
        }
    }
}

RefPtr<KeyframeAnimation<float>> DOMNode::SetPropertyFloatAnimationKeyframe(float begin, float end)
{
    auto propertyDimensionAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
    propertyDimensionAnimation->AddKeyframe(keyframeBegin);
    propertyDimensionAnimation->AddKeyframe(keyframeEnd);
    return propertyDimensionAnimation;
}

RefPtr<KeyframeAnimation<Color>> DOMNode::SetPropertyColorAnimationKeyframe(const Color& begin, const Color& end)
{
    auto propertyColorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, begin);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0f, end);
    propertyColorAnimation->AddKeyframe(keyframeBegin);
    propertyColorAnimation->AddKeyframe(keyframeEnd);
    return propertyColorAnimation;
}

void DOMNode::TransitionOptionSetKeyframes(TweenOption& transitionOption)
{
    transitionOption.SetDuration(transitionDuration_);
    transitionOption.SetFillMode(FillMode::FORWARDS);
    transitionOption.SetCurve(CreateCurve(transitionTimeFunction_));
    transitionOption.SetDelay(transitionDelay_);
    if (propertyWidthAnimation_ && !propertyWidthAnimation_->GetKeyframes().empty()) {
        transitionOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, propertyWidthAnimation_);
    }
    if (propertyHeightAnimation_ && !propertyHeightAnimation_->GetKeyframes().empty()) {
        transitionOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT,
            propertyHeightAnimation_);
    }
    if (propertyColorAnimation_ && !propertyColorAnimation_->GetKeyframes().empty()) {
        transitionOption.SetColorAnimation(propertyColorAnimation_);
    }
}

void DOMNode::SetStyle(const std::vector<std::pair<std::string, std::string>>& styles)
{
    std::vector<std::pair<std::string, std::string>> tempStyles;
    for (const auto& style : styles) {
        if ((style.first == DOM_TRANSITION_NAME) || (style.first == DOM_TRANSITION_PROPERTY_DURATION)) {
            transitionStyleUpdated_ = true;
        }
        if (style.first == DOM_BACKGROUND_COLOR) {
            isTransitionColor_ = true;
        }
        CachePseudoClassStyle(style);
        if (style.first.find(DOM_PSEUDO_CLASS_SYMBOL) == std::string::npos) {
            if (!SetCurrentStyle(style)) {
                tempStyles.emplace_back(style);
            }
        } else {
            tempStyles.emplace_back(style);
        }
    }
    if (declaration_) {
        declaration_->SetStyle(tempStyles);
    }
    OnSetStyleFinished();
}

bool DOMNode::SetCurrentStyle(const std::pair<std::string, std::string>& style)
{
    if (SetSpecializedStyle(style)) {
        // If the subclass consumes this property, it will no longer look in the general property.
        return true;
    }
    // Operator map for styles
    static const std::unordered_map<std::string, void (*)(const std::string&, DOMNode&)> styleOperators = {
        // Set width and height
        { DOM_TRANSFORM, &DOMNode::SetTransform },
        { DOM_TRANSITION_PROPERTY,
            [](const std::string& val, DOMNode& node) {
                node.ParseTransitionPropertyStyle(val);
            } },
        { DOM_TRANSITION_PROPERTY_DURATION,
            [](const std::string& val, DOMNode& node) {
                node.transitionDuration_ = StringUtils::StringToInt(val) * MS_TO_S;
            } },
        { DOM_TRANSITION_PROPERTY_TIMING_FUNCTION,
            [](const std::string& val, DOMNode& node) {
                node.transitionTimeFunction_ = val;
            } },
        { DOM_TRANSITION_PROPERTY_DELAY,
            [](const std::string& val, DOMNode& node) {
                node.transitionDelay_ = StringUtils::StringToInt(val) * MS_TO_S;
            } },
    };
    auto operatorIter = styleOperators.find(style.first);
    if (operatorIter != styleOperators.end()) {
        operatorIter->second(style.second, *this);
        return true;
    }
    return false;
}

void DOMNode::OnActive(bool isActive)
{
    isActive_ = isActive;
    UpdatePseudoStyle(true);
}

void DOMNode::OnFocus(bool isFocus)
{
    isFocus_ = isFocus;
    UpdatePseudoStyle(true);
}

void DOMNode::OnChecked(bool isChecked)
{
    if (declaration_) {
        declaration_->SetIsChecked(isChecked);
    }
    UpdatePseudoStyle(true);
}

void DOMNode::OnHover(bool isHover)
{
    if (declaration_) {
        declaration_->SetIsHover(isHover);
    }
    UpdatePseudoStyle(true);
}

void DOMNode::MarkNeedUpdate()
{
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext_ is nullptr");
        return;
    }
    rootComponent_->MarkNeedUpdate();
    rootComponent_->SetUpdateType(UpdateType::ALL);
    pipelineContext->ScheduleUpdate(rootComponent_);
}

void DOMNode::SetOnFocusClick(const EventMarker& eventMarker)
{
    if (!focusableEventComponent_) {
        return;
    }
    focusableEventComponent_->SetOnClickId(eventMarker);
}

bool DOMNode::IsNodeDisabled() const
{
    return declaration_ ? declaration_->IsDisabled() : false;
}

void DOMNode::ResetDefaultStyles()
{
    if (declaration_) {
        declaration_->ResetDefaultStyles();
    }

    displayComponent_.Reset();
    if (transformComponent_) {
        transformComponent_->ResetTransform();
    }
}

void DOMNode::UpdatePseudoStyleByStatus(int32_t status, bool isBackendChange)
{
    std::unordered_map<std::string, std::string> matchedStyleMap;
    do {
        if (pseudoClassStyleMap_.empty()) {
            break;
        }
        // first:use status as the key, search the complete matched;
        // second:if first do not matched any, calculate the max value of key & status, select the max value one.
        auto matchedStyle = pseudoClassStyleMap_.find(status);
        if (matchedStyle != pseudoClassStyleMap_.end()) {
            matchedStyleMap = matchedStyle->second;
        } else {
            uint32_t maxAndResult = 0;
            uint32_t maxValueKey = pseudoClassStyleMap_.begin()->first;
            // status & map key, select the max result one
            for (const auto& pseudoClass : pseudoClassStyleMap_) {
                uint32_t key = pseudoClass.first;
                uint32_t andResult = key & static_cast<uint32_t>(status);
                if (andResult > maxAndResult) {
                    maxAndResult = andResult;
                    maxValueKey = key;
                }
            }
            // if matched style except none pseudo style
            auto iter = pseudoClassStyleMap_.find(maxValueKey);
            if (maxAndResult > 0 && iter != pseudoClassStyleMap_.end()) {
                matchedStyleMap = iter->second;
            }
        }
    } while (0);

    ResetDefaultStyles();
    ResetInitializedStyle();

    // if not none pseudo style, need set none pseudo style first.
    auto nonePseudoStylesIter = pseudoClassStyleMap_.find(STATE_NORMAL);
    if (status != STATE_NORMAL && nonePseudoStylesIter != pseudoClassStyleMap_.end()) {
        for (const auto& noneTypeStyle : nonePseudoStylesIter->second) {
            if (!SetCurrentStyle(noneTypeStyle)) {
                declaration_->SetCurrentStyle(noneTypeStyle);
            }
        }
    }
    for (const auto& style : matchedStyleMap) {
        if (!SetCurrentStyle(style)) {
            declaration_->SetCurrentStyle(style);
        }
    }

    // do not move this for block into the up one
    for (const auto& style : matchedStyleMap) {
        if (style.first == DOM_TRANSITION_NAME) {
            if (isTransitionNameUpdateFirst_) {
                isTransitionNameUpdateFirst_ = false;
            } else {
                ParseTransitionNameStyle(style.second);
                propTransitionOption_ = TweenOption();
                TransitionOptionSetKeyframes(propTransitionOption_);
                transitionStyleUpdated_ = true;
            }
        }
        if (style.first == DOM_TRANSITION_PROPERTY_DURATION) {
            if (isTransitionDurationUpdateFirst_) {
                isTransitionDurationUpdateFirst_ = false;
            } else {
                CreatePropertyAnimation(transitionPropertyName_);
                propTransitionOption_ = TweenOption();
                TransitionOptionSetKeyframes(propTransitionOption_);
                transitionStyleUpdated_ = true;
            }
        }
    }
    if (isBackendChange) {
        auto pipelineContext = pipelineContext_.Upgrade();
        if (!pipelineContext) {
            LOGE("pipelineContext_ is nullptr");
            return;
        }
        if (GetTag() != DOM_NODE_TAG_SPAN) {
            UpdateUiComponents();
        }
        UpdateSpecializedComponentWithDeclaration();
        PrepareSpecializedComponent();
        CompositeComponents();
        rootComponent_->MarkNeedUpdate();
        rootComponent_->SetUpdateType(UpdateType::STYLE);
        pipelineContext->ScheduleUpdate(rootComponent_);
    }
}

uint32_t DOMNode::CalculatePseudoStatus() const
{
    uint32_t status = STATE_NORMAL;
    if (!declaration_) {
        return STATE_NORMAL;
    }
    if (isActive_) {
        status |= STATE_ACTIVE;
    }
    if (declaration_->IsDisabled()) {
        status |= STATE_DISABLED;
    }
    if (isFocus_) {
        status |= STATE_FOCUS;
    }
    if (declaration_->IsChecked()) {
        status |= STATE_CHECKED;
    }
    if (declaration_->IsWaiting()) {
        status |= STATE_WAITING;
    }
    if (declaration_->IsHover()) {
        status |= STATE_HOVERED;
    }
    return status;
}

void DOMNode::UpdateStyleWithChildren()
{
    auto status = CalculatePseudoStatus();
    UpdatePseudoStyleByStatus(status, true);

    for (const auto& child : children_) {
        if (child) {
            child->UpdateStyleWithChildren();
        }
    }
}

void DOMNode::UpdatePseudoStyle(bool isBackendChange)
{
    if (!HasPseudo()) {
        return;
    }
    auto status = CalculatePseudoStatus();
    LOGD("UpdatePseudoStyle status is:%{public}d, isBackendChange:%{public}d", status, isBackendChange);
    if (!isBackendChange) {
        UpdatePseudoStyleByStatus(status, false);
        return;
    }
    // Triggered by backend, elements may processing build or layout now. So post a new task to UI thread.
    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        LOGE("Context is null!");
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = AceType::WeakClaim(this), status = status]() {
            auto node = weak.Upgrade();
            if (!node) {
                return;
            }
            node->UpdatePseudoStyleByStatus(status, true);
        },
        TaskExecutor::TaskType::UI);
}

void DOMNode::CachePseudoClassStyle(const std::pair<std::string, std::string>& pseudoClassStyle)
{
    uint32_t pseudoClassType = STATE_NORMAL;
    const auto& styleKey = pseudoClassStyle.first;

    if (styleKey.find(DOM_ACTIVE_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_ACTIVE;
    }

    if (styleKey.find(DOM_DISABLED_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_DISABLED;
    }

    if (styleKey.find(DOM_FOCUS_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_FOCUS;
    }

    if (styleKey.find(DOM_CHECKED_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_CHECKED;
    }

    if (styleKey.find(DOM_WAITING_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_WAITING;
    }

    if (styleKey.find(DOM_HOVER_PSEUDO_CLASS) != std::string::npos) {
        pseudoClassType |= STATE_HOVERED;
    }

    cachedPseudoType_ |= pseudoClassType;

    auto pseudoSymbolLocation = styleKey.find(DOM_PSEUDO_CLASS_SYMBOL);
    auto dealedStyleKey =
        ((pseudoSymbolLocation != std::string::npos) ? styleKey.substr(0, pseudoSymbolLocation) : styleKey);

    auto styleMapIter = pseudoClassStyleMap_.find(pseudoClassType);

    if (styleMapIter != pseudoClassStyleMap_.end()) {
        auto result = styleMapIter->second.try_emplace(dealedStyleKey, pseudoClassStyle.second);
        if (!result.second) {
            result.first->second = pseudoClassStyle.second;
        }
        return;
    }
    std::unordered_map<std::string, std::string> newPseudoMap;
    newPseudoMap.emplace(dealedStyleKey, pseudoClassStyle.second);
    pseudoClassStyleMap_.emplace(pseudoClassType, newPseudoMap);
}

void DOMNode::GenerateComponentNode()
{
    UpdatePseudoStyle(false);
    if (GetTag() != DOM_NODE_TAG_SPAN) {
        UpdateUiComponents();
        UpdateTouchEventComponent();
        UpdateGestureEventComponent();
        UpdateMouseEventComponent();
        UpdateFocusableEventComponents();
        // Prepare for fixed position
        UpdatePositionComponent();
#ifndef WEARABLE_PRODUCT
        UpdateMultimodalComponent();
        PrepareScrollComponent();
#endif
    }
    UpdateSpecializedComponentWithDeclaration();
    PrepareSpecializedComponent();
    CompositeComponents();
    // Relative and absolute position needs to update the top component props.
    UpdatePositionProps();
    auto rootChild = AceType::DynamicCast<RenderComponent>(rootComponent_->GetChild());
    if (isCustomComponent_) {
        if (rootChild) {
            rootChild->SetIsCustomComponent(isCustomComponent_);
            rootChild->SetOnLayoutReadyMarker(EventMarker(GetNodeIdForEvent(), ""));
        }
    }
    if (rootChild) {
        int32_t zIndex = 0;
        if (declaration_) {
            auto& commonStyle = static_cast<CommonStyle&>(declaration_->GetStyle(StyleTag::COMMON_STYLE));
            if (commonStyle.IsValid()) {
                zIndex = commonStyle.zIndex;
            }
        }
        rootChild->SetZIndex(zIndex);
    }
    rootComponent_->MarkNeedUpdate();
}

void DOMNode::AddNode(const RefPtr<DOMNode>& node, int32_t slot)
{
    if (!node) {
        return;
    }
    auto isExist = std::find_if(children_.begin(), children_.end(),
        [node](const RefPtr<DOMNode>& child) { return child->GetNodeId() == node->GetNodeId(); });
    if (isExist != children_.end()) {
        LOGW("the node[%{public}d] has already in the children", node->GetNodeId());
        return;
    }
    auto pos = children_.begin();
    std::advance(pos, slot);
    children_.insert(pos, node);
    if (node->GetPosition() != PositionType::FIXED) {
        if (!node->IsProxy() && GetDisplay() == DisplayType::NONE) {
            node->GenerateComponentNode();
        }
        OnChildNodeAdded(node, slot);
    }
}

void DOMNode::RemoveNode(const RefPtr<DOMNode>& node)
{
    if (!node) {
        return;
    }
    children_.remove_if([node](const RefPtr<DOMNode>& child) { return node->GetNodeId() == child->GetNodeId(); });
    if (node->GetPosition() != PositionType::FIXED) {
        OnChildNodeRemoved(node);
    }
}

void DOMNode::SetDisplayStyle()
{
    switch (GetDisplay()) {
        case DisplayType::NONE:
            visible_ = VisibleType::GONE;
            break;
        case DisplayType::GRID:
        case DisplayType::FLEX:
        default:
            VisibilityType visibility = VisibilityType::NO_SETTING;
            if (declaration_) {
                auto& visibilityStyle =
                    static_cast<CommonVisibilityStyle&>(declaration_->GetStyle(StyleTag::COMMON_VISIBILITY_STYLE));
                if (visibilityStyle.IsValid()) {
                    visibility = visibilityStyle.visibility;
                }
            }
            visible_ = (visibility == VisibilityType::HIDDEN) ? VisibleType::INVISIBLE : VisibleType::VISIBLE;
            break;
    }
}

const RefPtr<PageTransitionComponent>& DOMNode::BuildTransitionComponent()
{
    transitionComponent_ = AceType::MakeRefPtr<PageTransitionComponent>();
    if (!declaration_) {
        return transitionComponent_;
    }

    transitionComponent_->SetTextDirection(declaration_->IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    auto& pageTransitionStyle =
        declaration_->MaybeResetStyle<CommonPageTransitionStyle>(StyleTag::COMMON_PAGE_TRANSITION_STYLE);
    if (!pageTransitionStyle.IsValid()) {
        return transitionComponent_;
    }
    if (pageTransitionStyle.transitionEnterOption.IsValid() || pageTransitionStyle.transitionExitOption.IsValid()) {
        if (!pageTransitionStyle.transitionEnterOption.GetCurve()) {
            // use FRICTION as transition default curve.
            pageTransitionStyle.transitionEnterOption.SetCurve(Curves::FRICTION);
            pageTransitionStyle.transitionExitOption.SetCurve(Curves::FRICTION);
        }
        if (SystemProperties::GetRosenBackendEnabled()) {
            pageTransitionStyle.transitionEnterOption.SetAllowRunningAsynchronously(true);
            pageTransitionStyle.transitionExitOption.SetAllowRunningAsynchronously(true);
        }
        transitionComponent_->SetContentTransitionOption(
            pageTransitionStyle.transitionEnterOption, pageTransitionStyle.transitionExitOption);
    }
    return transitionComponent_;
}

std::string DOMNode::GetTransformType(const std::unique_ptr<JsonValue>& transformJson)
{
    if (transformJson->IsNull()) {
        LOGE("transformJson is null");
        return "";
    }
    return transformJson->GetKey();
}

std::string DOMNode::GetTransformTypeValue(const std::unique_ptr<JsonValue>& transformJson)
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

void DOMNode::SetTransform(const std::string& value, DOMNode& node)
{
    if (!node.transformComponent_) {
        node.transformComponent_ = AceType::MakeRefPtr<TransformComponent>();
    }
    node.transformComponent_->ResetTransform(); // Avoid transfrom effect overlay.
    auto jsonValue = node.GetTransformJsonValue(value);
    std::unique_ptr<JsonValue> transformJson = JsonUtil::ParseJsonString(jsonValue);
    for (int32_t index = 0; index < transformJson->GetArraySize(); ++index) {
        std::string typeKey = node.GetTransformType(transformJson->GetArrayItem(index));
        std::string typeValue = node.GetTransformTypeValue(transformJson->GetArrayItem(index));
        if ((!typeKey.empty()) && (!typeValue.empty())) {
            // Operator map for transform
            static const std::unordered_map<std::string, void (*)(const std::string&, DOMNode&)> transformOperators = {
                { DOM_ROTATE,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->RotateZ(StringUtils::StringToDegree(typeValue));
                    } },
                { DOM_ROTATE_3D,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == FOUR_VALUES) {
                            auto dx = StringToDouble(offsets[0]);
                            auto dy = StringToDouble(offsets[1]);
                            auto dz = StringToDouble(offsets[2]);
                            auto degree = StringUtils::StringToDegree(offsets[3]);
                            node.transformComponent_->Rotate(dx, dy, dz, degree);
                        }
                    } },
                { DOM_ROTATE_X,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->RotateX(StringUtils::StringToDegree(typeValue));
                    } },
                { DOM_ROTATE_Y,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->RotateY(StringUtils::StringToDegree(typeValue));
                    } },
                { DOM_ROTATE_Z,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->RotateZ(StringUtils::StringToDegree(typeValue));
                    } },
                { SCALE,
                    [](const std::string& typeValue, DOMNode& node) {
                        if (typeValue.find(' ', 0) != std::string::npos) {
                            Offset offset = ConvertStrToOffset(typeValue);
                            node.transformComponent_->Scale(offset.GetX(), offset.GetY());
                        } else {
                            auto scaleValue = StringToDouble(typeValue);
                            node.transformComponent_->Scale(scaleValue, scaleValue);
                        }
                    } },
                { DOM_SCALE_3D,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == THREE_VALUES) {
                            auto scaleX = StringToDouble(offsets[0]);
                            auto scaleY = StringToDouble(offsets[1]);
                            auto scaleZ = StringToDouble(offsets[2]);
                            node.transformComponent_->Scale(scaleX, scaleY, scaleZ);
                        }
                    } },
                { DOM_SCALE_X, [](const std::string& typeValue,
                                   DOMNode& node) { node.transformComponent_->ScaleX(StringToDouble(typeValue)); } },
                { DOM_SCALE_Y, [](const std::string& typeValue,
                                   DOMNode& node) { node.transformComponent_->ScaleY(StringToDouble(typeValue)); } },
                { DOM_SCALE_Z, [](const std::string& typeValue,
                                   DOMNode& node) { node.transformComponent_->ScaleZ(StringToDouble(typeValue)); } },
                { DOM_TRANSLATE,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == TRANSFORM_DUAL) {
                            node.transformComponent_->Translate(
                                node.ParseDimension(offsets[0]), node.ParseDimension(offsets[1]));
                        } else if (offsets.size() == TRANSFORM_SINGLE) {
                            node.transformComponent_->TranslateX(node.ParseDimension(offsets[0]));
                        }
                    } },
                { DOM_TRANSLATE_3D,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == THREE_VALUES) {
                            auto dx = node.ParseDimension(offsets[0]);
                            auto dy = node.ParseDimension(offsets[1]);
                            auto dz = node.ParseDimension(offsets[2]);
                            node.transformComponent_->Translate(dx, dy, dz);
                        }
                    } },
                { DOM_TRANSLATE_X,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->TranslateX(node.ParseDimension(typeValue));
                    } },
                { DOM_TRANSLATE_Y,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->TranslateY(node.ParseDimension(typeValue));
                    } },
                { DOM_TRANSLATE_Z,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->TranslateZ(node.ParseDimension(typeValue));
                    } },
                { DOM_SKEW,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == TRANSFORM_DUAL) {
                            auto degreeX = StringUtils::StringToDegree(offsets[0]);
                            auto degreeY = StringUtils::StringToDegree(offsets[1]);
                            node.transformComponent_->Skew(degreeX, degreeY);
                        } else if (offsets.size() == TRANSFORM_SINGLE) {
                            auto degree = StringUtils::StringToDegree(typeValue);
                            node.transformComponent_->Skew(degree, 0.0f);
                        }
                    } },
                { DOM_SKEW_X,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->SkewX(StringUtils::StringToDegree(typeValue));
                    } },
                { DOM_SKEW_Y,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->SkewY(StringUtils::StringToDegree(typeValue));
                    } },
                { DOM_MATRIX,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == TRANSFORM_SIX) {
                            double scaleX = StringToDouble(offsets[0]);
                            double skewY = StringToDouble(offsets[1]);
                            double skewX = StringToDouble(offsets[2]);
                            double scaleY = StringToDouble(offsets[3]);
                            double translateX = StringToDouble(offsets[4]);
                            double translateY = StringToDouble(offsets[5]);
                            node.transformComponent_->Matrix(scaleX, skewY, skewX, scaleY, translateX, translateY);
                        }
                    } },
                { DOM_MATRIX_3D,
                    [](const std::string& typeValue, DOMNode& node) {
                        std::vector<std::string> offsets;
                        StringUtils::StringSpliter(typeValue, ' ', offsets);
                        if (offsets.size() == TRANSFORM_SIXTEEN) {
                            std::vector<double> matrix;
                            for (const auto& offset : offsets) {
                                matrix.push_back(StringToDouble(offset));
                            }
                            node.transformComponent_->Matrix3d(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4],
                                matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10], matrix[11],
                                matrix[12], matrix[13], matrix[14], matrix[15]);
                        }
                    } },
                { DOM_PERSPECTIVE,
                    [](const std::string& typeValue, DOMNode& node) {
                        node.transformComponent_->Perspective(node.ParseDimension(typeValue));
                    } },
            };

            auto operatorIter = transformOperators.find(typeKey);
            if (operatorIter != transformOperators.end()) {
                operatorIter->second(typeValue, node);
            }
        }
    }
}

// Convert transform style to json format, such as rotate(50deg) to {"ratate":"50deg"}
std::string DOMNode::GetTransformJsonValue(const std::string& value)
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

void DOMNode::AddKeyframe(double time, double typeValue, RefPtr<KeyframeAnimation<float>>& transformKeyframes)
{
    auto keyframe = AceType::MakeRefPtr<Keyframe<float>>(time, typeValue);
    transformKeyframes->AddKeyframe(keyframe);
}

void DOMNode::AddKeyframe(
    double time, const std::string& typeValue, RefPtr<KeyframeAnimation<float>>& transformKeyframes)
{
    DOMNode::AddKeyframe(time, StringToDouble(typeValue), transformKeyframes);
}

void DOMNode::SetSharedTransitionStyle(
    const std::vector<std::unordered_map<std::string, std::string>>& animationKeyframes)
{
    if (!declaration_) {
        return;
    }
    auto& shareTransitionStyle =
        declaration_->MaybeResetStyle<CommonShareTransitionStyle>(StyleTag::COMMON_SHARE_TRANSITION_STYLE);
    if (!shareTransitionStyle.IsValid()) {
        return;
    }
    if (!ParseAnimationStyle(animationKeyframes)) {
        return;
    }
    shareTransitionStyle.sharedTransitionOption = TweenOption();
    TweenOptionSetKeyframes(shareTransitionStyle.sharedTransitionOption);
}

bool DOMNode::ParseAnimationStyle(const std::vector<std::unordered_map<std::string, std::string>>& animationKeyframes)
{
    if (animationKeyframes.empty()) {
        return false;
    }
    animationUtil_.ParseAnimationStyle(animationKeyframes, declaration_, GetThemeConstants());
    propAnimations_ = animationUtil_.GetPropAnimationMap();
    animationName_ = animationUtil_.GetAnimationName();

    if (propAnimations_.find(AnimatableType::PROPERTY_OPACITY) != propAnimations_.end()) {
        if (declaration_) {
            declaration_->SetHasDisplayStyle(true);
        }
    }
    return true;
}

void DOMNode::SetAnimationStyle(const std::vector<std::unordered_map<std::string, std::string>>& animationKeyframes)
{
    if (!declaration_ || !ParseAnimationStyle(animationKeyframes)) {
        return;
    }
    if (isTransition_) {
        auto& pageTransitionStyle =
            declaration_->MaybeResetStyle<CommonPageTransitionStyle>(StyleTag::COMMON_PAGE_TRANSITION_STYLE);
        if (pageTransitionStyle.IsValid()) {
            if (isEnter_) {
                TweenOptionSetKeyframes(pageTransitionStyle.transitionEnterOption);
            } else {
                TweenOptionSetKeyframes(pageTransitionStyle.transitionExitOption);
            }
        }
    } else {
        auto& animationStyle = declaration_->MaybeResetStyle<CommonAnimationStyle>(StyleTag::COMMON_ANIMATION_STYLE);
        if (animationStyle.IsValid()) {
            animationStyle.tweenOption = TweenOption();
            TweenOptionSetKeyframes(animationStyle.tweenOption);
            animationStyleUpdated_ = true;
        }
    }
}

void DOMNode::UpdatePropAnimations(const PropAnimationMap& animations)
{
    if (boxComponent_ != nullptr) {
        boxComponent_->ClearAnimatables();
    }
    if (displayComponent_ != nullptr) {
        displayComponent_->ClearAnimatables();
    }
    for (const auto& [type, animation] : animations) {
        switch (type) {
            case AnimatableType::PROPERTY_WIDTH:
            case AnimatableType::PROPERTY_HEIGHT:
            case AnimatableType::PROPERTY_BG_COLOR:
            case AnimatableType::PROPERTY_MARGIN_LEFT:
            case AnimatableType::PROPERTY_MARGIN_TOP:
            case AnimatableType::PROPERTY_MARGIN_RIGHT:
            case AnimatableType::PROPERTY_MARGIN_BOTTOM:
            case AnimatableType::PROPERTY_PADDING_LEFT:
            case AnimatableType::PROPERTY_PADDING_TOP:
            case AnimatableType::PROPERTY_PADDING_RIGHT:
            case AnimatableType::PROPERTY_PADDING_BOTTOM:
            case AnimatableType::PROPERTY_BACKGROUND_POSITION:
            case AnimatableType::PROPERTY_BACKGROUND_SIZE:
            case AnimatableType::PROPERTY_BORDER_LEFT_WIDTH:
            case AnimatableType::PROPERTY_BORDER_TOP_WIDTH:
            case AnimatableType::PROPERTY_BORDER_RIGHT_WIDTH:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_WIDTH:
            case AnimatableType::PROPERTY_BORDER_LEFT_COLOR:
            case AnimatableType::PROPERTY_BORDER_TOP_COLOR:
            case AnimatableType::PROPERTY_BORDER_RIGHT_COLOR:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_COLOR:
            case AnimatableType::PROPERTY_BORDER_TOP_LEFT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_TOP_RIGHT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_LEFT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_LEFT_STYLE:
            case AnimatableType::PROPERTY_BORDER_RIGHT_STYLE:
            case AnimatableType::PROPERTY_BORDER_TOP_STYLE:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_STYLE:
            case AnimatableType::PROPERTY_FILTER_BLUR:
            case AnimatableType::PROPERTY_BACKDROP_FILTER_BLUR:
            case AnimatableType::PROPERTY_WINDOW_FILTER_BLUR:
            case AnimatableType::PROPERTY_BOX_SHADOW: {
                if (boxComponent_ != nullptr) {
                    boxComponent_->AddAnimatable(type, animation);
                }
                break;
            }
            case AnimatableType::PROPERTY_OPACITY: {
                if (displayComponent_ != nullptr) {
                    displayComponent_->AddAnimatable(type, animation);
                }
                break;
            }
            default:
                break;
        }
    }
}

void DOMNode::UpdatePositionAnimations(const RefPtr<Component> componet)
{
    if (componet != nullptr) {
        static const AnimatableType positionAnimatableType[] = {
            AnimatableType::PROPERTY_POSITION_LEFT,
            AnimatableType::PROPERTY_POSITION_TOP,
            AnimatableType::PROPERTY_POSITION_RIGHT,
            AnimatableType::PROPERTY_POSITION_BOTTOM,
        };

        for (const auto& type : positionAnimatableType) {
            const auto& animation = propAnimations_[type];
            if (animation) {
                componet->AddAnimatable(type, animation);
            }
        }
    }
}

void DOMNode::TweenOptionSetKeyframes(TweenOption& tweenOption)
{
    auto pos = propAnimations_.find(AnimatableType::PROPERTY_OPACITY);
    if (pos != propAnimations_.end()) {
        auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        for (auto& item : pos->second->GetAnimatable()) {
            auto opacityAnimatable = AceType::DynamicCast<AnimatableData<float>>(item);
            if (opacityAnimatable) {
                opacityAnimation->AddKeyframe(
                    AceType::MakeRefPtr<Keyframe<float>>(item->GetTimePoint(), opacityAnimatable->GetValue()));
            }
        }
        tweenOption.SetOpacityAnimation(opacityAnimation);
        propAnimations_.erase(pos);
    }
    if (!propAnimations_.empty()) {
        tweenOption.SetAnimatables(propAnimations_);
    }
    animationUtil_.GetTransformConvertor().AddAnimationToTweenOption(tweenOption);
}

void DOMNode::SetCustomAnimationStyleUpdate(bool enable)
{
    customAnimationStyleUpdated_ = enable;
}

void DOMNode::CompositeComponents()
{
    std::vector<RefPtr<SingleChild>> components;
    if (flexItemComponent_) {
        // Update flex item after PrepareSpecializedComponent to make sure sub class has finished prepare.
        UpdateFlexItemComponent();
        components.emplace_back(flexItemComponent_);
    }
    if (tweenComponent_) {
        tweenComponent_->SetLeafNode(IsLeafNode());
        components.emplace_back(tweenComponent_);
    }
    if (displayComponent_) {
        displayComponent_->DisableLayer(IsLeafNode());
        components.emplace_back(displayComponent_);
        if (focusableEventComponent_) {
            focusableEventComponent_->SetShow(GetDisplay() != DisplayType::NONE);
        }
    }
    if (transformComponent_) {
        components.emplace_back(transformComponent_);
    }
    if (focusableEventComponent_) {
        components.emplace_back(focusableEventComponent_);
    }
    if (touchEventComponent_) {
        touchEventComponent_->SetIsVisible(visible_ == VisibleType::VISIBLE);
        components.emplace_back(touchEventComponent_);
    }
    if (gestureEventComponent_) {
        gestureEventComponent_->SetIsVisible(visible_ == VisibleType::VISIBLE);
        components.emplace_back(gestureEventComponent_);
    }
#ifndef WEARABLE_PRODUCT
    if (multimodalComponent_) {
        components.emplace_back(multimodalComponent_);
    }
#endif
    if (mouseEventComponent_) {
        components.emplace_back(mouseEventComponent_);
    }
    if (propTransitionComponent_) {
        components.emplace_back(propTransitionComponent_);
    }
    if (boxComponent_) {
        components.emplace_back(boxComponent_);
    }
    if (scrollComponent_) {
        components.emplace_back(scrollComponent_);
    }
    if (sharedTransitionComponent_) {
        components.emplace_back(sharedTransitionComponent_);
    }

    auto mainComponent = GetSpecializedComponent();
    if (!components.empty() &&
        (AceType::InstanceOf<TextureComponent>(mainComponent) ||
        AceType::InstanceOf<TextFieldComponent>(mainComponent) ||
        AceType::InstanceOf<ListComponent>(mainComponent) ||
        AceType::InstanceOf<ListItemComponent>(mainComponent) ||
        AceType::InstanceOf<WebComponent>(mainComponent) ||
        AceType::InstanceOf<XComponentComponent>(mainComponent))) {
        Component::MergeRSNode(components);
        Component::MergeRSNode(mainComponent);
    } else {
        Component::MergeRSNode(components, mainComponent);
    }
    // Only fixed position has position component
    if (positionComponent_ && GetPosition() == PositionType::FIXED) {
        components.emplace(components.begin(), positionComponent_);
        Component::MergeRSNode(positionComponent_);
    }

    // First, composite all common components.
    for (int32_t idx = static_cast<int32_t>(components.size()) - 1; idx - 1 >= 0; --idx) {
        components[idx - 1]->SetChild(DynamicCast<Component>(components[idx]));
    }
    // Then composite specialized components.
    auto compositeComponent = CompositeSpecializedComponent(components);
    // At last add to composite component.
    rootComponent_->SetChild(compositeComponent);
    // final set disabled status.
    if (declaration_ && declaration_->IsDisabled() != rootComponent_->IsDisabledStatus()) {
        rootComponent_->SetDisabledStatus(declaration_->IsDisabled());
    }
}

void DOMNode::UpdateFlexItemComponent()
{
    if (!declaration_ || !flexItemComponent_) {
        return;
    }

    auto& flexStyle = static_cast<CommonFlexStyle&>(declaration_->GetStyle(StyleTag::COMMON_FLEX_STYLE));
    if (flexStyle.IsValid()) {
        flexItemComponent_->SetFlexGrow(flexStyle.flexGrow);
        flexItemComponent_->SetFlexShrink(flexStyle.flexShrink);
        flexItemComponent_->SetFlexBasis(flexStyle.flexBasis);
        flexItemComponent_->SetFlexWeight(flexStyle.flexWeight);
        flexItemComponent_->SetDisplayIndex(flexStyle.displayIndex);
    }
    const LinearMapNode<FlexAlign> ALIGN_SELF_TABLE[] = {
        { DOM_ALIGN_ITEMS_BASELINE, FlexAlign::BASELINE },
        { DOM_ALIGN_ITEMS_CENTER, FlexAlign::CENTER },
        { DOM_ALIGN_ITEMS_END, FlexAlign::FLEX_END },
        { DOM_ALIGN_ITEMS_START, FlexAlign::FLEX_START },
        { DOM_ALIGN_ITEMS_STRETCH, FlexAlign::STRETCH },
    };
    auto& commonStyle = static_cast<CommonStyle&>(declaration_->GetStyle(StyleTag::COMMON_STYLE));
    int64_t index = -1;
    if (commonStyle.IsValid()) {
        index = BinarySearchFindIndex(ALIGN_SELF_TABLE, ArraySize(ALIGN_SELF_TABLE), commonStyle.alignSelf.c_str());
    }
    if (index != -1) {
        flexItemComponent_->SetAlignSelf(ALIGN_SELF_TABLE[index].value);
    } else {
        flexItemComponent_->SetAlignSelf(FlexAlign::AUTO);
    }
    auto parentNode = parentNode_.Upgrade();
    if ((parentNode) &&
        (parentNode->GetTag() == DOM_NODE_TAG_DIV || parentNode->GetTag() == DOM_NODE_TAG_GRID_COLUMN)) {
        auto parent = AceType::DynamicCast<DOMDiv>(parentNode);
        // Stretch flag means that if the child's main size is determined, it can not be stretched.
        if ((parent->GetFlexDirection() == FlexDirection::ROW && (GreatOrEqual(GetHeight().Value(), 0.0) ||
            GetHeight().Unit() == DimensionUnit::CALC)) ||
            (parent->GetFlexDirection() == FlexDirection::COLUMN && (GreatOrEqual(GetWidth().Value(), 0.0) ||
            GetWidth().Unit() == DimensionUnit::CALC))) {
            flexItemComponent_->SetStretchFlag(false);
        } else {
            flexItemComponent_->SetStretchFlag(true);
        }
    }
    auto& sizeStyle = static_cast<CommonSizeStyle&>(declaration_->GetStyle(StyleTag::COMMON_SIZE_STYLE));
    if (sizeStyle.IsValid()) {
        // Make sure input layout constraint is valid
        flexItemComponent_->SetMaxHeight(sizeStyle.maxHeight);
        flexItemComponent_->SetMaxWidth(sizeStyle.maxWidth);
        flexItemComponent_->SetMinHeight(sizeStyle.minHeight);
        flexItemComponent_->SetMinWidth(sizeStyle.minWidth);
        // If set aspect Ratio, it cannot be stretch
        if (GreatNotEqual(sizeStyle.aspectRatio, 0.0)) {
            flexItemComponent_->SetStretchFlag(false);
        }
    }
    // If set display, this flexItem is ignored.
    flexItemComponent_->SetIsHidden(GetDisplay() == DisplayType::NONE);
}

void DOMNode::UpdateUiComponents()
{
    UpdateBoxComponent();
    UpdateDisplayComponent();
    UpdateTweenComponent();
}

void DOMNode::UpdateBoxSize(const CalcDimension& width, const CalcDimension& height)
{
    if (height.Unit() == DimensionUnit::CALC) {
        boxComponent_->SetHeight(height.CalcValue(), height.Unit());
    } else {
        boxComponent_->SetHeight(height.Value(), height.Unit());
    }
    if (width.Unit() == DimensionUnit::CALC) {
        boxComponent_->SetWidth(width.CalcValue(), width.Unit());
    } else {
        boxComponent_->SetWidth(width.Value(), width.Unit());
    }
}

void DOMNode::UpdateBoxPadding(const Edge& padding)
{
    boxComponent_->SetPadding(padding);
}

void DOMNode::UpdateBoxBorder(const Border& border)
{
    if (declaration_ && declaration_->GetBackDecoration()) {
        declaration_->GetBackDecoration()->SetBorder(border);
    }
}

void DOMNode::UpdateBoxComponent()
{
    if (!declaration_) {
        return;
    }

    if (declaration_->HasBoxStyle()) {
        if (GetWidth().Unit() != DimensionUnit::CALC && GetHeight().Unit() != DimensionUnit::CALC) {
            UpdateBoxSize(GetWidth(), GetHeight());
        } else if (GetWidth().Unit() == DimensionUnit::CALC && GetHeight().Unit() != DimensionUnit::CALC) {
            UpdateBoxSize(GetCalcWidth(), GetHeight());
        } else if (GetWidth().Unit() != DimensionUnit::CALC && GetHeight().Unit() == DimensionUnit::CALC) {
            UpdateBoxSize(GetWidth(), GetCalcHeight());
        } else {
            UpdateBoxSize(GetCalcWidth(), GetCalcHeight());
        }
        auto& paddingStyle = static_cast<CommonPaddingStyle&>(declaration_->GetStyle(StyleTag::COMMON_PADDING_STYLE));
        if (paddingStyle.IsValid()) {
            UpdateBoxPadding(paddingStyle.padding);
        }
        auto& marginStyle = static_cast<CommonMarginStyle&>(declaration_->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
        if (marginStyle.IsValid()) {
            boxComponent_->SetMargin(marginStyle.margin);
        }
        auto& commonStyle = static_cast<CommonStyle&>(declaration_->GetStyle(StyleTag::COMMON_STYLE));
        if (commonStyle.IsValid()) {
            boxComponent_->SetLayoutInBoxFlag(commonStyle.layoutInBox);
        }
    }

    if (flexItemComponent_) {
        boxComponent_->SetDeliverMinToChild(false);
        auto& sizeStyle = static_cast<CommonSizeStyle&>(declaration_->GetStyle(StyleTag::COMMON_SIZE_STYLE));
        if (sizeStyle.IsValid()) {
            boxComponent_->SetAspectRatio(sizeStyle.aspectRatio);
            boxComponent_->SetMinWidth(sizeStyle.minWidth);
            boxComponent_->SetMinHeight(sizeStyle.minHeight);
            boxComponent_->SetMaxWidth(sizeStyle.maxWidth);
            boxComponent_->SetMaxHeight(sizeStyle.maxHeight);
            boxComponent_->SetBoxSizing(sizeStyle.boxSizing);
        }
    }

    if (declaration_->HasDecorationStyle()) {
        auto& borderStyle = declaration_->MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
        if (borderStyle.IsValid()) {
            UpdateBoxBorder(borderStyle.border);
        }
        auto& backDecoration = declaration_->GetBackDecoration();
        auto& backgroundStyle =
            static_cast<CommonBackgroundStyle&>(declaration_->GetStyle(StyleTag::COMMON_BACKGROUND_STYLE));
        if (backgroundStyle.IsValid() && backgroundStyle.gradient.IsValid()) {
            backDecoration->SetGradient(backgroundStyle.gradient);
        }
        if (backgroundStyle.IsValid() && backgroundStyle.gradientBorderImage.IsValid()) {
            backDecoration->SetGradientBorderImage(backgroundStyle.gradientBorderImage);
        }

        if (declaration_->HasShadowStyle()) {
            auto& shadowStyle = static_cast<CommonShadowStyle&>(declaration_->GetStyle(StyleTag::COMMON_SHADOW_STYLE));
            if (shadowStyle.IsValid()) {
                backDecoration->ClearAllShadow();
                backDecoration->AddShadow(shadowStyle.shadow);
            }
        }
        boxComponent_->SetBackDecoration(backDecoration);
    }

    if (declaration_->HasFrontDecorationStyle()) {
        boxComponent_->SetFrontDecoration(declaration_->GetFrontDecoration());
    }
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (overflowStyle.IsValid()) {
        boxComponent_->SetOverflow(overflowStyle.overflow);
    }

    boxComponent_->SetOnDomDragEnter(GetDragEnterId());
    boxComponent_->SetOnDomDragOver(GetDragOverId());
    boxComponent_->SetOnDomDragLeave(GetDragLeaveId());
    boxComponent_->SetOnDomDragDrop(GetDragDropId());

    auto& clipPathStyle = static_cast<CommonClipPathStyle&>(declaration_->GetStyle(StyleTag::COMMON_CLIP_PATH_STYLE));
    if (clipPathStyle.IsValid() && clipPathStyle.clipPath != nullptr) {
        boxComponent_->SetClipPath(clipPathStyle.clipPath);
    }

    auto& maskStyle = static_cast<CommonMaskStyle&>(declaration_->GetStyle(StyleTag::COMMON_MASK_STYLE));
    if (maskStyle.IsValid() && !maskStyle.maskImage.empty()) {
        BackgroundImagePosition position;
        if (!maskStyle.maskPosition.empty()) {
            ParseBackgroundImagePosition(maskStyle.maskPosition, position);
        }

        BackgroundImageSize size;
        if (!maskStyle.maskSize.empty()) {
            ParseBackgroundImageSize(maskStyle.maskSize, size);
        }
        boxComponent_->SetMask(maskStyle.maskImage, size, position);
    }
}

void DOMNode::PrepareScrollComponent()
{
    if (!declaration_) {
        return;
    }

    // div is specially handled.
    if (GetTag() == DOM_NODE_TAG_DIV) {
        return;
    }
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (!declaration_->HasOverflowStyle() || !overflowStyle.IsValid() || overflowStyle.overflow != Overflow::SCROLL) {
        return;
    }
    if (boxComponent_->GetWidthDimension().IsValid() && boxComponent_->GetHeightDimension().IsValid()) {
        if (!scrollComponent_) {
            scrollComponent_ = AceType::MakeRefPtr<ScrollComponent>(nullptr);
        }
        scrollComponent_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
            overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
        declaration_->SetPositionController(scrollComponent_->GetScrollPositionController());
    }
}

void DOMNode::UpdateDisplayComponent()
{
    if (!declaration_ || !declaration_->HasDisplayStyle()) {
        return;
    }

    if (!displayComponent_) {
        displayComponent_ = AceType::MakeRefPtr<DisplayComponent>(rootComponent_->GetChild());
    }
    auto& opacityStyle = static_cast<CommonOpacityStyle&>(declaration_->GetStyle(StyleTag::COMMON_OPACITY_STYLE));
    if (opacityStyle.IsValid()) {
        displayComponent_->SetOpacity(opacityStyle.opacity);
        displayComponent_->SetAppearingDuration(opacityStyle.appearingDuration);
    }
    auto context = pipelineContext_.Upgrade();
    if (!context) {
        return;
    }
    SetDisplayStyle();
    displayComponent_->SetVisible(visible_);
    bool show = (GetDisplay() != DisplayType::NONE);
    if (!focusableEventComponent_ && !show && !context->IsJsCard()) {
        focusableEventComponent_ = AceType::MakeRefPtr<FocusableComponent>();
        focusableEventComponent_->SetFocusable(true);
        declaration_->SetFocusableController(focusableEventComponent_->GetFocusableController());
    }
    if (focusableEventComponent_) {
        focusableEventComponent_->SetShow(show);
    }
    if (declaration_->HasShadowStyle()) {
        auto& shadowStyle = static_cast<CommonShadowStyle&>(declaration_->GetStyle(StyleTag::COMMON_SHADOW_STYLE));
        if (shadowStyle.IsValid()) {
            displayComponent_->SetShadow(shadowStyle.shadow);
        }
    }
}

void DOMNode::UpdateTweenComponent()
{
    if (transitionStyleUpdated_) {
        if (!propTransitionComponent_) {
            propTransitionComponent_ = AceType::MakeRefPtr<TransitionComponent>(TRANSITION_COMPONENT_PREFIX
                + std::to_string(nodeId_), tag_);
        }
        if (SystemProperties::GetRosenBackendEnabled()) {
            propTransitionOption_.SetAllowRunningAsynchronously(true);
        }
        propTransitionComponent_->SetTransitionOption(propTransitionOption_);
        transitionStyleUpdated_ = false;
    }
    if (!declaration_) {
        return;
    }
    if (declaration_->HasTransformStyle() && !transformComponent_) {
        transformComponent_ = AceType::MakeRefPtr<TransformComponent>();
    }
    auto& animationStyle = static_cast<CommonAnimationStyle&>(declaration_->GetStyle(StyleTag::COMMON_ANIMATION_STYLE));
    auto& shadowStyle = static_cast<CommonShadowStyle&>(declaration_->GetStyle(StyleTag::COMMON_SHADOW_STYLE));
    auto& clickEffectAttr =
        static_cast<CommonClickEffectAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_CLICK_EFFECT_ATTR));
    if (transformComponent_) {
        if (clickEffectAttr.IsValid()) {
            transformComponent_->SetClickSpringEffectType(clickEffectAttr.clickEffect);
        }
        if (animationStyle.IsValid()) {
            // Set transform style.
            if (declaration_->HasTransformOriginStyle()) {
                transformComponent_->SetOriginDimension(
                    DimensionOffset(animationStyle.transformOriginX, animationStyle.transformOriginY));
            }
        }

        if (declaration_->HasShadowStyle() && shadowStyle.IsValid()) {
            transformComponent_->SetShadow(shadowStyle.shadow);
        }

        auto& cardTransitionStyle =
            static_cast<CommonCardTransitionStyle&>(declaration_->GetStyle(StyleTag::COMMON_CARD_TRANSITION_STYLE));
        if (cardTransitionStyle.IsValid()) {
            transformComponent_->SetTransitionEffect(cardTransitionStyle.transitionEffect);
        }
    }

    // Only check animation style here.
    if (animationStyle.IsValid() && animationStyle.tweenOption.IsValid() && animationStyleUpdated_) {
        if (!tweenComponent_) {
            tweenComponent_ = AceType::MakeRefPtr<TweenComponent>(COMPONENT_PREFIX + std::to_string(nodeId_), tag_);
        }
        if (SystemProperties::GetRosenBackendEnabled()) {
            animationStyle.tweenOption.SetAllowRunningAsynchronously(true);
        }
        tweenComponent_->SetTweenOption(animationStyle.tweenOption);
        tweenComponent_->UpdateAnimationName(animationName_);
        tweenComponent_->SetAnimationOperation(animationStyle.animationOperation);
        if (declaration_->HasShadowStyle() && shadowStyle.IsValid()) {
            tweenComponent_->SetShadow(shadowStyle.shadow);
        }
        UpdatePropAnimations(propAnimations_);
        animationStyleUpdated_ = false;
    }
    // for custom animation
    if (customAnimationStyleUpdated_) {
        UpdatePropAnimations(propAnimations_);
        customAnimationStyleUpdated_ = false;
    }
    if (!shareId_.empty()) {
        if (!sharedTransitionComponent_) {
            sharedTransitionComponent_ = AceType::MakeRefPtr<SharedTransitionComponent>(
                "FrontendShared" + std::to_string(nodeId_), tag_, shareId_);
            auto& sharedTransitionStyle = static_cast<CommonShareTransitionStyle&>(
                declaration_->GetStyle(StyleTag::COMMON_SHARE_TRANSITION_STYLE));
            if (SystemProperties::GetRosenBackendEnabled() && sharedTransitionStyle.IsValid()) {
                sharedTransitionStyle.sharedTransitionOption.SetAllowRunningAsynchronously(true);
            }
            if (sharedTransitionStyle.IsValid()) {
                sharedTransitionComponent_->SetOption(sharedTransitionStyle.sharedTransitionOption);
                sharedTransitionComponent_->SetEffect(sharedTransitionStyle.sharedEffect);
            }
        } else {
            sharedTransitionComponent_->SetShareId(shareId_);
        }
    }
}

bool DOMNode::IsTabbarSubNode() const
{
    if (isRootNode_) {
        return GetTag() == DOM_NODE_TAG_TAB_BAR;
    }
    RefPtr<DOMNode> parent = parentNode_.Upgrade();
    if (!parent) {
        return false;
    }

    if (parent->GetTag() == DOM_NODE_TAG_TAB_BAR) {
        return true;
    }

    while (!parent->IsRootNode()) {
        if (parent->GetTag() == DOM_NODE_TAG_TAB_BAR) {
            return true;
        }
        parent = parent->GetParentNode();
        if (!parent) {
            return false;
        }
    }
    return false;
}

void DOMNode::PrepareTouchEvent(EventMarker& eventMarker, uint32_t type)
{
    bool isTababrSubNode = IsTabbarSubNode();
    if (isTababrSubNode) {
        return;
    }

    auto weak = AceType::WeakClaim(this);
    if (eventMarker.IsEmpty()) {
        eventMarker = BackEndEventManager<void(const TouchEventInfo&)>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void(const TouchEventInfo&)>::GetInstance().BindBackendEvent(
            eventMarker, [](const TouchEventInfo&) {});
    }
    eventMarker.SetPreFunction([weak, type]() {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        domNode->OnActive(type != EventType::TOUCH_UP && type != EventType::TOUCH_CANCEL);
    });
}

void DOMNode::PrepareMouseHoverEvent()
{
    if (!declaration_) {
        return;
    }
    auto& mouseEvent = declaration_->MaybeResetEvent<CommonMouseEvent>(EventTag::COMMON_MOUSE_EVENT);
    if (!mouseEvent.IsValid()) {
        return;
    }
    auto weak = AceType::WeakClaim(this);
    if (mouseEvent.mouseHover.eventMarker.IsEmpty()) {
        mouseEvent.mouseHover.eventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        mouseEvent.mouseHover.isRefreshed = true;
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(mouseEvent.mouseHover.eventMarker, []() {});
    }
    mouseEvent.mouseHover.eventMarker.SetPreFunction([weak]() {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        domNode->OnHover(true);
    });

    if (mouseEvent.mouseHoverExit.eventMarker.IsEmpty()) {
        mouseEvent.mouseHoverExit.eventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        mouseEvent.mouseHoverExit.isRefreshed = true;
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(mouseEvent.mouseHoverExit.eventMarker, []() {});
    }
    mouseEvent.mouseHoverExit.eventMarker.SetPreFunction([weak]() {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        domNode->OnHover(false);
    });
}

void DOMNode::PrepareFocusableEventId()
{
    if (!declaration_) {
        return;
    }
    auto& focusEvent = declaration_->MaybeResetEvent<CommonFocusEvent>(EventTag::COMMON_FOCUS_EVENT);
    if (!focusEvent.IsValid()) {
        return;
    }

    auto weak = AceType::WeakClaim(this);
    if (focusEvent.focus.eventMarker.IsEmpty()) {
        focusEvent.focus.eventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        focusEvent.focus.isRefreshed = true;
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(focusEvent.focus.eventMarker, []() {});
    }

    focusEvent.focus.eventMarker.SetPreFunction([weak]() {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        domNode->OnFocus(true);
    });

    if (focusEvent.blur.eventMarker.IsEmpty()) {
        focusEvent.blur.eventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        focusEvent.blur.isRefreshed = true;
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(focusEvent.blur.eventMarker, []() {});
    }

    focusEvent.blur.eventMarker.SetPreFunction([weak]() {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        domNode->OnFocus(false);
    });
}

void DOMNode::UpdateTouchEventComponent()
{
    if (!declaration_) {
        return;
    }

    auto& swipeEvent = static_cast<CommonSwipeEvent&>(declaration_->GetEvent(EventTag::COMMON_SWIPE_EVENT));
    if (swipeEvent.IsValid() && !swipeEvent.swipe.eventMarker.IsEmpty()) {
        if (!touchEventComponent_) {
            touchEventComponent_ = AceType::MakeRefPtr<TouchListenerComponent>();
        }
        touchEventComponent_->SetOnSwipeId(swipeEvent.swipe.eventMarker);
    }

    auto& touchableAttr =
        static_cast<CommonTouchableAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_TOUCHABLE_ATTR));
    if (touchableAttr.IsValid() && !touchableAttr.touchable) {
        if (!touchEventComponent_) {
            touchEventComponent_ = AceType::MakeRefPtr<TouchListenerComponent>();
        }
        touchEventComponent_->SetTouchable(touchableAttr.touchable);
    }

    auto& rawEvent = static_cast<CommonRawEvent&>(declaration_->GetEvent(EventTag::COMMON_RAW_EVENT));
    if (!rawEvent.IsValid()) {
        return;
    }
    for (uint32_t eventAction = 0; eventAction < EventAction::SIZE; eventAction++) {
        for (uint32_t eventStage = 0; eventStage < EventStage::SIZE; eventStage++) {
            for (uint32_t touchEventType = 0; touchEventType < EventType::SIZE; touchEventType++) {
                EventMarker& eventMarker = GetTouchId(eventAction, eventStage, touchEventType);
                if (!eventMarker.IsEmpty() || HasActivePseudo()) {
                    if (!touchEventComponent_) {
                        touchEventComponent_ = AceType::MakeRefPtr<TouchListenerComponent>();
                    }
                    if (HasActivePseudo() && touchEventType != EventType::TOUCH_MOVE &&
                        eventAction == EventAction::ON && eventStage == EventStage::BUBBLE) {
                        PrepareTouchEvent(eventMarker, touchEventType);
                    }
                    touchEventComponent_->SetEvent(eventMarker, eventAction, eventStage, touchEventType);
                }
            }
        }
    }
}

void DOMNode::UpdateMouseEventComponent()
{
    if (!declaration_) {
        return;
    }
    auto& mouseEvent = declaration_->MaybeResetEvent<CommonMouseEvent>(EventTag::COMMON_MOUSE_EVENT);
    if (!mouseEvent.IsValid()) {
        return;
    }
    if (!mouseEvent.mouse.eventMarker.IsEmpty()) {
        if (!mouseEventComponent_) {
            mouseEventComponent_ = AceType::MakeRefPtr<MouseListenerComponent>();
        }
        mouseEventComponent_->SetOnMouseId(mouseEvent.mouse.eventMarker);
    }

    PrepareMouseHoverEvent();
    if (!mouseEvent.mouseHover.eventMarker.IsEmpty()) {
        if (!mouseEventComponent_) {
            mouseEventComponent_ = AceType::MakeRefPtr<MouseListenerComponent>();
        }
        mouseEventComponent_->SetOnMouseHoverId(mouseEvent.mouseHover.eventMarker);
    }
    if (!mouseEvent.mouseHoverExit.eventMarker.IsEmpty()) {
        if (!mouseEventComponent_) {
            mouseEventComponent_ = AceType::MakeRefPtr<MouseListenerComponent>();
        }
        mouseEventComponent_->SetOnMouseHoverExitId(mouseEvent.mouseHoverExit.eventMarker);
    }
}

void DOMNode::UpdateGestureEventComponent()
{
    if (!declaration_) {
        return;
    }

    if (!GetClickId().IsEmpty() || !GetDoubleClickId().IsEmpty() || !GetLongPressId().IsEmpty() ||
        !GetPinchStartId().IsEmpty() || !GetPinchUpdateId().IsEmpty() || !GetPinchEndId().IsEmpty() ||
        !GetPinchCancelId().IsEmpty() || declaration_->HasIdAttr() || !GetDragStartId().IsEmpty() ||
        !GetDragId().IsEmpty() || !GetDragEndId().IsEmpty()) {
        if (!gestureEventComponent_) {
            gestureEventComponent_ = AceType::MakeRefPtr<GestureListenerComponent>();
        }
        gestureEventComponent_->SetOnClickId(GetClickId());
        gestureEventComponent_->SetOnDoubleClickId(GetDoubleClickId());
        gestureEventComponent_->SetOnLongPressId(GetLongPressId());
        gestureEventComponent_->SetOnPinchStartId(GetPinchStartId());
        gestureEventComponent_->SetOnPinchUpdateId(GetPinchUpdateId());
        gestureEventComponent_->SetOnPinchEndId(GetPinchEndId());
        gestureEventComponent_->SetOnPinchCancelId(GetPinchCancelId());
        gestureEventComponent_->SetOnFreeDragStartId(GetDragStartId());
        gestureEventComponent_->SetOnFreeDragUpdateId(GetDragId());
        gestureEventComponent_->SetOnFreeDragEndId(GetDragEndId());
    }
}

void DOMNode::UpdateFocusableEventComponents()
{
    if (!declaration_ || (UNFOCUSABLED_NODE.find(tag_) != UNFOCUSABLED_NODE.end())) {
        return;
    }

    auto& focusableAttr =
        static_cast<CommonFocusableAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_FOCUSABLE_ATTR));
    auto& focusEvent = declaration_->MaybeResetEvent<CommonFocusEvent>(EventTag::COMMON_FOCUS_EVENT);
    auto& keyEvent = static_cast<CommonKeyEvent&>(declaration_->GetEvent(EventTag::COMMON_KEY_EVENT));
    bool focusable = focusableAttr.IsValid() && focusableAttr.focusable.second;
    bool supportFocus = focusEvent.IsValid() && !focusEvent.focus.eventMarker.IsEmpty();
    bool supportBlur = focusEvent.IsValid() && !focusEvent.blur.eventMarker.IsEmpty();
    bool supportKey = keyEvent.IsValid() && !keyEvent.key.eventMarker.IsEmpty();

    if (!focusable && GetClickId().IsEmpty() && !supportFocus && !supportBlur && !supportKey && !HasFocusPseudo()) {
        return;
    }
    if (!focusableEventComponent_) {
        focusableEventComponent_ = AceType::MakeRefPtr<FocusableComponent>();
        declaration_->SetFocusableController(focusableEventComponent_->GetFocusableController());
    }
    if (focusableAttr.IsValid() && focusableAttr.focusable.second) {
        focusableEventComponent_->SetFocusable(focusableAttr.focusable.first);
    } else {
        focusableEventComponent_->SetFocusable(true);
    }

    focusableEventComponent_->SetOnClickId(GetClickId());

    PrepareFocusableEventId();

    if (focusEvent.IsValid()) {
        focusableEventComponent_->SetOnBlurId(focusEvent.blur.eventMarker);
        focusableEventComponent_->SetOnFocusId(focusEvent.focus.eventMarker);
    }
    if (keyEvent.IsValid()) {
        focusableEventComponent_->SetOnKeyId(keyEvent.key.eventMarker);
    }
    if (g_focusableNode.empty()) {
        g_focusableNode = std::unordered_set<std::string>({
            DOM_NODE_TAG_BUTTON,
            DOM_NODE_TAG_DIV,
            DOM_NODE_TAG_FORM,
            DOM_NODE_TAG_LIST,
            DOM_NODE_TAG_LIST_ITEM,
            DOM_NODE_TAG_REFRESH,
            DOM_NODE_TAG_INPUT,
            DOM_NODE_TAG_OPTION,
            DOM_NODE_TAG_PROGRESS,
            DOM_NODE_TAG_POPUP,
            DOM_NODE_TAG_RATING,
            DOM_NODE_TAG_SELECT,
            DOM_NODE_TAG_SLIDER,
            DOM_NODE_TAG_STACK,
            DOM_NODE_TAG_STEPPER,
            DOM_NODE_TAG_STEPPER_ITEM,
            DOM_NODE_TAG_SWIPER,
            DOM_NODE_TAG_SWITCH,
            DOM_NODE_TAG_TABS,
            DOM_NODE_TAG_TAB_BAR,
            DOM_NODE_TAG_TAB_CONTENT,
            DOM_NODE_TAG_TEXTAREA,
            DOM_NODE_TAG_TOGGLE,
            DOM_NODE_TAG_SEARCH,
            DOM_NODE_TAG_VIDEO,
            DOM_NODE_TAG_CALENDAR,
            DOM_NODE_TAG_PICKER_DIALOG,
            DOM_NODE_TAG_PIECE,
        });
    }
    if (g_focusableNode.find(tag_) != g_focusableNode.end()) {
        focusableEventComponent_->SetFocusNode(false);
    } else {
        focusableEventComponent_->SetFocusNode(true);
    }
}

void DOMNode::UpdatePositionProps()
{
    if (!declaration_ || !declaration_->HasPositionStyle() || GetPosition() == PositionType::FIXED) {
        return;
    }

    declaration_->SetHasPositionStyle(false);
    auto tweenComponent = AceType::DynamicCast<TweenComponent>(rootComponent_->GetChild());
    if (tweenComponent) {
        UpdateTweenPosition(tweenComponent);
        UpdatePositionAnimations(tweenComponent);
    } else {
        // Set position props to first RenderComponent of node
        auto compositeComponent = AceType::DynamicCast<RenderComponent>(rootComponent_->GetChild());
        if (!compositeComponent) {
            return;
        }
        compositeComponent->SetHasLeft(declaration_->HasLeft());
        compositeComponent->SetHasTop(declaration_->HasTop());
        compositeComponent->SetHasBottom(declaration_->HasBottom());
        compositeComponent->SetHasRight(declaration_->HasRight());
        auto& positionStyle =
            static_cast<CommonPositionStyle&>(declaration_->GetStyle(StyleTag::COMMON_POSITION_STYLE));
        if (positionStyle.IsValid()) {
            if (declaration_->HasLeft()) {
                compositeComponent->SetLeft(positionStyle.left);
                // reset value
                declaration_->SetHasLeft(false);
            }
            if (declaration_->HasRight()) {
                compositeComponent->SetRight(positionStyle.right);
                declaration_->SetHasRight(false);
            }
            if (declaration_->HasTop()) {
                compositeComponent->SetTop(positionStyle.top);
                declaration_->SetHasTop(false);
            }
            if (declaration_->HasBottom()) {
                compositeComponent->SetBottom(positionStyle.bottom);
                declaration_->SetHasBottom(false);
            }
            compositeComponent->SetPositionType(positionStyle.position);
        }
        UpdatePositionAnimations(compositeComponent);
    }
}

void DOMNode::UpdateTweenPosition(const RefPtr<TweenComponent> tweenComponent)
{
    if (!declaration_ || !tweenComponent) {
        return;
    }
    tweenComponent->SetHasLeft(declaration_->HasLeft());
    tweenComponent->SetHasTop(declaration_->HasTop());
    tweenComponent->SetHasBottom(declaration_->HasBottom());
    tweenComponent->SetHasRight(declaration_->HasRight());

    auto& positionStyle = static_cast<CommonPositionStyle&>(declaration_->GetStyle(StyleTag::COMMON_POSITION_STYLE));
    if (!positionStyle.IsValid()) {
        return;
    }
    if (declaration_->HasLeft()) {
        tweenComponent->SetLeft(positionStyle.left);
        // reset value
        declaration_->SetHasLeft(false);
    }
    if (declaration_->HasRight()) {
        tweenComponent->SetRight(positionStyle.right);
        declaration_->SetHasRight(false);
    }
    if (declaration_->HasTop()) {
        tweenComponent->SetTop(positionStyle.top);
        declaration_->SetHasTop(false);
    }
    if (declaration_->HasBottom()) {
        tweenComponent->SetBottom(positionStyle.bottom);
        declaration_->SetHasBottom(false);
    }
    tweenComponent->SetPositionType(positionStyle.position);
}

void DOMNode::UpdatePositionComponent()
{
    if (!declaration_ || !declaration_->HasPositionStyle() || GetPosition() != PositionType::FIXED) {
        return;
    }
    if (!positionComponent_) {
        positionComponent_ = AceType::MakeRefPtr<PositionedComponent>();
    }
    auto& positionStyle = static_cast<CommonPositionStyle&>(declaration_->GetStyle(StyleTag::COMMON_POSITION_STYLE));
    if (positionStyle.IsValid()) {
        positionComponent_->SetTop(positionStyle.top);
        positionComponent_->SetRight(positionStyle.right);
        positionComponent_->SetBottom(positionStyle.bottom);
        positionComponent_->SetLeft(positionStyle.left);
        positionComponent_->SetHasLeft(declaration_->HasLeft());
        positionComponent_->SetHasRight(declaration_->HasRight());
        positionComponent_->SetHasBottom(declaration_->HasBottom());
        positionComponent_->SetHasTop(declaration_->HasTop());
    }
    UpdatePositionAnimations(positionComponent_);
}

RefPtr<Component> DOMNode::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    const auto& specializedComponent = GetSpecializedComponent();
    if (specializedComponent) {
        bool touchable = true;
        if (declaration_) {
            auto& touchableAttr =
                static_cast<CommonTouchableAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_TOUCHABLE_ATTR));
            if (touchableAttr.IsValid()) {
                touchable = touchableAttr.touchable;
            }
        }
        specializedComponent->SetTouchable(touchable);
    }
    if (components.empty()) {
        return specializedComponent;
    } else {
        const auto& parent = components.back();
        if (parent) {
            parent->SetChild(specializedComponent);
        }
        return AceType::DynamicCast<Component>(components.front());
    }
}

#ifndef WEARABLE_PRODUCT
void DOMNode::UpdateMultimodalComponent()
{
    if (!declaration_ || GetClickId().IsEmpty()) {
        return;
    }
    auto& multimodalAttr =
        static_cast<CommonMultimodalAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_MULTIMODAL_ATTR));
    if (!multimodalAttr.IsValid() || multimodalAttr.IsUnavailable() || multimodalAttr.scene == SceneLabel::SWITCH) {
        return;
    }
    if (!multimodalComponent_) {
        multimodalComponent_ = AceType::MakeRefPtr<MultimodalComponent>(pageId_);
    }
    multimodalComponent_->SetMultimodalProperties(multimodalAttr);
    multimodalComponent_->SetOnClickId(GetClickId());
}
#endif

RefPtr<ThemeConstants> DOMNode::GetThemeConstants() const
{
    auto themeManager = GetThemeManager();
    if (!themeManager) {
        return nullptr;
    }
    return themeManager->GetThemeConstants();
}

Color DOMNode::ParseColor(const std::string& value, uint32_t maskAlpha) const
{
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is null, return Transparent color.");
        return Color::TRANSPARENT;
    }
    auto&& noRefFunc = [&value, maskAlpha = maskAlpha]() { return Color::FromString(value, maskAlpha); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetColor(refId); };
    return ParseThemeReference<Color>(value, noRefFunc, idRefFunc, Color::TRANSPARENT);
}

double DOMNode::ParseDouble(const std::string& value) const
{
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is null, return 0.");
        return 0.0;
    }
    auto&& noRefFunc = [&value]() { return StringUtils::StringToDouble(value); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetDouble(refId); };
    return ParseThemeReference<double>(value, noRefFunc, idRefFunc, 0.0);
}

CalcDimension DOMNode::ParseDimension(const std::string& value) const
{
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is null, return 0 dimension.");
        return Dimension();
    }
    auto&& noRefFunc = [&value]() { return StringUtils::StringToDimension(value); };
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetDimension(refId); };
    if (value.find("calc") != std::string::npos) {
        return CalcDimension(value, DimensionUnit::CALC);
    } else {
        return ParseThemeReference<Dimension>(value, noRefFunc, idRefFunc, Dimension());
    }
}

Dimension DOMNode::ParseLineHeight(const std::string& value) const
{
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is null, return 0 line height.");
        return Dimension();
    }
    const auto& parseResult = ThemeUtils::ParseThemeIdReference(value, GetThemeConstants());
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

std::vector<std::string> DOMNode::ParseFontFamilies(const std::string& value) const
{
    std::vector<std::string> fontFamilies;
    std::stringstream stream(value);
    std::string fontFamily;

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is null, return empty font families.");
        return fontFamilies;
    }
    auto&& idRefFunc = [constants = themeConstants](uint32_t refId) { return constants->GetString(refId); };

    while (getline(stream, fontFamily, ',')) {
        auto&& noRefFunc = [&fontFamily]() { return fontFamily; };
        fontFamilies.emplace_back(ParseThemeReference<std::string>(fontFamily, noRefFunc, idRefFunc, fontFamily));
    }
    return fontFamilies;
}

std::vector<Dimension> DOMNode::ParsePreferFontSizes(const std::string& value) const
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

std::string DOMNode::ParseImageSrc(const std::string& imgSrc) const
{
    return ThemeUtils::ProcessImageSource(imgSrc, GetThemeConstants());
}

void DOMNode::AdjustParamInLiteMode()
{
    if (!declaration_) {
        return;
    }

    declaration_->SetUseLiteStyle(true);
    // Change default value
    if (boxComponent_) {
        boxComponent_->SetUseLiteStyle(true);
        boxComponent_->SetAlignment(Alignment::TOP_CENTER);
    }
    auto& flexStyle = declaration_->MaybeResetStyle<CommonFlexStyle>(StyleTag::COMMON_FLEX_STYLE);
    if (flexStyle.IsValid()) {
        flexStyle.flexShrink = 0.0;
    }
    AdjustSpecialParamInLiteMode();
}

void DOMNode::CreateDeclaration(const std::string& tag)
{
    declaration_ = DeclarationCreatorManager::GetInstance().CreateDeclaration(tag);
    if (!declaration_) {
        LOGE("node %{public}s doesn't support", tag.c_str());
        return;
    }
    declaration_->SetCachePseudoClassStyle(
        [weak = WeakClaim(this)](const std::pair<std::string, std::string>& pseudoClassStyle) {
            auto node = weak.Upgrade();
            if (node) {
                node->CachePseudoClassStyle(pseudoClassStyle);
            }
        });
    declaration_->SetOnSetStyleFinished([weak = WeakClaim(this)]() {
        auto node = weak.Upgrade();
        if (node) {
            node->OnSetStyleFinished();
        }
    });
    declaration_->SetOnSetAttribute([weakComponent = WeakPtr<ComposedComponent>(rootComponent_)]() {
        auto rootComponent = weakComponent.Upgrade();
        if (rootComponent) {
            rootComponent->SetUpdateType(UpdateType::ALL);
        }
    });
}

} // namespace OHOS::Ace::Framework
