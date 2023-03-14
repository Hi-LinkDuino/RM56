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

#include "frameworks/bridge/common/dom/dom_list.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/list/list_theme.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_list_item.h"
#include "frameworks/bridge/common/dom/dom_list_item_group.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t DEFAULT_NODE_INDEX = -1;
constexpr char INDEXER_ALPHABET_DIV = 10; // newline character

} // namespace

DOMList::DOMList(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

bool DOMList::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(DOMList&, const std::string&)> attrOperators[] = {
        {
            DOM_LIST_ACCESSIBILITY_DISABLED,
            [](DOMList& list, const std::string& val) { list.accessibilityDisabled_ = StringToBool(val); },
        },
        {
            LIST_BEGIN_INDEX,
            [](DOMList& list, const std::string& val) { list.beginIndex_ = StringUtils::StringToInt(val); },
        },
        {
            LIST_CACHED_COUNT,
            [](DOMList& list, const std::string& val) { list.cachedCount_ = StringUtils::StringToInt(val); },
        },
        {
            DOM_LIST_CENTER_LAYOUT,
            [](DOMList& list, const std::string& val) { list.centerLayout_ = StringToBool(val); },
        },
        {
            DOM_LIST_CHAIN_ANIMATION,
            [](DOMList& list, const std::string& val) { list.chainAnimation_ = StringToBool(val); },
        },
        {
            DOM_LIST_DIVIDER,
            [](DOMList& list, const std::string& val) { list.needDivider_ = StringToBool(val); },
        },
        {
            LIST_END_INDEX,
            [](DOMList& list, const std::string& val) { list.endIndex_ = StringUtils::StringToInt(val); },
        },
        {
            DOM_LIST_INDEXER,
            [](DOMList& list, const std::string& val) { list.ParseIndexer(val); },
        },
        {
            DOM_LIST_INDEXER_BUBBLE,
            [](DOMList& list, const std::string& val) {
                list.bubble_.first = StringToBool(val);
                list.bubble_.second = true;
            },
        },
        {
            DOM_LIST_INDEXER_MODE,
            [](DOMList& list, const std::string& val) {
                list.circleMode_.first = StringToBool(val);
                list.circleMode_.second = true;
            },
        },
        {
            DOM_LIST_INDEXER_MULTI,
            [](DOMList& list, const std::string& val) {
              list.multiLanguage_.first = StringToBool(val);
              list.multiLanguage_.second = true;
            },
        },
        {
            LIST_INDEX_OFFSET,
            [](DOMList& list, const std::string& val) { list.indexOffset_ = StringUtils::StringToInt(val); },
        },
        {
            DOM_LIST_INITIAL_INDEX,
            [](DOMList& list, const std::string& val) { list.initialIndex_ = StringToInt(val); },
        },
        {
            DOM_LIST_INITIAL_OFFSET,
            [](DOMList& list, const std::string& val) { list.initialOffset_ = StringToDouble(val); },
        },
        {
            DOM_LIST_ITEM_CENTER,
            [](DOMList& list, const std::string& val) { list.itemCenter_ = StringToBool(val); },
        },
        {
            DOM_LIST_ITEM_OPACITY,
            [](DOMList& list, const std::string& val) { list.itemOpacity_ = StringToBool(val); },
        },
        {
            DOM_LIST_ITEM_SCALE,
            [](DOMList& list, const std::string& val) { list.itemScale_ = StringToBool(val); },
        },
        {
            LIST_REPEATED_LENGTH,
            [](DOMList& list, const std::string& val) { list.repeatedLength_ = StringUtils::StringToInt(val); },
        },
        {
            DOM_LIST_ROTATION_VIBRATE,
            [](DOMList& list, const std::string& val) {
#ifdef WEARABLE_PRODUCT
                list.rotationVibrate_ = StringToBool(val);
                list.scrollVibrate_ = false;
#endif
            },
        },
        {
            DOM_SCROLL_SCROLLBAR,
            [](DOMList& list, const std::string& val) {
                if (val == DOM_SCROLL_SCROLLBAR_ON) {
                    list.displayMode_ = DisplayMode::ON;
                } else if (val == DOM_SCROLL_SCROLLBAR_AUTO) {
                    list.displayMode_ = DisplayMode::AUTO;
                } else {
                    list.displayMode_ = DisplayMode::OFF;
                }
            },
        },
        {
            DOM_SCROLL_EFFECT,
            [](DOMList& list, const std::string& val) {
                if (val == DOM_SCROLL_EFFECT_SPRING) {
                    list.edgeEffect_ = EdgeEffect::SPRING;
                } else if (val == DOM_SCROLL_EFFECT_FADE) {
                    list.edgeEffect_ = EdgeEffect::FADE;
                } else {
                    list.edgeEffect_ = EdgeEffect::NONE;
                }
            },
        },
        {
            DOM_LIST_SCROLLPAGE,
            [](DOMList& list, const std::string& val) { list.scrollPage_ = StringToBool(val); },
        },
        {
            DOM_LIST_SCROLL_VIBRATE,
            [](DOMList& list, const std::string& val) { list.scrollVibrate_ = StringToBool(val); },
        },
        {
            DOM_LIST_ATTR_SELECTED,
            [](DOMList& list, const std::string& val) { list.selectedItem_ = val; },
        },
        {
            DOM_SCROLL_SHAPE_MODE,
            [](DOMList& list, const std::string& val) {
                if (val == DOM_SCROLL_SHAPE_MODE_RECT) {
                    list.shapeMode_ = ShapeMode::RECT;
                } else if (val == DOM_SCROLL_SHAPE_MODE_ROUND) {
                    list.shapeMode_ = ShapeMode::ROUND;
                } else {
                    list.shapeMode_ = ShapeMode::DEFAULT;
                }
            },
        },
        {
            DOM_LIST_UPDATE_EFFECT,
            [](DOMList& list, const std::string& val) { list.updateEffect_ = StringToBool(val); },
        },
    };
    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

void DOMList::ParseIndexer(const std::string& indexerAlphabet)
{
    indexerAlphabet_.clear();
    indexer_ = false;

    auto context = GetPipelineContext().Upgrade();
    if (context && context->IsJsCard()) {
        return;
    }

    if (indexerAlphabet.empty() || indexerAlphabet.find("false") != std::string::npos) {
        return;
    }

    if (indexerAlphabet.find("true") != std::string::npos) {
        indexer_ = true;
        return;
    }

    StringUtils::StringSpliter(indexerAlphabet, INDEXER_ALPHABET_DIV, indexerAlphabet_);
    int32_t alphabetCount = static_cast<int32_t>(indexerAlphabet_.size());
    indexer_ = alphabetCount > 0;
    LOGI("IndexerFlag:%{public}d, AlphabetCount:%{public}d.", indexer_, alphabetCount);
}

bool DOMList::SupportChainAnimation() const
{
    return chainAnimation_ && !indexer_;
}

bool DOMList::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(int32_t, DOMList&)> eventOperators[] = {
        {
            DOM_LIST_EVENT_INDEXER_CHANGE,
            [](int32_t pageId, DOMList& list) {
                list.onIndexerChange_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_INDEXER_CHANGE, pageId);
            },
        },
        {
            LIST_EVENT_REQUEST_ITEM,
            [](int32_t pageId, DOMList& list) {
                list.onRequestItem_ = EventMarker(list.GetNodeIdForEvent(), LIST_EVENT_REQUEST_ITEM, pageId);
            },
        },
        {
            DOM_LIST_EVENT_SCROLL,
            [](int32_t pageId, DOMList& list) {
                list.onScroll_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_SCROLL, pageId);
            },
        },
        {
            DOM_LIST_EVENT_SCROLL_BOTTOM,
            [](int32_t pageId, DOMList& list) {
                list.onScrollBottom_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_SCROLL_BOTTOM, pageId);
            },
        },
        {
            DOM_LIST_EVENT_SCROLL_END,
            [](int32_t pageId, DOMList& list) {
                list.onScrollEnd_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_SCROLL_END, pageId);
            },
        },
        {
            DOM_LIST_EVENT_SCROLL_TOP,
            [](int32_t pageId, DOMList& list) {
                list.onScrollTop_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_SCROLL_TOP, pageId);
            },
        },
        {
            DOM_LIST_EVENT_SCROLL_TOUCH_UP,
            [](int32_t pageId, DOMList& list) {
                list.onScrollTouchUp_ = EventMarker(list.GetNodeIdForEvent(), DOM_LIST_EVENT_SCROLL_TOUCH_UP, pageId);
            },
        },
    };
    auto iter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (iter != -1) {
        eventOperators[iter].value(pageId, *this);
        return true;
    }
    return false;
}

