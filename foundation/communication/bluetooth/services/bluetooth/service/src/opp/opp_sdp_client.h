/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OPP_SDP_CLIENT_H
#define OPP_SDP_CLIENT_H

#include <string>

#include "sdp.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief opp sdp client
 * opp sdp client
 */
class OppSdpClient {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param service
     * @return
     */
    explicit OppSdpClient(std::string address);

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~OppSdpClient();

    /**
     * @brief search sdp recode
     * @details search sdp recode
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int SdpSearch();

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
    void SdpSearchCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum);
    uint8_t FindRFCommChannel(const SdpService &serviceAry);
    uint16_t FindL2capPSM(const SdpService &serviceAry);

private:
    std::string address_;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_SDP_CLIENT_H