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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONNECTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONNECTION_H

#include <atomic>

#include "base/memory/ace_type.h"
#include "base/thread/task_executor.h"
#include "core/common/ime/constant.h"
#include "core/common/ime/text_editing_value.h"
#include "core/common/ime/text_input_client.h"

namespace OHOS::Ace {

class TextInputConnection : public AceType {
    DECLARE_ACE_TYPE(TextInputConnection, AceType);

public:
    TextInputConnection() = delete;
    TextInputConnection(const WeakPtr<TextInputClient>& client, const RefPtr<TaskExecutor>& taskExecutor)
        : clientId_(id_++), client_(client), taskExecutor_(taskExecutor)
    {}

    ~TextInputConnection() override = default;

    virtual void Show(bool isFocusViewChanged, int32_t instanceId) = 0;
    virtual void SetEditingState(
        const TextEditingValue& value, int32_t instanceId, bool needFireChangeEvent = true) = 0;
    virtual void Close(int32_t instanceId) = 0;

    int32_t GetClientId() const
    {
        return clientId_;
    }

    RefPtr<TextInputClient> GetClient() const
    {
        return client_.Upgrade();
    }

    RefPtr<TaskExecutor> GetTaskExecutor() const
    {
        return taskExecutor_;
    }

protected:
    // May be incremented on multi Ace UI threads.
    static std::atomic_int32_t id_;

    int32_t clientId_;
    WeakPtr<TextInputClient> client_;
    RefPtr<TaskExecutor> taskExecutor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONNECTION_H
