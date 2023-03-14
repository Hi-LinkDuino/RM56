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

#include "pbap_pse_gap.h"
#include "gap_if.h"
#include "log.h"
#include "pbap_pse_def.h"

namespace OHOS {
namespace bluetooth {
PbapPseGap::PbapPseGap(uint8_t rfcommScn, uint16_t l2capPsm) : rfcommScn_(rfcommScn), l2capPsm_(l2capPsm)
{}

int PbapPseGap::Register() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int retVal = BT_NO_ERROR;
    GapSecChannel secChannel;
    secChannel.l2capPsm = l2capPsm_;
    secChannel.rfcommChannel = rfcommScn_;
#ifdef PBAP_PSE_L2CAP_ENABLE
    // register for l2cap
    GapServiceSecurityInfo securityInfoL2cap;
    securityInfoL2cap.direction = INCOMING;
    securityInfoL2cap.serviceId = GAP_Service::PBAP_SERVER;
    securityInfoL2cap.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    securityInfoL2cap.channelId = secChannel;
    retVal = GAPIF_RegisterServiceSecurity(nullptr,
        &securityInfoL2cap,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Gap GAPIF_RegisterServiceSecurity retVal = %{public}d", retVal);
        return retVal;
    }
#endif
    // register for rfcomm
    GapServiceSecurityInfo securityInfoRfcomm;
    securityInfoRfcomm.direction = INCOMING;
    securityInfoRfcomm.serviceId = GAP_Service::PBAP_SERVER;
    securityInfoRfcomm.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    securityInfoRfcomm.channelId = secChannel;
    retVal = GAPIF_RegisterServiceSecurity(nullptr,
        &securityInfoRfcomm,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Gap GAPIF_RegisterServiceSecurity retVal = %{public}d", retVal);
        return retVal;
    }
    return retVal;
}

void PbapPseGap::Deregister() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    GapSecChannel secChannel;
    secChannel.l2capPsm = l2capPsm_;
    secChannel.rfcommChannel = rfcommScn_;
    int retVal = BT_NO_ERROR;
#ifdef PBAP_PSE_L2CAP_ENABLE
    GapServiceSecurityInfo securityInfoL2cap;
    securityInfoL2cap.direction = INCOMING;
    securityInfoL2cap.serviceId = GAP_Service::PBAP_SERVER;
    securityInfoL2cap.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    securityInfoL2cap.channelId = secChannel;
    retVal = GAPIF_DeregisterServiceSecurity(nullptr, &securityInfoL2cap);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Gap GAPIF_DeregisterServiceSecurity retVal = %{public}d", retVal);
    }
#endif

    GapServiceSecurityInfo securityInfoRfcomm;
    securityInfoRfcomm.direction = INCOMING;
    securityInfoRfcomm.serviceId = GAP_Service::PBAP_SERVER;
    securityInfoRfcomm.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    securityInfoRfcomm.channelId = secChannel;
    retVal = GAPIF_DeregisterServiceSecurity(nullptr, &securityInfoRfcomm);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Gap GAPIF_DeregisterServiceSecurity retVal = %{public}d", retVal);
    }
}
}  // namespace bluetooth
}  // namespace OHOS