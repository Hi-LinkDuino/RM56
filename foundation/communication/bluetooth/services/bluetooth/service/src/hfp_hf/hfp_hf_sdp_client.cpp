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

#include <cstring>

#include "adapter_config.h"
#include "hfp_hf_profile_event_sender.h"
#include "hfp_hf_sdp_client.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
std::map<std::string, HfpHfRemoteSdpServiceArray> HfpHfSdpClient::g_remoteSdpServiceArrays;
std::recursive_mutex HfpHfSdpClient::g_hfpSdpMutex;
int agProfileState_ = -1;

HfpHfSdpClient::~HfpHfSdpClient()
{
    std::lock_guard<std::recursive_mutex> lk(g_hfpSdpMutex);
    auto it = g_remoteSdpServiceArrays.find(currentAddr_);
    if (it != g_remoteSdpServiceArrays.end()) {
        DeleteSdpServiceArray(it->second);
        g_remoteSdpServiceArrays.erase(it);
    }
}

void HfpHfSdpClient::SdpCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    int msgWhat = HFP_HF_SDP_DISCOVERY_RESULT_FAIL;
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    if (serviceNum > 0) {
        CopySdpServiceArray(address, serviceAry, serviceNum);
        msgWhat = HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS;
        agProfileState_ = HFP_HF_HFAG_FOUND;
    }
    int hspState = 1;
    AdapterConfig::GetInstance()->GetValue(HSP_HS_STATE_SECTION_NAME, HSP_HS_STATE_PROPERY_NAME, hspState);
    if (hspState == HSP_HS_STATE_BOTH) {
        HfpHfSdpClient *sdpClient = static_cast<HfpHfSdpClient *>(context);
        HfpHfService::GetService()->GetDispatcher()->PostTask(
            std::bind(&HfpHfSdpClient::DoHspAgDiscovery, sdpClient, address));
        return;
    }
    HfpHfProfileEventSender::GetInstance().ProcessSdpDiscoveryResult(address, msgWhat);
}

int HfpHfSdpClient::DoDiscovery(const std::string &remoteAddr, int role)
{
    agProfileState_ = -1;
    AdapterConfig::GetInstance()->GetValue(HSP_HS_STATE_SECTION_NAME, HSP_HS_STATE_PROPERY_NAME, hspState_);
    if (hspState_ == HSP_HS_STATE_HSP) {
        LOG_INFO("[HFP HF] start hsp ag dicovery");
        return DoHspAgDiscovery(remoteAddr);
    }

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(remoteAddr);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid[HFP_HF_CLIENT_CLASSID_NUM];
    classid[0].type = BT_UUID_16;
    classid[0].uuid16 = HFP_HF_UUID_SERVCLASS_HFP_AG;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = HFP_HF_CLIENT_CLASSID_NUM;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;
    if (HFP_HF_INITIATOR == role) {
        attributeIdList.attributeIdList.attributeIdNumber = HFP_HF_CLIENT_INITIATOR_ATTR_NUM;
        attributeIdList.attributeIdList.attributeId[SERVICE_CLASS_ID_LIST_INDEX] =
            SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
        attributeIdList.attributeIdList.attributeId[PROTOCOL_DESCRIPTOR_LIST_INDEX] =
            SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
        attributeIdList.attributeIdList.attributeId[INITIATOR_PROFILE_DESCRIPTOR_LIST_INDEX] =
            SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
        attributeIdList.attributeIdList.attributeId[INITIATOR_SUPPORTED_FEATURES_INDEX] =
            HFP_HF_SDP_ATTRIBUTE_SUPPORTED_FEATURES;
        attributeIdList.attributeIdList.attributeId[INITIATOR_DATA_STORES_OR_NETWORK_INDEX] =
            HFP_HF_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK;
    } else {
        attributeIdList.attributeIdList.attributeIdNumber = HFP_HF_CLIENT_ACCEPTOR_ATTR_NUM;
        attributeIdList.attributeIdList.attributeId[SERVICE_CLASS_ID_LIST_INDEX] =
            SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
        attributeIdList.attributeIdList.attributeId[ACCEPTER_PROFILE_DESCRIPTOR_LIST_INDEX] =
            SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
        attributeIdList.attributeIdList.attributeId[ACCEPTER_SUPPORTED_FEATURES_INDEX] =
            HFP_HF_SDP_ATTRIBUTE_SUPPORTED_FEATURES;
        attributeIdList.attributeIdList.attributeId[ACCEPTER_DATA_STORES_OR_NETWORK_INDEX] =
            HFP_HF_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK;
    }

    int ret = SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, &HfpHfSdpClient::SdpCallback);
    HFP_HF_RETURN_IF_FAIL(ret);
    currentAddr_ = remoteAddr;
    return ret;
}

