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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_H

#include "base/memory/referenced.h"
#include "base/thread/task_executor.h"
#include "core/common/ime/text_input_client.h"
#include "core/common/ime/text_input_configuration.h"
#include "core/common/ime/text_input_connection.h"

namespace OHOS::Ace {

class TextInput {
public:
    virtual ~TextInput() = default;

    virtual RefPtr<TextInputConnection> Attach(const WeakPtr<TextInputClient>& client,
        const TextInputConfiguration& config, const RefPtr<TaskExecutor>& taskExecutor, int32_t instanceId) = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_H
