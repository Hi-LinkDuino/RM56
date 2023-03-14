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

#ifndef PBAP_PSE_SDP_H
#define PBAP_PSE_SDP_H

#include <cstdint>
#include <cstring>

namespace OHOS {
namespace bluetooth {
/**
 * @brief pbap pce sdp service
 * pbap pce sdp service
 */
class PbapPseSdp {
public:
    /**
     * @brief constructor
     * @details constructor
     * @return
     */
    PbapPseSdp(uint8_t rfcommScn, uint16_t l2capPsm);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~PbapPseSdp() = default;

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
     */
    void Deregister() const;

private:
    static const uint16_t PABP_PROTOCOL_DESCRIPTOR_NUMBER = 3;
    // GoepL2capPsm (PBAP v1.2 and later)
    static const uint16_t PABP_GOEP_L2CAP_PSM_ATTRIBUTE_ID = 0x0200;
    static const uint16_t PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID = 0x0314;
    static const uint16_t PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID = 0x0317;
    static const uint16_t PBAP_PSE_UUID16 = 0x112F;
    static const uint16_t PBAP_PSE_PROFILE_UUID16 = 0x1130;
    static const uint16_t PBAP_PSE_VERSION_NUMBER = 0x0102;

    int AddServiceClassIdList() const;
    int AddProtocolDescriptorList() const;
    int AddAttributes();
    int AddBluetoothProfileDescriptorList() const;
    int AddServiceName() const;
    int AddBrowseGroupList() const;
    int RegisterServiceRecord() const;
    uint8_t rfcommScn_ = 0;   // rfcomm channel
    uint16_t l2capPsm_ = 0;   // l2cap psm
    uint32_t sdpHandle_ = 0;  // sdp handler
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_SDP_H