void HfpHfSdpClient::SdpHspCallback(const BtAddr *addr, const SdpService *serviceAry,
                                    uint16_t serviceNum, void *context)
{
    int msgWhat = HFP_HF_SDP_DISCOVERY_RESULT_FAIL;
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    if (serviceNum > 0) {
        CopySdpServiceArray(address, serviceAry, serviceNum);
        msgWhat = HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS;
        if (agProfileState_ == HFP_HF_HFAG_FOUND) {
            agProfileState_ = HFP_HF_HFAG_HSAG_FOUND;
        } else {
            agProfileState_ = HFP_HF_HSAG_FOUND;
        }
    }
    if (agProfileState_ != -1) {
        msgWhat = HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS;
        HfpHfProfileEventSender::GetInstance().ProcessSdpDiscoveryResult(address, msgWhat);
    }
}

int HfpHfSdpClient::DoHspAgDiscovery(const std::string &remoteAddr)
{
    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(remoteAddr);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid[HFP_HF_CLIENT_CLASSID_NUM];
    classid[0].type = BT_UUID_16;
    classid[0].uuid16 = HFP_HF_UUID_SERVCLASS_HSP_AG;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = HFP_HF_CLIENT_CLASSID_NUM;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;
    attributeIdList.attributeIdList.attributeIdNumber = HFP_HF_CLIENT_INITIATOR_ATTR_NUM;
    attributeIdList.attributeIdList.attributeId[SERVICE_CLASS_ID_LIST_INDEX] =
        SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
    attributeIdList.attributeIdList.attributeId[PROTOCOL_DESCRIPTOR_LIST_INDEX] =
        SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[INITIATOR_PROFILE_DESCRIPTOR_LIST_INDEX] =
        SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[INITIATOR_SUPPORTED_FEATURES_INDEX] =
        HFP_HF_SDP_ATTRIBUTE_SUPPORTED_FEATURES;

    int ret = SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, &HfpHfSdpClient::SdpHspCallback);
    HFP_HF_RETURN_IF_FAIL(ret);
    currentAddr_ = remoteAddr;
    return ret;
}

