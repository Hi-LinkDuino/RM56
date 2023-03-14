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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_list_bridge.h"

#include "frameworks/bridge/common/dom/dom_list.h"

namespace OHOS::Ace::Framework {

shared_ptr<JsValue> JsiListBridge::JsGetCurrentOffset(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetCurrentOffset failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetCurrentOffset failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetCurrentOffset failed. page is null.");
        return runtime->NewUndefined();
    }

    double offsetX = 0.0;
    double offsetY = 0.0;
    auto task = [nodeId, page, &offsetX, &offsetY]() {
        auto domDoc = page->GetDomDocument();
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

    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetCurrentOffset failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);
    shared_ptr<JsValue> offsetContext = runtime->NewObject();
    offsetContext->SetProperty(runtime, "x", runtime->NewNumber(offsetX));
    offsetContext->SetProperty(runtime, "y", runtime->NewNumber(offsetY));
    return offsetContext;
}

} // namespace OHOS::Ace::Framework
