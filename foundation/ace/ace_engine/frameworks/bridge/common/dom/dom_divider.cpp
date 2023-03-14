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

#include "frameworks/bridge/common/dom/dom_divider.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/divider/divider_theme.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMDivider::DOMDivider(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    dividerChild_ = AceType::MakeRefPtr<DividerComponent>();
}

void DOMDivider::InitializeStyle()
{
    auto theme = GetTheme<DividerTheme>();
    if (theme) {
        dividerChild_->SetDividerColor(theme->GetColor());
    }
}

bool DOMDivider::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMDivider&)> driverStyleOperators[] = {
        { DOM_COLOR, [](const std::string& val,
                     DOMDivider& divider) { divider.dividerChild_->SetDividerColor(divider.ParseColor(val)); } },
        { DOM_DIVIDER_LINE_CAP,
            [](const std::string& val, DOMDivider& divider) {
                if (val == "butt") {
                    divider.dividerChild_->SetLineCap(LineCap::BUTT);
                } else if (val == "round") {
                    divider.dividerChild_->SetLineCap(LineCap::ROUND);
                } else if (val == "square") {
                    divider.dividerChild_->SetLineCap(LineCap::SQUARE);
                } else {
                    LOGE("not support value: %{public}s", val.c_str());
                }
            } },
        { DOM_DIVIDER_STROKE_WIDTH,
            [](const std::string& val, DOMDivider& divider) {
                divider.dividerChild_->SetStrokeWidth(divider.ParseDimension(val));
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(driverStyleOperators, ArraySize(driverStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        driverStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

void DOMDivider::PrepareSpecializedComponent()
{
    if (dividerChild_->IsVertical()) {
        boxComponent_->SetFlex(BoxFlex::FLEX_Y);
    } else {
        boxComponent_->SetFlex(BoxFlex::FLEX_X);
    }
}

bool DOMDivider::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_DIVIDER_VERTICAL) {
        dividerChild_->SetVertical(StringToBool(attr.second));
        return true;
    }
    return false;
}

void DOMDivider::ResetInitializedStyle()
{
    InitializeStyle();
}

} // namespace OHOS::Ace::Framework