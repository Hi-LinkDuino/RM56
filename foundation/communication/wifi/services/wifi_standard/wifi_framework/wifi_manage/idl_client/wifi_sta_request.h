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

#ifndef OHOS_WIFI_STA_REQUEST_H
#define OHOS_WIFI_STA_REQUEST_H

#include <string>

namespace OHOS {
namespace Wifi {
struct WifiStaRequest {
    std::string ifName;  /* Interface Name */
    int cmdId;           /* Command ID */
    std::string content; /* Command content */
    /* This interface needs to be detailed based on specific services. */

    WifiStaRequest() : cmdId(0)
    {}

    ~WifiStaRequest()
    {}
};
}  // namespace Wifi
}  // namespace OHOS

#endif