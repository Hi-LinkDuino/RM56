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

#include "frameworks/bridge/js_frontend/engine/v8/v8_image_animator_bridge.h"

#include "frameworks/bridge/common/dom/dom_image_animator.h"

namespace OHOS::Ace::Framework {

v8::Local<v8::String> V8ImageAnimatorBridge::JsGetState(v8::Isolate* isolate, NodeId nodeId)
{
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    const char* state = "";
    auto task = [nodeId, page, &state]() {
        auto domDoc = (*page)->GetDomDocument();
        if (!domDoc) {
            return;
        }
        auto domImageAnimator = AceType::DynamicCast<DOMImageAnimator>(domDoc->GetDOMNodeById(nodeId));
        if (!domImageAnimator) {
            return;
        }
        state = domImageAnimator->GetState();
    };
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);
    return v8::String::NewFromUtf8(isolate, state).ToLocalChecked();
}

} // namespace OHOS::Ace::Framework