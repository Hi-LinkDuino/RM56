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

#include "bridge/declarative_frontend/jsview/js_rendering_context_settings.h"
#include "bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSRenderingContextSettings::JSRenderingContextSettings()
{
}

void JSRenderingContextSettings::Constructor(const JSCallbackInfo& args)
{
    auto jsRenderingContextSettings = Referenced::MakeRefPtr<JSRenderingContextSettings>();
    jsRenderingContextSettings->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsRenderingContextSettings));
    bool anti = false;
    bool alpha = false;
    if (args.Length() > 0) {
        JSViewAbstract::ParseJsBool(args[0], anti);
    }
    if (args.Length() > 1) {
        JSViewAbstract::ParseJsBool(args[1], alpha);
    }
    jsRenderingContextSettings->SetAntialias(anti);
    jsRenderingContextSettings->SetAlpha(alpha);
}

void JSRenderingContextSettings::Destructor(JSRenderingContextSettings* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

void JSRenderingContextSettings::JSBind(BindingTarget globalObj)
{
    JSClass<JSRenderingContextSettings>::Declare("RenderingContextSettings");
    JSClass<JSRenderingContextSettings>::Bind(globalObj, JSRenderingContextSettings::Constructor,
        JSRenderingContextSettings::Destructor);
}

} // namespace OHOS::Ace::Framework
