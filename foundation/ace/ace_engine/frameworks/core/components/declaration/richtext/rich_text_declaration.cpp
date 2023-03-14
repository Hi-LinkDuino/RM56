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

#include "core/components/declaration/richtext/rich_text_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void RichTextDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_RICH_TEXT_ATTR);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_RICH_TEXT_EVENT);
}

bool RichTextDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(RichTextDeclaration&, const std::string&)> richTextAttrOperators[] = {
        { DOM_RICH_TEXT_DATA,
          [](RichTextDeclaration& declaration, const std::string& data) { declaration.SetData(data); }
        },
    };
    auto item = BinarySearchFindIndex(richTextAttrOperators, ArraySize(richTextAttrOperators), attr.first.c_str());
    if (item != -1) {
        richTextAttrOperators[item].value(*this, attr.second);
        return true;
    }
    return false;
}

bool RichTextDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    static const LinearMapNode<void (*)(RichTextDeclaration&, const EventMarker&)> eventOperators[] = {
        { DOM_LOAD_COMPLETE, [](RichTextDeclaration& declaration, const EventMarker& event) {
            auto& richTextEvent = declaration.MaybeResetEvent<RichTextEvent>(EventTag::SPECIALIZED_EVENT);
            if (richTextEvent.IsValid()) {
                richTextEvent.pageFinishEventId = event;
            }}
        },
        { DOM_LOAD_START, [](RichTextDeclaration& declaration, const EventMarker& event) {
            auto& richTextEvent = declaration.MaybeResetEvent<RichTextEvent>(EventTag::SPECIALIZED_EVENT);
            if (richTextEvent.IsValid()) {
                richTextEvent.pageStartEventId = event;
            }}
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
