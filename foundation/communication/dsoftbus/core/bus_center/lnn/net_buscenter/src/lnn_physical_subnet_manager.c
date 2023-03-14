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
#include "lnn_physical_subnet_manager.h"

#include <stddef.h>
#include <string.h>

#include "lnn_network_manager.h"
#include "softbus_adapter_thread.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

#define MAX_SUPPORTED_PHYSICAL_SUBNET 4

static SoftBusMutex g_physicalSubnetsLock;
static LnnPhysicalSubnet *g_physicalSubnets[MAX_SUPPORTED_PHYSICAL_SUBNET];

#define CALL_WITH_LOCK(RET, LOCK, ACTION)                                    \
    do {                                                                     \
        ret = SoftBusMutexLock(LOCK);                                        \
        if (ret != SOFTBUS_OK) {                                             \
            HILOG_ERROR(SOFTBUS_HILOG_ID, "%s:lock mutex failed", __func__); \
            break;                                                           \
        }                                                                    \
        (RET) = (ACTION);                                                    \
        SoftBusMutexUnlock(LOCK);                                            \
    } while (false)

#define CALL_VOID_FUNC_WITH_LOCK(LOCK, ACTION)                               \
    do {                                                                     \
        if (SoftBusMutexLock(LOCK) != 0) {                                   \
            HILOG_ERROR(SOFTBUS_HILOG_ID, "%s:lock mutex failed", __func__); \
            break;                                                           \
        }                                                                    \
        (ACTION);                                                            \
        SoftBusMutexUnlock(LOCK);                                            \
    } while (false)

int32_t LnnInitPhysicalSubnetManager(void)
{
    return SoftBusMutexInit(&g_physicalSubnetsLock, NULL);
}

static void ClearSubnetManager(void)
{
    for (uint8_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] != NULL) {
            if (g_physicalSubnets[i]->Destroy != NULL) {
                g_physicalSubnets[i]->Destroy(g_physicalSubnets[i]);
            }
            g_physicalSubnets[i] = NULL;
        }
    }
}

void LnnDeinitPhysicalSubnetManager(void)
{
    CALL_VOID_FUNC_WITH_LOCK(&g_physicalSubnetsLock, ClearSubnetManager());
    if (SoftBusMutexDestroy(&g_physicalSubnetsLock) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s: destroy mutex failed!", __func__);
    }
}

static int32_t DoRegistSubnet(LnnPhysicalSubnet *subnet)
{
    for (uint8_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] == NULL) {
            g_physicalSubnets[i] = subnet;
            g_physicalSubnets[i]->OnNetifStatusChanged(g_physicalSubnets[i]);
            return SOFTBUS_OK;
        }
    }
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s: subnet list is full", __func__);
    return SOFTBUS_ERR;
}

int32_t LnnRegistPhysicalSubnet(LnnPhysicalSubnet *subnet)
{
    int32_t ret = SOFTBUS_OK;
    CALL_WITH_LOCK(ret, &g_physicalSubnetsLock, DoRegistSubnet(subnet));
    return ret;
}

static int32_t DoUnregistSubnetByType(ProtocolType type)
{
    for (uint8_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] != NULL && g_physicalSubnets[i]->protocolType == type) {
            if (g_physicalSubnets[i]->Destroy != NULL) {
                g_physicalSubnets[i]->Destroy(g_physicalSubnets[i]);
            }
            g_physicalSubnets[i] = NULL;
        }
    }
    return SOFTBUS_OK;
}

int32_t LnnUnregistPhysicalSubnetByType(ProtocolType type)
{
    int32_t ret = SOFTBUS_OK;
    CALL_WITH_LOCK(ret, &g_physicalSubnetsLock, DoUnregistSubnetByType(type));
    return ret;
}

void DoNotifyAddressChange(const char *ifName, ProtocolType protocolType)
{
    for (uint16_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] == NULL || g_physicalSubnets[i]->protocolType != protocolType) {
            continue;
        }

        if (strcmp(g_physicalSubnets[i]->ifName, ifName) != 0) {
            continue;
        }

        if (g_physicalSubnets[i]->OnNetifStatusChanged != NULL) {
            g_physicalSubnets[i]->OnNetifStatusChanged(g_physicalSubnets[i]);
        }
    }
}

void LnnNotifyPhysicalSubnetAddressChanged(const char *ifName, ProtocolType protocolType)
{
    CALL_VOID_FUNC_WITH_LOCK(&g_physicalSubnetsLock, DoNotifyAddressChange(ifName, protocolType));
}

static void EnableResetingSubnetByType(ProtocolType protocolType)
{
    for (uint16_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] == NULL || g_physicalSubnets[i]->protocolType != protocolType) {
            continue;
        }
        if (g_physicalSubnets[i]->OnSoftbusNetworkDisconnected != NULL) {
            g_physicalSubnets[i]->OnSoftbusNetworkDisconnected(g_physicalSubnets[i]);
        }
    }
}

void LnnNotifyAllTypeOffline(ConnectionAddrType type)
{
    if (type == CONNECTION_ADDR_ETH || type == CONNECTION_ADDR_WLAN || type == CONNECTION_ADDR_MAX) {
        CALL_VOID_FUNC_WITH_LOCK(&g_physicalSubnetsLock, EnableResetingSubnetByType(LNN_PROTOCOL_IP));
    }
}

static bool DoVisitSubnet(LnnVisitPhysicalSubnetCallback callback, void *data)
{
    VisitNextChoice result = CHOICE_VISIT_NEXT;
    for (uint16_t i = 0; i < MAX_SUPPORTED_PHYSICAL_SUBNET; i++) {
        if (g_physicalSubnets[i] == NULL) {
            continue;
        }
        result = callback(g_physicalSubnets[i], data);
        if (result == CHOICE_FINISH_VISITING) {
            return false;
        }
    }
    return true;
}

bool LnnVisitPhysicalSubnet(LnnVisitPhysicalSubnetCallback callback, void *data)
{
    bool ret = false;
    CALL_WITH_LOCK(ret, &g_physicalSubnetsLock, DoVisitSubnet(callback, data));
    return ret;
}
