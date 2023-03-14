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

#include "frameworks/bridge/common/dom/dom_clock.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/clock/clock_theme.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMClock::DOMClock(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    clockChild_ = AceType::MakeRefPtr<ClockComponent>();
}

void DOMClock::PrepareSpecializedComponent()
{
    const auto& declaration = AceType::DynamicCast<ClockDeclaration>(declaration_);
    if (!declaration) {
        LOGE("clock declaration is null!");
        return;
    }
    declaration->SetClockConfig(clockConfig_);
    clockChild_->SetDeclaration(declaration);
}

void DOMClock::InitializeStyle()
{
    auto declaration = AceType::DynamicCast<ClockDeclaration>(declaration_);
    if (!declaration) {
        LOGE("clock declaration is null!");
        return;
    }
    declaration->InitializeStyle();
}

void DOMClock::ResetInitializedStyle()
{
    InitializeStyle();
}

} // namespace OHOS::Ace::Framework