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

#include "bridge/declarative_frontend/jsview/js_clipboard.h"

#include <functional>

#include "base/log/ace_trace.h"
#include "core/common/clipboard/clipboard_proxy.h"
#include "core/common/container.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_clipboard_function.h"

namespace OHOS::Ace::Framework {

void JSClipboard::JSBind(BindingTarget globalObj)
{
    JSClass<JSClipboard>::Declare("JSClipboard");
    JSClass<JSClipboard>::StaticMethod("get", &JSClipboard::Get);
    JSClass<JSClipboard>::StaticMethod("set", &JSClipboard::Set);
    JSClass<JSClipboard>::StaticMethod("clear", &JSClipboard::Clear);
    JSClass<JSClipboard>::Bind(globalObj);
}

void JSClipboard::Get(const JSCallbackInfo& info)
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto executor = container->GetTaskExecutor();
    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(executor);
    if (info[0]->IsFunction()) {
        RefPtr<JsClipboardFunction> callback = AceType::MakeRefPtr<JsClipboardFunction>(JSRef<JSFunc>::Cast(info[0]));
        auto function = [execCtx = info.GetExecutionContext(), callback](const std::string& str) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto func = std::move(callback);
            ACE_SCORING_EVENT("clipboard.get");
            func->Execute(str);
        };
        if (clipboard) {
            clipboard->GetData(function);
        }
    }
}

void JSClipboard::Set(const std::string& data)
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto executor = container->GetTaskExecutor();
    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(executor);
    if (clipboard) {
        clipboard->SetData(data);
    }
}

void JSClipboard::Clear()
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto executor = container->GetTaskExecutor();
    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(executor);
    if (clipboard) {
        clipboard->Clear();
    }
}

} // namespace OHOS::Ace::Framework