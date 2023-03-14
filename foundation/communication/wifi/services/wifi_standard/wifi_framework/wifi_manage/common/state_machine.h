/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_STA_MACHINE_H
#define OHOS_STA_MACHINE_H

#include <map>
#include <string>
#include <vector>
#include "handler.h"
#include "message_queue.h"
#include "state.h"

namespace OHOS {
namespace Wifi {
#define CMD_SET_OPERATIONAL_MODE 1
/* Message Processed */
static const bool EXECUTED = true;
/* The message is not processed. */
static const bool NOT_EXECUTED = false;

class StateMachineHandler;
class StateMachine {
public:
    /**
     * @Description : StateMachine Initialization Function
     *
     * @return true :success, false : failed.
     */
    bool InitialStateMachine();

    /**
     * @Description : Start StateMachine.
     *
     */
    void StartStateMachine();

    /**
     * @Description : Set Handler.
     *
     * @param handler - StateMachineHandler instance.[in]
     */
    void SetHandler(StateMachineHandler *handler);

    /**
     * @Description : The Message is not handled.
     *
     * @param msg - Message object.[in]
     */
    void NotExecutedMessage(const InternalMessage *msg);

    /**
     * @Description Stop Handler Thread.
     *
     */
    void StopHandlerThread();

    /**
     * @Description : Start the timer.
     *
     * @param timerName - Timer Name.[in]
     * @param interval - Timer duration, in milliseconds.[in]
     */
    virtual void StartTimer(int timerName, int64_t interval);

    /**
     * @Description : Stop the timer.
     *
     * @param timerName - Timer Name.[in]
     */
    virtual void StopTimer(int timerName);

    /**
     * @Description : Construct internal messages.
     *
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage();

    /**
     * @Description : Construct an information message based on
     * the original message.
     *
     * @param orig - Original message.[in]
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(const InternalMessage *orig);

    /**
     * @Description : Construct internal messages.
     *
     * @param msgName - Message Name.[in]
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(int msgName);

    /**
     * @Description : Construct internal messages.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(int msgName, int param1);

    /**
     * @Description : Construct internal messages.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @param param2 - Message parameters.[in]
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(int msgName, int param1, int param2);
    
    /**
     * @Description : Construct internal messages.
     *
     * @param msgName - Message Name.[in]
     * @param messageObj - User-defined data
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(int msgName, const std::any &messageObj);

    /**
     * @Description : Constructs internal messages.
     *
     * @param msgName - Message name.[in]
     * @param param1 - First Message parameter.[in]
     * @param param2 - Second Message parameter.[in]
     * @param messageObj - User-defined data
     * @return InternalMessage* : Pointer to the constructed internal message.
     */
    InternalMessage *CreateMessage(int msgName, int param1, int param2, const std::any &messageObj);

    /**
     * @Description : Constructs internal messages and places the
     * messages in the message queue of the state machine.
     *
     * @param msgName - Message name.[in]
     */
    virtual void SendMessage(int msgName);

    /**
     * @Description : Constructs internal messages and places the messages
     * in the message queue of the state machine.
     *
     * @param msgName - Message name.[in]
     * @param param1 - Message parameter.[in]
     */
    virtual void SendMessage(int msgName, int param1);

    /**
     * @Description : Constructs internal messages and places the messages
     * in the message queue of the state machine.
     *
     * @param msgName - Message name.[in]
     * @param param1 - Message parameter.[in]
     * @param param2 - Message parameter.[in]
     */
    virtual void SendMessage(int msgName, int param1, int param2);

    /**
     * @Description : Puts messages into the message queue of the state machine.
     *
     * @param msg - Message to be sent.[in]
     */
    virtual void SendMessage(InternalMessage *msg);

    /**
     * @Description : Puts messages into the message queue of the state machine.
     *
     * @param msgName - Message Name.[in]
     * @param messageObj -  User-defined data
     */
    virtual void SendMessage(int msgName, const std::any &messageObj);

    /**
     * @Description : Puts messages into the message queue of the state machine.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @param param2 - Message parameters.[in]
     * @param messageObj - User-defined data
     */
    virtual void SendMessage(int msgName, int param1, int param2, const std::any &messageObj);

    /**
     * @Description  Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msgName - Message Name.[in]
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(int msgName, int64_t delayTimeMs);

    /**
     * @Description : Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(int msgName, int param1, int64_t delayTimeMs);

    /**
     * @Description : Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @param param2 - Message parameters.[in]
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(int msgName, int param1, int param2, int64_t delayTimeMs);

    /**
     * @Description : Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msg - Message to be sent.[in]
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(InternalMessage *msg, int64_t delayTimeMs);

    /**
     * @Description : Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msgName - Message Name.[in]
     * @param messageObj -User-defined data
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(int msgName, const std::any &messageObj, int64_t delayTimeMs);

    /**
     * @Description : Constructs internal messages and places them in the
     * message queue of the state machine. The messages are processed
     * after the specified delay time.
     *
     * @param msgName - Message Name.[in]
     * @param param1 - Message parameters.[in]
     * @param param2 - Message parameters.[in]
     * @param messageObj - User-defined data
     * @param delayTimeMs - Delay time, in milliseconds.[in]
     */
    void MessageExecutedLater(int msgName, int param1, int param2, const std::any &messageObj, int64_t delayTimeMs);

protected:
    /**
     * @Description : Construct a new State Machine:: State Machine object.
     *
     * @param name - State name.[in]
     */
    explicit StateMachine(const std::string &name);

    /**
     * @Description : Destroy the State Machine:: State Machine object.
     *
     */
    virtual ~StateMachine();

    /**
     * @Description : Add state.
     *
     * @param state - state.[in]
     * @param upper - upper state.[in]
     */
    void StatePlus(State *state, State *upper);

