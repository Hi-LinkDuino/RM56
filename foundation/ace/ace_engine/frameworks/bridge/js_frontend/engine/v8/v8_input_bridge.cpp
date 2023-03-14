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

#include "frameworks/bridge/js_frontend/engine/v8/v8_input_bridge.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

void GetInputOption(const v8::Local<v8::Context>& ctx, const v8::Local<v8::Value>& valObject, InputOption& inputOption)
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
        if (val->IsNumber() || val->IsBoolean() || val->IsString()) {
            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
            const char* valStr = *valV8Str;
            if (valStr == nullptr) {
                continue;
            }
            if (strcmp(keyStr, DOM_INPUT_OPTION_ICON) == 0) {
                inputOption.image = valStr;
            } else if (strcmp(keyStr, DOM_INPUT_OPTION_CONTENT) == 0) {
                inputOption.text = valStr;
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace

void V8InputBridge::ParseInputOptions(const v8::Local<v8::Context>& ctx, const v8::Local<v8::Value>& valArray)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> v8ValObj;
    if (!valArray->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    for (int32_t i = 0; i < len; ++i) {
        v8::Local<v8::Value> itemKey = properties->Get(ctx, i).ToLocalChecked();
        v8::Local<v8::Value> itemVal = v8ValObj->Get(ctx, itemKey).ToLocalChecked();
        InputOption inputOption;
        if (itemVal->IsObject()) {
            GetInputOption(ctx, itemVal, inputOption);
            inputOptions_.push_back(inputOption);
        }
    }
}

} // namespace OHOS::Ace::Framework