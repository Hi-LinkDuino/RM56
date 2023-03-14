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

#include "socket_sdp_server.h"
#include "log.h"
#include "socket_def.h"

namespace OHOS {
namespace bluetooth {
int SocketSdpServer::RegisterSdpService(const std::string &name, const Uuid &uuid, uint8_t scn)
{
    LOG_INFO("[sock]%{public}s", __func__);

    sdpHandle_ = SDP_CreateServiceRecord();

    int ret = AddServiceClassId(uuid);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s AddServiceClassId error", __FUNCTION__);
        return ret;
    }

    LOG_INFO("[sock]%{public}s AddProtocol scn:%hhu", __func__, scn);
    ret = AddProtocol(scn);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s AddProtocol error", __FUNCTION__);
        return ret;
    }

    ret = AddProfile();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s AddProfile error", __FUNCTION__);
        return ret;
    }

    ret = AddServiceName(name);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s AddServiceName error", __FUNCTION__);
        return ret;
    }

    ret = AddBrowseGroupList();
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s AddBrowseGroupList error", __FUNCTION__);
        return ret;
    }

    ret = SDP_RegisterServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s SDP_RegisterServiceRecord error", __FUNCTION__);
        return ret;
    }

    return ret;
}

int SocketSdpServer::UnregisterSdpService()
{
    LOG_INFO("[sock]%{public}s", __func__);

    int ret = SDP_DeregisterServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s SDP_DeregisterServiceRecord error", __FUNCTION__);
        return ret;
    }

    ret = SDP_DestroyServiceRecord(sdpHandle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s SDP_DestroyServiceRecord error", __FUNCTION__);
        return ret;
    }

    return ret;
}

int SocketSdpServer::AddServiceClassId(const Uuid &uuid)
{
    BtUuid classid[1];
    classid[0].type = BT_UUID_128;
    uuid.ConvertToBytesLE(classid[0].uuid128);

    return SDP_AddServiceClassIdList(sdpHandle_, classid, SPP_CLASSID_NUM);
}

int SocketSdpServer::AddProtocol(uint8_t scn)
{
    SdpProtocolDescriptor protocol[SPP_PRRTOCOL_NUM];
    protocol[0].protocolUuid.type = BT_UUID_16;
    protocol[0].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    protocol[0].parameterNumber = 0;
    protocol[1].protocolUuid.type = BT_UUID_16;
    protocol[1].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    protocol[1].parameterNumber = 1;
    protocol[1].parameter[0].type = SDP_TYPE_UINT_8;
    protocol[1].parameter[0].value = scn;

    return SDP_AddProtocolDescriptorList(sdpHandle_, protocol, SPP_PRRTOCOL_NUM);
}

int SocketSdpServer::AddProfile()
{
    SdpProfileDescriptor profile[SPP_PROFILE_NUM];
    profile[0].profileUuid.type = BT_UUID_16;
    profile[0].profileUuid.uuid16 = UUID_SERVCLASS_SERIAL_PORT;
    profile[0].versionNumber = SPP_PROFILE_VERSION;

    return SDP_AddBluetoothProfileDescriptorList(sdpHandle_, profile, SPP_PROFILE_NUM);
}

int SocketSdpServer::AddServiceName(const std::string &name)
{
    return SDP_AddServiceName(sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, name.c_str(), name.length());
}

int SocketSdpServer::AddBrowseGroupList()
{
    BtUuid browseGroupList[SPP_BROWSE_LIST_NUM];
    browseGroupList[0].type = BT_UUID_16;
    browseGroupList[0].uuid16 = SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID;

    return SDP_AddBrowseGroupList(sdpHandle_, browseGroupList, SPP_BROWSE_LIST_NUM);
}
}  // namespace bluetooth
}  // namespace OHOS