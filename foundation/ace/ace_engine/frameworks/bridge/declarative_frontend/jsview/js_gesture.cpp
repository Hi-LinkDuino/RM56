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

#include "frameworks/bridge/declarative_frontend/jsview/js_gesture.h"

#include "frameworks/bridge/declarative_frontend/engine/functions/js_gesture_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/core/components/gesture_listener/gesture_component.h"
#include "frameworks/core/gestures/exclusive_recognizer.h"
#include "frameworks/core/gestures/gesture_group.h"
#include "frameworks/core/gestures/long_press_gesture.h"
#include "frameworks/core/gestures/pan_gesture.h"
#include "frameworks/core/gestures/parallel_recognizer.h"
#include "frameworks/core/gestures/pinch_gesture.h"
#include "frameworks/core/gestures/rotation_gesture.h"
#include "frameworks/core/gestures/slide_gesture.h"
#include "frameworks/core/gestures/tap_gesture.h"
#include "frameworks/core/gestures/timeout_gesture.h"

namespace OHOS::Ace::Framework {

namespace {
constexpr int32_t DEFAULT_TAP_FINGER = 1;
constexpr int32_t DEFAULT_TAP_COUNT = 1;
constexpr int32_t DEFAULT_LONG_PRESS_FINGER = 1;
constexpr int32_t DEFAULT_LONG_PRESS_DURATION = 500;
constexpr int32_t DEFAULT_PINCH_FINGER = 2;
constexpr double DEFAULT_PINCH_DISTANCE = 3.0;
constexpr int32_t DEFAULT_PAN_FINGER = 1;
constexpr double DEFAULT_PAN_DISTANCE = 5.0;
constexpr int32_t DEFAULT_SLIDE_FINGER = DEFAULT_PAN_FINGER;
constexpr double DEFAULT_SLIDE_SPEED = 100.0;
constexpr int32_t DEFAULT_ROTATION_FINGER = 2;
constexpr double DEFAULT_ROTATION_ANGLE = 1.0;

constexpr char GESTURE_FINGERS[] = "fingers";
constexpr char GESTURE_DISTANCE[] = "distance";
constexpr char GESTURE_SPEED[] = "speed";
constexpr char TAP_COUNT[] = "count";
constexpr char LONG_PRESS_REPEAT[] = "repeat";
constexpr char LONG_PRESS_DURATION[] = "duration";
constexpr char PAN_DIRECTION[] = "direction";
constexpr char SWIPE_DIRECTION[] = "direction";
constexpr char ROTATION_ANGLE[] = "angle";
} // namespace

void JSGesture::Create(const JSCallbackInfo& info)
{
    LOGD("JS gesture create");
    GesturePriority priority = GesturePriority::Low;
    if (info.Length() > 0 && info[0]->IsNumber()) {
        int32_t priorityNum = info[0]->ToNumber<int32_t>();
        if (priorityNum > static_cast<int32_t>(GesturePriority::Begin) &&
            priorityNum < static_cast<int32_t>(GesturePriority::End)) {
            priority = static_cast<GesturePriority>(priorityNum);
        }
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();

    gestureComponent->SetPriority(priority);

    GestureMask gestureMask = GestureMask::Normal;
    if (info.Length() > 1 && info[1]->IsNumber()) {
        int32_t gestureMaskNum = info[1]->ToNumber<int32_t>();
        if (gestureMaskNum > static_cast<int32_t>(GestureMask::Begin) &&
            gestureMaskNum < static_cast<int32_t>(GestureMask::End)) {
            gestureMask = static_cast<GestureMask>(gestureMaskNum);
        }
    }
    gestureComponent->SetGestureMask(gestureMask);
}

void JSGesture::Finish()
{
    LOGD("JS gesture finish");
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = gestureComponent->FinishGesture();
    if (!gesture) {
        LOGE("gesture is not exist when component finish");
        return;
    }

    gesture->SetGestureMask(gestureComponent->GetGestureMask());

    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();

    boxComponent->AddGesture(gestureComponent->GetPriority(), gesture);
}

void JSGesture::Pop()
{
    LOGD("JS gesture pop");
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    gestureComponent->PopGesture();
}

void JSTapGesture::Create(const JSCallbackInfo& args)
{
    int32_t countNum = DEFAULT_TAP_COUNT;
    int32_t fingersNum = DEFAULT_TAP_FINGER;
    if (args.Length() > 0 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> count = obj->GetProperty(TAP_COUNT);
        JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);

        if (count->IsNumber()) {
            int32_t countNumber = count->ToNumber<int32_t>();
            countNum = countNumber <= DEFAULT_TAP_COUNT ? DEFAULT_TAP_COUNT : countNumber;
        }

        if (fingers->IsNumber()) {
            int32_t fingersNumber = fingers->ToNumber<int32_t>();
            fingersNum = fingersNumber <= DEFAULT_TAP_FINGER ? DEFAULT_TAP_FINGER : fingersNumber;
        }
    }

    LOGD("JS Tap gesture created with count %{public}d, fingers %{public}d", countNum, fingersNum);
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::TapGesture>(countNum, fingersNum);
    gestureComponent->PushGesture(gesture);
}

void JSLongPressGesture::Create(const JSCallbackInfo& args)
{
    LOGD("JSLongPressGesture Create");

    int32_t fingersNum = DEFAULT_LONG_PRESS_FINGER;
    bool repeatResult = false;
    int32_t durationNum = DEFAULT_LONG_PRESS_DURATION;
    if (args.Length() > 0 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
        JSRef<JSVal> repeat = obj->GetProperty(LONG_PRESS_REPEAT);
        JSRef<JSVal> duration = obj->GetProperty(LONG_PRESS_DURATION);

        if (fingers->IsNumber()) {
            int32_t fingersNumber = fingers->ToNumber<int32_t>();
            fingersNum = fingersNumber <= DEFAULT_LONG_PRESS_FINGER ? DEFAULT_LONG_PRESS_FINGER : fingersNumber;
        }

        if (repeat->IsBoolean()) {
            repeatResult = repeat->ToBoolean();
        }

        if (duration->IsNumber()) {
            int32_t durationNumber = duration->ToNumber<int32_t>();
            durationNum = durationNumber <= 0 ? DEFAULT_LONG_PRESS_DURATION : durationNumber;
        }
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::LongPressGesture>(fingersNum, repeatResult, durationNum);
    gestureComponent->PushGesture(gesture);
}

void JSPanGesture::Create(const JSCallbackInfo& args)
{
    LOGD("JSPanGesture Create");

    int32_t fingersNum = DEFAULT_PAN_FINGER;
    double distanceNum = DEFAULT_PAN_DISTANCE;
    PanDirection panDirection;
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    if (args.Length() <= 0 || !args[0]->IsObject()) {
        auto gesture = AceType::MakeRefPtr<OHOS::Ace::PanGesture>(fingersNum, panDirection, distanceNum);
        gestureComponent->PushGesture(gesture);
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    JSPanGestureOption* panGestureOption = obj->Unwrap<JSPanGestureOption>();
    if (panGestureOption != nullptr) {
        auto gesture = AceType::MakeRefPtr<OHOS::Ace::PanGesture>(panGestureOption->GetPanGestureOption());
        gestureComponent->PushGesture(gesture);
        return;
    }

    JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
    JSRef<JSVal> distance = obj->GetProperty(GESTURE_DISTANCE);
    JSRef<JSVal> directionNum = obj->GetProperty(PAN_DIRECTION);

    if (fingers->IsNumber()) {
        int32_t fingersNumber = fingers->ToNumber<int32_t>();
        fingersNum = fingersNumber <= DEFAULT_PAN_FINGER ? DEFAULT_PAN_FINGER : fingersNumber;
    }

    if (distance->IsNumber()) {
        double distanceNumber = distance->ToNumber<double>();
        distanceNum = LessNotEqual(distanceNumber, 0.0) ? DEFAULT_PAN_DISTANCE : distanceNumber;
    }

    if (directionNum->IsNumber()) {
        uint32_t directNum = directionNum->ToNumber<uint32_t>();
        if (directNum >= static_cast<uint32_t>(PanDirection::NONE) &&
            directNum <= static_cast<uint32_t>(PanDirection::ALL)) {
            panDirection.type = directNum;
        }
    }
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::PanGesture>(fingersNum, panDirection, distanceNum);
    gestureComponent->PushGesture(gesture);
}

void JSSwipeGesture::Create(const JSCallbackInfo& args)
{
    int32_t fingersNum = DEFAULT_SLIDE_FINGER;
    double speedNum = DEFAULT_SLIDE_SPEED;
    SwipeDirection slideDirection;
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    if (args.Length() <= 0 || !args[0]->IsObject()) {
        auto gesture = AceType::MakeRefPtr<SwipeGesture>(fingersNum, slideDirection, speedNum);
        gestureComponent->PushGesture(gesture);
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);

    JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
    JSRef<JSVal> speed = obj->GetProperty(GESTURE_SPEED);
    JSRef<JSVal> directionNum = obj->GetProperty(SWIPE_DIRECTION);

    if (fingers->IsNumber()) {
        int32_t fingersNumber = fingers->ToNumber<int32_t>();
        fingersNum = fingersNumber <= DEFAULT_PAN_FINGER ? DEFAULT_PAN_FINGER : fingersNumber;
    }

    if (speed->IsNumber()) {
        double speedNumber = speed->ToNumber<double>();
        speedNum = LessNotEqual(speedNumber, 0.0) ? DEFAULT_SLIDE_SPEED : speedNumber;
    }

    if (directionNum->IsNumber()) {
        uint32_t directNum = directionNum->ToNumber<uint32_t>();
        if (directNum >= static_cast<uint32_t>(SwipeDirection::NONE) &&
            directNum <= static_cast<uint32_t>(SwipeDirection::ALL)) {
            slideDirection.type = directNum;
        }
    }
    auto gesture = AceType::MakeRefPtr<SwipeGesture>(fingersNum, slideDirection, speedNum);
    gestureComponent->PushGesture(gesture);
}

void JSPinchGesture::Create(const JSCallbackInfo& args)
{
    LOGD("JSPinchGesture Create");

    int32_t fingersNum = DEFAULT_PINCH_FINGER;
    double distanceNum = DEFAULT_PINCH_DISTANCE;
    if (args.Length() > 0 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
        JSRef<JSVal> distance = obj->GetProperty(GESTURE_DISTANCE);

        if (fingers->IsNumber()) {
            int32_t fingersNumber = fingers->ToNumber<int32_t>();
            fingersNum = fingersNumber <= DEFAULT_PINCH_FINGER ? DEFAULT_PINCH_FINGER : fingersNumber;
        }
        if (distance->IsNumber()) {
            double distanceNumber = distance->ToNumber<double>();
            distanceNum = LessNotEqual(distanceNumber, 0.0) ? DEFAULT_PINCH_DISTANCE : distanceNumber;
        }
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::PinchGesture>(fingersNum, distanceNum);
    gestureComponent->PushGesture(gesture);
}

void JSRotationGesture::Create(const JSCallbackInfo& args)
{
    LOGD("JSRotationGesture Create");

    double angleNum = DEFAULT_ROTATION_ANGLE;
    int32_t fingersNum = DEFAULT_ROTATION_FINGER;
    if (args.Length() > 0 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
        JSRef<JSVal> angle = obj->GetProperty(ROTATION_ANGLE);

        if (fingers->IsNumber()) {
            int32_t fingersNumber = fingers->ToNumber<int32_t>();
            fingersNum = fingersNumber <= DEFAULT_ROTATION_FINGER ? DEFAULT_ROTATION_FINGER : fingersNumber;
        }

        if (angle->IsNumber()) {
            double angleNumber = angle->ToNumber<double>();
            angleNum = LessNotEqual(angleNumber, 0.0) ? DEFAULT_ROTATION_ANGLE : angleNumber;
        }
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::RotationGesture>(fingersNum, angleNum);
    gestureComponent->PushGesture(gesture);
}

void JSGestureGroup::Create(const JSCallbackInfo& args)
{
    int32_t gestureMode = 0;

    if (args.Length() > 0 && args[0]->IsNumber()) {
        gestureMode = args[0]->ToNumber<int32_t>();
    }

    LOGD("Js Gesture group create with mode %{public}d", gestureMode);
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<OHOS::Ace::GestureGroup>(static_cast<GestureMode>(gestureMode));
    gestureComponent->PushGesture(gesture);
}

void JSGesture::JsHandlerOnGestureEvent(JSGestureEvent action, const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsFunction()) {
        LOGE("args is not js function");
        return;
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = gestureComponent->TopGesture();
    if (!gesture) {
        LOGE("top gesture is illegal");
        return;
    }
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("fail to get inspector for on handle event");
        return;
    }
    auto impl = inspector->GetInspectorFunctionImpl();

    RefPtr<JsGestureFunction> handlerFunc = AceType::MakeRefPtr<JsGestureFunction>(JSRef<JSFunc>::Cast(args[0]));

    if (action == JSGestureEvent::CANCEL) {
        auto onActionCancelFunc = [execCtx = args.GetExecutionContext(), func = std::move(handlerFunc), impl]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto info = GestureEvent();
            if (impl) {
                impl->UpdateEventInfo(info);
            }
            ACE_SCORING_EVENT("Gesture.onCancel");
            func->Execute(info);
        };
        gesture->SetOnActionCancelId(onActionCancelFunc);
        return;
    }

    auto onActionFunc = [execCtx = args.GetExecutionContext(), func = std::move(handlerFunc), impl](
                            GestureEvent& info) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        if (impl) {
            impl->UpdateEventInfo(info);
        }
        ACE_SCORING_EVENT("Gesture.onActionCancel");
        func->Execute(info);
    };

    switch (action) {
        case JSGestureEvent::ACTION:
            gesture->SetOnActionId(onActionFunc);
            break;
        case JSGestureEvent::START:
            gesture->SetOnActionStartId(onActionFunc);
            break;
        case JSGestureEvent::UPDATE:
            gesture->SetOnActionUpdateId(onActionFunc);
            break;
        case JSGestureEvent::END:
            gesture->SetOnActionEndId(onActionFunc);
            break;
        default:
            LOGW("Unknown gesture action %{public}d", action);
            break;
    }
}

void JSGesture::JsHandlerOnAction(const JSCallbackInfo& args)
{
    JSGesture::JsHandlerOnGestureEvent(JSGestureEvent::ACTION, args);
}
void JSGesture::JsHandlerOnActionStart(const JSCallbackInfo& args)
{
    JSGesture::JsHandlerOnGestureEvent(JSGestureEvent::START, args);
}
void JSGesture::JsHandlerOnActionUpdate(const JSCallbackInfo& args)
{
    JSGesture::JsHandlerOnGestureEvent(JSGestureEvent::UPDATE, args);
}

void JSGesture::JsHandlerOnActionEnd(const JSCallbackInfo& args)
{
    JSGesture::JsHandlerOnGestureEvent(JSGestureEvent::END, args);
}
void JSGesture::JsHandlerOnActionCancel(const JSCallbackInfo& args)
{
    JSGesture::JsHandlerOnGestureEvent(JSGestureEvent::CANCEL, args);
}

void JSPanGestureOption::JSBind(BindingTarget globalObj)
{
    JSClass<JSPanGestureOption>::Declare("PanGestureOption");
    JSClass<JSPanGestureOption>::CustomMethod("setDirection", &JSPanGestureOption::SetDirection);
    JSClass<JSPanGestureOption>::CustomMethod("setDistance", &JSPanGestureOption::SetDistance);
    JSClass<JSPanGestureOption>::CustomMethod("setFingers", &JSPanGestureOption::SetFingers);
    JSClass<JSPanGestureOption>::Bind(globalObj, &JSPanGestureOption::Constructor, &JSPanGestureOption::Destructor);
}

void JSPanGestureOption::SetDirection(const JSCallbackInfo& args)
{
    if (args.Length() > 0 && args[0]->IsNumber()) {
        PanDirection direction = { args[0]->ToNumber<int32_t>() };
        panGestureOption_->SetDirection(direction);
    }
}

void JSPanGestureOption::SetDistance(const JSCallbackInfo& args)
{
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto distance = args[0]->ToNumber<double>();
        panGestureOption_->SetDistance(distance);
    }
}

void JSPanGestureOption::SetFingers(const JSCallbackInfo& args)
{
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto fingers = args[0]->ToNumber<int32_t>();
        panGestureOption_->SetFingers(fingers);
    }
}

