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

#include "hfp_hf_rfcomm_connection.h"

#include "btstack.h"
#include "hfp_hf_gap_client.h"

namespace OHOS {
namespace bluetooth {
std::map<uint16_t, std::string> HfpHfRfcommConnection::g_devMap;

int HfpHfRfcommConnection::Init()
{
    g_devMap.clear();
    return BT_NO_ERROR;
}

void HfpHfRfcommConnection::CleanUp()
{
    g_devMap.clear();
}

int HfpHfRfcommConnection::Create()
{
    int ret = gap_->RegisterServiceSecurity(remoteAddr_, remoteScn_);
    HFP_HF_RETURN_IF_FAIL(ret);

    RfcommConnectReqInfo info = {
        remoteAddr_, remoteScn_, HFP_HF_COMMAND_MTU,
        HFP_RFCOMM_CONNECTION_EVENTS, fn_, nullptr
    };
    ret = RFCOMM_ConnectChannel(&info, &connHandle_);

    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

int HfpHfRfcommConnection::Connect()
{
    int ret = Create();
    HFP_HF_RETURN_IF_FAIL(ret);

    std::string addr = RawAddress::ConvertToString(remoteAddr_.addr).GetAddress();
    HfpHfRfcommConnection::AddConnectionDevice(connHandle_, addr);

    return BT_NO_ERROR;
}

int HfpHfRfcommConnection::Disconnect() const
{
    int ret = RFCOMM_DisconnectChannel(connHandle_);
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = gap_->DeregisterServiceSecurity(remoteAddr_);
    HFP_HF_RETURN_IF_FAIL(ret);

    return BT_NO_ERROR;
}

int HfpHfRfcommConnection::ReadData(Packet **pkt) const
{
    int ret = RFCOMM_Read(connHandle_, pkt);
    if ((ret != RFCOMM_SUCCESS) && (ret != RFCOMM_NO_DATA)) {
        LOG_ERROR("[HFP HF]%{public}s():ret[%{public}d]", __FUNCTION__, ret);
    }
    return ret;
}

int HfpHfRfcommConnection::WriteData(Packet &pkt) const
{
    int ret = RFCOMM_Write(connHandle_, &pkt);
    HFP_HF_RETURN_IF_FAIL(ret);
    return BT_NO_ERROR;
}

void HfpHfRfcommConnection::SetConnectionHandle(uint16_t handle)
{
    connHandle_ = handle;
}

uint16_t HfpHfRfcommConnection::GetConnectionHandle() const
{
    return connHandle_;
}

void HfpHfRfcommConnection::SetRemoteAddr(const std::string &addr)
{
    RawAddress rawAddr(addr);
    rawAddr.ConvertToUint8(remoteAddr_.addr);
    remoteAddr_.type = BT_PUBLIC_DEVICE_ADDRESS;
}

void HfpHfRfcommConnection::SetRemoteScn(uint8_t scn)
{
    remoteScn_ = scn;
}

uint8_t HfpHfRfcommConnection::GetRemoteScn() const
{
    return remoteScn_;
}

std::string HfpHfRfcommConnection::GetRemoteAddressByHandle(uint16_t handle)
{
    auto it = g_devMap.find(handle);
    if (it != g_devMap.end()) {
        return g_devMap[handle];
    } else {
        return "";
    }
}

void HfpHfRfcommConnection::AddConnectionDevice(uint16_t handle, std::string addr)
{
    g_devMap.insert(std::make_pair<uint16_t, std::string>(std::move(handle), std::move(addr)));
}

void HfpHfRfcommConnection::RemoveConnectionDevice(uint16_t handle)
{
    auto it = g_devMap.find(handle);
    if (it != g_devMap.end()) {
        g_devMap.erase(it);
    }
}
}  // namespace bluetooth
}  // namespace OHOS