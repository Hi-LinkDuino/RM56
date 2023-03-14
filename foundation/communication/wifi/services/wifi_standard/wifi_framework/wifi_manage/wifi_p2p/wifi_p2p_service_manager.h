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
#ifndef OHOS_P2P_SERVICE_MANAGER_H
#define OHOS_P2P_SERVICE_MANAGER_H

#include <vector>
#include <map>
#include <mutex>
#include "wifi_p2p_service_request_list.h"
#include "wifi_p2p_service_response_list.h"

namespace OHOS {
namespace Wifi {
class WifiP2pServiceManager {
public:
    /**
     * @Description Construct a new WifiP2pServiceManager object.
     *
     */
    WifiP2pServiceManager();

    /**
     * @Description Destroy the WifiP2pServiceManager object.
     *
     */
    virtual ~WifiP2pServiceManager() = default;

    /**
     * @Description - Initialization service manager.
     *
     */
    virtual void Init();

    /**
     * @Description - Clear related records.
     */
    virtual void ClearAll();

    /**
     * @Description - Set queryId.
     * @param  setQueryId - queryId to be set
     */
    virtual void SetQueryId(const std::string &setQueryId);

    /**
     * @Description - Get queryId.
     * @return - const std::string& return the string of queryId
     */
    virtual const std::string &GetQueryId() const;

    /**
     * @Description Calculate and return the latest service transaction ID.
     * @return - unsigned char Latest returned service transaction ID after calculation
     */
    virtual unsigned char GetTransId();

    /**
     * @Description Add all responses to the device service discovery response list.
     *
     * @param p2pSvrRespList service response information list
     * @return true local response list is updated
     * @return false local response list is not updated
     */
    virtual bool AddServiceResponse(const WifiP2pServiceResponseList &p2pSvrRespList);

    /**
     * @Description Delete all responses from the device service discovery response list.
     *
     * @param p2pSvrRespList service response information list
     * @return true deleted successfully
     * @return false deletion failed
     */
    virtual bool RemoveServiceResponse(const WifiP2pServiceResponseList &p2pSvrRespList);

    /**
     * @Description - Deletes all matching responses based on the device address.
     * @param  deviceAddress - device address to be matched
     * @return - bool true: updated successfully   false: no update
     */
    virtual bool RemoveServiceResponse(const std::string &deviceAddress);

    /**
     * @Description Clear all service responses on all devices.
     *
     * @return true cleared successfully
     * @return false clearing failed
     */
    virtual bool ClearAllServiceResponse();

    /**
     * @Description Obtains all service discovery responses of all devices.
     *
     * @param respList a list of saving service discovery responses
     * @return true obtained successfully
     * @return false obtaining failed
     */
    virtual bool GetServiceResponseList(std::vector<WifiP2pServiceResponseList> &respList);

    /**
     * @Description - Updates response information on the device based on responseList.
     * @param rspList - service discovery responses
     * @return true: update    false: no update
     */
    virtual bool AddDeviceResponses(const WifiP2pServiceResponseList &rspList);

    /**
     * @Description - Updates the ServiceInfo information on the peer device based on the response.
     * @param rsp - service discovery response
     * @param dev - device information of response
     * @return true: update    false: no update
     */
    virtual bool AddDeviceService(const WifiP2pServiceResponse &rsp, const WifiP2pDevice &dev);

    /**
     * @Description - Deleting a saved peer device service.
     *
     * @param devAddr - peer device address
     * @return true: deleted successfully    false: deletion failed
     */
    virtual bool DelServicesFormAddress(const std::string &devAddr);

    /**
     * @Description Obtain services on all known devices.
     * @param serviceList - a list of saving all known services
     */
    virtual void GetDeviceServices(std::vector<WifiP2pServiceInfo> &services);

    /**
     * @Description - Sends the received request to a specific service. After processing the request,
                      the service returns a response.
     * @param  reqList - service request list
     * @return - WifiP2pServiceResponseList
     */
    virtual WifiP2pServiceResponseList ProcessServiceRequestList(const WifiP2pServiceRequestList &reqList);

    /**
     * @Description - Send the received response to the specific WifiP2pServiceInfo for processing.
     * @param  rspList - list of responses to be submitted to specific WifiP2pServiceInfo
     */
    virtual void ProcessServiceResponseList(const WifiP2pServiceResponseList &rspList) const;

    /**
     * @Description - Delete all discovered peer device services.
     *
     * @return true: deleted successfully    false: deletion failed
     */
    virtual bool ClearAllDeviceService();

    /**
     * @Description Adding a locally registered service.
     *
     * @param p2pSvrInfo registered service information
     * @return true adding succeeded
     * @return false adding failed (already exists)
     */
    virtual bool AddLocalService(const WifiP2pServiceInfo &p2pSvrInfo);
    /**
     * @Description Removing a locally registered service.
     *
     * @param p2pSvrInfo information about the removed service
     * @return true removal succeeded
     * @return false removal failed
     */
    virtual bool RemoveLocalService(const WifiP2pServiceInfo &p2pSvrInfo);
    /**
     * @Description Clearing all local registered services.
     *
     */
    virtual void ClearLocalService();

    /**
     * @Description - Check whether the service request has been recorded.
     * @param  deviceAddress - source address of the recorded request
     * @param  dialogToken - dialog ID of the recorded request
     * @return - bool true: recorded   false: not recorded
     */
    virtual bool IsRecordedRequest(const std::string &deviceAddress, int dialogToken);

    /**
     * @Description - Add a request record.
     * @param  deviceAddress - source address of the recorded request
     * @param  dialogToken - dialog ID of the recorded request
     */
    virtual void AddRequestRecord(const std::string &deviceAddress, int dialogToken);

    /**
     * @Description - Delete a request record.
     * @param  deviceAddress - source address of the recorded request
     * @param  dialogToken - dialog ID of the recorded request
     */
    virtual void RemoveRequestRecord(const std::string &deviceAddress, int dialogToken);

    /**
     * @Description - Remove all records.
     */
    virtual void ClearAllRequestRecord();
    /**
     * @Description Obtains the service information list of the local service.
     *
     * @return const std::vector<WifiP2pServiceInfo>&
     */
    virtual const std::vector<WifiP2pServiceInfo> &GetLocalServiceList();
    /**
     * @Description - Update the information about the corresponding service name based on the parameter.
     *
     * @param devAddr - device address
     * @param svrInfo - service info
     * @return true - updated successfully
     * @return false - update failed
     */
    virtual bool UpdateServiceName(const std::string &devAddr, const WifiP2pServiceResponse &svrInfo);

private:
    /* non-zero service transaction ID */
    unsigned char transId;
    /**
     * Indicates the reqID of a request record. This parameter is used to cancel a specified req. When a request is
     * responded to, this parameter needs to be cleared.
     */
    std::string queryId;
    /* Records the current request, which is used to intercept repeated requests. */
    std::map<std::string, std::vector<int>> requestRecord;
    /* services on devices discovered by the service */
    std::map<std::string, std::vector<WifiP2pServiceInfo>> deviceService;
    /* locally received responses from the peer end (target device + all responses from the target device) */
    std::map<std::string, WifiP2pServiceResponseList> serviceRespons;
    /* Locally registered services. Theoretically, all services are provided. */
    std::vector<WifiP2pServiceInfo> localServicesInfo;
    std::mutex serviceMutex;
};
} // namespace Wifi
} // namespace OHOS

#endif  /* OHOS_P2P_SERVICE_MANAGER_H */
