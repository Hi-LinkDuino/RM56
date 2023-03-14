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

#ifndef WIFI_WPA_HAL_H
#define WIFI_WPA_HAL_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "wifi_wpa_common.h"
#include "wifi_hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_CONF_FILE_PATH_LEN 256
#define BUFF_SIZE 1024

typedef struct AddInterfaceArgv {
    char name[WIFI_IFACE_NAME_MAXLEN];
    char confName[WIFI_CONF_FILE_PATH_LEN];
} AddInterfaceArgv;

typedef struct stWpaIfaceInfo {
    char name[WIFI_IFACE_NAME_MAXLEN];
    int num;
    struct stWpaIfaceInfo *next;
} WpaIfaceInfo;

typedef struct stWifiWpaInterface WifiWpaInterface;
struct stWifiWpaInterface {
    WpaCtrl wpaCtrl;
    pthread_t tid;
    int threadRunFlag;
    WpaIfaceInfo *ifaces;

    int (*wpaCliConnect)(WifiWpaInterface *p);
    void (*wpaCliClose)(WifiWpaInterface *p);

    int (*wpaCliAddIface)(WifiWpaInterface *p, const AddInterfaceArgv *argv);
    int (*wpaCliRemoveIface)(WifiWpaInterface *p, const char *name);
};

WifiWpaInterface *GetWifiWapGlobalInterface(void);
void ReleaseWpaGlobalInterface(void);
WpaCtrl *GetWpaCtrl(void);
int ExcuteCmd(const char *szCmd);

#ifdef __cplusplus
}
#endif
#endif