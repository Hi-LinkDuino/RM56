/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "btm_acl.h"

#include <securec.h>

#include "hci/hci.h"
#include "hci/hci_error.h"
#include "log.h"
#include "platform/include/alarm.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/queue.h"
#include "platform/include/semaphore.h"
#include "platform/include/thread.h"

#include "btm.h"
#include "btm_acl_def.h"
#include "btm_controller.h"
#include "btm_inq_db.h"
#include "btm_le_sec.h"
#include "btm_thread.h"
#include "btm_wl.h"

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

#define COD_SIZE 3

#define CLEANUP_TIMEOUT 5000

#define REQUEST_NOT_COMPLETED 0xff

typedef enum {
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED,
} BtmAclConnectionState;

typedef struct {
    uint16_t connectionHandle;
    uint8_t transport;
    BtAddr addr;
    bool isInitiator;
    BtmAclConnectionState state;
    uint8_t refCount;
    uint8_t encryption;
    Alarm *timeoutTimer;
    union {
        struct {
            uint8_t featureStatus;
            HciLmpFeatures lmpFeatures;
            uint8_t extendedFeatureStatus;
            uint8_t maxPageNumber;
            HciExtendedLmpFeatures extendedLmpFeatures;
        } bredr;
        struct {
            uint8_t featureStatus;
            HciLeFeatures leFeatures;
        } le;
    } remoteFeatures;
    struct {
        uint8_t version;
        uint16_t manufactureName;
        uint16_t subVersion;
    } remoteVersion;
    uint8_t remoteCod[COD_SIZE];
    BtAddr leLocalAddr;
    BtAddr lePeerAddr;
} BtmAclConnection;

typedef struct {
    const BtmAclCallbacks *callbacks;
    void *context;
} BtmAclCallbacksBlock;

typedef enum {
    REMOTE_FEATURE_COMPLETE,
    REMOTE_EXTENDED_FEATURE_COMPLETE,
    REMOTE_LE_FEATURE_COMPLETE,
} BtmRemoteDeviceSupportEvent;

typedef enum {
    EDR_ACL_2MB_MODE,
    EDR_ACL_3MB_MODE,
} BtmRemoteDeviceFeature;

typedef enum {
    SECURE_SIMPLE_PAIRING_HOST_SUPPORT,
} BtmRemoteDeviceExtendedFeature;

typedef enum {
    CONNECTION_PARAMETER_REQUEST,
} BtmRemoteDeviceLeFeature;

typedef struct {
    BtAddr addr;
    uint16_t connectionHandle;
    BTMRemoteDeviceSupportCallback callback;
    BtmRemoteDeviceSupportEvent event;
    union {
        BtmRemoteDeviceFeature feature;
        BtmRemoteDeviceExtendedFeature extendedFeature;
        BtmRemoteDeviceLeFeature leFreature;
    } feature;
} BtmRemoteDeviceSupportRequest;

#define ACTION_ADD_TO_WHITE_LIST 1
#define ACTION_REMOVE_FORM_WHITE_LIST 2

typedef struct {
    uint8_t action;
    BtAddr addr;
} BtmWhiteListPendingAction;

static List *g_aclList = NULL;
static Mutex *g_aclListLock = NULL;
static List *g_aclCallbackList = NULL;
static Mutex *g_aclCallbackListLock = NULL;
static List *g_remoteSupportRequestList = NULL;

static HciEventCallbacks g_hciEventCallbacks;

static Mutex *g_cleanupMutex = NULL;
static uint16_t g_cleanupHandle = 0xffff;
static Semaphore *g_cleanupEvent = NULL;
static Alarm *g_cleanupTimer = NULL;

static uint8_t g_status = STATUS_NONE;

static uint16_t g_leScanInterval = LE_SCAN_INTERVAL_DEFAULT;
static uint16_t g_leScanWindow = LE_SCAN_WINDOW_DEFAULT;

static Mutex *g_leConnectionCancelLock = NULL;
static List *g_leConnectionCancelList = NULL;

static Queue *g_leWhiteListPendingActionQueue;

static Mutex *g_autoConnectLock = NULL;

static BtmAclConnection *AllocAclConnection()
{
    BtmAclConnection *connection = MEM_CALLOC.alloc(sizeof(BtmAclConnection));
    if (connection != NULL) {
        connection->refCount = 0;
        connection->isInitiator = false;
        connection->timeoutTimer = AlarmCreate(NULL, false);

        connection->remoteFeatures.bredr.featureStatus = REQUEST_NOT_COMPLETED;
        connection->remoteFeatures.bredr.extendedFeatureStatus = REQUEST_NOT_COMPLETED;
        connection->remoteFeatures.le.featureStatus = REQUEST_NOT_COMPLETED;
    }
    return connection;
}

static void FreeAclConnection(void *connection)
{
    BtmAclConnection *aclConnection = (BtmAclConnection *)connection;
    if (aclConnection != NULL) {
        if (aclConnection->timeoutTimer != NULL) {
            AlarmCancel(aclConnection->timeoutTimer);
            AlarmDelete(aclConnection->timeoutTimer);
            aclConnection->timeoutTimer = NULL;
        }
    }
    MEM_CALLOC.free(connection);
}

static BtmAclCallbacksBlock *BtmAllocAclCallbacksBlock(const BtmAclCallbacks *callbacks, void *context)
{
    BtmAclCallbacksBlock *block = MEM_CALLOC.alloc(sizeof(BtmAclCallbacksBlock));
    if (block != NULL) {
        block->callbacks = callbacks;
        block->context = context;
    }
    return block;
}

static void BtmFreeAclCallbacksBlock(void *block)
{
    MEM_CALLOC.free(block);
}

static void BtmAclAllocRes()
{
    g_aclListLock = MutexCreate();
    g_aclList = ListCreate(FreeAclConnection);

    g_remoteSupportRequestList = ListCreate(MEM_MALLOC.free);

    g_aclCallbackListLock = MutexCreate();
    g_aclCallbackList = ListCreate(BtmFreeAclCallbacksBlock);

    g_cleanupMutex = MutexCreate();
    g_cleanupEvent = SemaphoreCreate(0);
    g_cleanupTimer = AlarmCreate("AclCleanup", false);

    g_leScanInterval = LE_SCAN_INTERVAL_DEFAULT;
    g_leScanWindow = LE_SCAN_WINDOW_DEFAULT;

    g_leConnectionCancelLock = MutexCreate();
    g_leConnectionCancelList = ListCreate(MEM_MALLOC.free);

    g_leWhiteListPendingActionQueue = QueueCreate(UINT16_MAX);
    g_autoConnectLock = MutexCreate();
}

void BtmInitAcl()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    BtmAclAllocRes();

    g_status = STATUS_INITIALIZED;

    LOG_DEBUG("%{public}s end", __FUNCTION__);
}

static void BtmAclFreeRes()
{
    if (g_cleanupMutex != NULL) {
        MutexDelete(g_cleanupMutex);
        g_cleanupMutex = NULL;
    }
    if (g_cleanupEvent != NULL) {
        SemaphoreDelete(g_cleanupEvent);
        g_cleanupEvent = NULL;
    }
    if (g_cleanupTimer != NULL) {
        AlarmDelete(g_cleanupTimer);
        g_cleanupTimer = NULL;
    }
    if (g_aclList != NULL) {
        ListDelete(g_aclList);
        g_aclList = NULL;
    }
    if (g_remoteSupportRequestList != NULL) {
        ListDelete(g_remoteSupportRequestList);
        g_remoteSupportRequestList = NULL;
    }
    if (g_aclListLock != NULL) {
        MutexDelete(g_aclListLock);
        g_aclListLock = NULL;
    }
    if (g_aclCallbackListLock != NULL) {
        MutexDelete(g_aclCallbackListLock);
        g_aclCallbackListLock = NULL;
    }
    if (g_aclCallbackList != NULL) {
        ListDelete(g_aclCallbackList);
        g_aclCallbackList = NULL;
    }
    if (g_leConnectionCancelList != NULL) {
        ListDelete(g_leConnectionCancelList);
        g_leConnectionCancelList = NULL;
    }
    if (g_leConnectionCancelLock != NULL) {
        MutexDelete(g_leConnectionCancelLock);
        g_leConnectionCancelLock = NULL;
    }
    if (g_autoConnectLock != NULL) {
        MutexDelete(g_autoConnectLock);
        g_autoConnectLock = NULL;
    }
    if (g_leWhiteListPendingActionQueue != NULL) {
        QueueDelete(g_leWhiteListPendingActionQueue, MEM_MALLOC.free);
        g_leWhiteListPendingActionQueue = NULL;
    }
}

void BtmCloseAcl()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    g_status = STATUS_NONE;

    BtmAclFreeRes();

    LOG_DEBUG("%{public}s end", __FUNCTION__);
}

void BtmStartAcl()
{
    g_leScanInterval = LE_SCAN_INTERVAL_DEFAULT;
    g_leScanWindow = LE_SCAN_WINDOW_DEFAULT;

    HCI_RegisterEventCallbacks(&g_hciEventCallbacks);
}

void BtmStopAcl()
{
    HCI_DeregisterEventCallbacks(&g_hciEventCallbacks);

    MutexLock(g_aclListLock);
    ListClear(g_aclList);
    ListClear(g_remoteSupportRequestList);
    MutexUnlock(g_aclListLock);

    MutexLock(g_aclCallbackListLock);
    ListClear(g_aclCallbackList);
    MutexUnlock(g_aclCallbackListLock);

    MutexLock(g_leConnectionCancelLock);
    ListClear(g_leConnectionCancelList);
    MutexUnlock(g_leConnectionCancelLock);
}

