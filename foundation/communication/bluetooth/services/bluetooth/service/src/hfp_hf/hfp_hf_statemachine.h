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

#ifndef HFP_HF_STATEMACHINE_H
#define HFP_HF_STATEMACHINE_H

#include <list>
#include <memory>
#include <string>

#include "hfp_hf_call_manager.h"
#include "hfp_hf_defines.h"
#include "hfp_hf_message.h"
#include "hfp_hf_profile.h"
#include "state_machine.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for HF state machine.
 */
class HfpHfStateMachine : public utility::StateMachine {
public:
    /**
     * @brief Construct a new HfpHfStateMachine object.
     *
     * @param address Device address.
     */
    explicit HfpHfStateMachine(const std::string &address);

    /**
     * @brief Destroy the HfpHfStateMachine object.
     */
    ~HfpHfStateMachine() = default;

    /**
     * @brief Initialise the state machine.
     */
    void Init();

    /**
     * @brief Get the IsInbandRing flag.
     *
     * @return Returns isInBandRing flag.
     */
    bool GetIsInbandRing() const;

    /**
     * @brief Get the volume stored in state machine.
     *
     * @return Returns the volume stored in state machine.
     */
    int GetVolume() const;

    /**
     * @brief Get the State Int object.
     *
     * @return Returns the state number.
     */
    int GetDeviceStateInt() const;

    /**
     * @brief Get the current call list.
     *
     * @return Returns the current call list.
     */
    std::vector<HandsFreeUnitCalls> GetCurrentCallList();

    /**
     * @brief Process interactive event from profile.
     *
     * @param event The event from profile.
     */
    void ProcessInteractiveEvent(const HfpHfMessage &event);

    /**
     * @brief Process accept call event from service.
     *
     * @param flag Accept call flag.
     */
    void ProcessAcceptCall(int flag);

    /**
     * @brief Process hold call event from service.
     */
    void ProcessHoldCall();

    /**
     * @brief Process reject call event from service.
     */
    void ProcessRejectCall();

    /**
     * @brief Process send key pressed event.
     *
     */
    void PrecessSendKeyPressed();

    /**
     * @brief Process handle incoming call event from service.
     *
     * @param flag Handle call flag.
     */
    void ProcessHandleIncomingCall(int flag);

    /**
     * @brief Process handle incoming call event from service.
     *
     * @param flag Handle call flag.
     * @param index call index
     */
    void ProcessHandleMultiCall(int flag, int index);

    /**
     * @brief Process dial last number event from service.
     *
     */
    void ProcessDialLastNumber();

    /**
     * @brief Process dial memory number event from service.
     *
     * @param index Memory number index
     */
    void ProcessDialMemory(int index);

    /**
     * @brief Process send voice tag number event from service.
     *
     * @param index voice tag index
     */
    void ProcessSendVoiceTag(int index);

    /**
     * @brief Process finish call event from service.
     *
     * @param event The finish call event.
     */
    void ProcessFinishCall(const HfpHfMessage &event);

    /**
     * @brief Process dial call event from service.
     *
     * @param event The dial call event.
     */
    void ProcessDialCall(const HfpHfMessage &event);

    /**
     * @brief Process open voice recognition event from service.
     */
    void ProcessOpenVoiceRecognition();

    /**
     * @brief Process close voice recognition event from service.
     */
    void ProcessCloseVoiceRecognition();

    /**
     * @brief Get the voice recognition state.
     *
     * @return Returns the voice recognition state.
     */
    int GetOpenVoiceRecognitionState() const;

    /**
     * @brief Add the deferred message.
     *
     * @param msg The message.
     */
    void AddDeferredMessage(const HfpHfMessage &msg);

    /**
     * @brief Process the deferred message.
     */
    void ProcessDeferredMessage();

    /**
     * @brief Get the service event name.
     *
     * @param what Service event number.
     * @return Returns service event string name.
     */
    static std::string GetEventName(int what);

    /**
     * @brief Get the service interactive event type name.
     *
     * @param type Interactive event type number.
     * @return Returns interactive event type string name.
     */
    std::string GetInteractiveEventType(int type) const;

    /**
     * @brief Notify the state change.
     */
    void NotifyStateTransitions();

    /**
     * @brief the state change from panrent state to child state.
     *
     * @param fromState The previous state.
     * @param toState The current state
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
     * @brief synchronize the sco event when sco is on.
     *
     * @param state : The state of the sco.
     */
    void SyncScoEvents(int state);

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

    /**
     * @brief Process Network state change from profile.
     *
     * @param event Network state change event.
     */
    void ProcessNetworkStateEvent(const HfpHfMessage &event);

    /**
     * @brief Process Network roam change from profile.
     *
     * @param event Network roam change event.
     */
    void ProcessNetworkRoamEvent(const HfpHfMessage &event);

    /**
     * @brief Process Network signal change from profile.
     *
     * @param event Network signal change event.
     */
    void ProcessNetworkSignalEvent(const HfpHfMessage &event);

    /**
     * @brief Process Battery level change from profile.
     *
     * @param event Battery level change event.
     */
    void ProcessBatteryLevelEvent(const HfpHfMessage &event);

    /**
     * @brief Process Battery level change from profile.
     *
     * @param event Battery level change event.
     */
    void ProcessOperatorEvent(const HfpHfMessage &event);

    /**
     * @brief Process Call indicator change event.
     *
     * @param event Call indicator change event.
     */
    void ProcessCallIndicatorEvent(const HfpHfMessage &event);

    /**
     * @brief Process Call indicator change event.
     *
     * @param event Call indicator change event.
     */
    void ProcessCurrentCallEvent(const HfpHfMessage &event) const;

