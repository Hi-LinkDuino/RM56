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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LIST_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LIST_H

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {

class JSList : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& args);

    static void JsWidth(const JSCallbackInfo& info);
    static void JsHeight(const JSCallbackInfo& info);
    static void ScrollCallback(const JSCallbackInfo& args);
    static void ReachStartCallback(const JSCallbackInfo& args);
    static void ReachEndCallback(const JSCallbackInfo& args);
    static void ScrollStopCallback(const JSCallbackInfo& args);
    static void ItemDeleteCallback(const JSCallbackInfo& args);
    static void ItemMoveCallback(const JSCallbackInfo& args);
    static void ScrollIndexCallback(const JSCallbackInfo& args);

    static void SetDivider(const JSCallbackInfo& args);
    static void SetDirection(int32_t direction);
    static void SetScrollBar(int32_t scrollBar);
    static void SetEdgeEffect(int32_t edgeEffect);
    static void SetEditMode(bool editMode);
    static void SetCachedCount(int32_t cachedCount);
    static void SetChainAnimation(bool enableChainAnimation);
    static void SetMultiSelectable(bool multiSelectable);

    static void ItemDragStartCallback(const JSCallbackInfo& info);
    static void ItemDragEnterCallback(const JSCallbackInfo& info);
    static void ItemDragMoveCallback(const JSCallbackInfo& info);
    static void ItemDragLeaveCallback(const JSCallbackInfo& info);
    static void ItemDropCallback(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_LIST_H
