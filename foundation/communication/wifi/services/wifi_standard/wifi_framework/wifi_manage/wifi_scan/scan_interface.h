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

#ifndef OHOS_WIFI_SCAN_INTERFACE_H
#define OHOS_WIFI_SCAN_INTERFACE_H

#include <map>
#include "define.h"
#include "iscan_service.h"
#include "scan_service.h"

namespace OHOS {
namespace Wifi {
class ScanInterface : public IScanService {
    FRIEND_GTEST(ScanInterface);
public:
    ScanInterface();
    ~ScanInterface();

    /**
     * @Description  Scan service initialization function.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode Init();
    /**
     * @Description  Stopping the Scan Service
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode UnInit();
    /**
     * @Description Processes interface service scan request.
     *
     * @param externFlag it is from an external scan[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode Scan(bool externFlag);
    /**
     * @Description Processes interface service scan with param request.
     *
     * @param wifiScanParams Parameters in the scan request[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode ScanWithParam(const WifiScanParams &wifiScanParams);
    /**
     * @Description Disable/Restore the scanning operation.
     *
     * * @param params - disable or not.
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode DisableScan(bool disable);
    /**
     * @Description Processes interface service screen change request.
     *
     * @param screenState screen state[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnScreenStateChanged(int screenState);
    /**
     * @Description Processes interface service sta status change request.
     *
     * @param staStatus sta status[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnClientModeStatusChanged(int staStatus);
    /**
     * @Description Processes interface service appMode change request.
     *
     * @param appMode operate app mode[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnAppRunningModeChanged(ScanMode appRunMode);
    /**
     * @Description Updates the MovingFreeze state when the associated state changes.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnMovingFreezeStateChange();
    /**
     * @Description Processes interface service custom scene change request.
     *
     * @param customScene custom scene[in]
     * @param customSceneStatus Enter or exit the customized scenario[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnCustomControlStateChanged(int customScene, int customSceneStatus);
    /**
     * @Description Get custom scene state.
     *
     * @param sceneMap custom scene state map[out]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnGetCustomSceneState(std::map<int, time_t>& customSceneStateMap) const;
    /**
     * @Description Processes interface service scan control info change request.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode OnControlStrategyChanged();
    /**
     * @Description Registers the callback function of the scanning module to the interface service.
     *
     * @param scanSerivceCallbacks callback function
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    ErrCode RegisterScanCallbacks(const IScanSerivceCallbacks &scanSerivceCallbacks);

private:
    ScanService *pScanService;
    IScanSerivceCallbacks mScanSerivceCallbacks;
};
}  // namespace Wifi
}  // namespace OHOS

#endif