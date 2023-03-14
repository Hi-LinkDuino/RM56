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

#ifndef BUS_CENTER_EVENT_H
#define BUS_CENTER_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#include "softbus_bus_center.h"
#include "bus_center_info_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* event from system monitor */
    LNN_EVENT_IP_ADDR_CHANGED,
    LNN_EVENT_WIFI_STATE_CHANGED,
    LNN_EVENT_WLAN_PARAM,
    /* event from internal lnn */
    LNN_EVENT_NODE_ONLINE_STATE_CHANGED,
    LNN_EVENT_RELATION_CHANGED,
    LNN_EVENT_NODE_MASTER_STATE_CHANGED,
    LNN_EVENT_TYPE_MAX,
} LnnEventType;

typedef struct {
    LnnEventType event;
} LnnEventBasicInfo;

typedef struct {
    LnnEventBasicInfo basic;
    char ifName[NET_IF_NAME_LEN];
} LnnMonitorAddressChangedEvent;

typedef enum {
    SOFTBUS_WIFI_CONNECTED,
    SOFTBUS_WIFI_DISCONNECTED,
    SOFTBUS_WIFI_DISABLED,
    SOFTBUS_UNKNOWN,
} SoftBusWifiState;

typedef struct {
    LnnEventBasicInfo basic;
    uint8_t status;
} LnnMonitorWlanStateChangedEvent;

typedef struct {
    LnnEventBasicInfo basic;
    bool isOnline;
    const char *networkId;
} LnnOnlineStateEventInfo;

typedef struct {
    LnnEventBasicInfo basic;
    ConnectionAddrType type;
    uint8_t relation;
    bool isJoin;
    const char *udid;
} LnnRelationChanedEventInfo;

typedef struct {
    LnnEventBasicInfo basic;
    const char* masterNodeUDID;
    int32_t weight;
    bool isMasterNode;
} LnnMasterNodeChangedEvent;

typedef void (*LnnEventHandler)(const LnnEventBasicInfo *info);

int32_t LnnInitBusCenterEvent(void);
void LnnDeinitBusCenterEvent(void);

int32_t LnnRegisterEventHandler(LnnEventType event, LnnEventHandler handler);
void LnnUnregisterEventHandler(LnnEventType event, LnnEventHandler handler);

void LnnNotifyJoinResult(ConnectionAddr *addr,
    const char *networkId, int32_t retCode);
void LnnNotifyLeaveResult(const char *networkId, int32_t retCode);

void LnnNotifyOnlineState(bool isOnline, NodeBasicInfo *info);
void LnnNotifyBasicInfoChanged(NodeBasicInfo *info, NodeBasicInfoType type);

void LnnNotifyWlanStateChangeEvent(SoftBusWifiState state);
void LnnNotifyAddressChangedEvent(const char* ifName);
void LnnNotifyLnnRelationChanged(const char *udid, ConnectionAddrType type, uint8_t relation, bool isJoin);

void LnnNotifyTimeSyncResult(const char *pkgName, const TimeSyncResultInfo *info, int32_t retCode);

void LnnNotifyMasterNodeChanged(bool isMaster, const char* masterNodeUdid, int32_t weight);

#ifdef __cplusplus
}
#endif
#endif
