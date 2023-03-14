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
#ifndef OHOS_WIFI_P2P_SERVICE_RESPONSE_LIST_H
#define OHOS_WIFI_P2P_SERVICE_RESPONSE_LIST_H

#include "wifi_p2p_msg.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
const int MAX_BUF_SIZE = 1024;
class WifiP2pServiceResponseList {
public:
    /**
     * @Description Construct a new WifiP2pServiceResponseList object.
     *
     */
    WifiP2pServiceResponseList();
    /**
     * @Description Construct a new WifiP2pServiceResponseList object.
     *
     * @param device source/destination device
     * @param respList service discovery response list
     */
    WifiP2pServiceResponseList(const WifiP2pDevice &device, const std::vector<WifiP2pServiceResponse> &respList);
    /**
     * @Description Default destructor.
     *
     */
    virtual ~WifiP2pServiceResponseList() = default;
    /**
     * @Description Adding a service response to the response list.
     *
     * @param resp service response
     * @return true adding succeeded
     * @return false adding failed
     */
    virtual bool AddServiceResponse(const WifiP2pServiceResponse &resp);
    /**
     * @Description Remove the corresponding service response from the response list.
     *
     * @param resp service response
     * @return true removal succeeded
     * @return false removal failed
     */
    virtual bool RemoveServiceResponse(const WifiP2pServiceResponse &resp);
    /**
     * @Description Default assignment operator function.
     *
     * @return WifiP2pServiceResponseList& assigned object
     */
    WifiP2pServiceResponseList &operator=(const WifiP2pServiceResponseList &) = default;
    WifiP2pServiceResponseList(const WifiP2pServiceResponseList &) = default;
    /**
     * @Description Obtain the const reference of srvRespList.
     * @return - const std::vector<WifiP2pServiceResponse>&
     */
    virtual const std::vector<WifiP2pServiceResponse> &GetServiceResponseList() const;
    /**
     * @Description - Filter the response list based on the response status.
     * @param  status - response status
     * @return - WifiP2pServiceResponseList list that meets the conditions
     */
    virtual WifiP2pServiceResponseList FilterSerivceResponse(P2pServiceStatus status) const;
    /**
     * @Description - Reverse filter the response list based on the response status.
     * @param  status - response status
     * @return - WifiP2pServiceResponseList list that does not meet the conditions
     */
    virtual WifiP2pServiceResponseList ReverseFilterSerivceResponse(P2pServiceStatus status) const;
    /**
     * @Description - Deduplicated responses and combine the same responses.
                      (The data must be the same device but not the same dialogToken)
     * @return - bool eligible for merger and merger   false:ineligible for consolidation
     */
    virtual bool MergerAndDeduplicate(const WifiP2pServiceResponseList &respList);
    /**
     * @Description If there are multiple WifiP2pServiceResponse messages, the and form a TLVS packet.
     * @return - std::vector<unsigned char>
     */
    virtual std::vector<unsigned char> GetTlvs();
    /**
     * @Description Set the update indicate.
     *
     * @param setUpdateIndic update indicate
     */
    virtual void SetUpdateIndic(unsigned short setUpdateIndic);
    /**
     * @Description Get the update indicate.
     *
     * @return unsigned short update indicate
     */
    virtual unsigned short GetUpdateIndic() const;
    /**
     * @Description Set the operating frequency.
     *
     * @param setFrequency frequency number
     */
    virtual void SetFrequency(int setFrequency);
    /**
     * @Description Get the operating frequency.
     *
     * @return int frequency number
     */
    virtual int GetFrequency() const;
    /**
     * @Description Set the dialog ID.
     *
     * @param setDialogToken dialog ID
     */
    virtual void SetDialogToken(int setDialogToken);
    /**
     * @Description Get the dialog ID.
     *
     * @return int dialog ID
     */
    virtual int GetDialogToken() const;
    /**
     * @Description Set the source or target device.
     *
     * @param device device information
     */
    virtual void SetDevice(const WifiP2pDevice &device);
    /**
     * @Description Get the source or target device.
     *
     * @return const WifiP2pDevice& device information
     */
    virtual const WifiP2pDevice &GetDevice() const;
    /**
     * @Description Parses the TLVS packet of the service response and saves the data to srvRespList.
     *
     * @param tlvList TLVS data packet
     * @return true parsing succeeded
     * @return false parsing failed
     */
    virtual bool ParseTlvs2RespList(const std::vector<unsigned char> &tlvList);

private:
    unsigned short updateIndic;                       /* update counter ID */
    int frequency;                                    /* operating frequency */
    int dialogToken;                                  /* dialog ID */
    WifiP2pDevice p2pDevice;                             /* source or target device */
    std::vector<WifiP2pServiceResponse> srvRespList;  /* response received or responded */
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_WIFI_P2P_SERVICE_RESPONSE_LIST_H */
