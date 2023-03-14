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

#include "liteplayer_state_machine.h"
#include "liteplayer.h"
#include "liteplayer_comm.h"
#include "hi_liteplayer_err.h"

namespace OHOS {
namespace Media {

namespace {
    /*****************************************************************************
    status table of operation, false is not support
    *******************************************************************/
    const bool LITEPLAY_STATE[PLAYERCONTROL_MSG_BUTT][PLAY_STATUS_BUTT] = {
        /* idle,    init,     prepare   play,     tplay,    pause,    err */
        { false, false, true,  false, false, false, false }, /* setMedia */
        { true,  true,  true,  true,  true,  true,  true },  /* regCallbk */
        { true,  true,  false, false, false, false, false }, /* setsrc_fd */
        { true,  true,  false, false, false, false, false }, /* setsrc_uri */
        { true,  true,  false, false, false, false, false }, /* setsrc_stream */
        { false, true,  true,  false, false, false, false }, /* prepare */
        { false, false, true,  true,  true,  true,  false }, /* play */
        { false, false, true,  true,  true,  true,  false }, /* seek */
        { false, false, false, true,  false, true,  false }, /* pause */
        { false, false, false, true,  true,  false, false }, /* tplay */
        { true,  true,  true,  true,  true,  true,  true },  /* stop */
        { false, false, true,  true,  true,  true,  false }, /* getInfo */
        { true,  false, false, true,  true,  true,  false }, /* handledata */
        { true,  true,  true,  true,  true,  true,  true },  /* Error */
        { false, true,  true,  true,  true,  true,  false },  /* setVolume */
        { true,  true,  true,  true,  true,  true,  true },  /* invoke */
        { true,  true,  true,  false, false, false, false },  /* set audio stream type */
    };

    struct PlayerControlStatusMap {
        PlayerStatus state;
        const std::string stateName;
    };

