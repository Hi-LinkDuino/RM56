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

#ifndef HFP_HF_PROFILE_EVENT_SENDER_H
#define HFP_HF_PROFILE_EVENT_SENDER_H

#include <cstdint>
#include <string>

#include "hfp_hf_message.h"
#include "hfp_hf_service.h"
#include "message.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides a set of methods that is used to sent the event to service.
 */
class HfpHfProfileEventSender {
public:
    /**
     * @brief Get the instance of the HfpHfProfileEventSender.
     *
     * @return Returns the quote of the HfpHfProfileEventSender instance.
     */
    static HfpHfProfileEventSender &GetInstance();

    /**
     * @brief Send the event of the network state change.
     *
     * @param address The device address.
     * @param state The state of the network.
     */
    void NetworkStateChanged(const std::string &address, int state) const;

    /**
     * @brief Send the event of the network roam change.
     *
     * @param address The device address.
     * @param state The state of the roam.
     */
    void NetworkRoamingChanged(const std::string &address, int state) const;

    /**
     * @brief Send the event of the signal strength change.
     *
     * @param address The device address.
     * @param signal The signal strength.
     */
    void NetworkSignalChanged(const std::string &address, int signal) const;

    /**
     * @brief Send the event of the battery level change.
     *
     * @param address The device address.
     * @param level The battery level.
     */
    void BatteryLevelChanged(const std::string &address, int level) const;

    /**
     * @brief Send the event of the network operator.
     *
     * @param address The device address.
     * @param name The network operator name.
     */
    void CurrentOperatorReply(const std::string &address, const std::string &name) const;

    /**
     * @brief Send the event of the call state.
     *
     * @param address The device address.
     * @param call The state of call.
     */
    void CallStateNotify(const std::string &address, int call) const;

    /**
     * @brief Send the setup state of the call.
     *
     * @param address The device address.
     * @param callsetup The state of call setup.
     */
    void CallSetupStateNotify(const std::string &address, int callsetup) const;

    /**
     * @brief Send the held state of the call.
     *
     * @param address The device address.
     * @param callheld The state of call held.
     */
    void CallHeldStateNotify(const std::string &address, int callheld) const;

    /**
     * @brief Send the hold result.
     *
     * @param address The device address.
     * @param respAndHold The result of the hold.
     */
    void HoldResultReply(const std::string &address, int respAndHold) const;

    /**
     * @brief Send the calling line identification.
     *
     * @param address The device address.
     * @param number The number of incoming call.
     */
    void Callinglineidentification(const std::string &address, const std::string &number) const;

    /**
     * @brief Send the call waiting.
     *
     * @param address The device address.
     * @param number The number of the waiting call.
     */
    void CallWaitingNotify(const std::string &address, const std::string &number) const;

    /**
     * @brief Send the current call.
     *
     * @param address The device address.
     * @param call The current call.
     */
    void CurrentCallsReply(const std::string &address, const HfpHfCurrentCallData &call) const;

    /**
     * @brief Set the hf volume.
     *
     * @param address The device address.
     * @param type The type of the volume.
     * @param volume The value of the volume.
     */
    void SetVolume(const std::string &address, int type, int volume) const;

    /**
     * @brief Send the result of the at cmd.
     *
     * @param address The device address.
     * @param type The type of cmd.
     * @param cme The result of cmd.
     */
    void AtCmdResultReply(const std::string &address, int type, int cme) const;

    /**
     * @brief Send the subscriber number.
     *
     * @param address The device address.
     * @param number The subscriber number
     * @param type The type of the number.
     */
    void SubscriberNumberReply(const std::string &address, const std::string &number, int type) const;

    /**
     * @brief Set the in-banding ring
     *
     * @param address The device address.
     * @param inBand the value of the in-banding ring.
     */
    void SetInBandRing(const std::string &address, int inBand, bool isFirstTime) const;

    /**
     * @brief Send the indicator of the ring.
     *
     * @param address The device address.
     */
    void RingIndicationNotify(const std::string &address) const;

    /**
     * @brief Send the unknown event.
     *
     * @param address The device address.
     * @param eventString The event string.
     */
    void UnknownEventNotify(const std::string &address, const std::string &eventString) const;

    /**
     * @brief Transfer connect request.
     *
     * @param device Remote bluetooth device address.
     * @param handle Rfcomm handle.
     * @param what Event tag.
     */
    void ConnectRequest(const std::string &device, int handle, int what) const;

    /**
     * @brief Update the state of the slc connection.
     *
     * @param address Remote bluetooth device address.
     * @param what Event tag.
     */
    void UpdateConnectState(const std::string &device, int what) const;

    /**
     * @brief Transfer sco connect request.
     *
     * @param device remote bluetooth address.
     * @param what the event tag.
     * @param linkType sco link type
     */
    void ScoConnectRequest(const std::string &device, int what, uint8_t linkType) const;

    /**
     * @brief Update the state of the sco connection.
     *
     * @param device: remote bluetooth address.
     * @param what: the event tag.
     */
    void UpdateScoConnectState(const std::string &device, int what) const;

    /**
     * @brief Process the result of the sdp discovery.
     *
     * @param address Remote bluetooth device address.
     * @param what Event tag.
     */
    void ProcessSdpDiscoveryResult(const std::string &device, int what) const;

    /**
     * @brief Report the current call list reply done.
     *
     * @param address Remote bluetooth address
     */
    void CurrentCallsReplyDone(const std::string &address) const;

    /**
     * @brief Report remove state machine.
     *
     * @param address Remote bluetooth address.
     */
    void RemoveSateMachine(const std::string &address) const;

    /**
     * @brief Notify the result of opening voice recognition.
     *
     * @param address The bluetooth device address.
     * @param result the result of opening voice recognition.
     */
    void OpenVoiceRecognitionResult(const std::string &address, int result) const;

    /**
     * @brief Notify the result of closing voice recognition.
     *
     * @param address The bluetooth device address.
     * @param result The result of closing voice recognition.
     */
    void CloseVoiceRecognitionResult(const std::string &address, int result) const;

    /**
     * @brief notify the voice recognition change.
     *
     * @param address The bluetooth device address.
     * @param status The status of the the voice recognition.
     */
    void VoiceRecognitionChanged(const std::string &address, int status) const;

    /**
     * @brief Get service dispatcher.
     *
     * @return service dispatcher.
     */
    utility::Dispatcher *GetDispatchter() const;

private:
    /**
     * @brief Construct a new HfpHfProfileEventSender object.
     */
    HfpHfProfileEventSender() = default;

    /**
     * @brief Destroy the HfpHfProfileEventSender object.
     */
    ~HfpHfProfileEventSender() = default;

    /**
     * @brief Send message to HandsfreeUnit service.
     *
     * @param event HandsfreeUnit message.
     */
    static void SendMessageToService(const HfpHfMessage &event);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_PROFILE_EVENT_SENDER_H
