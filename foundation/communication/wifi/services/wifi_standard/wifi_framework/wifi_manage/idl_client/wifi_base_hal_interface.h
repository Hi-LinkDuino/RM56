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

#ifndef OHOS_WIFI_BASE_HAL_INTERFACE_H
#define OHOS_WIFI_BASE_HAL_INTERFACE_H

#include "wifi_idl_client.h"

namespace OHOS {
namespace Wifi {
class WifiBaseHalInterface {
public:
    /**
     * @Description Construct a new Wifi Base Hal Interface object.
     *
     */
    WifiBaseHalInterface();
    /**
     * @Description Destroy the Wifi Base Hal Interface object.
     *
     */
    virtual ~WifiBaseHalInterface();
    /**
     * @Description Init IdlClient.
     *
     */
    bool InitIdlClient(void);
    /**
     * @Description WifiManagerService When exiting, send a message to the HAL layer to
     *              clear the data.
     *
     */
    void ExitAllIdlClient(void);

public:
    WifiIdlClient *mIdlClient;
};
}  // namespace Wifi
}  // namespace OHOS

#endif
