/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HAL_WIFIAWARE_H
#define HAL_WIFIAWARE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum {
    HAL_WIFI_SDP_PUBLISH = 0x01,
    HAL_WIFI_SDP_SUBSCRIBE = 0x02,
    HAL_WIFI_SDP_BUTT
};

typedef int (*HalRecvCallback)(unsigned char* macAddr, unsigned char peerHandle, unsigned char localHandle,
    unsigned char* msg, unsigned char len);

int HalWifiSdpInit(const char* ifname);

unsigned int HalCipherHashSha256(const char* input, unsigned int inputLen, unsigned char* hash, unsigned hashLen);

int HalWifiSdpStartService(const char* svcName, unsigned char localHandle, HalRecvCallback recvCB, unsigned char role);

int HalWifiSdpSend(unsigned char* macAddr, unsigned char peerHandle, unsigned char localHandle,
    unsigned char* msg, int len);

int HalWifiSdpStopService(unsigned char localHandle, unsigned char role);

int HalWifiSdpDeinit(void);

int HalWifiSdpAdjustTxPower(const char *ifname, signed char power);

int HalWifiSdpRestoreTxPower(const char *ifname);

int HalWifiSdpBeaconSwitch(const char *ifname, unsigned char enable);

int HalWifiSdpSetRetryTimes(unsigned int retries);

int HalWifiSdpGetSyncMode(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // HAL_WIFIAWARE_H