    const PlayerControlStatusMap LITEPLAY_STATEMAP[PLAY_STATUS_BUTT] = {
        { PLAY_STATUS_IDLE,     "idle" },
        { PLAY_STATUS_INIT,     "init" },
        { PLAY_STATUS_PREPARED, "prepare" },
        { PLAY_STATUS_PLAY,     "play" },
        { PLAY_STATUS_TPLAY,    "tplay" },
        { PLAY_STATUS_PAUSE,    "pause" },
        { PLAY_STATUS_ERR,      "error" },
    };
}

#define CHECK_NULL_RETURN(value, ret, printfString) \
do { \
    if (value == nullptr) { \
        MEDIA_ERR_LOG( "playerStateMachine %s \n", printfString ? printfString : " "); \
        return ret; \
    } \
} while (0)

PlayerStatus StateConvert2Enum(const std::string stateName)
{
    bool isFound = false;
    uint32_t i = 0;

    for (; i < PLAY_STATUS_BUTT; i++) {
        if (stateName == LITEPLAY_STATEMAP[i].stateName) {
            isFound = true;
            break;
        }
    }
    if (isFound == false) {
        MEDIA_ERR_LOG("could not find respond state :%s", stateName.c_str());
        return PLAY_STATUS_BUTT;
    }
    return LITEPLAY_STATEMAP[i].state;
}

static std::string StateEnum2Name(PlayerStatus state)
{
    return LITEPLAY_STATEMAP[state].stateName;
}

PlayerControlState::~PlayerControlState()
{
}

int PlayerControlState::Enter()
{
    PlayerStatus playerStatus = StateConvert2Enum(Name());
    CHECK_NULL_RETURN(playerControlHandle_, HI_ERR_PLAYERCONTROL_NULL_PTR, "handle null");
    playerControlHandle_->StateChangeCallback(playerStatus);
    MEDIA_DEBUG_LOG( "enter state: %s\n", Name().c_str());
    return HI_SUCCESS;
}

int PlayerControlState::Exit()
{
    MEDIA_DEBUG_LOG( "exit state: %s", Name().c_str());
    return HI_SUCCESS;
}

HiState *PlayerControlState::FindTransition(int event)
{
    HiState *dstState = HiState::FindTransition(event);
    return dstState;
}

int PlayerControlState::HandleMessage(const MsgInfo& msgInfo)
{
    int32_t ret = HI_SUCCESS;

    CHECK_NULL_RETURN(curState_, HI_ERR_PLAYERCONTROL_NULL_PTR, "curState null");
    if (!curState_->EventValidAtCurState((PlayerControlMsgType)msgInfo.what)) {
        MEDIA_ERR_LOG( "invalid event :%d at current state: %s", msgInfo.what,
                      Name().c_str());
        return HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION;
    }
    CHECK_NULL_RETURN(playerControlHandle_, HI_ERR_PLAYERCONTROL_NULL_PTR, "handle null");
    switch (msgInfo.what) {
        case PLAYERCONTROL_MSG_REGCALLBACK:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoRegCallback(*reinterpret_cast<PlayerCtrlCallbackParam *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_URI:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetDataSource(reinterpret_cast<const char *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_FD:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetDataSource(*reinterpret_cast<const int *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetDataSource(*reinterpret_cast<BufferStream *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SETATTR:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetMedia(*reinterpret_cast<PlayerControlStreamAttr *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_PAUSE:
            ret = playerControlHandle_->DoPause();
            break;
        case PLAYERCONTROL_MSG_TPLAY:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoTPlay(*reinterpret_cast<TplayAttr *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_PREPARE:
            ret = playerControlHandle_->DoPrepare();
            break;
        case PLAYERCONTROL_MSG_PLAY:
            ret = playerControlHandle_->DoPlay();
            break;
        case PLAYERCONTROL_MSG_SET_VOLUME:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetVolume(*reinterpret_cast<VolumeAttr *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_HANDLEDATA:
            ret = HI_SUCCESS;
            break;
        case PLAYERCONTROL_MSG_STOP:
            ret = playerControlHandle_->DoStop();
            break;
        case PLAYERCONTROL_MSG_SEEK:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSeek(*reinterpret_cast<int64_t *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_GETFILEINFO:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoGetFileInfo(*reinterpret_cast<FormatFileInfo *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_INVOKE:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoInvoke(*reinterpret_cast<InvokeParameter *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_AUDIOSTREAM_TYPE:
            CHECK_NULL_RETURN(msgInfo.msgData, HI_ERR_PLAYERCONTROL_NULL_PTR, "msgData null");
            ret = playerControlHandle_->DoSetAudioStreamType(*reinterpret_cast<int32_t *>(msgInfo.msgData));
            break;
        default:
            ret = HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
            break;
    }

    return ret;
}

void PlayerControlSMObserver::OnEventHandled(const HiStateMachine &curState, int event, int result)
{
    (void)curState;
    if ((result == HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION) && (event == PLAYERCONTROL_MSG_SEEK) &&
        (playerControlHandle_ != nullptr)) {
        playerControlHandle_->NotifyError(PLAYERCONTROL_ERROR_ILLEGAL_STATEACTION);
    }
}

PlayerControlSMObserver::~PlayerControlSMObserver()
{
}

PlayerControlStateMachine::PlayerControlStateMachine(PlayerControlHandle& handle)
    : HiStateMachine(), playerControlHandle_(&handle), stateIdle_(nullptr), stateInit_(nullptr),
      statePrepare_(nullptr), statePlay_(nullptr), stateTPlay_(nullptr),
      statePause_(nullptr), stateError_(nullptr)
{
}

PlayerControlStateMachine::~PlayerControlStateMachine()
{
    (void)Deinit();
}

int32_t PlayerControlStateMachine::CreateStates()
{
    CHECK_NULL_RETURN(playerControlHandle_, HI_ERR_PLAYERCONTROL_NULL_PTR, "handle null");
    stateIdle_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_IDLE));
    if (stateIdle_ == nullptr) {
        MEDIA_ERR_LOG( "new stateIdle_ failed");
        return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
    }
    stateInit_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_INIT));
    if (stateInit_ == nullptr) {
        MEDIA_ERR_LOG( "new stateInit_ failed");
        goto DESTROY;
    }
    statePrepare_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_PREPARED));
    if (statePrepare_ == nullptr) {
        MEDIA_ERR_LOG( "new statePrepare_ failed");
        goto DESTROY;
    }
    statePlay_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_PLAY));
    if (statePlay_ == nullptr) {
        MEDIA_ERR_LOG( "new statePlay_ failed");
        goto DESTROY;
    }
    stateTPlay_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_TPLAY));
    if (stateTPlay_ == nullptr) {
        MEDIA_ERR_LOG( "new stateTPlay_ failed");
        goto DESTROY;
    }
    statePause_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_PAUSE));
    if (statePause_ == nullptr) {
        MEDIA_ERR_LOG( "new statePause_ failed");
        goto DESTROY;
    }
    stateError_ = new(std::nothrow) PlayerControlState(*playerControlHandle_, *this,
        StateEnum2Name(PLAY_STATUS_ERR));
    if (stateError_ == nullptr) {
        MEDIA_ERR_LOG( "new stateError_ failed");
        goto DESTROY;
    }
    return HI_SUCCESS;
DESTROY:
    DestroyStates();
    return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
}

