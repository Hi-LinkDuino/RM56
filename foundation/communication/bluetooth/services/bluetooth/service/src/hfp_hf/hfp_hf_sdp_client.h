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

#ifndef HFP_HF_SDP_CLIENT_H
#define HFP_HF_SDP_CLIENT_H

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "base_def.h"
#include "btstack.h"
#include "hfp_hf_defines.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
typedef struct {
    uint16_t attributeId {0};
    SdpDataType type {SDP_TYPE_UINT_8};
    uint16_t attributeValue {0};
} HfpHfSdpAttribute;

typedef struct {
    std::vector<SdpProtocolDescriptor> descriptors {};        // AG protocol descriptor
    std::vector<SdpProfileDescriptor> profileDescriptors {};  // AG profile descriptor
    std::vector<HfpHfSdpAttribute> attributes {};             // AG attribute descriptor
} HfpHfRemoteSdpService;

typedef struct {
    std::vector<HfpHfRemoteSdpService> services {};  // AG service descriptor
} HfpHfRemoteSdpServiceArray;

/**
 * @brief Class for discovery remote AG device's SDP server and finding out
 *        related SDP info required by HFP HF role.
 */
class HfpHfSdpClient {
public:
    /**
     * @brief Construct a new HfpHfSdpClient object.
     */
    HfpHfSdpClient() = default;

    /**
     * @brief Destroy the HfpHfSdpClient object.
     */
    ~HfpHfSdpClient();

    /**
     * @brief Callback function of SDP discovery.
     *
     * @param addr Remote device address defined bt stack.
     * @param serviceAry Array of services discovered.
     * @param serviceNum Number of services discovered.
     * @param context Upper layer context.
     */
    static void SdpCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context);

    /**
     * @brief Start a service discovery job.
     *
     * @param remoteAddr Remote device address.
     * @param role Role in connection.
     * @return Returns the error code of the discovery result.
     */
    int DoDiscovery(const std::string &remoteAddr, int role);

    /**
     * @brief Callback function of SDP discovery for HSP.
     *
     * @param addr Remote device address defined bt stack.
     * @param serviceAry Array of services discovered.
     * @param serviceNum Number of services discovered.
     * @param context Upper layer context.
     */
    static void SdpHspCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context);

    /**
     * @brief Start a service discovery job.
     *
     * @param remoteAddr Remote device address.
     * @param role Role in connection.
     * @return Returns the error code of the discovery result.
     */
    int DoHspAgDiscovery(const std::string &remoteAddr);

    /**
     * @brief Start a find service attributes job.
     *
     * @param remoteAddr Remote device address.
     * @param role Role in connection.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool FindAttributes(const std::string &remoteAddr, int role);

    /**
     * @brief Get the Remote Sdp Info object.
     *
     * @return Returns the HfpAgRemoteSdpInfo object.
     */
    HfpHfRemoteSdpInfo GetRemoteSdpInfo() const;

private:
    /**
     * @brief Cache SDP result data locally.
     *
     * @param remoteAddr Remote device address.
     * @param serviceAry Array of services discovered.
     * @param serviceNum Number of services discovered.
     */
    static void CopySdpServiceArray(const std::string &remoteAddr, const SdpService *serviceAry, uint16_t serviceNum);

    /**
     * @brief Delete local cached SDP result data.
     *
     * @param array Array of services Reserved.
     */
    static void DeleteSdpServiceArray(HfpHfRemoteSdpServiceArray &array);

    /**
     * @brief Loop all protocol to find rfcomm scn.
     *
     * @param loopNum Loop Number.
     * @param array Remote AG device SDP service array.
     * @param scn Server channel number.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    bool LoopAllProtocolRfcomm(uint16_t &loopNum, const HfpHfRemoteSdpServiceArray &array, uint8_t &scn) const;

    /**
     * @brief Find out remote server channel number.
     *
     * @param protocols Protocol descriptors.
     * @param scn[out] Remote server channel number.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    static bool FindProtocolRfcomm(const std::vector<SdpProtocolDescriptor> &protocols, uint8_t &scn);

    /**
     * @brief Find out remote profile version.
     *
     * @param profiles Profile descriptors.
     * @param version[out] Remote profile version.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    static bool FindProfileVersion(const std::vector<SdpProfileDescriptor> &profiles, uint16_t &version);

    /**
     * @brief Find out remote AG features.
     *
     * @param attributes Attribute descriptors.
     * @param features[out] Remote AG features.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    static bool FindProfileFeatures(const std::vector<HfpHfSdpAttribute> &attributes, uint16_t &features);

    /**
     * @brief Find out remote AG network for ability to reject calls in Attribute descriptor.
     *
     * @param attributes Attribute descriptors.
     * @param network Network value.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> if the operation fails.
     */
    static bool FindProfileNetwork(const std::vector<HfpHfSdpAttribute> &attributes, uint8_t &network);

    /**
     * @brief Select remote AG features and codec.
     *
     * @param features[out] Remote AG features.
     * @param codec [out] Remote AG features.
     */
    static void SelectProfileFeatures(uint16_t &features, int &codec);

    // Attribute occupy one byte
    inline static constexpr int ATTRIBUTE_LENGTH_UINT8 = 1;

    // Attribute occupy two bytes
    inline static constexpr int ATTRIBUTE_LENGTH_UINT16 = 2;

    inline static constexpr uint16_t HFP_HF_CLIENT_CLASSID_NUM = 1;
    inline static constexpr uint16_t HFP_HF_CLIENT_INITIATOR_ATTR_NUM = 5;
    inline static constexpr uint16_t HFP_HF_CLIENT_ACCEPTOR_ATTR_NUM = 4;
    inline static constexpr int SERVICE_CLASS_ID_LIST_INDEX = 0;
    inline static constexpr int PROTOCOL_DESCRIPTOR_LIST_INDEX = 1;
    inline static constexpr int INITIATOR_PROFILE_DESCRIPTOR_LIST_INDEX = 2;
    inline static constexpr int ACCEPTER_PROFILE_DESCRIPTOR_LIST_INDEX = 1;
    inline static constexpr int INITIATOR_SUPPORTED_FEATURES_INDEX = 3;
    inline static constexpr int ACCEPTER_SUPPORTED_FEATURES_INDEX = 2;
    inline static constexpr int INITIATOR_DATA_STORES_OR_NETWORK_INDEX = 4;
    inline static constexpr int ACCEPTER_DATA_STORES_OR_NETWORK_INDEX = 3;

    // Remote device SDP info after DoDiscovery()
    static std::map<std::string, HfpHfRemoteSdpServiceArray> g_remoteSdpServiceArrays;

    // Current remote device address
    std::string currentAddr_ {""};

    // Remote device SDP info after last FindAttributes()
    HfpHfRemoteSdpInfo remoteSdpInfo_ {};

    // The mutex variable
    static std::recursive_mutex g_hfpSdpMutex;

    int hspState_ = 1;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfSdpClient);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_SDP_CLIENT_H