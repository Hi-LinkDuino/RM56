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

#ifndef POWERMGR_SCREEN_SAVER_INTERFACE_H
#define POWERMGR_SCREEN_SAVER_INTERFACE_H

#include <iproxy_client.h>
#include <iproxy_server.h>
#include <iunknown.h>

#include "screen_saver_intf_define.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    SCREENSAVER_FUNCID_SETSTATE = 0,
    SCREENSAVER_FUNCID_BUTT,
} ScreenSaverFuncId;

typedef struct {
    INHERIT_SERVER_IPROXY;
    INHERIT_SCREENSAVER_INTERFACE;
} ScreenSaverInterface;

typedef struct {
    INHERIT_CLIENT_IPROXY;
    INHERIT_SCREENSAVER_INTERFACE;
} ScreenSaverProxyInterface;

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_SCREEN_SAVER_INTERFACE_H
