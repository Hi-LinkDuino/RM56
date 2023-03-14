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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_PROXY_H

#include <memory>

#include "base/utils/singleton.h"
#include "core/common/ime/text_input.h"

namespace OHOS::Ace {

class TextInputProxy : public Singleton<TextInputProxy>, public TextInput {
    DECLARE_SINGLETON(TextInputProxy);

public:
    void SetDelegate(std::unique_ptr<TextInput>&& delegate);

    /**
     * @brief When component needs to interact with IME, it is the first step.
     *
     * Internally an auto-increment `clientId` will be generated to identify each connection.
     *
     * @param[in] client The event receiver to handle IME event. Make it weak.
     * @param[in] config Indicate how to show the keyboard.
     * @param[in] taskExecutor The task executor to be posted to run JNI task.
     *
     * @return A connection which can interact with IME.
     */
    RefPtr<TextInputConnection> Attach(const WeakPtr<TextInputClient>& client, const TextInputConfiguration& config,
        const RefPtr<TaskExecutor>& taskExecutor, int32_t instanceId) override;

private:
    std::unique_ptr<TextInput> delegate_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_PROXY_H
