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

#include "frameworks/bridge/js_frontend/engine/quickjs/clock_bridge.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

void ClockBridge::ParseClockConfig(JSContext* ctx, JSValueConst valObject)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val) || JS_IsNumber(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            // static linear map must be sorted by key.
            static const LinearMapNode<void (*)(const char*, ClockBridge&)> clockConfigOperators[] = {
                { DOM_DIGIT_COLOR,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitColor_ = valStr;
                    } },
                { DOM_DIGIT_COLOR_NIGHT,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitColorNight_ = valStr;
                    } },
                { DOM_DIGIT_RADIUS_RATIO,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitRadiusRatio_ = StringToDouble(valStr);
                    } },
                { DOM_DIGIT_SIZE_RATIO,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.digitSizeRatio_ = StringToDouble(valStr);
                    } },
                { DOM_CLOCK_FACE_SOURCE,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.clockFaceSrc_ = valStr;
                    } },
                { DOM_CLOCK_FACE_SOURCE_NIGHT,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.clockFaceNightSrc_ = valStr;
                    } },
                { DOM_HOUR_HAND_SOURCE, [](const char* valStr,
                                        ClockBridge& clockBridge) { clockBridge.clockConfig_.hourHandSrc_ = valStr; } },
                { DOM_HOUR_HAND_SOURCE_NIGHT,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.hourHandNightSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.minuteHandSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE_NIGHT,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.minuteHandNightSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.secondHandSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE_NIGHT,
                    [](const char* valStr, ClockBridge& clockBridge) {
                        clockBridge.clockConfig_.secondHandNightSrc_ = valStr;
                    } },
            };
            auto operatorIter = BinarySearchFindIndex(clockConfigOperators, ArraySize(clockConfigOperators), key);
            if (operatorIter != -1) {
                clockConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

} // namespace OHOS::Ace::Framework
