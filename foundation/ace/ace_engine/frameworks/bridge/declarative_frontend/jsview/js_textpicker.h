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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTPICKER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTPICKER_H

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components/picker/picker_text_component.h"

namespace OHOS::Ace::Framework {

enum class PickerStyle {
    INLINE = 0,
    BLOCK,
    FADE,
};

class JSTextPicker : public JSViewAbstract {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);

    static void SetDefaultPickerItemHeight(const JSCallbackInfo& info);

    static void OnAccept(const JSCallbackInfo& info);
    static void OnCancel(const JSCallbackInfo& info);
    static void OnChange(const JSCallbackInfo& info);
};

class JSTextPickerDialog {
public:
    static void JSBind(BindingTarget globalObj);
    static void Show(const JSCallbackInfo& info);
private:
    static void ParseText(RefPtr<PickerTextComponent>& component, const JSRef<JSObject>& paramObj);
    static void AddEvent(RefPtr<PickerTextComponent>& picker, const JSCallbackInfo& info);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTPICKER_H