    /**
     * @brief Process volume change event.
     *
     * @param event Volume change event.
     */
    void ProcessSetVolumeEvent(const HfpHfMessage &event) const;

    /**
     * @brief Process Subscriber number event.
     *
     * @param event Subscriber number event.
     */
    void ProcessSubscriberNumberEvent(const HfpHfMessage &event);

    /**
     * @brief Process Inband ring event.
     *
     * @param event Inband ring event.
     */
    void ProcessInbandRingEvent(const HfpHfMessage &event);

    /**
     * @brief Process voice recognition change event.
     *
     * @param event Voice recognition change event.
     */
    void ProcessVrChangedEvent(const HfpHfMessage &event);

    /**
     * @brief Process open voice recognition result event.
     *
     * @param event Open voice recognition result event.
     */
    void ProcessOpenVrResultEvent(const HfpHfMessage &event);

    /**
     * @brief Process close voice recognition result event.
     *
     * @param event Close voice recognition result event.
     */
    void ProcessCloseVrResultEvent(const HfpHfMessage &event);

    /**
     * @brief Notify the status of the voice recognition.
     *
     * @param status The status of the voice recognition.
     */
    void NotifyVoiceRecognitionStatusChanged(int status);

    /**
     * @brief Route hf audio state.
     *
     * @param flag The state to toute.
     */
    void routeHfAudio(bool state);

    inline static const std::string DISCONNECTED = "Disconnected";
    inline static const std::string CONNECTING = "Connecting";
    inline static const std::string DISCONNECTING = "Disconnecting";
    inline static const std::string CONNECTED = "Connected";
    inline static const std::string AUDIO_CONNECTING = "AudioConnecting";
    inline static const std::string AUDIO_DISCONNECTING = "AudioDisconnecting";
    inline static const std::string AUDIO_CONNECTED = "AudioConnected";

private:
    std::string address_;
    HfpHfProfile profile_;
    std::unique_ptr<HfpHfCallManager> calls_ {nullptr};
    bool isInBandRing_ {false};
    int networkState_ {0};
    int networkTRoam_ {0};
    int networkSignal_ {0};
    int batteryLevel_ {0};
    int volume_ {0};
    std::string operatorName_ {""};
    std::string subscriberNumber_ {""};
    std::list<HfpHfMessage> deferMsgs_ {};
    int preState_ {0};
    int voiceRecognitionStatus_ {HFP_HF_VR_STATE_CLOSED};
    std::unique_ptr<utility::Timer> connTimer_ {nullptr};
    inline static const int CONNECTION_TIMEOUT_MS {60000};  // 60s
    bool isRemoving_ {false};
    bool isAudioRouted_ {false};

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfStateMachine);
};

class HfpHfState : public utility::StateMachine::State {
public:
    HfpHfState(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile, int stateInt,
        utility::StateMachine::State &parent)
        : State(name, statemachine, parent), profile_(profile), stateInt_(stateInt), stateMachine_((HfpHfStateMachine &)statemachine)
    {}

    HfpHfState(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile, int stateInt)
        : State(name, statemachine), profile_(profile), stateInt_(stateInt), stateMachine_((HfpHfStateMachine &)statemachine)
    {}

    virtual ~HfpHfState()
    {}
    int GetStateInt() const
    {
        return stateInt_;
    }

protected:
    HfpHfProfile& profile_;
    int stateInt_ {HFP_HF_STATE_DISCONNECTED};
    HfpHfStateMachine &stateMachine_;
};

class HfpHfDisconnected : public HfpHfState {
public:
    HfpHfDisconnected(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile)
        : HfpHfState(name, statemachine, profile, HFP_HF_STATE_DISCONNECTED)
    {}
    ~HfpHfDisconnected() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
    bool isReentry_ {false};
};

class HfpHfConnecting : public HfpHfState {
public:
    HfpHfConnecting(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile)
        : HfpHfState(name, statemachine, profile, HFP_HF_STATE_CONNECTING)
    {}
    ~HfpHfConnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpHfDisconnecting : public HfpHfState {
public:
    HfpHfDisconnecting(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile)
        : HfpHfState(name, statemachine, profile, HFP_HF_STATE_DISCONNECTING)
    {}
    ~HfpHfDisconnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpHfConnected : public HfpHfState {
public:
    HfpHfConnected(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile)
        : HfpHfState(name, statemachine, profile, HFP_HF_STATE_CONNECTED)
    {}
    ~HfpHfConnected() override = default;
    void Entry() override;
    void Exit() override
    {}
    bool Dispatch(const utility::Message &msg) override;
};

class HfpHfAudioConnecting : public HfpHfState {
public:
    HfpHfAudioConnecting(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile,
        utility::StateMachine::State &parent)
        : HfpHfState(name, statemachine, profile, HFP_HF_AUDIO_STATE_CONNECTING, parent)
    {}
    ~HfpHfAudioConnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpHfAudioDisconnecting : public HfpHfState {
public:
    HfpHfAudioDisconnecting(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile,
        utility::StateMachine::State &parent)
        : HfpHfState(name, statemachine, profile, HFP_HF_AUDIO_STATE_DISCONNECTING, parent)
    {}
    ~HfpHfAudioDisconnecting() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class HfpHfAudioConnected : public HfpHfState {
public:
    HfpHfAudioConnected(const std::string &name, utility::StateMachine &statemachine, HfpHfProfile &profile,
        utility::StateMachine::State &parent)
        : HfpHfState(name, statemachine, profile, HFP_HF_AUDIO_STATE_CONNECTED, parent)
    {}
    ~HfpHfAudioConnected() override = default;
    void Entry() override;
    void Exit() override
    {}
    bool Dispatch(const utility::Message &msg) override;

private:
    void ProcessDisconnect(const HfpHfMessage &event);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_STATEMACHINE_H