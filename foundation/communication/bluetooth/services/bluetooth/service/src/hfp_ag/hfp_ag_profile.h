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

#ifndef HFP_AG_PROFILE_H
#define HFP_AG_PROFILE_H

#include <cstdint>
#include <string>

#include "hfp_ag_audio_connection.h"
#include "hfp_ag_command_parser.h"
#include "hfp_ag_data_connection.h"
#include "hfp_ag_data_connection_server.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_sdp_client.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for providing functions to relate to other underlying layers.
 *        The functions of this layer are called by AG state machine.
 */
class HfpAgProfile {
public:
    typedef struct {
        int status {0};
        int call {0};
        int callsetup {0};
        int callheld {0};
        int signal {0};
        int roam {0};
        int battchg {0};
    } CindIndicators;

    /**
     * @brief Register SDP service and data connection server for local device only once.
     */
    static int RegisterService();

    /**
     * @brief Deregister SDP service and data connection server for local device only once.
     */
    static int DeregisterService();

    /**
     * @brief Check if audio connection established;
     *
     * @return Returns <b>true</b> if audio connected; returns <b>false</b> if audio not connected.
     */
    static bool IsAudioConnected(const std::string &address);

    /**
     * @brief Construct a new HfpAgProfile object.
     *
     * @param address Remote device address.
     */
    explicit HfpAgProfile(const std::string &address);

    /**
     * @brief Destroy the HfpAgProfile object.
     */
    ~HfpAgProfile() = default;

    /**
     * @brief Initialise the profile object.
     */
    void Init();

    /**
     * @brief Start a SDP discovery job of remote SDP server.
     *
     * @param role Role in connection.
     * @return Returns the sdp discovery result.
     */
    int DoServiceDiscovery(int role, int handle = -1);

    /**
     * @brief  Find out attributes required by AG and set the SDP result to data connection.
     *
     * @return Returns the find sdp discovery result.
     */
    int ServiceDiscoveryResult();

    /**
     * @brief Process service level connection established event.
     */
    void ProcessSlcEstablished();

    /**
     * @brief Init inband ring tone according to features.
     */
    void InitInbandRingTone();

    /**
     * @brief Periodically send ring & clip to remote device.
     */
    void RingTimeout();

    /**
     * @brief Establish data connection by AG.
     *
     * @return Returns the result of establish data connection.
     */
    int EstablishDataConnection();

    /**
     * @brief Release the existed data connection by AG.
     *
     * @return Returns the result of release data connection.
     */
    int ReleaseDataConnection() const;

    /**
     * @brief Accept data connection from remote HF device.
     *
     * @param handle Data connection handle.
     * @return Returns the result of accept data connection.
     */
    int AcceptDataConnection(uint16_t handle);

    /**
     * @brief Read data from data connection.
     *
     * @return Returns the result of read data from data connection.
     */
    int ReadData();

    /**
     * @brief Initiate the establishment of an audio connection to remote HF device by AG.
     *
     * @return Returns the result of establish audio data connection.
     */
    int EstablishAudioConnection();

    /**
     * @brief Select codec between AG and HF.
     *
     * @return Returns the result of select codec.
     */
    int SetupCodecConnection();

    /**
     * @brief Setup codec connection to CVSD.
     *
     * @return Returns the result of setup codec.
     */
    int SetupCodecCvsd();

    /**
     * @brief Release the existed audio connection between AG and HF by AG.
     *
     * @return Returns the result of release audio connection.
     */
    int ReleaseAudioConnection() const;

    /**
     * @brief Process audio connect request.
     *
     * @return Returns <b>true</b> if accept audio connection; returns <b>false</b> if reject audio connection.
     */
    bool ProcessAudioConnectRequest();

    /**
     * @brief Accept audio connection request from HF.
     *
     * @return Returns the result of accept audio connection.
     */
    int AcceptAudioConnection();

    /**
     * @brief Reject audio connection request from HF.
     *
     * @return Returns the result of reject audio connection.
     */
    int RejectAudioConnection() const;

    /**
     * @brief AG shall initiate the audio connection establishment if audio connection is not present.
     *
     * @return Returns the result of active voice recognition.
     */
    int ActivateVoiceRecognition();

    /**
     * @brief Disable Voice recognition in AG.
     *
     * @return Returns the result of deactivate voice recognition.
     */
    int DeactivateVoiceRecognition() const;

    /**
     * @brief Set microphone gain to remote HF device.
     *
     * @param val Microphone gain value.
     * @return Returns the result of set mic value.
     */
    int SetMicrophoneGain(int val) const;

    /**
     * @brief Set speaker volume to remote HF device.
     *
     * @param val Speaker volume value.
     * @return Returns the result of set speaker value.
     */
    int SetSpeakerVolume(int val) const;

