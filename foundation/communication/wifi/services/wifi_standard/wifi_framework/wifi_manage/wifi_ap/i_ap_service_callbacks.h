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

#ifndef OHOS_IAP_SERVICE_CALLBACK_H
#define OHOS_IAP_SERVICE_CALLBACK_H

#include <functional>
#include "wifi_errcode.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
/* All callbacks provided by the AP service */
struct IApServiceCallbacks {
    std::function<void(ApState)> OnApStateChangedEvent;
    std::function<void(const StationInfo &)> OnHotspotStaJoinEvent;   // STA device join event.
    std::function<void(const StationInfo &)> OnHotspotStaLeaveEvent;  // STA device leaving event.
};
}  // namespace Wifi
}  // namespace OHOS
#endif