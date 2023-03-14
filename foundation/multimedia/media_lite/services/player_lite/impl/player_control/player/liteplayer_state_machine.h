/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef PLAYERCONTROL_STATE_MACHINE_H
#define PLAYERCONTROL_STATE_MACHINE_H

#include <string>
#include "hi_state_machine.h"
#include "liteplayer_msgtype.h"
#include "liteplayer_msgtype.h"
#include "liteplayer_define.h"
#include "hi_liteplayer.h"
#include "format_type.h"
#include "hi_demuxer.h"
#include "hi_state.h"

using OHOS::HiState;
using OHOS::HiStateMachineObserver;
using OHOS::HiStateMachine;

namespace OHOS {
namespace Media {
class PlayerControlCurState {
public:
    PlayerControlCurState()
    {}
    virtual ~PlayerControlCurState()
    {}
    virtual PlayerStatus GetCurState(void) = 0;
    virtual bool EventValidAtCurState(PlayerControlMsgType type) = 0;
};

class PlayerControlHandle {
public:
    PlayerControlHandle() {};
    virtual ~PlayerControlHandle() {};

    virtual void StateChangeCallback(PlayerStatus state) = 0;
    virtual int32_t DoRegCallback(PlayerCtrlCallbackParam &observer) = 0;
    virtual int32_t DoSetDataSource(const std::string filepath) = 0;
    virtual int32_t DoSetDataSource(const int fd) = 0;
    virtual int32_t DoSetDataSource(BufferStream &stream) = 0;
    virtual int32_t DoPrepare(void) = 0;
    virtual int32_t DoPlay(void) = 0;
    virtual int32_t DoSetVolume(VolumeAttr &volumeAttr) = 0;
    virtual int32_t ReadPacketAndPushToDecoder(void) = 0;
    virtual int32_t DoStop(void) = 0;
    virtual int32_t DoSeek(int64_t timeInMs) = 0;
    virtual int32_t DoPause(void) = 0;
    virtual int32_t DoGetFileInfo(FormatFileInfo &fileInfo) = 0;
    virtual int32_t DoSetMedia(PlayerControlStreamAttr &mediaAttr) = 0;
    virtual int32_t DoTPlay(TplayAttr& trickPlayAttr) = 0;
    virtual int32_t DoInvoke(InvokeParameter& invokeParam) = 0;
    virtual int32_t DoSetAudioStreamType(int32_t type) = 0;
    virtual void NotifyError(PlayerControlError playerError) = 0;
};

class PlayerControlState : public HiState {
public:
    PlayerControlState(PlayerControlHandle& handle, PlayerControlCurState& curState, std::string name)
        : HiState(name), playerControlHandle_(&handle), curState_(&curState) {};
    virtual ~PlayerControlState(void);
    virtual int HandleMessage(const MsgInfo& msg);
    virtual HiState *FindTransition(int event);
    virtual int Enter(void);
    virtual int Exit(void);

protected:
    PlayerControlHandle *playerControlHandle_;
    PlayerControlCurState *curState_;
};

class PlayerControlSMObserver : public HiStateMachineObserver {
public:
    explicit PlayerControlSMObserver(PlayerControlHandle& handle)
        : HiStateMachineObserver(), playerControlHandle_(&handle) {};
    virtual ~PlayerControlSMObserver();

    void OnEventHandled(const HiStateMachine& stateMachine, int event, int result);

private:
    PlayerControlHandle *playerControlHandle_;
};

class PlayerControlStateMachine : public PlayerControlCurState, public HiStateMachine {
public:
    friend class PlayerControlState;

    explicit PlayerControlStateMachine(PlayerControlHandle& handle);
    ~PlayerControlStateMachine() override;
    int32_t Init(uint32_t maxQueueSize, uint32_t maxMsgPayloadSize);
    int32_t Deinit(void);
    PlayerStatus GetCurState(void) override;
    bool EventValidAtCurState(PlayerControlMsgType type) override;

private:
    int32_t CreateStates(void);
    void DestroyStates(void);

private:
    PlayerControlHandle *playerControlHandle_;
    PlayerControlState *stateIdle_;
    PlayerControlState *stateInit_;
    PlayerControlState *statePrepare_;
    PlayerControlState *statePlay_;
    PlayerControlState *stateTPlay_;
    PlayerControlState *statePause_;
    PlayerControlState *stateError_;
};
}
}
#endif  // PLAYERCONTROL_STATEMACHINE_H
