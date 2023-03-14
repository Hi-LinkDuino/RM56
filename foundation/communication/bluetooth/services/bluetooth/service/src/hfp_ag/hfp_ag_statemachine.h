/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HFP_AG_STATEMACHINE_H
#define HFP_AG_STATEMACHINE_H

#include <list>
#include <memory>
#include <string>

#include "hfp_ag_message.h"
#include "hfp_ag_profile.h"
#include "hfp_ag_system_event_processer.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for AG state machine.
 */
class HfpAgStateMachine : public utility::StateMachine {
public:
    /**
     * @brief Construct a new HfpAgStateMachine object.
     *
     * @param address Device address.
     */
    explicit HfpAgStateMachine(const std::string &address);

    /**
     * @brief Destroy the HfpAgStateMachine object.
     */
    ~HfpAgStateMachine() = default;

    /**
     * @brief Initialise the state machine.
     */
    void Init();

    /**
     * @brief Get the State Int object.
     *
     * @return Returns the state number.
     */
    int GetStateInt() const;

    /**
     * @brief Get the device address.
     *
     * @return Returns the device address.
     */
    std::string GetDeviceAddr() const;

    /**
     * @brief Add the deferred message.
     *
     * @param msg The message.
     */
    void AddDeferredMessage(const HfpAgMessage &msg);

    /**
     * @brief Process the deferred message.
     */
    void ProcessDeferredMessage();

    /**
     * @brief Notify the state change.
     */
    void NotifyStateTransitions();

    /**
     * @brief the state change from panrent state to child state.
     *
     * @param fromState The previous state.
     * @param toState The current state.
     */
    void NotifyChildStateToParentState(int fromState, int toState);

    /**
     * @brief Process audio disconnected event.
     */
    void ProcessAudioDisconnected();

    /**
     * @brief Start the connection timer.
     */
    void StartConnectionTimer() const;

    /**
     * @brief Stop the connection timer.
     */
    void StopConnectionTimer() const;

    /**
     * @brief Connection timeout handler.
     */
    void ConnectionTimeout() const;

    /**
     * @brief Get the service event name.
     *
     * @param what Service event number.
     * @return Returns the service event string name.
     */
    static std::string GetEventName(int what);

    /**
     * @brief Check if current statemachine is removing.
     *
     * @return Returns <b>true</b> if the statemachine is removing; returns <b>false</b> if not.
     */
    bool IsRemoving() const;

    /**
     * @brief Mark statemachine removing.
     *
     * @param isRemoving removing mark.
     */
    void SetRemoving(bool isRemoving);

    void ProcessKeyPressed(const RawAddress &device, const int &phoneState) const;

    inline static const std::string DISCONNECTED = "Disconnected";
    inline static const std::string CONNECTING = "Connecting";
    inline static const std::string DISCONNECTING = "Disconnecting";
    inline static const std::string CONNECTED = "Connected";
    inline static const std::string AUDIO_CONNECTING = "AudioConnecting";
    inline static const std::string AUDIO_DISCONNECTING = "AudioDisconnecting";
    inline static const std::string AUDIO_CONNECTED = "AudioConnected";

private:
    std::string address_;
    HfpAgProfile profile_;
    HfpAgSystemEventProcesser eventProcessor_;
    std::list<HfpAgMessage> deferMsgs_ {};
    int preState_ {HFP_AG_STATE_DISCONNECTED};
    std::unique_ptr<utility::Timer> connTimer_ {nullptr};
    inline static const int CONNECTION_TIMEOUT_MS {60000};  // 60s
    bool isRemoving_ {false};

    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgStateMachine);
};

class HfpAgState : public utility::StateMachine::State {
public:
    struct HfpAgSmInfo {
        HfpAgSmInfo(const std::string &name, utility::StateMachine& statemachine)
            : name_(name), sm_(statemachine)
        {}
        const std::string& name_;
        utility::StateMachine& sm_;
    };

