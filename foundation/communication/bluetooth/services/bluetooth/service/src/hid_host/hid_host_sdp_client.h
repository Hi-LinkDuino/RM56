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

#ifndef HID_HOST_SDP_CLIENT_H
#define HID_HOST_SDP_CLIENT_H

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "base_def.h"
#include "btstack.h"
#include "hid_host_defines.h"
#include "sdp.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for discovery remote H device's SDP server and finding out
 *        related SDP info required by HFP HF role.
 */
class HidHostSdpClient {
public:
    /**
     * @brief Construct a new HidHostSdpClient object.
     */
    explicit HidHostSdpClient(std::string address);

    /**
     * @brief Destroy the HidHostSdpClient object.
     */
    ~HidHostSdpClient();

    /**
     * @brief Callback function of SDP discovery.
     *
     * @param addr Remote device address defined bt stack.
     * @param serviceAry Array of services discovered.
     * @param serviceNum Number of services discovered.
     * @param context Upper layer context.
     */
    static void SdpCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context);
    static void SdpPnpCallback(const BtAddr *addr,
        const SdpService *serviceAry, uint16_t serviceNum, void *context);

    /**
     * @brief Start a service discovery job.
     *
     * @param remoteAddr Remote device address.
     * @return Returns the error code of the discovery result.
     */
    int DoDiscovery(const std::string &remoteAddr);

    /**
     * @brief Get the Remote Sdp Pnp Info.
     *
     * @return Returns the PnpInformation.
     */
    PnpInformation& GetRemoteSdpPnpInfo();

    /**
     * @brief Get the Remote Sdp hid Info.
     *
     * @return Returns the HidInformation.
     */
    HidInformation& GetRemoteSdpHidInfo();

    bool CheckIsSdpDone();

private:
    int DoPnpDiscovery(const std::string &remoteAddr);
    int DoHidDiscovery(const std::string &remoteAddr);
    void SdpCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum);
    void SdpPnpCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum);
    void SendSdpComplete(int result);
    bool ParseHidDescInfo(const SdpService *serviceAry);
    uint8_t CheckAttributeValueLengthAvalid(SdpSequenceAttribute attribute);
    void printHidSdpInfo();
    // Current remote device address
    std::string currentAddr_ {""};

    PnpInformation pnpInf_ {};
    HidInformation hidInf_ {};
    bool isSdpDone_ = false;
    bool isPnpSdpDone_ = false;

    BT_DISALLOW_COPY_AND_ASSIGN(HidHostSdpClient);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HID_HOST_SDP_CLIENT_H
