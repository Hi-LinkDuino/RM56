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
#include "mock_scan_service.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_SCAN_LABEL("MockScanService");

namespace OHOS {
namespace Wifi {
bool MockScanService::InitScanService(const IScanSerivceCallbacks &iScanSerivceCallbacks)
{
    (void)iScanSerivceCallbacks;
    WIFI_LOGI("Enter MockScanService::InitScanService");
    return true;
}

void MockScanService::UnInitScanService()
{
    WIFI_LOGI("Enter MockScanService::UnInitScanService");
}

void MockScanService::RegisterScanCallbacks(const IScanSerivceCallbacks &iScanSerivceCallbacks)
{
    (void)iScanSerivceCallbacks;
    WIFI_LOGI("Enter MockScanService::RegisterScanCallbacks");
}

ErrCode MockScanService::Scan(bool externFlag)
{
    (void)externFlag;
    WIFI_LOGI("Enter MockScanService::Scan");
    return WIFI_OPT_SUCCESS;
}

ErrCode MockScanService::ScanWithParam(const WifiScanParams &params)
{
    (void)params;
    WIFI_LOGI("Enter MockScanService::ScanWithParam");
    return WIFI_OPT_SUCCESS;
}

void MockScanService::SystemScanProcess(bool scanAtOnce)
{
    (void)scanAtOnce;
    WIFI_LOGI("Enter MockScanService::SystemScanProcess");
}

void MockScanService::HandleScreenStatusChanged()
{
    WIFI_LOGI("Enter MockScanService::HandleScreenStatusChanged");
}

void MockScanService::HandleStaStatusChanged(int status)
{
    (void)status;
    WIFI_LOGI("Enter MockScanService::HandleStaStatusChanged");
}

void MockScanService::HandleCustomStatusChanged(int customScene, int customSceneStatus)
{
    (void)customScene;
    (void)customSceneStatus;
    WIFI_LOGI("Enter MockScanService::HandleCustomStatusChanged");
}

void MockScanService::HandleGetCustomSceneState(std::map<int, time_t>& sceneMap) const
{
    (void)sceneMap;
    WIFI_LOGI("Enter MockScanService::HandleGetCustomSceneState");
}

void MockScanService::GetScanControlInfo()
{
    WIFI_LOGI("Enter MockScanService::GetScanControlInfo");
}

void MockScanService::ClearScanControlValue()
{
    WIFI_LOGI("Enter MockScanService::ClearScanControlValue");
}

void MockScanService::SetStaCurrentTime()
{
    WIFI_LOGI("Enter MockScanService::SetStaCurrentTime");
}

void MockScanService::HandleScanStatusReport(ScanStatusReport &scanStatusReport)
{
    (void)scanStatusReport;
    WIFI_LOGI("Enter MockScanService::HandleScanStatusReport");
}
} // namespace Wifi
} // namespace OHOS
