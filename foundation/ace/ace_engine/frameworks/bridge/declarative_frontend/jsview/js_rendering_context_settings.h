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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDERING_CONTEXT_SETTINGS_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDERING_CONTEXT_SETTINGS_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSRenderingContextSettings : public Referenced {
public:
    JSRenderingContextSettings();
    ~JSRenderingContextSettings() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSRenderingContextSettings* scroller);

    bool GetAlpha()
    {
        return alpha_;
    }
    bool GetAntialias()
    {
        return antialias_;
    }
    void SetAlpha(bool alpha)
    {
        alpha_ = alpha;
    }
    void SetAntialias(bool anti)
    {
        antialias_ = anti;
    }

    ACE_DISALLOW_COPY_AND_MOVE(JSRenderingContextSettings);

private:
    bool alpha_ = false;
    bool antialias_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDERING_CONTEXT_SETTINGS_H