    /**
     * @brief Set the on/off action of in-band ring tone to remote HF device.
     *
     * @param action In-band ring tone on/off action.
     * @return Returns the result of set in-band ring.
     */
    int SetInbandRingTone(int action);

    /**
     * @brief Set the active device for audio connection.
     *
     * @param address Device address.
     */
    static void SetActiveDevice(const std::string &address);

    /**
     * @brief Get the active device for audio connection.
     *
     * @return Returns the active device.
     */
    static std::string GetActiveDevice();

    /**
     * @brief AG response to the query of subscriber number from HF.
     *
     * @param type Format type of the subscriber number.
     * @param number Subscriber number.
     * @param service Service the subscriber number relates to.
     * @return Returns the result of send subscriber number.
     */
    int SendSubscriberNumberInformation(
        uint16_t type, const std::string &number, int service = HFP_AG_CNUM_SERVICE_VOICE);

    /**
     * @brief Send the general and unsolicited AG results codes response.
     *
     * @param result Result code type.
     * @return Returns the result of send result code.
     */
    int SendResultCode(int result) const;

    void startMock(int state);

    /**
     * @brief Reports the change of the call status to remote HF device.
     *
     * @param call Call status.
     * @return Returns the result of report call status.
     */
    int ReportCallStatus(uint32_t call);

    /**
     * @brief Reports the change of the callsetup status to remote HF device.
     *
     * @param callsetup Callsetup status.
     * @return Returns the result of report callsetup status.
     */
    int ReportCallsetupStatus(uint32_t callsetup);

    /**
     * @brief Reports the change of the callheld status to remote HF device.
     *
     * @param callheld Callheld status.
     * @return Returns the result of report callheld status.
     */
    int ReportCallheldStatus(uint32_t callheld);

    /**
     * @brief Reports the change of the response hold status to remote HF device.
     *
     * @param state Response status.
     * @return Returns the result of report callheld status.
     */
    int ReportResponseHoldStatus(uint32_t state, int test);

    /**
     * @brief Reports the change of the registration status to remote HF device.
     *
     * @param status Registration status.
     * @return Returns the result of report registration status.
     */
    int ReportRegistrationStatus(uint32_t status);

    /**
     * @brief Reports the change of the signal strength to remote HF device.
     *
     * @param signal Signal strength.
     * @return Returns the result of report signal strength.
     */
    int ReportSignalStrength(uint32_t signal);

    /**
     * @brief Reports the change of the roaming status to remote HF device.
     *
     * @param state Roaming status.
     * @return Returns the result of report roaming status.
     */
    int ReportRoamingState(uint32_t state);

    /**
     * @brief Reports the change of the battery level to remote HF device.
     *
     * @param battery Battery level.
     * @return Returns the result of report battery level.
     */
    int ReportBatteryLevel(uint32_t battery);

    /**
     * @brief AG response to the query of network operator name from HF.
     *
     * @param operatorName Network operator name.
     * @return Returns the result of report network operator.
     */
    int ReportCurrentNetworkOperator(const std::string &operatorName);

    /**
     * @brief Reports the changes of current phone state to remote HF device.
     *
     * @param phoneState Current phone state.
     * @return Returns the result of phone state change.
     */
    int PhoneStateChange(const HfpAgPhoneState &phoneState);

    /**
     * @brief Reports the list of current calls to remote HF device.
     *
     * @param clcc Single current call.
     * @return Returns the result of report single call list.
     */
    int ReportCurrentCallList(const HfpAgCallList &clcc) const;

    /**
     * @brief respones ok to hf.
     *
     */
    void ResponesOK() const;

    /**
     * @brief Remote the state machine.
     */
    void RemoveStateMachine();

    /**
     * @brief Send RING and CLIP command to remote HF device.
     *
     * @return Returns the result of send ring.
     */
    void SendRingAndClip();

    /**
     * @brief Set the active device for audio connection.
     *
     * @param address Device address.
     * @return Returns the result of set active device.
     */
    void PostAudioConnectionEstablishment();

    /**
     * @brief Report indicators to hf.
     *
     * @param indicators Audio Gateway indicators.
     * @return Returns the result report indicators.
     */
    int ReportIndicators(const CindIndicators &indicators);

    /**
     * @brief Activate/Deactivate indicators notifications.
     *
     * @param service service indicator.
     * @param roam roam indicator.
     * @param signal signal indicator.
     * @param battery battery indicator.
     */
    void ProcessBia(bool service, bool roam, bool signal, bool battery);

    /**
     * @brief Remove scn loging.
     */
    void RemoveRemoteScnLoging() const;

