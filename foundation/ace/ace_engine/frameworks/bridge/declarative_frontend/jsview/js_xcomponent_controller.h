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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_CONTROLLER_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "core/components/xcomponent/xcomponent_component.h"

namespace OHOS::Ace::Framework {
class JSXComponentController : public Referenced {
public:
    JSXComponentController() = default;
    ~JSXComponentController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSXComponentController* xcomponentController);

    void GetSurfaceId(const JSCallbackInfo& args);
    void SetSurfaceConfig(const JSCallbackInfo& args);

    void GetXComponentContext(const JSCallbackInfo& args)
    {
        args.SetReturnValue(renderContext_);
    }

    void SetXComponentContext(const JSRef<JSVal> &renderContext)
    {
        renderContext_ = renderContext;
    }

    RefPtr<XComponentController> GetController() const
    {
        return xcomponentController_;
    }

    void SetController(const RefPtr<XComponentController>& xcomponentController)
    {
        xcomponentController_ = xcomponentController;
    }

private:
    RefPtr<XComponentController> xcomponentController_;
    JSRef<JSVal> renderContext_;
    ACE_DISALLOW_COPY_AND_MOVE(JSXComponentController);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_XCOMPONENT_CONTROLLER_H