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

#include "frameworks/bridge/js_frontend/engine/quickjs/image_animator_bridge.h"

#include "frameworks/bridge/common/dom/dom_image_animator.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"

namespace OHOS::Ace::Framework {

JSValue ImageAnimatorBridge::JsGetState(JSContext* ctx, NodeId nodeId)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return JS_NULL;
    }
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }
    const char* state = "";
    auto task = [nodeId, page, &state]() {
        auto domDoc = page->GetDomDocument();
        if (!domDoc) {
            return;
        }
        auto domImageAnimator = AceType::DynamicCast<DOMImageAnimator>(domDoc->GetDOMNodeById(nodeId));
        if (!domImageAnimator) {
            return;
        }
        state = domImageAnimator->GetState();
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);
    return JS_NewString(ctx, state);
}

} // namespace OHOS::Ace::Framework
