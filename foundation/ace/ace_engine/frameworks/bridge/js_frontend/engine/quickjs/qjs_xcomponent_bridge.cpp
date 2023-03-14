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

#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_xcomponent_bridge.h"

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"
#include "frameworks/bridge/js_frontend/js_command.h"
#include "frameworks/core/common/ace_view.h"
#include "frameworks/core/common/container.h"

namespace OHOS::Ace::Framework {
QjsXComponentBridge::QjsXComponentBridge()
{
    nativeXComponentImpl_ = AceType::MakeRefPtr<NativeXComponentImpl>();
    nativeXComponent_ = new OH_NativeXComponent(AceType::RawPtr(nativeXComponentImpl_));
    renderContext_ = JS_NULL;
}

QjsXComponentBridge::~QjsXComponentBridge()
{
    if (nativeXComponent_) {
        delete nativeXComponent_;
        nativeXComponent_ = nullptr;
    }
}

void QjsXComponentBridge::HandleContext(JSContext* ctx, NodeId id, const std::string& args)
{
    if (hasPluginLoaded_) {
        return;
    }

    auto engine = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (engine == nullptr) {
        LOGE("QjsXComponentBridge::HandleJsContext, engine is null.");
        return;
    }

    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("QjsXComponentBridge page is null.");
        return;
    }
    auto domXcomponent = AceType::DynamicCast<DOMXComponent>(page->GetDomDocument()->GetDOMNodeById(id));
    if (!domXcomponent) {
        LOGE("QjsXComponentBridge domXcomponent is null.");
        return;
    }
    auto xcomponent = AceType::DynamicCast<XComponentComponent>(domXcomponent->GetSpecializedComponent());
    if (!xcomponent) {
        LOGE("QjsXComponentBridge xcomponent is null.");
        return;
    }
    auto textureId = static_cast<int64_t>(xcomponent->GetTextureId());

    auto container = Container::Current();
    if (!container) {
        LOGE("QjsXComponentBridge Current container null");
        return;
    }
    auto nativeView = static_cast<AceView*>(container->GetView());
    if (!nativeView) {
        LOGE("QjsXComponentBridge nativeView null");
        return;
    }
    auto nativeWindow = const_cast<void*>(nativeView->GetNativeWindowById(textureId));
    if (!nativeWindow) {
        LOGE("QjsXComponentBridge::HandleJsContext nativeWindow invalid");
        return;
    }

    nativeXComponentImpl_->SetSurface(nativeWindow);
    nativeXComponentImpl_->SetXComponentId(xcomponent->GetId());

#ifdef XCOMPONENT_SUPPORTED
    auto nativeEngine = static_cast<QuickJSNativeEngine*>(engine->GetQuickJSNativeEngine());
    if (nativeEngine == nullptr) {
        LOGE("nativeEngine is null");
        return;
    }

    renderContext_ = nativeEngine->LoadModuleByName(xcomponent->GetLibraryName(), true,
                                                    args, OH_NATIVE_XCOMPONENT_OBJ,
                                                    reinterpret_cast<void*>(nativeXComponent_));

    auto delegate = engine->GetDelegate();
    if (delegate == nullptr) {
        LOGE("delegate is null.");
        return;
    }
    auto task = [weak = WeakClaim(this), xcomponent]() {
        auto pool = xcomponent->GetTaskPool();
        if (!pool) {
            return;
        }
        auto bridge = weak.Upgrade();
        if (bridge) {
            pool->NativeXComponentInit(bridge->nativeXComponent_,
                                       AceType::WeakClaim(AceType::RawPtr(bridge->nativeXComponentImpl_)));
        }
    };
    delegate->PostSyncTaskToPage(task);
#endif
    hasPluginLoaded_ = true;
    return;
}
} // namespace OHOS::Ace::Framework