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

#ifndef OHOS_WIFISCANPARAM_H
#define OHOS_WIFISCANPARAM_H

#include <string>
#include <vector>

namespace OHOS {
namespace Wifi {
struct WifiScanParam {
    std::vector<std::string> hiddenNetworkSsid; /* Hiding Network SSIDs */
    std::vector<int> scanFreqs;                 /* Scan frequency */
    int scanStyle;
};

struct WifiPnoScanParam {
    int scanInterval;                    /* PNO Scan Interval */
    std::vector<int> scanFreqs;          /* Scanning frequency */
    std::vector<std::string> hiddenSsid; /* Network name of hidden network */
    std::vector<std::string> savedSsid;  /* Network name of saved network */
    int minRssi2Dot4Ghz;                 /* Minimum 2.4 GHz network signal strength */
    int minRssi5Ghz;                     /* Minimum 5 GHz network signal strength */

    WifiPnoScanParam()
    {
        scanFreqs.clear();
        hiddenSsid.clear();
        savedSsid.clear();

        scanInterval = 0;
        minRssi2Dot4Ghz = 0;
        minRssi5Ghz = 0;
    }

    ~WifiPnoScanParam()
    {
        scanFreqs.clear();
        hiddenSsid.clear();
        savedSsid.clear();
    }
};
}  // namespace Wifi
}  // namespace OHOS
#endif