bool HfpHfSdpClient::FindAttributes(const std::string &remoteAddr, int role)
{
    std::lock_guard<std::recursive_mutex> lk(g_hfpSdpMutex);
    auto it = g_remoteSdpServiceArrays.find(remoteAddr);
    if (it == g_remoteSdpServiceArrays.end()) {
        LOG_ERROR("[HFP HF]%{public}s():Not found the attributes", __FUNCTION__);
        return false;
    }

    HfpHfRemoteSdpInfo info;
    uint16_t num = 0;
    if (HFP_HF_INITIATOR == role) {
        if (!LoopAllProtocolRfcomm(num, it->second, info.remoteServerChannelNumber)) {
            LOG_ERROR("[HFP HF]%{public}s():Not found peer rfcomm scn", __FUNCTION__);
            return false;
        }
    }

    if (!FindProfileVersion(it->second.services[num].profileDescriptors, info.remoteVersion)) {
        info.remoteVersion = HFP_HF_HFP_VERSION_1_1;
        LOG_DEBUG("[HFP HF]%{public}s():Not found peer HFP version, using default version[1.1]", __FUNCTION__);
    }

    if (!FindProfileFeatures(it->second.services[num].attributes, info.remoteFeatures)) {
        info.remoteFeatures = HFP_HF_AG_FEATURES_NONE;
        LOG_DEBUG("[HFP HF]%{public}s():Not found peer HFP features, using default features", __FUNCTION__);
    }
    SelectProfileFeatures(info.remoteFeatures, info.remoteCodec);

    if (!FindProfileNetwork(it->second.services[num].attributes, info.remoteNetwork)) {
        info.remoteNetwork = HFP_HF_NETWORK_VALUE_NONE;
        LOG_DEBUG("[HFP HF]%{public}s():Not found peer network", __FUNCTION__);
    } else {
        if (info.remoteNetwork == HFP_HF_NETWORK_VALUE_HAVE) {
            info.remoteFeatures |= HFP_HF_AG_FEATURES_REJECT_CALL;
        }
    }

    remoteSdpInfo_ = info;
    DeleteSdpServiceArray(it->second);
    g_remoteSdpServiceArrays.erase(it);
    return true;
}

HfpHfRemoteSdpInfo HfpHfSdpClient::GetRemoteSdpInfo() const
{
    return remoteSdpInfo_;
}

void HfpHfSdpClient::CopySdpServiceArray(
    const std::string &remoteAddr, const SdpService *serviceAry, uint16_t serviceNum)
{
    std::lock_guard<std::recursive_mutex> lk(g_hfpSdpMutex);
    HfpHfRemoteSdpServiceArray array;
    for (uint16_t n = 0; n < serviceNum; n++) {
        HfpHfRemoteSdpService service;
        for (uint16_t i = 0; i < serviceAry[n].descriptorNumber; i++) {
            SdpProtocolDescriptor descriptor = serviceAry[n].descriptor[i];
            service.descriptors.push_back(descriptor);
        }
        for (uint16_t j = 0; j < serviceAry[n].profileDescriptorNumber; j++) {
            SdpProfileDescriptor profileDescriptor = serviceAry[n].profileDescriptor[j];
            service.profileDescriptors.push_back(profileDescriptor);
        }
        for (uint16_t k = 0; k < serviceAry[n].attributeNumber; k++) {
            HfpHfSdpAttribute attribute;
            attribute.attributeId = serviceAry[n].attribute[k].attributeId;
            attribute.type = serviceAry[n].attribute[k].type;
            uint16_t length = serviceAry[n].attribute[k].attributeValueLength;
            if (length == ATTRIBUTE_LENGTH_UINT16) {
                attribute.attributeValue = *static_cast<uint16_t*>(serviceAry[n].attribute[k].attributeValue);
            } else if (length == ATTRIBUTE_LENGTH_UINT8) {
                attribute.attributeValue = *static_cast<uint8_t*>(serviceAry[n].attribute[k].attributeValue);
            } else {
                LOG_ERROR("[HFP HF]%{public}s():Error attribute(n[%hu] k[%hu]) length[%hu]", __FUNCTION__, n, k, length);
            }
            service.attributes.push_back(attribute);
        }
        array.services.push_back(service);
    }
    g_remoteSdpServiceArrays.insert_or_assign(remoteAddr, array);
}

void HfpHfSdpClient::DeleteSdpServiceArray(HfpHfRemoteSdpServiceArray &array)
{
    if (array.services.capacity() == 0) {
        return;
    }

    for (uint16_t n = 0; n < array.services.size(); n++) {
        if (array.services[n].descriptors.capacity() != 0) {
            std::vector<SdpProtocolDescriptor>().swap(array.services[n].descriptors);
        }
        if (array.services[n].profileDescriptors.capacity() != 0) {
            std::vector<SdpProfileDescriptor>().swap(array.services[n].profileDescriptors);
        }
        if (array.services[n].attributes.capacity() != 0) {
            std::vector<HfpHfSdpAttribute>().swap(array.services[n].attributes);
        }
    }
    std::vector<HfpHfRemoteSdpService>().swap(array.services);
}

