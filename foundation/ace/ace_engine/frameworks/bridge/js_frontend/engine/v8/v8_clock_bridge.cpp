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

#include "frameworks/bridge/js_frontend/engine/v8/v8_clock_bridge.h"

#include "base/utils/linear_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

void V8ClockBridge::ParseClockConfig(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsString() || val->IsNumber()) {
            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
            const char* valStr = *valV8Str;
            if (valStr == nullptr) {
                continue;
            }
            // static linear map must be sorted by key.
            static const LinearMapNode<void (*)(const char*, V8ClockBridge&)> clockConfigOperators[] = {
                { DOM_DIGIT_COLOR,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.digitColor_ = valStr;
                    } },
                { DOM_DIGIT_COLOR_NIGHT,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.digitColorNight_ = valStr;
                    } },
                { DOM_DIGIT_RADIUS_RATIO,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.digitRadiusRatio_ = StringToDouble(valStr);
                    } },
                { DOM_DIGIT_SIZE_RATIO,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.digitSizeRatio_ = StringToDouble(valStr);
                    } },
                { DOM_CLOCK_FACE_SOURCE,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.clockFaceSrc_ = valStr;
                    } },
                { DOM_CLOCK_FACE_SOURCE_NIGHT,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.clockFaceNightSrc_ = valStr;
                    } },
                { DOM_HOUR_HAND_SOURCE,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.hourHandSrc_ = valStr;
                    } },
                { DOM_HOUR_HAND_SOURCE_NIGHT,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.hourHandNightSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.minuteHandSrc_ = valStr;
                    } },
                { DOM_MINUTE_HAND_SOURCE_NIGHT,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.minuteHandNightSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.secondHandSrc_ = valStr;
                    } },
                { DOM_SECOND_HAND_SOURCE_NIGHT,
                    [](const char* valStr, V8ClockBridge& v8ClockBridge) {
                        v8ClockBridge.clockConfig_.secondHandNightSrc_ = valStr;
                    } },
            };
            auto operatorIter = BinarySearchFindIndex(clockConfigOperators, ArraySize(clockConfigOperators), keyStr);
            if (operatorIter != -1) {
                clockConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace OHOS::Ace::Framework
