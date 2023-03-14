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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_touch_function.h"

#include "base/log/log.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"

namespace OHOS::Ace::Framework {

JSRef<JSObject> JsTouchFunction::CreateTouchInfo(const TouchLocationInfo& touchInfo)
{
    JSRef<JSObject> touchInfoObj = JSRef<JSObject>::New();
    const OHOS::Ace::Offset& globalLocation = touchInfo.GetGlobalLocation();
    const OHOS::Ace::Offset& localLocation = touchInfo.GetLocalLocation();
    touchInfoObj->SetProperty<int32_t>("type", static_cast<int32_t>(touchInfo.GetTouchType()));
    touchInfoObj->SetProperty<int32_t>("id", touchInfo.GetFingerId());
    touchInfoObj->SetProperty<double>("screenX", SystemProperties::Px2Vp(globalLocation.GetX()));
    touchInfoObj->SetProperty<double>("screenY", SystemProperties::Px2Vp(globalLocation.GetY()));
    touchInfoObj->SetProperty<double>("x", SystemProperties::Px2Vp(localLocation.GetX()));
    touchInfoObj->SetProperty<double>("y", SystemProperties::Px2Vp(localLocation.GetY()));
    return touchInfoObj;
}

JSRef<JSObject> JsTouchFunction::CreateJSEventInfo(TouchEventInfo& info)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    objectTemplate->SetInternalFieldCount(1);
    JSRef<JSObject> eventObj = objectTemplate->NewInstance();
    JSRef<JSArray> touchArr = JSRef<JSArray>::New();
    JSRef<JSArray> changeTouchArr = JSRef<JSArray>::New();
    eventObj->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));
    eventObj->SetProperty<double>("timestamp", static_cast<double>(info.GetTimeStamp().time_since_epoch().count()));
    auto target = CreateEventTargetObject(info);
    eventObj->SetPropertyObject("target", target);

    const std::list<TouchLocationInfo>& touchList = info.GetTouches();
    uint32_t idx = 0;
    for (const TouchLocationInfo& location : touchList) {
        JSRef<JSObject> element = CreateTouchInfo(location);
        touchArr->SetValueAt(idx++, element);
    }
    eventObj->SetPropertyObject("touches", touchArr);

    idx = 0; // reset index counter
    const std::list<TouchLocationInfo>& changeTouch = info.GetChangedTouches();
    for (const TouchLocationInfo& change : changeTouch) {
        JSRef<JSObject> element = CreateTouchInfo(change);
        changeTouchArr->SetValueAt(idx++, element);
    }
    if (changeTouch.size() > 0) {
        eventObj->SetProperty<int32_t>("type", static_cast<int32_t>(changeTouch.front().GetTouchType()));
    }
    eventObj->SetPropertyObject("changedTouches", changeTouchArr);
    eventObj->SetPropertyObject(
        "stopPropagation", JSRef<JSFunc>::New<FunctionCallback>(JsStopPropagation));
    eventObj->Wrap<TouchEventInfo>(&info);
    return eventObj;
}

void JsTouchFunction::Execute(TouchEventInfo& info)
{
    LOGD("JsTouchFunction: eventType[%s]", info.GetType().c_str());
    JSRef<JSVal> param = JSRef<JSObject>::Cast(CreateJSEventInfo(info));
    JsFunction::ExecuteJS(1, &param);
}

} // namespace OHOS::Ace::Framework