void DOMList::ResetInitializedStyle()
{
    if (!listComponent_) {
        LOGE("list component is null, reset style failed.");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    // list theme
    RefPtr<ListTheme> listTheme = GetTheme<ListTheme>();
    if (listTheme) {
        listComponent_->InitStyle(listTheme);
        if (declaration_) {
            auto& backgroundStyle =
                static_cast<CommonBackgroundStyle&>(declaration_->GetStyle(StyleTag::COMMON_BACKGROUND_STYLE));
            if (declaration_->HasBackGroundColor() && backgroundStyle.IsValid()) {
                listComponent_->SetBackgroundColor(backgroundStyle.backgroundColor);
            }
        }
        scrollDistance_ = listTheme->GetScrollDistance();
    }

    if (!scrollBar_ || displayMode_ == DisplayMode::OFF) {
        return;
    }
    // scrollBar_ theme
    RefPtr<ScrollBarTheme> scrollBarTheme = GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        if (scrollBar_->GetShapeMode() == ShapeMode::DEFAULT) {
            scrollBar_->SetShapeMode(scrollBarTheme->GetShapeMode());
        }
        scrollBar_->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar_->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar_->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar_->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar_->SetMinDynamicHeight(scrollBarTheme->GetMinDynamicHeight());
        if (scrollbarPositionY_.first == false) {
            scrollBar_->SetReservedHeight(scrollBarTheme->GetReservedHeight());
        } else {
            scrollBar_->SetReservedHeight(scrollbarPositionY_.second);
        }
        scrollBar_->SetTouchWidth(scrollBarTheme->GetTouchWidth());
        scrollBar_->SetBackgroundColor(scrollBarTheme->GetBackgroundColor());
        scrollBar_->SetForegroundColor(scrollBarTheme->GetForegroundColor());
        scrollBar_->SetPadding(scrollBarTheme->GetPadding());
    }
}

