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
#include "scan_interface.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_SCAN_LABEL("ScanInterface");

namespace OHOS {
namespace Wifi {
ScanInterface::ScanInterface() : pScanService(nullptr)
{}

ScanInterface::~ScanInterface()
{
    WIFI_LOGI("Enter ScanInterface::~ScanInterface.");
    if (pScanService != nullptr) {
        delete pScanService;
        pScanService = nullptr;
    }
}

extern "C" IScanService *Create(void)
{
    return new ScanInterface();
}
extern "C" void Destroy(ScanInterface *scanInterface)
{
    delete scanInterface;
    scanInterface = nullptr;
}

ErrCode ScanInterface::Init()
{
    WIFI_LOGI("Enter ScanInterface::Init.\n");

    pScanService = new (std::nothrow)ScanService();
    if (pScanService == nullptr) {
        WIFI_LOGE("New ScanService failed.\n");
        return WIFI_OPT_INVALID_PARAM;
    }

    if (!(pScanService->InitScanService(mScanSerivceCallbacks))) {
        WIFI_LOGE("InitScanService failed.\n");
        delete pScanService;
        pScanService = nullptr;
        return WIFI_OPT_INVALID_PARAM;
    }

    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::UnInit()
{
    WIFI_LOGI("Enter ScanInterface::UnInit.\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->UnInitScanService();
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::Scan(bool externFlag)
{
    WIFI_LOGI("Enter ScanInterface::Scan\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    return pScanService->Scan(externFlag);
}

ErrCode ScanInterface::ScanWithParam(const WifiScanParams &wifiScanParams)
{
    WIFI_LOGI("Enter ScanInterface::ScanWithParam\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    return pScanService->ScanWithParam(wifiScanParams);
}

ErrCode ScanInterface::DisableScan(bool disable)
{
    WIFI_LOGI("Enter ScanInterface::DisableScan");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    return pScanService->DisableScan(disable);
}

ErrCode ScanInterface::OnScreenStateChanged(int screenState)
{
    WIFI_LOGI("Enter ScanInterface::OnScreenStateChanged\n");

    if (screenState != MODE_STATE_OPEN && screenState != MODE_STATE_CLOSE) {
        WIFI_LOGE("screenState param is error");
        return WIFI_OPT_INVALID_PARAM;
    }
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->HandleScreenStatusChanged();
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnClientModeStatusChanged(int staStatus)
{
    WIFI_LOGI("Enter ScanInterface::OnClientModeStatusChanged\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->HandleStaStatusChanged(staStatus);
    pScanService->SetStaCurrentTime();
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnAppRunningModeChanged(ScanMode appRunMode)
{
    WIFI_LOGI("Enter ScanInterface::OnAppRunningModeChanged, appRunMode=%{public}d\n", static_cast<int>(appRunMode));
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnMovingFreezeStateChange()
{
    LOGI("Enter ScanInterface::OnMovingFreezeStateChange");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->HandleMovingFreezeChanged();
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnCustomControlStateChanged(int customScene, int customSceneStatus)
{
    WIFI_LOGI("Enter ScanInterface::OnCustomControlStateChanged\n");

    if (customSceneStatus != MODE_STATE_OPEN && customSceneStatus != MODE_STATE_CLOSE) {
        WIFI_LOGE("screenState param is error");
        return WIFI_OPT_INVALID_PARAM;
    }
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->HandleCustomStatusChanged(customScene, customSceneStatus);
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnGetCustomSceneState(std::map<int, time_t>& sceneMap) const
{
    WIFI_LOGI("Enter ScanInterface::OnGetCustomSceneState\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->HandleGetCustomSceneState(sceneMap);
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::OnControlStrategyChanged()
{
    WIFI_LOGI("Enter ScanInterface::OnControlStrategyChanged\n");
    CHECK_NULL_AND_RETURN(pScanService, WIFI_OPT_FAILED);
    pScanService->ClearScanControlValue();
    pScanService->GetScanControlInfo();
    pScanService->SystemScanProcess(true);
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanInterface::RegisterScanCallbacks(const IScanSerivceCallbacks &scanSerivceCallbacks)
{
    mScanSerivceCallbacks = scanSerivceCallbacks;
    return WIFI_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS