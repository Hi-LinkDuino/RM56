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

#include "socket_sdp_client.h"
#include "log.h"
#include "socket_service.h"

namespace OHOS {
namespace bluetooth {
SocketSdpClient::~SocketSdpClient()
{}

int SocketSdpClient::StartDiscovery(const std::string &addr, const Uuid &uuid, void *context)
{
    LOG_INFO("[sock]%{public}s", __func__);

    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid[1];
    classid[0].type = BT_UUID_128;
    uuid.ConvertToBytesLE(classid[0].uuid128);
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = SPP_CLASSID_NUM;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;
    attributeIdList.attributeIdList.attributeIdNumber = SPP_ATTR_NUM;
    attributeIdList.attributeIdList.attributeId[SOCK_SDP_IDX0] = SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST;
    attributeIdList.attributeIdList.attributeId[SOCK_SDP_IDX1] = SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeId[SOCK_SDP_IDX2] = SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;

    int ret =
        SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, context, &SocketSdpClient::SdpSearchCallback);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[sock]%{public}s SearchAttribute error", __FUNCTION__);
    }
    return ret;
}

uint8_t SocketSdpClient::SearchAttributes(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum)
{
    LOG_INFO("[sock]%{public}s", __func__);

    uint8_t scn = -1;
    uint16_t num = 0;
    while (num < serviceNum) {
        if (!(SearchScn(serviceAry->descriptorNumber, serviceAry->descriptor, scn))) {
            num++;
            if (num == serviceNum) {
                LOG_ERROR("[sock]%{public}s search scn error", __FUNCTION__);
            }
        } else {
            break;
        }
    }
    return scn;
}

bool SocketSdpClient::SearchScn(uint16_t protocolNum, const SdpProtocolDescriptor *protocol, uint8_t &scn)
{
    uint16_t num = 0;

    while (num < protocolNum) {
        if ((protocol[num].protocolUuid.uuid16 == UUID_PROTOCOL_RFCOMM) &&
            (protocol[num].parameter[0].type == SDP_TYPE_UINT_8)) {
            scn = (uint8_t)protocol[num].parameter[0].value;
            LOG_INFO("[sock]%{public}s scn:%hhu", __func__, scn);
            return true;
        }
        num++;
    }
    return false;
}

void SocketSdpClient::SdpSearchCallback(
    const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    LOG_INFO("[sock]%{public}s", __func__);

    uint8_t scn = SearchAttributes(addr, serviceAry, serviceNum);
    LOG_INFO("[sock]%{public}s scn:%hhu", __func__, scn);

    utility::Message msg(SOCKET_SDP_DISCOVERY_RESULT);
    msg.arg1_ = scn;
    msg.arg2_ = context;
    SocketService *socketService =
        static_cast<SocketService *>(IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP));
    socketService->GetDispatcher()->PostTask(std::bind(&SocketService::ProcessMessage, socketService, msg));
}
}  // namespace bluetooth
}  // namespace OHOS