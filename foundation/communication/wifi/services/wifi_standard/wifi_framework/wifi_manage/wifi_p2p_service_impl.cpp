/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "wifi_p2p_service_impl.h"
#include <file_ex.h>
#include "wifi_permission_utils.h"
#include "wifi_auth_center.h"
#include "wifi_config_center.h"
#include "wifi_manager.h"
#include "wifi_service_manager.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_logger.h"
#include "define.h"
#include "wifi_dumper.h"
#include "wifi_hid2d_service_utils.h"
#include "if_config.h"
#include "wifi_hid2d_cfg.h"
#include "wifi_net_agent.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pServiceImpl");

namespace OHOS {
namespace Wifi {
std::mutex WifiP2pServiceImpl::instanceLock;
sptr<WifiP2pServiceImpl> WifiP2pServiceImpl::instance;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(WifiP2pServiceImpl::GetInstance().GetRefPtr());

sptr<WifiP2pServiceImpl> WifiP2pServiceImpl::GetInstance()
{
    if (instance == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock);
        if (instance == nullptr) {
            auto service = new (std::nothrow) WifiP2pServiceImpl;
            instance = service;
        }
    }
    return instance;
}

WifiP2pServiceImpl::WifiP2pServiceImpl()
    : SystemAbility(WIFI_P2P_ABILITY_ID, true), mPublishFlag(false), mState(ServiceRunningState::STATE_NOT_START)
{}

WifiP2pServiceImpl::~WifiP2pServiceImpl()
{}

void WifiP2pServiceImpl::OnStart()
{
    if (mState == ServiceRunningState::STATE_RUNNING) {
        WIFI_LOGD("P2p service has already started.");
        return;
    }
    if (!Init()) {
        WIFI_LOGE("Failed to init p2p service");
        OnStop();
        return;
    }
    mState = ServiceRunningState::STATE_RUNNING;
    WIFI_LOGI("Start p2p service!");
    WifiManager::GetInstance().AddSupportedFeatures(WifiFeatures::WIFI_FEATURE_P2P);
}

void WifiP2pServiceImpl::OnStop()
{
    mState = ServiceRunningState::STATE_NOT_START;
    mPublishFlag = false;
    WIFI_LOGI("Stop p2p service!");
}

bool WifiP2pServiceImpl::Init()
{
    if (!mPublishFlag) {
        bool ret = Publish(WifiP2pServiceImpl::GetInstance());
        if (!ret) {
            WIFI_LOGE("Failed to publish p2p service!");
            return false;
        }
        mPublishFlag = true;
    }
    return true;
}

ErrCode WifiP2pServiceImpl::CheckCanEnableP2p(void)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("EnableP2p:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
        WIFI_LOGD("current airplane mode and can not use p2p, open failed!");
        return WIFI_OPT_FORBID_AIRPLANE;
    }
    if (WifiConfigCenter::GetInstance().GetPowerSavingModeState() == 1) {
        WIFI_LOGD("current power saving mode and can not use p2p, open failed!");
        return WIFI_OPT_FORBID_POWSAVING;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::EnableP2p(void)
{
    WIFI_LOGI("EnableP2p");
    ErrCode errCode = CheckCanEnableP2p();
    if (errCode != WIFI_OPT_SUCCESS) {
        return errCode;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetP2pMidState();
    if (curState != WifiOprMidState::CLOSED) {
        WIFI_LOGD("current p2p state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::CLOSING) {
            return WIFI_OPT_OPEN_FAIL_WHEN_CLOSING;
        } else {
            return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
        }
    }
    if (!WifiConfigCenter::GetInstance().SetP2pMidState(curState, WifiOprMidState::OPENING)) {
        WIFI_LOGD("set p2p mid state opening failed! may be other activity has been operated");
        return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
    }
    ErrCode ret = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_P2P) < 0) {
            WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_P2P);
            break;
        }
        IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_P2P);
            break;
        }
        ret = pService->RegisterP2pServiceCallbacks(WifiManager::GetInstance().GetP2pCallback());
        if (ret != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register p2p service callback failed!");
            break;
        }
        ret = pService->EnableP2p();
        if (ret != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("service EnableP2p failed, ret %{public}d!", static_cast<int>(ret));
            break;
        }
    } while (false);
    if (ret != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_P2P);
    }
    return ret;
}

