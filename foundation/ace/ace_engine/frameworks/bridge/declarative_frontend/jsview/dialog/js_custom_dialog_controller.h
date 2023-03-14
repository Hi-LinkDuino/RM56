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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_DIALOG_JS_CUSTOM_DIALOG_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_DIALOG_JS_CUSTOM_DIALOG_CONTROLLER_H

#include "core/components/dialog/dialog_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view.h"

namespace OHOS::Ace::Framework {

enum class DialogOperation {
    DIALOG_OPEN = 0,
    DIALOG_CLOSE,
};

class JSCustomDialogController {
public:
    explicit JSCustomDialogController(JSView* ownerView) : ownerView_(ownerView) {}
    ~JSCustomDialogController() = default;

    static void JSBind(BindingTarget object);
    static void ConstructorCallback(const JSCallbackInfo& args);
    static void DestructorCallback(JSCustomDialogController* instance);

    void JsOpenDialog(const JSCallbackInfo& info);
    void JsCloseDialog(const JSCallbackInfo& info);

private:
    void ShowDialog();
    void CloseDialog();
    void NotifyDialogOperation(DialogOperation operation);

    JSView* ownerView_ = nullptr;
    bool isShown_ = false;
    bool pending_ = false;
    DialogProperties dialogProperties_;
    RefPtr<JsFunction> jsBuilderFunction_;
    RefPtr<JsFunction> jsCancelFunction_;
    RefPtr<Component> customDialog_;
    RefPtr<DialogComponent> dialogComponent_;
    std::list<DialogOperation> dialogOperation_;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_DIALOG_JS_CUSTOM_DIALOG_CONTROLLER_H