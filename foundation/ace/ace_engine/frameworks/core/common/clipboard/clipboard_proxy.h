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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CLIPBOARD_CLIPBOARD_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CLIPBOARD_CLIPBOARD_PROXY_H

#include "base/utils/singleton.h"
#include "core/common/clipboard/clipboard_interface.h"

namespace OHOS::Ace {

class ACE_EXPORT ClipboardProxy : public ClipboardInterface {
public:
    static ClipboardProxy* GetInstance();
    ClipboardProxy() = default;
    ~ClipboardProxy() = default;
    void SetDelegate(std::unique_ptr<ClipboardInterface>&& delegate);
    RefPtr<Clipboard> GetClipboard(const RefPtr<TaskExecutor>& taskExecutor) const override;

private:
    static ClipboardProxy* inst_;
    static std::mutex mutex_;
    std::unique_ptr<ClipboardInterface> delegate_;

    ACE_DISALLOW_COPY_AND_MOVE(ClipboardProxy);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_CLIPBOARD_CLIPBOARD_PROXY_H