void DOMList::CreateOrUpdateList()
{
    if (!listComponent_) {
        listComponent_ = AceType::MakeRefPtr<ListComponent>();
    }

    listComponent_->SetScrollVibrate(scrollVibrate_);
    listComponent_->MarkNeedRotationVibrate(rotationVibrate_);

    listComponent_->SetDirection(flexDirection_);
    if (flexDirection_ == FlexDirection::COLUMN || flexDirection_ == FlexDirection::COLUMN_REVERSE) {
        listComponent_->SetScrollPage(scrollPage_);
        boxComponent_->SetScrollPage(scrollPage_);
    } else {
        listComponent_->SetScrollPage(false);
        boxComponent_->SetScrollPage(false);
    }
    listComponent_->SetChainProperty(chainProperty_);
    listComponent_->SetChainAnimation(SupportChainAnimation());
    if (useDefaultOverSpringProperty_) {
        if (SupportChainAnimation()) {
            listComponent_->SetOverSpringProperty(SpringChainProperty::GetDefaultOverSpringProperty());
        } else {
            listComponent_->SetOverSpringProperty(Scrollable::GetDefaultOverSpringProperty());
        }
    } else {
        listComponent_->SetOverSpringProperty(1.0, overStiffness_, overDamping_);
    }

    listComponent_->SetFlexAlign(crossAxisAlign_);
    listComponent_->SetRightToLeft(IsRightToLeft());
    listComponent_->SetOnRequestItem(onRequestItem_);
    listComponent_->SetOnScroll(onScroll_);
    listComponent_->SetOnScrollBottom(onScrollBottom_);
    listComponent_->SetOnScrollTop(onScrollTop_);
    listComponent_->SetOnScrollEnd(onScrollEnd_);
    listComponent_->SetOnScrollTouchUp(onScrollTouchUp_);
    if (cachedCount_ > 0) {
        listComponent_->SetCachedCount(cachedCount_);
    }
    listComponent_->SetBeginIndex(beginIndex_);
    listComponent_->SetEndIndex(endIndex_);
    listComponent_->SetRepeatedLength(repeatedLength_);
    listComponent_->SetIndexOffset(indexOffset_);
    listComponent_->SetColumnCount(listColumns_);
    listComponent_->SetItemExtent(itemExtent_);
    listComponent_->SetUpdateEffect(updateEffect_);
    listComponent_->SetAccessibilityDisabled(accessibilityDisabled_);
    if (listColumns_ > 1) {
        // itemScale is not supported in 'columns > 1' case.
        itemScale_ = false;
    }
    listComponent_->SetItemScale(itemScale_);
    listComponent_->MarkCenterLayout(centerLayout_);
    listComponent_->SetSupportItemCenter(itemCenter_);
    if (edgeEffect_ == EdgeEffect::SPRING) {
        listComponent_->SetScrollEffect(AceType::MakeRefPtr<ScrollSpringEffect>());
    } else if (edgeEffect_ == EdgeEffect::FADE) {
        listComponent_->SetScrollEffect(AceType::MakeRefPtr<ScrollFadeEffect>(fadeColor_));
    } else {
        listComponent_->SetScrollEffect(AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE));
    }

    // keep list state during update.
    if (!listComponent_->GetPositionController()) {
        listComponent_->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    }
    if (declaration_) {
        declaration_->SetPositionController(listComponent_->GetPositionController());
    }
    listComponent_->GetPositionController()->SetInitialIndex(initialIndex_);
    listComponent_->GetPositionController()->SetInitialOffset(initialOffset_);

    if (declaration_ && !GetRotateId().IsEmpty()) {
        listComponent_->SetOnRotateId(GetRotateId());
    }

    SetScrollBar();
    ResetInitializedStyle();
    InitScrollBarWithSpecializedStyle();
    SetChildActive();
}

