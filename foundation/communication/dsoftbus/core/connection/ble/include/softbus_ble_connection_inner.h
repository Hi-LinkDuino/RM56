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

#ifndef BLE_CONNECTION_INNER_H
#define BLE_CONNECTION_INNER_H

#include "common_list.h"
#include "softbus_adapter_ble_gatt_server.h"
#include "softbus_conn_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_CACHE_NUM_PER_CONN 3
#define BLE_CLIENT_TYPE 0
#define BLE_SERVER_TYPE 1
#define SOFTBUS_SERVICE_UUID "11C8B310-80E4-4276-AFC0-F81590B2177F"
#define SOFTBUS_CHARA_BLENET_UUID "00002B00-0000-1000-8000-00805F9B34FB"
#define SOFTBUS_CHARA_BLECONN_UUID "00002B01-0000-1000-8000-00805F9B34FB"
#define SOFTBUS_DESCRIPTOR_CONFIGURE_UUID "00002902-0000-1000-8000-00805F9B34FB"

typedef struct {
    int32_t isUsed;
    int32_t timeStamp;
    uint32_t seq;
    uint32_t currentSize;
    char *cache;
} BleRecvCache;

typedef struct {
    ListNode node;
    uint32_t requestId;
    ConnectResult callback;
} BleRequestInfo;

typedef struct {
    ListNode node;
    int32_t halConnId;
    uint32_t connId;
    SoftBusBtAddr btBinaryAddr;
    ConnectionInfo info;
    int32_t state;
    int32_t refCount;
    int32_t mtu;
    int32_t peerType;
    ListNode requestList;
    char peerDevId[UDID_BUF_LEN];
    BleRecvCache recvCache[MAX_CACHE_NUM_PER_CONN];
} BleConnectionInfo;

typedef struct {
    int32_t halConnId;
    bool isServer;
} BleHalConnInfo;

typedef struct {
    void (*BleOnDataReceived)(bool isBleConn, BleHalConnInfo halConnInfo, uint32_t len, const char *value);
    void (*BleDisconnectCallback)(BleHalConnInfo halConnInfo);
    void (*BleConnectCallback)(int32_t halConnId, const char *bleStrMac, const SoftBusBtAddr *btAddr);
    BleConnectionInfo* (*GetBleConnInfoByHalConnId)(BleHalConnInfo halConnInfo);
} SoftBusBleConnCalback;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif