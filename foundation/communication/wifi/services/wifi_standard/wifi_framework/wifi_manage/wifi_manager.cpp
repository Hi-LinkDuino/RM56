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

#include "wifi_manager.h"
#include "wifi_global_func.h"
#include "wifi_logger.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_chip_hal_interface.h"
#include "wifi_auth_center.h"
#include "wifi_config_center.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_service_manager.h"
#include "wifi_settings.h"
#include "wifi_common_event_helper.h"

DEFINE_WIFILOG_LABEL("WifiManager");

namespace OHOS {
namespace Wifi {
WifiManager &WifiManager::GetInstance()
{
    static WifiManager gWifiManager;
    static std::mutex gInitMutex;
    if (gWifiManager.GetInitStatus() == INIT_UNKNOWN) {
        std::unique_lock<std::mutex> lock(gInitMutex);
        if (gWifiManager.GetInitStatus() == INIT_UNKNOWN) {
            if (gWifiManager.Init() != 0) {
                WIFI_LOGE("Failed to `WifiManager::Init` !");
            }
        }
    }

    return gWifiManager;
}

WifiManager::WifiManager() : mInitStatus(INIT_UNKNOWN), mSupportedFeatures(0)
{}

WifiManager::~WifiManager()
{}

void WifiManager::AutoStartStaService(void)
{
    WifiOprMidState staState = WifiConfigCenter::GetInstance().GetWifiMidState();
    if (staState == WifiOprMidState::CLOSED) {
        if (!WifiConfigCenter::GetInstance().SetWifiMidState(staState, WifiOprMidState::OPENING)) {
            WIFI_LOGD("set sta mid state opening failed! may be other activity has been operated");
            return;
        }
        ErrCode errCode = WIFI_OPT_FAILED;
        do {
            if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_STA) < 0) {
                WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_STA);
                break;
            }
            IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
            if (pService == nullptr) {
                WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_STA);
                break;
            }
            errCode = pService->RegisterStaServiceCallback(WifiManager::GetInstance().GetStaCallback());
            if (errCode != WIFI_OPT_SUCCESS) {
                WIFI_LOGE("Register sta service callback failed!");
                break;
            }
            errCode = pService->EnableWifi();
            if (errCode != WIFI_OPT_SUCCESS) {
                WIFI_LOGE("service enable sta failed, ret %{public}d!", static_cast<int>(errCode));
                break;
            }
        } while (0);
        if (errCode != WIFI_OPT_SUCCESS) {
            WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
            WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
        }
    }
    return;
}

void WifiManager::AutoStartP2pService(void)
{
    WifiOprMidState p2pState = WifiConfigCenter::GetInstance().GetP2pMidState();
    if (p2pState == WifiOprMidState::CLOSED) {
        if (!WifiConfigCenter::GetInstance().SetP2pMidState(p2pState, WifiOprMidState::OPENING)) {
            WIFI_LOGD("set p2p mid state opening failed!");
            return;
        }
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
    return;
}

void WifiManager::AutoStartScanService(void)
{
    if (!WifiConfigCenter::GetInstance().IsScanAlwaysActive()) {
        WIFI_LOGD("Scan always is not open, not open scan service.");
        return;
    }
    ScanControlInfo info;
    WifiConfigCenter::GetInstance().GetScanControlInfo(info);
    if (!IsAllowScanAnyTime(info)) {
        WIFI_LOGD("Scan control does not support scan always, not open scan service here.");
        return;
    }
    CheckAndStartScanService();
    return;
}

int WifiManager::Init()
{
    if (WifiConfigCenter::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiConfigCenter Init failed!");
        mInitStatus = CONFIG_CENTER_INIT_FAILED;
        return -1;
    }
    if (WifiAuthCenter::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiAuthCenter Init failed!");
        mInitStatus = AUTH_CENTER_INIT_FAILED;
        return -1;
    }
    if (WifiServiceManager::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiServiceManager Init failed!");
        mInitStatus = SERVICE_MANAGER_INIT_FAILED;
        return -1;
    }
    if (WifiInternalEventDispatcher::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiInternalEventDispatcher Init failed!");
        mInitStatus = EVENT_BROADCAST_INIT_FAILED;
        return -1;
    }
    mCloseServiceThread = std::thread(WifiManager::DealCloseServiceMsg, std::ref(*this));

    mInitStatus = INIT_OK;
    InitStaCallback();
    InitScanCallback();
    InitApCallback();
    InitP2pCallback();
    if (!WifiConfigCenter::GetInstance().GetSupportedBandChannel()) {
        WIFI_LOGE("Failed to get current chip supported band and channel!");
    }

    if (WifiServiceManager::GetInstance().CheckPreLoadService() < 0) {
        WIFI_LOGE("WifiServiceManager check preload feature service failed!");
        WifiManager::GetInstance().Exit();
        return -1;
    }
    if (WifiConfigCenter::GetInstance().GetStaLastRunState()) { /* Automatic startup upon startup */
        WIFI_LOGE("AutoStartStaApService");
        AutoStartP2pService();
        AutoStartStaService();
    } else {
        /**
         * The sta service automatically starts upon startup. After the sta
         * service is started, the scanning is directly started.
         */
        WIFI_LOGE("AutoStartScanService");
        AutoStartScanService();
    }
    return 0;
}

void WifiManager::Exit()
{
    WifiServiceManager::GetInstance().UninstallAllService();
    WifiStaHalInterface::GetInstance().ExitAllIdlClient();
    WifiInternalEventDispatcher::GetInstance().Exit();
    if (mCloseServiceThread.joinable()) {
        PushServiceCloseMsg(WifiCloseServiceCode::SERVICE_THREAD_EXIT);
        mCloseServiceThread.join();
    }
    return;
}

void WifiManager::PushServiceCloseMsg(WifiCloseServiceCode code)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mEventQue.push_back(code);
    mCondition.notify_one();
    return;
}

void WifiManager::AddSupportedFeatures(WifiFeatures feature)
{
    mSupportedFeatures |= static_cast<long>(feature);
}

int WifiManager::GetSupportedFeatures(long &features)
{
    int capability = 0;
    if (WifiChipHalInterface::GetInstance().GetChipCapabilities(capability) != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("Failed to get chip capability!");
        return -1;
    }
    long supportedFeatures = mSupportedFeatures;
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_INFRA);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_INFRA_5G);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_PASSPOINT);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_AP_STA);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_WPA3_SAE);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_WPA3_SUITE_B);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_OWE);
    features = (supportedFeatures & capability);
    return 0;
}

InitStatus WifiManager::GetInitStatus()
{
    return mInitStatus;
}

void WifiManager::CloseStaService(void)
{
    WIFI_LOGD("close sta service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
    WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::CLOSED);
    WifiConfigCenter::GetInstance().SetWifiStaCloseTime();
    return;
}

