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
#include "wifi_p2p_service_request_list.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pServiceRequestList");

namespace OHOS {
namespace Wifi {
WifiP2pServiceRequestList::WifiP2pServiceRequestList() : updateIndic(0), frequency(0), dialogToken(-1), p2pDevice(), srvReqList()
{}
bool WifiP2pServiceRequestList::AddServiceRequest(const WifiP2pServiceRequest &req)
{
    for (auto it : srvReqList) {
        if (it == req) {
            return false;
        }
    }
    srvReqList.push_back(req);
    return true;
}
bool WifiP2pServiceRequestList::RemoveServiceRequest(const WifiP2pServiceRequest &req)
{
    for (auto it = srvReqList.begin(); it != srvReqList.end(); it++) {
        if (*it == req) {
            srvReqList.erase(it);
            return true;
        }
    }
    return false;
}
bool WifiP2pServiceRequestList::ClearServiceRequest()
{
    srvReqList.clear();
    return true;
}
const std::vector<WifiP2pServiceRequest> &WifiP2pServiceRequestList::GetServiceRequestList() const
{
    return srvReqList;
}

void WifiP2pServiceRequestList::SetUpdateIndic(unsigned short setUpdateIndic)
{
    updateIndic = setUpdateIndic;
}
unsigned short WifiP2pServiceRequestList::GetUpdateIndic() const
{
    return updateIndic;
}
void WifiP2pServiceRequestList::SetFrequency(int setFrequency)
{
    frequency = setFrequency;
}
int WifiP2pServiceRequestList::GetFrequency() const
{
    return frequency;
}
void WifiP2pServiceRequestList::SetDialogToken(int setDialogToken)
{
    dialogToken = setDialogToken;
}
int WifiP2pServiceRequestList::GetDialogToken() const
{
    return dialogToken;
}
std::vector<unsigned char> WifiP2pServiceRequestList::GetTlvs() const
{
    std::vector<unsigned char> ret;

    for (auto tlvsIter = srvReqList.begin(); tlvsIter != srvReqList.end(); ++tlvsIter) {
        std::vector<unsigned char> buf = tlvsIter->GetTlv();
        for (auto dataIter = buf.begin(); dataIter != buf.end(); ++dataIter) {
            ret.push_back(*dataIter);
        }
    }

    return ret;
}
void WifiP2pServiceRequestList::SetDevice(const WifiP2pDevice &device)
{
    p2pDevice = device;
}

const WifiP2pDevice &WifiP2pServiceRequestList::GetDevice() const
{
    return p2pDevice;
}

bool WifiP2pServiceRequestList::ParseTlvs2ReqList(const std::vector<unsigned char> &tlvs)
{
    std::size_t leftLength = tlvs.size();
    std::size_t headLength = SERVICE_TLV_LENGTH_SIZE + PROTOCOL_SIZE + TRANSACTION_ID_SIZE;
    std::size_t pos = 0;
    while (leftLength > 0) {
        if (leftLength < headLength) {
            WIFI_LOGW("Failed to format input tlv packet!");
            return false;
        }
        unsigned short length = tlvs[pos] + (tlvs[pos + 1] << CHAR_BIT);
        unsigned short dataLength = length - PROTOCOL_SIZE - TRANSACTION_ID_SIZE;
        int protocolType = tlvs[pos + SERVICE_TLV_LENGTH_SIZE];
        unsigned char transId = tlvs[pos + SERVICE_TLV_LENGTH_SIZE + PROTOCOL_SIZE];

        if (dataLength > leftLength - headLength || dataLength < 0) {
            WIFI_LOGW("A tlv packet error!");
            return false;
        }

        pos += headLength;
        std::vector<unsigned char> query;
        for (unsigned short i = 0; i < dataLength; ++i) {
            query.push_back(tlvs[pos + i]);
        }
        pos += dataLength;

        WifiP2pServiceRequest buf;
        buf.SetProtocolType(static_cast<P2pServicerProtocolType>(protocolType));
        buf.SetTransactionId(transId);
        buf.SetQuery(query);
        srvReqList.push_back(buf);
        leftLength = leftLength - headLength - dataLength;
    }
    return true;
}
}  // namespace Wifi
}  // namespace OHOS
