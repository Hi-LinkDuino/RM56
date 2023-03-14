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
#include "wifi_p2p_service_manager.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pServiceManager");

namespace OHOS {
namespace Wifi {
WifiP2pServiceManager::WifiP2pServiceManager() : transId(0), queryId(""), requestRecord(),
    deviceService(), serviceRespons(), localServicesInfo(), serviceMutex()
{}

void WifiP2pServiceManager::Init()
{
}

void WifiP2pServiceManager::ClearAll()
{
    ClearAllServiceResponse();
    ClearAllDeviceService();
    ClearLocalService();
}

void WifiP2pServiceManager::SetQueryId(const std::string &setQueryId)
{
    queryId = setQueryId;
}

const std::string &WifiP2pServiceManager::GetQueryId() const
{
    return queryId;
}

unsigned char WifiP2pServiceManager::GetTransId()
{
    transId++;
    if (transId == 0) {
        transId++;
    }
    return transId;
}

bool WifiP2pServiceManager::AddLocalService(const WifiP2pServiceInfo &p2pSvrInfo)
{
    for (auto iter = localServicesInfo.begin(); iter != localServicesInfo.end(); ++iter) {
        if (iter->GetServicerProtocolType() == p2pSvrInfo.GetServicerProtocolType() &&
            iter->GetQueryList() == p2pSvrInfo.GetQueryList() &&
            iter->GetDeviceAddress() == p2pSvrInfo.GetDeviceAddress()) {
            WIFI_LOGD("WifiP2pServiceInfo has been added when AddLocalService.");
            return false;
        }
    }
    localServicesInfo.push_back(p2pSvrInfo);
    return true;
}

bool WifiP2pServiceManager::RemoveLocalService(const WifiP2pServiceInfo &p2pSvrInfo)
{
    for (auto iter = localServicesInfo.begin(); iter != localServicesInfo.end(); ++iter) {
        if (iter->GetServicerProtocolType() == p2pSvrInfo.GetServicerProtocolType() &&
            iter->GetQueryList() == p2pSvrInfo.GetQueryList() &&
            iter->GetDeviceAddress() == p2pSvrInfo.GetDeviceAddress()) {
            localServicesInfo.erase(iter);
            return true;
        }
    }
    WIFI_LOGD("Cannot find p2pSvrInfo when RemoveLocalService.");
    return false;
}
void WifiP2pServiceManager::ClearLocalService()
{
    localServicesInfo.clear();
}
const std::vector<WifiP2pServiceInfo> &WifiP2pServiceManager::GetLocalServiceList()
{
    return localServicesInfo;
}

bool WifiP2pServiceManager::AddDeviceResponses(const WifiP2pServiceResponseList &rspList)
{
    const WifiP2pServiceResponseList responseList = rspList.FilterSerivceResponse(P2pServiceStatus::PSRS_SUCCESS);
    const WifiP2pDevice &device = responseList.GetDevice();

    auto iter = serviceRespons.find(device.GetDeviceAddress());
    if (iter != serviceRespons.end()) {
        WifiP2pServiceResponseList &deviceRespList = iter->second;
        if (!deviceRespList.MergerAndDeduplicate(responseList)) {
            WIFI_LOGE("Merge response error.");
            return false;
        }
    } else {
        serviceRespons.insert(
            std::pair<std::string, WifiP2pServiceResponseList>(device.GetDeviceAddress(), responseList));
    }
    return true;
}

bool WifiP2pServiceManager::AddDeviceService(const WifiP2pServiceResponse &rsp, const WifiP2pDevice &dev)
{
    if (rsp.GetServiceStatus() == P2pServiceStatus::PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE) {
        return false;
    }

    WifiP2pServiceInfo service;
    service.SetServicerProtocolType(rsp.GetProtocolType());
    service.SetDeviceAddress(dev.GetDeviceAddress());
    std::vector<std::string> queryList;
    std::string query;
    for (auto itRsp = rsp.GetData().begin(); itRsp != rsp.GetData().end(); ++itRsp) {
        query += *itRsp;
    }
    queryList.push_back(query);
    service.SetQueryList(queryList);

    std::unique_lock<std::mutex> lock(serviceMutex);
    auto iter = deviceService.find(dev.GetDeviceAddress());
    if (iter == deviceService.end()) {
        std::vector<WifiP2pServiceInfo> services;
        services.push_back(service);
        deviceService.insert(
            std::pair<std::string, std::vector<WifiP2pServiceInfo>>(dev.GetDeviceAddress(), services));
        return true;
    }

    std::vector<WifiP2pServiceInfo> &deviceServices = iter->second;
    for (auto devService = deviceServices.begin(); devService != deviceServices.end(); ++devService) {
        if (*devService == service) {
            return false;
        }
    }
    deviceServices.push_back(service);

    return true;
}

bool WifiP2pServiceManager::DelServicesFormAddress(const std::string &devAddr)
{
    std::unique_lock<std::mutex> lock(serviceMutex);
    auto it = deviceService.find(devAddr);
    if (it == deviceService.end()) {
        WIFI_LOGD("no svr to del.");
        return false;
    }
    deviceService.erase(it);
    return true;
}

bool WifiP2pServiceManager::ClearAllDeviceService()
{
    std::unique_lock<std::mutex> lock(serviceMutex);
    deviceService.clear();
    return true;
}

void WifiP2pServiceManager::GetDeviceServices(std::vector<WifiP2pServiceInfo> &services)
{
    std::unique_lock<std::mutex> lock(serviceMutex);
    for (auto mapIter = deviceService.begin(); mapIter != deviceService.end(); ++mapIter) {
        for (auto svrInfoIter = mapIter->second.begin(); svrInfoIter != mapIter->second.end(); ++svrInfoIter) {
            WifiP2pServiceInfo buf;
            buf.SetDeviceAddress(svrInfoIter->GetDeviceAddress());
            buf.SetServicerProtocolType(svrInfoIter->GetServicerProtocolType());
            buf.SetServiceName(svrInfoIter->GetServiceName());
            buf.SetQueryList(svrInfoIter->GetQueryList());
            services.push_back(buf);
        }
    }
}

bool WifiP2pServiceManager::AddServiceResponse(const WifiP2pServiceResponseList &p2pSvrRespList)
{
    auto findIter = serviceRespons.find(p2pSvrRespList.GetDevice().GetDeviceAddress());
    if (findIter == serviceRespons.end()) {
        serviceRespons.emplace(std::make_pair(p2pSvrRespList.GetDevice().GetDeviceAddress(), p2pSvrRespList));
        return true;
    }

    bool bUpdate = false;
    for (auto addedRespIter = p2pSvrRespList.GetServiceResponseList().begin();
         addedRespIter != p2pSvrRespList.GetServiceResponseList().end(); ++addedRespIter) {
        bool equal = false;
        for (auto foundRespIter = findIter->second.GetServiceResponseList().begin();
             foundRespIter != findIter->second.GetServiceResponseList().end(); ++foundRespIter) {
            if (*addedRespIter == *foundRespIter) {
                equal = true;
                break;
            }
        }
        if (!equal) {
            findIter->second.AddServiceResponse(*addedRespIter);
            bUpdate = true;
        }
    }
    return bUpdate;
}

bool WifiP2pServiceManager::RemoveServiceResponse(const WifiP2pServiceResponseList &p2pSvrRespList)
{
    auto findIter = serviceRespons.find(p2pSvrRespList.GetDevice().GetDeviceAddress());
    if (findIter == serviceRespons.end()) {
        WIFI_LOGE("Cannot find %{private}s respList", p2pSvrRespList.GetDevice().GetDeviceAddress().c_str());
        return false;
    }

    for (auto delRespIter = p2pSvrRespList.GetServiceResponseList().begin();
         delRespIter != p2pSvrRespList.GetServiceResponseList().end(); ++delRespIter) {
        bool found = false;
        for (auto foundRespIter = findIter->second.GetServiceResponseList().begin();
             foundRespIter != findIter->second.GetServiceResponseList().end(); ++foundRespIter) {
            if (*delRespIter == *foundRespIter) {
                found = true;
                findIter->second.RemoveServiceResponse(*delRespIter);
                break;
            }
        }
        if (!found) {
            WIFI_LOGE("No %{public}d resp in stored %{private}s",
                delRespIter->GetTransactionId(),
                p2pSvrRespList.GetDevice().GetDeviceAddress().c_str());
        }
    }
    return true;
}

bool WifiP2pServiceManager::RemoveServiceResponse(const std::string &deviceAddress)
{
    auto findIter = serviceRespons.find(deviceAddress);
    if (findIter == serviceRespons.end()) {
        return false;
    } else {
        serviceRespons.erase(findIter);
        return true;
    }
}

bool WifiP2pServiceManager::ClearAllServiceResponse()
{
    serviceRespons.clear();
    return true;
}

bool WifiP2pServiceManager::GetServiceResponseList(std::vector<WifiP2pServiceResponseList> &respList)
{
    for (auto respListIter = respList.begin(); respListIter != respList.end(); ++respListIter) {
        auto findIter = serviceRespons.find(respListIter->GetDevice().GetDeviceAddress());
        if (findIter != serviceRespons.end()) {
            *respListIter = findIter->second;
        } else {
            WIFI_LOGE("Cannot get %{private}s responseList", respListIter->GetDevice().GetDeviceAddress().c_str());
        }
    }
    return true;
}

WifiP2pServiceResponseList WifiP2pServiceManager::ProcessServiceRequestList(const WifiP2pServiceRequestList &reqList)
{
    WifiP2pServiceResponseList responseList;
    responseList.SetDevice(reqList.GetDevice());
    responseList.SetFrequency(reqList.GetFrequency());
    responseList.SetDialogToken(reqList.GetDialogToken());
    const std::vector<WifiP2pServiceRequest> &list = reqList.GetServiceRequestList();

    for (const auto &request : list) {
        bool isFind = false;
        WifiP2pServiceResponse response;
        response.SetTransactionId(request.GetTransactionId());

        for (const auto &service : localServicesInfo) {
            if (request.GetProtocolType() == service.GetServicerProtocolType() ||
                request.GetProtocolType() == P2pServicerProtocolType::SERVICE_TYPE_ALL) {
                isFind = true;
                response.SetProtocolType(service.GetServicerProtocolType());
                std::vector<unsigned char> data;
                P2pServiceStatus status = service.ProcessServiceRequest(request.GetQuery(), data);
                response.SetServiceStatus(status);
                response.SetData(data);
                responseList.AddServiceResponse(response);
            }
        }

        if (!isFind) {
            response.SetProtocolType(request.GetProtocolType());
            response.SetServiceStatus(P2pServiceStatus::PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE);
            responseList.AddServiceResponse(response);
        }
    }

    return responseList;
}

void WifiP2pServiceManager::ProcessServiceResponseList(const WifiP2pServiceResponseList &rspList) const
{
    const std::vector<WifiP2pServiceResponse> &list = rspList.GetServiceResponseList();

    for (const auto &response : list) {
        for (const auto &service : localServicesInfo) {
            if (response.GetProtocolType() == service.GetServicerProtocolType()) {
                service.ProcessServiceResponse(response.GetData());
            }
        }
    }
}

bool WifiP2pServiceManager::IsRecordedRequest(const std::string &deviceAddress, int dialogToken)
{
    auto findIter = requestRecord.find(deviceAddress);
    if (findIter == requestRecord.end()) {
        return false;
    }
    for (auto diaIter = findIter->second.begin(); diaIter != findIter->second.end(); ++diaIter) {
        if (*diaIter == dialogToken) {
            return true;
        }
    }
    return false;
}

void WifiP2pServiceManager::AddRequestRecord(const std::string &deviceAddress, int dialogToken)
{
    auto findIter = requestRecord.find(deviceAddress);
    if (findIter == requestRecord.end()) {
        std::vector<int> dias;
        dias.push_back(dialogToken);
        requestRecord.emplace(std::make_pair(deviceAddress, dias));
        return;
    }
    for (auto diaIter = findIter->second.begin(); diaIter != findIter->second.end(); ++diaIter) {
        if (*diaIter == dialogToken) {
            return;
        }
    }
    findIter->second.push_back(dialogToken);
    return;
}

void WifiP2pServiceManager::RemoveRequestRecord(const std::string &deviceAddress, int dialogToken)
{
    auto findIter = requestRecord.find(deviceAddress);
    if (findIter != requestRecord.end()) {
        for (auto diaIter = findIter->second.begin(); diaIter != findIter->second.end(); ++diaIter) {
            if (*diaIter == dialogToken) {
                findIter->second.erase(diaIter);
                return;
            }
        }
    }
    return;
}

void WifiP2pServiceManager::ClearAllRequestRecord()
{
    requestRecord.clear();
}

bool WifiP2pServiceManager::UpdateServiceName(const std::string &devAddr, const WifiP2pServiceResponse &svrInfo)
{
    auto iter = deviceService.find(devAddr);
    if (iter == deviceService.end()) {
        WIFI_LOGE("Cannot find %{public}s, update service name failed!", devAddr.c_str());
        return false;
    }

    std::string tlvString(svrInfo.GetData().begin(), svrInfo.GetData().end());
    std::vector<WifiP2pServiceInfo> &svrInfos = iter->second;
    for (auto iterSvrInfo = svrInfos.begin(); iterSvrInfo != svrInfos.end(); ++iterSvrInfo) {
        if (iterSvrInfo->GetServicerProtocolType() == svrInfo.GetProtocolType() &&
            iterSvrInfo->GetDeviceAddress() == devAddr && iterSvrInfo->GetQueryList().at(0) == tlvString) {
            iterSvrInfo->SetServiceName(svrInfo.GetServiceName());
            return true;
        }
    }

    WIFI_LOGE("Cannot find same service info, update service name failed!");
    return false;
}
} // namespace Wifi
} // namespace OHOS