bool HfpHfSdpClient::LoopAllProtocolRfcomm(uint16_t &loopNum,
                                           const HfpHfRemoteSdpServiceArray &array,
                                           uint8_t &scn) const
{
    uint16_t serviceNum = array.services.size();
    for (uint16_t num = 0; num < serviceNum; num++) {
        loopNum = num;
        if (FindProtocolRfcomm(array.services[loopNum].descriptors, scn)) {
            break;
        }
    }

    bool ret = false;
    if (loopNum < serviceNum) {
        ret = true;
    }
    return ret;
}

bool HfpHfSdpClient::FindProtocolRfcomm(const std::vector<SdpProtocolDescriptor> &protocols, uint8_t &scn)
{
    uint16_t num = 0;
    while (num < protocols.size()) {
        if ((protocols[num].protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) &&
            (protocols[num].parameter[0].type == SDP_TYPE_UINT_8)) {
            scn = static_cast<uint8_t>(protocols[num].parameter[0].value);
            LOG_DEBUG("[HFP HF]%{public}s():Found rfcomm scn is [%hhu]", __FUNCTION__, scn);
            return true;
        }
        num++;
    }
    return false;
}

bool HfpHfSdpClient::FindProfileVersion(const std::vector<SdpProfileDescriptor> &profiles, uint16_t &version)
{
    uint16_t num = 0;
    while (num < profiles.size()) {
        if (profiles[num].profileUuid.uuid16 == HFP_HF_UUID_SERVCLASS_HFP_AG) {
            version = profiles[num].versionNumber;
            LOG_DEBUG("[HFP HF]%{public}s():Found profile version is [%hu]", __FUNCTION__, version);
            return true;
        } else if (profiles[num].profileUuid.uuid16 == HFP_HF_UUID_SERVCLASS_HSP_AG) {
            version = profiles[num].versionNumber;
            LOG_DEBUG("[HSP HF]%{public}s():Found profile version is [%hu]", __FUNCTION__, version);
            return true;
        }
        num++;
    }
    return false;
}

bool HfpHfSdpClient::FindProfileFeatures(const std::vector<HfpHfSdpAttribute> &attributes, uint16_t &features)
{
    uint16_t num = 0;
    while (num < attributes.size()) {
        if (attributes[num].attributeId == HFP_HF_SDP_ATTRIBUTE_SUPPORTED_FEATURES) {
            features = HFP_HF_AG_FEATURES_BRSF_MASK & attributes[num].attributeValue;
            LOG_DEBUG("[HFP HF]%{public}s():Found profile features are [%hu]", __FUNCTION__, features);
            return true;
        }
        num++;
    }
    return false;
}

bool HfpHfSdpClient::FindProfileNetwork(const std::vector<HfpHfSdpAttribute> &attributes, uint8_t &network)
{
    uint16_t num = 0;
    while (num < attributes.size()) {
        if (attributes[num].attributeId == HFP_HF_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK) {
            network = static_cast<uint8_t>(attributes[num].attributeValue);
            LOG_DEBUG("[HFP HF]%{public}s():Found profile features are [%hhu]", __FUNCTION__, network);
            return true;
        }
        num++;
    }
    return false;
}

void HfpHfSdpClient::SelectProfileFeatures(uint16_t &features, int &codec)
{
    if (features & HFP_HF_AG_FEATURES_SUPPORT_WBS) {
        codec = HFP_HF_CODEC_MSBC;
        // SDP and BRSF WBS bit are different, correct it if set
        features &= HFP_HF_AG_FEATURES_SUPPORT_WBS;
        features |= HFP_HF_AG_FEATURES_CODEC_NEGOTIATION;
    } else {
        codec = HFP_HF_CODEC_CVSD;
    }
}
}  // namespace bluetooth
}  // namespace OHOS