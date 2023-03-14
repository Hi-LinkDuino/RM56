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

#include "frameworks/bridge/card_frontend/card_frontend_delegate.h"

#include <string>

#include "base/log/event_report.h"
#include "core/common/thread_checker.h"

namespace OHOS::Ace::Framework {

CardFrontendDelegate::CardFrontendDelegate() : jsAccessibilityManager_(AccessibilityNodeManager::Create()) {}

CardFrontendDelegate::~CardFrontendDelegate()
{
    CHECK_RUN_ON(JS);
    LOG_DESTROY();
}

void CardFrontendDelegate::FireCardEvent(const EventMarker& eventMarker, const std::string& params)
{
    if (!page_) {
        LOGE("FireCardEvent failed due to page is null");
        return;
    }
    auto context = page_->GetPipelineContext().Upgrade();
    if (!context) {
        LOGE("FireCardEvent get pipeline context failed");
        EventReport::SendFormException(FormExcepType::FIRE_FORM_EVENT_ERR);
        return;
    }
    auto nodeId = eventMarker.GetData().eventId == "_root" ? DOM_ROOT_NODE_ID_BASE + page_->GetPageId()
                                                           : StringToInt(eventMarker.GetData().eventId);
    auto action = page_->GetNodeEventAction(nodeId, eventMarker.GetData().eventType);
    auto node = page_->GetDomDocument()->GetDOMNodeById(nodeId);
    if (!node) {
        LOGE("FireCardEvent get node failed");
        EventReport::SendFormException(FormExcepType::FIRE_FORM_EVENT_ERR);
        return;
    }

    auto routerAction = JsonUtil::ParseJsonString(action);
    auto paramsJson = JsonUtil::ParseJsonString(params);
    if (!routerAction->IsValid()) {
        LOGE("FireCardEvent router action is null");
        return;
    }
    auto paramsInfo = routerAction->GetValue("params");
    if (paramsInfo->IsValid()) {
        auto child = paramsInfo->GetChild();
        while (child->IsValid()) {
            auto key = child->GetKey();
            auto value = child->GetString();
            child = child->GetNext();
            if (Framework::StartWith(value, "$event.")) {
                paramsInfo->Replace(key.c_str(), paramsJson->GetValue(value.substr(7)));
            }
        }
    }
    action = routerAction->ToString();
    context->OnActionEvent(action);

    // send event to accessibility
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = nodeId;
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    context->SendEventToAccessibility(accessibilityEvent);
}

RefPtr<JsAcePage>& CardFrontendDelegate::CreatePage(int32_t pageId, const std::string& url)
{
    if (!page_) {
        auto document = AceType::MakeRefPtr<DOMDocument>(pageId);
        page_ = AceType::MakeRefPtr<Framework::JsAcePage>(pageId, document, url);
    }
    return page_;
}

RefPtr<JsAcePage>& CardFrontendDelegate::CreatePage(
    int32_t pageId, const std::string& url, const WeakPtr<StageElement>& container)
{
    if (!page_) {
        auto document = AceType::MakeRefPtr<DOMDocument>(pageId);
        page_ = AceType::MakeRefPtr<Framework::JsAcePage>(pageId, document, url, container);
    }
    return page_;
}

RefPtr<JsAcePage>& CardFrontendDelegate::GetPage()
{
    return page_;
}

} // namespace OHOS::Ace::Framework