ErrCode WifiP2pServiceImpl::DisableP2p(void)
{
    WIFI_LOGI("DisableP2p");
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableP2p:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetP2pMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGD("current p2p state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::OPENING) {
            return WIFI_OPT_CLOSE_FAIL_WHEN_OPENING;
        } else {
            return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
        }
    }
    if (!WifiConfigCenter::GetInstance().SetP2pMidState(curState, WifiOprMidState::CLOSING)) {
        WIFI_LOGD("set p2p mid state opening failed! may be other activity has been operated");
        return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
    }
    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_P2P);
        return WIFI_OPT_SUCCESS;
    }
    ErrCode ret = pService->DisableP2p();
    if (ret != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::CLOSING, WifiOprMidState::RUNNING);
    }
    return ret;
}

ErrCode WifiP2pServiceImpl::DiscoverDevices(void)
{
    WIFI_LOGI("DiscoverDevices");
    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DiscoverDevices:VerifyGetWifiInfoInternalPermission PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("DiscoverDevices:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("DiscoverDevices:VerifyGetScanInfosPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->DiscoverDevices();
}

ErrCode WifiP2pServiceImpl::StopDiscoverDevices(void)
{
    WIFI_LOGI("StopDiscoverDevices");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("StopDiscoverDevices:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->StopDiscoverDevices();
}

ErrCode WifiP2pServiceImpl::DiscoverServices(void)
{
    WIFI_LOGI("DiscoverServices");
    if (WifiPermissionUtils::VerifyGetWifiDirectDevicePermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DiscoverServices:VerifyGetWifiDirectDevicePermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->DiscoverServices();
}

ErrCode WifiP2pServiceImpl::StopDiscoverServices(void)
{
    WIFI_LOGI("StopDiscoverServices");
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->StopDiscoverServices();
}

ErrCode WifiP2pServiceImpl::RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request)
{
    WIFI_LOGI("RequestService");
    if (WifiPermissionUtils::VerifyGetWifiDirectDevicePermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RequestService:VerifyGetWifiDirectDevicePermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->RequestService(device, request);
}

ErrCode WifiP2pServiceImpl::PutLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    WIFI_LOGI("PutLocalP2pService, service name is [%{public}s]", srvInfo.GetServiceName().c_str());
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->PutLocalP2pService(srvInfo);
}

ErrCode WifiP2pServiceImpl::DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    WIFI_LOGI("DeleteLocalP2pService, service name is [%{public}s]", srvInfo.GetServiceName().c_str());
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->DeleteLocalP2pService(srvInfo);
}

ErrCode WifiP2pServiceImpl::StartP2pListen(int period, int interval)
{
    WIFI_LOGI("StartP2pListen, period %{public}d, interval %{public}d", period, interval);
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("StartP2pListen:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->StartP2pListen(period, interval);
}

ErrCode WifiP2pServiceImpl::StopP2pListen()
{
    WIFI_LOGI("StopP2pListen");
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("StopP2pListen:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->StopP2pListen();
}

ErrCode WifiP2pServiceImpl::CreateGroup(const WifiP2pConfig &config)
{
    WIFI_LOGI("CreateGroup, network name is [%{public}s]", config.GetGroupName().c_str());
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("CreateGroup:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->CreateGroup(config);
}

ErrCode WifiP2pServiceImpl::RemoveGroup()
{
    WIFI_LOGI("RemoveGroup");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RemoveGroup:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->RemoveGroup();
}

ErrCode WifiP2pServiceImpl::DeleteGroup(const WifiP2pGroupInfo &group)
{
    WIFI_LOGI("DeleteGroup, group name [%{public}s]", group.GetGroupName().c_str());
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DeleteGroup:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DeleteGroup:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->DeleteGroup(group);
}

ErrCode WifiP2pServiceImpl::P2pConnect(const WifiP2pConfig &config)
{
    WIFI_LOGI("P2pConnect device address [%{private}s]", config.GetDeviceAddress().c_str());
    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("P2pConnect:VerifyGetWifiInfoInternalPermission PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("P2pConnect:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("P2pConnect:VerifyGetScanInfosPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->P2pConnect(config);
}

ErrCode WifiP2pServiceImpl::P2pCancelConnect()
{
    WIFI_LOGI("P2pCancelConnect");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("P2pCancelConnect:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->P2pCancelConnect();
}

ErrCode WifiP2pServiceImpl::QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo)
{
    WIFI_LOGI("QueryP2pLinkedInfo group owner address [%{private}s]", linkedInfo.GetGroupOwnerAddress().c_str());
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("QueryP2pLinkedInfo:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    ErrCode ret = pService->QueryP2pLinkedInfo(linkedInfo);
    if (ret == WIFI_OPT_SUCCESS) {
        if (WifiPermissionUtils::VerifyGetWifiLocalMacPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("QueryP2pLinkedInfo:VerifyGetWifiLocalMacPermission PERMISSION_DENIED!");
            linkedInfo.SetIsGroupOwnerAddress("00.00.00.00");
        }
    }

    return ret;
}

ErrCode WifiP2pServiceImpl::GetCurrentGroup(WifiP2pGroupInfo &group)
{
    WIFI_LOGI("GetCurrentGroup");

    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetCurrentGroup:VerifyGetWifiInfoInternalPermission PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetCurrentGroup:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetCurrentGroup:VerifyGetScanInfosPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->GetCurrentGroup(group);
}

ErrCode WifiP2pServiceImpl::GetP2pEnableStatus(int &status)
{
    WIFI_LOGI("GetP2pEnableStatus");
    status = WifiConfigCenter::GetInstance().GetP2pState();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::GetP2pDiscoverStatus(int &status)
{
    WIFI_LOGI("GetP2pDiscoverStatus");
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->GetP2pDiscoverStatus(status);
}

ErrCode WifiP2pServiceImpl::GetP2pConnectedStatus(int &status)
{
    WIFI_LOGI("GetP2pConnectedStatus");
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->GetP2pConnectedStatus(status);
}

ErrCode WifiP2pServiceImpl::QueryP2pDevices(std::vector<WifiP2pDevice> &devives)
{
    WIFI_LOGI("QueryP2pDevices");
    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("QueryP2pDevices:VerifyGetWifiInfoInternalPermission PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("QueryP2pDevices:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("QueryP2pDevices:VerifyGetScanInfosPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->QueryP2pDevices(devives);
}

ErrCode WifiP2pServiceImpl::QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups)
{
    WIFI_LOGI("QueryP2pGroups");
    if (WifiPermissionUtils::VerifyGetWifiDirectDevicePermission() == PERMISSION_DENIED) {
        WIFI_LOGE("QueryP2pGroups:VerifyGetWifiDirectDevicePermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->QueryP2pGroups(groups);
}

ErrCode WifiP2pServiceImpl::QueryP2pServices(std::vector<WifiP2pServiceInfo> &services)
{
    WIFI_LOGI("QueryP2pServices");
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->QueryP2pServices(services);
}

ErrCode WifiP2pServiceImpl::RegisterCallBack(const sptr<IWifiP2pCallback> &callback)
{
    WIFI_LOGI("WifiP2pServiceImpl::RegisterCallBack!");
    WifiInternalEventDispatcher::GetInstance().SetSingleP2pCallback(callback);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::GetSupportedFeatures(long &features)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetSupportedFeatures:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    int ret = WifiManager::GetInstance().GetSupportedFeatures(features);
    if (ret < 0) {
        WIFI_LOGE("Failed to get supported features!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

bool WifiP2pServiceImpl::IsP2pServiceRunning()
{
    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetP2pMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGD("p2p service does not started!");
        return false;
    }
    return true;
}

ErrCode WifiP2pServiceImpl::SetP2pDeviceName(const std::string &deviceName)
{
    WIFI_LOGI("SetP2pDeviceName:%{public}s", deviceName.c_str());
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("SetP2pDeviceName:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("SetP2pDeviceName:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    int length = deviceName.length();
    if (length > DEVICE_NAME_LENGTH || length < 0) {
        return WIFI_OPT_INVALID_PARAM;
    }
    WifiConfigCenter::GetInstance().SetP2pDeviceName(deviceName);
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_SUCCESS;
    }
    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->SetP2pDeviceName(deviceName);
}

ErrCode WifiP2pServiceImpl::SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo)
{
    WIFI_LOGI("SetP2pWfdInfo");
    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->SetP2pWfdInfo(wfdInfo);
}

ErrCode WifiP2pServiceImpl::Hid2dRequestGcIp(const std::string& gcMac, std::string& ipAddr)
{
    WIFI_LOGI("Hid2dRequestGcIp");

    WifiP2pGroupInfo group;
    ErrCode ret = GetCurrentGroup(group);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGI("Apply IP get current group failed!");
    }

    IpPool::InitIpPool(group.GetGoIpAddress());
    ipAddr = IpPool::GetIp(gcMac);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dSharedlinkIncrease()
{
    WIFI_LOGI("Hid2dSharedlinkIncrease");
    int status = static_cast<int>(P2pConnectedState::P2P_DISCONNECTED);
    ErrCode ret = GetP2pConnectedStatus(status);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGI("Hid2dSharedlinkIncrease get P2P connect status error!");
        return ret;
    }
    if (status != static_cast<int>(P2pConnectedState::P2P_CONNECTED)) {
        WIFI_LOGE("Hid2dSharedlinkIncrease P2P not in connected state!");
        return WIFI_OPT_FAILED;
    }
    SharedLinkManager::IncreaseSharedLink();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dSharedlinkDecrease()
{
    WIFI_LOGI("Hid2dSharedlinkDecrease");
    SharedLinkManager::DecreaseSharedLink();
    if (SharedLinkManager::GetSharedLinkCount() == 0) {
        WIFI_LOGI("Shared link count == 0, remove group!");
        RemoveGroup();
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dCreateGroup(const int frequency, FreqType type)
{
    WIFI_LOGI("Hid2dCreateGroup");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("CreateGroup:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->Hid2dCreateGroup(frequency, type);
}

ErrCode WifiP2pServiceImpl::Hid2dRemoveGcGroup(const std::string& gcIfName)
{
    WIFI_LOGI("Hid2dRemoveGcGroup:, gcIfName: %{public}s", gcIfName.c_str());
    // TO Imple: delete by interface
    return RemoveGroup();
}

ErrCode WifiP2pServiceImpl::Hid2dConnect(const Hid2dConnectConfig& config)
{
    WIFI_LOGI("Hid2dConnect");
    if (WifiPermissionUtils::VerifyGetWifiDirectDevicePermission() == PERMISSION_DENIED) {
        WIFI_LOGE("Hid2dConnect:VerifyGetWifiDirectDevicePermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsP2pServiceRunning()) {
        WIFI_LOGE("P2pService is not runing!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }

    IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("Get P2P service failed!");
        return WIFI_OPT_P2P_NOT_OPENED;
    }
    return pService->Hid2dConnect(config);
}

ErrCode WifiP2pServiceImpl::Hid2dConfigIPAddr(const std::string& ifName, const IpAddrInfo& ipInfo)
{
    WIFI_LOGI("Hid2dConfigIPAddr, ifName: %{public}s", ifName.c_str());
    IfConfig::GetInstance().AddIpAddr(ifName, ipInfo.ip, ipInfo.netmask, IpType::IPTYPE_IPV4);
    WifiNetAgent::GetInstance().AddRoute(ifName, ipInfo.ip, IpTools::GetMaskLength(ipInfo.netmask));
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dReleaseIPAddr(const std::string& ifName)
{
    WIFI_LOGI("Hid2dReleaseIPAddr");
    IfConfig::GetInstance().FlushIpAddr(ifName, IpType::IPTYPE_IPV4);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dGetRecommendChannel(const RecommendChannelRequest& request,
    RecommendChannelResponse& response)
{
    WIFI_LOGI("Hid2dGetRecommendChannel");

    /*
     * channel: 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
     * center frequency: 5180, 5200, 5220, 5240, 5260, 5280, 5300, 5320, 5745, 5765, 5785, 5805, 5825
    */
    constexpr int defaultRecommendFrequency = 5180;
    response.status = RecommendStatus::RS_SUCCESS;
    response.centerFreq2 = defaultRecommendFrequency;
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dGetChannelListFor5G(std::vector<int>& vecChannelList)
{
    WIFI_LOGI("Hid2dGetChannelListFor5G");

    std::vector<int> temp5Glist = {36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165};
    vecChannelList.clear();
    std::swap(temp5Glist, vecChannelList);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen)
{
    WIFI_LOGI("Hid2dGetSelfWifiCfgInfo");
    WifiHid2dCfg::GetInstance().GetSelfDeviceCfg(cfgType, cfgData, *getDatValidLen);
    return (*getDatValidLen == 0) ? WIFI_OPT_FAILED : WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pServiceImpl::Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen)
{
    WIFI_LOGI("Hid2dSetPeerWifiCfgInfo");
    int ret = WifiHid2dCfg::GetInstance().Hid2dSetPeerWifiCfgInfo(cfgType, cfgData, setDataValidLen);
    if (ret != 0) {
        WIFI_LOGE("set peer wifi cfg info failed: %{public}d", ret);
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

void WifiP2pServiceImpl::SaBasicDump(std::string& result)
{
    result.append("P2P enable status: ");
    int status = WifiConfigCenter::GetInstance().GetP2pState();
    std::string strStatus = (status == static_cast<int>(P2pState::P2P_STATE_STARTED)) ? "enable" : "disable";
    result.append(strStatus);
    result += "\n";
}

int32_t WifiP2pServiceImpl::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::vector<std::string> vecArgs;
    std::transform(args.begin(), args.end(), std::back_inserter(vecArgs), [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    WifiDumper dumper;
    std::string result;
    dumper.P2pDump(SaBasicDump, vecArgs, result);
    if (!SaveStringToFd(fd, result)) {
        WIFI_LOGE("WiFi P2p save string to fd failed.");
        return ERR_OK;
    }
    return ERR_OK;
}
}  // namespace Wifi
}  // namespace OHOS