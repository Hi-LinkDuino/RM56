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

#include "pbap_pce_gap.h"
#include <cstring>
#include "pbap_pce_header_msg.h"
#include "pbap_pce_service.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
PbapPceGap::PbapPceGap(
    const IProfilePbapPce &service, const RawAddress &device, GapSecChannel channel, bool rfcommOrPsm)
    : service_(service), device_(device), gapSecChannel_(channel), l2capOrRfcomm_(rfcommOrPsm)
{}

PbapPceGap::~PbapPceGap()
{
    Deregister();
}

int PbapPceGap::Register()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    BtAddr btAddr;
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    device_.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapServiceSecurityInfo securityInfo;
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::PBAP_CLIENT;
    securityInfo.channelId.l2capPsm = 0;
    securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    if (l2capOrRfcomm_) {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
        securityInfo.channelId.l2capPsm = gapSecChannel_.l2capPsm;
        PBAP_PCE_LOG_DEBUG("Register L2cap psm 0x%04X", gapSecChannel_.l2capPsm);
    } else {
        securityInfo.channelId.rfcommChannel = gapSecChannel_.rfcommChannel;
        PBAP_PCE_LOG_DEBUG("Register Rfcomm psm 0x%02X", gapSecChannel_.rfcommChannel);
    }
    registered_ = true;
    int ret = GAPIF_RegisterServiceSecurity(&btAddr,
        &securityInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    PBAP_PCE_LOG_INFO("%{public}s end, GAPIF_RegisterServiceSecurity=%{public}d", __PRETTY_FUNCTION__, ret);
    return ret;
}

int PbapPceGap::Deregister()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    if (!registered_) {
        return BT_NO_ERROR;
    }

    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    device_.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapServiceSecurityInfo securityInfo;
    (void)memset_s(&securityInfo, sizeof(securityInfo), 0, sizeof(securityInfo));
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::PBAP_CLIENT;

    if (l2capOrRfcomm_) {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    } else {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    }
    securityInfo.channelId = gapSecChannel_;
    registered_ = false;

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return GAPIF_DeregisterServiceSecurity(&btAddr, &securityInfo);
}

int PbapPceGap::RequestSecurity()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (!l2capOrRfcomm_) {
        utility::Message msg(PCE_GAP_FINISH);
        ((PbapPceService &)service_).PostMessage(device_, msg);
        PBAP_PCE_LOG_INFO("%{public}s end, rfcomm", __PRETTY_FUNCTION__);
        return RET_NO_ERROR;
    }

    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    device_.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapRequestSecurityParam requestParam;
    (void)memset_s(&requestParam, sizeof(requestParam), 0, sizeof(requestParam));
    GapServiceSecurityInfo &securityInfo = requestParam.info;
    (void)memset_s(&securityInfo, sizeof(securityInfo), 0, sizeof(securityInfo));
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::PBAP_CLIENT;
    securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    securityInfo.channelId = gapSecChannel_;
    requestParam.callback = &PbapPceGap::RequestSecurityCallback;
    requestParam.context = this;
    int ret = GAPIF_RequestSecurity(&btAddr, &requestParam);
    PBAP_PCE_LOG_INFO("%{public}s end, GAPIF_RequestSecurity=%{public}d", __PRETTY_FUNCTION__, ret);
    return ret;
}

void PbapPceGap::RequestSecurityCallback(uint16_t result) const
{
    // RequestSecurity callback
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (result == GAP_STATUS_SUCCESS) {
        utility::Message msg(PCE_GAP_FINISH);
        ((PbapPceService &)service_).PostMessage(device_, msg);
    } else {
        utility::Message msg(PCE_GAP_FAILED);
        ((PbapPceService &)service_).PostMessage(device_, msg);
        PBAP_PCE_LOG_ERROR("%{public}s gap request security failed.", __PRETTY_FUNCTION__);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceGap::RequestSecurityCallback(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    PBAP_PCE_LOG_INFO("%{public}s start, result = %{public}d", __PRETTY_FUNCTION__, int(result));
    auto pThis = static_cast<PbapPceGap *>(context);
    if (pThis != nullptr) {
        pThis->RequestSecurityCallback(result);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}
}  // namespace bluetooth
}  // namespace OHOS