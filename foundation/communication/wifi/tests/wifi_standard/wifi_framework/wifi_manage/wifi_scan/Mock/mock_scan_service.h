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
#ifndef OHOS_MOCK_SCANSERVICE_H
#define OHOS_MOCK_SCANSERVICE_H

#include <gmock/gmock.h>
#include "scan_service.h"

namespace OHOS {
namespace Wifi {
class MockScanService : public ScanService {
public:
    MockScanService(){}
    ~MockScanService(){}
    bool InitScanService(const IScanSerivceCallbacks &iScanSerivceCallbacks);
    void UnInitScanService();
    void RegisterScanCallbacks(const IScanSerivceCallbacks &iScanSerivceCallbacks);
    ErrCode Scan(bool externFlag);
    ErrCode ScanWithParam(const WifiScanParams &params);
    void SystemScanProcess(bool scanAtOnce);
    void HandleScreenStatusChanged();
    void HandleStaStatusChanged(int status);
    void HandleCustomStatusChanged(int customScene, int customSceneStatus);
    void HandleGetCustomSceneState(std::map<int, time_t>& sceneMap) const;
    void GetScanControlInfo();
    void ClearScanControlValue();
    void SetStaCurrentTime();
    void HandleScanStatusReport(ScanStatusReport &scanStatusReport);
};
}  // namespace Wifi
}  // namespace OHOS
#endif