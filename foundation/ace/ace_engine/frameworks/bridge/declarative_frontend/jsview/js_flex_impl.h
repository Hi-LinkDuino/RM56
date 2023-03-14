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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_IMPL_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_IMPL_H

#include "core/components/wrap/wrap_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_flex.h"

namespace OHOS::Ace::Framework {

class JSFlexImpl : public JSFlex {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);
    static void JsFlexWidth(const JSCallbackInfo& info);
    static void JsFlexHeight(const JSCallbackInfo& info);
    static void JsFlexSize(const JSCallbackInfo& info);

protected:
    static RefPtr<FlexComponent> CreateFlexComponent(const JSCallbackInfo& info);
    static RefPtr<WrapComponent> CreateWrapComponent(const JSCallbackInfo& info, int32_t wrapVal);
    static void JsFlexWidth(const JSRef<JSVal>& jsValue);
    static void JsFlexHeight(const JSRef<JSVal>& jsValue);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_IMPL_H
