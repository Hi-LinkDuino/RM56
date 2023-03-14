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

#ifndef BUS_CENTER_MANAGER_H
#define BUS_CENTER_MANAGER_H

#include <stdint.h>

#include "bus_center_info_key.h"
#include "disc_manager.h"
#include "lnn_lane_info.h"
#include "softbus_bus_center.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LNN_MESSAGE_LANE = 1,
    LNN_BYTES_LANE,
    LNN_FILE_LANE,
    LNN_STREAM_LANE,
    LNN_LANE_PROPERTY_BUTT,
} LnnLaneProperty;

typedef struct {
    LnnLaneProperty prop;
    uint32_t laneNum;
    int32_t laneId[0];
} LnnLanesObject;

typedef union  {
    IServerDiscInnerCallback serverCb;
    DiscInnerCallback innerCb;
} InnerCallback;

int32_t LnnPublishService(const char *pkgName, const PublishInfo *info, bool isInnerRequest);
int32_t LnnUnPublishService(const char *pkgName, int32_t publishId, bool isInnerRequest);
int32_t LnnStartDiscDevice(const char *pkgName, const SubscribeInfo *info, const InnerCallback *cb,
    bool isInnerRequest);
int32_t LnnStopDiscDevice(const char *pkgName, int32_t subscribeId, bool isInnerRequest);

int32_t LnnGetRemoteStrInfo(const char *netWorkId, InfoKey key, char *info, uint32_t len);
int32_t LnnGetRemoteNumInfo(const char *netWorkId, InfoKey key, int32_t *info);
int32_t LnnSetLocalStrInfo(InfoKey key, const char *info);
int32_t LnnSetLocalNumInfo(InfoKey key, int32_t info);
int32_t LnnGetLocalStrInfo(InfoKey key, char *info, uint32_t len);
int32_t LnnGetLocalNumInfo(InfoKey key, int32_t *info);
int32_t LnnGetLocalNum64Info(InfoKey key, int64_t *info);
int32_t LnnSetLocalNum64Info(InfoKey key, int64_t info);
int32_t LnnGetAllOnlineNodeInfo(NodeBasicInfo **info, int32_t *infoNum);
int32_t LnnGetLocalDeviceInfo(NodeBasicInfo *info);
int32_t LnnGetNodeKeyInfo(const char *networkId, int key, uint8_t *info, uint32_t infoLen);
int32_t LnnGetNetworkIdByUuid(const char *uuid, char *buf, uint32_t len);
bool LnnIsMasterNode(void);

int32_t LnnServerJoin(ConnectionAddr *addr);
int32_t LnnServerLeave(const char *networkId);

LnnLanesObject *LnnRequestLanesObject(const char *netWorkId, int32_t pid, LnnLaneProperty prop,
    const LnnPreferredLinkList *list, uint32_t laneNum);
void LnnReleaseLanesObject(LnnLanesObject *lanesObject);
int32_t LnnGetLaneId(LnnLanesObject *lanesObject, uint32_t num);
const LnnLaneInfo *LnnGetLaneInfo(int32_t laneId);

int32_t BusCenterServerInit(void);
void BusCenterServerDeinit(void);

int32_t LnnSyncP2pInfo(void);

#ifdef __cplusplus
}
#endif
#endif // BUS_CENTER_MANAGER_H