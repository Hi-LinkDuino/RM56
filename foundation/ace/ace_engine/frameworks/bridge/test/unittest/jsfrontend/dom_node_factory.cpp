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

#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

#include <map>

#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_mock.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_test_constants.h"

namespace OHOS::Ace::Framework {
namespace {

const std::map<std::string, const std::vector<std::string>&> DOMSTYLELIST = {
    { DOM_NODE_TAG_DIV, DIV_STYLE_LIST },
    { DOM_NODE_TAG_TEXT, TEXT_STYLE_LIST },
    { DOM_NODE_TAG_PROGRESS, PROGRESS_STYLE_LIST },
    { DOM_NODE_TAG_SLIDER, SLIDER_STYLE_LIST },
    { DOM_NODE_TAG_SWIPER, SWIPER_STYLE_LIST },
    { DOM_NODE_TAG_REFRESH, REFRESH_STYLE_LIST },
    { DOM_NODE_TAG_IMAGE, IMAGE_STYLE_LIST },
    { DOM_NODE_TAG_SPAN, SPAN_STYLE_LIST },
    { DOM_NODE_TAG_VIDEO, VIDEO_STYLE_LIST },
    { DOM_NODE_TAG_RATING, RATING_STYLE_LIST },
    { DOM_NODE_TAG_MARQUEE, MARQUEE_STYLE_LIST },
    { DOM_NODE_TAG_TEXTAREA, TEXTAREA_STYLE_LIST },
    { DOM_NODE_TAG_POPUP, POPUP_STYLE_LIST },
    { DOM_NODE_TAG_LIST, LIST_STYLE_LIST },
    { DOM_NODE_TAG_INPUT, INPUT_STYLE_LIST },
    { DOM_NODE_TAG_LABEL, TEXT_STYLE_LIST },
    { DOM_NODE_TAG_DIVIDER, DIVIDER_STYLE_LIST },
    { DOM_NODE_TAG_SWITCH, SWITCH_STYLE_LIST },
    { DOM_NODE_TAG_BUTTON, BUTTON_STYLE_LIST },
    { DOM_NODE_TAG_SEARCH, SEARCH_STYLE_LIST },
};

const std::map<std::string, std::vector<std::string>> DOMATTRLIST = {
    { DOM_NODE_TAG_PROGRESS, PROGRESS_ATTR_LIST },
    { DOM_NODE_TAG_SLIDER, SLIDER_ATTR_LIST },
    { DOM_NODE_TAG_SWIPER, SWIPER_ATTR_LIST },
    { DOM_NODE_TAG_SWITCH, SWITCH_ATTR_LIST },
    { DOM_NODE_TAG_TEXT, TEXT_ATTR_LIST },
    { DOM_NODE_TAG_SVG_TEXT, SVG_TEXT_ATTR_LIST },
    { DOM_NODE_TAG_TEXT_PATH, SVG_TEXT_PATH_ATTR_LIST },
    { DOM_NODE_TAG_TSPAN, SVG_TSPAN_ATTR_LIST },
    { DOM_NODE_TAG_IMAGE, IMAGE_ATTR_LIST },
    { DOM_NODE_TAG_SPAN, SPAN_ATTR_LIST },
    { DOM_NODE_TAG_TABS, TABS_ATTR_LIST },
    { DOM_NODE_TAG_TAB_BAR, TABBAR_ATTR_LIST },
    { DOM_NODE_TAG_TAB_CONTENT, TABCONTENT_ATTR_LIST },
    { DOM_NODE_TAG_REFRESH, REFRESH_ATTR_LIST },
    { DOM_NODE_TAG_VIDEO, VIDEO_ATTR_LIST },
    { DOM_NODE_TAG_RATING, RATING_ATTR_LIST },
    { DOM_NODE_TAG_MARQUEE, MARQUEE_ATTR_LIST },
    { DOM_NODE_TAG_TEXTAREA, TEXTAREA_ATTR_LIST },
    { DOM_NODE_TAG_POPUP, POPUP_ATTR_LIST },
    { DOM_NODE_TAG_LIST, LIST_ATTR_LIST },
    { DOM_NODE_TAG_INPUT, INPUT_ATTR_LIST },
    { DOM_NODE_TAG_LABEL, LABEL_ATTR_LIST },
    { DOM_NODE_TAG_BUTTON, BUTTON_ATTR_LIST },
    { DOM_NODE_TAG_DIVIDER, DIVIDER_ATTR_LIST },
    { DOM_NODE_TAG_SEARCH, SEARCH_ATTR_LIST },
    { DOM_NODE_TAG_IMAGE_ANIMATOR, IMAGE_ANIMATOR_ATTR_LIST },
    { DOM_NODE_TAG_PIECE, PIECE_ATTR_LIST },
    { DOM_NODE_TAG_DIV, DIV_ATTR_LIST },
    { DOM_NODE_TAG_SVG, SVG_ATTR_LIST },
    { DOM_NODE_TAG_RECT, SVG_RECT_ATTR_LIST },
    { DOM_NODE_TAG_CIRCLE, SVG_CIRCLE_ATTR_LIST },
    { DOM_NODE_TAG_ELLIPSE, SVG_ELLIPSE_ATTR_LIST },
    { DOM_NODE_TAG_PATH, SVG_PATH_ATTR_LIST },
    { DOM_NODE_TAG_LINE, SVG_LINE_ATTR_LIST },
    { DOM_NODE_TAG_POLYLINE, SVG_POLYLINE_ATTR_LIST },
    { DOM_NODE_TAG_POLYGON, SVG_POLYGON_ATTR_LIST },
    { DOM_NODE_TAG_ANIMATE, SVG_ANIMATE_ATTR_LIST },
    { DOM_NODE_TAG_ANIMATE_MOTION, SVG_ANIMATE_MOTION_ATTR_LIST },
};
const std::map<std::string, std::vector<std::string>> DOMEVENTLIST = {
    { DOM_NODE_TAG_SWIPER, SWIPER_EVENT_LIST },
    { DOM_NODE_TAG_SLIDER, SLIDER_EVENT_LIST },
    { DOM_NODE_TAG_SWITCH, SWITCH_EVENT_LIST },
    { DOM_NODE_TAG_IMAGE, IMAGE_EVENT_LIST },
    { DOM_NODE_TAG_VIDEO, VIDEO_EVENT_LIST },
    { DOM_NODE_TAG_REFRESH, REFRESH_EVENT_LIST },
    { DOM_NODE_TAG_RATING, RATING_EVENT_LIST },
    { DOM_NODE_TAG_MARQUEE, MARQUEE_EVENT_LIST },
    { DOM_NODE_TAG_TEXTAREA, TEXTAREA_EVENT_LIST },
    { DOM_NODE_TAG_POPUP, POPUP_EVENT_LIST },
    { DOM_NODE_TAG_INPUT, INPUT_EVENT_LIST },
    { DOM_NODE_TAG_SEARCH, SEARCH_EVENT_LIST },
    { DOM_NODE_TAG_PIECE, PIECE_EVENT_LIST },
};

std::unique_ptr<JsonValue> ParseJsonString(const std::string& jsonValue)
{
    return JsonUtil::ParseJsonString(jsonValue);
}

RefPtr<DOMNode> CreateDomNode(const JsonValue& componentValue, int32_t nodeId)
{
    if (!componentValue.Contains(DOM_TAG)) {
        LOGE("not have tag field.");
        return nullptr;
    }

    auto typeValue = componentValue.GetValue(DOM_TAG);
    std::string type = typeValue->GetString();
    return DOMNodeFactory::GetInstance().GetDocument()->CreateNodeWithId(type, nodeId);
}

void AppendChildNode(const JsonValue& jsonValue, const RefPtr<DOMNode>& parentNode)
{
    NodeId nodeId = parentNode->GetNodeId() + 1;
    if (!jsonValue.IsArray()) {
        auto child = CreateDomNode(jsonValue, nodeId);
        parentNode->AddNode(child);
        return;
    }

    int32_t arraySize = jsonValue.GetArraySize();
    for (int32_t index = 0; index < arraySize; ++index, nodeId++) {
        auto item = jsonValue.GetArrayItem(index);
        auto child = CreateDomNode(*item, nodeId);
        parentNode->AddNode(child);
        if (jsonValue.Contains(CHILD_TAG)) {
            const auto childValue = jsonValue.GetValue(CHILD_TAG);
            if (childValue != nullptr) {
                AppendChildNode(*childValue, child);
            }
        }
    }
}

void SetStyleFromJson(
    const std::string& jsonStr, const RefPtr<DOMNode>& domNode, const RefPtr<Declaration>& declaration)
{
    if (!declaration || !domNode) {
        return;
    }
    auto domJson = ParseJsonString(jsonStr);
    std::vector<std::pair<std::string, std::string>> styles;
    if (domJson->Contains(COMMON_STYLE)) {
        const auto commonStyleValue = domJson->GetValue(COMMON_STYLE);
        int32_t size = commonStyleValue->GetArraySize();
        for (int32_t index = 0; index < size; index++) {
            auto item = commonStyleValue->GetArrayItem(index);
            for (const auto& commonStyle : COMMON_STYLE_LIST) {
                if (item->Contains(commonStyle)) {
                    styles.emplace_back(commonStyle, item->GetValue(commonStyle)->GetString());
                    break;
                }
            }
        }
    }
    std::string type = domJson->GetValue(DOM_TAG)->GetString();
    if (DOMSTYLELIST.find(type) == DOMSTYLELIST.end()) {
        LOGD("json file not define style for %s", type.c_str());
    } else {
        const std::vector<std::string> styleList = DOMSTYLELIST.find(type)->second;
        if (domJson->Contains(DOM_STYLE)) {
            const auto styleValue = domJson->GetValue(DOM_STYLE);
            int32_t size = styleValue->GetArraySize();
            for (int32_t index = 0; index < size; index++) {
                auto item = styleValue->GetArrayItem(index);
                for (const auto& style : styleList) {
                    if (item->Contains(style)) {
                        styles.emplace_back(style, item->GetValue(style)->GetString());
                        break;
                    }
                }
            }
        }
    }
    declaration->SetStyle(styles);
    domNode->SetStyle(styles);
}

void SetAttrFromJson(const std::string& jsonStr, const RefPtr<DOMNode>& domNode, const RefPtr<Declaration>& declaration)
{
    if (!declaration || !domNode) {
        return;
    }
    auto domJson = ParseJsonString(jsonStr);
    std::string type = domJson->GetValue(DOM_TAG)->GetString();
    if (DOMATTRLIST.find(type) == DOMATTRLIST.end()) {
        LOGD("json file not define attribute for %s", type.c_str());
        return;
    }
    const std::vector<std::string> attrList = DOMATTRLIST.find(type)->second;
    std::vector<std::pair<std::string, std::string>> attrs;
    if (domJson->Contains(DOM_ATTR)) {
        const auto attrValue = domJson->GetValue(DOM_ATTR);
        int32_t size = attrValue->GetArraySize();
        for (int32_t index = 0; index < size; index++) {
            auto item = attrValue->GetArrayItem(index);
            for (const auto& attr : attrList) {
                if (item->Contains(attr)) {
                    attrs.emplace_back(attr, item->GetValue(attr)->GetString());
                    break;
                }
            }
        }
    }
    declaration->SetAttr(attrs);
    domNode->SetAttr(attrs);
}

void SetEventFromJson(
    const std::string& jsonStr, const RefPtr<DOMNode>& domNode, const RefPtr<Declaration>& declaration)
{
    if (!declaration || !domNode) {
        return;
    }
    auto domJson = ParseJsonString(jsonStr);
    std::vector<std::string> events;
    if (domJson->Contains(COMMON_EVENT)) {
        const auto commonEventValue = domJson->GetValue(COMMON_EVENT);
        int32_t size = commonEventValue->GetArraySize();
        for (int32_t index = 0; index < size; index++) {
            auto itemStr = commonEventValue->GetArrayItem(index)->GetString();
            auto eventIter = std::find_if(std::begin(COMMON_EVENT_LIST), std::end(COMMON_EVENT_LIST),
                [itemStr](const std::string& commonEvent) { return itemStr == commonEvent; });
            if (eventIter != std::end(COMMON_EVENT_LIST)) {
                events.emplace_back(*eventIter);
            }
        }
    }
    std::string type = domJson->GetValue(DOM_TAG)->GetString();
    if (DOMEVENTLIST.find(type) == DOMEVENTLIST.end()) {
        LOGD("json file not define event for %s", type.c_str());
    } else {
        const std::vector<std::string> eventList = DOMEVENTLIST.find(type)->second;
        if (domJson->Contains(DOM_EVENT)) {
            const auto eventValue = domJson->GetValue(DOM_EVENT);
            int32_t size = eventValue->GetArraySize();
            for (int32_t index = 0; index < size; index++) {
                auto item = eventValue->GetArrayItem(index);
                if (!item->IsString()) {
                    continue;
                }

                auto itemStr = item->GetString();
                auto eventIter = std::find_if(std::begin(eventList), std::end(eventList),
                    [itemStr](const std::string& event) { return itemStr == event; });
                if (eventIter != std::end(COMMON_EVENT_LIST)) {
                    events.emplace_back(*eventIter);
                }
            }
        }
    }
    declaration->AddEvent(PAGE_ID, domNode->GetNodeIdForEvent(), events);
    domNode->AddEvent(PAGE_ID, events);
}

} // namespace

DOMNodeFactory::DOMNodeFactory() = default;
DOMNodeFactory::~DOMNodeFactory() = default;

RefPtr<DOMNode> DOMNodeFactory::CreateDOMNodeFromDsl(const std::string& jsonStr)
{
    auto domJson = ParseJsonString(jsonStr);
    auto typeValue = domJson->GetValue(DOM_TAG);
    std::string type = typeValue->GetString();
    document_ = AceType::MakeRefPtr<DOMDocument>(DOM_ROOT_NODE_ID_BASE);
    RefPtr<DOMNode> domNodeRoot = document_->CreateNodeWithId(type, DOM_ROOT_NODE_ID_BASE);

    if (!context_) {
        auto frontend = AceType::MakeRefPtr<MockFrontend>();
        auto platformWindow = std::make_unique<MockPlatformWindow>();
        auto window = std::make_unique<Window>(std::move(platformWindow));
        context_ = AceType::MakeRefPtr<PipelineContext>(std::move(window), nullptr, nullptr, nullptr, frontend, 0);
        context_->SetThemeManager(AceType::MakeRefPtr<ThemeManager>());
    }

    domNodeRoot->SetPipelineContext(context_);
    domNodeRoot->InitializeStyle();
    auto declaration = domNodeRoot->GetDeclaration();
    if (declaration) {
        declaration->BindPipelineContext(context_);
        declaration->InitializeStyle();
    }
    SetAttrFromJson(jsonStr, domNodeRoot, declaration);
    SetStyleFromJson(jsonStr, domNodeRoot, declaration);
    SetEventFromJson(jsonStr, domNodeRoot, declaration);
    domNodeRoot->GenerateComponentNode();

    if (domJson->Contains(CHILD_TAG)) {
        const auto childValue = domJson->GetValue(CHILD_TAG);
        if (childValue != nullptr) {
            AppendChildNode(*childValue, domNodeRoot);
        }
    }
    return domNodeRoot;
}

RefPtr<BoxComponent> DOMNodeFactory::GetBoxChildComponent(const RefPtr<DOMNode>& domNode)
{
    RefPtr<BoxComponent> boxComponent = domNode->GetBoxComponent();
    return boxComponent;
}

} // namespace OHOS::Ace::Framework
