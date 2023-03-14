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

#include "opp_sdp_client.h"

#include "btstack.h"
#include "log.h"
#include "opp_service.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
OppSdpClient::OppSdpClient(std::string address)
{
    address_ = address;
}

OppSdpClient::~OppSdpClient()
{
}

int OppSdpClient::SdpSearch()
{
    HILOGI("[OPP SDP CLIENT]:enter");

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(address_);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid;
    classid.type = BT_UUID_16;
    classid.uuid16 = OPP_UUID16;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = 1;
    sdpUUid.uuid = &classid;

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_RANGE;
    attributeIdList.attributeIdRange.start = 0x0000;
    attributeIdList.attributeIdRange.end = 0xFFFF;

    int ret = SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, SdpSearchCallback);
    if (ret != BT_NO_ERROR) {
        HILOGE("[OPP SDP CLIENT]:SDP_ServiceSearchAttribute failed!");
    }

    return ret;
}

void OppSdpClient::SdpSearchCallback(const BtAddr *addr, const SdpService *serviceAry,
    uint16_t serviceNum, void *context)
{
    auto pThis = static_cast<OppSdpClient *>(context);
    if ((pThis == nullptr) || (addr == nullptr)) {
        HILOGE("[OPP SDP CLIENT]:OppSdpClient or addr is null.");
        return;
    }
    pThis->SdpSearchCallback_(addr, serviceAry, serviceNum);
}

void OppSdpClient::SdpSearchCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum)
{
    OppSdpInformation sdpInfo;
    for (int serviceCount = 0; serviceCount < serviceNum; ++serviceCount, ++serviceAry) {
        BtUuid *classId = serviceAry->classId;
        if (classId->uuid16 != OPP_UUID16) {
            continue;
        }
        sdpInfo.rfcommNo = FindRFCommChannel(*serviceAry);
        sdpInfo.psm = FindL2capPSM(*serviceAry);
    }

    if ((sdpInfo.psm != 0) || (sdpInfo.rfcommNo != 0)) {
        OppMessage event(OPP_SDP_CMPL_EVT, OPP_SDP_SUCCESS);
        event.dev_ = address_;
        event.sdpInfo_ = sdpInfo;
        OppService::GetService()->PostEvent(event);
    } else {
        OppMessage event(OPP_SDP_CMPL_EVT, OPP_SDP_FAILED);
        event.dev_ = address_;
        OppService::GetService()->PostEvent(event);
        HILOGE("[OPP SDP CLIENT] serviceNum = %{public}d", int(serviceNum));
    }
}

uint8_t OppSdpClient::FindRFCommChannel(const SdpService &serviceAry)
{
    uint8_t rfcommNo = 0;
    SdpProtocolDescriptor *descriptor = serviceAry.descriptor;
    for (int descriptorCount = 0; descriptorCount < serviceAry.descriptorNumber; ++descriptorCount, ++descriptor) {
        if (descriptor->protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) {
            if (&(descriptor->parameter[0]) != nullptr) {
                rfcommNo = descriptor->parameter[0].value;
                break;
            }
        }
    }
    return rfcommNo;
}

uint16_t OppSdpClient::FindL2capPSM(const SdpService &serviceAry)
{
    uint16_t psm = 0;
    SdpAttribute *attribute = serviceAry.attribute;
    for (uint16_t attSeqCount = 0; attSeqCount < serviceAry.attributeNumber; ++attSeqCount, ++attribute) {
        if (attribute->attributeId == OPP_GOEP_L2CAP_PSM_ATTRIBUTE_ID) {
            psm = *static_cast<uint16_t*>(attribute->attributeValue);
        }
    }
    return psm;
}
}  // namespace bluetooth
}  // namespace OHOS