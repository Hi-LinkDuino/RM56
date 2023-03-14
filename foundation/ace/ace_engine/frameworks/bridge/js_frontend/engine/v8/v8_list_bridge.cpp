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

#include "frameworks/bridge/js_frontend/engine/v8/v8_list_bridge.h"

#include "frameworks/bridge/common/dom/dom_list.h"

namespace OHOS::Ace::Framework {

v8::Local<v8::Object> V8ListBridge::JsGetCurrentOffset(v8::Isolate* isolate, NodeId nodeId)
{
    if (!isolate) {
        return v8::Local<v8::Object>();
    }
    double offsetX = 0.0;
    double offsetY = 0.0;
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    auto task = [nodeId, page, &offsetX, &offsetY]() {
        auto domDoc = (*page)->GetDomDocument();
        if (!domDoc) {
            return;
        }

        auto domList = AceType::DynamicCast<DOMList>(domDoc->GetDOMNodeById(nodeId));
        if (!domList) {
            return;
        }
        auto offset = domList->GetCurrentOffset();
        offsetX = offset.GetX();
        offsetY = offset.GetY();
    };

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);

    auto ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> offsetContext = v8::Object::New(ctx->GetIsolate());
    offsetContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "x").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), offsetX))
        .ToChecked();
    offsetContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "y").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), offsetY))
        .ToChecked();
    return offsetContext;
}

} // namespace OHOS::Ace::Framework
