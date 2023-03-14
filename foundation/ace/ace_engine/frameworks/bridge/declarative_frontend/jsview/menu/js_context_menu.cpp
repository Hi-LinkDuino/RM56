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

#include "frameworks/bridge/declarative_frontend/jsview/menu/js_context_menu.h"

#include "core/common/container.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/subwindow/subwindow.h"

namespace OHOS::Ace::Framework {

void JSContextMenu::Close(const JSCallbackInfo& args)
{
#if defined(MULTIPLE_WINDOW_SUPPORTED)
    SubwindowManager::GetInstance()->CloseMenu();
#else
    // Close context menu.
    auto container = Container::Current();
    if (container) {
        auto context = container->GetPipelineContext();
        auto executor = Container::CurrentTaskExecutor();
        if (executor) {
            executor->PostTask(
                [context]() {
                    if (context) {
                        context->CloseContextMenu();
                    }
                },
                TaskExecutor::TaskType::UI);
        }
    }
#endif
    args.SetReturnValue(args.This());
}

void JSContextMenu::JSBind(BindingTarget globalObj)
{
    JSClass<JSContextMenu>::Declare("ContextMenu");
    JSClass<JSContextMenu>::StaticMethod("close", &JSContextMenu::Close);

    JSClass<JSContextMenu>::Inherit<JSViewAbstract>();
    JSClass<JSContextMenu>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
