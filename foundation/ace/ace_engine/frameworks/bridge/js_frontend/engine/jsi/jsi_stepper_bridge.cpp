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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_stepper_bridge.h"

namespace OHOS::Ace::Framework {

void JsiStepperBridge::GetAttrLabel(
    shared_ptr<JsRuntime> runtime, const shared_ptr<JsValue>& valObject, StepperLabels& stepperLabel)
{
    if (!runtime) {
        LOGE("runtime is null");
        return;
    }
    if (!valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> item = valObject->GetProperty(runtime, key);
        if (!item) {
            LOGW("item value is null. Ignoring!");
            continue;
        }
        if (item->IsString(runtime) || item->IsNumber(runtime)) {
            std::string valStr = item->ToString(runtime);
            if (keyStr == DOM_STEPPER_LEFT_LABEL) {
                stepperLabel.leftLabel = valStr;
            } else if (keyStr == DOM_STEPPER_RIGHT_LABEL) {
                stepperLabel.rightLabel = valStr;
            } else if (keyStr == DOM_STEPPER_INITIAL_STATUS) {
                stepperLabel.initialStatus = valStr;
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace OHOS::Ace::Framework
