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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_ITEM_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_ITEM_H

#include "core/components/touch_listener/touch_listener_component.h"
#include "core/event/ace_event_handler.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/core/components/grid_layout/grid_layout_item_component.h"

namespace OHOS::Ace::Framework {

class JSGridItem : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create();

protected:
    static void SetColumnStart(int32_t columnStart);
    static void SetColumnEnd(int32_t columnEnd);
    static void SetRowStart(int32_t rowStart);
    static void SetRowEnd(int32_t rowEnd);
    static void ForceRebuild(bool forceRebuild);
    static void SetSelectable(bool selectable);
    static void SelectCallback(const JSCallbackInfo& args);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_ITEM_H
