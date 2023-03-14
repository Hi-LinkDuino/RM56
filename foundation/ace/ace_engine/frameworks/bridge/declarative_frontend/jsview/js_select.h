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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SELECT_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SELECT_H

#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {
class JSSelect : public JSViewAbstract {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);

    static void Selected(int value);
    static void Value(const std::string& value);
    static void Font(const JSCallbackInfo& info);
    static void FontColor(const JSCallbackInfo& info);
    static void SelectedOptionBgColor(const JSCallbackInfo& info);
    static void SelectedOptionFont(const JSCallbackInfo& info);
    static void SelectedOptionFontColor(const JSCallbackInfo& info);
    static void OptionBgColor(const JSCallbackInfo& info);
    static void OptionFont(const JSCallbackInfo& info);
    static void OptionFontColor(const JSCallbackInfo& info);
    static void OnSelected(const JSCallbackInfo& info);
    static void JsWidth(const JSCallbackInfo& info);
    static void JsHeight(const JSCallbackInfo& info);
    static void Width(const JSRef<JSVal>& jsValue);
    static void Height(const JSRef<JSVal>& jsValue);
    static void JsSize(const JSCallbackInfo& info);
    static void JsPadding(const JSCallbackInfo& info);
    static void SetPaddingTop(const JSCallbackInfo& info);
    static void SetPaddingBottom(const JSCallbackInfo& info);
    static void SetPaddingLeft(const JSCallbackInfo& info);
    static void SetPaddingRight(const JSCallbackInfo& info);
};
} // namespace OHOS::Ace::Framework
#endif
