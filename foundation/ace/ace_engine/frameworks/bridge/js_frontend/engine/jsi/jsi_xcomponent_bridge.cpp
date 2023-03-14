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

#include <memory>

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_xcomponent_bridge.h"

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/jsi/ark_js_value.h"
#include "frameworks/bridge/js_frontend/js_command.h"
#include "frameworks/core/common/ace_view.h"
#include "frameworks/core/common/container.h"

namespace OHOS::Ace::Framework {
JsiXComponentBridge::JsiXComponentBridge()
{
    nativeXcomponentImpl_ = AceType::MakeRefPtr<NativeXComponentImpl>();
    nativeXComponent_ = new OH_NativeXComponent(AceType::RawPtr(nativeXcomponentImpl_));
}

JsiXComponentBridge::~JsiXComponentBridge()
{
    if (nativeXComponent_) {
        delete nativeXComponent_;
        nativeXComponent_ = nullptr;
    }
}

void JsiXComponentBridge::HandleContext(const shared_ptr<JsRuntime>& runtime, NodeId id, const std::string& args)
{
    if (hasPluginLoaded_) {
        return;
    }

    if (!runtime) {
        LOGE("Runtime is null");
        return;
    }

    auto engineInstance = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    auto page = engineInstance->GetRunningPage();
    if (!page) {
        LOGE("JsiXComponentBridge page is null.");
        return;
    }
    auto domXcomponent = AceType::DynamicCast<DOMXComponent>(page->GetDomDocument()->GetDOMNodeById(id));
    if (!domXcomponent) {
        LOGE("JsiXComponentBridge domXcomponent is null.");
        return;
    }
    auto xcomponent = AceType::DynamicCast<XComponentComponent>(domXcomponent->GetSpecializedComponent());
    if (!xcomponent) {
        LOGE("JsiXComponentBridge xcomponent is null.");
        return;
    }
    auto textureId = static_cast<int64_t>(xcomponent->GetTextureId());

    auto container = Container::Current();
    if (!container) {
        LOGE("JsiXComponentBridge Current container null");
        return;
    }
    auto nativeView = static_cast<AceView*>(container->GetView());
    if (!nativeView) {
        LOGE("JsiXComponentBridge nativeView null");
        return;
    }
    auto nativeWindow = const_cast<void*>(nativeView->GetNativeWindowById(textureId));
    if (!nativeWindow) {
        LOGE("JsiXComponentBridge::HandleJsContext nativeWindow invalid");
        return;
    }
    nativeXcomponentImpl_->SetSurface(nativeWindow);
    nativeXcomponentImpl_->SetXComponentId(xcomponent->GetId());

    auto nativeEngine = static_cast<ArkNativeEngine*>(engineInstance->GetNativeEngine());
    if (!nativeEngine) {
        LOGE("NativeEngine is null");
        return;
    }

    auto arkObjectRef = nativeEngine->LoadModuleByName(xcomponent->GetLibraryName(), true,
                                                       args, OH_NATIVE_XCOMPONENT_OBJ,
                                                       reinterpret_cast<void*>(nativeXComponent_));

    if (arkObjectRef.CheckException()) {
        LOGE("LoadModuleByName failed.");
        return;
    }
    renderContext_ = runtime->NewObject();
    auto renderContext = std::static_pointer_cast<ArkJSValue>(renderContext_);
    shared_ptr<ArkJSRuntime> pandaRuntime = std::static_pointer_cast<ArkJSRuntime>(runtime);
    LocalScope scope(pandaRuntime->GetEcmaVm());
    Local<ObjectRef> obj = arkObjectRef->ToObject(pandaRuntime->GetEcmaVm());
    if (obj.CheckException()) {
        LOGE("Get local object failed.");
        return;
    }
    renderContext->SetValue(pandaRuntime, obj);

    auto task = [weak = WeakClaim(this), xcomponent]() {
        auto pool = xcomponent->GetTaskPool();
        if (!pool) {
            return;
        }
        auto bridge = weak.Upgrade();
        if (bridge) {
            pool->NativeXComponentInit(
                bridge->nativeXComponent_,
                AceType::WeakClaim(AceType::RawPtr(bridge->nativeXcomponentImpl_)));
        }
    };

    auto delegate = engineInstance->GetFrontendDelegate();
    if (!delegate) {
        LOGE("Delegate is null");
        return;
    }
    delegate->PostSyncTaskToPage(task);

    hasPluginLoaded_ = true;
    return;
}

#ifdef OHOS_STANDARD_SYSTEM
shared_ptr<JsValue> JsiXComponentBridge::JsGetXComponentSurfaceId(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetXComponentSurfaceId failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetXComponentSurfaceId failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetXComponentSurfaceId failed. page is null.");
        return runtime->NewUndefined();
    }
    uint64_t surfaceId = 0;
    auto task = [nodeId, page, &surfaceId]() {
        auto domDoc = page->GetDomDocument();
        if (!domDoc) {
            return;
        }
        auto domXComponent = AceType::DynamicCast<DOMXComponent>(domDoc->GetDOMNodeById(nodeId));
        if (!domXComponent) {
            return;
        }
        surfaceId = domXComponent->GetSurfaceId();
    };
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetXComponentSurfaceId failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);
    return runtime->NewString(std::to_string(surfaceId));
}
#endif
} // namespace OHOS::Ace::Framework