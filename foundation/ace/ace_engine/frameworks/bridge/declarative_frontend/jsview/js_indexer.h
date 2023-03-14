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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INDEXER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INDEXER_H

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {
class JSIndexer : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& args);
    static void JsOnSelected(const JSCallbackInfo& args);
    static void SetColor(const JSCallbackInfo& args);
    static void SetSelectedColor(const JSCallbackInfo& args);
    static void SetPopupColor(const JSCallbackInfo& args);
    static void SetSelectedBackgroundColor(const JSCallbackInfo& args);
    static void SetPopupBackground(const JSCallbackInfo& args);
    static void SetUsingPopup(bool state);
    static void SetSelectedFont(const JSCallbackInfo& args);
    static void SetFont(const JSCallbackInfo& args);
    static void SetPopupFont(const JSCallbackInfo& args);
    static void GetFontContent(const JSCallbackInfo& args, TextStyle& textStyle);
    static void SetItemSize(const JSCallbackInfo& args);
    static void SetAlignStyle(int32_t value);
    static void JsOnRequestPopupData(const JSCallbackInfo& args);
    static void JsOnPopupSelected(const JSCallbackInfo& args);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INDEXER_H
