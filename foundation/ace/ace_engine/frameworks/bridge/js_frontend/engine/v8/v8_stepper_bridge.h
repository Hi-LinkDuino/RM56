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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_STEPPER_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_STEPPER_BRIDGE_H

#include "frameworks/bridge/common/dom/dom_stepper.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

namespace OHOS::Ace::Framework {

class V8StepperBridge : virtual public AceType {
    DECLARE_ACE_TYPE(V8StepperBridge, AceType)

public:
    void GetAttrLabel(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, StepperLabels& stepperLabel);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_STEPPER_BRIDGE_H
