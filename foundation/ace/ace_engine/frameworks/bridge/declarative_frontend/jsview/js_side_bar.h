/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SIDE_BAR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SIDE_BAR_H

#include "frameworks/bridge/declarative_frontend/jsview/js_stack.h"
#include "core/components/side_bar/side_bar_container_component.h"

namespace OHOS::Ace::Framework {
class JSSideBar : public JSStack {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void SetShowControlButton(bool isShow);
    static void OnChange(const JSCallbackInfo& info);
    static void JsSideBarWidth(double length);
    static void JsMaxSideBarWidth(double length);
    static void JsMinSideBarWidth(double length);
    static void JsShowSideBar(bool isShow);
    static void JsControlButton(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SIDE_BAR_H