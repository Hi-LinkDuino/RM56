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

#include "core/components/declaration/web/web_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void WebDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_WEB_ATTR);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_WEB_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_WEB_METHOD);
}

bool WebDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(WebDeclaration&, const std::string&)> webAttrOperators[] = {
                { DOM_WEB_WEBSRC, [](WebDeclaration& declaration, const std::string& src) {
                    declaration.SetWebSrc(src);
                } },
    };
    auto operatorIter = BinarySearchFindIndex(webAttrOperators, ArraySize(webAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        webAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool WebDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    static const LinearMapNode<void (*)(WebDeclaration&, const EventMarker&)> eventOperators[] = {
            { DOM_PAGEERROR,
                [](WebDeclaration& declaration, const EventMarker& event) {
                    auto& webEvent = declaration.MaybeResetEvent<WebEvent>(EventTag::SPECIALIZED_EVENT);
                    if (webEvent.IsValid()) {
                        webEvent.pageErrorEventId = event;
                    }
            } },
            { DOM_WEB_MESSAGE,
                [](WebDeclaration& declaration, const EventMarker& event) {
                    auto& webEvent = declaration.MaybeResetEvent<WebEvent>(EventTag::SPECIALIZED_EVENT);
                    if (webEvent.IsValid()) {
                        webEvent.messageEventId = event;
                    }
            } },
            { DOM_PAGEFINISH,
                [](WebDeclaration& declaration, const EventMarker& event) {
                    auto& webEvent = declaration.MaybeResetEvent<WebEvent>(EventTag::SPECIALIZED_EVENT);
                    if (webEvent.IsValid()) {
                        webEvent.pageFinishEventId = event;
                    }
            } },
            { DOM_PAGESTART,
                [](WebDeclaration& declaration, const EventMarker& event) {
                    auto& webEvent = declaration.MaybeResetEvent<WebEvent>(EventTag::SPECIALIZED_EVENT);
                    if (webEvent.IsValid()) {
                        webEvent.pageStartEventId = event;
                    }
            } },
    };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(*this, EventMarker(eventId, event, pageId));
        return true;
    }
    return false;
}

void WebDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    auto& webMethod = static_cast<WebMethod&>(GetMethod(MethodTag::SPECIALIZED_METHOD));
    if (!webMethod.IsValid()) {
        LOGW("method of web is null");
        return;
    }

    // Operator map for method
    static const std::unordered_map<std::string,
            void (*)(WebMethod&, const std::string&)>
            methedOperators = {
            { DOM_METHOD_RELOAD, [](WebMethod& webMethod, const std::string& args) {
                webMethod.Reload();
            } },
    };
    auto operatorIter = methedOperators.find(method);
    if (operatorIter != methedOperators.end()) {
        operatorIter->second(webMethod, args);
    }
}

} // namespace OHOS::Ace