static bool IsEqualAddr(const uint8_t addr1[BT_ADDRESS_SIZE], const uint8_t addr2[BT_ADDRESS_SIZE])
{
    bool isEqual = true;
    for (uint8_t i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr1[i] != addr2[i]) {
            isEqual = false;
            break;
        }
    }
    return isEqual;
}

static BtmAclConnection *BtmAclFindConnectionByAddr(const BtAddr *addr)
{
    BtmAclConnection *connection = NULL;

    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        connection = ListGetNodeData(node);
        if (connection->transport == TRANSPORT_BREDR) {
            if (IsEqualAddr(connection->addr.addr, addr->addr)) {
                break;
            }
        }
        connection = NULL;
        node = ListGetNextNode(node);
    }

    return connection;
}

static BtmAclConnection *BtmAclFindLeConnectionByAddr(const BtAddr *addr)
{
    BtmAclConnection *connection = NULL;

    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        connection = ListGetNodeData(node);
        if (connection->transport == TRANSPORT_LE) {
            if (IsEqualAddr(connection->addr.addr, addr->addr)) {
                break;
            }
        }
        connection = NULL;
        node = ListGetNextNode(node);
    }

    return connection;
}

static BtmAclConnection *BtmAclFindConnectionByHandle(uint16_t handle)
{
    BtmAclConnection *connection = NULL;

    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        connection = ListGetNodeData(node);
        if (connection != NULL) {
            if (connection->connectionHandle == handle) {
                break;
            }
            connection = NULL;
        }

        node = ListGetNextNode(node);
    }

    return connection;
}

