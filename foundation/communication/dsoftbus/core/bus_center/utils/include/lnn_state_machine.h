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

#ifndef LNN_STATE_MACHINE_H
#define LNN_STATE_MACHINE_H

#include <stdint.h>

#include "common_list.h"
#include "message_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FSM_FLAG_RUNNING 0x1

struct tagFsmStateMachine;

typedef void (*StateEnterFunc)(struct tagFsmStateMachine *fsm);
typedef void (*StateExitFunc)(struct tagFsmStateMachine *fsm);
typedef bool (*StateProcessFunc)(struct tagFsmStateMachine *fsm, int32_t msgType, void *para);

typedef struct {
    ListNode list;
    StateEnterFunc enter;
    StateProcessFunc process;
    StateExitFunc exit;
} FsmState;

typedef void (*FsmDinitCallback)(struct tagFsmStateMachine *fsm);

typedef struct tagFsmStateMachine {
    FsmState *curState;
    uint32_t flag;

    ListNode stateList;
    SoftBusLooper *looper;
    SoftBusHandler handler;

    FsmDinitCallback deinitCallback;
} FsmStateMachine;

int32_t LnnFsmInit(FsmStateMachine *fsm, char *name, FsmDinitCallback cb);
int32_t LnnFsmDeinit(FsmStateMachine *fsm);

int32_t LnnFsmAddState(FsmStateMachine *fsm, FsmState *state);

int32_t LnnFsmStart(FsmStateMachine *fsm, FsmState *initialState);
int32_t LnnFsmStop(FsmStateMachine *fsm);

int32_t LnnFsmPostMessage(FsmStateMachine *fsm, uint32_t msgType, void *data);
int32_t LnnFsmPostMessageDelay(FsmStateMachine *fsm, uint32_t msgType,
    void *data, uint64_t delayMillis);
int32_t LnnFsmRemoveMessage(FsmStateMachine *fsm, int32_t msgType);

int32_t LnnFsmTransactState(FsmStateMachine *fsm, FsmState *state);

#ifdef __cplusplus
}
#endif

#endif
