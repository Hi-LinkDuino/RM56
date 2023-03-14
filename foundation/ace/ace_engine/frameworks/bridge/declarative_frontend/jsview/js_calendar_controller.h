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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_CONTROLLER_H

#include "bridge/declarative_frontend/engine/bindings.h"

#include "base/memory/referenced.h"
#include "core/components/calendar/calendar_controller_v2.h"

namespace OHOS::Ace::Framework {

class JSCalendarController : public Referenced {
public:
    JSCalendarController() : controller_(AceType::MakeRefPtr<CalendarControllerV2>()) {}
    ~JSCalendarController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSCalendarController* scroller);

    void BackToToday(const JSCallbackInfo& args);
    void GoTo(const JSCallbackInfo& args);

    const RefPtr<CalendarControllerV2>& GetController() const
    {
        return controller_;
    }

private:
    RefPtr<CalendarControllerV2> controller_;

    ACE_DISALLOW_COPY_AND_MOVE(JSCalendarController);
};

} // OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_CONTROLLER_H