static int BtmAclCreateConnection(const BtAddr *addr)
{
    uint8_t pageScanRepetitionMode = HCI_PAGE_SCAN_REPETITION_MODE_R1;
    uint16_t clockOffset = 0;

    BtmInquiryInfo inquiryInfo = {0};
    int queryResult = BtmQueryInquiryInfoByAddr(addr, &inquiryInfo);
    if (queryResult == BT_NO_ERROR) {
        pageScanRepetitionMode = inquiryInfo.pageScanRepetitionMode;
        clockOffset = inquiryInfo.clockOffset;
    }

    HciCreateConnectionParam param = {
        .bdAddr =
            {
                .raw = {0},
            },
        .packetType = (HCI_PACKET_TYPE_DM1 | HCI_PACKET_TYPE_DH1 | HCI_PACKET_TYPE_DM3 | HCI_PACKET_TYPE_DH3 |
                       HCI_PACKET_TYPE_DM5 | HCI_PACKET_TYPE_DH5),
        .pageScanRepetitionMode = pageScanRepetitionMode,
        .reserved = 0,
        .clockOffset = clockOffset,
        .allowRoleSwitch = BTM_IsControllerSupportRoleSwitch() ? HCI_ALLOW_ROLE_SWITCH : HCI_NOT_ALLOW_ROLE_SWITCH,
    };
    (void)memcpy_s(param.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_CreateConnection(&param);
}

int BTM_AclConnect(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    if (ListGetSize(g_aclList) >= BTM_MAX_ACL) {
        MutexUnlock(g_aclListLock);
        result = BT_CONNECT_NUM_MAX;
    } else {
        BtmAclConnection *connection = BtmAclFindConnectionByAddr(addr);
        if (connection != NULL) {
            if (connection->state != CONNECTING) {
                result = BT_BAD_STATUS;
            }
            MutexUnlock(g_aclListLock);
        } else {
            connection = AllocAclConnection();
            if (connection != NULL) {
                connection->addr = *addr;
                connection->transport = TRANSPORT_BREDR;
                connection->isInitiator = true;
                connection->state = CONNECTING;

                ListAddLast(g_aclList, connection);
            } else {
                MutexUnlock(g_aclListLock);
                return BT_NO_MEMORY;
            }
            result = BtmAclCreateConnection(addr);
            if (result != BT_NO_ERROR) {
                ListRemoveNode(g_aclList, connection);
            }

            MutexUnlock(g_aclListLock);
        }
    }

    return result;
}

static void BtmAclTimeoutTask(void *context)
{
    BtmAclConnection *connection = (BtmAclConnection *)context;
    MutexLock(g_aclListLock);
    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        if (connection == ListGetNodeData(node)) {
            if (connection->refCount == 0) {
                connection->state = DISCONNECTING;

                HciDisconnectParam disconnectParam = {
                    .connectionHandle = connection->connectionHandle,
                    .reason = HCI_REMOTE_USER_TERMINATED_CONNECTION,
                };
                HCI_Disconnect(&disconnectParam);
            }
            break;
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_aclListLock);
}

static void BtmAclTimeout(void *context)
{
    Thread *thread = BTM_GetProcessingThread();
    if (thread != NULL) {
        ThreadPostTask(thread, BtmAclTimeoutTask, context);
    }
}

NO_SANITIZE("cfi") static void BtmOnConnectionComplete(const HciConnectionCompleteEventParam *eventParam)
{
    if (eventParam->linkType != HCI_LINK_TYPE_ACL ||
        (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS)) {
        return;
    }

    BtAddr addr = {0};
    addr.type = BT_PUBLIC_DEVICE_ADDRESS;
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

    uint8_t cod[COD_SIZE] = {0, 0, 0};

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByAddr(&addr);
    if (connection != NULL) {
        if (eventParam->status == HCI_SUCCESS) {
            connection->connectionHandle = eventParam->connectionHandle;
            connection->state = CONNECTED;

            (void)memcpy_s(cod, COD_SIZE, connection->remoteCod, COD_SIZE);

            AlarmSet(connection->timeoutTimer,
                connection->isInitiator ? ACL_TIMEOUT : ACL_PASSIVE_TIMEOUT,
                BtmAclTimeout,
                connection);
        } else {
            ListRemoveNode(g_aclList, connection);
        }
    }
    MutexUnlock(g_aclListLock);

    if (eventParam->status == HCI_SUCCESS) {
        HciReadRemoteVersionInformationParam cmdParam = {0};
        cmdParam.connectionHandle = eventParam->connectionHandle;
        HCI_ReadRemoteVersionInformation(&cmdParam);
    }

    BtmAclConnectCompleteParam connectCompleteParam = {0};
    connectCompleteParam.status = eventParam->status;
    connectCompleteParam.connectionHandle = eventParam->connectionHandle;
    connectCompleteParam.addr = &addr;
    (void)memcpy_s(connectCompleteParam.classOfDevice, COD_SIZE, cod, COD_SIZE);
    connectCompleteParam.encyptionEnabled = eventParam->encryptionEnabled;

    MutexLock(g_aclCallbackListLock);

    ListNode *node = ListGetFirstNode(g_aclCallbackList);
    BtmAclCallbacksBlock *block = NULL;
    while (node != NULL) {
        block = (BtmAclCallbacksBlock *)ListGetNodeData(node);
        if (block->callbacks->connectionComplete != NULL) {
            block->callbacks->connectionComplete(&connectCompleteParam, block->context);
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_aclCallbackListLock);
}

static void BtmOnConnectionrequest(const HciConnectionRequestEventParam *eventParam)
{
    if (eventParam->linkType != HCI_LINK_TYPE_ACL) {
        return;
    }

    BtAddr addr = {
        .addr = {0},
        .type = BT_PUBLIC_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

    MutexLock(g_aclListLock);
    if (ListGetSize(g_aclList) >= BTM_MAX_ACL) {
        MutexUnlock(g_aclListLock);
        HciRejectConnectionRequestParam param = {
            .bdAddr = eventParam->bdAddr,
            .reason = HCI_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES,
        };
        HCI_RejectConnectionRequest(&param);
        return;
    }

    BtmAclConnection *connection = AllocAclConnection();
    if (connection == NULL) {
        MutexUnlock(g_aclListLock);
        return;
    }

    connection->addr = addr;
    connection->transport = TRANSPORT_BREDR;
    connection->state = CONNECTING;
    connection->isInitiator = false;

    (void)memcpy_s(connection->remoteCod, COD_SIZE, eventParam->classOfDevice, COD_SIZE);

    uint8_t role = HCI_ROLE_SLAVE;
    if (ListGetSize(g_aclList) > 0) {
        if (BTM_IsControllerSupportRoleSwitch()) {
            role = HCI_ROLE_MASTER;
        }
    }

    ListAddLast(g_aclList, connection);

    HciAcceptConnectionReqestParam acceptParam = {
        .bdAddr = eventParam->bdAddr,
        .role = role,
    };
    int result = HCI_AcceptConnectionRequest(&acceptParam);
    if (result != BT_NO_ERROR) {
        ListRemoveNode(g_aclList, connection);
    }

    MutexUnlock(g_aclListLock);
}

static void BtmConvertAddressForConnection(BtAddr *addr)
{
    bool localResolve = false;
    BtAddr address;
    if (BTM_IsControllerSupportLlPrivacy()) {
        if (BTM_IsDeviceInResolvingList(addr)) {
            int result = BTM_GetRemoteIdentityAddress(addr, &address);
            if (result == BT_NO_ERROR) {
                *addr = address;
            }
        } else {
            localResolve = true;
        }
    } else {
        localResolve = true;
    }

    if (localResolve) {
        int result = BTM_GetCurrentRemoteAddress(addr, &address);
        if (result == BT_NO_ERROR) {
            *addr = address;
        }
    }
}

static int BtmLeExtendedCreateConnection()
{
    uint8_t ownAddressType = LOCAL_ADDR_TYPE_PUBLIC;
    if (BTM_GetOwnAddressType() == OWN_ADDRESS_TYPE_RANDOM) {
        ownAddressType = BTM_IsControllerSupportLlPrivacy() ? LOCAL_ADDR_TYPE_RPA_OR_RANDOM : LOCAL_ADDR_TYPE_RANDOM;
    }

    uint8_t countOfSets = 1;
    uint8_t initiatingPhys = LE_1M_PHY;
    if (BTM_IsControllerSupportLe2MPhy()) {
        initiatingPhys |= LE_2M_PHY;
        countOfSets++;
    }
    if (BTM_IsControllerSupportLeCodedPhy()) {
        initiatingPhys |= LE_CODED_PHY;
        countOfSets++;
    }

    uint16_t leScanInterval = g_leScanInterval;
    uint16_t leScanWindow = g_leScanWindow;

    HciLeConnectionParamSet *sets = MEM_MALLOC.alloc(sizeof(HciLeConnectionParamSet) * countOfSets);
    if (sets == NULL) {
        LOG_ERROR("sets is NULL");
        return BT_NO_MEMORY;
    }
    for (uint8_t i = 0; i < countOfSets; i++) {
        sets[i].scanInterval = leScanInterval;
        sets[i].scanWindow = leScanWindow;
        sets[i].connIntervalMin = LE_CONN_INTERVAL_MIN_DEFAULT;
        sets[i].connIntervalMax = LE_CONN_INTERVAL_MAX_DEFAULT;
        sets[i].connLatency = LE_CONN_LATENCY_DEFAULT;
        sets[i].supervisionTimeout = LE_SUPERVISION_TIMEOUT_DEFAULT;
        sets[i].minimumCELength = LE_MINIMUM_CE_LENGTH_DEFAULT;
        sets[i].maximumCELength = LE_MAXIMUM_CE_LENGTH_DEFAULT;
    }

    HciLeExtendedCreateConnectionParam param = {
        .initiatingFilterPolicy = INITIATOR_FILTER_USE_WHITE_LIST,
        .peerAddressType = 0,
        .peerAddress = {.raw = {0}},
        .ownAddressType = ownAddressType,
        .initiatingPhys = initiatingPhys,
        .sets = sets,
    };

    int result = HCI_LeExtenedCreateConnection(&param);

    MEM_MALLOC.free(sets);

    return result;
}

static int BtmStartAutoConnectionInternal()
{
    if (BTM_IsControllerSupportLeExtendedAdvertising()) {
        return BtmLeExtendedCreateConnection();
    } else {
        uint8_t ownAddressType = LOCAL_ADDR_TYPE_PUBLIC;
        if (BTM_GetOwnAddressType() == OWN_ADDRESS_TYPE_RANDOM) {
            ownAddressType =
                BTM_IsControllerSupportLlPrivacy() ? LOCAL_ADDR_TYPE_RPA_OR_RANDOM : LOCAL_ADDR_TYPE_RANDOM;
        }

        uint16_t leScanInterval = g_leScanInterval;
        uint16_t leScanWindow = g_leScanWindow;

        HciLeCreateConnectionParam param = {
            .leScanInterval = leScanInterval,
            .leScanWindow = leScanWindow,
            .initiatorFilterPolicy = INITIATOR_FILTER_USE_WHITE_LIST,
            .peerAddressType = 0,
            .peerAddress =
                {
                    .raw = {0},
                },
            .ownAddressType = ownAddressType,
            .connIntervalMin = LE_CONN_INTERVAL_MIN_DEFAULT,
            .connIntervalMax = LE_CONN_INTERVAL_MAX_DEFAULT,
            .connLatency = LE_CONN_LATENCY_DEFAULT,
            .supervisionTimeout = LE_SUPERVISION_TIMEOUT_DEFAULT,
            .minimumCELength = LE_MINIMUM_CE_LENGTH_DEFAULT,
            .maximumCELength = LE_MAXIMUM_CE_LENGTH_DEFAULT,
        };
        return HCI_LeCreateConnection(&param);
    }
}

int BtmStartAutoConnection()
{
    int result;
    MutexLock(g_autoConnectLock);
    if (BtmGetDeviceCountInWhiteList() > 0) {
        result = BtmStartAutoConnectionInternal();
    } else {
        result = BT_NO_ERROR;
    }
    MutexUnlock(g_autoConnectLock);
    return result;
}

static int BtmStopAutoConnectionInternal(void)
{
    return HCI_LeCreateConnectionCancel();
}

int BtmStopAutoConnection()
{
    int result;
    MutexLock(g_autoConnectLock);
    if (BtmGetDeviceCountInWhiteList() > 0) {
        result = BtmStopAutoConnectionInternal();
    } else {
        result = BT_NO_ERROR;
    }
    MutexUnlock(g_autoConnectLock);
    return result;
}

static void BtmUpdateWhiteList()
{
    BtmWhiteListPendingAction *action = QueueTryDequeue(g_leWhiteListPendingActionQueue);
    while (action != NULL) {
        uint8_t whiteListAddressType = (action->addr.type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC
                                                                                       : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        if (action->action == ACTION_ADD_TO_WHITE_LIST) {
            BtmAddDeviceToWhiteList(whiteListAddressType, &action->addr);
        } else if (action->action == ACTION_REMOVE_FORM_WHITE_LIST) {
            BtmRemoveDeviceFromWhiteList(whiteListAddressType, &action->addr);
        }
        MEM_MALLOC.free(action);

        action = QueueTryDequeue(g_leWhiteListPendingActionQueue);
    }
}

static int BtmLeCreateConnection(const BtAddr *addr)
{
    MutexLock(g_autoConnectLock);
    if (BtmGetDeviceCountInWhiteList() > 0) {
        BtmWhiteListPendingAction *action = MEM_MALLOC.alloc(sizeof(BtmWhiteListPendingAction));
        if (action != NULL) {
            action->action = ACTION_ADD_TO_WHITE_LIST;
            action->addr = *addr;
            QueueEnqueue(g_leWhiteListPendingActionQueue, action);
        }
        MutexUnlock(g_autoConnectLock);
        return BtmStopAutoConnectionInternal();
    } else {
        uint8_t whiteListAddressType =
            (addr->type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        BtmAddDeviceToWhiteList(whiteListAddressType, addr);
        MutexUnlock(g_autoConnectLock);
        return BtmStartAutoConnectionInternal();
    }
}

int BTM_LeConnect(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    BtAddr leAddr = *addr;
    BtmConvertAddressForConnection(&leAddr);

    MutexLock(g_aclListLock);

    if (ListGetSize(g_aclList) >= BTM_MAX_ACL) {
        MutexUnlock(g_aclListLock);
        return BT_CONNECT_NUM_MAX;
    }

    BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->state != CONNECTING) {
            result = BT_BAD_STATUS;
        }
    } else {
        connection = AllocAclConnection();
        if (connection != NULL) {
            connection->addr = *addr;
            connection->transport = TRANSPORT_LE;
            connection->isInitiator = true;
            connection->state = CONNECTING;

            if (BTM_GetOwnAddressType() == OWN_ADDRESS_TYPE_RANDOM) {
                // Random Address
                BTM_GetLeRandomAddress(&connection->leLocalAddr);
            } else {
                // Public Address
                BTM_GetLocalAddr(&connection->leLocalAddr);
            }

            connection->lePeerAddr = leAddr;

            ListAddLast(g_aclList, connection);
        }

        result = BtmLeCreateConnection(&leAddr);
    }

    MutexUnlock(g_aclListLock);

    return result;
}

static void BtmConvertAddressForConnectionComplete(BtAddr *addr)
{
    BtAddr pairedAddress;
    int result = BTM_ConvertToPairedAddress(addr, &pairedAddress);
    if (result == BT_NO_ERROR) {
        *addr = pairedAddress;
    }
}

static void BtmRemoveAllConnectingLeConnection()
{
    BtmAclConnection *connection = NULL;
    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        connection = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if (connection->transport == TRANSPORT_LE && connection->state == CONNECTING) {
            ListRemoveNode(g_aclList, connection);
        }
    }
}

static void BtmUpdateLeConnectionOnConnectComplete(
    const BtAddr *addr, uint8_t peerAddrType, const HciLeConnectionCompleteEventParam *eventParam)
{
    MutexLock(g_aclListLock);

    if (eventParam->status == HCI_SUCCESS) {
        BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
        if (connection != NULL) {
            connection->connectionHandle = eventParam->connectionHandle;
            connection->state = CONNECTED;

            (void)memcpy_s(connection->lePeerAddr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);
            connection->lePeerAddr.type = peerAddrType;
        } else {
            connection = AllocAclConnection();
            if (connection == NULL) {
                MutexUnlock(g_aclListLock);
                return;
            }

            connection->addr = *addr;
            connection->connectionHandle = eventParam->connectionHandle;
            connection->transport = TRANSPORT_LE;
            connection->state = CONNECTED;
            connection->isInitiator = false;

            if (BTM_GetOwnAddressType() == OWN_ADDRESS_TYPE_RANDOM) {
                // Random Address
                BTM_GetLeRandomAddress(&connection->leLocalAddr);
            } else {
                // Public Address
                BTM_GetLocalAddr(&connection->leLocalAddr);
            }

            (void)memcpy_s(connection->lePeerAddr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);
            connection->lePeerAddr.type = peerAddrType;

            ListAddLast(g_aclList, connection);
        }
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        // Cancel. Do nothing.
    } else if (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS) {
        // Already exists. Do nothing.
    } else {
        BtmRemoveAllConnectingLeConnection();
    }

    MutexUnlock(g_aclListLock);
}

NO_SANITIZE("cfi") static void BtmLeCancelConnectCallback(uint8_t status)
{
    MutexLock(g_leConnectionCancelLock);

    ListNode *node1 = ListGetFirstNode(g_leConnectionCancelList);
    BtAddr *addr = NULL;

    ListNode *node2 = NULL;
    BtmAclCallbacksBlock *block = NULL;

    while (node1 != NULL) {
        addr = ListGetNodeData(node1);

        MutexLock(g_aclCallbackListLock);
        node2 = ListGetFirstNode(g_aclCallbackList);

        while (node2 != NULL) {
            block = (BtmAclCallbacksBlock *)ListGetNodeData(node2);
            if (block->callbacks->leConnectionComplete != NULL) {
                block->callbacks->leConnectionComplete(status, 0, addr, 0, block->context);
            }
            node2 = ListGetNextNode(node2);
        }
        MutexUnlock(g_aclCallbackListLock);

        node1 = ListGetNextNode(node1);
    }

    ListClear(g_leConnectionCancelList);

    MutexUnlock(g_leConnectionCancelLock);
}

static void BtmLeReadRemoteFeatures(uint16_t connectionHandle)
{
    HciLeReadRemoteFeaturesParam cmdParam = {
        .connectionHandle = connectionHandle,
    };
    HCI_LeReadRemoteFeatures(&cmdParam);
}

NO_SANITIZE("cfi") static void BtmOnLeConnectCallback(
    const BtAddr *addrList, uint8_t addrCount, uint8_t status, uint16_t connectionHandle, uint16_t role)
{
    MutexLock(g_aclCallbackListLock);

    ListNode *node = NULL;
    BtmAclCallbacksBlock *block = NULL;

    for (uint8_t i = 0; i < addrCount; i++) {
        node = ListGetFirstNode(g_aclCallbackList);
        while (node != NULL) {
            block = (BtmAclCallbacksBlock *)ListGetNodeData(node);
            if (block->callbacks->leConnectionComplete != NULL) {
                block->callbacks->leConnectionComplete(status, connectionHandle, addrList + i, role, block->context);
            }
            node = ListGetNextNode(node);
        }
    }

    MutexUnlock(g_aclCallbackListLock);
}

static void BtmGetLeConnectingAddr(BtAddr **addrList, uint8_t *addrCount)
{
    MutexLock(g_aclListLock);

    uint8_t count = 0;
    BtmAclConnection *connection = NULL;

    ListNode *node = ListGetFirstNode(g_aclList);
    while (node != NULL) {
        connection = ListGetNodeData(node);
        if ((connection->transport == TRANSPORT_LE) && (connection->state == CONNECTING)) {
            count++;
        }
        connection = NULL;
        node = ListGetNextNode(node);
    }

    if (count > 0) {
        *addrList = MEM_CALLOC.alloc(sizeof(BtAddr) * count);
        *addrCount = count;
    }

    if (*addrList != NULL) {
        uint8_t index = 0;
        node = ListGetFirstNode(g_aclList);
        while (node != NULL) {
            connection = ListGetNodeData(node);
            if (connection->transport == TRANSPORT_LE && connection->state == CONNECTING) {
                (*addrList)[index] = connection->addr;
                index++;
            }
            connection = NULL;
            node = ListGetNextNode(node);
        }
    }

    MutexUnlock(g_aclListLock);
}

static bool IsZeroAddress(const uint8_t addr[BT_ADDRESS_SIZE])
{
    bool isZeroAddress = true;
    for (uint8_t i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr[i] != 0) {
            isZeroAddress = false;
            break;
        }
    }
    return isZeroAddress;
}

static void BtmUpdateUpdateWhiteListOnLeConnectionComplete(
    const BtAddr *addrList, const uint8_t addrCount, const HciLeConnectionCompleteEventParam *eventParam)
{
    MutexLock(g_autoConnectLock);
    BtAddr addr = {
        .addr = {0},
        .type = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC) ? BT_PUBLIC_DEVICE_ADDRESS
                                                                           : BT_RANDOM_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);

    bool restartAutoConnection = false;
    if (eventParam->status == HCI_SUCCESS) {
        uint8_t whiteListAddrType = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC)
                                        ? WHITE_LIST_ADDRESS_TYPE_PUBLIC
                                        : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        restartAutoConnection = BtmIsDeviceInWhiteList(whiteListAddrType, &addr);
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        restartAutoConnection = true;
    } else if (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS) {
        restartAutoConnection = true;
    }

    BtmUpdateWhiteList();

    if ((eventParam->status == HCI_SUCCESS) || (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS)) {
        uint8_t addrType =
            (addr.type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        BtmRemoveDeviceFromWhiteList(addrType, &addr);
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        // Cancel. Do nothing.
    } else {
        for (uint8_t i = 0; i < addrCount; i++) {
            uint8_t addrType = ((addrList + i)->type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC
                                                                                  : WHITE_LIST_ADDRESS_TYPE_RANDOM;
            BtmRemoveDeviceFromWhiteList(addrType, addrList + i);
        }
    }

    if (BtmGetDeviceCountInWhiteList() > 0 && restartAutoConnection) {
        BtmStartAutoConnection();
    }
    MutexUnlock(g_autoConnectLock);
}

static void BtmOnLeConnectionComplete(const HciLeConnectionCompleteEventParam *eventParam)
{
    uint8_t peerAddrType = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC) ? BT_PUBLIC_DEVICE_ADDRESS
                                                                                      : BT_RANDOM_DEVICE_ADDRESS;

    BtAddr addr = {
        .addr = {0},
        .type = peerAddrType,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);

    if (!IsZeroAddress(addr.addr)) {
        BtmConvertAddressForConnectionComplete(&addr);
    }

    BtAddr *addrList = NULL;
    uint8_t addrCount = 0;
    BtmGetLeConnectingAddr(&addrList, &addrCount);

    BtmUpdateLeConnectionOnConnectComplete(&addr, peerAddrType, eventParam);

    if (eventParam->status == HCI_SUCCESS) {
        BtmLeReadRemoteFeatures(eventParam->connectionHandle);
    }

    BtmUpdateUpdateWhiteListOnLeConnectionComplete(addrList, addrCount, eventParam);

    switch (eventParam->status) {
        case HCI_SUCCESS:
            BtmOnLeConnectCallback(&addr, 1, eventParam->status, eventParam->connectionHandle, eventParam->role);
            break;
        case HCI_UNKNOWN_CONNECTION_IDENTIFIER:
            BtmLeCancelConnectCallback(eventParam->status);
            break;
        case HCI_CONNECTION_ALREADY_EXISTS:
            break;
        default:
            BtmOnLeConnectCallback(
                addrList, addrCount, eventParam->status, eventParam->connectionHandle, eventParam->role);
            break;
    }

    if (addrList != NULL) {
        MEM_CALLOC.free(addrList);
    }
}

