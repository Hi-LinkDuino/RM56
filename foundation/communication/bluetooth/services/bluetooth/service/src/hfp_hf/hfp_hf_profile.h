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

#ifndef HFP_HF_PROFILE_H
#define HFP_HF_PROFILE_H

#include <cstdint>
#include <string>

#include "hfp_hf_audio_connection.h"
#include "hfp_hf_command_parser.h"
#include "hfp_hf_data_connection.h"
#include "hfp_hf_data_connection_server.h"
#include "hfp_hf_sdp_client.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for providing functions to relate to other underlying layers.
 *        The functions of this layer are called by HF state machine.
 */
class HfpHfProfile {
public:
    /**
     * @brief Register SDP service and data connection server for local device only once.
     */
    static int RegisterService();

    /**
     * @brief Deregister SDP service and data connection server for local device only once.
     */
    static int DeregisterService();

    /**
     * @brief Construct a new HfpHfProfile object.
     *
     * @param address Remote device address
     */
    explicit HfpHfProfile(std::string address);

    /**
     * @brief Destroy the HfpHfProfile object.
     */
    ~HfpHfProfile() = default;

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
    int DoServiceDiscovery(int role);

    /**
     * @brief Find out attributes required by HF and set the SDP result to data connection.
     *
     * @return Returns the find sdp discovery result.
     */
    int ServiceDiscoveryResult();

    /**
     * @brief Process service level connection established event.
     */
    void ProcessSlcEstablished();

    /**
     * @brief Establish data connection by HF.
     *
     * @return Returns the result of establish data connection.
     */
    int EstablishDataConnection();

    /**
     * @brief Release the existed data connection by HF.
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
     * @brief Establish service level connection between Audio Gateway and HandsFree Uint.
     *
     * @return Returns the result of establish service level connection.
     */
    int EstablishServiceLevelConnection();

    /**
     * @brief Read data from data connection.
     *
     * @return Returns the result of read data from data connection.
     */
    int ReadData();

    /**
     * @brief Select codec between AG and HF.
     *
     * @return Returns the result of select codec.
     */
    int SetupCodecConnection();

    /**
     * @brief Initiate the establishment of an audio connection to remote HF device by AG.
     *
     * @return Returns the result of establish audio data connection.
     */
    int EstablishAudioConnection();

    /**
     * @brief Release the existed audio connection between AG and HF by AG.
     *
     * @return Returns the result of release audio connection.
     */
    int ReleaseAudioConnection() const;

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
     * @brief Restore codec setup role.
     *
     * @return Returns the result of restore codec setup role.
     */
    int RestorCodecSetupRole();

    /**
     * @brief Process audio connect request in audio connecting state.
     *
     * @return Returns the result of process result.
     */
    int ProcessAudioConnReqInAudioConnecting();

    /**
     * @brief Query the current call list.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool QueryCurrentCalls();

    /**
     * @brief Query the current operator name.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool QueryOperatorName(bool isQuery);

    /**
     * @brief Query the subscriber information.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool QuerySubscriberInfo();

    /**
     * @brief Send the DTMF.
     *
     * @param code The code of the DTMF.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendDtmf(uint8_t code);

    /**
     * @brief Set the Volume.
     *
     * @param volume The value of the volume.
     * @param type The type of the volume.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SetHfVolume(int volume, int type);

    /**
     * @brief Open the voice recognition.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool OpenVoiceRecognition();

    /**
     * @brief Close the voice recognition.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool CloseVoiceRecognition();

    /**
     * @brief Dial out the call by the number.
     *
     * @param number The number of the call.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool DialOutCall(const std::string &number);

    /**
     * @brief Send key pressed event.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendKeyPressed();

    /**
     * @brief Dial out the call by the memory.
     *
     * @param number The index of the memory.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool DialMemory(int memory);

    /**
     * @brief Send voice tag.
     *
     * @param index The index of voice tag.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendVoiceTag(int index);

    /**
     * @brief Send ATA command.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendAta();

    /**
     * @brief Send CHLD command.
     *
     * @param action Action of CHLD.
     * @param idx Index to specific, default 0(mean no index).
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendChld(int action, int idx = 0);

    /**
     * @brief Send CHUP command.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendChup();

    /**
     * @brief Send BTRH command.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendBtrh(int action);

    /**
     * @brief Send BCC command.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool SendBcc();

    /**
     * @brief Send BLDN command, which used for calling the last phone number dialed.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool CallLastDialedNumber();

    /**
     * @brief Send NREC command to disable NREC of the remote AG device.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool CloseRemoteNrec();

    /**
     * @brief Reports the change of the battery level to remote HF device.
     *
     * @param battery Battery level.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool ReportBatteryLevel(uint32_t battery);

    /**
     * @brief Reports the change of the driver safety value to remote HF device.
     *
     * @param safety Driver safety value.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool ReportDriverSafety(uint32_t safety);

    /**
     * @brief Remove the hf state machine.
     */
    void RemoveStateMachine();

    /**
     * @brief Send the AT command.
     * @param cmdId : The id of the at command
     * @param arg : The string of the at command
     */
    void SendAtCommand(int cmdId, const std::string &arg);

    /**
     * @brief Process audio connect request.
     */
    void ProcessAudioConnectRequest();

    /**
     * @brief Remove scn loging.
     */
    void RemoveRemoteScnLoging() const;

private:
    /**
     * @brief Reports the specific indicator to remote AG device.
     *
     * @param indicator Indicator type.
     * @param value Indicator value.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool ReportHfIndicator(int indicator, int value);

    // Codec setup initiator
    bool isCodecSetupInitiator_ {false};

    // Remote device address
    std::string address_;
    // HfpHfSdpClient object
    HfpHfSdpClient sdpClient_ {};
    // HfpHFDataConnection object
    HfpHfDataConnection dataConn_ {};
    // HfpAgAudioConnection object
    HfpHfAudioConnection audioConn_ {};
    // HfpHfDataConnectionServer instance
    HfpHfDataConnectionServer &dataServer_ {HfpHfDataConnectionServer::GetInstance()};
    // HfpHfCommandProcessor
    HfpHfCommandProcessor commandProcessor_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfProfile);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_PROFILE_H