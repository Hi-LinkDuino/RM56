/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef LNN_SUBNET_MANAGER_H
#define LNN_SUBNET_MANAGER_H

#include "lnn_network_manager.h"
#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    LNN_SUBNET_IDLE, // can be enable
    LNN_SUBNET_RUNNING,
    LNN_SUBNET_SHUTDOWN, // will not be auto Enable
    LNN_SUBNET_RESETTING, // Will be auto Enable in high pri
    LNN_SUBNET_STATUS_MAX
} LnnPhysicalSubnetStatus;

typedef struct LnnPhysicalSubnet {
    char ifName[NET_IF_NAME_LEN];
    ProtocolType protocolType;
    LnnPhysicalSubnetStatus status;
    void (*Destroy)(struct LnnPhysicalSubnet *);
    void (*OnNetifStatusChanged)(struct LnnPhysicalSubnet *);
    void (*OnSoftbusNetworkDisconnected)(struct LnnPhysicalSubnet *);
} LnnPhysicalSubnet;

typedef int16_t PhysicalSubnetId;
#define PHYSICAL_SUBNET_ID_NOT_EXIST (-1)

int32_t LnnInitPhysicalSubnetManager(void);

void LnnDeinitPhysicalSubnetManager(void);

int32_t LnnRegistPhysicalSubnet(LnnPhysicalSubnet *manager);

int32_t LnnUnregistPhysicalSubnetByType(ProtocolType type);

void LnnNotifyPhysicalSubnetAddressChanged(const char *ifName, ProtocolType type);

typedef VisitNextChoice (*LnnVisitPhysicalSubnetCallback)(const LnnPhysicalSubnet *, void *);
bool LnnVisitPhysicalSubnet(LnnVisitPhysicalSubnetCallback callback, void *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif