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

#include "core/components/declaration/xcomponent/xcomponent_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {
using namespace Framework;

void XComponentDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_XCOMPONENT_ATTR);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_XCOMPONENT_EVENT);
}

bool XComponentDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(XComponentDeclaration&, const std::string&)> xComponentAttrOperators[] = {
        {
            DOM_XCOMPONENT_LIBRARYNAME,
            [](XComponentDeclaration& declaration, const std::string& libraryName) {
                declaration.SetXComponentLibraryName(libraryName);
            }
        },
        {
            DOM_XCOMPONENT_NAME,
            [](XComponentDeclaration& declaration, const std::string& name) {
                declaration.SetXComponentName(name);
            }
        },
        {
            DOM_XCOMPONENT_TYPE,
            [](XComponentDeclaration& declaration, const std::string& type) {
                declaration.SetXComponentType(type);
            }
        },
    };
    auto operatorIter = BinarySearchFindIndex(xComponentAttrOperators,
                                              ArraySize(xComponentAttrOperators),
                                              attr.first.c_str());
    if (operatorIter != -1) {
        xComponentAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}


bool XComponentDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    static const LinearMapNode<void (*)(XComponentDeclaration&, const EventMarker&)> eventOperators[] = {
        {
            DOM_XCOMPONENT_DESTROY,
            [](XComponentDeclaration& declaration, const EventMarker& event) {
                auto& xComponentEvent = declaration.MaybeResetEvent<XComponentEvent>(EventTag::SPECIALIZED_EVENT);
                if (xComponentEvent.IsValid()) {
                    xComponentEvent.xComponentDestroyEventId = event;
                }
            }
        },
        {
            DOM_XCOMPONENT_INIT,
            [](XComponentDeclaration& declaration, const EventMarker& event) {
                auto& xComponentEvent = declaration.MaybeResetEvent<XComponentEvent>(EventTag::SPECIALIZED_EVENT);
                if (xComponentEvent.IsValid()) {
                    xComponentEvent.xComponentInitEventId = event;
                }
            }
        },
    };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(*this, EventMarker(eventId, event, pageId));
        return true;
    }
    return false;
}
} // namespace OHOS::Ace
