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

#include "frameworks/bridge/js_frontend/engine/quickjs/list_bridge.h"

#include "frameworks/bridge/common/dom/dom_list.h"

namespace OHOS::Ace::Framework {

JSValue ListBridge::JsGetCurrentOffset(JSContext* ctx, NodeId nodeId)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
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
    instance->GetDelegate()->PostSyncTaskToPage(task);
    JSValue offsetContext = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, offsetContext, "x", JS_NewFloat64(ctx, offsetX));
    JS_SetPropertyStr(ctx, offsetContext, "y", JS_NewFloat64(ctx, offsetY));
    return offsetContext;
}

} // namespace OHOS::Ace::Framework