    HfpAgState(HfpAgSmInfo stu, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch, int stateInt,
        utility::StateMachine::State &parent)
        : State(stu.name_, stu.sm_, parent),
          profile_(profile),
          eventProcessor_(eventDispatch),
          stateInt_(stateInt),
          stateMachine_(static_cast<HfpAgStateMachine &>(stu.sm_))
    {}

    HfpAgState(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch, int stateInt)
        : State(name, statemachine),
          profile_(profile),
          eventProcessor_(eventDispatch),
          stateInt_(stateInt),
          stateMachine_((HfpAgStateMachine &)statemachine)
    {}
    virtual ~HfpAgState()
    {}
    int GetStateInt() const
    {
        return stateInt_;
    }

protected:
    HfpAgProfile& profile_;
    int callState_ = 0;
    HfpAgSystemEventProcesser &eventProcessor_;
    int stateInt_ {HFP_AG_STATE_DISCONNECTED};
    HfpAgStateMachine &stateMachine_;
};

class HfpAgDisconnected : public HfpAgState {
public:
    HfpAgDisconnected(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch)
        : HfpAgState(name, statemachine, profile, eventDispatch, HFP_AG_STATE_DISCONNECTED)
    {}
    ~HfpAgDisconnected() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
    bool isReentry_ {false};
};

class HfpAgConnecting : public HfpAgState {
public:
    HfpAgConnecting(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch)
        : HfpAgState(name, statemachine, profile, eventDispatch, HFP_AG_STATE_CONNECTING)
    {}
    ~HfpAgConnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
private:
    int hspState_ = 1;
};

class HfpAgDisconnecting : public HfpAgState {
public:
    HfpAgDisconnecting(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch)
        : HfpAgState(name, statemachine, profile, eventDispatch, HFP_AG_STATE_DISCONNECTING)
    {}
    ~HfpAgDisconnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpAgConnected : public HfpAgState {
public:
    HfpAgConnected(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch)
        : HfpAgState(name, statemachine, profile, eventDispatch, HFP_AG_STATE_CONNECTED)
    {}
    ~HfpAgConnected() override = default;
    void Entry() override;
    void Exit() override
    {}
    bool Dispatch(const utility::Message &msg) override;

private:
    void ProcessVoiceRecognitionResult(int result);
    void ProcessPhoneStateChange(const HfpAgMessage &event) const;
    void ProcessResponseClcc(const HfpAgMessage &event) const;
    void ProcessResponesOK() const;
};

class HfpAgAudioConnecting : public HfpAgState {
public:
    HfpAgAudioConnecting(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch, utility::StateMachine::State &parent)
        : HfpAgState(HfpAgSmInfo(name, statemachine), profile, eventDispatch, HFP_AG_AUDIO_STATE_CONNECTING, parent)
    {}
    ~HfpAgAudioConnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpAgAudioDisconnecting : public HfpAgState {
public:
    HfpAgAudioDisconnecting(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch, utility::StateMachine::State &parent)
        : HfpAgState(HfpAgSmInfo(name, statemachine), profile, eventDispatch, HFP_AG_AUDIO_STATE_DISCONNECTING, parent)
    {}
    ~HfpAgAudioDisconnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpAgAudioConnected : public HfpAgState {
public:
    HfpAgAudioConnected(const std::string &name, utility::StateMachine &statemachine, HfpAgProfile &profile,
        HfpAgSystemEventProcesser &eventDispatch, utility::StateMachine::State &parent)
        : HfpAgState(HfpAgSmInfo(name, statemachine), profile, eventDispatch, HFP_AG_AUDIO_STATE_CONNECTED, parent)
    {}
    ~HfpAgAudioConnected() override = default;
    void Entry() override;
    void Exit() override
    {}
    bool Dispatch(const utility::Message &msg) override;

private:
    void ProcessDisconnect(const HfpAgMessage &event);
    void ProcessSetVolume(const HfpAgMessage &event);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_STATEMACHINE_H
