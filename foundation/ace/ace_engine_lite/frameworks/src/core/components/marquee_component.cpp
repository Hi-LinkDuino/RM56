/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "marquee_component.h"
#include "ace_log.h"
#include "js_app_context.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
static constexpr uint16_t MILLISECONDS_PER_SECOND = 1000;

MarqueeComponent::MarqueeComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : TextComponent(options, children, styleManager)
{
    SetComponentName(K_MARQUEE);
}

bool MarqueeComponent::CreateNativeViews()
{
    if (TextComponent::CreateNativeViews()) {
        UILabelTypeWrapper *uiLabel = TextComponent::GetUILabelView();
        if (uiLabel != nullptr) {
            // default mode, speed
            uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
            SetRollSpeed();
            return true;
        }
    }

    return false;
}

bool MarqueeComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    bool isSuccess = TextComponent::SetPrivateAttribute(attrKeyId, attrValue);
    if (!isSuccess) {
        switch (attrKeyId) {
            case K_SCROLLAMOUNT: {
                SetScrollamount(IntegerOf(attrValue));
                break;
            }
            case K_SCROLLDELAY: {
                SetScrolldelay(IntegerOf(attrValue));
                break;
            }
            default: {
                return false;
            }
        }
    }
    return true;
}

void MarqueeComponent::SetScrollamount(uint16_t scrollamount)
{
    scrollamount_ = scrollamount;
    SetRollSpeed();
}

void MarqueeComponent::SetScrolldelay(uint16_t scrolldelay)
{
    const int16_t minScrolldelay = 60;
    if (scrolldelay >= minScrolldelay) {
        scrolldelay_ = scrolldelay;
    } else {
        scrolldelay_ = minScrolldelay;
    }
    SetRollSpeed();
}

void MarqueeComponent::SetRollSpeed()
{
    UILabelTypeWrapper *uiLabel = TextComponent::GetUILabelView();
    if (uiLabel != nullptr) {
        const int32_t supportScrolldelayApiVersion = 5;
        if (JsAppContext::GetInstance()->GetTargetApi() >= supportScrolldelayApiVersion) {
            uint32_t rollSpeed = (uint32_t)scrollamount_ * MILLISECONDS_PER_SECOND / scrolldelay_;
            if (rollSpeed > UINT16_MAX) {
                rollSpeed = UINT16_MAX;
            }
            uiLabel->SetRollSpeed(rollSpeed);
        } else {
            uiLabel->SetRollSpeed(scrollamount_);
        }
    }
}
} // namespace ACELite
} // namespace OHOS
