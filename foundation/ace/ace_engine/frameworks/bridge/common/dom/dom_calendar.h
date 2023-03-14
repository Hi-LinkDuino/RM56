/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CALENDAR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CALENDAR_H

#include "core/components/calendar/calendar_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DomCalendar : public DOMNode {
    DECLARE_ACE_TYPE(DomCalendar, DOMNode);

public:
    DomCalendar(NodeId nodeId, const std::string& nodeName);
    ~DomCalendar() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return calendarComponent_;
    }

protected:
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

private:
    bool ParseDataAdapter(const std::string& value);
    void HandleGoTo(const std::string& args);

    EventMarker selectedChangeEvent_;
    EventMarker requestDataEvent_;
    RefPtr<CalendarComponent> calendarComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CALENDAR_H
