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

#ifndef WIFI_HAL_CMD_H
#define WIFI_HAL_CMD_H

#include "wifi_hal_ap_feature.h"
#include "wifi_hal_common.h"
#include "wifi_driver_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_CHANNEL_NUM 14
int32_t HalCmdGetAvailableNetwork(void);
int32_t HalCmdGetSupportType(uint8_t *supType);
int32_t HalCmdGetSupportCombo(uint64_t *supCombo, uint32_t size);
int32_t HalCmdGetDevMacAddr(const char *ifName, int32_t type, unsigned char *mac, uint8_t len);
int32_t HalCmdSetMacAddr(const char *ifName, unsigned char *mac, uint8_t len);
int32_t HalCmdSetTxPower(const char *ifName, int32_t power);
int32_t HalCmdSetCountryCode(const char *ifName, const char *code, uint32_t len);
int32_t HalCmdGetAsscociatedStas(const char *ifName, struct StaInfo *staInfo, uint32_t count, uint32_t *num);
int32_t HalCmdSetScanningMacAddress(const char *ifName, unsigned char *scanMac, uint8_t len);
int32_t HalCmdGetValidFreqWithBand(const char *ifName, int32_t band, int32_t *freqs, uint32_t size, uint32_t *num);
int32_t HalCmdGetChipId(const char *ifName, uint8_t *chipId);
int32_t HalCmdGetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num);
int32_t HalCmdSetResetDriver(const uint8_t chipId, const char *ifName);
int32_t HalCmdStartScanInner(const char *ifName, WifiScan *scan);

struct DListHead *GetNetworkHead(void);
void ClearIWiFiList(void);
void InitIWiFiList(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* WIFI_HAL_CMD_H */
