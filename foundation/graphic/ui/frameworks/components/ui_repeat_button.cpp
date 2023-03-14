/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_repeat_button.h"
#include "components/ui_button.h"

namespace OHOS {
UIRepeatButton* UIRepeatButton::repeatButton_ = nullptr;

/** default tick interval 200ms. */
UIRepeatButton::UIRepeatButton() : ticksInterval_(200), event_({ 0, 0 }),
    longPressed_(false), timerRepeatID_(0) {}

UIRepeatButton::~UIRepeatButton() {}

void UIRepeatButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    UIButton::OnDraw(gfxDstBuffer, invalidatedArea);
}

bool UIRepeatButton::OnReleaseEvent(const ReleaseEvent& event)
{
    if (longPressed_ && (timerRepeatID_ != 0)) {
        longPressed_ = false;
    }
    return UIButton::OnReleaseEvent(event);
}

bool UIRepeatButton::OnClickEvent(const ClickEvent& event)
{
    SetEvent(event);
    return UIButton::OnClickEvent(event);
}

void UIRepeatButton::HandleTickEvent()
{
    UIButton::OnClickEvent(event_);
}

bool UIRepeatButton::OnLongPressEvent(const LongPressEvent& event)
{
    longPressed_ = true;
    repeatButton_ = this;
    return UIButton::OnLongPressEvent(event);
}

void UIRepeatButton::SetEvent(const ClickEvent& event)
{
    event_ = event;
}
} // namespace OHOS
