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

#include "frameworks/bridge/common/dom/dom_web.h"

#include "base/log/log.h"
#include "core/components/declaration/web/web_declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMWeb::DOMWeb(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    webChild_ = AceType::MakeRefPtr<WebComponent>(nodeName);
}

void DOMWeb::PrepareSpecializedComponent()
{
    auto webDeclaration = AceType::DynamicCast<WebDeclaration>(declaration_);
    if (!webDeclaration) {
        return;
    }
    webChild_->SetDeclaration(webDeclaration);
}

} // namespace OHOS::Ace::Framework