static void BtmUpdateLeConnectionOnEnhancedConnectComplete(
    BtmAclConnection *connection, uint8_t peerAddrType, const HciLeEnhancedConnectionCompleteEventParam *eventParam)
{
    connection->connectionHandle = eventParam->connectionHandle;
    connection->state = CONNECTED;

    if (!IsZeroAddress(eventParam->localResolvablePrivateAddress.raw)) {
        (void)memcpy_s(connection->leLocalAddr.addr,
            BT_ADDRESS_SIZE,
            eventParam->localResolvablePrivateAddress.raw,
            BT_ADDRESS_SIZE);
        connection->leLocalAddr.type = BT_RANDOM_DEVICE_ADDRESS;
    }
    if (!IsZeroAddress(eventParam->peerResolvablePrivateAddress.raw)) {
        (void)memcpy_s(connection->lePeerAddr.addr,
            BT_ADDRESS_SIZE,
            eventParam->peerResolvablePrivateAddress.raw,
            BT_ADDRESS_SIZE);
        connection->lePeerAddr.type = BT_RANDOM_DEVICE_ADDRESS;
    }
}

static void BtmAllocLeConnectionOnEnhancedConnectComplete(
    const BtAddr *addr, uint8_t peerAddrType, const HciLeEnhancedConnectionCompleteEventParam *eventParam)
{
    BtmAclConnection *connection = AllocAclConnection();
    if (connection == NULL) {
        return;
    }

    connection->addr = *addr;
    connection->connectionHandle = eventParam->connectionHandle;
    connection->transport = TRANSPORT_LE;
    connection->state = CONNECTED;
    connection->isInitiator = false;

    if (!IsZeroAddress(eventParam->localResolvablePrivateAddress.raw)) {
        (void)memcpy_s(connection->leLocalAddr.addr,
            BT_ADDRESS_SIZE,
            eventParam->localResolvablePrivateAddress.raw,
            BT_ADDRESS_SIZE);
        connection->leLocalAddr.type = BT_RANDOM_DEVICE_ADDRESS;
    } else {
        if (BTM_GetOwnAddressType() == OWN_ADDRESS_TYPE_RANDOM) {
            // Random Address
            BTM_GetLeRandomAddress(&connection->leLocalAddr);
        } else {
            // Public Address
            BTM_GetLocalAddr(&connection->leLocalAddr);
        }
    }

    if (!IsZeroAddress(eventParam->peerResolvablePrivateAddress.raw)) {
        (void)memcpy_s(connection->lePeerAddr.addr,
            BT_ADDRESS_SIZE,
            eventParam->peerResolvablePrivateAddress.raw,
            BT_ADDRESS_SIZE);
        connection->lePeerAddr.type = BT_RANDOM_DEVICE_ADDRESS;
    } else {
        (void)memcpy_s(connection->lePeerAddr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);
        connection->lePeerAddr.type = peerAddrType;
    }

    ListAddLast(g_aclList, connection);
}

