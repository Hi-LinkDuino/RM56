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
#ifndef OHOS_P2P_TEMP_DISC_EVENT_H
#define OHOS_P2P_TEMP_DISC_EVENT_H

#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
enum class DiscEvent {
    UNKNOWN = 0,
    PBC_REQ = 1,
    PBC_RESP = 2,
    ENTER_PIN = 3,
    SHOW_PIN = 4,
};
class WifiP2pTempDiscEvent {
public:
    /**
     * @Description Construct a new WifiP2pTempDiscEvent object.
     *
     */
    WifiP2pTempDiscEvent();
    /**
     * @Description Destroy the WifiP2pTempDiscEvent object.
     *
     */
    ~WifiP2pTempDiscEvent() = default;
    /**
     * @Description Set the event object.
     *
     * @param setEvent discovered setEvent
     */
    void SetDiscEvent(DiscEvent setEvent);
    /**
     * @Description Get the event object.
     *
     * @return DiscEvent
     */
    DiscEvent GetDiscEvent() const;
    /**
     * @Description Set the device object.
     *
     * @param setDevice setDevice information
     */
    void SetDevice(const WifiP2pDevice &setDevice);
    /**
     * @Description Get the device object.
     *
     * @return const WifiP2pDevice& device
     */
    const WifiP2pDevice &GetDevice() const;
    /**
     * @Description Set the pin object.
     *
     * @param setPin the string of setPin
     */
    void SetPin(std::string setPin);
    /**
     * @Description Get the pin object.
     *
     * @return const std::string& pin
     */
    const std::string &GetPin() const;

private:
    DiscEvent event;
    WifiP2pDevice device;
    std::string pin;
}; /* class WifiP2pTempDiscEvent */
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_P2P_TEMP_DISC_EVENT_H */