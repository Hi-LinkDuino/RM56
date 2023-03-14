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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SHAPE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SHAPE_H

#include "core/components/shape/shape_container_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {

class JSShape : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void SetViewPort(const JSCallbackInfo& info);

    static void JsWidth(const JSCallbackInfo& info);
    static void JsHeight(const JSCallbackInfo& info);
    static void JsWidth(const JSRef<JSVal>& jsValue);
    static void JsHeight(const JSRef<JSVal>& jsValue);
    static void JsSize(const JSCallbackInfo& info);

    static void SetStroke(const JSCallbackInfo& info);
    static void SetFill(const JSCallbackInfo& info);
    static void SetStrokeDashOffset(const JSCallbackInfo& info);
    static void SetStrokeLineCap(int lineCap);
    static void SetStrokeLineJoin(int lineJoin);
    static void SetStrokeMiterLimit(const JSCallbackInfo& info);
    static void SetStrokeOpacity(const JSCallbackInfo& info);
    static void SetFillOpacity(const JSCallbackInfo& info);
    static void SetStrokeWidth(const JSCallbackInfo& info);
    static void SetAntiAlias(bool antiAlias);
    static void SetStrokeDashArray(const JSCallbackInfo& info);
    static void SetBitmapMesh(const JSCallbackInfo& info);

private:
    static void InitBox(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SHAPE_H