void DOMList::CreateOrUpdateIndexer()
{
    CreateOrUpdateList();
    bool isCircle = circleMode_.second ? circleMode_.first : SystemProperties::GetDeviceType() == DeviceType::WATCH;
    bool bubble = bubble_.second ? bubble_.first : true;
    bool multiLanguage = multiLanguage_.second ? multiLanguage_.first : false;
    if (!indexerComponent_) {
        if (indexerAlphabet_.empty()) {
            indexerComponent_ = AceType::MakeRefPtr<IndexerListComponent>(
                listComponent_, isCircle, IsRightToLeft(), bubble, multiLanguage);
        } else {
            indexerComponent_ = AceType::MakeRefPtr<IndexerListComponent>(
                listComponent_, indexerAlphabet_, isCircle, IsRightToLeft(), bubble, multiLanguage);
        }
        if (isCircle && !onIndexerChange_.IsEmpty()) {
            indexerComponent_->SetIndexerChangeEvent(onIndexerChange_);
        }
    }
    indexerComponent_->SetBubbleEnabled(bubble_.first);
}

void DOMList::SetScrollBar()
{
    if (displayMode_ == DisplayMode::ON || displayMode_ == DisplayMode::AUTO) {
        if (shapeMode_ == ShapeMode::ROUND || shapeMode_ == ShapeMode::RECT) {
            scrollBar_ = AceType::MakeRefPtr<ScrollBar>(displayMode_, shapeMode_);
        } else {
            scrollBar_ = AceType::MakeRefPtr<ScrollBar>(displayMode_, ShapeMode::DEFAULT);
        }
        scrollBar_->SetPositionMode(IsRightToLeft() ? PositionMode::LEFT : PositionMode::RIGHT);
    } else {
        scrollBar_ = AceType::MakeRefPtr<ScrollBar>(DisplayMode::OFF, ShapeMode::DEFAULT);
    }
    listComponent_->SetScrollBar(scrollBar_);
}

