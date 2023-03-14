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

#include "pbap_pce_sdp.h"
#include <cstring>
#include <memory>
#include "btm.h"
#include "btstack.h"
#include "message.h"
#include "pbap_pce_header_msg.h"
#include "pbap_pce_service.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
PbapPceSdp::PbapPceSdp(const IProfilePbapPce &service) : service_(service)
{}

PbapPceSdp::~PbapPceSdp()
{}

int PbapPceSdp::Register()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    sdpHandle_ = SDP_CreateServiceRecord();
    std::unique_ptr<BtUuid> classId = std::make_unique<BtUuid>();
    if (classId == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, classId is nullptr", __PRETTY_FUNCTION__);
        return BT_NO_MEMORY;
    }
    classId->type = BT_UUID_16;
    classId->uuid16 = PBAP_PCE_UUID16;
    /// create ServiceClass
    int retVal = SDP_AddServiceClassIdList(sdpHandle_, classId.get(), 1);
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("Call SDP_AddServiceClassIdList Error");
    }

    /// create Service Name
    retVal = SDP_AddServiceName(
        sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, PCE_SERVICE_NAME.c_str(), PCE_SERVICE_NAME.length());
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("Call SDP_AddServiceName Error");
    }

    std::unique_ptr<SdpProfileDescriptor> profileDescriptor = std::make_unique<SdpProfileDescriptor>();
    profileDescriptor->profileUuid.type = BT_UUID_16;
    profileDescriptor->profileUuid.uuid16 = PBAP_PCE_PROFILE_UUID16;
    profileDescriptor->versionNumber = PBAP_PCE_VERSION_NUMBER;
    /// create Bluetooth Profile Descriptor List
    retVal = SDP_AddBluetoothProfileDescriptorList(sdpHandle_, profileDescriptor.get(), 1);
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("Call SDP_AddBluetoothProfileDescriptorList Error");
    }

    retVal = SDP_RegisterServiceRecord(sdpHandle_);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retVal;
}

int PbapPceSdp::Deregister() const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    uint32_t retVal = BT_NO_ERROR;
    retVal |= SDP_DeregisterServiceRecord(sdpHandle_);
    retVal |= SDP_DestroyServiceRecord(sdpHandle_);
    PBAP_PCE_LOG_INFO("%{public}s end, retVal = %u", __PRETTY_FUNCTION__, retVal);
    return retVal;
}

int PbapPceSdp::SdpSearch(const RawAddress &device)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int retVal;

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    HILOGI("pbap client connect to [%{public}s]", GET_ENCRYPT_ADDR(device));
    device.ConvertToUint8(address.addr, sizeof(address.addr));

    BtUuid classId;
    classId.type = BT_UUID_16;
    classId.uuid16 = PBAP_PSE_UUID16;
    SdpUuid sdpUuid;
    sdpUuid.uuidNum = 1;
    sdpUuid.uuid = &classId;
    const int attrIdNum = 6;
    enum {
        SDP_ATTRIBUTE_ID0 = 0,
        SDP_ATTRIBUTE_ID1,
        SDP_ATTRIBUTE_ID2,
        SDP_ATTRIBUTE_ID3,
        SDP_ATTRIBUTE_ID4,
        SDP_ATTRIBUTE_ID5,
    };
    SdpAttributeIdList attrIDs;
    attrIDs.type = SDP_TYPE_LIST;
    attrIDs.attributeIdList.attributeIdNumber = attrIdNum;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID0] = SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID1] = SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID2] = SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID3] = PBAP_GOEP_L2CAP_PSM_ATTRIBUTE_ID;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID4] = PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID;
    attrIDs.attributeIdList.attributeId[SDP_ATTRIBUTE_ID5] = PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID;
    retVal = SDP_ServiceSearchAttribute(&address, &sdpUuid, attrIDs, this, PbapPceSdp::SdpSearchCallback);
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("pce client Call SDP_ServiceSearchAttribute Error");
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return retVal;
}

uint8_t PbapPceSdp::FindRFCommChannel(const SdpService &serviceAry, bool &isGoepL2capPSM)
{
    /// find rfcomm channel
    uint8_t rfcommNo = 0;
    SdpProtocolDescriptor *descriptor = serviceAry.descriptor;
    for (int descriptorCount = 0; descriptorCount < serviceAry.descriptorNumber; ++descriptorCount, ++descriptor) {
        if (descriptor->protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) {
            if (&(descriptor->parameter[0]) != nullptr) {
                rfcommNo = descriptor->parameter[0].value;
                isGoepL2capPSM = false;
                break;
            }
        }
    }
    return rfcommNo;
}

uint16_t PbapPceSdp::FindVersionNumber(const SdpService &serviceAry)
{
    uint16_t versionNumber = 0;
    SdpProfileDescriptor *profileDescriptor = serviceAry.profileDescriptor;
    for (int profileCnt = 0; profileCnt < serviceAry.profileDescriptorNumber; ++profileCnt, ++profileDescriptor) {
        if (profileDescriptor->profileUuid.uuid16 == PBAP_PCE_PROFILE_UUID16) {
            versionNumber = profileDescriptor->versionNumber;
            break;
        }
    }
    return versionNumber;
}

