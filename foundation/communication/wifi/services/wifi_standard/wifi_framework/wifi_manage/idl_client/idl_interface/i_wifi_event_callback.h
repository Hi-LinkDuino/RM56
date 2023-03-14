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

#ifndef OHOS_IDL_IWIFI_EVENT_CALLBACK_H
#define OHOS_IDL_IWIFI_EVENT_CALLBACK_H

#include "wifi_error_no.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IWifiEventCallback {
    void (*onStarted)(void);
    void (*onStopped)(void);
    void (*onFailure)(WifiErrorNo errCode);
    void (*onConnectChanged)(int status, int networkId, const char *bssid);
    void (*onWpaStateChanged)(int status);
    void (*onSsidWrongkey)(int status);
    void (*onWpsOverlap)(int status);
    void (*onWpsTimeOut)(int status);
    void (*onWpsConnectionFull)(int status);
    void (*onWpsConnectionReject)(int status);
} IWifiEventCallback;

#ifdef __cplusplus
}
#endif
#endif