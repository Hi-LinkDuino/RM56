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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_GESTURE_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_GESTURE_FUNCTION_H

#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JsGestureFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsGestureFunction, JsFunction)

public:
    explicit JsGestureFunction(const JSRef<JSFunc>& jsFunction)
        : JsFunction(JSRef<JSObject>(), jsFunction) {}

    ~JsGestureFunction() override
    {
        LOGD("Destroy: JsGestureFunction");
    };
    void Execute();
    void Execute(const GestureEvent& info);

private:
    JSRef<JSObject> CreateGestureEvent(const GestureEvent& info);
    JSRef<JSObject> CreateFingerInfo(const FingerInfo& fingerInfo);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_GESTURE_FUNCTION_H