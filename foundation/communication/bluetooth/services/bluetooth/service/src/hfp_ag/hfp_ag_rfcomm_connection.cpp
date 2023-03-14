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

#include "hfp_ag_rfcomm_connection.h"

#include "btstack.h"
#include "hfp_ag_gap_client.h"

namespace OHOS {
namespace bluetooth {
std::map<uint16_t, std::string> HfpAgRfcommConnection::g_devMap;

int HfpAgRfcommConnection::Init()
{
    g_devMap.clear();
    return BT_NO_ERROR;
}

void HfpAgRfcommConnection::CleanUp()
{
    g_devMap.clear();
}

int HfpAgRfcommConnection::Create()
{
    int ret = gap_->RegisterServiceSecurity(remoteAddr_, remoteScn_);
    HFP_AG_RETURN_IF_FAIL(ret);

    RfcommConnectReqInfo info = {
        remoteAddr_, remoteScn_, HFP_AG_COMMAND_MTU,
        HFP_RFCOMM_CONNECTION_EVENTS, fn_, nullptr
    };
    ret = RFCOMM_ConnectChannel(&info, &connHandle_);
    HFP_AG_RETURN_IF_FAIL(ret);

    return BT_NO_ERROR;
}

int HfpAgRfcommConnection::Connect()
{
    int ret = Create();
    HFP_AG_RETURN_IF_FAIL(ret);

    std::string addr = RawAddress::ConvertToString(remoteAddr_.addr).GetAddress();
    HfpAgRfcommConnection::AddConnectionDevice(connHandle_, addr);

    return BT_NO_ERROR;
}

int HfpAgRfcommConnection::Disconnect() const
{
    int ret = RFCOMM_DisconnectChannel(connHandle_);
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = gap_->DeregisterServiceSecurity(remoteAddr_);
    HFP_AG_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpAgRfcommConnection::ReadData(Packet **pkt) const
{
    int ret = RFCOMM_Read(connHandle_, pkt);
    if ((ret != RFCOMM_SUCCESS) && (ret != RFCOMM_NO_DATA)) {
        LOG_ERROR("[HFP AG]%{public}s():ret[%{public}d]", __FUNCTION__, ret);
    }
    return ret;
}

int HfpAgRfcommConnection::WriteData(Packet &pkt) const
{
    int ret = RFCOMM_Write(connHandle_, &pkt);
    HFP_AG_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

void HfpAgRfcommConnection::SetConnectionHandle(uint16_t handle)
{
    connHandle_ = handle;
}

uint16_t HfpAgRfcommConnection::GetConnectionHandle() const
{
    return connHandle_;
}

void HfpAgRfcommConnection::SetRemoteAddr(const std::string &addr)
{
    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(remoteAddr_.addr);
    remoteAddr_.type = BT_PUBLIC_DEVICE_ADDRESS;
}

void HfpAgRfcommConnection::SetRemoteScn(uint8_t scn)
{
    remoteScn_ = scn;
}

uint8_t HfpAgRfcommConnection::GetRemoteScn() const
{
    return remoteScn_;
}

std::string HfpAgRfcommConnection::GetRemoteAddressByHandle(uint16_t handle)
{
    auto it = g_devMap.find(handle);
    if (it != g_devMap.end()) {
        return g_devMap[handle];
    } else {
        return "";
    }
}

void HfpAgRfcommConnection::AddConnectionDevice(uint16_t handle, std::string addr)
{
    g_devMap.insert(std::make_pair<uint16_t, std::string>(std::move(handle), std::move(addr)));
}

void HfpAgRfcommConnection::RemoveConnectionDevice(uint16_t handle)
{
    auto it = g_devMap.find(handle);
    if (it != g_devMap.end()) {
        g_devMap.erase(it);
    }
}
}  // namespace bluetooth
}  // namespace OHOS