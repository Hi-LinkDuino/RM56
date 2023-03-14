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

#include "di_service_over_bredr.h"
#include "../gatt_defines.h"
#include "bt_def.h"
#include "class_creator.h"
#include "log.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
int DiServiceOverBredr::RegisterSDP(uint16_t startHandle, uint16_t endHandle)
{
    LOG_DEBUG("[DeviceInformationService]::%{public}s", __FUNCTION__);

    sdpHandle_ = SDP_CreateServiceRecord();
    if (sdpHandle_ == 0) {
        LOG_ERROR("%{public}s: SDP_CreateServiceRecord Failed!", __FUNCTION__);
        return false;
    }

    int ret = SdpAddServiceClassIdList();
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddServiceClassIdList Failed", __FUNCTION__);
    }

    ret = SdpAddProtocol(startHandle, endHandle);
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddProtocolDescriptorList Failed", __FUNCTION__);
    }

    ret = SdpAddBrowseGroupList();
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddBrowseGroupList Failed", __FUNCTION__);
    }

    ret = SDP_RegisterServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_RegisterServiceRecord Failed", __FUNCTION__);
        return GattStatus::GATT_FAILURE;
    }

    return GattStatus::GATT_SUCCESS;
}

void DiServiceOverBredr::DeregisterSDP()
{
    LOG_DEBUG("[GenericAttributeService]::%{public}s", __FUNCTION__);
    int ret = SDP_DeregisterServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_DeregisterServiceRecord Failed", __FUNCTION__);
    }

    ret = SDP_DestroyServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_DestroyServiceRecord Failed", __FUNCTION__);
    }
}

int DiServiceOverBredr::SdpAddServiceClassIdList() const
{
    BtUuid classid;
    classid.type = BT_UUID_16;
    classid.uuid16 = UUID_DEVICE_INFORMATION_SERVICE;
    return SDP_AddServiceClassIdList(sdpHandle_, &classid, 1);
}

int DiServiceOverBredr::SdpAddProtocol(uint16_t startHandle, uint16_t endHandle) const
{
    static const size_t disProtocolNum = 2;
    static const size_t gattProtParmNum = 2;
    static const uint16_t gattPsmAtt = 0x001F;
    SdpProtocolDescriptor protocol[disProtocolNum];
    protocol[0].protocolUuid.type = BT_UUID_16;
    protocol[0].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    protocol[0].parameterNumber = 1;
    protocol[0].parameter[0].type = SDP_TYPE_UINT_16;
    protocol[0].parameter[0].value = gattPsmAtt;

    protocol[1].protocolUuid.type = BT_UUID_16;
    protocol[1].protocolUuid.uuid16 = UUID_PROTOCOL_ATT;
    protocol[1].parameterNumber = gattProtParmNum;
    protocol[1].parameter[0].type = SDP_TYPE_UINT_16;
    protocol[1].parameter[0].value = startHandle;
    protocol[1].parameter[1].type = SDP_TYPE_UINT_16;
    protocol[1].parameter[1].value = endHandle;
    return SDP_AddProtocolDescriptorList(sdpHandle_, protocol, disProtocolNum);
}

int DiServiceOverBredr::SdpAddBrowseGroupList() const
{
    BtUuid browseGroupList[1];
    browseGroupList[0].type = BT_UUID_16;
    browseGroupList[0].uuid16 = SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID;
    return SDP_AddBrowseGroupList(sdpHandle_, browseGroupList, 1);
}

REGISTER_CLASS_CREATOR(DiServiceOverBredr);
}  // namespace bluetooth
}  // namespace OHOS