void PlayerControlStateMachine::DestroyStates()
{
    if (stateIdle_ != nullptr) {
        delete stateIdle_;
        stateIdle_ = nullptr;
    }
    if (stateInit_ != nullptr) {
        delete stateInit_;
        stateInit_ = nullptr;
    }
    if (statePrepare_ != nullptr) {
        delete statePrepare_;
        statePrepare_ = nullptr;
    }
    if (statePlay_ != nullptr) {
        delete statePlay_;
        statePlay_ = nullptr;
    }
    if (stateTPlay_ != nullptr) {
        delete stateTPlay_;
        stateTPlay_ = nullptr;
    }
    if (statePause_ != nullptr) {
        delete statePause_;
        statePause_ = nullptr;
    }
    if (stateError_ != nullptr) {
        delete stateError_;
        stateError_ = nullptr;
    }
}

int32_t PlayerControlStateMachine::Init(uint32_t maxQueueSize, uint32_t maxMsgPayloadSize)
{
    const std::string playerControlSmName = "playerCtrl_sm";
    int32_t ret = HiStateMachine::Init(maxQueueSize, maxMsgPayloadSize, playerControlSmName);
    if (ret != HI_SUCCESS) {
        printf("HiStateMachine::Init failed");
        return ret;
    }
    ret = CreateStates();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG( "createStates failed");
        return ret;
    }
    stateIdle_->AddTransition(PLAYERCONTROL_MSG_SET_DATASOURCE_FD, *stateInit_);
    stateIdle_->AddTransition(PLAYERCONTROL_MSG_SET_DATASOURCE_URI, *stateInit_);
    stateIdle_->AddTransition(PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM, *stateInit_);
    stateIdle_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    stateInit_->AddTransition(PLAYERCONTROL_MSG_PREPARE, *statePrepare_);
    stateInit_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    stateInit_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    statePrepare_->AddTransition(PLAYERCONTROL_MSG_PLAY, *statePlay_);
    statePrepare_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    statePrepare_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    statePlay_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    statePlay_->AddTransition(PLAYERCONTROL_MSG_TPLAY, *stateTPlay_);
    statePlay_->AddTransition(PLAYERCONTROL_MSG_PAUSE, *statePause_);
    statePlay_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    stateTPlay_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    stateTPlay_->AddTransition(PLAYERCONTROL_MSG_PLAY, *statePlay_);
    stateTPlay_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    statePause_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    statePause_->AddTransition(PLAYERCONTROL_MSG_PLAY, *statePlay_);
    statePause_->AddTransition(PLAYERCONTROL_MSG_ERROR, *stateError_);
    stateError_->AddTransition(PLAYERCONTROL_MSG_STOP, *stateIdle_);
    (void)AddState(*stateIdle_);
    (void)AddState(*stateInit_);
    (void)AddState(*statePrepare_);
    (void)AddState(*statePlay_);
    (void)AddState(*stateTPlay_);
    (void)AddState(*statePause_);
    (void)AddState(*stateError_);
    (void)SetInitialState(*stateIdle_);
    return HI_SUCCESS;
}

int32_t PlayerControlStateMachine::Deinit()
{
    DestroyStates();
    int32_t ret = HiStateMachine::Deinit();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG( "HiStateMachine::Deinit failed");
        return ret;
    }
    return HI_SUCCESS;
}

PlayerStatus PlayerControlStateMachine::GetCurState()
{
    const HiState* curState = CurrentState();
    return StateConvert2Enum(curState->Name());
}

bool PlayerControlStateMachine::EventValidAtCurState(PlayerControlMsgType type)
{
    if (type < PLAYERCONTROL_MSG_SETATTR || type >= PLAYERCONTROL_MSG_BUTT) {
        MEDIA_ERR_LOG( "EventValidAtCurState MSG type not support %d", type);
        return false;
    }
    return LITEPLAY_STATE[type][GetCurState()];
}
}
}
