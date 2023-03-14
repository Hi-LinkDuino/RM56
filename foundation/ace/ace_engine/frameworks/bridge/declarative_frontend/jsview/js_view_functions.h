/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_FUNCTIONS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_FUNCTIONS_H

#include <string>

#include "core/pipeline/base/composed_component.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace {
  class ComposedElement;
} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {

class JSView;

class ViewFunctions : public AceType {
    DECLARE_ACE_TYPE(ViewFunctions, AceType);

public:
    ViewFunctions(JSRef<JSObject> jsObject, JSRef<JSFunc> jsRenderFunction);
    ~ViewFunctions()
    {
        LOGD("Destroy: ViewFunctions");
    }

    void Destroy(JSView* parentCustomView);

    void ExecuteRender();
    void ExecuteAppear();
    void ExecuteDisappear();
    void ExecuteAboutToBeDeleted();
    void ExecuteAboutToRender();
    void ExecuteOnRenderDone();
    void ExecuteTransition();
    bool ExecuteOnBackPress();
    void ExecuteShow();
    void ExecuteHide();
    void ExecuteUpdateWithValueParams(const std::string& jsonData);

    bool HasPageTransition() const;

    void ExecuteFunction(JSWeak<JSFunc>& func, const char* debugInfo);
    void ExecuteFunctionWithParams(JSWeak<JSFunc>& func, const char* debugInfo, const std::string& jsonData);
    JSRef<JSVal> ExecuteFunctionWithReturn(JSWeak<JSFunc>& func, const char* debugInfo);

    void SetContext(const JSExecutionContext& context)
    {
        context_ = context;
    }

private:
    JSWeak<JSObject> jsObject_;
    JSWeak<JSFunc> jsAppearFunc_;
    JSWeak<JSFunc> jsDisappearFunc_;
    JSWeak<JSFunc> jsAboutToRenderFunc_;
    JSWeak<JSFunc> jsAboutToBeDeletedFunc_;
    JSWeak<JSFunc> jsRenderDoneFunc_;
    JSWeak<JSFunc> jsAboutToBuildFunc_;
    JSWeak<JSFunc> jsBuildDoneFunc_;
    JSWeak<JSFunc> jsRenderFunc_;
    JSWeak<JSFunc> jsTransitionFunc_;
    JSWeak<JSVal> jsRenderResult_;

    JSWeak<JSFunc> jsOnHideFunc_;
    JSWeak<JSFunc> jsOnShowFunc_;
    JSWeak<JSFunc> jsBackPressFunc_;
    JSWeak<JSFunc> jsUpdateWithValueParamsFunc_;

    JSExecutionContext context_;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_VIEW_FUNCTIONS_H