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

#ifndef HISTREAMER_HIPLAYER_STATE_H
#define HISTREAMER_HIPLAYER_STATE_H

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include "common/any.h"
#include "foundation/log.h"
#include "pipeline/core/error_code.h"
#include "play_executor.h"

namespace OHOS {
namespace Media {
enum class StateId {
    IDLE = 0,
    INIT = 1,
    PREPARING = 2,
    READY = 3,
    PAUSE = 4,
    PLAYING = 5,
    STOPPED = 6,
    EOS = 7,
    BUTT,
};

enum class Intent {
    SET_SOURCE,
    SEEK,
    PREPARE,
    PLAY,
    PAUSE,
    RESUME,
    STOP,
    RESET,
    SET_ATTRIBUTE,
    NOTIFY_READY,
    NOTIFY_COMPLETE,
    NOTIFY_ERROR,
    INTENT_BUTT
};

enum class Action {
    TRANS_TO_IDLE,
    TRANS_TO_INIT,
    TRANS_TO_PREPARING,
    TRANS_TO_READY,
    TRANS_TO_PLAYING,
    TRANS_TO_PAUSE,
    TRANS_TO_STOPPED,
    TRANS_TO_EOS,
    ACTION_PENDING,
    ACTION_BUTT
};

class State {
public:
    State(StateId stateId, std::string name, PlayExecutor& executor);
    virtual ~State() = default;
    virtual std::tuple<ErrorCode, Action> Enter(Intent intent);
    virtual void Exit();
    std::tuple<ErrorCode, Action> Execute(Intent intent, const Plugin::Any& param);
    const std::string& GetName();
    StateId GetStateId();
    virtual std::tuple<ErrorCode, Action> SetSource(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> Prepare();
    virtual std::tuple<ErrorCode, Action> Play();
    virtual std::tuple<ErrorCode, Action> Stop();
    virtual std::tuple<ErrorCode, Action> Reset();
    virtual std::tuple<ErrorCode, Action> Pause();
    virtual std::tuple<ErrorCode, Action> Resume();
    virtual std::tuple<ErrorCode, Action> Seek(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> SetAttribute();
    virtual std::tuple<ErrorCode, Action> OnReady();
    virtual std::tuple<ErrorCode, Action> OnError(const Plugin::Any& param) final;
    virtual std::tuple<ErrorCode, Action> OnComplete();

    static const char* GetStateName(StateId state);
    static const char* GetIntentName(Intent intent);
    static const char* GetActionName(Action action);

protected:
    std::tuple<ErrorCode, Action> DispatchIntent(Intent intent, const Plugin::Any& param);

    const StateId stateId_;
    const std::string name_;
    PlayExecutor& executor_;
};
} // namespace Media
} // namespace OHOS
#endif
