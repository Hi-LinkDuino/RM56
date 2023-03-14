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

#ifndef PBAP_PCE_SDP_H
#define PBAP_PCE_SDP_H

#include <cstdint>
#include <cstring>
#include <memory>
#include "interface_profile_pbap_pce.h"
#include "raw_address.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief pbap pce sdp service
 * pbap pce sdp service
 */
class PbapPceSdp {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param service
     * @return
     */
    explicit PbapPceSdp(const IProfilePbapPce &service);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~PbapPceSdp();

    /**
     * @brief register sdp
     * @details register sdp service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Register();

    /**
     * @brief deregister sdp
     * @details deregister sdp service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Deregister() const;

    /**
     * @brief search sap recode
     * @details search sap recode
     * @param device remote address
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int SdpSearch(const RawAddress &device);

    /**
     * @brief Callback of SDP discovery
     * @details Callback of SDP discovery
     * @param addr The address of the bluetooth device
     * @param serviceAry The serviceAry is Array of services discovered
     * @param serviceNum The serviceNum is number of services discovered
     * @param context The context is used to send the event in the callback
     */
    static void SdpSearchCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context);

private:
    void SdpSearchCallback(const BtAddr *addr, const SdpService &serviceArray, uint16_t serviceNum) const;
    static uint8_t FindRFCommChannel(const SdpService &serviceAry, bool &isGoepL2capPSM);
    static uint16_t FindVersionNumber(const SdpService &serviceAry);
    static bool FindL2capPSM(const SdpService &serviceAry, uint16_t &psm, uint8_t &supportedRes,
        uint32_t &supportedFeature, bool &featureFlag);

private:
    uint32_t sdpHandle_ = 0;                                                  // sdp record handler
    static const uint16_t PBAP_PSE_UUID16 = 0x112F;                           // pse uuid
    static const uint16_t PBAP_PCE_UUID16 = 0x112E;                           // pce uuid
    static const uint16_t PBAP_PCE_PROFILE_UUID16 = 0x1130;                   // pce profile uuid
    static const uint16_t PBAP_PCE_VERSION_NUMBER = 0x0102;                   // pce version number
    static const uint16_t PBAP_GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;          // pce psm value
    static const uint16_t PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID = 0x0314;  // Supported pbap functions
    static const uint16_t PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID = 0x0317;      // Supported pbap features
    static const uint32_t PBAP_PCE_SUPPORTED_FEATURES = 0x03;                 // Supported default features
    const IProfilePbapPce &service_;                                          // PbapPceService
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PCE_SDP_H