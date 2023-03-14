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
#ifndef OHOS_WIFI_P2P_SERVICE_REQUEST_LIST_H
#define OHOS_WIFI_P2P_SERVICE_REQUEST_LIST_H

#include "wifi_p2p_msg.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class WifiP2pServiceRequestList {
public:
    /**
     * @Description Construct a new WifiP2pServiceRequestList object.
     *
     */
    WifiP2pServiceRequestList();

    /**
     * @Description Default destructor.
     *
     */
    virtual ~WifiP2pServiceRequestList() = default;

    /**
     * @Description Add service discovery request.
     *
     * @param req service discovery request
     * @return true adding succeeded
     * @return false adding failed
     */
    virtual bool AddServiceRequest(const WifiP2pServiceRequest &req);

    /**
     * @Description Deleting service discovery request.
     *
     * @param req service discovery request
     * @return true deleted successfully
     * @return false deletion failed
     */
    virtual bool RemoveServiceRequest(const WifiP2pServiceRequest &req);

    /**
     * @Description Clear the list of service discovery requests.
     *
     * @return true deleted successfully
     * @return false deletion failed
     */
    virtual bool ClearServiceRequest();

    /**
     * @Description Obtains the list of all service discovery requests.
     *
     * @param srvReqList a list of saving service discovery requests
     * @return true deleted successfully
     * @return false deletion failed
     */
    virtual const std::vector<WifiP2pServiceRequest> &GetServiceRequestList() const;

    /**
     * @Description Set the update indicate.
     *
     * @param setUpdateIndic number of the update indicate
     */
    virtual void SetUpdateIndic(unsigned short setUpdateIndic);

    /**
     * @Description Get the update indicate.
     *
     * @return unsigned short number of the update indicate
     */
    virtual unsigned short GetUpdateIndic() const;

    /**
     * @Description Set the operating frequency.
     *
     * @param setFrequency number of frequency
     */
    virtual void SetFrequency(int setFrequency);

    /**
     * @Description Get the operating frequency.
     *
     * @return int number of frequency
     */
    virtual int GetFrequency() const;

    /**
     * @Description Set the dialog ID.
     *
     * @param setDialogToken number of the dialog ID
     */
    virtual void SetDialogToken(int setDialogToken);

    /**
     * @Description Get the dialog ID.
     *
     * @return int number of the dialog ID
     */
    virtual int GetDialogToken() const;

    /**
     * @Description Obtains the TLVs of the list combination.
     * @return - std::vector<unsigned char>
     */
    virtual std::vector<unsigned char> GetTlvs() const;

    /**
     * @Description Set the p2pDevice member.
     *
     * @param device device object to be set
     */
    virtual void SetDevice(const WifiP2pDevice &device);

    /**
     * @Description Get the p2pDevice member instance.
     *
     * @return const WifiP2pDevice& p2pDevice
     */
    virtual const WifiP2pDevice &GetDevice() const;
    /**
     * @Description Parses the TLVS package of the service request and saves the data to the srvReqList table.
     *
     * @param tlvs TLVS data packet
     * @return true parsing succeeded
     * @return false parsing failed
     */
    virtual bool ParseTlvs2ReqList(const std::vector<unsigned char> &tlvs);

private:
    unsigned short updateIndic;                       /* update counter ID */
    int frequency;                                    /* operating frequency */
    int dialogToken;                                  /* dialog ID */
    WifiP2pDevice p2pDevice;                             /* source (or target) device */
    std::vector<WifiP2pServiceRequest> srvReqList;    /* received (or sent) a list of requests */
};
}  // namespace Wifi
}  // namespace OHOS

#endif  /* OHOS_WIFI_P2P_SERVICE_REQUEST_LIST_H */
