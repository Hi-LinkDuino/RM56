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

#include "opp_gap_client.h"
#include "btstack.h"
#include "log.h"
#include "opp_defines.h"
#include "opp_message.h"
#include "opp_service.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
OppGapClient::OppGapClient(
    const std::string &address, GapSecChannel channel, bool rfcommOrPsm)
    : address_(address), gapSecChannel_(channel), l2capOrRfcomm_(rfcommOrPsm)
{}

OppGapClient::~OppGapClient()
{
    Deregister();
}

int OppGapClient::Register()
{
    HILOGI("[OPP GAP CLIENT]start");

    BtAddr btAddr;
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(address_);
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapServiceSecurityInfo securityInfo;
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::OPP_CLIENT;
    securityInfo.channelId.l2capPsm = 0;
    securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    if (l2capOrRfcomm_) {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
        securityInfo.channelId.l2capPsm = gapSecChannel_.l2capPsm;
        HILOGI("[OPP GAP CLIENT]Register L2cap psm 0x%04X", gapSecChannel_.l2capPsm);
    } else {
        securityInfo.channelId.rfcommChannel = gapSecChannel_.rfcommChannel;
        HILOGI("[OPP GAP CLIENT]Register Rfcomm psm 0x%02X", gapSecChannel_.rfcommChannel);
    }
    registered_ = true;
    int ret = GAPIF_RegisterServiceSecurity(&btAddr,
        &securityInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION);
    HILOGI("[OPP GAP CLIENT]end, GAPIF_RegisterServiceSecurity=%{public}d", ret);
    return ret;
}

int OppGapClient::Deregister()
{
    HILOGI("[OPP GAP CLIENT]start");

    if (!registered_) {
        return BT_NO_ERROR;
    }

    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    RawAddress rawAddr(address_);
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapServiceSecurityInfo securityInfo;
    (void)memset_s(&securityInfo, sizeof(securityInfo), 0, sizeof(securityInfo));
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::OPP_CLIENT;

    if (l2capOrRfcomm_) {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    } else {
        securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    }
    securityInfo.channelId = gapSecChannel_;
    registered_ = false;

    HILOGI("[OPP GAP CLIENT]end");
    return GAPIF_DeregisterServiceSecurity(&btAddr, &securityInfo);
}

int OppGapClient::RequestSecurity()
{
    HILOGI("[OPP GAP CLIENT]start");
    if (!l2capOrRfcomm_) {
        OppMessage event(OPP_GAP_CMPL_EVT, OPP_GAP_SUCCESS);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
        HILOGI("[OPP GAP CLIENT]end, rfcomm");
        return RET_NO_ERROR;
    }

    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    RawAddress rawAddr(address_);
    rawAddr.ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));

    GapRequestSecurityParam requestParam;
    (void)memset_s(&requestParam, sizeof(requestParam), 0, sizeof(requestParam));
    GapServiceSecurityInfo &securityInfo = requestParam.info;
    (void)memset_s(&securityInfo, sizeof(securityInfo), 0, sizeof(securityInfo));
    securityInfo.direction = OUTGOING;
    securityInfo.serviceId = GAP_Service::OPP_CLIENT;
    securityInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP;
    securityInfo.channelId = gapSecChannel_;
    requestParam.callback = &OppGapClient::RequestSecurityCallback;
    requestParam.context = this;
    int ret = GAPIF_RequestSecurity(&btAddr, &requestParam);
    HILOGI("[OPP GAP CLIENT]end, GAPIF_RequestSecurity=%{public}d", ret);
    return ret;
}

void OppGapClient::RequestSecurityCallback(uint16_t result) const
{
    // RequestSecurity callback
    HILOGI("[OPP GAP CLIENT]start");
    if (result == GAP_STATUS_SUCCESS) {
        OppMessage event(OPP_GAP_CMPL_EVT, OPP_GAP_SUCCESS);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
    } else {
        OppMessage event(OPP_GAP_CMPL_EVT, OPP_GAP_FAILED);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
        HILOGE("[OPP GAP CLIENT]gap request security failed.");
    }
    HILOGI("[OPP GAP CLIENT]end");
}

void OppGapClient::RequestSecurityCallback(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    HILOGI("[OPP GAP CLIENT]start, result = %{public}d", int(result));
    auto pThis = static_cast<OppGapClient *>(context);
    if (pThis != nullptr) {
        pThis->RequestSecurityCallback(result);
    }
    HILOGI("[OPP GAP CLIENT]end");
}
}  // namespace bluetooth
}  // namespace OHOS