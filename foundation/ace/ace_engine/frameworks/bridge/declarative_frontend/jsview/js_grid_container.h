/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_CONTAINER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_CONTAINER_H

#include "bridge/declarative_frontend/jsview/js_column.h"
#include "core/components/common/layout/grid_container_info.h"

namespace OHOS::Ace::Framework {
class JSGridContainer : public JSColumn {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static RefPtr<GridContainerInfo> GetContainer()
    {
        return gridContainerStack_.empty() ? nullptr : gridContainerStack_.back();
    }
    static void Pop();

private:
    static thread_local std::vector<RefPtr<GridContainerInfo>> gridContainerStack_;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_GRID_CONTAINER_H
