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

#include "hfp_ag_sdp_server.h"

#include "adapter_config.h"
#include "hfp_ag_data_connection.h"
#include "hfp_ag_defines.h"
#include "raw_address.h"
#include "rfcomm.h"

namespace OHOS {
namespace bluetooth {
HfpAgSdpServer &HfpAgSdpServer::GetInstance()
{
    static HfpAgSdpServer instance;
    return instance;
}

int HfpAgSdpServer::RegisterSdpService(uint8_t scn)
{
    if (scn == 0) {
        LOG_ERROR("[HFP AG]%{public}s():Unavailable server channel number", __FUNCTION__);
        return HFP_AG_FAILURE;
    }

    AdapterConfig::GetInstance()->GetValue(HSP_AG_STATE_SECTION_NAME, HSP_AG_STATE_PROPERY_NAME, hspState_);
    LOG_INFO("[HFP AG]HSP STATE = %{public}d", hspState_);

    // Create and register service record
    sdpHandle_ = SDP_CreateServiceRecord();

    int ret = AddServiceClassId();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddProtocol(scn);
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddProfile();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddServiceName();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddNetwork();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddFeatures();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = AddBrowseGroupList();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = SDP_RegisterServiceRecord(sdpHandle_);
    HFP_AG_RETURN_IF_FAIL(ret);
    return ret;
}

int HfpAgSdpServer::DeregisterSdpService()
{
    int ret = SDP_DeregisterServiceRecord(sdpHandle_);
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = SDP_DestroyServiceRecord(sdpHandle_);
    HFP_AG_RETURN_IF_FAIL(ret);

    sdpHandle_ = 0;
    return ret;
}

int HfpAgSdpServer::AddServiceClassId() const
{
    switch(hspState_) {
        case HSP_AG_STATE_BOTH:
            BtUuid classIdBoth[HFP_AG_HSP_SERVER_CLASSID_NUM];
            classIdBoth[0].type = BT_UUID_16;
            classIdBoth[0].uuid16 = HFP_AG_UUID_SERVCLASS_HFP_AG;
            classIdBoth[1].type = BT_UUID_16;
            classIdBoth[1].uuid16 = HFP_AG_UUID_SERVCLASS_GENERIC_AUDIO;
            classIdBoth[2].type = BT_UUID_16;
            classIdBoth[2].uuid16 = HSP_AG_UUID_SERVCLASS;
            return SDP_AddServiceClassIdList(sdpHandle_, classIdBoth, HFP_AG_HSP_SERVER_CLASSID_NUM);
        case HSP_AG_STATE_HSP:
            BtUuid classIdHsp[HFP_AG_SERVER_CLASSID_NUM];
            classIdHsp[0].type = BT_UUID_16;
            classIdHsp[0].uuid16 = HSP_AG_UUID_SERVCLASS;
            classIdHsp[1].type = BT_UUID_16;
            classIdHsp[1].uuid16 = HFP_AG_UUID_SERVCLASS_GENERIC_AUDIO;
            return SDP_AddServiceClassIdList(sdpHandle_, classIdHsp, HFP_AG_SERVER_CLASSID_NUM);
        case HSP_AG_STATE_NONE:
            BtUuid classIdHfp[HFP_AG_SERVER_CLASSID_NUM];
            classIdHfp[0].type = BT_UUID_16;
            classIdHfp[0].uuid16 = HFP_AG_UUID_SERVCLASS_HFP_AG;
            classIdHfp[1].type = BT_UUID_16;
            classIdHfp[1].uuid16 = HFP_AG_UUID_SERVCLASS_GENERIC_AUDIO;
            return SDP_AddServiceClassIdList(sdpHandle_, classIdHfp, HFP_AG_SERVER_CLASSID_NUM);
        default:
            break;
    }
    return BT_CONFIG_ERROR;
}

int HfpAgSdpServer::AddProtocol(uint8_t scn) const
{
    SdpProtocolDescriptor protocol[HFP_AG_SERVER_PROTOCOL_NUM];
    protocol[0].protocolUuid.type = BT_UUID_16;
    protocol[0].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    protocol[0].parameterNumber = 0;
    protocol[1].protocolUuid.type = BT_UUID_16;
    protocol[1].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    protocol[1].parameterNumber = 1;
    protocol[1].parameter[0].type = SDP_TYPE_UINT_8;
    protocol[1].parameter[0].value = scn;
    return SDP_AddProtocolDescriptorList(sdpHandle_, protocol, HFP_AG_SERVER_PROTOCOL_NUM);
}

int HfpAgSdpServer::AddProfile() const
{
    switch(hspState_) {
        case HSP_AG_STATE_BOTH:
            SdpProfileDescriptor profileBoth[HFP_AG_HSP_SERVER_PROFILE_NUM];
            profileBoth[0].profileUuid.type = BT_UUID_16;
            profileBoth[0].profileUuid.uuid16 = HFP_AG_UUID_SERVCLASS_HFP_AG;
            profileBoth[0].versionNumber = HFP_AG_HFP_VERSION_1_7;
            profileBoth[1].profileUuid.type = BT_UUID_16;
            profileBoth[1].profileUuid.uuid16 = HSP_AG_UUID_SERVCLASS;
            profileBoth[1].versionNumber = HSP_AG_HSP_VERSION_1_2;
            return SDP_AddBluetoothProfileDescriptorList(sdpHandle_, profileBoth, HFP_AG_HSP_SERVER_PROFILE_NUM);
        case HSP_AG_STATE_HSP:
            SdpProfileDescriptor profileHsp[HFP_AG_SERVER_PROFILE_NUM];
            profileHsp[0].profileUuid.type = BT_UUID_16;
            profileHsp[0].profileUuid.uuid16 = HSP_AG_UUID_SERVCLASS;
            profileHsp[0].versionNumber = HSP_AG_HSP_VERSION_1_2;
            return SDP_AddBluetoothProfileDescriptorList(sdpHandle_, profileHsp, HFP_AG_SERVER_PROFILE_NUM);
        case HSP_AG_STATE_NONE:
            SdpProfileDescriptor profileHfp[HFP_AG_SERVER_PROFILE_NUM];
            profileHfp[0].profileUuid.type = BT_UUID_16;
            profileHfp[0].profileUuid.uuid16 = HFP_AG_UUID_SERVCLASS_HFP_AG;
            profileHfp[0].versionNumber = HFP_AG_HFP_VERSION_1_7;
            return SDP_AddBluetoothProfileDescriptorList(sdpHandle_, profileHfp, HFP_AG_SERVER_PROFILE_NUM);
        default:
            break;
    }
    return BT_CONFIG_ERROR;
}

int HfpAgSdpServer::AddServiceName() const
{
    return SDP_AddServiceName(sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE,
        HFP_AG_SERVER_SERVICE_NAME.c_str(), HFP_AG_SERVER_SERVICE_NAME.length());
}

int HfpAgSdpServer::AddNetwork() const
{
    uint32_t features = HfpAgDataConnection::GetLocalFeatures();
    uint8_t networkValue = (features & HFP_AG_FEATURES_REJECT_CALL) ? 1 : 0;
    SdpAttribute network;
    network.attributeId = HFP_AG_SDP_ATTRIBUTE_DATA_STORES_OR_NETWORK;
    network.type = SDP_TYPE_UINT_8;
    network.attributeValueLength = HFP_AG_SERVER_NETWORK_LENGTH;
    network.attributeValue = &networkValue;
    return AddAttribute(network);
}

int HfpAgSdpServer::AddFeatures() const
{
    uint32_t features = HfpAgDataConnection::GetLocalFeatures();
    bool codecMSBC = (features & HFP_AG_FEATURES_CODEC_NEGOTIATION) ? true : false;
    features &= HFP_AG_FEATURES_SDP_SPEC;
    if (codecMSBC) {
        // Codec bit position is different in SDP (bit 5) and in BRSF (bit 7)
        features |= HFP_AG_HF_FEATURES_SUPPORT_WBS;
    }
    SdpAttribute featuresAttr;
    featuresAttr.attributeId = HFP_AG_SDP_ATTRIBUTE_SUPPORTED_FEATURES;
    featuresAttr.type = SDP_TYPE_UINT_16;
    featuresAttr.attributeValueLength = HFP_AG_SERVER_FEATURES_LENGTH;
    featuresAttr.attributeValue = &features;
    return AddAttribute(featuresAttr);
}

int HfpAgSdpServer::AddBrowseGroupList() const
{
    BtUuid browseGroupList[HFP_AG_SERVER_BROWSE_LIST_NUM];
    browseGroupList[0].type = BT_UUID_16;
    browseGroupList[0].uuid16 = SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID;
    return SDP_AddBrowseGroupList(sdpHandle_, browseGroupList, HFP_AG_SERVER_BROWSE_LIST_NUM);
}

int HfpAgSdpServer::AddAttribute(const SdpAttribute &attribute) const
{
    return SDP_AddAttribute(
        sdpHandle_, attribute.attributeId, attribute.type, attribute.attributeValue, attribute.attributeValueLength);
}
}  // namespace bluetooth
}  // namespace OHOS