void WifiManager::CloseApService(void)
{
    WIFI_LOGD("close ap service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_AP);
    WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::CLOSED);
    WifiSettings::GetInstance().SetHotspotState(static_cast<int>(ApState::AP_STATE_CLOSED));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(ApState::AP_STATE_CLOSED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::CloseScanService(void)
{
    WIFI_LOGD("close scan service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_SCAN);
    WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::CLOSED);
    return;
}

void WifiManager::CloseP2pService(void)
{
    WIFI_LOGD("close p2p service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_P2P);
    WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::CLOSED);
    WifiSettings::GetInstance().SetP2pState(static_cast<int>(P2pState::P2P_STATE_CLOSED));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(P2pState::P2P_STATE_CLOSED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealCloseServiceMsg(WifiManager &manager)
{
    const int waitDealTime = 10 * 1000; /* 10 ms */
    while (true) {
        std::unique_lock<std::mutex> lock(manager.mMutex);
        while (manager.mEventQue.empty()) {
            manager.mCondition.wait(lock);
        }
        WifiCloseServiceCode msg = manager.mEventQue.front();
        manager.mEventQue.pop_front();
        lock.unlock();
        usleep(waitDealTime);
        switch (msg) {
            case WifiCloseServiceCode::STA_SERVICE_CLOSE:
                CloseStaService();
                break;
            case WifiCloseServiceCode::SCAN_SERVICE_CLOSE:
                CloseScanService();
                break;
            case WifiCloseServiceCode::AP_SERVICE_CLOSE:
                CloseApService();
                break;
            case WifiCloseServiceCode::P2P_SERVICE_CLOSE:
                CloseP2pService();
                break;
            case WifiCloseServiceCode::SERVICE_THREAD_EXIT:
                WIFI_LOGD("DealCloseServiceMsg thread exit!");
                return;
            default:
                WIFI_LOGD("Unknown message code, %{public}d", static_cast<int>(msg));
                break;
        }
    }
    WIFI_LOGD("WifiManager Thread exit");
    return;
}

void WifiManager::InitStaCallback(void)
{
    mStaCallback.OnStaOpenRes = DealStaOpenRes;
    mStaCallback.OnStaCloseRes = DealStaCloseRes;
    mStaCallback.OnStaConnChanged = DealStaConnChanged;
    mStaCallback.OnWpsChanged = DealWpsChanged;
    mStaCallback.OnStaStreamChanged = DealStreamChanged;
    mStaCallback.OnStaRssiLevelChanged = DealRssiChanged;
    return;
}

StaServiceCallback WifiManager::GetStaCallback()
{
    return mStaCallback;
}

void WifiManager::DealStaOpenRes(OperateResState state)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
    if (state == OperateResState::OPEN_WIFI_FAILED) {
        WIFI_LOGD("DealStaOpenRes:upload wifi open failed event!");
        cbMsg.msgData = static_cast<int>(WifiState::UNKNOWN);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    } else if (state == OperateResState::OPEN_WIFI_OPENING) {
        cbMsg.msgData = static_cast<int>(WifiState::ENABLING);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    } else if (state == OperateResState::OPEN_WIFI_DISABLED) {
        WIFI_LOGD("DealStaOpenRes:wifi open failed,close wifi sta service!");
        DealStaCloseRes(state);
    } else {
        WIFI_LOGD("DealStaOpenRes:wifi open successfully!");
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
        WifiConfigCenter::GetInstance().SetStaLastRunState(true);
        if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
            WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(true);
        }

        cbMsg.msgData = static_cast<int>(WifiState::ENABLED);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);

        CheckAndStartScanService();
    }

    return;
}

void WifiManager::DealStaCloseRes(OperateResState state)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
    if (state == OperateResState::CLOSE_WIFI_CLOSING) {
        cbMsg.msgData = static_cast<int>(WifiState::DISABLING);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
        return;
    }
    if (state == OperateResState::CLOSE_WIFI_FAILED) {
        WIFI_LOGD("DealStaCloseRes:upload wifi close failed event!");
        cbMsg.msgData = static_cast<int>(WifiState::UNKNOWN);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    }

    if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
        WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(false);
    }

    cbMsg.msgData = static_cast<int>(WifiState::DISABLED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);

    CheckAndStopScanService();
    WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::STA_SERVICE_CLOSE);
    return;
}

void WifiManager::DealStaConnChanged(OperateResState state, const WifiLinkedInfo &info)
{
    bool isReport = true;
    int reportStateNum = static_cast<int>(ConvertConnStateInternal(state, isReport));
    if (isReport) {
        WifiEventCallbackMsg cbMsg;
        cbMsg.msgCode = WIFI_CBK_MSG_CONNECTION_CHANGE;
        cbMsg.msgData = reportStateNum;
        cbMsg.linkInfo = info;
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    }

    if (state == OperateResState::CONNECT_CONNECTING || state == OperateResState::CONNECT_AP_CONNECTED ||
        state == OperateResState::DISCONNECT_DISCONNECTING || state == OperateResState::DISCONNECT_DISCONNECTED ||
        state == OperateResState::CONNECT_OBTAINING_IP || state == OperateResState::CONNECT_ASSOCIATING ||
        state == OperateResState::CONNECT_ASSOCIATED) {
        if (WifiConfigCenter::GetInstance().GetScanMidState() == WifiOprMidState::RUNNING) {
            IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
            if (pService != nullptr) {
                pService->OnClientModeStatusChanged(static_cast<int>(state));
            }
        }
    }
    return;
}

