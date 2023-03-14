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

#ifndef HFP_AG_SYSTEM_EVENT_PROCESS_H
#define HFP_AG_SYSTEM_EVENT_PROCESS_H

#include <string>

#include "hfp_ag_message.h"
#include "hfp_ag_profile.h"
#include "hfp_ag_system_interface.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods that is used to process system event.
 */
class HfpAgSystemEventProcesser {
public:
    /**
     * @brief Construct a new HfpAgSystemEventProcesser object.
     *
     * @param profile AG profile instance.
     * @param address Remote device address.
     */
    HfpAgSystemEventProcesser(HfpAgProfile &profile, const std::string &address);

    /**
     * @brief Destroy the HfpAgSystemEventProcesser object.
     */
    ~HfpAgSystemEventProcesser() = default;

    /**
     * @brief Process system event.
     *
     * @param event The system event.
     */
    void ExecuteEventProcess(const HfpAgMessage &event);

    /**
     * @brief Start the system event processor.
     */
    static void Start();

    /**
     * @brief Stop the system event processor.
     */
    static void Stop();

    /**
     * @brief Set the audio actice device.
     *
     * @param audioActive The address of the bluetooth device.
     */
    static void SetAudioActive(const std::string &audioActive);

private:
    /**
     * @brief Process the event of answering call.
     */
    void ProcessAnswerCallEvent() const;

    /**
     * @brief Process the event of hanging up call.
     */
    void ProcessHangupCallEvent() const;

    /**
     * @brief Process the event of rejecting call.
     */
    void ProcessRejectCallEvent();

    /**
     * @briefProcess the event of the hf volume changed.
     *
     * @param type The volume type.
     * @param volume The volume valume.
     */
    void ProcessHfVolumeChangedEvent(int type, int volume) const;

    /**
     * @brief Process the event of dialing out call.
     *
     * @param number The number of the call.
     */
    void ProcessDialOutCallEvent(const std::string &number, int dialType);

    /**
     * @brief Process the event of sending the DTMF.
     *
     * @param dtmf The code of the DTMF.
     */
    void ProcessSendDtmfEvent(int dtmf) const;

    /**
     * @brief Process the event of noise reduction.
     *
     * @param flag Enable/disable flag.
     */
    void ProcessNoiseReductionEvent(int flag) const;

    /**
     * @brief Process the event of the wide band speech.
     *
     * @param codec The code of the wide band speech.
     */
    void ProcessWideBandSpeechEvent(int codec) const;

    /**
     * @brief Process the event of holding the call.
     *
     * @param chld The value of the CHLD command.
     */
    void ProcessHoldCallEvent(int chld) const;

    /**
     * @brief Get the Subscriber Number of the phone.
     */
    void GetSubscriberNumber() const;

    /**
     * @brief Get the Ag indicator.
     */
    void GetAgIndicator();

    /**
     * @brief Get the network operator of the phone.
     */
    void GetNetworkOperator() const;

    /**
     * @brief Get the current calls list.
     */
    void GetCurrentCalls() const;

    /**
     * @brief Process the event of the AT unknown command.
     *
     * @param atString The string of the command.
     */
    void ProcessAtUnknownEvent(const std::string &atString) const;

    /**
     * @brief Process the event of the key pressed.
     */
    void ProcessKeyPressedEvent() const;

    /**
     * @brief Process the event of the AT bind.
     *
     * @param atString The string of the command.
     */
    void ProcessATBindEvent(const std::string &atString) const;

    /**
     * @brief Process the event of sinding the hf indicator.
     *
     * @param indId The id of the indicator.
     * @param indValue The value of the indicator.
     */
    void SendHfIndicator(int indId, int indValue) const;

    /**
     * @brief Process the event of the AT BIA.
     *
     * @param data The BIA event.
     */
    void ProcessAtBiaEvent(const HfpAgMessage &event);

    /**
     * @brief process the event of querying AG indicator.
     */
    void QueryAgIndicator();

    /**
     * @brief The status of the voice recognition change.
     *
     * @param status The status of the voice recognition.
     */
    void VoiceRecognitionStateChanged(int status);

    /**
     * @brief process the event of update AG indicator.
     */
    void UpdataAgIndicator();

    /**
     * @brief get voice number.
     */
    void GetVoiceNumber();

    /**
     * @brief get response hold state.
     */
    void GetResponseHoldState(std::string address);

    void SetResponseHold(std::string address, int btrh);

    /**
     * @brief get response hold state.
     */
    void HandlePhoneStateMock(std::string number, int state, int type);

    /**
     * @brief Get the control other modules event type.
     *
     * @param type Event type number.
     * @return Returns the event type string name.
     */
    static std::string GetEventType(int type);

    // The quote of the HfpAgSystemInterface class.
    HfpAgSystemInterface &systemInterface_ {HfpAgSystemInterface::GetInstance()};
    // The pointer of the HfpAgProfile.
    HfpAgProfile& profile_;
    // The address of the bluetooth device.
    std::string address_ {""};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_SYSTEM_EVENT_PROCESS_H