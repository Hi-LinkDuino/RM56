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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_clock_bridge.h"

namespace OHOS::Ace::Framework {

void JsiClockBridge::ParseClockConfig(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject)
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
            // static linear map must be sorted by key.
            static const LinearMapNode<void (*)(std::string, JsiClockBridge&)> clockConfigOperators[] = {
                { DOM_DIGIT_COLOR,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitColor_ = valStr;
                    } },
                { DOM_DIGIT_COLOR_NIGHT,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitColorNight_ = valStr;
                    } },
                { DOM_DIGIT_RADIUS_RATIO,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitRadiusRatio_ = StringToDouble(valStr);
                    } },
                { DOM_DIGIT_SIZE_RATIO,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitSizeRatio_ = StringToDouble(valStr);
                    } },
                { DOM_CLOCK_FACE_SOURCE,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.clockFaceSrc_ = valStr;
                    } },
                { DOM_CLOCK_FACE_SOURCE_NIGHT,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.clockFaceNightSrc_ = valStr;
                    } },
                { DOM_HOUR_HAND_SOURCE,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.hourHandSrc_ = valStr;
                    } },
                { DOM_HOUR_HAND_SOURCE_NIGHT,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.hourHandNightSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.minuteHandSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE_NIGHT,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.minuteHandNightSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.secondHandSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE_NIGHT,
                    [](std::string valStr, JsiClockBridge& clockBridge) {
                        clockBridge.clockConfig_.secondHandNightSrc_ = valStr;
                    } },
            };
            auto operatorIter =
                BinarySearchFindIndex(clockConfigOperators, ArraySize(clockConfigOperators), keyStr.c_str());
            if (operatorIter != -1) {
                clockConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace OHOS::Ace::Framework
