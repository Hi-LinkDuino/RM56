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

#ifndef HFP_AG_PROFILE_EVENT_SENDER_H
#define HFP_AG_PROFILE_EVENT_SENDER_H

#include <cstdint>
#include <string>

#include "dispatcher.h"
#include "hfp_ag_message.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods that is used to sent the event to service.
 */
class HfpAgProfileEventSender {
public:
    /**
     * @brief Get the instance of the HfpAgProfileEventSender.
     *
     * @return Returns the quote of the HfpAgProfileEventSender instance.
     */
    static HfpAgProfileEventSender &GetInstance();

    /**
     * @brief Answer the call.
     *
     * @param device Remote bluetooth address.
     */
    void AnswerCall(const std::string &device) const;

    /**
     * @brief Hangup the call.
     *
     * @param device Remote bluetooth address.
     */
    void HangupCall(const std::string &device) const;

    /**
     * @brief Hf notify the volume changed.
     *
     * @param device Remote bluetooth address.
     * @param type Speaker or microphone.
     * @param volume The value of the volume.
     */
    void HfVolumeChanged(const std::string &device, int type, int volume) const;

    /**
     * @brief Dial out the call.
     *
     * @param device Remote bluetooth address.
     * @param number The number of the call.
     */
    void DialOutCall(const std::string &device, const std::string &number, int dialType) const;

    /**
     * @brief send the DTMF.
     *
     * @param device Remote bluetooth address.
     * @param dtmf Code of DTMF.
     */
    void SendDtmf(const std::string &device, int dtmf) const;

    /**
     * @brief Enable the noise reduction.
     *
     * @param device Remote bluetooth address.
     * @param enable True or false.
     */
    void EnableNoiseReduction(const std::string &device, bool enable) const;

    /**
     * @brief hold the call.
     *
     * @param device Remote bluetooth address.
     * @param chld Value of the CHLD.
     */
    void HoldCall(const std::string &device, int chld) const;

    /**
     * @brief Get the number of the phone.
     *
     * @param device Remote bluetooth address.
     */
    void GetSubscriberNumber(const std::string &device) const;

    /**
     * @brief Get the indicator of the AG.
     *
     * @param device Remote bluetooth address.
     */
    void GetAgIndicator(const std::string &device) const;

    /**
     * @brief Get the network operater.
     *
     * @param device Remote bluetooth address.
     */
    void GetNetworkOperater(const std::string &device) const;

    /**
     * @brief Get the current call list.
     *
     * @param device Remote bluetooth address.
     */
    void GetCurrentCalls(const std::string &device) const;

    /**
     * @brief Process the indicator of the HF.
     *
     * @param device Remote bluetooth address.
     * @param atString AT command.
     */
    void ProcessATBind(const std::string &device, const std::string &atString) const;

    /**
     * @brief Process Audio Connection Setup by HF event.
     *
     * @param device Remote bluetooth address.
     */
    void ProcessAtBcc(const std::string &device) const;

    /**
     * @brief Process codec negotiation completed event.
     *
     * @param device Remote bluetooth address.
     */
    void ProcessAtBcs(const std::string &device) const;

    /**
     * @brief Send the indicator of the HF.
     *
     * @param device Remote bluetooth address.
     * @param indId Id of hf indicator.
     * @param indValue Value of hf indicator.
     */
    void SendHfIndicator(const std::string &device, int indId, int indValue) const;

    /**
     * @brief Update the state of the ag indictor enable.
     *
     * @param device Remote bluetooth address.
     * @param data The enable tag of the ag indictor
     */
    void ProcessAtBia(const std::string &device, const HfpAgTransferData &data) const;

    /**
     * @brief Transfer connect request.
     *
     * @param device Remote bluetooth address.
     * @param handle rfcomm handle.
     * @param what The event tag.
     */
    void ConnectRequest(const std::string &device, int handle, int what) const;

    /**
     * @brief Update the state of the slc connection.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     */
    void UpdateConnectState(const std::string &device, int what) const;

    /**
     * @brief Transfer sco connect request.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     * @param linkType Sco link type.
     */
    void ScoConnectRequest(const std::string &device, int what, uint8_t linkType) const;

    /**
     * @brief Update the state of the sco connection.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     */
    void UpdateScoConnectState(const std::string &device, int what) const;

    /**
     * @brief Get response and hold state.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     */
    void GetResponseHoldState(const std::string &device, int what) const;

    /**
     * @brief Get response and hold state.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     */
    void SetResponseHoldState(const std::string &device, int what, int btrh) const;

    /**
     * @brief Process the result of the sdp discovery.
     *
     * @param device Remote bluetooth address.
     * @param what The event tag.
     */
    void ProcessSdpDiscoveryResult(const std::string &device, int what) const;

    /**
     * @brief Remove the state machine.
     *
     * @param device Remote bluetooth address.
     */
    void RemoveStateMachine(const std::string &device) const;

    /**
     * @brief The status of the voice recognition change.
     *
     * @param device Remote bluetooth address.
     * @param status The status of the voice recognition
     */
    void VoiceRecognitionStateChanged(const std::string &device, int status) const;

    /**
     * @brief Set active device success event.
     *
     * @param device Remote bluetooth address.
     */
    void SetActiveDeviceSuccess(const std::string &device) const;

    /**
     * @brief Set ring and clip event.
     *
     * @param device Remote bluetooth address.
     */
    void SendRingAndClip(const std::string &device) const;

    /**
     * @brief Get voice number.
     *
     * @param device Remote bluetooth address.
     */
    void GetVoiceTagNumber(const std::string &device) const;

    /**
     * @brief Process CKpd Event.
     *
     * @param device Remote bluetooth address.
     */
    void ProcessCKpdEvent(const std::string &device) const;

    /**
     * @brief Get service dispatcher.
     *
     * @return service dispatcher.
     */
    utility::Dispatcher *GetDispatchter() const;

private:
    /**
     * @brief Send the message to the service.
     *
     * @param msg The message object.
     */
    static void SendMessageToService(const HfpAgMessage &msg);

    /**
     * @brief Construct a new HfpAgProfileEventSender object.
     */
    HfpAgProfileEventSender() = default;

    /**
     * @brief Destroy the HfpAgProfileEventSender object.
     */
    ~HfpAgProfileEventSender() = default;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_PROFILE_EVENT_SENDER_H
