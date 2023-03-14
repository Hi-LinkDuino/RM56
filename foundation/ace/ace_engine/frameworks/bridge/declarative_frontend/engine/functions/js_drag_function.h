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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_DRAG_FUNCTION_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_DRAG_FUNCTION_H

#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {
class JsDragFunction : public JsFunction {
    DECLARE_ACE_TYPE(JsDragFunction, JsFunction)

public:
    explicit JsDragFunction(const JSRef<JSFunc>& jsFunction) : JsFunction(JSRef<JSObject>(), jsFunction) {}
    static void JSBind(BindingTarget globalObj);

    ~JsDragFunction() override
    {
        LOGD("Destroy: JsDragFunction");
    };
    void Execute();
    void Execute(const RefPtr<DragEvent>& info);
    JSRef<JSVal> ItemDragStartExecute(const ItemDragInfo& info, int32_t itemIndex);
    void ItemDragEnterExecute(const ItemDragInfo& info);
    void ItemDragMoveExecute(const ItemDragInfo& info, int32_t itemIndex, int32_t insertIndex);
    void ItemDragLeaveExecute(const ItemDragInfo& info, int32_t itemIndex);
    void ItemDropExecute(const ItemDragInfo& info, int32_t itemIndex, int32_t insertIndex, bool isSuccess);
    JSRef<JSVal> Execute(const RefPtr<DragEvent>& info, const std::string& extraParams);

private:
    JSRef<JSObject> CreateDragEvent(const RefPtr<DragEvent>& info);
    JSRef<JSObject> CreatePasteData(const RefPtr<PasteData>& info);
    JSRef<JSObject> CreateItemDragInfo(const ItemDragInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_FUNCTION_V8_JS_DRAG_FUNCTION_H