static void BtmUpdateConnectionInfoOnLeEnhancedConnectionComplete(
    const BtAddr *addr, uint8_t peerAddrType, const HciLeEnhancedConnectionCompleteEventParam *eventParam)
{
    MutexLock(g_aclListLock);

    if (eventParam->status == HCI_SUCCESS) {
        BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
        if (connection != NULL) {
            BtmUpdateLeConnectionOnEnhancedConnectComplete(connection, peerAddrType, eventParam);
        } else {
            BtmAllocLeConnectionOnEnhancedConnectComplete(addr, peerAddrType, eventParam);
        }
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        // Cancel. Do nothing.
    } else if (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS) {
        // Already exists. Do nothing.
    } else {
        BtmRemoveAllConnectingLeConnection();
    }

    MutexUnlock(g_aclListLock);
}

static void BtmUpdateWhiteListOnLeEnhancedConnectionComplete(
    const BtAddr *addrList, const uint8_t addrCount, const HciLeEnhancedConnectionCompleteEventParam *eventParam)
{
    MutexLock(g_autoConnectLock);
    BtAddr addr = {
        .addr = {0},
        .type = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC) ? BT_PUBLIC_DEVICE_ADDRESS
                                                                           : BT_RANDOM_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);

    bool restartAutoConnection = false;
    if (eventParam->status == HCI_SUCCESS) {
        uint8_t whiteListAddrType = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC)
                                        ? WHITE_LIST_ADDRESS_TYPE_PUBLIC
                                        : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        restartAutoConnection = BtmIsDeviceInWhiteList(whiteListAddrType, &addr);
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        restartAutoConnection = true;
    } else if (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS) {
        restartAutoConnection = true;
    }

    BtmUpdateWhiteList();

    if ((eventParam->status == HCI_SUCCESS) || (eventParam->status == HCI_CONNECTION_ALREADY_EXISTS)) {
        uint8_t addrType =
            (addr.type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC : WHITE_LIST_ADDRESS_TYPE_RANDOM;
        BtmRemoveDeviceFromWhiteList(addrType, &addr);
    } else if (eventParam->status == HCI_UNKNOWN_CONNECTION_IDENTIFIER) {
        // Cancel. Do nothing.
    } else {
        for (uint8_t i = 0; i < addrCount; i++) {
            uint8_t addrType = ((addrList + i)->type == BT_PUBLIC_DEVICE_ADDRESS) ? WHITE_LIST_ADDRESS_TYPE_PUBLIC
                                                                                  : WHITE_LIST_ADDRESS_TYPE_RANDOM;
            BtmRemoveDeviceFromWhiteList(addrType, addrList + i);
        }
    }

    if (BtmGetDeviceCountInWhiteList() > 0 && restartAutoConnection) {
        BtmStartAutoConnectionInternal();
    }
    MutexUnlock(g_autoConnectLock);
}

static void BtmOnLeEnhancedConnectionComplete(const HciLeEnhancedConnectionCompleteEventParam *eventParam)
{
    uint8_t peerAddrType = (eventParam->peerAddressType == HCI_PEER_ADDR_TYPE_PUBLIC) ? BT_PUBLIC_DEVICE_ADDRESS
                                                                                      : BT_RANDOM_DEVICE_ADDRESS;

    BtAddr addr = {
        .addr = {0},
        .type = peerAddrType,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->peerAddress.raw, BT_ADDRESS_SIZE);

    if (!IsZeroAddress(addr.addr)) {
        BtmConvertAddressForConnectionComplete(&addr);
    }

    BtAddr *addrList = NULL;
    uint8_t addrCount = 0;
    BtmGetLeConnectingAddr(&addrList, &addrCount);

    BtmUpdateConnectionInfoOnLeEnhancedConnectionComplete(&addr, peerAddrType, eventParam);

    if (eventParam->status == HCI_SUCCESS) {
        BtmLeReadRemoteFeatures(eventParam->connectionHandle);
    }

    BtmUpdateWhiteListOnLeEnhancedConnectionComplete(addrList, addrCount, eventParam);

    switch (eventParam->status) {
        case HCI_SUCCESS:
            BtmOnLeConnectCallback(&addr, 1, eventParam->status, eventParam->connectionHandle, eventParam->role);
            break;
        case HCI_UNKNOWN_CONNECTION_IDENTIFIER:
            BtmLeCancelConnectCallback(eventParam->status);
            break;
        case HCI_CONNECTION_ALREADY_EXISTS:
            break;
        default:
            BtmOnLeConnectCallback(
                addrList, addrCount, eventParam->status, eventParam->connectionHandle, eventParam->role);
            break;
    }

    if (addrList != NULL) {
        MEM_CALLOC.free(addrList);
    }
}

uint8_t BTM_GetAclTranspot(uint16_t connectionHandle)
{
    if (!IS_INITIALIZED()) {
        return -1;
    }

    uint8_t transport = -1;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(connectionHandle);
    if (connection != NULL) {
        transport = connection->transport;
    }
    MutexUnlock(g_aclListLock);
    return transport;
}

int BTM_AclDisconnect(uint16_t connectionHandle, uint8_t reason)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(connectionHandle);
    if (connection != NULL) {
        if (connection->state != DISCONNECTING) {
            connection->state = DISCONNECTING;

            HciDisconnectParam param = {
                .connectionHandle = connectionHandle,
                .reason = reason,
            };
            result = HCI_Disconnect(&param);
        }
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

NO_SANITIZE("cfi") static void BtmOnDisconnectComplete(const HciDisconnectCompleteEventParam *eventParam)
{
    uint8_t transport = 0;

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        transport = connection->transport;

        if (eventParam->status == HCI_SUCCESS) {
            connection->state = DISCONNECTED;
            ListRemoveNode(g_aclList, connection);
            connection = NULL;
        }
    }
    MutexUnlock(g_aclListLock);

    MutexLock(g_aclCallbackListLock);
    ListNode *node = ListGetFirstNode(g_aclCallbackList);
    BtmAclCallbacksBlock *block = NULL;
    while (node != NULL) {
        block = (BtmAclCallbacksBlock *)ListGetNodeData(node);
        if (transport == TRANSPORT_BREDR) {
            if (block->callbacks->disconnectionComplete != NULL) {
                block->callbacks->disconnectionComplete(
                    eventParam->status, eventParam->connectionHandle, eventParam->reason, block->context);
            }
        } else if (transport == TRANSPORT_LE) {
            if (block->callbacks->leDisconnectionComplete != NULL) {
                block->callbacks->leDisconnectionComplete(
                    eventParam->status, eventParam->connectionHandle, eventParam->reason, block->context);
            }
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_aclCallbackListLock);
}

static void BtmGetRemoteDeviceSupportRequests(
    uint16_t connectionHandle, BtmRemoteDeviceSupportEvent event, List *requests)
{
    BtmRemoteDeviceSupportRequest *request = NULL;
    BtmRemoteDeviceSupportRequest *duplicated = NULL;

    ListNode *node = ListGetFirstNode(g_remoteSupportRequestList);
    while (node != NULL) {
        request = ListGetNodeData(node);
        node = ListGetNextNode(node);

        if (request->event == event && request->connectionHandle == connectionHandle) {
            duplicated = MEM_MALLOC.alloc(sizeof(BtmRemoteDeviceSupportRequest));
            if (duplicated != NULL) {
                *duplicated = *request;
                ListAddLast(requests, duplicated);
            }

            ListRemoveNode(g_remoteSupportRequestList, request);
        }
    }
}

static void BtmOnLeRemoteFeatureComplete(const BtAddr *addr, const List *requests, const HciLeFeatures *leFeatures)
{
    BtmRemoteDeviceSupportRequest *request = NULL;
    ListNode *node = ListGetFirstNode(requests);
    while (node != NULL) {
        request = ListGetNodeData(node);

        switch (request->feature.leFreature) {
            case CONNECTION_PARAMETER_REQUEST:
                request->callback(addr, HCI_SUPPORT_CONNECTION_PARAMETERS_REQUEST_PROCEDURE(leFeatures->raw));
                break;
            default:
                break;
        }

        node = ListGetNextNode(node);
    }
}

static void BtmOnLeReadRemoteFeaturesComplete(const HciLeReadRemoteFeaturesCompleteEventParam *eventParam)
{
    BtAddr addr = {0};
    HciLeFeatures leFeatures = {0};
    List *requests = ListCreate(MEM_MALLOC.free);

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        connection->remoteFeatures.le.featureStatus = eventParam->status;
        if (eventParam->status == HCI_SUCCESS) {
            connection->remoteFeatures.le.leFeatures = eventParam->leFeatures;
        }

        addr = connection->addr;
        leFeatures = connection->remoteFeatures.le.leFeatures;
        BtmGetRemoteDeviceSupportRequests(eventParam->connectionHandle, REMOTE_LE_FEATURE_COMPLETE, requests);
    }
    MutexUnlock(g_aclListLock);

    if (eventParam->status == HCI_SUCCESS) {
        HciReadRemoteVersionInformationParam cmdParam = {
            .connectionHandle = eventParam->connectionHandle,
        };
        HCI_ReadRemoteVersionInformation(&cmdParam);
    }

    if (ListGetSize(requests)) {
        BtmOnLeRemoteFeatureComplete(&addr, requests, &leFeatures);
    }

    ListDelete(requests);
}

static void BtmOnReadRemoteVersionInformationComplete(
    const HciReadRemoteVersionInformationCompleteEventParam *eventParam)
{
    uint8_t transport = 0;

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        if (eventParam->status == HCI_SUCCESS) {
            connection->remoteVersion.version = eventParam->version;
            connection->remoteVersion.manufactureName = eventParam->manufacturerName;
            connection->remoteVersion.subVersion = eventParam->subVersion;
        }

        transport = connection->transport;
    }
    MutexUnlock(g_aclListLock);

    if (transport == TRANSPORT_BREDR) {
        HciReadRemoteSupportedFeaturesParam cmdParam = {
            .connectionHandle = eventParam->connectionHandle,
        };
        HCI_ReadRemoteSupportedFeatures(&cmdParam);
    }
}