void JSPanGestureOption::Constructor(const JSCallbackInfo& args)
{
    auto panGestureOption = Referenced::MakeRefPtr<JSPanGestureOption>();
    panGestureOption->IncRefCount();
    RefPtr<PanGestureOption> option = AceType::MakeRefPtr<PanGestureOption>();

    int32_t fingersNum = DEFAULT_PAN_FINGER;
    double distanceNum = DEFAULT_PAN_DISTANCE;
    PanDirection panDirection;

    if (args.Length() > 0 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> fingers = obj->GetProperty(GESTURE_FINGERS);
        JSRef<JSVal> distance = obj->GetProperty(GESTURE_DISTANCE);
        JSRef<JSVal> directionNum = obj->GetProperty(PAN_DIRECTION);

        if (fingers->IsNumber()) {
            int32_t fingersNumber = fingers->ToNumber<int32_t>();
            fingersNum = fingersNumber <= DEFAULT_PAN_FINGER ? DEFAULT_PAN_FINGER : fingersNumber;
        }

        if (distance->IsNumber()) {
            double distanceNumber = distance->ToNumber<double>();
            distanceNum = LessNotEqual(distanceNumber, 0.0) ? DEFAULT_PAN_DISTANCE : distanceNumber;
        }

        if (directionNum->IsNumber()) {
            uint32_t directNum = directionNum->ToNumber<uint32_t>();
            if (directNum >= static_cast<uint32_t>(PanDirection::NONE) &&
                directNum <= static_cast<uint32_t>(PanDirection::ALL)) {
                panDirection.type = directNum;
            }
        }
    }
    option->SetDirection(panDirection);
    option->SetDistance(distanceNum);
    option->SetFingers(fingersNum);

    panGestureOption->SetPanGestureOption(option);
    args.SetReturnValue(Referenced::RawPtr(panGestureOption));
}

