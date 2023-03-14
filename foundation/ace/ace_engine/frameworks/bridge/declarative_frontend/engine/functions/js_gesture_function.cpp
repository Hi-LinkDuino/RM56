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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_gesture_function.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"

namespace OHOS::Ace::Framework {

void JsGestureFunction::Execute()
{
    JsFunction::Execute();
}

void JsGestureFunction::Execute(const GestureEvent& info)
{
    JSRef<JSVal> param = JSRef<JSObject>::Cast(CreateGestureEvent(info));
    JsFunction::ExecuteJS(1, &param);
}

JSRef<JSObject> JsGestureFunction::CreateGestureEvent(const GestureEvent& info)
{
    JSRef<JSObject> gestureInfoObj = JSRef<JSObject>::New();
    gestureInfoObj->SetProperty<bool>("repeat", info.GetRepeat());
    gestureInfoObj->SetProperty<double>("offsetX", SystemProperties::Px2Vp(info.GetOffsetX()));
    gestureInfoObj->SetProperty<double>("offsetY", SystemProperties::Px2Vp(info.GetOffsetY()));
    gestureInfoObj->SetProperty<double>("scale", info.GetScale());
    gestureInfoObj->SetProperty<double>("angle", info.GetAngle());
    gestureInfoObj->SetProperty<double>("speed", info.GetSpeed());
    gestureInfoObj->SetProperty<double>("timestamp", info.GetTimeStamp().time_since_epoch().count());
    gestureInfoObj->SetProperty<double>("globalX", SystemProperties::Px2Vp(info.GetGlobalLocation().GetX()));
    gestureInfoObj->SetProperty<double>("globalY", SystemProperties::Px2Vp(info.GetGlobalLocation().GetY()));
    gestureInfoObj->SetProperty<double>("localX", SystemProperties::Px2Vp(info.GetLocalLocation().GetX()));
    gestureInfoObj->SetProperty<double>("localY", SystemProperties::Px2Vp(info.GetLocalLocation().GetY()));
    gestureInfoObj->SetProperty<double>("pinchCenterX", SystemProperties::Px2Vp(info.GetPinchCenter().GetX()));
    gestureInfoObj->SetProperty<double>("pinchCenterY", SystemProperties::Px2Vp(info.GetPinchCenter().GetY()));
    gestureInfoObj->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));

    JSRef<JSArray> fingerArr = JSRef<JSArray>::New();
    const std::list<FingerInfo>& fingerList = info.GetFingerList();
    uint32_t idx = 0;
    for (const FingerInfo& info : fingerList) {
        JSRef<JSObject> element = CreateFingerInfo(info);
        fingerArr->SetValueAt(idx++, element);
    }
    gestureInfoObj->SetPropertyObject("fingerList", fingerArr);

    auto target = CreateEventTargetObject(info);
    gestureInfoObj->SetPropertyObject("target", target);
    return gestureInfoObj;
}

JSRef<JSObject> JsGestureFunction::CreateFingerInfo(const FingerInfo& fingerInfo)
{
    JSRef<JSObject> fingerInfoObj = JSRef<JSObject>::New();
    const OHOS::Ace::Offset& globalLocation = fingerInfo.globalLocation_;
    const OHOS::Ace::Offset& localLocation = fingerInfo.localLocation_;
    fingerInfoObj->SetProperty<int32_t>("id", fingerInfo.fingerId_);
    fingerInfoObj->SetProperty<double>("globalX", SystemProperties::Px2Vp(globalLocation.GetX()));
    fingerInfoObj->SetProperty<double>("globalY", SystemProperties::Px2Vp(globalLocation.GetY()));
    fingerInfoObj->SetProperty<double>("localX", SystemProperties::Px2Vp(localLocation.GetX()));
    fingerInfoObj->SetProperty<double>("localY", SystemProperties::Px2Vp(localLocation.GetY()));
    return fingerInfoObj;
}

} // namespace OHOS::Ace::Framework
