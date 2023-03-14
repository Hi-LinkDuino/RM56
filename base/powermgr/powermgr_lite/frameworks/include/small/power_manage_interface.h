/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_POWER_MANAGE_INTERFACE_H
#define POWERMGR_POWER_MANAGE_INTERFACE_H

#include <iproxy_client.h>
#include <iproxy_server.h>
#include <iunknown.h>

#include "power_manage_intf_define.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    POWERMANAGE_FUNCID_ACQUIRERUNNINGLOCK = 0,
    POWERMANAGE_FUNCID_RELEASERUNNINGLOCK,
    POWERMANAGE_FUNCID_ISANYRUNNINGLOCKHOLDING,
    POWERMANAGE_FUNCID_SUSPEND,
    POWERMANAGE_FUNCID_WAKEUP,
    POWERMANAGE_FUNCID_BUTT,
} PowerManageFuncId;

typedef struct {
    INHERIT_SERVER_IPROXY;
    INHERIT_POWERMANAGE_INTERFACE;
} PowerManageInterface;

typedef struct {
    INHERIT_CLIENT_IPROXY;
    INHERIT_POWERMANAGE_INTERFACE;
} PowerManageProxyInterface;

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_POWER_MANAGE_INTERFACE_H
