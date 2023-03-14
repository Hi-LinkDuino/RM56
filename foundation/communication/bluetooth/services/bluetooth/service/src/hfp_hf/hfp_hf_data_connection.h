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

#ifndef HFP_HF_DATA_CONNECTION_H
#define HFP_HF_DATA_CONNECTION_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "base_def.h"
#include "hfp_hf_defines.h"
#include "hfp_hf_rfcomm_connection.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for HFP HF data connection.
 */
class HfpHfDataConnection {
    typedef struct {
        std::string name {""};
        uint32_t index {0};
        uint32_t minVal {0};
        uint32_t maxVal {0};
        uint32_t value {0};
        bool isEnabled {false};
    } AgIndicator;

    typedef struct {
        uint16_t anum {0};
        uint32_t value {0};
        uint32_t minVal {0};
        uint32_t maxVal {0};
        bool isSupported {false};
        bool isEnabled {false};
    } HfIndicator;

public:
    /**
     * @brief Data connection callback function.
     *
     * @param handle Handle.
     * @param eventId Event id.
     * @param eventData Event data.
     * @param context Context.
     */
    static void DataConnectionCallback(uint16_t handle, uint32_t eventId, const void *eventData, void *context);

    /**
     * @brief Get the AG local features.
     *
     * @return Returns the local AG support features.
     */
    static uint32_t GetLocalFeatures();

    /**
     * @brief Construct a new HfpHfDataConnection object.
     */
    HfpHfDataConnection() = default;

    /**
     * @brief Destroy the HfpHfDataConnection object.
     */
    ~HfpHfDataConnection() = default;

    /**
     * @brief Init data connection.
     */
    static int Init();

    /**
     * @brief Clean up data connection.
     */
    static void CleanUp();

    /**
     * @brief  This function used to connect remote device rfcomm channel after SDP discovery OK (Initiator).
     *
     * @return Returns the error code of the connect result.
     */
    int Connect();

    /**
     * @brief This function used to disconnect remote device rfcomm channel.
     *
     * @return Returns the error code of the disconnect result.
     */
    int Disconnect() const;

    /**
     * @brief  This function used to read data from remote device.
     *
     * @param pkt The address of data packet pointer.
     * @return Returns the error code of the read data result.
     */
    int ReadData(Packet **pkt) const;

    /**
     * @brief This function used to write data to remote device.
     *
     * @param pkt Data packet pointer.
     * @return Returns the error code of the write data result.
     */
    int WriteData(Packet &pkt) const;

    /**
     * @brief Set the remote address.
     *
     * @param addr Remote device address.
     */
    void SetRemoteAddr(const std::string &addr);

    /**
     * @brief Set the SDP info.
     *
     * @param sdpInfo Remote device SDP info.
     */
    void SetSdpInfo(HfpHfRemoteSdpInfo sdpInfo);

    /**
     * @brief Get the connection role.
     *
     * @return Returns the connection role.
     */
    int GetRole() const;

    /**
     * @brief Set the connection role.
     *
     * @param role Role in connection.
     */
    void SetRole(int role);

    /**
     * @brief Get the connection handle.
     *
     * @return Returns the connection handle.
     */
    uint16_t GetConnectionHandle() const;

    /**
     * @brief Set the connection handle.
     *
     * @param handle Connection handle.
     */
    void SetConnectionHandle(uint16_t handle);

    /**
     * @brief Set the SLC connect state.
     *
     * @param state SLC connect state.
     */
    void SetSlcConnectState(bool state);

    /**
     * @brief  This function used to get support of codec negotiation.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool IsCodecNegotiationSupport() const;

    /**
     * @brief This function used to get support of ESCO.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    static bool IsEscoSupport();

    /**
     * @brief This function used to get support of ESCO S4.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool IsEscoS4Support() const;

    /**
     * @brief This function get remote server channel number.
     *
     * @return Remote server channel number
     */
    uint8_t GetRemoteScn() const;

private:
    /**
     * @brief Process Data connection callback function in hfp thread.
     *
     * @param handle Handle.
     * @param eventId Event id.
     */
    static void ProcessDataConnectionCallback(uint16_t handle, uint32_t eventId);

    friend class HfpHfProfile;
    friend class HfpHfCommandProcessor;

    static inline const std::string BIND_SETTINGS = "1,2";  // Enhanced Driver Status & Battery Level Status
    static constexpr uint8_t ANUM_CVSD = 0x02;              // Coding Format(1 Octet) Assign Number CVSD
    static constexpr uint8_t ANUM_MSBC = 0x05;              // Coding Format(1 Octet) Assign Number MSBC
    static uint32_t g_localFeatures;
    static inline int g_localSupportCodecs {HFP_HF_CODEC_NONE};
    int role_ {HFP_HF_ACCEPTOR};
    HfpHfConnectState state_ {HFP_HF_STATE_DISCONNECTED};
    uint32_t remoteFeatures_ {0};
    int localDemandCodec_ {HFP_HF_CODEC_MSBC};
    int localSelectedCodec_ {HFP_HF_CODEC_MSBC};
    int remoteSelectedCodec_ {HFP_HF_CODEC_MSBC};
    int remoteSupportCodecs_ {HFP_HF_CODEC_NONE};
    int inUseCodec_ {HFP_HF_CODEC_CVSD};
    bool slcConnected_ {false};
    uint32_t remoteChldFeatures_ {0};
    std::string remoteAddr_ {""};
    HfpHfRemoteSdpInfo sdpInfo_ {};
    std::vector<AgIndicator> remoteAgIndicators_ {};
    std::vector<HfIndicator> remoteHfIndicators_ {};
    std::vector<HfIndicator> localHfIndicators_ = {
        {1, 0, 0, 1, true, true},   /* Enhanced Driver Status */
        {2, 0, 0, 100, true, true}  /* Battery Level Status */
    };
    HfpHfRfcommConnection rfcommConnection_ {&HfpHfDataConnection::DataConnectionCallback};

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfDataConnection);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_DATA_CONNECTION_H