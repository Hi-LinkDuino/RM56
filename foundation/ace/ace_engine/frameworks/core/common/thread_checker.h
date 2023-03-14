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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_THREAD_CHECKER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_THREAD_CHECKER_H

#include "base/thread/task_executor.h"
#include "base/utils/macros.h"

#ifdef ACE_DEBUG
#define CHECK_RUN_ON(threadType) ACE_DCHECK(OHOS::Ace::CheckThread(TaskExecutor::TaskType::threadType))
#else
#define CHECK_RUN_ON(threadType)
#endif

namespace OHOS::Ace {

bool ACE_EXPORT CheckThread(TaskExecutor::TaskType threadType);

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_THREAD_CHECKER_H
