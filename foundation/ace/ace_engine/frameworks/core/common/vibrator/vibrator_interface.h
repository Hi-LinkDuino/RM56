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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_VIBRATOR_VIBRATOR_INTERFACE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_VIBRATOR_VIBRATOR_INTERFACE_H

#include "base/memory/referenced.h"
#include "base/thread/task_executor.h"
#include "core/common/vibrator/vibrator.h"

namespace OHOS::Ace {

class VibratorInterface {
public:
    virtual ~VibratorInterface() = default;

    virtual RefPtr<Vibrator> GetVibrator(const RefPtr<TaskExecutor>& taskExecutor) = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_VIBRATOR_VIBRATOR_INTERFACE_H
