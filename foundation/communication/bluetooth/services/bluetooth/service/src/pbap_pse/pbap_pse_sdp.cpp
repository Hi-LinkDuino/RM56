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

#include "pbap_pse_sdp.h"
#include "btstack.h"
#include "log.h"
#include "pbap_pse_def.h"
#include "sdp.h"
namespace OHOS {
namespace bluetooth {
PbapPseSdp::PbapPseSdp(uint8_t rfcommScn, uint16_t l2capPsm) : rfcommScn_(rfcommScn), l2capPsm_(l2capPsm)
{}

int PbapPseSdp::Register()
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int retVal = BT_NO_ERROR;

    sdpHandle_ = SDP_CreateServiceRecord();

    retVal = AddServiceClassIdList();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddServiceClassIdList retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = AddProtocolDescriptorList();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddProtocolDescriptorList retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = AddAttributes();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddAttributes retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = AddServiceName();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddServiceName retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = AddBluetoothProfileDescriptorList();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddBluetoothProfileDescriptorList retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = AddBrowseGroupList();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddBrowseGroupList retVal = %{public}d", retVal);
        return retVal;
    }

    retVal = RegisterServiceRecord();
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp RegisterServiceRecord retVal = %{public}d", retVal);
        return retVal;
    }

    return retVal;
}

void PbapPseSdp::Deregister() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int retVal = BT_NO_ERROR;

    retVal = SDP_DeregisterServiceRecord(sdpHandle_);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp SDP_DeregisterServiceRecord retVal = %{public}d", retVal);
    }

    retVal = SDP_DestroyServiceRecord(sdpHandle_);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp SDP_DestroyServiceRecord retVal = %{public}d", retVal);
    }
}

int PbapPseSdp::AddServiceClassIdList() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    BtUuid classId[1];
    classId[0].type = BT_UUID_16;
    classId[0].uuid16 = PBAP_PSE_UUID16;

    // ServiceClassID
    int retVal = SDP_AddServiceClassIdList(sdpHandle_, classId, 1);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddServiceClassIdList retVal = %{public}d", retVal);
        return retVal;
    }
    return retVal;
}

int PbapPseSdp::AddProtocolDescriptorList() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    SdpProtocolDescriptor protocol[PABP_PROTOCOL_DESCRIPTOR_NUMBER];
    uint8_t index = 0;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    protocol[index].parameterNumber = 0;
    index++;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    protocol[index].parameterNumber = 1;
    protocol[index].parameter[0].type = SDP_TYPE_UINT_8;
    protocol[index].parameter[0].value = rfcommScn_;
    index++;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_OBEX;
    protocol[index].parameterNumber = 0;
    int retVal = SDP_AddProtocolDescriptorList(sdpHandle_, protocol, PABP_PROTOCOL_DESCRIPTOR_NUMBER);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddProtocolDescriptorList retVal = %{public}d", retVal);
    }
    return retVal;
}

int PbapPseSdp::AddAttributes()
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
#ifdef PBAP_PSE_L2CAP_ENABLE
    // GoepL2CapPsm
    int retValL2cap =
        SDP_AddAttribute(sdpHandle_, PABP_GOEP_L2CAP_PSM_ATTRIBUTE_ID, SDP_TYPE_UINT_16, &l2capPsm_, sizeof(l2capPsm_));
    if (retValL2cap != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp SDP_AddAttribute retVal = %{public}d", retValL2cap);
        return retValL2cap;
    }
    PBAP_PSE_LOG_INFO(
        "PABP_GOEP_L2CAP_PSM_ATTRIBUTE_ID:[%#X], value:[%#X]", PABP_GOEP_L2CAP_PSM_ATTRIBUTE_ID, l2capPsm_);
#endif
    uint8_t supportedRepos = PBAP_PSE_SUPPORTED_REPOSITORIES;
    // Supported Repositories
    int retVal = SDP_AddAttribute(
        sdpHandle_, PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID, SDP_TYPE_UINT_8, &supportedRepos, sizeof(supportedRepos));
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp SDP_AddAttribute retVal = %{public}d", retVal);
        return retVal;
    }
    PBAP_PSE_LOG_INFO("PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID:[%#X], value:[%#X]",
        PBAP_SUPPORTED_REPOSITORIES_ATTRIBUTE_ID,
        supportedRepos);

    // PbapSupportedFeatures
    uint32_t supportedFeatures = PBAP_PSE_SUPPORTED_FEATURES;
    retVal = SDP_AddAttribute(sdpHandle_,
        PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID,
        SDP_TYPE_UINT_32,
        &supportedFeatures,
        sizeof(supportedFeatures));
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp SDP_AddAttribute retVal = %{public}d", retVal);
        return retVal;
    }
    PBAP_PSE_LOG_INFO("PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID:[%#X], value:[%#X]",
        PBAP_SUPPORTED_FEATURES_ATTRIBUTE_ID,
        supportedFeatures);
    return retVal;
}

int PbapPseSdp::AddBluetoothProfileDescriptorList() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    // Bluetooth Profile Descriptor List
    SdpProfileDescriptor profileDescriptor;
    profileDescriptor.profileUuid.type = BT_UUID_16;
    profileDescriptor.profileUuid.uuid16 = PBAP_PSE_PROFILE_UUID16;
    profileDescriptor.versionNumber = PBAP_PSE_VERSION_NUMBER;
    // create Bluetooth Profile Descriptor List
    int retVal = SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDescriptor, 1);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddBluetoothProfileDescriptorList retVal = %{public}d", retVal);
    }
    return retVal;
}

int PbapPseSdp::AddServiceName() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    // Service Name
    int retVal = SDP_AddServiceName(
        sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, PBAP_PSE_SERVICE_NAME.c_str(), PBAP_PSE_SERVICE_NAME.length());
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddServiceName retVal = %{public}d", retVal);
    }
    return retVal;
}

int PbapPseSdp::AddBrowseGroupList() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    BtUuid btUuid = {BT_UUID_16, {SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID}};
    int retVal = SDP_AddBrowseGroupList(sdpHandle_, &btUuid, 1);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp AddBrowseGroupList retVal = %{public}d", retVal);
    }
    return retVal;
}

int PbapPseSdp::RegisterServiceRecord() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int retVal = SDP_RegisterServiceRecord(sdpHandle_);
    if (retVal != BT_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Sdp RegisterServiceRecord retVal = %{public}d", retVal);
    }
    return retVal;
}
}  // namespace bluetooth
}  // namespace OHOS