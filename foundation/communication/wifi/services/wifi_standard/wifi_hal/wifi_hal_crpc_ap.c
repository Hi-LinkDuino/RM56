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

#include "wifi_hal_crpc_ap.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_define.h"

int RpcStartSoftAp(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = StartSoftAp();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcStopSoftAp(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    WifiErrorNo err = StopSoftAp();
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcSetHostapdConfig(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    HostapdConfig config;
    if (memset_s(&config, sizeof(config), 0, sizeof(config)) != EOK) {
        return -1;
    }

    if (ReadStr(context, config.ssid, sizeof(config.ssid)) != 0 || ReadInt(context, &config.ssidLen) < 0 ||
        ReadStr(context, config.preSharedKey, sizeof(config.preSharedKey)) != 0 ||
        ReadInt(context, &config.preSharedKeyLen) < 0 || ReadInt(context, &config.securityType) < 0 ||
        ReadInt(context, &config.band) < 0 || ReadInt(context, &config.channel) < 0 ||
        ReadInt(context, &config.maxConn) < 0) {
        return -1;
    }
    WifiErrorNo err = SetHostapdConfig(&config);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcGetStaInfos(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int maxSize = 0;
    if (ReadInt(context, &maxSize) < 0 || maxSize <= 0) {
        return -1;
    }
    char *infos = (char *)calloc(maxSize, sizeof(char));
    if (infos == NULL) {
        return -1;
    }
    WifiErrorNo err = GetStaInfos(infos, &maxSize);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, maxSize);
        WriteStr(context, infos);
    }
    WriteEnd(context);
    free(infos);
    infos = NULL;
    return 0;
}

int RpcSetCountryCode(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    char countryCode[WIFI_COUNTRY_CODE_MAXLEN + 1] = {0};
    if (ReadStr(context, countryCode, sizeof(countryCode)) != 0) {
        return -1;
    }
    WifiErrorNo err = SetCountryCode(countryCode);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    return 0;
}

int RpcSetMacFilter(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int lenMac = 0;
    if (ReadInt(context, &lenMac) < 0 || lenMac <= 0) {
        return -1;
    }
    int len = lenMac + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }
    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        mac = NULL;
        return -1;
    }
    WifiErrorNo err = SetMacFilter(mac, lenMac);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    mac = NULL;
    return 0;
}

int RpcDelMacFilter(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int lenMac = 0;
    if (ReadInt(context, &lenMac) < 0 || lenMac <= 0) {
        return -1;
    }
    int len = lenMac + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }
    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        mac = NULL;
        return -1;
    }
    WifiErrorNo err = DelMacFilter(mac, lenMac);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    mac = NULL;
    return 0;
}

int RpcDisassociateSta(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int lenMac = 0;
    if (ReadInt(context, &lenMac) < 0 || lenMac <= 0) {
        return -1;
    }
    int len = lenMac + 1;
    unsigned char *mac = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (mac == NULL) {
        return -1;
    }

    if (ReadUStr(context, mac, len) != 0) {
        free(mac);
        mac = NULL;
        return -1;
    }
    WifiErrorNo err = DisassociateSta(mac, lenMac);
    WriteBegin(context, 0);
    WriteInt(context, err);
    WriteEnd(context);
    free(mac);
    mac = NULL;
    return 0;
}

int RpcGetValidFrequenciesForBand(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int band = 0;
    int size = 0;
    if (ReadInt(context, &band) < 0 || ReadInt(context, &size) < 0) {
        return -1;
    }
    if (size <= 0) {
        return -1;
    }
    int *frequencies = (int *)calloc(size, sizeof(int));
    if (frequencies == NULL) {
        return -1;
    }
    WifiErrorNo err = GetValidFrequenciesForBand(band, frequencies, &size);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, size);
        for (int i = 0; i < size; ++i) {
            WriteInt(context, frequencies[i]);
        }
    }
    WriteEnd(context);
    free(frequencies);
    frequencies = NULL;
    return 0;
}