    /**
     * @brief Reports the presence of new call waiting to remote HF device when there are ongoing calls.
     *
     * @param type Format type of the incoming call number.
     * @param number Incoming call number.
     * @return Returns the result of notify incoming call waiting.
     */
    int NotifyIncomingCallWaiting(uint16_t type, const std::string &number) const;

    /**
     * @brief send binp to hf
     *
     * @param number voice number
     */
    void SendBinp(std::string number) const;

    /**
     * @brief Reports the incoming call number to remote HF device after the new call coming.
     *
     * @param type Format type of the incoming call number.
     * @param number Incoming call number.
     * @return Returns the result of notify calling line identification.
     */
    int NotifyCallingLineIdentification(uint16_t type, const std::string &number) const;

private:
    /**
     * @brief Reports the change of the call status to remote HF device by active call number
     *        and held call number.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of report call status by call nums.
     */
    int ReportCallStatusByCallNums(int numActive, int numHeld);

    /**
     * @brief Reports the change of the callheld status to remote HF device by active call number
     *        and held call number.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of report callheld status by call nums.
     */
    int ReportCallheldStatusByCallNums(int numActive, int numHeld);

    /**
     * @brief Reports the specific indicator to remote HF device.
     *
     * @param indicator Indicator type.
     * @param value Indicator value.
     * @return Returns the result of report AG indicator.
     */
    int ReportAgIndicator(int indicator, int value) const;

    /**
     * @brief Send RING command to remote HF device.
     *
     * @return Returns the result of send ring.
     */
    int SendRing() const;

    /**
     * @brief Process phone change when current call state is dialing.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of process current call state dialing.
     */
    int ProcessCurrentCallStateDialing(int numActive, int numHeld);

    /**
     * @brief Process phone change when current call state is alerting.
     *
     * @return Returns the result of process current call state alerting.
     */
    int ProcessCurrentCallStateAlerting();

    /**
     * @brief Process phone change when current call state is incoming.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @param type Call type.
     * @param number Call number.
     * @return Returns the result of process current call state incoming.
     */
    int ProcessCurrentCallStateIncominging(int numActive, int numHeld, uint16_t type, const std::string &number);

    /**
     * @brief Process phone change when current call state is idle.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of process current call state idle.
     */
    int ProcessCurrentCallStateIdle(int numActive, int numHeld);

    /**
     * @brief Process phone change when current call state is idle and previous call state is dialing or alerting.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of process current call state idle and previous dialing or alerting.
     */
    int ProcessPreviousCallStateDialingAlerting(int numActive, int numHeld);

    /**
     * @brief Process phone change when current call state is idle and previous call state is incoming.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of process current call state idle and previous incoming.
     */
    int ProcessPreviousCallStateIncoming(int numActive, int numHeld);

    /**
     * @brief Process phone change when current call state is idle and previous call state is idle.
     *
     * @param numActive Active call number.
     * @param numHeld Held call number.
     * @return Returns the result of process current call state idle and previous idle.
     */
    int ProcessPreviousCallStateIdle(int numActive, int numHeld);

    /**
     * @brief Set the call states, including held calls number, active calls number and current call state.
     *
     * @param numHeld Held calls number.
     * @param numActive Active calls number.
     * @param callState Current call state.
     */
    void SetCallStates(int numHeld, int numActive, int callState);

    /**
     * @brief Get the call state name.
     *
     * @param state Call state number.
     * @return Returns the name of the call state.
     */
    static std::string GetCallState(int state);

    inline static const uint16_t HFP_AG_COPS_MODE_SET_FORMAT = 3;
    inline static const uint16_t HFP_AG_COPS_FORMAT_LONG = 0;

    // Remote device address
    std::string address_;

    int tempRfcommHandle_ {-1};

    // Last held calls number
    int preNumHeldCalls_ {0};

    // Last active calls number
    int preNumActiveCalls_ {0};

    // Last call state
    int preCallState_ {HFP_AG_CALL_STATE_IDLE};

    // Need Send ring after sco connected
    bool scoPostProcess_ {false};

    // HfpAgSdpClient object
    HfpAgSdpClient sdpClient_ {};

    // HfpAgDataConnection object
    HfpAgDataConnection dataConn_ {};

    // HfpAgAudioConnection object
    HfpAgAudioConnection audioConn_ {};

    // HfpAgDataConnectionServer instance
    HfpAgDataConnectionServer &dataServer_ {HfpAgDataConnectionServer::GetInstance()};

    // HfpAgCommandProcessor instance
    HfpAgCommandProcessor &commandProcessor_ {HfpAgCommandProcessor::GetInstance()};

    int mockState_ {0};
    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgProfile);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_PROFILE_H