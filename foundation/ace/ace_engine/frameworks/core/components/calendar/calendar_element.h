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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_ELEMENT_H

#include "core/components/calendar/calendar_component.h"
#include "core/components/text/text_element.h"
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class CalendarElement : public ComposedElement, public FocusGroup {
    DECLARE_ACE_TYPE(CalendarElement, ComposedElement, FocusGroup);

public:
    explicit CalendarElement(const ComposeId& id) : ComposedElement(id) {}
    ~CalendarElement() override = default;

    void PerformBuild() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override
    {
        return false;
    }

    void OnFocus() override
    {
        FocusGroup::OnFocus();
        if (calendarController_) {
            calendarController_->NotifyFocusChanged(true);
        }
    }

    void OnBlur() override
    {
        FocusGroup::OnBlur();
        if (calendarController_) {
            calendarController_->NotifyFocusChanged(false);
        }
    }

    void BuildCardCalendar(const RefPtr<CalendarComponent>& calendar, const RefPtr<Element>& element);

private:
    void UpdateAttr(const RefPtr<CalendarComponent>& calendar);
    void SetArrowImage(const RefPtr<Element>& element, bool isLeft);
    RefPtr<TextElement> GetTextElement(const RefPtr<Element>& flex);

    RefPtr<CalendarController> calendarController_;
    RefPtr<RenderSwiper> renderSwiper_;
    std::function<void(const std::string&)> dateEvent_;
};

class CalendarMonthElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(CalendarMonthElement, RenderElement, FocusNode);

public:
    CalendarMonthElement() = default;
    ~CalendarMonthElement() override = default;

    void Update() override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    void OnFocus() override;
    void OnBlur() override;

private:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_ELEMENT_H
