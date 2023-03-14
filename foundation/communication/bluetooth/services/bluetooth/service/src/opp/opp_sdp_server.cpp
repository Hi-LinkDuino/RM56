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

#include "opp_sdp_server.h"

#include "log.h"
#include "opp_defines.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
int OppSdpServer::Register(uint8_t rfcommScn, uint16_t l2capPsm)
{
    HILOGI("[OPP SDP SERVER]:enter");
    int result = BT_NO_ERROR;

    sdpHandle_ = SDP_CreateServiceRecord();

    result = AddServiceClassIdList();
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddServiceClassIdList result = %{public}d", result);
        return result;
    }

    result = AddProtocolDescriptorList(rfcommScn);
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddProtocolDescriptorList result = %{public}d", result);
        return result;
    }

    result = AddBrowseGroupList();
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddBrowseGroupList result = %{public}d", result);
        return result;
    }

    result = AddBluetoothProfileDescriptorList();
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddBluetoothProfileDescriptorList result = %{public}d", result);
        return result;
    }

    result = AddServiceName();
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddServiceName result = %{public}d", result);
        return result;
    }

    result = AddL2capPsm(l2capPsm);
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddL2capPsm result = %{public}d", result);
        return result;
    }

    result = AddSupportedFormatsList();
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] Sdp AddSupportedFormatsList result = %{public}d", result);
        return result;
    }

    result = SDP_RegisterServiceRecord(sdpHandle_);
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] RegisterServiceRecord result = %{public}d", result);
        return result;
    }

    return result;
}

void OppSdpServer::Deregister()
{
    HILOGI("[OPP SDP SERVER] Call");
    int result = BT_NO_ERROR;

    result = SDP_DeregisterServiceRecord(sdpHandle_);
    if (result != BT_NO_ERROR) {
        HILOGE("[PAN SDP] SDP_DeregisterServiceRecord result = %{public}d", result);
        return;
    }

    result = SDP_DestroyServiceRecord(sdpHandle_);
    if (result != BT_NO_ERROR) {
        HILOGE("[OPP SDP SERVER] SDP_DestroyServiceRecord result = %{public}d", result);
    }
    sdpHandle_ = 0;
}

int OppSdpServer::AddServiceClassIdList() const
{
    HILOGI("[OPP SDP SERVER] Call");
    BtUuid classId[1];
    classId[0].type = BT_UUID_16;
    classId[0].uuid16 = OPP_UUID16;

    // ServiceClassID
    return SDP_AddServiceClassIdList(sdpHandle_, classId, 1);
}

int OppSdpServer::AddProtocolDescriptorList(uint8_t rfcommScn) const
{
    HILOGI("[OPP SDP SERVER] Call");
    SdpProtocolDescriptor protocol[OPP_PROTOCOL_DESCRIPTOR_NUMBER];
    uint8_t index = 0;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    protocol[index].parameterNumber = 0;
    index++;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    protocol[index].parameterNumber = 1;
    protocol[index].parameter[0].type = SDP_TYPE_UINT_8;
    protocol[index].parameter[0].value = rfcommScn;
    index++;
    protocol[index].protocolUuid.type = BT_UUID_16;
    protocol[index].protocolUuid.uuid16 = UUID_PROTOCOL_OBEX;
    protocol[index].parameterNumber = 0;
    return SDP_AddProtocolDescriptorList(sdpHandle_, protocol, OPP_PROTOCOL_DESCRIPTOR_NUMBER);
}

int OppSdpServer::AddBrowseGroupList() const
{
    HILOGI("[OPP SDP SERVER] Call");
    BtUuid btUuid = {BT_UUID_16, {SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID}};
    return SDP_AddBrowseGroupList(sdpHandle_, &btUuid, 1);
}

int OppSdpServer::AddBluetoothProfileDescriptorList() const
{
    HILOGI("[OPP SDP SERVER] Call");
    // Bluetooth Profile Descriptor List
    SdpProfileDescriptor profileDescriptor;
    profileDescriptor.profileUuid.type = BT_UUID_16;
    profileDescriptor.profileUuid.uuid16 = OPP_UUID16;
    profileDescriptor.versionNumber = OPP_VERSION_NUMBER;
    // create Bluetooth Profile Descriptor List
    return SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDescriptor, 1);
}

int OppSdpServer::AddServiceName() const
{
    HILOGI("[OPP SDP SERVER] Call");
    // Service Name
    return SDP_AddServiceName(sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE,
        OPP_SERVICE_NAME.c_str(), OPP_SERVICE_NAME.length());
}

int OppSdpServer::AddL2capPsm(uint16_t l2capPsm) const
{
    HILOGI("[OPP SDP SERVER] Call");
    // GoepL2CapPsm
    return SDP_AddAttribute(sdpHandle_,
        OPP_GOEP_L2CAP_PSM_ATTRIBUTE_ID, SDP_TYPE_UINT_16, &l2capPsm, sizeof(l2capPsm));
}

int OppSdpServer::AddSupportedFormatsList() const
{
    HILOGI("[OPP SDP SERVER] Call");
    return SDP_AddSequenceAttribute(sdpHandle_, OPP_GOEP_SUPPORTED_FORMATS_LIST_ATTRIBUTE_ID,
        reinterpret_cast<uint8_t*>(const_cast<uint8_t*>(OPP_SUPPORTED_FORMATS_LIST_DATA)),
        sizeof(OPP_SUPPORTED_FORMATS_LIST_DATA));
}
}  // namespace bluetooth
}  // namespace OHOS