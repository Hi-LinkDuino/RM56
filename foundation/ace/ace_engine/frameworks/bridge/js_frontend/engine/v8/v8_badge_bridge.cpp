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

#include "frameworks/bridge/js_frontend/engine/v8/v8_badge_bridge.h"

namespace OHOS::Ace::Framework {

void V8BadgeBridge::ParseBadgeConfig(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    if (isolate == nullptr) {
        LOGE("isolate is none");
        return;
    }
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
            static const LinearMapNode<void (*)(const char*, V8BadgeBridge&)> badgeConfigOperators[] = {
                { DOM_BADGE_COLOR,
                    [](const char* valStr, V8BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.badgeColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_CIRCLE_SIZE,
                    [](const char* valStr, V8BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.badgeSize = { StringToDimension(valStr), true };
                    } },
                { DOM_BADGE_TEXT_COLOR,
                    [](const char* valStr, V8BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.textColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_TEXT_FONT_SIZE,
                    [](const char* valStr, V8BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.textSize = { StringToDimension(valStr), true };
                    } }
            };
            auto operatorIter = BinarySearchFindIndex(badgeConfigOperators, ArraySize(badgeConfigOperators), keyStr);
            if (operatorIter != -1) {
                badgeConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace OHOS::Ace::Framework