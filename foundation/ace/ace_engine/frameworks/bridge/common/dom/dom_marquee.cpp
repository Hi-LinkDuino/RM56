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

#include "frameworks/bridge/common/dom/dom_marquee.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/marquee/marquee_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMMarquee::DOMMarquee(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    marqueeChild_ = AceType::MakeRefPtr<MarqueeComponent>();
}

void DOMMarquee::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMMarquee::ResetInitializedStyle()
{
    RefPtr<MarqueeTheme> theme = GetTheme<MarqueeTheme>();
    // the marquee text lines is only one.
    textStyle_.SetMaxLines(1);
    if (theme) {
        textStyle_.SetFontSize(theme->GetFontSize());
        textStyle_.SetTextColor(theme->GetTextColor());
        marqueeChild_->SetTextStyle(textStyle_);
    }
}

void DOMMarquee::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    auto controller = marqueeChild_->GetController();
    if (!controller) {
        LOGE("get controller failed");
        EventReport::SendComponentException(ComponentExcepType::MARQUEE_ERR);
        return;
    }
    if (method == DOM_MARQUEE_METHOD_START) {
        controller->Start();
    } else if (method == DOM_MARQUEE_METHOD_STOP) {
        controller->Stop();
    } else {
        LOGE("Method not support: %{private}s", method.c_str());
    }
}

bool DOMMarquee::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(MarqueeComponent&, const std::string&)> attrOperators[] = {
        { DOM_MARQUEE_DIRECTION,
            [](MarqueeComponent& marquee, const std::string& val) {
                marquee.SetDirection(
                    val == DOM_MARQUEE_DIRECTION_RIGHT ? MarqueeDirection::RIGHT : MarqueeDirection::LEFT);
            } },
        { DOM_MARQUEE_LOOP,
            [](MarqueeComponent& marquee, const std::string& val) { marquee.SetLoop(StringToInt(val)); } },
        { DOM_MARQUEE_SCROLL_AMOUNT,
            [](MarqueeComponent& marquee, const std::string& val) { marquee.SetScrollAmount(StringToDouble(val)); } },
        { DOM_MARQUEE_VALUE, [](MarqueeComponent& marquee, const std::string& val) { marquee.SetValue(val); } },
    };
    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(*marqueeChild_, attr.second);
        return true;
    }
    return false;
}

bool DOMMarquee::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(TextStyle&, const std::string&, const DOMMarquee&)> textStyleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE, [](TextStyle& textStyle, const std::string& val,
                                const DOMMarquee&) { textStyle.SetAllowScale(StringToBool(val)); } },
        { DOM_MARQUEE_COLOR, [](TextStyle& textStyle, const std::string& val,
                             const DOMMarquee& node) { textStyle.SetTextColor(node.ParseColor(val)); } },
        { DOM_MARQUEE_FONT_FAMILY,
            [](TextStyle& textStyle, const std::string& val, const DOMMarquee& node) {
                textStyle.SetFontFamilies(node.ParseFontFamilies(val));
            } },
        { DOM_MARQUEE_FONT_SIZE, [](TextStyle& textStyle, const std::string& val,
                                 const DOMMarquee& node) { textStyle.SetFontSize(node.ParseDimension(val)); } },
        { DOM_MARQUEE_FONT_WEIGHT, [](TextStyle& textStyle, const std::string& val,
                                   const DOMMarquee&) { textStyle.SetFontWeight(ConvertStrToFontWeight(val)); } },
    };
    auto operatorIter = BinarySearchFindIndex(textStyleOperators, ArraySize(textStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        textStyleOperators[operatorIter].value(textStyle_, style.second, *this);
        marqueeChild_->SetTextStyle(textStyle_);
        return true;
    }
    return false;
}

bool DOMMarquee::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_MARQUEE_EVENT_BOUNCE) {
        marqueeChild_->SetBounceEventId(EventMarker(GetNodeIdForEvent(), event, pageId));
    } else if (event == DOM_MARQUEE_EVENT_FINISH) {
        marqueeChild_->SetFinishEventId(EventMarker(GetNodeIdForEvent(), event, pageId));
    } else if (event == DOM_MARQUEE_EVENT_START) {
        marqueeChild_->SetStartEventId(EventMarker(GetNodeIdForEvent(), event, pageId));
    } else {
        return false;
    }
    return true;
}

void DOMMarquee::PrepareSpecializedComponent() {}

} // namespace OHOS::Ace::Framework