void WifiManager::DealWpsChanged(WpsStartState state, const int pinCode)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_WPS_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    cbMsg.pinCode = std::to_string(pinCode);
    int len = cbMsg.pinCode.length();
    if (len < 8) { /* Fill in 8 digits. */
        cbMsg.pinCode = std::string(8 - len, '0') + cbMsg.pinCode;
    }
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealStreamChanged(StreamDirection direction)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STREAM_DIRECTION;
    cbMsg.msgData = static_cast<int>(direction);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealRssiChanged(int rssi)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_RSSI_CHANGE;
    cbMsg.msgData = rssi;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::CheckAndStartScanService(void)
{
    WifiOprMidState scanState = WifiConfigCenter::GetInstance().GetScanMidState();
    if (scanState != WifiOprMidState::CLOSED) {
        /* If the scanning function is enabled when the STA is not enabled, you need to start the scheduled
             scanning function immediately when the STA is enabled. */
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService != nullptr) {
            pService->OnClientModeStatusChanged(static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED));
        }
        return;
    }
    if (!WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::OPENING)) {
        WIFI_LOGD("Failed to set scan mid state opening! may be other activity has been operated");
        return;
    }
    ErrCode errCode = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_SCAN) < 0) {
            WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_SCAN);
            break;
        }
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_SCAN);
            break;
        }
        errCode = pService->RegisterScanCallbacks(WifiManager::GetInstance().GetScanCallback());
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register scan service callback failed!");
            break;
        }
        errCode = pService->Init();
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("init scan service failed, ret %{public}d!", static_cast<int>(errCode));
            break;
        }
    } while (0);
    if (errCode != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_SCAN);
    }
    return;
}

void WifiManager::CheckAndStopScanService(void)
{
    /**
     * Check unload SCAN service
     * When anytime scanning is enabled and the control policy allows, airplane
     * mode and power saving mode are disabled.   --- Do not disable the scan
     * service. Otherwise, disable the SCAN service.
     */
    WifiOprMidState scanState = WifiConfigCenter::GetInstance().GetScanMidState();
    if (scanState != WifiOprMidState::OPENING && scanState != WifiOprMidState::RUNNING) {
        return;
    }
    ScanControlInfo info;
    WifiConfigCenter::GetInstance().GetScanControlInfo(info);
    if (WifiConfigCenter::GetInstance().IsScanAlwaysActive() && IsAllowScanAnyTime(info) &&
        WifiConfigCenter::GetInstance().GetAirplaneModeState() == MODE_STATE_CLOSE &&
        WifiConfigCenter::GetInstance().GetPowerSavingModeState() == MODE_STATE_CLOSE) {
        return;
    }
    /* After check condition over, begin unload SCAN service */
    if (WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::CLOSING)) {
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService == nullptr) {
            WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::SCAN_SERVICE_CLOSE);
            return;
        }
        ErrCode ret = pService->UnInit();
        if (ret != WIFI_OPT_SUCCESS) {
            WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::CLOSING, scanState);
        }
    }
}

void WifiManager::InitScanCallback(void)
{
    mScanCallback.OnScanStartEvent = DealScanOpenRes;
    mScanCallback.OnScanStopEvent = DealScanCloseRes;
    mScanCallback.OnScanFinishEvent = DealScanFinished;
    mScanCallback.OnScanInfoEvent = DealScanInfoNotify;
}

IScanSerivceCallbacks WifiManager::GetScanCallback()
{
    return mScanCallback;
}

void WifiManager::DealScanOpenRes(void)
{
    WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
}

void WifiManager::DealScanCloseRes(void)
{
    WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::SCAN_SERVICE_CLOSE);
}

void WifiManager::DealScanFinished(int state)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_SCAN_STATE_CHANGE;
    cbMsg.msgData = state;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishScanFinishedEvent(state,"OnScanFinished");
}

void WifiManager::DealScanInfoNotify(std::vector<InterScanInfo> &results)
{
    if (WifiConfigCenter::GetInstance().GetWifiMidState() == WifiOprMidState::RUNNING) {
        IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
        if (pService != nullptr) {
            pService->ConnectivityManager(results);
        }
    }
}