static void BtmOnRemoteFeatureComplete(const BtAddr *addr, const List *requests, const HciLmpFeatures *lmpFeatures)
{
    BtmRemoteDeviceSupportRequest *request = NULL;
    ListNode *node = ListGetFirstNode(requests);
    while (node != NULL) {
        request = ListGetNodeData(node);

        switch (request->feature.feature) {
            case EDR_ACL_2MB_MODE:
                request->callback(addr, HCI_SUPPORT_EDR_ACL_2MBS_MODE(lmpFeatures->raw));
                break;
            case EDR_ACL_3MB_MODE:
                request->callback(addr, HCI_SUPPORT_EDR_ACL_3MBS_MODE(lmpFeatures->raw));
                break;
            default:
                break;
        }

        node = ListGetNextNode(node);
    }
}

static void BtmOnReadRemoteSupportedFeaturesComplete(const HciReadRemoteSupportedFeaturesCompleteEventParam *eventParam)
{
    BtAddr addr = {0};
    HciLmpFeatures lmpFeatures = {0};
    List *requests = ListCreate(MEM_MALLOC.free);

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        connection->remoteFeatures.bredr.featureStatus = eventParam->status;
        if (eventParam->status == HCI_SUCCESS) {
            connection->remoteFeatures.bredr.lmpFeatures = eventParam->lmpFeatures;
        }

        addr = connection->addr;
        lmpFeatures = connection->remoteFeatures.bredr.lmpFeatures;
        BtmGetRemoteDeviceSupportRequests(eventParam->connectionHandle, REMOTE_FEATURE_COMPLETE, requests);
    }
    MutexUnlock(g_aclListLock);

    HciReadRemoteExtendedFeaturesParam cmdParam = {
        .connectionHandle = eventParam->connectionHandle,
        .pageNumber = 0,
    };
    HCI_ReadRemoteExtendedFeatures(&cmdParam);

    if (ListGetSize(requests)) {
        BtmOnRemoteFeatureComplete(&addr, requests, &lmpFeatures);
    }

    ListDelete(requests);
}

static void BtmOnRemoteExtendedFeatureComplete(
    const BtAddr *addr, const List *requests, const HciExtendedLmpFeatures *extendedFeatures)
{
    BtmRemoteDeviceSupportRequest *request = NULL;

    ListNode *node = ListGetFirstNode(requests);
    while (node != NULL) {
        request = ListGetNodeData(node);

        switch (request->feature.extendedFeature) {
            case SECURE_SIMPLE_PAIRING_HOST_SUPPORT:
                request->callback(addr, HCI_SUPPORT_SECURE_SIMPLE_PAIRING_HOST(extendedFeatures->page[1]));
                break;
            default:
                break;
        }

        node = ListGetNextNode(node);
    }
}

static void BtmOnReadRemoteExtendedFeaturesComplete(const HciReadRemoteExtendedFeaturesCompleteEventParam *eventParam)
{
    uint8_t nextPageNumber = 0;
    BtAddr addr = {0};
    HciExtendedLmpFeatures extendedFeatures = {0};
    List *requests = ListCreate(MEM_MALLOC.free);

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        uint8_t status = REQUEST_NOT_COMPLETED;
        if (eventParam->status == HCI_SUCCESS) {
            connection->remoteFeatures.bredr.maxPageNumber = eventParam->maximumPageNumber;
            if (eventParam->pageNumber <= MAX_EXTENED_FEATURES_PAGE_NUMBER) {
                (void)memcpy_s(connection->remoteFeatures.bredr.extendedLmpFeatures.page[eventParam->pageNumber],
                    LMP_FEATURES_SIZE,
                    eventParam->extendedLMPFeatures,
                    LMP_FEATURES_SIZE);
            }

            if (eventParam->pageNumber < eventParam->maximumPageNumber) {
                nextPageNumber = eventParam->pageNumber + 1;
            }

            if (eventParam->pageNumber == eventParam->maximumPageNumber) {
                connection->remoteFeatures.bredr.extendedFeatureStatus = eventParam->status;
                status = eventParam->status;
            }
        } else {
            connection->remoteFeatures.bredr.extendedFeatureStatus = eventParam->status;
            status = eventParam->status;
        }

        if (status != REQUEST_NOT_COMPLETED) {
            BtmGetRemoteDeviceSupportRequests(eventParam->connectionHandle, REMOTE_EXTENDED_FEATURE_COMPLETE, requests);

            addr = connection->addr;
            extendedFeatures = connection->remoteFeatures.bredr.extendedLmpFeatures;
        }
    }
    MutexUnlock(g_aclListLock);

    if (nextPageNumber > 0) {
        HciReadRemoteExtendedFeaturesParam cmdParam = {
            .connectionHandle = eventParam->connectionHandle,
            .pageNumber = nextPageNumber,
        };
        HCI_ReadRemoteExtendedFeatures(&cmdParam);
    }

    if (ListGetSize(requests)) {
        BtmOnRemoteExtendedFeatureComplete(&addr, requests, &extendedFeatures);
    }

    ListDelete(requests);
}

static void BtmOnReadRssiComplete(const HciReadRssiReturnParam *returnParam)
{
    BtAddr addr;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(returnParam->handle);
    if (connection != NULL) {
        addr = connection->addr;
    }
    MutexUnlock(g_aclListLock);

    MutexLock(g_aclCallbackListLock);
    BtmAclCallbacksBlock *block = NULL;
    ListNode *node = ListGetFirstNode(g_aclCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callbacks->readRssiComplete != NULL) {
            block->callbacks->readRssiComplete(returnParam->status, &addr, returnParam->rssi, block->context);
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_aclCallbackListLock);
}

static void BtmAclOnCommandStatus(uint8_t status, uint16_t commandOpcode)
{
    if (commandOpcode == HCI_DISCONNECT) {
        MutexLock(g_cleanupMutex);
        if (g_cleanupHandle != 0xffff) {
            AlarmCancel(g_cleanupTimer);
            SemaphorePost(g_cleanupEvent);
        }
        MutexUnlock(g_cleanupMutex);
    }
}

int BTM_RegisterAclCallbacks(const BtmAclCallbacks *callbacks, void *context)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmAclCallbacksBlock *block = BtmAllocAclCallbacksBlock(callbacks, context);
    if (block == NULL) {
        return BT_NO_MEMORY;
    }

    MutexLock(g_aclCallbackListLock);
    ListAddLast(g_aclCallbackList, block);
    MutexUnlock(g_aclCallbackListLock);
    return BT_NO_ERROR;
}

int BTM_DeregisterAclCallbacks(const BtmAclCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_aclCallbackListLock);
    BtmAclCallbacksBlock *block = NULL;
    ListNode *node = ListGetFirstNode(g_aclCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callbacks == callbacks) {
            ListRemoveNode(g_aclCallbackList, block);
            break;
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_aclCallbackListLock);
    return BT_NO_ERROR;
}