void JSPanGestureOption::Destructor(JSPanGestureOption* panGestureOption)
{
    if (panGestureOption != nullptr) {
        panGestureOption->DecRefCount();
    }
}

void JSGesture::JSBind(BindingTarget globalObj)
{
    JSClass<JSGesture>::Declare("Gesture");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSGesture>::StaticMethod("create", &JSGesture::Create, opt);
    JSClass<JSGesture>::StaticMethod("pop", &JSGesture::Finish);
    JSClass<JSGesture>::Bind<>(globalObj);

    JSClass<JSTapGesture>::Declare("TapGesture");
    JSClass<JSTapGesture>::StaticMethod("create", &JSTapGesture::Create, opt);
    JSClass<JSTapGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSTapGesture>::StaticMethod("onAction", &JSGesture::JsHandlerOnAction);
    JSClass<JSTapGesture>::StaticMethod("onActionUpdate", &JSGesture::JsHandlerOnActionUpdate);
    JSClass<JSTapGesture>::Bind<>(globalObj);

    JSClass<JSLongPressGesture>::Declare("LongPressGesture");
    JSClass<JSLongPressGesture>::StaticMethod("create", &JSLongPressGesture::Create, opt);
    JSClass<JSLongPressGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSLongPressGesture>::StaticMethod("onAction", &JSGesture::JsHandlerOnAction);
    JSClass<JSLongPressGesture>::StaticMethod("onActionEnd", &JSGesture::JsHandlerOnActionEnd);
    JSClass<JSLongPressGesture>::StaticMethod("onActionCancel", &JSGesture::JsHandlerOnActionCancel);
    JSClass<JSLongPressGesture>::StaticMethod("onActionUpdate", &JSGesture::JsHandlerOnActionUpdate);
    JSClass<JSLongPressGesture>::Bind(globalObj);

    JSClass<JSPanGesture>::Declare("PanGesture");
    JSClass<JSPanGesture>::StaticMethod("create", &JSPanGesture::Create, opt);
    JSClass<JSPanGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSPanGesture>::StaticMethod("onActionStart", &JSGesture::JsHandlerOnActionStart);
    JSClass<JSPanGesture>::StaticMethod("onActionUpdate", &JSGesture::JsHandlerOnActionUpdate);
    JSClass<JSPanGesture>::StaticMethod("onActionEnd", &JSGesture::JsHandlerOnActionEnd);
    JSClass<JSPanGesture>::StaticMethod("onActionCancel", &JSGesture::JsHandlerOnActionCancel);
    JSClass<JSPanGesture>::Bind(globalObj);

    JSClass<JSSwipeGesture>::Declare("SwipeGesture");
    JSClass<JSSwipeGesture>::StaticMethod("create", &JSSwipeGesture::Create, opt);
    JSClass<JSSwipeGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSSwipeGesture>::StaticMethod("onAction", &JSGesture::JsHandlerOnAction);
    JSClass<JSSwipeGesture>::Bind(globalObj);

    JSClass<JSPinchGesture>::Declare("PinchGesture");
    JSClass<JSPinchGesture>::StaticMethod("create", &JSPinchGesture::Create, opt);
    JSClass<JSPinchGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSPinchGesture>::StaticMethod("onActionStart", &JSGesture::JsHandlerOnActionStart);
    JSClass<JSPinchGesture>::StaticMethod("onActionUpdate", &JSGesture::JsHandlerOnActionUpdate);
    JSClass<JSPinchGesture>::StaticMethod("onActionEnd", &JSGesture::JsHandlerOnActionEnd);
    JSClass<JSPinchGesture>::StaticMethod("onActionCancel", &JSGesture::JsHandlerOnActionCancel);
    JSClass<JSPinchGesture>::Bind(globalObj);

    JSClass<JSRotationGesture>::Declare("RotationGesture");
    JSClass<JSRotationGesture>::StaticMethod("create", &JSRotationGesture::Create, opt);
    JSClass<JSRotationGesture>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSRotationGesture>::StaticMethod("onActionStart", &JSGesture::JsHandlerOnActionStart);
    JSClass<JSRotationGesture>::StaticMethod("onActionUpdate", &JSGesture::JsHandlerOnActionUpdate);
    JSClass<JSRotationGesture>::StaticMethod("onActionEnd", &JSGesture::JsHandlerOnActionEnd);
    JSClass<JSRotationGesture>::StaticMethod("onActionCancel", &JSGesture::JsHandlerOnActionCancel);
    JSClass<JSRotationGesture>::Bind(globalObj);

    JSClass<JSGestureGroup>::Declare("GestureGroup");
    JSClass<JSGestureGroup>::StaticMethod("create", &JSGestureGroup::Create, opt);
    JSClass<JSGestureGroup>::StaticMethod("pop", &JSGesture::Pop);
    JSClass<JSGestureGroup>::StaticMethod("onCancel", &JSGesture::JsHandlerOnActionCancel);
    JSClass<JSGestureGroup>::Bind<>(globalObj);

    JSClass<JSTimeoutGesture>::Declare("TimeoutGesture");
    JSClass<JSTimeoutGesture>::StaticMethod("create", &JSTimeoutGesture::Create, opt);
    JSClass<JSTimeoutGesture>::StaticMethod("pop", &JSGesture::Pop);

    JSClass<JSTimeoutGesture>::Bind<>(globalObj);
}

void JSTimeoutGesture::Create(const JSCallbackInfo& args)
{
    LOGD("JSTimeoutGesture::Create()");
    if (!args[0]->IsNumber()) {
        return;
    }

    auto gestureComponent = ViewStackProcessor::GetInstance()->GetGestureComponent();
    auto gesture = AceType::MakeRefPtr<TimeoutGesture>(
                std::chrono::duration<float>(args[0]->ToNumber<float>()));
    gestureComponent->PushGesture(gesture);
}

void JSTimeoutGesture::JSBind(BindingTarget globalObj)
{
    JSClass<JSTimeoutGesture>::Declare("TimeoutGesture");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTimeoutGesture>::StaticMethod("create", &JSTimeoutGesture::Create, opt);
    JSClass<JSTimeoutGesture>::StaticMethod("pop", &JSGesture::Pop);

    JSClass<JSTimeoutGesture>::Bind<>(globalObj);
}

}; // namespace OHOS::Ace::Framework

