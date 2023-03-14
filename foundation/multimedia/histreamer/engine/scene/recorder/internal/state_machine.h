/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_HIRECORDER_STATE_MACHINE_H
#define HISTREAMER_HIRECORDER_STATE_MACHINE_H

#include <map>
#include <memory>
#include <queue>
#include <tuple>
#include "error_state.h"
#include "foundation/log.h"
#include "foundation/osal/base/synchronizer.h"
#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/task.h"
#include "init_state.h"
#include "pause_state.h"
#include "pipeline/core/error_code.h"
#include "plugin/common/any.h"
#include "ready_state.h"
#include "recording_setting_state.h"
#include "recording_state.h"
#include "recorder_executor.h"
#include "utils/blocking_queue.h"

namespace OHOS {
namespace Media {
namespace Record {
class StateChangeCallback {
public:
    virtual ~StateChangeCallback() = default;
    virtual void OnStateChanged(StateId state) = 0;
};

class StateMachine final : public OSAL::Task {
public:
    explicit StateMachine(RecorderExecutor& executor);

    ~StateMachine() override = default;

    void Stop() override;

    void SetStateCallback(StateChangeCallback* callback);

    const std::string& GetCurrentState() const;

    StateId GetCurrentStateId() const;

    ErrorCode SendEvent(Intent intent, const Plugin::Any& param = {});

    ErrorCode SendEvent(Intent intent, const Plugin::Any& param = {}) const;

    ErrorCode SendEventAsync(Intent intent, const Plugin::Any& param = {});

    ErrorCode SendEventAsync(Intent intent, const Plugin::Any& param = {}) const;

private:
    using Job = std::function<Action()>;

    Action ProcessIntent(Intent intent, const Plugin::Any& param);

    void DoTask() override;

    void AddState(const std::shared_ptr<State>& state);

    ErrorCode ProcAction(Action nextAction);

    ErrorCode TransitionTo(const std::shared_ptr<State>& state);

    void OnIntentExecuted(Intent intent, Action action, ErrorCode result);

    mutable OSAL::Mutex mutex_ {};
    mutable OSAL::Synchronizer<Intent, ErrorCode> intentSync_;
    mutable std::shared_ptr<State> curState_ {nullptr};
    mutable Intent lastIntent {Intent::INTENT_BUTT};
    std::map<StateId, std::shared_ptr<State>> states_ {};
    Media::BlockingQueue<Job> jobs_;
    std::queue<Job> pendingJobs_ {};
    StateChangeCallback* callback_ {nullptr};
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif
