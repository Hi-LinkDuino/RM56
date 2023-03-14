/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMERR_HIRECORDER_ERROR_STATE_H
#define HISTREAMERR_HIRECORDER_ERROR_STATE_H

#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "pipeline/core/error_code.h"
#include "recorder_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
namespace Record {
class ErrorState : public State {
public:
    explicit ErrorState(StateId stateId, RecorderExecutor& executor) : State(stateId, "ErrorState", executor)
    {
    }

    ~ErrorState() override = default;
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif // HISTREAMERR_HIRECORDER_ERROR_STATE_H