void DOMList::InitScrollBarWithSpecializedStyle()
{
    if (!scrollBar_) {
        return;
    }

    if (scrollbarColor_.first) {
        scrollBar_->SetForegroundColor(scrollbarColor_.second);
    }
    if (scrollbarWidth_.first) {
        scrollBar_->SetInactiveWidth(scrollbarWidth_.second);
        scrollBar_->SetNormalWidth(scrollbarWidth_.second);
        scrollBar_->SetActiveWidth(scrollbarWidth_.second);
        scrollBar_->SetTouchWidth(scrollbarWidth_.second);
    }
    if (scrollbarPositionX_.first) {
        scrollBar_->SetPosition(scrollbarPositionX_.second);
    }
}

void DOMList::SetChildActive()
{
    if (selectedItem_.empty()) {
        return;
    }
    for (const auto& child : GetChildList()) {
        auto childItem = AceType::DynamicCast<DOMListItem>(child);
        if (!childItem) {
            continue;
        }
        auto listItem = AceType::DynamicCast<ListItemComponent>(childItem->GetSpecializedComponent());
        if (!listItem) {
            continue;
        }
        auto itemKey = childItem->GetItemKey();
        bool isItemActive = !itemKey.empty() && selectedItem_ == itemKey;
        if (listItem->IsActive() != isItemActive) {
            listItem->SetIsActive(isItemActive);
            childItem->MarkNeedUpdate();
        }
    }
}

bool DOMList::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<bool (*)(const std::string& val, DOMList& list)> styleOperators[] = {
        { DOM_ALIGN_ITEMS,
            [](const std::string& val, DOMList& list) {
              list.crossAxisAlign_ = ConvertStrToFlexAlign(val);
              return true;
            } },
        { DOM_BACKGROUND_COLOR,
            [](const std::string& val, DOMList& list) {
              // The list component uses backgroundColor as the foreground color.
              // The backgroundColor still needs to be set to the background color of the box component, so return
              // false.
              list.backgroundColor_ = list.ParseColor(val);
              return false;
            } },
        { DOM_LIST_COLUMNS,
            [](const std::string& val, DOMList& list) {
              list.listColumns_ = StringUtils::StringToInt(val);
              return true;
            } },
        { DOM_LIST_DIVIDER_COLOR,
            [](const std::string& val, DOMList& list) {
              list.dividerColor_ = list.ParseColor(val);
              return true;
            } },
        { DOM_LIST_DIVIDER_HEIGHT,
            [](const std::string& val, DOMList& list) {
              list.dividerHeight_ = list.ParseDimension(val);
              return true;
            } },
        { DOM_LIST_DIVIDER_LENGTH,
            [](const std::string& val, DOMList& list) {
              list.dividerLength_ = list.ParseDimension(val);
              return true;
            } },
        { DOM_LIST_DIVIDER_ORIGIN,
            [](const std::string& val, DOMList& list) {
              list.dividerOrigin_ = list.ParseDimension(val);
              return true;
            } },
        { DOM_FADE_COLOR,
            [](const std::string& val, DOMList& list) {
              list.fadeColor_ = list.ParseColor(val);
              return true;
            } },
        { DOM_FLEX_DIRECTION,
            [](const std::string& val, DOMList& list) {
                list.flexDirection_ = val == DOM_FLEX_ROW ? FlexDirection::ROW : FlexDirection::COLUMN;
                return true;
            } },
        { DOM_LIST_ITEM_EXTENT,
            [](const std::string& val, DOMList& list) {
                list.itemExtent_ = list.ParseDimension(val);
                return true;
            } },
        { DOM_SCROLL_OVER_SCROLL_EFFECT,
            [](const std::string& val, DOMList& list) {
                if (val == DOM_SCROLL_EFFECT_SPRING) {
                    list.edgeEffect_ = EdgeEffect::SPRING;
                } else if (val == DOM_SCROLL_EFFECT_FADE) {
                    list.edgeEffect_ = EdgeEffect::FADE;
                } else {
                    list.edgeEffect_ = EdgeEffect::NONE;
                }
                return true;
            } },
        { DOM_SCROLL_SCROLLBAR_COLOR,
            [](const std::string& val, DOMList& list) {
                list.scrollbarColor_.first = true;
                list.scrollbarColor_.second = list.ParseColor(val);
                return true;
            } },
        { DOM_SCROLL_SCROLLBAR_OFFSET,
            [](const std::string& val, DOMList& list) {
                std::vector<std::string> offset;
                OHOS::Ace::StringUtils::StringSpliter(val, ',', offset);
                list.scrollbarPositionX_.first = true;
                auto position = list.ParseDimension(offset[0]);
                list.scrollbarPositionX_.second = position.IsValid() ? position : Dimension();
                if (offset.size() > 1) {
                    list.scrollbarPositionY_.first = true;
                    auto positionY = list.ParseDimension(offset[1]);
                    list.scrollbarPositionY_.second = positionY.IsValid() ? positionY : Dimension();
                }
                return true;
            } },
        { DOM_SCROLL_SCROLLBAR_WIDTH,
            [](const std::string& val, DOMList& list) {
                list.scrollbarWidth_.first = true;
                auto width = list.ParseDimension(val);
                list.scrollbarWidth_.second = width.IsValid() ? width : Dimension();
                return true;
            } },
        { DOM_SCROLL_SCROLLBAR_POSITION,
            [](const std::string& val, DOMList& list) {
                list.scrollbarPositionX_.first = true;
                auto position = list.ParseDimension(val);
                list.scrollbarPositionX_.second = position.IsValid() ? position : Dimension();
                return true;
            } },
    };
    auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), style.first.c_str());
    if (operatorIter != -1) {
        return styleOperators[operatorIter].value(style.second, *this);
    }
    return false;
}

