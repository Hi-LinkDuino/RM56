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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_GESTURE_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_GESTURE_H

#include "core/event/ace_event_handler.h"
#include "core/gestures/gesture_info.h"

#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSGesture : public virtual AceType {
    DECLARE_ACE_TYPE(JSGesture, AceType);

public:
    JSGesture() = default;
    ~JSGesture() override = default;

    enum class JSGestureEvent { ACTION, START, UPDATE, END, CANCEL };

    static void JSBind(BindingTarget globalObj);

    static void Create(const JSCallbackInfo& info);
    static void Pop();
    static void Finish();

    static void JsHandlerOnAction(const JSCallbackInfo& args);
    static void JsHandlerOnActionStart(const JSCallbackInfo& args);
    static void JsHandlerOnActionUpdate(const JSCallbackInfo& args);
    static void JsHandlerOnActionEnd(const JSCallbackInfo& args);
    static void JsHandlerOnActionCancel(const JSCallbackInfo& args);
    static void JsHandlerOnGestureEvent(JSGestureEvent action, const JSCallbackInfo& args);
}; // JSGesture

class JSTapGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSTapGesture, JSGesture);

public:
    JSTapGesture() = default;
    ~JSTapGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSLongPressGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSLongPressGesture, JSGesture);

public:
    JSLongPressGesture() = default;
    ~JSLongPressGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSPanGestureOption final : public Referenced {
public:
    JSPanGestureOption() = default;
    ~JSPanGestureOption() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSPanGestureOption* panGestureOption);
    void SetDirection(const JSCallbackInfo& args);
    void SetDistance(const JSCallbackInfo& args);
    void SetFingers(const JSCallbackInfo& args);

    void SetPanGestureOption(const RefPtr<PanGestureOption>& panGestureOption)
    {
        panGestureOption_ = panGestureOption;
    }

    RefPtr<PanGestureOption> GetPanGestureOption() const
    {
        return panGestureOption_;
    }

private:
    RefPtr<PanGestureOption> panGestureOption_;
};

class JSPanGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSPanGesture, JSGesture);

public:
    JSPanGesture() = default;
    ~JSPanGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSSwipeGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSSwipeGesture, JSGesture);
public:
    JSSwipeGesture() = default;
    ~JSSwipeGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSPinchGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSPinchGesture, JSGesture);

public:
    JSPinchGesture() = default;
    ~JSPinchGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSRotationGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSRotationGesture, JSGesture);

public:
    JSRotationGesture() = default;
    ~JSRotationGesture() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSGestureGroup : public JSGesture {
    DECLARE_ACE_TYPE(JSGestureGroup, JSGesture);

public:
    JSGestureGroup() = default;
    ~JSGestureGroup() override = default;

    static void Create(const JSCallbackInfo& args);
};

class JSParallelGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSParallelGesture, JSGesture)
public:
    static void Create();
    static void JSBind(BindingTarget globalObj);
};

class JSTimeoutGesture : public JSGesture {
    DECLARE_ACE_TYPE(JSTimeoutGesture, JSGesture)
public:
    static void Create(const JSCallbackInfo& args);
    static void Pop();
    static void JSBind(BindingTarget globalObj);
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_GESTURE_H

