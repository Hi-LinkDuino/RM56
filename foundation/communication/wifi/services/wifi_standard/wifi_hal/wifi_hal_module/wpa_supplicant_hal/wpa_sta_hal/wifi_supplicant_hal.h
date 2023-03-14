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
#ifndef WIFI_WPA_SUPPLICANT_HAL_H
#define WIFI_WPA_SUPPLICANT_HAL_H

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include "wifi_hal_struct.h"
#include "wifi_hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif
#define KEY_MGMT_LENG 20
struct WpaHalCmdStatus {
    char bssid[WIFI_BSSID_LENGTH];
    int freq;
    char ssid[WIFI_SSID_LENGTH];
    int id;
    char keyMgmt[KEY_MGMT_LENG];
    char address[WIFI_BSSID_LENGTH];
};

struct WpaSetNetworkArgv {
    int id;          /* network id */
    DeviceConfigType param; /* set network param */
    char value[WIFI_NETWORK_CONFIG_VALUE_LENGTH];  /* set network value */
};

typedef struct WpaSsidField {
    DeviceConfigType field;
    char fieldName[32];
    int flag; /* 0 need add "" 1 no need */
} WpaSsidField;

struct WpaGetNetworkArgv {
    int id; /* network id. */
    char param[WIFI_NETWORK_CONFIG_VALUE_LENGTH]; /* parameter */
};

struct WpaWpsPbcArgv {
    int anyFlag;
    int multiAp;
    char bssid[WIFI_BSSID_LENGTH];
};

struct WpaWpsPinArgv {
    char bssid[WIFI_BSSID_LENGTH];
    char pinCode[WIFI_PIN_CODE_LENGTH + 1];
};

typedef struct WifiWpaStaInterface WifiWpaStaInterface;
struct WifiWpaStaInterface {
    int staNo;
    char ifname[WIFI_IFACE_NAME_MAXLEN];
    WifiWpaStaInterface *next;

    int (*wpaCliCmdStatus)(WifiWpaStaInterface *p, struct WpaHalCmdStatus *pcmd);
    int (*wpaCliCmdAddNetworks)(WifiWpaStaInterface *p);
    int (*wpaCliCmdReconnect)(WifiWpaStaInterface *p);
    int (*wpaCliCmdReassociate)(WifiWpaStaInterface *p);
    int (*wpaCliCmdDisconnect)(WifiWpaStaInterface *p);
    int (*wpaCliCmdSaveConfig)(WifiWpaStaInterface *p);
    int (*wpaCliCmdSetNetwork)(WifiWpaStaInterface *p, const struct WpaSetNetworkArgv *argv);
    int (*wpaCliCmdEnableNetwork)(WifiWpaStaInterface *p, int networkId);
    int (*wpaCliCmdSelectNetwork)(WifiWpaStaInterface *p, int networkId);
    int (*wpaCliCmdDisableNetwork)(WifiWpaStaInterface *p, int networkId);
    int (*wpaCliCmdRemoveNetwork)(WifiWpaStaInterface *p, int networkId);
    int (*wpaCliCmdGetNetwork)(WifiWpaStaInterface *p, const struct WpaGetNetworkArgv *argv, char *pcmd, unsigned size);
    int (*wpaCliCmdWpsPbc)(WifiWpaStaInterface *p, const struct WpaWpsPbcArgv *wpspbc);
    int (*wpaCliCmdWpsPin)(WifiWpaStaInterface *p, const struct WpaWpsPinArgv *wpspin, int *pincode);
    int (*wpaCliCmdWpsCancel)(WifiWpaStaInterface *p);
    int (*wpaCliCmdPowerSave)(WifiWpaStaInterface *p, int enable);
    int (*wpaCliCmdSetCountryCode)(WifiWpaStaInterface *p, const char *countryCode);
    int (*wpaCliCmdGetCountryCode)(WifiWpaStaInterface *p, char *countryCode, int codeSize);
    int (*wpaCliCmdSetAutoConnect)(WifiWpaStaInterface *p, int enable);
    int (*wpaCliCmdWpaBlockListClear)(WifiWpaStaInterface *p);
    int (*wpaCliCmdListNetworks)(WifiWpaStaInterface *p, WifiNetworkInfo *pcmd, int *size);
    int (*wpaCliCmdScan)(WifiWpaStaInterface *p, const ScanSettings *settings);
    int (*wpaCliCmdScanInfo)(WifiWpaStaInterface *p, ScanInfo *pcmd, int *size);
    int (*wpaCliCmdGetSignalInfo)(WifiWpaStaInterface *p, WpaSignalInfo *info);
};

/**
 * @Description Get wpa interface about sta.
 *
 * @return WifiWpaStaInterface*.
 */
WifiWpaStaInterface *GetWifiStaInterface(int staNo);

/**
 * @Description Release sta interface
 *
 */
void ReleaseWifiStaInterface(int staNo);

WifiWpaStaInterface *TraversalWifiStaInterface(void);
int GetStaInterfaceNo(const char *ifName);

#ifdef __cplusplus
}
#endif
#endif