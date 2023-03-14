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

#include "frameworks/bridge/declarative_frontend/engine/js_types.h"

namespace OHOS::Ace::Framework {

#ifdef USE_V8_ENGINE
void JsStopPropagation(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> thisObj = info.This();
    BaseEventInfo* eventInfo = static_cast<BaseEventInfo*>(thisObj->GetAlignedPointerFromInternalField(0));
    if (eventInfo) {
        eventInfo->SetStopPropagation(true);
    }
}

#elif USE_QUICKJS_ENGINE
JSValue JsStopPropagation(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* arg)
{
    auto eventInfo = static_cast<BaseEventInfo*>(JS_GetOpaque3(new_target));
    if (eventInfo) {
        LOGD("JsStopPropagation is trigger");
        eventInfo->SetStopPropagation(true);
    }
    return JS_UNDEFINED;
}

#elif USE_ARK_ENGINE
Local<JSValueRef> JsStopPropagation(
    EcmaVM* vm, Local<JSValueRef> thisObj, const Local<JSValueRef> argv[], int32_t argc, void* data)
{
    auto eventInfo = static_cast<BaseEventInfo*>(panda::Local<panda::ObjectRef>(thisObj)->GetNativePointerField(0));
    if (eventInfo) {
        LOGD("JsStopPropagation is trigger");
        eventInfo->SetStopPropagation(true);
    }
    return JSValueRef::Undefined(vm);
}
#endif

} // namespace OHOS::Ace::Framework
