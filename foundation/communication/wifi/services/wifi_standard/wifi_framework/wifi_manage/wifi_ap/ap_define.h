/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_AP_DEFINE_H
#define OHOS_AP_DEFINE_H

#include <any>
#include <string>

#include "ap_macro.h"

namespace OHOS {
namespace Wifi {
const std::string IN_INTERFACE = "wlan0";
const std::string OUT_INTERFACE = "wlan0";
const int MAX_SSID_LENGTH = 32;
const int MAX_PSK_LENGTH = 63;
const int MAC_LENGTH = 17;
const int MAX_IP_LENGTH = 39;
const std::string GETTING_INFO = "Obtaining...";

/* *****************************ApService********************************** */

/* *****************************ApMonitor********************************** */

/* *********************************StateMachine*************************** */

/* Internal communication message of the state machine. */
enum class ApStatemachineEvent {
    CMD_START_HOTSPOT,               /* start */
    CMD_STOP_HOTSPOT,                /* stop */
    CMD_FAIL,                        /* failed */
    CMD_STATION_JOIN,                /* new station */
    CMD_STATION_LEAVE,               /* station leave */
    CMD_SET_HOTSPOT_CONFIG,          /* set configure */
    CMD_ADD_BLOCK_LIST,              /* add block station */
    CMD_DEL_BLOCK_LIST,              /* del block station */
    CMD_DISCONNECT_STATION,          /* disconnect station */
    CMD_UPDATE_HOTSPOTCONFIG_RESULT, /* Update hotspot configuration result */
                                     /* (asynchronous result) */
};

using HandlerMethod = void(ApStatemachineEvent, int, int, const std::any &);
}
} // namespace OHOS
#endif /* OHOS_AP_DEFINE_H */