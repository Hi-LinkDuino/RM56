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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CAPABILITY_CLIPBOARD_CLIPBOARD_IMPL_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CAPABILITY_CLIPBOARD_CLIPBOARD_IMPL_H

#include "core/common/clipboard/clipboard_interface.h"

namespace OHOS::Ace {

class ClipboardImpl : public Clipboard {
public:
    explicit ClipboardImpl(const RefPtr<TaskExecutor>& taskExecutor) : Clipboard(taskExecutor) {}
    ~ClipboardImpl() override = default;

    // Implement Clipboard
    void SetData(const std::string& data) override;
    void GetData(const std::function<void(const std::string&)>& callback) override;
    void Clear() override;
};

class ClipboardProxyImpl final : public ClipboardInterface {
public:
    ClipboardProxyImpl() = default;
    ~ClipboardProxyImpl() = default;

    // Implement ClipboardInterface
    RefPtr<Clipboard> GetClipboard(const RefPtr<TaskExecutor>& taskExecutor) const override
    {
        return AceType::MakeRefPtr<ClipboardImpl>(taskExecutor);
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CAPABILITY_CLIPBOARD_CLIPBOARD_IMPL_H