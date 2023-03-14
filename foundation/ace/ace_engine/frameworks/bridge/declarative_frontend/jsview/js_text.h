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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_H

#include "core/components/text/text_component_v2.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSText : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void SetWidth(const JSCallbackInfo& info);
    static void SetHeight(const JSCallbackInfo& info);
    static void SetFontSize(const JSCallbackInfo& info);
    static void SetFontWeight(const std::string& value);
    static void SetTextColor(const JSCallbackInfo& info);
    static void SetTextOverflow(const JSCallbackInfo& info);
    static void SetMaxLines(int32_t value);
    static void SetFontStyle(int32_t value);
    static void SetTextAlign(int32_t value);
    static void SetLineHeight(const JSCallbackInfo& info);
    static void SetFontFamily(const JSCallbackInfo& info);
    static void SetMinFontSize(const JSCallbackInfo& info);
    static void SetMaxFontSize(const JSCallbackInfo& info);
    static void SetLetterSpacing(const JSCallbackInfo& info);
    static void SetTextCase(int32_t value);
    static void SetBaselineOffset(const JSCallbackInfo& info);
    static void SetDecoration(const JSCallbackInfo& info);
    static void JsOnClick(const JSCallbackInfo& info);
    static void JsRemoteMessage(const JSCallbackInfo& info);

private:
    static RefPtr<TextComponentV2> GetComponent();
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXT_H