    /**
     * @Description : Remove state.
     *
     * @param state - state.[in]
     */
    void StateDelete(State *state);

    /**
     * @Description : Set first state.
     *
     * @param firstState - First state.[in]
     */
    void SetFirstState(State *firstState);

    /**
     * @Description : Transition to orther state.
     *
     * @param targetState - state.[in]
     */
    void SwitchState(State *targetState);

    /**
     * @Description : Delay Message.
     *
     * @param msg - Message object.[in]
     */
    void DelayMessage(const InternalMessage *msg);

private:
    StateMachineHandler *pStateMachineHandler;
    std::string mStateName;
};

typedef struct StateInfo {
    State *state;
    StateInfo *upperStateInfo;
    bool active;
} StateInfo;

class StateMachineHandler : public Handler {
public:
    using StateInfoMap = std::map<std::string, StateInfo *>;
    using StateVector = std::vector<StateInfo *>;
    using DelayedMessage = std::vector<InternalMessage *>;

    /**
     * @Description : Construct a new state machine Handler:: StateMachine Handler object.
     *
     * @param pStateMgr - Handler pointer.[in]
     */
    explicit StateMachineHandler(StateMachine *pStateMgr);

    /**
     * @Description : Destroy the StateMachine Handler:: StateMachine Handler object.
     *
     */
    ~StateMachineHandler();

    /**
     * @Description : StateMachineHandler Initialization Function.
     *
     * @return true : success, false : failed.
     */
    bool InitialSmHandler();

    /**
     * @Description : Add a new state.
     *
     * @param state - State to be added.[in]
     * @param upper - upper of state.[in]
     * @return StateInfo*
     */
    StateInfo *StatePlus(State *state, State *upper);

    /**
     * @Description : Delete a state.
     *
     * @param state - State to be deleted.[in]
     */
    void StateDelete(State *state);

    /**
     * @Description : Sets the Initialization State.
     *
     * @param firstState - Initialization State.[in]
     */
    void SetFirstState(State *firstState);

    /**
     * @Description : State transition function.
     *
     * @param targetState - Destination State.[in]
     */
    void SwitchState(State *targetState);

    /**
     * @Description : Delay Message Processing Function.
     *
     * @param msg - Message body pointer.[in]
     */
    void DelayMessage(const InternalMessage *msg);

    /**
     * @Description : The state machine is constructed.
     *
     */
    void BuildTreeComplete();

private:
    /**
     * @Description : Sets the initial state sequence.
     *
     */
    void BuildStateInitVector();

    /**
     * @Description : Writes the inactive upper states of targetState
     * and targetState to the sequenceStateVector list.
     *
     * @param targetState - Target State Machine.[in]
     * @return StateInfo*
     */
    StateInfo *BuildSequenceStateVector(State *targetState);

    /**
     * @Description : Move Delayed Message At Front Of Queue.
     *
     */
    void PlaceDelayedMsgQueueTop();

    /**
     * @Description : Release all messages in delayed Messages.
     *
     */
    void ReleaseDelayedMessages();

    /**
     * @Description : Fill the status in the sequential status
     * list in reverse order.
     *
     * @return int
     */
    int MoveSequenceToStateVector();

    /**
     * @Description : Invoke the ExecuteStateMsg interface of the current state
     * to process messages sent to the state machine. The entry/exit of the
     * state machine is also called, and the delayed messagei s put back
     * into queue when transitioning to a new state.
     *
     * @param msg - Messages.[in]
     */
    void ExecuteMessage(InternalMessage *msg) override;

    /**
     * @Description : Clean up After Quitting.
     *
     */
    void ClearWhenQuit();

    /**
     * @Description : Performing Status Transitions.
     *
     * @param msgProcessedState - Message processing status.[in]
     * @param msg - Messages.[in]
     */
    void PerformSwitchState(State *msgProcessedState, InternalMessage *msg);

    /**
     * @Description : Process messages. If the current state doesnot process it,
     * the upper state processing is called, and so on. If all upper states
     * are not processed, invoke the NotExecutedMessage method of the state machine.
     *
     * @param msg - Message body pointer.[in]
     * @return State*
     */
    State *ExecuteTreeStateMsg(InternalMessage *msg);

    /**
     * @Description : Invoke GoOutState() for each state from the first
     * state in the list to the public upper state.
     *
     * @param commonStateInfo - common upper state machine.[in]
     */
    void CallTreeStateExits(StateInfo *commonStateInfo);

    /**
     * @Description : Call the GoInState method from the start state
     * index to the top of the state stack.
     *
     * @param index - Start state index of the
     *                                 state machine list.
     */
    void CallTreeStateEnters(int index);

private:
    /* All state mappings of the state machine */
    StateInfoMap mStateInfoMap;
    /* From child state to upper state list */
    StateVector mStateVector;
    /* Top index of mStateVector */
    int mStateVectorTopIndex;
    /* From upper state to child state list */
    StateVector mSequenceStateVector;
    /* Top of mSequenceStateVector */
    int mSequenceStateVectorCount;
    /* Delayed Message Queue */
    DelayedMessage mDelayedMessages;
    /* State machine instance */
    StateMachine *pStateMachine;
    /* Initial state */
    State *pFirstState;
    /* Target Status */
    State *pTargetState;
    /* StateMachine exit or not */
    bool mQuitFlag;
    /* Whether the state machine has been built */
    bool mBuildCompleteFlag;
    /*
     * All State exit/enter calls are true before the
     * last enter call in the target state.
     */
    bool mSwitchingStateFlag;
    /* Current Message */
    InternalMessage *pCurrentMsg;
};
}  // namespace Wifi
}  // namespace OHOS
#endif