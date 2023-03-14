/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "operator_overload.h"

namespace OHOS {
namespace Wifi {
bool operator==(const StationInfo &a, const StationInfo &b)
{
    if (a.deviceName != b.deviceName) {
        return false;
    }
    if (a.bssid != b.bssid) {
        return false;
    }
    if (a.ipAddr != b.ipAddr) {
        return false;
    }
    return true;
}

bool operator==(const HotspotConfig &a, const HotspotConfig &b)
{
    if (a.GetSsid() != b.GetSsid()) {
        return false;
    }
    if (a.GetPreSharedKey() != b.GetPreSharedKey()) {
        return false;
    }
    if (a.GetSecurityType() != b.GetSecurityType()) {
        return false;
    }
    if (a.GetBand() != b.GetBand()) {
        return false;
    }
    if (a.GetChannel() != b.GetChannel()) {
        return false;
    }
    if (a.GetMaxConn() != b.GetMaxConn()) {
        return false;
    }
    return true;
}
} // namespace Wifi
} // namespace OHOS