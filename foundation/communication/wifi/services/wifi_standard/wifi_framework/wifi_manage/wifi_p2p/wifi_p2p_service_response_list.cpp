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
#include "wifi_p2p_service_response_list.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pServiceResponseList");

namespace OHOS {
namespace Wifi {
WifiP2pServiceResponseList::WifiP2pServiceResponseList() : updateIndic(0), frequency(0), dialogToken(-1), p2pDevice(), srvRespList()
{}

WifiP2pServiceResponseList::WifiP2pServiceResponseList(
    const WifiP2pDevice &device, const std::vector<WifiP2pServiceResponse> &respList)
    : updateIndic(0), frequency(0), dialogToken(-1), p2pDevice(device), srvRespList(respList)
{}

bool WifiP2pServiceResponseList::AddServiceResponse(const WifiP2pServiceResponse &resp)
{
    for (auto it = srvRespList.begin(); it != srvRespList.end(); ++it) {
        if (*it == resp) {
            return true;
        }
    }
    srvRespList.push_back(resp);
    return true;
}
bool WifiP2pServiceResponseList::RemoveServiceResponse(const WifiP2pServiceResponse &resp)
{
    for (auto it = srvRespList.begin(); it != srvRespList.end(); ++it) {
        if (*it == resp) {
            srvRespList.erase(it);
            return true;
        }
    }
    return false;
}

const std::vector<WifiP2pServiceResponse> &WifiP2pServiceResponseList::GetServiceResponseList() const
{
    return srvRespList;
}

WifiP2pServiceResponseList WifiP2pServiceResponseList::FilterSerivceResponse(P2pServiceStatus status) const
{
    std::vector<WifiP2pServiceResponse> pushSrvRespList;
    for (auto it = srvRespList.begin(); it != srvRespList.end(); ++it) {
        if (it->GetServiceStatus() == status) {
            pushSrvRespList.push_back(*it);
        }
    }
    return WifiP2pServiceResponseList(p2pDevice, pushSrvRespList);
}
WifiP2pServiceResponseList WifiP2pServiceResponseList::ReverseFilterSerivceResponse(P2pServiceStatus status) const
{
    std::vector<WifiP2pServiceResponse> pushSrvRespList;
    for (auto it = srvRespList.begin(); it != srvRespList.end(); ++it) {
        if (it->GetServiceStatus() != status) {
            pushSrvRespList.push_back(*it);
        }
    }
    return WifiP2pServiceResponseList(p2pDevice, pushSrvRespList);
}
bool WifiP2pServiceResponseList::MergerAndDeduplicate(const WifiP2pServiceResponseList &respList)
{
    if (p2pDevice.GetDeviceAddress() != respList.GetDevice().GetDeviceAddress()) {
        WIFI_LOGE("Diffrent device!");
        return false;
    }

    for (auto mergerRespIter = respList.GetServiceResponseList().begin();
         mergerRespIter != respList.GetServiceResponseList().end();
         ++mergerRespIter) {
        bool findSameResp = false;
        for (auto iter = srvRespList.begin(); iter != srvRespList.end(); ++iter) {
            if (mergerRespIter->GetProtocolType() == iter->GetProtocolType() &&
                mergerRespIter->GetData() == iter->GetData()) {
                findSameResp = true;
                break;
            }
        }
        if (!findSameResp) {
            srvRespList.push_back(*mergerRespIter);
        }
    }
    return true;
}
std::vector<unsigned char> WifiP2pServiceResponseList::GetTlvs()
{
    std::vector<unsigned char> ret;

    for (auto tlvsIter = srvRespList.begin(); tlvsIter != srvRespList.end(); ++tlvsIter) {
        std::vector<unsigned char> buf = tlvsIter->GetTlv();
        for (auto dataIter = buf.begin(); dataIter != buf.end(); ++dataIter) {
            ret.push_back(*dataIter);
        }
    }

    return ret;
}
void WifiP2pServiceResponseList::SetUpdateIndic(unsigned short setUpdateIndic)
{
    updateIndic = setUpdateIndic;
}
unsigned short WifiP2pServiceResponseList::GetUpdateIndic() const
{
    return updateIndic;
}
void WifiP2pServiceResponseList::SetFrequency(int setFrequency)
{
    frequency = setFrequency;
}
int WifiP2pServiceResponseList::GetFrequency() const
{
    return frequency;
}
void WifiP2pServiceResponseList::SetDialogToken(int setDialogToken)
{
    dialogToken = setDialogToken;
}
int WifiP2pServiceResponseList::GetDialogToken() const
{
    return dialogToken;
}
void WifiP2pServiceResponseList::SetDevice(const WifiP2pDevice &device)
{
    p2pDevice = device;
}
const WifiP2pDevice &WifiP2pServiceResponseList::GetDevice() const
{
    return p2pDevice;
}

bool WifiP2pServiceResponseList::ParseTlvs2RespList(const std::vector<unsigned char> &tlvList)
{
    if (tlvList.empty()) {
        WIFI_LOGW("No Response Tlvs");
        return false;
    }
    std::size_t leftLength = tlvList.size();
    std::size_t headLength = SERVICE_TLV_LENGTH_SIZE + PROTOCOL_SIZE + TRANSACTION_ID_SIZE + SERVICE_STATUS_SIZE;
    std::size_t pos = 0;
    while (leftLength > 0) {
        unsigned short length = tlvList[pos] + (tlvList[pos + 1] << CHAR_BIT);
        unsigned short dataLength = length - PROTOCOL_SIZE - TRANSACTION_ID_SIZE - SERVICE_STATUS_SIZE;
        int type = tlvList[pos + SERVICE_TLV_LENGTH_SIZE];
        unsigned char transId = tlvList[pos + SERVICE_TLV_LENGTH_SIZE + PROTOCOL_SIZE];
        int status = tlvList[pos + SERVICE_TLV_LENGTH_SIZE + PROTOCOL_SIZE + TRANSACTION_ID_SIZE];

        if (dataLength > leftLength - headLength || dataLength < 0) {
            WIFI_LOGW("A tlv packet error!");
            return false;
        }

        pos += headLength;
        if (dataLength <= MAX_BUF_SIZE) {
            std::vector<unsigned char> data;
            for (unsigned short i = 0; i < dataLength; ++i) {
                data.push_back(tlvList[pos + i]);
            }
            WifiP2pServiceResponse buf(
                static_cast<P2pServicerProtocolType>(type), static_cast<P2pServiceStatus>(status), transId, data);
            srvRespList.push_back(buf);
        }
        pos += dataLength;
        leftLength = leftLength - headLength - dataLength;
    }
    return true;
}
}  // namespace Wifi
}  // namespace OHOS
