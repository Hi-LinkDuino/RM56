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

#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_register.h"

namespace OHOS::Ace::Framework {

void JsClickFunction::Execute()
{
    JsFunction::ExecuteJS();

    // This is required to request for new frame, which eventually will call
    // FlushBuild, FlushLayout and FlushRender on the dirty elements
#ifdef USE_V8_ENGINE
    V8DeclarativeEngineInstance::TriggerPageUpdate();
#elif USE_QUICKJS_ENGINE
    QJSDeclarativeEngineInstance::TriggerPageUpdate(QJSDeclarativeEngineInstance::GetCurrentContext());
#elif USE_ARK_ENGINE
    JsiDeclarativeEngineInstance::TriggerPageUpdate(JsiDeclarativeEngineInstance::GetCurrentRuntime());
#endif
}

void JsClickFunction::Execute(const ClickInfo& info)
{
    JSRef<JSObject> obj = JSRef<JSObject>::New();
    Offset globalOffset = info.GetGlobalLocation();
    Offset localOffset = info.GetLocalLocation();
    obj->SetProperty<double>("screenX", SystemProperties::Px2Vp(globalOffset.GetX()));
    obj->SetProperty<double>("screenY", SystemProperties::Px2Vp(globalOffset.GetY()));
    obj->SetProperty<double>("x", SystemProperties::Px2Vp(localOffset.GetX()));
    obj->SetProperty<double>("y", SystemProperties::Px2Vp(localOffset.GetY()));
    obj->SetProperty<double>("timestamp", static_cast<double>(info.GetTimeStamp().time_since_epoch().count()));
    obj->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));
    auto target = CreateEventTargetObject(info);
    obj->SetPropertyObject("target", target);

    LOGD("globalOffset.GetX() = %lf, globalOffset.GetY() = %lf, localOffset.GetX() = %lf, localOffset.GetY() = %lf",
        globalOffset.GetX(), globalOffset.GetY(), localOffset.GetX(), localOffset.GetY());

    JSRef<JSVal> param = obj;
    JsFunction::ExecuteJS(1, &param);
}

void JsClickFunction::Execute(const GestureEvent& info)
{
    JSRef<JSObject> obj = JSRef<JSObject>::New();
    Offset globalOffset = info.GetGlobalLocation();
    Offset localOffset = info.GetLocalLocation();
    obj->SetProperty<double>("screenX", SystemProperties::Px2Vp(globalOffset.GetX()));
    obj->SetProperty<double>("screenY", SystemProperties::Px2Vp(globalOffset.GetY()));
    obj->SetProperty<double>("x", SystemProperties::Px2Vp(localOffset.GetX()));
    obj->SetProperty<double>("y", SystemProperties::Px2Vp(localOffset.GetY()));
    obj->SetProperty<double>("timestamp", static_cast<double>(info.GetTimeStamp().time_since_epoch().count()));
    obj->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));
    auto target = CreateEventTargetObject(info);
    obj->SetPropertyObject("target", target);

    LOGD("globalOffset.GetX() = %lf, globalOffset.GetY() = %lf, localOffset.GetX() = %lf, localOffset.GetY() = %lf",
        globalOffset.GetX(), globalOffset.GetY(), localOffset.GetX(), localOffset.GetY());

    JSRef<JSVal> param = obj;
    JsFunction::ExecuteJS(1, &param);
}

void JsClickFunction::Execute(MouseInfo& info)
{
    JSRef<JSObjTemplate> objectTemplate = JSRef<JSObjTemplate>::New();
    objectTemplate->SetInternalFieldCount(1);
    JSRef<JSObject> obj = objectTemplate->NewInstance();
    obj->SetProperty<int32_t>("button", static_cast<int32_t>(info.GetButton()));
    obj->SetProperty<int32_t>("action", static_cast<int32_t>(info.GetAction()));
    Offset globalOffset = info.GetGlobalLocation();
    Offset localOffset = info.GetLocalLocation();
    obj->SetProperty<double>("screenX", SystemProperties::Px2Vp(globalOffset.GetX()));
    obj->SetProperty<double>("screenY", SystemProperties::Px2Vp(globalOffset.GetY()));
    obj->SetProperty<double>("x", SystemProperties::Px2Vp(localOffset.GetX()));
    obj->SetProperty<double>("y", SystemProperties::Px2Vp(localOffset.GetY()));
    obj->SetProperty<double>("timestamp", static_cast<double>(info.GetTimeStamp().time_since_epoch().count()));
    obj->SetPropertyObject(
        "stopPropagation", JSRef<JSFunc>::New<FunctionCallback>(JsStopPropagation));
    obj->SetProperty<double>("source", static_cast<int32_t>(info.GetSourceDevice()));
    auto target = CreateEventTargetObject(info);
    obj->SetPropertyObject("target", target);
    obj->Wrap<MouseInfo>(&info);

    LOGD("button = %d, action = %d, globalOffset = (%lf, %lf), localOffset = (%lf, %lf),", info.GetButton(),
        info.GetAction(), globalOffset.GetX(), globalOffset.GetY(), localOffset.GetX(), localOffset.GetY());

    JSRef<JSVal> param = JSRef<JSObject>::Cast(obj);
    JsFunction::ExecuteJS(1, &param);
}

} // namespace OHOS::Ace::Framework
