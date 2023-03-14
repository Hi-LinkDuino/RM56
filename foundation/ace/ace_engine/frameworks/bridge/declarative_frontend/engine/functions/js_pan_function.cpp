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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_pan_function.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"

namespace OHOS::Ace::Framework {

JSRef<JSObject> JsPanFunction::createPanInfo(const TouchLocationInfo& info)
{
    JSRef<JSObject> panInfoObj = JSRef<JSObject>::New();
    const OHOS::Ace::Offset& globalLocation = info.GetGlobalLocation();
    const OHOS::Ace::Offset& localLocation = info.GetLocalLocation();
    panInfoObj->SetProperty<double>("globalX", globalLocation.GetX());
    panInfoObj->SetProperty<double>("globalY", globalLocation.GetY());
    panInfoObj->SetProperty<double>("localX", localLocation.GetX());
    panInfoObj->SetProperty<double>("localY", localLocation.GetY());
    return panInfoObj;
}

void JsPanFunction::Execute(const DragStartInfo& info)
{
    LOGD("JsPanFunction: eventType[%s]", info.GetType().c_str());
    JSRef<JSVal> param = createPanInfo(static_cast<TouchLocationInfo>(info));
    JsFunction::ExecuteJS(1, &param);
}

void JsPanFunction::Execute(const DragUpdateInfo& info)
{
    LOGD("JsPanFunction: eventType[%s]", info.GetType().c_str());
    JSRef<JSObject> paramObj = createPanInfo(static_cast<TouchLocationInfo>(info));
    const OHOS::Ace::Offset& deltaLocation = info.GetDelta();
    JSRef<JSObject> deltaInfoObj = JSRef<JSObject>::New();
    deltaInfoObj->SetProperty<double>("x", deltaLocation.GetX());
    deltaInfoObj->SetProperty<double>("y", deltaLocation.GetY());
    paramObj->SetPropertyObject("delta", deltaInfoObj);
    paramObj->SetProperty<double>("mainDelta", info.GetMainDelta());

    JSRef<JSVal> param = paramObj;
    JsFunction::ExecuteJS(1, &param);
}

void JsPanFunction::Execute(const DragEndInfo& info)
{
    LOGD("JsPanFunction: eventType[%s]", info.GetType().c_str());

    JSRef<JSObject> paramObj = createPanInfo(static_cast<TouchLocationInfo>(info));
    const OHOS::Ace::Velocity& velocityLocation = info.GetVelocity();
    JSRef<JSObject> velocityInfoObj = JSRef<JSObject>::New();
    velocityInfoObj->SetProperty<double>("x", velocityLocation.GetVelocityX());
    velocityInfoObj->SetProperty<double>("y", velocityLocation.GetVelocityY());
    paramObj->SetPropertyObject("velocity", velocityInfoObj);
    paramObj->SetProperty<double>("mainVelocity", info.GetMainVelocity());

    JSRef<JSVal> param = paramObj;
    JsFunction::ExecuteJS(1, &param);
}

void JsPanFunction::Execute()
{
    JsFunction::ExecuteJS();
}

} // namespace OHOS::Ace::Framework