void DOMList::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    auto childListItem = AceType::DynamicCast<DOMListItem>(child);
    if (!childListItem) {
        LOGE("child is not list item, add to list failed.");
        return;
    }

    // childIndex is generated by js framework, just the position of this new list item should be added into the list.
    auto childIndex = childListItem->GetItemIndex();
    LOGD("DOMList AddChild %{public}s, childIndex: %{public}d", child->GetTag().c_str(), childIndex);
    if (childIndex != DEFAULT_NODE_INDEX) {
        if (indexer_) {
            needUpdateIds_ = true;
            indexerComponent_->InsertChild(childIndex, child->GetRootComponent());
        } else {
            listComponent_->InsertChild(childIndex, child->GetRootComponent());
        }
    } else {
        if (indexer_) {
            needUpdateIds_ = true;
            indexerComponent_->AppendChild(child->GetRootComponent());
        } else {
            listComponent_->AppendChild(child->GetRootComponent());
        }
    }
    auto item = AceType::DynamicCast<ListItemComponent>(childListItem->GetSpecializedComponent());
    if (!item) {
        return;
    }
    if (!selectedItem_.empty() && !childListItem->GetItemKey().empty() &&
        (selectedItem_ == childListItem->GetItemKey())) {
        item->SetIsActive(true);
    } else {
        item->SetIsActive(false);
    }
}

void DOMList::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_LIST_METHOD_SCROLL_TO) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
            LOGE("parse args error");
            return;
        }
        std::unique_ptr<JsonValue> indexValue = argsValue->GetArrayItem(0)->GetValue("index");
        if (!indexValue || !indexValue->IsNumber()) {
            LOGE("get index failed");
            return;
        }
        int32_t index = indexValue->GetInt();
        ScrollToMethod(index);
    } else if (method == DOM_LIST_METHOD_SCROLL_ARROW) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
            LOGE("parse args error");
            return;
        }
        std::unique_ptr<JsonValue> scrollArrowParams = argsValue->GetArrayItem(0);
        bool reverse = scrollArrowParams->GetBool("reverse", false);
        bool isSmooth = scrollArrowParams->GetBool("smooth", false);
        ScrollArrowMethod(reverse, isSmooth);
    } else if (method == DOM_LIST_METHOD_SCROLL_TOP || method == DOM_LIST_METHOD_SCROLL_BOTTOM) {
        ScrollToEdgeMethod(method, args);
    } else if (method == DOM_LIST_METHOD_SCROLL_PAGE) {
        ScrollPageMethod(method, args);
    } else if (method == DOM_LIST_METHOD_EXPAND_GROUP || method == DOM_LIST_METHOD_COLLAPSE_GROUP) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        std::string groupId;
        if (argsValue && argsValue->IsArray() && argsValue->GetArraySize() == 1) {
            std::unique_ptr<JsonValue> expandParams = argsValue->GetArrayItem(0);
            std::unique_ptr<JsonValue> value = expandParams->GetValue("groupid");
            if (value && value->IsString()) {
                groupId = value->GetString();
            }
        }
        if (method == DOM_LIST_METHOD_EXPAND_GROUP) {
            ExpandGroup(groupId, true);
        } else {
            ExpandGroup(groupId, false);
        }
    } else if (method == DOM_ROTATION) {
        auto controller = listComponent_->GetRotationController();
        if (controller) {
            LOGD("Rotation focus list request");
            controller->RequestRotation(true);
        }
    } else {
        LOGE("CallSpecializedMethod: undefined method :%{private}s", method.c_str());
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
    }
}