bool PbapPceSdp::FindL2capPSM(const SdpService &serviceAry, uint16_t &psm, uint8_t &supportedRes,
    uint32_t &supportedFeature, bool &featureFlag)
{
    SdpAttribute *attribute = serviceAry.attribute;
    bool isGoepL2capPSM = false;
    PBAP_PCE_LOG_INFO("attribute num:[%{public}d]", int(serviceAry.attributeNumber));
    for (uint16_t attSeqCount = 0; attSeqCount < serviceAry.attributeNumber; ++attSeqCount, ++attribute) {
        PBAP_PCE_LOG_INFO("attributeId:[%#x]", attribute->attributeId);

        if (attribute->attributeId == PBAP_GOEP_L2CAP_PSM_ATTRIBUTE_ID) {
            psm = *(uint16_t *)attribute->attributeValue;
#ifdef PBAP_PCE_ENABLE_L2CAP_PSM
            isGoepL2capPSM = true;
#else
            isGoepL2capPSM = false;
#endif
        } else if (attribute->attributeId == PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID) {
            supportedRes = *(uint8_t *)attribute->attributeValue;
        } else if (attribute->attributeId == PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID) {
            featureFlag = true;
            supportedFeature = *(uint32_t *)attribute->attributeValue;
        }
    }
    return isGoepL2capPSM;
}

void PbapPceSdp::SdpSearchCallback(const BtAddr *addr, const SdpService &serviceArray, uint16_t serviceNum) const
{
    uint16_t psm = 0;
    uint8_t rfcommNo = 0;
    uint16_t versionNumber = 0;
    uint8_t supportedRes = 0;
    uint32_t supportedFeature = 0;
    bool featureFlag = false;
    ObexClientConfig obexConfig;  // PceConnectingState response for free this
    RawAddress device = RawAddress::ConvertToString(addr->addr);
    device.ConvertToUint8(obexConfig.addr_.addr, sizeof(obexConfig.addr_.addr));
    obexConfig.addr_.type = addr->type;
#ifdef PBAP_PCE_ENABLE_L2CAP_PSM
    obexConfig.lpsm_ = PBAP_PCE_LOCAL_GOEP_L2CAP_PSM;
#endif
    SdpService *serviceAry = const_cast<SdpService *>(&serviceArray);
    for (int serviceCount = 0; serviceCount < serviceNum; ++serviceCount, ++serviceAry) {
        BtUuid *classId = serviceAry->classId;
        if (classId->uuid16 != PBAP_PSE_UUID16) {
            continue;  // is not pbap service
        }
        rfcommNo = FindRFCommChannel(*serviceAry, obexConfig.isGoepL2capPSM_);
        versionNumber = FindVersionNumber(*serviceAry);
        obexConfig.isGoepL2capPSM_ = FindL2capPSM(*serviceAry, psm, supportedRes, supportedFeature, featureFlag);
    }
    if (!featureFlag) {
        supportedFeature = PBAP_PCE_SUPPORTED_FEATURES;
    }
    if ((versionNumber != 0) && (supportedRes != 0) && (supportedFeature != 0) &&
        (psm != 0 || rfcommNo != 0)) {  // sdp search success
        if (obexConfig.isGoepL2capPSM_) {
            obexConfig.scn_ = psm;
            obexConfig.isSupportSrm_ = true;
            PBAP_PCE_LOG_INFO("connect with psm:[%{public}d]", int(psm));
            BTM_AddRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, psm, &obexConfig.addr_);
        } else {
            obexConfig.scn_ = rfcommNo;
            PBAP_PCE_LOG_INFO("connect with rfcommNo:[%u]", rfcommNo);
            BTM_AddRemoteRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP, rfcommNo, &obexConfig.addr_);
        }
        auto sdpMsg = std::make_unique<PbapPceHeaderSdpMsg>(
            obexConfig, versionNumber, supportedRes, supportedFeature, featureFlag);
        utility::Message msg(PCE_SDP_FINISH, 0, static_cast<void *>(sdpMsg.release()));
        ((PbapPceService &)service_).PostMessage(device, msg);
    } else {  // sdp search failure
        utility::Message msg(PCE_SDP_FAILED);
        ((PbapPceService &)service_).PostMessage(device, msg);
        PBAP_PCE_LOG_ERROR("%{public}s sdp failed", __PRETTY_FUNCTION__);
    }
}

void PbapPceSdp::SdpSearchCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    auto pThis = static_cast<PbapPceSdp *>(context);
    if ((pThis == nullptr) || (addr == nullptr)) {
        PBAP_PCE_LOG_ERROR("%{public}s PbapPceSdp or addr is null.", __PRETTY_FUNCTION__);
        return;
    }

    if ((serviceAry == nullptr) || (serviceNum == 0) ) {
        RawAddress device = RawAddress::ConvertToString(addr->addr);
        utility::Message msg(PCE_SDP_FAILED);
        ((PbapPceService &)(pThis->service_)).PostMessage(device, msg);
        PBAP_PCE_LOG_ERROR("%{public}s serviceNum = %{public}d", __PRETTY_FUNCTION__, int(serviceNum));
        return;
    }

    pThis->SdpSearchCallback(addr, *serviceAry, serviceNum);
}
}  // namespace bluetooth
}  // namespace OHOS