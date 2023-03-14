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

#ifndef OHOS_WIFI_CHIP_HAL_INTERFACE_H
#define OHOS_WIFI_CHIP_HAL_INTERFACE_H

#include <string>
#include <vector>
#include "i_wifi_chip.h"
#include "i_wifi_struct.h"
#include "wifi_base_hal_interface.h"
#include "wifi_chip_event_callback.h"

namespace OHOS {
namespace Wifi {
class WifiChipHalInterface : public WifiBaseHalInterface {
public:
    /**
     * @Description Get the Instance object.
     *
     * @return WifiChipHalInterface&
     */
    static WifiChipHalInterface &GetInstance(void);

    /**
     * @Description Obtains the chip object by ID.
     *
     * @param id
     * @param chip
     * @return WifiErrorNo
     */
    WifiErrorNo GetWifiChipObject(int id, IWifiChip &chip);

    /**
     * @Description Obtains the Wi-Fi chip ID set.
     *
     * @param ids
     * @return WifiErrorNo
     */
    WifiErrorNo GetChipIds(std::vector<int> &ids);

    /**
     * @Description Obtains the chip ID.
     *
     * @param id
     * @return WifiErrorNo
     */
    WifiErrorNo GetUsedChipId(int &id);

    /**
     * @Description Obtains chip capabilities.
     *
     * @param capabilities
     * @return WifiErrorNo
     */
    WifiErrorNo GetChipCapabilities(int &capabilities);

    /**
     * @Description Obtains the joint mode supported by the chip,
     *              for example, sta+sta/sta+p2p/sta+ap/sta+nan/ap+nan.
     *
     * @param modes
     * @return WifiErrorNo
     */
    WifiErrorNo GetSupportedModes(std::vector<int> &modes);

    /**
     * @Description Configure the current joint mode of the chip.
     *
     * @param mode
     * @return WifiErrorNo
     */
    WifiErrorNo ConfigRunModes(int mode);

    /**
     * @Description Gets the current federation mode.
     *
     * @param mode
     * @return WifiErrorNo
     */
    WifiErrorNo GetCurrentMode(int &mode);

    /**
     * @Description Registering a Wi-Fi Chip Event.
     *
     * @param callback
     * @return WifiErrorNo
     */
    WifiErrorNo RegisterChipEventCallback(WifiChipEventCallback &callback);

    /**
     * @Description Requesting the debugging information of the firmware chip.
     *
     * @param debugInfo
     * @return WifiErrorNo
     */
    WifiErrorNo RequestFirmwareDebugInfo(std::string &debugInfo);

    /**
     * @Description Setting the Power Mode.
     *
     * @param mode
     * @return WifiErrorNo
     */
    WifiErrorNo SetWifiPowerMode(int mode);
};
}  // namespace Wifi
}  // namespace OHOS

#endif