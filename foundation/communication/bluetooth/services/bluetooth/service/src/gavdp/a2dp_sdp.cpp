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

#include "a2dp_sdp.h"

#include "log.h"

namespace OHOS {
namespace bluetooth {
const int ATTR_NUMBER2 = 2;
const int ATTR_NUMBER1 = 1;
void A2dpSdpManager::SetProfileRole(uint8_t role)
{
    role_ = role;
}

int A2dpSdpManager::RegisterService()
{
    LOG_INFO("[A2dpSdpManager]%{public}s role[%u]\n", __func__, role_);
    int rtnSts;
    sdpHandle_ = SDP_CreateServiceRecord();

    /// Service Class ID List.
    BtUuid classIdList[A2DP_SERVICE_CLASS_ID_LIST_NUMBER];
    classIdList[0].type = BT_UUID_16;
    if (role_ == A2DP_ROLE_SOURCE) {
        classIdList[0].uuid16 = A2DP_SERVICE_CLASS_UUID;
    } else {
        classIdList[0].uuid16 = A2DP_SINK_SERVICE_CLASS_UUID;
    }
    rtnSts = SDP_AddServiceClassIdList(sdpHandle_, classIdList, A2DP_SERVICE_CLASS_ID_LIST_NUMBER);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_AddServiceClassIdList[result:%{public}d]\n", __func__, rtnSts);
        return rtnSts;
    }

    /// Protocol Descriptor List.
    SdpProtocolDescriptor dscList[A2DP_PROTOCOL_DESCRIPTOR_LIST_NUMBER];
    dscList[0].parameter[0].type = SDP_TYPE_UINT_16;
    dscList[0].parameter[0].value = A2DP_PROTOCOL_UUID_AVDTP;  // PSM= AVDTP
    dscList[0].parameterNumber = 1;
    dscList[0].protocolUuid.type = BT_UUID_16;
    dscList[0].protocolUuid.uuid16 = A2DP_PROTOCOL_UUID_L2CAP;
    dscList[ATTR_NUMBER1].parameter[0].type = SDP_TYPE_UINT_16;
    dscList[ATTR_NUMBER1].parameter[0].value = A2DP_PROFILE_REV_1_3;  // Profile Version
    dscList[ATTR_NUMBER1].parameterNumber = ATTR_NUMBER1;
    dscList[ATTR_NUMBER1].protocolUuid.type = BT_UUID_16;
    dscList[ATTR_NUMBER1].protocolUuid.uuid16 = A2DP_PROTOCOL_UUID_AVDTP;
    rtnSts = SDP_AddProtocolDescriptorList(sdpHandle_, dscList, A2DP_PROTOCOL_DESCRIPTOR_LIST_NUMBER);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_AddProtocolDescriptorList[result:%{public}d]\n", __func__, rtnSts);
        return rtnSts;
    }

    /// Bluetooth Profile Descriptor List.
    SdpProfileDescriptor profileDsc;
    profileDsc.versionNumber = A2DP_PROFILE_REV_1_3;
    profileDsc.profileUuid.type = BT_UUID_16;
    profileDsc.profileUuid.uuid16 = A2DP_PROFILE_UUID;
    rtnSts =
        SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDsc, A2DP_BLUETOOTH_PROFILE_DESCRIPTOR_LIST_NUMBER);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_AddBluetoothProfileDescriptorList[result:%{public}d]\n", __func__, rtnSts);
        return rtnSts;
    }

    /// Register target service.
    rtnSts = SDP_RegisterServiceRecord(sdpHandle_);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_RegisterServiceRecord[result:%{public}d]\n", __func__, rtnSts);
    }
    return rtnSts;
}

int A2dpSdpManager::UnregisterService(void) const
{
    LOG_INFO("[A2dpSdpManager]%{public}s\n", __func__);
    int rtnSts;

    rtnSts = SDP_DestroyServiceRecord(sdpHandle_);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_DestroyServiceRecord[result:%{public}d]\n", __func__, rtnSts);
        return rtnSts;
    }

    rtnSts = SDP_DeregisterServiceRecord(sdpHandle_);
    if (rtnSts != RET_NO_ERROR) {
        LOG_WARN("[A2dpSdpManager]%{public}s SDP_DeregisterServiceRecord[result:%{public}d]\n", __func__, rtnSts);
        return rtnSts;
    }

    return rtnSts;
}

int A2dpSdpManager::FindSnkService(const BtAddr &addr, void *a2dpInstance,
    void (*callback)(const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context)) const
{
    LOG_INFO("[A2dpSdpManager]%{public}s\n", __func__);
    void *context = a2dpInstance;
    BtUuid classid[A2DP_SERVICE_CLASS_ID_LIST_NUMBER];

    classid[0].type = BT_UUID_16;
    if (role_ == A2DP_ROLE_SOURCE) {
        classid[0].uuid16 = A2DP_SINK_SERVICE_CLASS_UUID;
    } else {
        classid[0].uuid16 = A2DP_SERVICE_CLASS_UUID;
    }

    SdpUuid sdpUUid;
    sdpUUid.uuidNum = A2DP_SERVICE_CLASS_ID_LIST_NUMBER;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;

    attributeIdList.attributeIdList.attributeIdNumber = A2DP_SDP_ATTRIBUTE_NUM;
    attributeIdList.attributeIdList.attributeId[0] = SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
    attributeIdList.attributeIdList.attributeId[ATTR_NUMBER1] = SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[ATTR_NUMBER2] = SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;

    return SDP_ServiceSearchAttribute(&addr, &sdpUUid, attributeIdList, context, callback);
}
}  // namespace bluetooth
}  // namespace OHOS
