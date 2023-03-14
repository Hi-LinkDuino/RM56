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

#if (defined(SYSTEM_CLIPBOARD_SUPPORTED))
#include "pasteboard_client.h"
#endif

#include "core/common/clipboard/clipboard_impl.h"
namespace OHOS::Ace {
void ClipboardImpl::SetData(const std::string& data)
{
#if (defined(SYSTEM_CLIPBOARD_SUPPORTED))
    if (taskExecutor_) {
        auto pasteData = OHOS::MiscServices::PasteboardClient::GetInstance()->CreatePlainTextData(data);
        if (!pasteData) {
            LOGE("create SystemKeyboardData fail from MiscServices");
            return;
        }
        taskExecutor_->PostTask(
            [pasteData]() { OHOS::MiscServices::PasteboardClient::GetInstance()->SetPasteData(*pasteData); },
            TaskExecutor::TaskType::IO);
    }
#else
    LOGI("Current device doesn't support system clipboard");
#endif
}

void ClipboardImpl::GetData(const std::function<void(const std::string&)>& callback)
{
#if (defined(SYSTEM_CLIPBOARD_SUPPORTED))
    if (taskExecutor_) {
        auto has = OHOS::MiscServices::PasteboardClient::GetInstance()->HasPasteData();
        if (!has) {
            LOGE("SystemKeyboardData is not exist from MiscServices");
            return;
        }
        OHOS::MiscServices::PasteData pasteData;
        auto ok = OHOS::MiscServices::PasteboardClient::GetInstance()->GetPasteData(pasteData);
        if (!ok) {
            LOGE("Get SystemKeyboardData fail from MiscServices");
            return;
        }
        auto textData = pasteData.GetPrimaryText();
        if (!textData) {
            LOGE("Get SystemKeyboardTextData fail from MiscServices");
            return;
        }
        taskExecutor_->PostTask(
            [callback, taskExecutor = WeakClaim(RawPtr(taskExecutor_)), textData]() { callback(*textData); },
            TaskExecutor::TaskType::IO);
    }
#else
    LOGI("Current device doesn't support system clipboard");
#endif
}
void ClipboardImpl::Clear() {}
} // namespace OHOS::Ace