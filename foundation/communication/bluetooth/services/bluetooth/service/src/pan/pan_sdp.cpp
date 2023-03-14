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

#include "pan_sdp.h"
#include "btstack.h"
#include "log.h"
#include "sdp.h"
namespace OHOS {
namespace bluetooth {
PanSdp::PanSdp()
{}

int PanSdp::Register()
{
    LOG_DEBUG("[PAN SDP]%{public}s():enter", __PRETTY_FUNCTION__);
    int result = BT_NO_ERROR;
    sdpHandle_ = SDP_CreateServiceRecord();
    result = RegisterServiceInfo();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] RegisterServiceInfo result = %{public}d", result);
        return result;
    }
    result = AddPanDescriptionList();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddPanDescriptionList result = %{public}d", result);
        return result;
    }
    result = AddPanSecurityDescriptionId();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddPanSecurityDescriptionId result = %{public}d", result);
        return result;
    }
    result = RegisterNetInfo();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] RegisterNetInfo result = %{public}d", result);
        return result;
    }
    result = AddBluetoothProfileDescriptorList();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddBluetoothProfileDescriptorList result = %{public}d", result);
        return result;
    }
    result = AddLanguageBaseAttributeIdList();
        if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddLanguageBaseAttributeIdList result = %{public}d", result);
        return result;
    }
    result = AddBrowseGroupList();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddBrowseGroupList result = %{public}d", result);
        return result;
    }
    result = RegisterServiceRecord();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] RegisterServiceRecord result = %{public}d", result);
        return result;
    }
    return result;
}

void PanSdp::Deregister()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    int result = BT_NO_ERROR;
    result = SDP_DeregisterServiceRecord(sdpHandle_);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] SDP_DeregisterServiceRecord result = %{public}d", result);
        return;
    }
    result = SDP_DestroyServiceRecord(sdpHandle_);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] SDP_DestroyServiceRecord result = %{public}d", result);
    }
    sdpHandle_ = 0;
}

int PanSdp::AddServiceClassIdList()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    BtUuid classId[1];
    classId[0].type = BT_UUID_16;
    classId[0].uuid16 = NAP_UUID16;
    // ServiceClassID
    int result = SDP_AddServiceClassIdList(sdpHandle_, classId, 1);
    return result;
}

int PanSdp::AddPanSecurityDescriptionId()
{
    LOG_INFO("[PAN SDP] Call %{public}s", __PRETTY_FUNCTION__);
    uint16_t security = 0;
    security |= 0x0001;
    int result = SDP_AddAttribute(sdpHandle_, PAN_SECURITY_DESCRIPTION_ID,
        SDP_TYPE_UINT_16, &security, sizeof(security));
    return result;
}

int PanSdp::AddPanNetAccessRate()
{
    LOG_INFO("[PAN SDP] Call %{public}s", __PRETTY_FUNCTION__);
    uint16_t PanNetAccessType = 0x0005;
    int result = SDP_AddAttribute(sdpHandle_, PAN_NET_ACCESS_TYPE_ID, SDP_TYPE_UINT_16,
        &PanNetAccessType, PAN_NET_ACCESS_TYPE_LENGTH);
        return result;
}

int PanSdp::AddPanNetAccessType()
{
    LOG_INFO("[PAN SDP] Call %{public}s", __PRETTY_FUNCTION__);
    uint32_t PanNetAccessRate = 0x0001312D0;
    int result = SDP_AddAttribute(sdpHandle_, PAN_MAX_NET_ACCESS_RATE_ID, SDP_TYPE_UINT_32,
        &PanNetAccessRate, PAN_NET_ACCESS_RATE_LENGTH);
    return result;
}

int PanSdp::AddPanDescriptionList()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    static const uint8_t panProtocolDescriptorListData[] = {
        0x35, 0x06,        // l2cap sequence
        0x19, 0x01, 0x00,  // l2cap uuid(0x0100)
        0x09, 0x00, 0x0F,  // l2cap psm(0x000f)
        0x35, 0x0E,        // bnep seqence
        0x19, 0x00, 0x0F,  // bnep uuid(0x000f)
        0x09, 0x01, 0x00,  // bnep version(0x0100)
        0x35, 0x06,        // network information
        0x09, 0x08, 0x00,  // network packet type IPv4(0x0800)
        0x09, 0x08, 0x06   // network packet type ARP(0x0806)
    };
    int result = SDP_AddSequenceAttribute(sdpHandle_, SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST,
        (uint8_t*)panProtocolDescriptorListData, sizeof(panProtocolDescriptorListData));
    return result;
}
int PanSdp::AddBluetoothProfileDescriptorList()
{
    LOG_INFO("[PAN SDP] Call %{public}s", __PRETTY_FUNCTION__);
    // Bluetooth Profile Descriptor List
    SdpProfileDescriptor profileDescriptor;
    profileDescriptor.profileUuid.type = BT_UUID_16;
    profileDescriptor.profileUuid.uuid16 = NAP_UUID16;
    profileDescriptor.versionNumber = PAN_VERSION_NUMBER;
    // create Bluetooth Profile Descriptor List
    int result = SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDescriptor, 1);
    return result;
}
int PanSdp::AddServiceName()
{
    LOG_INFO("[PAN SDP] Call %{public}s", __PRETTY_FUNCTION__);
    // Service Name
    int result = SDP_AddServiceName(
        sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, PAN_SERVICE_NAME.c_str(), PAN_SERVICE_NAME.length());
    return result;
}
int PanSdp::AddLanguageBaseAttributeIdList()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    SdpLanguageBaseAttributeId languageBaseAttr;
    languageBaseAttr.languageIdentifier = PAN_LANG_ID_CODE_ENGLISH;
    languageBaseAttr.characterEncodingIdentifier = PAN_LANG_ID_CHAR_ENCODE_UTF8;
    languageBaseAttr.baseAttributeId = PAN_LANGUAGE_BASE_ID;
    int result = SDP_AddLanguageBaseAttributeIdList(sdpHandle_, &languageBaseAttr, 1);
    return result;
}
int PanSdp::AddBrowseGroupList()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    BtUuid btUuid = {BT_UUID_16, {SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID}};
    int result = SDP_AddBrowseGroupList(sdpHandle_, &btUuid, 1);
    return result;
}
int PanSdp::AddServiceDescription()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    int result = SDP_AddServiceDescription(sdpHandle_, PAN_SERVICE_DESCRIPTION_ID,
        PAN_SERVICE_DESCRIPTION.c_str(), PAN_SERVICE_DESCRIPTION.length());
    return result;
}
int PanSdp::RegisterServiceRecord()
{
    LOG_INFO("[PAN SDP]Call %{public}s", __PRETTY_FUNCTION__);
    int result = SDP_RegisterServiceRecord(sdpHandle_);
    return result;
}
int PanSdp::RegisterNetInfo()
{
    int result = BT_NO_ERROR;
    result = AddPanNetAccessRate();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddPanNetAccessRate result = %{public}d", result);
        return result;
    }
    result = AddPanNetAccessType();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddPanNetAccessRate result = %{public}d", result);
        return result;
    }
    return result;
}
int PanSdp::RegisterServiceInfo()
{
    int result = BT_NO_ERROR;
    result = AddServiceClassIdList();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddServiceClassIdList result = %{public}d", result);
        return result;
    }
    result = AddServiceName();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddServiceName result = %{public}d", result);
        return result;
    }
    result = AddServiceDescription();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[PAN SDP] AddServiceName result = %{public}d", result);
        return result;
    }
    return result;
}
}  // namespace bluetooth
}  // namespace OHOS