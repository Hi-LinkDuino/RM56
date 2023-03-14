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

#include "hfp_ag_data_connection_server.h"


#include "hfp_ag_defines.h"
#include "hfp_ag_profile_event_sender.h"
#include "hfp_ag_rfcomm_connection.h"
#include "log_util.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
HfpAgDataConnectionServer &HfpAgDataConnectionServer::GetInstance()
{
    static HfpAgDataConnectionServer instance;
    return instance;
}

void HfpAgDataConnectionServer::ProcessDataConnectionServerCallback(
    uint16_t handle, uint32_t eventId, const std::string &inComingAddr)
{
    auto addr = HfpAgRfcommConnection::GetRemoteAddressByHandle(handle);
    HILOGI("Event from rfcomm device: %{public}s, handle: %{public}hu, eventId: %{public}u",
        GetEncryptAddr(addr).c_str(), handle, eventId);
    int event = HFP_AG_INVALID_EVT;
    switch (eventId) {
        case RFCOMM_CHANNEL_EV_CONNECT_INCOMING:
            addr = inComingAddr;
            HfpAgRfcommConnection::AddConnectionDevice(handle, inComingAddr);
            event = HFP_AG_CONNECT_REQUEST_EVT;
            break;
        case RFCOMM_CHANNEL_EV_CONNECT_SUCCESS:
            event = HFP_AG_CONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_CONNECT_FAIL:
            event = HFP_AG_CONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECTED:
        case RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS:
            HfpAgRfcommConnection::RemoveConnectionDevice(handle);
            event = HFP_AG_DISCONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECT_FAIL:
            event = HFP_AG_DISCONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_REV_DATA:
            event = HFP_AG_DATA_AVAILABLE_EVT;
            break;
        default:
            LOG_INFO("[HFP AG]%{public}s():Invalid event from rfcomm eventId[%u]", __FUNCTION__, eventId);
            break;
    }

    HfpAgProfileEventSender::GetInstance().ConnectRequest(addr, handle, event);
    return;
}

void HfpAgDataConnectionServer::DataConnectionServerCallback(
    uint16_t handle, uint32_t eventId, const void *eventData, void *context)
{
    LOG_INFO("[HFP AG]%{public}s():", __PRETTY_FUNCTION__);
    std::string addr;

    if (eventId == RFCOMM_CHANNEL_EV_CONNECT_INCOMING) {
        auto evtData = const_cast<RfcommIncomingInfo *>(static_cast<const RfcommIncomingInfo *>(eventData));
        addr = RawAddress::ConvertToString(evtData->addr.addr).GetAddress();
    }

    HfpAgProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpAgDataConnectionServer::ProcessDataConnectionServerCallback, handle, eventId, addr));
    return;
}

int HfpAgDataConnectionServer::RegisterServer() const
{
    return rfcommServer_.RegisterServer(&HfpAgDataConnectionServer::DataConnectionServerCallback);
}

int HfpAgDataConnectionServer::RemoveServer()
{
    return rfcommServer_.RemoveServer();
}

int HfpAgDataConnectionServer::AcceptConnection(uint16_t handle) const
{
    return rfcommServer_.AcceptConnection(handle);
}

int HfpAgDataConnectionServer::RejectConnection(uint16_t handle) const
{
    HfpAgRfcommConnection::RemoveConnectionDevice(handle);
    return rfcommServer_.RejectConnection(handle);
}

uint8_t HfpAgDataConnectionServer::AssignLocalScn()
{
    return rfcommServer_.AssignLocalScn();
}

uint8_t HfpAgDataConnectionServer::GetLocalScn() const
{
    return rfcommServer_.GetLocalScn();
}
}  // namespace bluetooth
}  // namespace OHOS