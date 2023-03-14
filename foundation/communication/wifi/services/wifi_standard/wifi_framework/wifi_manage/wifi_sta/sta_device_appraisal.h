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
#ifndef OHOS_WIFI_NETWORAPPRAISAL_H
#define OHOS_WIFI_NETWORAPPRAISAL_H

#include <vector>
#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "log_helper.h"
#include "sta_define.h"
#include "wifi_internal_msg.h"

namespace OHOS {
namespace Wifi {
static const int MIN_24_FREQUENCY = 2400;
static const int MAX_24_FREQUENCY = 2500;
static const int MIN_5_FREQUENCY = 5000;
static const int MAX_5_FREQUENCY = 5900;
class StaDeviceAppraisal {
public:
    virtual ~StaDeviceAppraisal()
    {}

    virtual ErrCode DeviceAppraisals(
        WifiDeviceConfig &electedDevice, std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif