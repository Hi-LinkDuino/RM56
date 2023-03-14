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
#include "wifi_hal_adapter.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalAdapter"

WifiHalVendorInterface *g_wifiHalVendorInterface = NULL;

#define MODULE_NAME_MAX_LEN 256
#define MODULE_CONFIG_FILE_PATH "/data/misc/wifi/wifi_hal_vendor.conf"
static int ReadConfigModuleName(char *name, int size)
{
    if (name == NULL) {
        return -1;
    }
    FILE *fp = fopen(MODULE_CONFIG_FILE_PATH, "r");
    if (fp == NULL) {
        LOGE("open module configuration file failed");
        return 0; /* file not exist, use default operators */
    }
    int flag = 0;
    do {
        fseek(fp, 0, SEEK_END);
        int len = ftell(fp);
        if ((len >= size) || (len == -1)) {
            LOGE("config file size too big, config file may not correct!");
            break;
        }
        rewind(fp);
        int ret = fread(name, sizeof(char), (size_t)len, fp);
        if (ret != len) {
            LOGE("read file failed!");
            break;
        }
        flag += 1;
    } while (0);
    fclose(fp);
    return (flag == 0) ? -1 : 0;
}

static int OpenHalVendorModule(WifiHalVendorInterface *pInterface)
{
    if (pInterface == NULL) {
        return -1;
    }
    char name[MODULE_NAME_MAX_LEN] = {0};
    if (ReadConfigModuleName(name, MODULE_NAME_MAX_LEN) < 0) {
        return -1;
    }
    if (strlen(name) <= 0) {
        LOGW("module name is null.");
        return 0;
    }

    void *handle = dlopen(name, RTLD_LAZY);
    if (handle == NULL) {
        LOGE("open config [%{public}s] so failed![%{public}s]", name, dlerror());
        return -1;
    }
    int flag = 0;
    do {
        pInitHalVendorFunc pFunc = (pInitHalVendorFunc)dlsym(handle, "InitHalVendorFunc");
        if (pFunc == NULL) {
            LOGE("Not find InitHalVendorFunc, cannot use this [%{public}s] so", name);
            break;
        }
        HalVendorError err = pFunc(&pInterface->func);
        if (err != HAL_VENDOR_SUCCESS) {
            LOGE("init hal vendor function table failed! name [%{public}s], ret[%{public}d]", name, err);
            break;
        }
        err = pInterface->func.wifiInitialize();
        if (err != HAL_VENDOR_SUCCESS) {
            LOGE("init vendor hal failed!, ret[%{public}d]", err);
            break;
        }
        pInterface->handle = handle;
        flag += 1;
    } while (0);
    if (flag == 0) {
        dlclose(handle);
        return -1;
    }
    return 0;
}

WifiHalVendorInterface *GetWifiHalVendorInterface(void)
{
    if (g_wifiHalVendorInterface != NULL) {
        return g_wifiHalVendorInterface;
    }
    g_wifiHalVendorInterface = (WifiHalVendorInterface *)calloc(1, sizeof(WifiHalVendorInterface));
    if (g_wifiHalVendorInterface == NULL) {
        return NULL;
    }
    InitDefaultHalVendorFunc(&g_wifiHalVendorInterface->func);
    int ret = OpenHalVendorModule(g_wifiHalVendorInterface);
    if (ret < 0) {
        ReleaseWifiHalVendorInterface();
    }
    return g_wifiHalVendorInterface;
}

void ReleaseWifiHalVendorInterface(void)
{
    if (g_wifiHalVendorInterface != NULL) {
        if (g_wifiHalVendorInterface->handle != NULL) {
            if (g_wifiHalVendorInterface->func.wifiCleanUp) {
                g_wifiHalVendorInterface->func.wifiCleanUp();
            }
            dlclose(g_wifiHalVendorInterface->handle);
        }
        free(g_wifiHalVendorInterface);
        g_wifiHalVendorInterface = NULL;
    }
    return;
}