int BtmGetAclHandleByAddress(const BtAddr *addr, uint16_t *handle)
{
    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        *handle = connection->connectionHandle;
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

int BtmGetLeAclHandleByAddress(const BtAddr *addr, uint16_t *handle)
{
    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
    if (connection != NULL) {
        *handle = connection->connectionHandle;
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

int BtmGetAclAddressByHandle(const uint16_t handle, BtAddr *addr)
{
    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(handle);
    if (connection != NULL) {
        *addr = connection->addr;
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

int BTM_AclAddRef(uint16_t connectionHandle)
{
    if (!IS_INITIALIZED()) {
        return BT_OPERATION_FAILED;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(connectionHandle);
    if (connection != NULL) {
        if (connection->state == DISCONNECTING) {
            result = BT_BAD_STATUS;
        } else {
            connection->refCount++;
            AlarmCancel(connection->timeoutTimer);
        }
    } else {
        result = BT_BAD_PARAM;
    }
    MutexUnlock(g_aclListLock);

    return result;
}

void BtmReleaseConnection(BtmAclConnection *connection)
{
    if (connection->isInitiator) {
        connection->state = DISCONNECTING;

        HciDisconnectParam param = {
            .connectionHandle = connection->connectionHandle,
            .reason = HCI_REMOTE_USER_TERMINATED_CONNECTION,
        };
        HCI_Disconnect(&param);
    } else {
        if (connection->transport == TRANSPORT_BREDR) {
            AlarmSet(connection->timeoutTimer, ACL_PASSIVE_TIMEOUT, BtmAclTimeout, connection);
        } else if (connection->transport == TRANSPORT_LE) {
            connection->state = DISCONNECTING;

            HciDisconnectParam param = {
                .connectionHandle = connection->connectionHandle,
                .reason = HCI_REMOTE_USER_TERMINATED_CONNECTION,
            };
            HCI_Disconnect(&param);
        }
    }
}

void BTM_AclRelease(uint16_t connectionHandle)
{
    if (!IS_INITIALIZED()) {
        return;
    }

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(connectionHandle);
    if (connection != NULL) {
        connection->refCount--;
        if (connection->refCount == 0) {
            BtmReleaseConnection(connection);
        }
    }
    MutexUnlock(g_aclListLock);
}

int BTM_ReadRssi(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    uint16_t handle = 0xffff;

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
    if (connection != NULL && connection->state == CONNECTED) {
        handle = connection->connectionHandle;
    } else {
        MutexUnlock(g_aclListLock);
        return BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);

    HciReadRssiParam param = {
        .handle = handle,
    };
    return HCI_ReadRssi(&param);
}

int BTM_GetLeConnectionAddress(uint16_t connectionHandle, BtAddr *localAddr, BtAddr *peerAddr)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(connectionHandle);
    if (connection != NULL) {
        *localAddr = connection->leLocalAddr;
        *peerAddr = connection->lePeerAddr;
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

static void BtmOnEncryptionChange(const HciEncryptionChangeEventParam *eventParam)
{
    if (eventParam->status != HCI_SUCCESS) {
        return;
    }

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByHandle(eventParam->connectionHandle);
    if (connection != NULL) {
        connection->encryption = eventParam->encryptionEnabled;
    }
    MutexUnlock(g_aclListLock);
}

bool BTM_IsSecureConnection(const BtAddr *addr)
{
    if (!IS_INITIALIZED()) {
        return false;
    }

    bool isSecureConnection = false;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->encryption == LINK_LEVEL_ENCRYPTION_ON_AES_CCM) {
            isSecureConnection = true;
        }
    }
    MutexUnlock(g_aclListLock);
    return isSecureConnection;
}

static void BtmAclOnCleanUpTimeout(void *parameter)
{
    SemaphorePost(g_cleanupEvent);
}

void BtmCloseAclConnectionByTransport(uint8_t transport)
{
    bool noActiveConnection = false;

    BtmAclConnection *connection = NULL;
    HciDisconnectParam param = {
        .reason = HCI_REMOTE_USER_TERMINATED_CONNECTION,
    };

    while (!noActiveConnection) {
        uint16_t connectionHandle = 0xffff;
        MutexLock(g_aclListLock);
        ListNode *node = ListGetFirstNode(g_aclList);
        while (node != NULL) {
            connection = ListGetNodeData(node);
            if (connection->transport == transport && connection->state == CONNECTED) {
                connectionHandle = connection->connectionHandle;
                connection->state = DISCONNECTING;
                break;
            }
            node = ListGetNextNode(node);
        }
        MutexUnlock(g_aclListLock);

        if (connectionHandle != 0xffff) {
            MutexLock(g_cleanupMutex);
            g_cleanupHandle = connectionHandle;
            MutexUnlock(g_cleanupMutex);

            param.connectionHandle = connectionHandle;
            HCI_Disconnect(&param);

            AlarmSet(g_cleanupTimer, CLEANUP_TIMEOUT, BtmAclOnCleanUpTimeout, NULL);
            SemaphoreWait(g_cleanupEvent);

            MutexLock(g_cleanupMutex);
            g_cleanupHandle = 0xffff;
            MutexUnlock(g_cleanupMutex);
        } else {
            noActiveConnection = true;
        }
    }
}

int BTM_AclCancelConnect(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result;
    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        ListRemoveNode(g_aclList, connection);
        HciCreateConnectionCancelParam param = {
            .bdAddr =
                {
                    .raw = {0},
                },
        };
        (void)memcpy_s(param.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
        result = HCI_CreateConnectionCancel(&param);
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_aclListLock);
    return result;
}

int BTM_LeCancelConnect(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int reuslt = BT_NO_ERROR;

    MutexLock(g_aclListLock);

    BtmAclConnection *connection = BtmAclFindLeConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->state == CONNECTING) {
            MutexLock(g_leConnectionCancelLock);
            BtAddr *cancelAddr = MEM_MALLOC.alloc(sizeof(BtAddr));
            if (cancelAddr != NULL) {
                *cancelAddr = *addr;
                ListAddLast(g_leConnectionCancelList, cancelAddr);
            }
            MutexUnlock(g_leConnectionCancelLock);

            BtmWhiteListPendingAction *action = MEM_MALLOC.alloc(sizeof(BtmWhiteListPendingAction));
            if (action != NULL) {
                action->action = ACTION_REMOVE_FORM_WHITE_LIST;
                action->addr = *addr;
                BtmConvertAddressForConnection(&action->addr);
                QueueEnqueue(g_leWhiteListPendingActionQueue, action);
            }

            BtmStopAutoConnection();

            ListRemoveNode(g_aclList, connection);
        } else {
            reuslt = BT_BAD_STATUS;
        }
    } else {
        reuslt = BT_BAD_STATUS;
    }

    MutexUnlock(g_aclListLock);

    return reuslt;
}

void BTM_IsRemoteDeviceSupportHostSecureSimplePairing(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback)
{
    BtmAclConnection *connection = NULL;
    BTMRemoteDeviceSupportCallback callback_ = NULL;
    bool isSupported = false;

    MutexLock(g_aclListLock);

    connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->remoteFeatures.bredr.extendedFeatureStatus != REQUEST_NOT_COMPLETED) {
            callback_ = callback;
            isSupported =
                HCI_SUPPORT_SECURE_SIMPLE_PAIRING_HOST(connection->remoteFeatures.bredr.extendedLmpFeatures.page[1]);
        } else {
            BtmRemoteDeviceSupportRequest *request = MEM_MALLOC.alloc(sizeof(BtmRemoteDeviceSupportRequest));
            if (request != NULL) {
                request->addr = *addr;
                request->callback = callback;
                request->connectionHandle = connection->connectionHandle;
                request->event = REMOTE_EXTENDED_FEATURE_COMPLETE;
                request->feature.extendedFeature = SECURE_SIMPLE_PAIRING_HOST_SUPPORT;

                ListAddLast(g_remoteSupportRequestList, request);
            }
        }
    } else {
        callback_ = callback;
    }

    MutexUnlock(g_aclListLock);

    if (callback_ != NULL) {
        callback_(addr, isSupported);
    }
}

void BTM_IsRemoteDeviceSupportConnectionParametersRequest(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback)
{
    BtmAclConnection *connection = NULL;
    BTMRemoteDeviceSupportCallback callback_ = NULL;
    bool isSupported = false;

    MutexLock(g_aclListLock);

    connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->remoteFeatures.bredr.extendedFeatureStatus != REQUEST_NOT_COMPLETED) {
            callback_ = callback;
            isSupported =
                HCI_SUPPORT_CONNECTION_PARAMETERS_REQUEST_PROCEDURE(connection->remoteFeatures.le.leFeatures.raw);
        } else {
            BtmRemoteDeviceSupportRequest *request = MEM_MALLOC.alloc(sizeof(BtmRemoteDeviceSupportRequest));
            if (request != NULL) {
                request->addr = *addr;
                request->callback = callback;
                request->connectionHandle = connection->connectionHandle;
                request->event = REMOTE_LE_FEATURE_COMPLETE;
                request->feature.leFreature = CONNECTION_PARAMETER_REQUEST;

                ListAddLast(g_remoteSupportRequestList, request);
            }
        }
    } else {
        callback_ = callback;
    }

    MutexUnlock(g_aclListLock);

    if (callback_ != NULL) {
        callback_(addr, isSupported);
    }
}

void BTM_IsRemoteDeviceSupportEdrAcl2MbMode(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback)
{
    BtmAclConnection *connection = NULL;
    BTMRemoteDeviceSupportCallback callback_ = NULL;
    bool isSupported = false;

    MutexLock(g_aclListLock);

    connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->remoteFeatures.bredr.featureStatus != REQUEST_NOT_COMPLETED) {
            callback_ = callback;
            isSupported = HCI_SUPPORT_EDR_ACL_2MBS_MODE(connection->remoteFeatures.bredr.lmpFeatures.raw);
        } else {
            BtmRemoteDeviceSupportRequest *request = MEM_MALLOC.alloc(sizeof(BtmRemoteDeviceSupportRequest));
            if (request != NULL) {
                request->addr = *addr;
                request->callback = callback;
                request->connectionHandle = connection->connectionHandle;
                request->event = REMOTE_FEATURE_COMPLETE;
                request->feature.feature = EDR_ACL_2MB_MODE;

                ListAddLast(g_remoteSupportRequestList, request);
            }
        }
    } else {
        callback_ = callback;
    }

    MutexUnlock(g_aclListLock);

    if (callback_ != NULL) {
        callback_(addr, isSupported);
    }
}

void BTM_IsRemoteDeviceSupportEdrAcl3MbMode(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback)
{
    BtmAclConnection *connection = NULL;
    BTMRemoteDeviceSupportCallback callback_ = NULL;
    bool isSupported = false;

    MutexLock(g_aclListLock);

    connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        if (connection->remoteFeatures.bredr.featureStatus != REQUEST_NOT_COMPLETED) {
            callback_ = callback;
            isSupported = HCI_SUPPORT_EDR_ACL_3MBS_MODE(connection->remoteFeatures.bredr.lmpFeatures.raw);
        } else {
            BtmRemoteDeviceSupportRequest *request = MEM_MALLOC.alloc(sizeof(BtmRemoteDeviceSupportRequest));
            if (request != NULL) {
                request->addr = *addr;
                request->callback = callback;
                request->connectionHandle = connection->connectionHandle;
                request->event = REMOTE_FEATURE_COMPLETE;
                request->feature.feature = EDR_ACL_3MB_MODE;

                ListAddLast(g_remoteSupportRequestList, request);
            }
        }
    } else {
        callback_ = callback;
    }

    MutexUnlock(g_aclListLock);

    if (callback_ != NULL) {
        callback_(addr, isSupported);
    }
}

int BTM_SetDefaultLinkPolicySettings(uint16_t linkPolicySettings)
{
    HciWriteDefaultLinkPolicySettingsParam param = {
        .defaultLinkPolicySettings = HCI_LINK_POLICY_DISABLE_ALL,
    };

    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_ROLE_SWITCH) && BTM_IsControllerSupportRoleSwitch()) {
        param.defaultLinkPolicySettings |= HCI_LINK_POLICY_ENABLE_ROLE_SWITCH;
    }
    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_HOLD_MODE) && BTM_IsControllerSupportHoldMode()) {
        param.defaultLinkPolicySettings |= HCI_LINK_POLICY_ENABLE_HOLD_MODE;
    }
    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_SNIFF_MODE) && BTM_IsControllerSupportSniffMode()) {
        param.defaultLinkPolicySettings |= HCI_LINK_POLICY_ENABLE_SNIFF_MODE;
    }

    return HCI_WriteDefaultLinkPolicySettings(&param);
}

