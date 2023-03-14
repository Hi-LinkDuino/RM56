/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_INTERFACE
#define BATTERY_INTERFACE

#include <iproxy_client.h>
#include <iproxy_server.h>
#include <iunknown.h>
#include "batterymgr_intf_define.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    BATTERY_FUNCID_GETSOC = 0,
    BATTERY_FUNCID_GETCHARGING,
    BATTERY_FUNCID_GETHEALTH,
    BATTERY_FUNCID_GETPLUGTYPE,
    BATTERY_FUNCID_GETVOLTAGE,
    BATTERY_FUNCID_GETTECHNOLONY,
    BATTERY_FUNCID_GETTEMPERATURE,
    BATTERY_FUNCID_END,
} BatteryFuncId;

typedef struct {
    INHERIT_SERVER_IPROXY;
    INHERIT_BATTERY_INTERFACE;
} BatteryInterface;

typedef struct {
    INHERIT_CLIENT_IPROXY;
    INHERIT_BATTERY_INTERFACE;
} BatteryProxyInterface;


#ifdef __cplusplus
}
#endif // __cplusplus
#endif