void WifiManager::InitApCallback(void)
{
    mApCallback.OnApStateChangedEvent = DealApStateChanged;
    mApCallback.OnHotspotStaJoinEvent = DealApGetStaJoin;
    mApCallback.OnHotspotStaLeaveEvent = DealApGetStaLeave;
    return;
}

IApServiceCallbacks WifiManager::GetApCallback()
{
    return mApCallback;
}

void WifiManager::DealApStateChanged(ApState state)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    if (state == ApState::AP_STATE_IDLE) {
        WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::AP_SERVICE_CLOSE);
    }
    if (state == ApState::AP_STATE_STARTED) {
        WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
    }
    WifiCommonEventHelper::PublishHotspotStateChangedEvent((int)state, "OnHotspotStateChanged");
    return;
}

void WifiManager::DealApGetStaJoin(const StationInfo &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_JOIN;
    cbMsg.staInfo = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishApStaJoinEvent(0, "ApStaJoined");
    return;
}

void WifiManager::DealApGetStaLeave(const StationInfo &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_LEAVE;
    cbMsg.staInfo = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishApStaLeaveEvent(0, "ApStaLeaved");
    return;
}

void WifiManager::InitP2pCallback(void)
{
    mP2pCallback.OnP2pStateChangedEvent = DealP2pStateChanged;
    mP2pCallback.OnP2pPeersChangedEvent = DealP2pPeersChanged;
    mP2pCallback.OnP2pServicesChangedEvent = DealP2pServiceChanged;
    mP2pCallback.OnP2pConnectionChangedEvent = DealP2pConnectionChanged;
    mP2pCallback.OnP2pThisDeviceChangedEvent = DealP2pThisDeviceChanged;
    mP2pCallback.OnP2pDiscoveryChangedEvent = DealP2pDiscoveryChanged;
    mP2pCallback.OnP2pGroupsChangedEvent = DealP2pGroupsChanged;
    mP2pCallback.OnP2pActionResultEvent = DealP2pActionResult;
    return;
}

IP2pServiceCallbacks WifiManager::GetP2pCallback(void)
{
    return mP2pCallback;
}

void WifiManager::DealP2pStateChanged(P2pState state)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    if (state == P2pState::P2P_STATE_IDLE) {
        WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::P2P_SERVICE_CLOSE);
    }
    if (state == P2pState::P2P_STATE_STARTED) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
    }
    WifiCommonEventHelper::PublishP2pStateChangedEvent((int)state, "OnP2pStateChanged");
    return;
}

void WifiManager::DealP2pPeersChanged(const std::vector<WifiP2pDevice> &vPeers)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_PEER_CHANGE;
    cbMsg.device = vPeers;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pPeersStateChangedEvent(vPeers.size(), "OnP2pPeersChanged");
    return;
}

void WifiManager::DealP2pServiceChanged(const std::vector<WifiP2pServiceInfo> &vServices)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_SERVICE_CHANGE;
    cbMsg.serviceInfo = vServices;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealP2pConnectionChanged(const WifiP2pLinkedInfo &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_CONNECT_CHANGE;
    cbMsg.p2pInfo = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pConnStateEvent((int)info.GetConnectState(), "OnP2pConnectStateChanged");
    return;
}

void WifiManager::DealP2pThisDeviceChanged(const WifiP2pDevice &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_THIS_DEVICE_CHANGE;
    cbMsg.p2pDevice = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pCurrentDeviceStateChangedEvent(
        (int)info.GetP2pDeviceStatus(), "OnP2pThisDeviceChanged");
    return;
}

void WifiManager::DealP2pDiscoveryChanged(bool bState)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_DISCOVERY_CHANGE;
    cbMsg.msgData = static_cast<int>(bState);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealP2pGroupsChanged()
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_PERSISTENT_GROUPS_CHANGE;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pGroupStateChangedEvent(0, "OnP2pGroupStateChanged");
    return;
}

void WifiManager::DealP2pActionResult(P2pActionCallback action, ErrCode code)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_ACTION_RESULT;
    cbMsg.p2pAction = action;
    cbMsg.msgData = static_cast<int>(code);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

}  // namespace Wifi
}  // namespace OHOS
