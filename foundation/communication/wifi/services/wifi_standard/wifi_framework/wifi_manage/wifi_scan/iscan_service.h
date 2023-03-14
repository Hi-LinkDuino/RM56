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
#ifndef OHOS_ISCAN_SERVICE_H
#define OHOS_ISCAN_SERVICE_H

#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "iscan_service_callbacks.h"

namespace OHOS {
namespace Wifi {
class IScanService {
public:
    virtual ~IScanService() = default;
    /**
     * @Description  Scan service initialization function.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode Init() = 0;
    /**
     * @Description  Stopping the Scan Service.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode UnInit() = 0;
    /**
     * @Description Processes interface service scan request.
     *
     * @param externFlag it is from an external scan[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode Scan(bool externFlag) = 0;
    /**
     * @Description Processes interface service scan with param request.
     *
     * @param wifiScanParams Parameters in the scan request[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode ScanWithParam(const WifiScanParams &wifiScanParams) = 0;
    /**
     * @Description Disable/Restore the scanning operation.
     *
     * * @param params - disable or not.
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode DisableScan(bool disable) = 0;
    /**
     * @Description Processes interface service screen change request.
     *
     * @param screenState screen state[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnScreenStateChanged(int screenState) = 0;
    /**
     * @Description Processes interface service sta status change request.
     *
     * @param staStatus sta status[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnClientModeStatusChanged(int staStatus) = 0;
    /**
     * @Description Processes interface service appMode change request.
     *
     * @param appMode operate app mode[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnAppRunningModeChanged(ScanMode appRunMode) = 0;
    /**
     * @Description Updates the MovingFreeze state when the associated state changes.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnMovingFreezeStateChange() = 0;
    /**
     * @Description Processes interface service custom scene change request.
     *
     * @param customScene custom scene[in]
     * @param customSceneStatus Enter or exit the customized scenario[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnCustomControlStateChanged(int customScene, int customSceneStatus) = 0;
    /**
     * @Description Get custom scene state.
     *
     * @param sceneMap custom scene state map[out]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnGetCustomSceneState(std::map<int, time_t>& sceneMap) const = 0;
    /**
     * @Description Processes interface service scan control info change request.
     *
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode OnControlStrategyChanged() = 0;
    /**
     * @Description Registers the callback function of the scanning module to the interface service.
     *
     * @param scanSerivceCallbacks callback function
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode RegisterScanCallbacks(const IScanSerivceCallbacks &scanSerivceCallbacks) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif