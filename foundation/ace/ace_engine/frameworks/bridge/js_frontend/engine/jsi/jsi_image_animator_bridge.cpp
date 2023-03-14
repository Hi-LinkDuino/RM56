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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_image_animator_bridge.h"

#include "frameworks/bridge/common/dom/dom_image_animator.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

namespace OHOS::Ace::Framework {

shared_ptr<JsValue> JsiImageAnimatorBridge::JsGetState(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetState failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetState failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetState failed. page is null.");
        return runtime->NewUndefined();
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetCurrentOffset failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);
    return runtime->NewString(state);
}

} // namespace OHOS::Ace::Framework