int BTM_SetLinkPolicySettings(const BtAddr *addr, uint16_t linkPolicySettings)
{
    uint16_t connectionHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(addr, &connectionHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    HciWriteLinkPolicySettingsParam param = {
        .connectionHandle = connectionHandle,
        .linkPolicySettings = HCI_LINK_POLICY_DISABLE_ALL,
    };

    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_ROLE_SWITCH) && BTM_IsControllerSupportRoleSwitch()) {
        param.linkPolicySettings |= HCI_LINK_POLICY_ENABLE_ROLE_SWITCH;
    }
    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_HOLD_MODE) && BTM_IsControllerSupportHoldMode()) {
        param.linkPolicySettings |= HCI_LINK_POLICY_ENABLE_HOLD_MODE;
    }
    if ((linkPolicySettings & BTM_LINK_POLICY_ENABLE_SNIFF_MODE) && BTM_IsControllerSupportSniffMode()) {
        param.linkPolicySettings |= HCI_LINK_POLICY_ENABLE_SNIFF_MODE;
    }

    return HCI_WriteLinkPolicySettings(&param);
}

void BtmOnRoleChange(const HciRoleChangeEventParam *eventParam)
{
    BtAddr addr = {
        .addr = {0},
        .type = BT_PUBLIC_DEVICE_ADDRESS,
    };
    (void)memcpy_s(addr.addr, BT_ADDRESS_SIZE, eventParam->bdAddr.raw, BT_ADDRESS_SIZE);

    MutexLock(g_aclCallbackListLock);
    ListNode *node = ListGetFirstNode(g_aclCallbackList);
    BtmAclCallbacksBlock *block = NULL;
    while (node != NULL) {
        block = (BtmAclCallbacksBlock *)ListGetNodeData(node);
        if (block->callbacks->roleChange != NULL) {
            block->callbacks->roleChange(eventParam->status, &addr, eventParam->newRole, block->context);
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_aclCallbackListLock);
}

int BTM_SwitchRole(const BtAddr *addr, uint8_t role)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    if (addr == NULL || (role != BTM_ROLE_MASTER && role != BTM_ROLE_SLAVE)) {
        return BT_BAD_PARAM;
    }

    uint16_t connectionHandle = 0xffff;
    int result = BtmGetAclHandleByAddress(addr, &connectionHandle);
    if (result != BT_NO_ERROR) {
        return result;
    }

    BTM_ExitSniffMode(addr);

    HciSwitchRoleParam param = {
        .bdAddr =
            {
                .raw = {0},
            },
        .role = role,
    };
    (void)memcpy_s(param.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    result = HCI_SwitchRole(&param);

    return result;
}

int BTM_SetLeConnectionModeToFast()
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_autoConnectLock);

    if (g_leScanInterval != LE_SCAN_INTERVAL_FAST || g_leScanWindow != LE_SCAN_WINDOW_FAST) {
        if (BtmGetDeviceCountInWhiteList() > 0) {
            BtmStopAutoConnectionInternal();
        }

        g_leScanInterval = LE_SCAN_INTERVAL_FAST;
        g_leScanWindow = LE_SCAN_WINDOW_FAST;
    }

    MutexUnlock(g_autoConnectLock);

    return BT_NO_ERROR;
}

int BTM_SetLeConnectionModeToSlow()
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_autoConnectLock);

    if (g_leScanInterval != LE_SCAN_INTERVAL_SLOW || g_leScanWindow != LE_SCAN_WINDOW_SLOW) {
        if (BtmGetDeviceCountInWhiteList() > 0) {
            BtmStopAutoConnectionInternal();
        }

        g_leScanInterval = LE_SCAN_INTERVAL_SLOW;
        g_leScanWindow = LE_SCAN_WINDOW_SLOW;
    }

    MutexUnlock(g_autoConnectLock);

    return BT_NO_ERROR;
}

static uint16_t BtmGenerateSupportedPacketTypes(const HciLmpFeatures *lmpFeature)
{
    uint16_t packetType = BTM_ACL_PACKET_TYPE_DH1 | BTM_ACL_PACKET_TYPE_DM1;

    if (HCI_SUPPORT_3_SLOT_PACKETS(lmpFeature->raw)) {
        packetType |= BTM_ACL_PACKET_TYPE_DH3;
        packetType |= BTM_ACL_PACKET_TYPE_DM3;
    }

    if (HCI_SUPPORT_5_SLOT_PACKETS(lmpFeature->raw)) {
        packetType |= BTM_ACL_PACKET_TYPE_DH5;
        packetType |= BTM_ACL_PACKET_TYPE_DM5;
    }

    if (!HCI_SUPPORT_EDR_ACL_2MBS_MODE(lmpFeature->raw)) {
        packetType |= BTM_ACL_PACKET_TYPE_NO_2_DH1;
        packetType |= BTM_ACL_PACKET_TYPE_NO_2_DH3;
        packetType |= BTM_ACL_PACKET_TYPE_NO_2_DH5;
    } else {
        if (!HCI_SUPPORT_3_SLOT_EDR_PACKET(lmpFeature->raw)) {
            packetType |= BTM_ACL_PACKET_TYPE_NO_2_DH3;
        }
        if (!HCI_SUPPORT_5_SLOT_EDR_PACKET(lmpFeature->raw)) {
            packetType |= BTM_ACL_PACKET_TYPE_NO_2_DH5;
        }
    }

    if (!HCI_SUPPORT_EDR_ACL_3MBS_MODE(lmpFeature->raw)) {
        packetType |= BTM_ACL_PACKET_TYPE_NO_3_DH1;
        packetType |= BTM_ACL_PACKET_TYPE_NO_3_DH3;
        packetType |= BTM_ACL_PACKET_TYPE_NO_3_DH5;
    } else {
        if (!HCI_SUPPORT_3_SLOT_EDR_PACKET(lmpFeature->raw)) {
            packetType |= BTM_ACL_PACKET_TYPE_NO_3_DH3;
        }
        if (!HCI_SUPPORT_5_SLOT_EDR_PACKET(lmpFeature->raw)) {
            packetType |= BTM_ACL_PACKET_TYPE_NO_3_DH5;
        }
    }

    return packetType;
}

int BTM_ChangeConnectionPacketType(const BtAddr *addr, uint16_t packetType)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    uint16_t connectionHandle = 0xffff;
    uint16_t peerPacketType = 0;

    MutexLock(g_aclListLock);
    BtmAclConnection *connection = BtmAclFindConnectionByAddr(addr);
    if (connection != NULL) {
        connectionHandle = connection->connectionHandle;
        peerPacketType = BtmGenerateSupportedPacketTypes(&connection->remoteFeatures.bredr.lmpFeatures);
    }
    MutexUnlock(g_aclListLock);

    if (connectionHandle == 0xffff) {
        return BT_BAD_STATUS;
    }

    HciLmpFeatures lmpFeatures = {0};
    BtmGetLocalSupportedFeature(&lmpFeatures);
    uint16_t localPacketType = BtmGenerateSupportedPacketTypes(&lmpFeatures);

    const uint16_t usedTypes = BTM_ACL_PACKET_TYPE_DM1 | BTM_ACL_PACKET_TYPE_DH1 | BTM_ACL_PACKET_TYPE_DM3 |
                               BTM_ACL_PACKET_TYPE_DH3 | BTM_ACL_PACKET_TYPE_DM5 | BTM_ACL_PACKET_TYPE_DH5;
    const uint16_t unusedTypes = BTM_ACL_PACKET_TYPE_NO_2_DH1 | BTM_ACL_PACKET_TYPE_NO_3_DH1 |
                                 BTM_ACL_PACKET_TYPE_NO_2_DH3 | BTM_ACL_PACKET_TYPE_NO_3_DH3 |
                                 BTM_ACL_PACKET_TYPE_NO_2_DH5 | BTM_ACL_PACKET_TYPE_NO_3_DH5;

    uint16_t hciPacketType = (packetType & usedTypes) & (localPacketType & usedTypes) & (peerPacketType & usedTypes);
    hciPacketType |= ((packetType & unusedTypes) | (localPacketType & unusedTypes) | (peerPacketType & unusedTypes));

    HciChangeConnectionPacketTypeParam param = {
        .connectionHandle = connectionHandle,
        .packetType = hciPacketType,
    };
    return HCI_ChangeConnectionPacketType(&param);
}

static HciEventCallbacks g_hciEventCallbacks = {
    .connectionComplete = BtmOnConnectionComplete,
    .connectionRequest = BtmOnConnectionrequest,
    .disconnectComplete = BtmOnDisconnectComplete,
    .encryptionChange = BtmOnEncryptionChange,
    .readRemoteSupportedFeaturesComplete = BtmOnReadRemoteSupportedFeaturesComplete,
    .readRemoteVersionInformationComplete = BtmOnReadRemoteVersionInformationComplete,
    .commandStatus = BtmAclOnCommandStatus,
    .roleChange = BtmOnRoleChange,
    .readRemoteExtendedFeaturesComplete = BtmOnReadRemoteExtendedFeaturesComplete,

    .readRssiComplete = BtmOnReadRssiComplete,

    .leConnectionComplete = BtmOnLeConnectionComplete,
    .leReadRemoteFeaturesComplete = BtmOnLeReadRemoteFeaturesComplete,
    .leEnhancedConnectionComplete = BtmOnLeEnhancedConnectionComplete,
};
