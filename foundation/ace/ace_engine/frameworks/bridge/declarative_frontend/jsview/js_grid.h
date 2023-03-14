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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_H

#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/core/components/grid_layout/grid_layout_component.h"

namespace OHOS::Ace::Framework {

class JSGrid : public JSContainerBase {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);

    static void SetColumnsTemplate(const std::string& value);
    static void SetRowsTemplate(const std::string& value);
    static void SetColumnsGap(const JSCallbackInfo& info);
    static void SetRowsGap(const JSCallbackInfo& info);
    static void SetScrollBar(int32_t displayMode);
    static void SetScrollBarColor(const std::string& color);
    static void SetScrollBarWidth(const std::string& scrollWidth);
    static void JsOnScrollIndex(const JSCallbackInfo& info);
    static void SetCachedCount(int32_t cachedCount);
    static void SetEditMode(bool editMode);
    static void SetMultiSelectable(bool multiSelectable);
    static void SetMaxCount(int32_t maxCount);
    static void SetMinCount(int32_t minCount);
    static void CellLength(int32_t cellLength);
    static void SetLayoutDirection(int32_t value);
    static void SetDirection(const std::string& dir);
    static void SetSupportAnimation(bool supportAnimation);
    static void SetDragAnimation(bool value);
    static void SetEdgeEffection(int32_t value);
    static void JsOnGridDragEnter(const JSCallbackInfo& info);
    static void JsOnGridDragMove(const JSCallbackInfo& info);
    static void JsOnGridDragLeave(const JSCallbackInfo& info);
    static void JsOnGridDragStart(const JSCallbackInfo& info);
    static void JsOnGridDrop(const JSCallbackInfo& info);
    static void JsGridHeight(const JSCallbackInfo& info);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_H