void DOMList::OnScrollBy(double dx, double dy, bool isSmooth)
{
    ScrollByMethod(dx, dy, isSmooth);
}

void DOMList::ExpandGroup(const std::string& groupId, bool expand)
{
    if (!listComponent_) {
        LOGE("DOMList ExpandGroup listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    if (groupId.empty()) {
        listComponent_->SetGroupState(INDEX_EXPAND_ALL, expand);
    }
    auto children = GetChildList();
    for (const auto& child : children) {
        auto itemGroup = AceType::DynamicCast<DOMListItemGroup>(child);
        if (!itemGroup) {
            continue;
        }
        if (groupId.empty() || groupId == itemGroup->GetGroupId()) {
            if (!groupId.empty()) {
                listComponent_->SetGroupState(itemGroup->GetItemIndex(), expand);
                itemGroup->Update();
                break;
            }
            itemGroup->Update();
        }
    }
}

void DOMList::ScrollToMethod(int32_t index)
{
    if (!listComponent_) {
        LOGE("DOMList ScrollToMethod listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    controller->JumpTo(index);
}

void DOMList::ScrollByMethod(double x, double y, bool isSmooth)
{
    if (!listComponent_) {
        LOGE("DOMList ScrollByMethod listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    controller->ScrollBy(x, y, isSmooth);
}

void DOMList::ScrollArrowMethod(bool reverse, bool isSmooth)
{
    if (!listComponent_) {
        LOGE("DOMList ScrollArrowMethod listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    controller->ScrollArrow(scrollDistance_, reverse, isSmooth);
}

void DOMList::ScrollToEdgeMethod(const std::string& method, const std::string& args)
{
    if (!listComponent_) {
        LOGE("DOMList ScrollToEdgeMethod listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }

    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
        LOGE("parse args error");
        return;
    }
    std::unique_ptr<JsonValue> params = argsValue->GetArrayItem(0);
    bool isSmooth = params->GetBool("smooth", false);
    if (method == DOM_LIST_METHOD_SCROLL_TOP) {
        controller->ScrollToEdge(ScrollEdgeType::SCROLL_TOP, isSmooth);
    } else {
        controller->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, isSmooth);
    }
}

void DOMList::ScrollPageMethod(const std::string& method, const std::string& args)
{
    if (!listComponent_) {
        LOGE("DOMList ScrollPageMethod listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return;
    }

    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != 1) {
        LOGE("parse args error");
        return;
    }
    std::unique_ptr<JsonValue> params = argsValue->GetArrayItem(0);
    bool reverse = params->GetBool("reverse", false);
    bool isSmooth = params->GetBool("smooth", false);
    controller->ScrollPage(reverse, isSmooth);
}

Offset DOMList::GetCurrentOffset() const
{
    if (!listComponent_) {
        LOGE("DOMList GetCurrentOffset listComponent_ is null");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return Offset::Zero();
    }
    auto controller = listComponent_->GetPositionController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::LIST_COMPONENT_ERR);
        return Offset::Zero();
    }
    return controller->GetCurrentOffset();
}

void DOMList::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!listComponent_ || !child) {
        LOGE("DOMList OnChildNodeRemoved listComponent_ is null");
        return;
    }
    LOGD("DOMList %{public}s, remove child %{public}s", GetTag().c_str(), child->GetTag().c_str());
    listComponent_->RemoveChild(child->GetRootComponent());
}

void DOMList::PrepareSpecializedComponent()
{
    if (indexer_) {
        CreateOrUpdateIndexer();
    } else {
        CreateOrUpdateList();
    }
}

void DOMList::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto parent = parentNode;
    while (parent) {
        if (parent->GetTag() == DOM_NODE_TAG_REFRESH) {
            listComponent_->SetInRefresh(true);
            break;
        }
        parent = parent->GetParentNode();
    }
}

RefPtr<AccessibilityNode> DOMList::GetAccessibilityNode()
{
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        return RefPtr<AccessibilityNode>();
    }
    auto accessibilityManager = pipelineContext->GetAccessibilityManager();
    if (!accessibilityManager) {
        return RefPtr<AccessibilityNode>();
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        return RefPtr<AccessibilityNode>();
    }

    return accessibilityNode;
}

void DOMList::UpdateAccessibilityOrder()
{
    auto accessibilityNode = GetAccessibilityNode();
    if (!accessibilityNode) {
        return;
    }

    if (listComponent_) {
        listComponent_->UpdateListItemIndex();
    }

    children_.sort([](const RefPtr<DOMNode>& node1, const RefPtr<DOMNode>& node2) {
        RefPtr<ListItemComponent> item1, item2;
        auto itemNode1 = DOMListItem::GetDOMListItem(node1);
        auto itemNode2 = DOMListItem::GetDOMListItem(node2);
        if (itemNode1) {
            item1 = itemNode1->GetListItemComponent();
        }
        if (itemNode2) {
            item2 = itemNode2->GetListItemComponent();
        }
        if (item1 && item2) {
            return item1->GetIndex() < item2->GetIndex();
        }
        return false;
    });

    std::list<RefPtr<AccessibilityNode>> children;
    auto nodes = accessibilityNode->GetChildList();
    for (const auto& child : children_) {
        auto item = DOMListItem::GetDOMListItem(child);
        if (item) {
            auto it = std::find_if(
                nodes.begin(), nodes.end(), [nodeId = item->GetNodeId()](const RefPtr<AccessibilityNode>& node) {
                    return node->GetNodeId() == nodeId;
                });
            if (it != nodes.end()) {
                children.emplace_back(*it);
            }
        }
    }
    if (!children.empty()) {
        accessibilityNode->ResetChildList(children);
    } else {
        LOGW("Update accessibility node order failed.");
    }
}

void DOMList::UpdateAccessibilityByVisible()
{
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto accessibilityManager = pipelineContext->GetAccessibilityManager();
    if (!accessibilityManager) {
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        return;
    }

    bool visibleRange = false;
    std::list<RefPtr<AccessibilityNode>> children;
    for (auto& child : children_) {
        auto childAccessibilityNode = accessibilityManager->GetAccessibilityNodeById(child->GetNodeId());
        if (childAccessibilityNode &&
            childAccessibilityNode->GetWidth() != 0 && childAccessibilityNode->GetHeight() != 0) {
            children.emplace_back(childAccessibilityNode);
            visibleRange = true;
        } else {
            if (visibleRange) {
                break; // Just load the visible range item.
            }
        }
    }
    if (!children.empty()) {
        accessibilityNode->ResetChildList(children);
    }
}

void DOMList::OnPageLoadFinish()
{
    if (listComponent_) {
        listComponent_->SetPageReady(true);
    }

    auto accessibilityNode = GetAccessibilityNode();
    if (!accessibilityNode) {
        return;
    }

    accessibilityNode->SetActionUpdateIdsImpl([weakList = AceType::WeakClaim(this), indexer = this->indexer_]() {
        auto list = weakList.Upgrade();
        if (list) {
            if (indexer && list->needUpdateIds_) {
                list->UpdateAccessibilityOrder();
                list->needUpdateIds_ = false;
            }

            list->UpdateAccessibilityByVisible();
        }
    });
}

void DOMList::AdjustSpecialParamInLiteMode()
{
    itemScale_ = false;
}

} // namespace OHOS::Ace::Framework
