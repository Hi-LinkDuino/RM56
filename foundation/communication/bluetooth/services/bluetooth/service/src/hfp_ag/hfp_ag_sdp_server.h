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

#ifndef HFP_AG_SDP_SERVER_H
#define HFP_AG_SDP_SERVER_H

#include <cstdint>
#include "base/base_def.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for managing SDP server register/unregister as HFP AG role.
 */
class HfpAgSdpServer {
public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns HfpAgSdpServer instance.
     */
    static HfpAgSdpServer &GetInstance();

    /**
     * @brief Create and register SDP service required by HFP Ag role, including Service class id,
     *        protocol, profile, service name, network, AG support features and browse group list.
     *
     * @param scn Server channel number.
     * @return Returns error code of register sdp service result.
     */
    int RegisterSdpService(uint8_t scn);

    /**
     * @brief Destroy and unregister the previously registered SDP service.
     *
     * @return Returns error code of deregister sdp service result.
     */
    int DeregisterSdpService();

private:
    /**
     * @brief Construct a new HfpAgSdpServer object.
     */
    HfpAgSdpServer() = default;

    /**
     * @brief Destroy the HfpAgSdpServer object.
     */
    ~HfpAgSdpServer() = default;

    /**
     * @brief Add service class id info.
     *
     * @return Returns error code of the result.
     */
    int AddServiceClassId() const;

    /**
     * @brief Add protocol info.
     *
     * @param scn Server channel number.
     * @return Returns error code of the result.
     */
    int AddProtocol(uint8_t scn) const;

    /**
     * @brief Add profile info.
     *
     * @return Returns error code of the result.
     */
    int AddProfile() const;

    /**
     * @brief Add service name info.
     *
     * @return Returns error code of the result.
     */
    int AddServiceName() const;

    /**
     * @brief Add network info.
     *
     * @return Returns error code of the result.
     */
    int AddNetwork() const;

    /**
     * @brief Add local features.
     *
     * @return Returns error code of the result.
     */
    int AddFeatures() const;

    /**
     * @brief Add browse group list info.
     *
     * @return Returns error code of the result.
     */
    int AddBrowseGroupList() const;

    /**
     * @brief Add attribute info.
     *
     * @return Returns error code of the result.
     */
    int AddAttribute(const SdpAttribute &attribute) const;

    inline static constexpr uint16_t HFP_AG_SERVER_CLASSID_NUM = 2;
    inline static constexpr uint16_t HFP_AG_HSP_SERVER_CLASSID_NUM = 3;
    inline static constexpr uint16_t HFP_AG_SERVER_PROTOCOL_NUM = 2;
    inline static constexpr uint16_t HFP_AG_SERVER_PROFILE_NUM = 1;
    inline static constexpr uint16_t HFP_AG_HSP_SERVER_PROFILE_NUM = 2;
    inline static constexpr uint16_t HFP_AG_SERVER_BROWSE_LIST_NUM = 1;
    inline static constexpr uint16_t HFP_AG_SERVER_NETWORK_LENGTH = 1;
    inline static constexpr uint16_t HFP_AG_SERVER_FEATURES_LENGTH = 2;
    inline static constexpr uint16_t HSP_AG_SERVER_CLASSID_NUM = 2;

    // SDP service record handle
    uint32_t sdpHandle_ {0};

    int hspState_ = 1;

    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgSdpServer);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_SDP_SERVER_H