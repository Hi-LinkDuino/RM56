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
#include "wifi_p2p_temp_disc_event.h"
#include "wifi_logger.h"

namespace OHOS {
namespace Wifi {
WifiP2pTempDiscEvent::WifiP2pTempDiscEvent() : event(DiscEvent::UNKNOWN), device(), pin()
{}
void WifiP2pTempDiscEvent::SetDiscEvent(DiscEvent setEvent)
{
    event = setEvent;
}
DiscEvent WifiP2pTempDiscEvent::GetDiscEvent() const
{
    return event;
}
void WifiP2pTempDiscEvent::SetDevice(const WifiP2pDevice &setDevice)
{
    device = setDevice;
}
const WifiP2pDevice &WifiP2pTempDiscEvent::GetDevice() const
{
    return device;
}
void WifiP2pTempDiscEvent::SetPin(std::string setPin)
{
    pin = setPin;
}
const std::string &WifiP2pTempDiscEvent::GetPin() const
{
    return pin;
}
}  // namespace Wifi
}  // namespace OHOS