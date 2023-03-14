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

#include "btm.h"

#include <stdint.h>

#include "btstack.h"
#include "hci/hci.h"
#include "log.h"
#include "module.h"
#include "platform/include/alarm.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/thread.h"

#include "btm_acl.h"
#include "btm_controller.h"
#include "btm_inq_db.h"
#include "btm_le_sec.h"
#include "btm_pm.h"
#include "btm_sco.h"
#include "btm_snoop.h"
#include "btm_thread.h"
#include "btm_wl.h"

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

#define MODE_NONE 0
#define MODE_BREDR 1
#define MODE_LE 2
#define MODE_DUAL (MODE_BREDR | MODE_LE)

typedef struct {
    const BtmCallbacks *callbacks;
    void *context;
} BtmCallbacksBlock;

static uint8_t g_currentMode = MODE_NONE;
static Mutex *g_modeLock = NULL;

static HciFailureCallbacks g_hciFailureCallbacks;

static const int G_COUNT_OF_ALL_MODULES = 8;
static const char *g_allModules[] = {
    MODULE_NAME_L2CAP,
    MODULE_NAME_GAP,
    MODULE_NAME_SDP,
    MODULE_NAME_AVCTP,
    MODULE_NAME_AVDTP,
    MODULE_NAME_RFCOMM,
    MODULE_NAME_ATT,
    MODULE_NAME_SMP,
};

static const int G_COUNTOF_BREDR_AND_SHARED_MODULES = 8;
static const char *g_bredrAndSharedModules[] = {
    MODULE_NAME_L2CAP,
    MODULE_NAME_SMP,
    MODULE_NAME_GAP,
    MODULE_NAME_SDP,
    MODULE_NAME_AVCTP,
    MODULE_NAME_AVDTP,
    MODULE_NAME_RFCOMM,
    MODULE_NAME_ATT,
};

static const int G_COUNT_OF_BREDR_MODULES = 4;
static const char *g_bredrModules[] = {
    MODULE_NAME_SDP,
    MODULE_NAME_AVCTP,
    MODULE_NAME_AVDTP,
    MODULE_NAME_RFCOMM,
};

static const int G_COUNT_OF_LE_AND_SHARED_MODULES = 4;
static const char *g_leAndSharedModules[] = {
    MODULE_NAME_L2CAP,
    MODULE_NAME_SMP,
    MODULE_NAME_GAP,
    MODULE_NAME_ATT,
};

static const int G_COUNT_OF_LE_MODULES = 0;
static const char *g_leModules[] = {};

static List *g_btmCallbackList = NULL;
static Mutex *g_btmCallbackListLock = NULL;
static uint8_t g_status = STATUS_NONE;

static BtmCallbacksBlock *BtmAllocCallbacksBlock(const BtmCallbacks *callbacks, void *context)
{
    BtmCallbacksBlock *block = MEM_MALLOC.alloc(sizeof(BtmCallbacksBlock));
    if (block != NULL) {
        block->callbacks = (BtmCallbacks *)callbacks;
        block->context = context;
    }
    return block;
}

static void BtmFreeCallbacksBlock(void *block)
{
    MEM_MALLOC.free(block);
}

#ifdef DEBUG
static void BtmOutputCurrentStatus()
{
    LOG_DEBUG("BR/EDR: %{public}s LE: %{public}s",
        (g_currentMode & MODE_BREDR) ? "Enabled" : "Disabled",
        (g_currentMode & MODE_LE) ? "Enabled" : "Disabled");
}
#endif

static void BtmInitFeatures()
{
    BtmInitThread();
    BtmInitSnoop();
    BtmInitAcl();
    BtmInitSco();
    BtmInitPm();
    BtmInitInquiryDb();
    BtmInitLeSecurity();
    BtmInitWhiteList();
}

static void BtmCloseFeatures()
{
    BtmCloseWhiteList();
    BtmCloseLeSecurity();
    BtmCloseInquiryDb();
    BtmClosePm();
    BtmCloseSco();
    BtmCloseAcl();
    BtmCloseSnoop();
    BtmCloseThread();
}

int BTM_Initialize()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    int result = BT_NO_ERROR;
    do {
        int32_t ret = AlarmModuleInit();
        if (ret != 0) {
            result = BT_OPERATION_FAILED;
            break;
        }

        g_modeLock = MutexCreate();
        if (g_modeLock == NULL) {
            result = BT_OPERATION_FAILED;
            break;
        }

        g_btmCallbackList = ListCreate(BtmFreeCallbacksBlock);
        if (g_btmCallbackList == NULL) {
            result = BT_OPERATION_FAILED;
            break;
        }

        g_btmCallbackListLock = MutexCreate();
        if (g_btmCallbackListLock == NULL) {
            result = BT_OPERATION_FAILED;
            break;
        }

        BtmInitFeatures();

        LOG_DEBUG("ModuleInit start");
        for (int i = 0; i < G_COUNT_OF_ALL_MODULES; i++) {
            LOG_DEBUG("ModuleInit: %{public}s", g_allModules[i]);
            ModuleInit(g_allModules[i], 0);
        }
        LOG_DEBUG("ModuleInit end");

        g_status = STATUS_INITIALIZED;
    } while (0);

    LOG_DEBUG("%{public}s end", __FUNCTION__);

    return result;
}

int BTM_Close()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    if (BTM_Disable(LE_CONTROLLER) != BT_NO_ERROR) {
        LOG_WARN("Disable LE Failed");
    }
    if (BTM_Disable(BREDR_CONTROLLER) != BT_NO_ERROR) {
        LOG_WARN("Disable BREDR Failed");
    }

    g_status = STATUS_NONE;

    LOG_DEBUG("ModuleCleanup start");
    for (int i = G_COUNT_OF_ALL_MODULES - 1; i >= 0; i--) {
        LOG_DEBUG("ModuleCleanup: %{public}s", g_allModules[i]);
        ModuleCleanup(g_allModules[i]);
    }
    LOG_DEBUG("ModuleCleanup end");

    BtmCloseFeatures();

    if (g_btmCallbackListLock != NULL) {
        MutexDelete(g_btmCallbackListLock);
        g_btmCallbackListLock = NULL;
    }

    if (g_btmCallbackList != NULL) {
        ListDelete(g_btmCallbackList);
        g_btmCallbackList = NULL;
    }

    if (g_modeLock != NULL) {
        MutexDelete(g_modeLock);
        g_modeLock = NULL;
    }

    AlarmModuleCleanup();

    LOG_DEBUG("%{public}s end", __FUNCTION__);

    return BT_NO_ERROR;
}

static int BtmEnableBrEdrAndSharedModules()
{
    BtmStartSnoopOutput();

    int result = HCI_Initialize();
    if (result == BT_NO_ERROR) {
        HCI_RegisterFailureCallback(&g_hciFailureCallbacks);

        result = BtmInitController();
        if (result == BT_NO_ERROR) {
            BtmStartAcl();
            BtmStartPm();
            BtmStartSco();

            LOG_DEBUG("ModuleStartup start");
            for (int i = 0; i < G_COUNTOF_BREDR_AND_SHARED_MODULES; i++) {
                LOG_DEBUG("ModuleStartup: %{public}s", g_bredrAndSharedModules[i]);
                ModuleStartup(g_bredrAndSharedModules[i]);
            }
            LOG_DEBUG("ModuleStartup end");
        }
    }

    if (result != BT_NO_ERROR) {
        HCI_Close();

        BtmStopSnoopOutput();
    }

    return result;
}

static int BtmEnableBrEdrModules()
{
    BtmStartPm();
    BtmStartSco();

    LOG_DEBUG("ModuleStartup start");
    for (int i = 0; i < G_COUNT_OF_BREDR_MODULES; i++) {
        LOG_DEBUG("ModuleStartup: %{public}s", g_bredrModules[i]);
        ModuleStartup(g_bredrModules[i]);
    }
    LOG_DEBUG("ModuleStartup end");

    return BT_NO_ERROR;
}

static int BtmEnableLeAndSharedModules()
{
    BtmStartSnoopOutput();

    int result = HCI_Initialize();
    if (result == BT_NO_ERROR) {
        HCI_RegisterFailureCallback(&g_hciFailureCallbacks);

        result = BtmInitController();
        if (result == BT_NO_ERROR) {
            BtmStartAcl();
            BtmStartLeSecurity();
            BtmStartWhiteList();

            LOG_DEBUG("ModuleStartup start");
            for (int i = 0; i < G_COUNT_OF_LE_AND_SHARED_MODULES; i++) {
                LOG_DEBUG("ModuleStartup: %{public}s", g_leAndSharedModules[i]);
                ModuleStartup(g_leAndSharedModules[i]);
            }
            LOG_DEBUG("ModuleStartup end");
        }
    }

    if (result != BT_NO_ERROR) {
        HCI_Close();

        BtmStopSnoopOutput();
    }

    return result;
}

static int BtmEnableLeModules()
{
    BtmStartLeSecurity();
    BtmStartWhiteList();

    LOG_DEBUG("ModuleStartup start");
    for (int i = 0; i < G_COUNT_OF_LE_MODULES; i++) {
        LOG_DEBUG("ModuleStartup: %{public}s", g_leModules[i]);
        ModuleStartup(g_leModules[i]);
    }
    LOG_DEBUG("ModuleStartup end");

    return BT_NO_ERROR;
}

static void BtmDisableBrEdrAndSharedModules()
{
    BtmCloseAclConnectionByTransport(TRANSPORT_BREDR);

    LOG_DEBUG("ModuleShutdown start");
    for (int i = G_COUNTOF_BREDR_AND_SHARED_MODULES - 1; i >= 0; i--) {
        LOG_DEBUG("ModuleShutdown: %{public}s", g_bredrAndSharedModules[i]);
        ModuleShutdown(g_bredrAndSharedModules[i]);
    }
    LOG_DEBUG("ModuleShutdown end");

    BtmClearInquiryDb();
    BtmStopSco();
    BtmStopPm();
    BtmStopAcl();
    BtmCloseController();

    HCI_DeregisterFailureCallback(&g_hciFailureCallbacks);
    HCI_Close();

    BtmStopSnoopOutput();
}

static void BtmDisableBrEdrModules()
{
    BtmCloseAclConnectionByTransport(TRANSPORT_BREDR);

    LOG_DEBUG("ModuleShutdown start");
    for (int i = G_COUNT_OF_BREDR_MODULES - 1; i >= 0; i--) {
        LOG_DEBUG("ModuleShutdown: %{public}s", g_bredrModules[i]);
        ModuleShutdown(g_bredrModules[i]);
    }
    LOG_DEBUG("ModuleShutdown end");

    BtmClearInquiryDb();
    BtmStopSco();
    BtmStopPm();
}

static void BtmDisableLeAndSharedModules()
{
    BtmCloseAclConnectionByTransport(TRANSPORT_LE);

    LOG_DEBUG("ModuleShutdown start");
    for (int i = G_COUNT_OF_LE_AND_SHARED_MODULES - 1; i >= 0; i--) {
        LOG_DEBUG("ModuleShutdown: %{public}s", g_leAndSharedModules[i]);
        ModuleShutdown(g_leAndSharedModules[i]);
    }
    LOG_DEBUG("ModuleShutdown end");

    BtmStopWhiteList();
    BtmStopLeSecurity();
    BtmStopAcl();
    BtmCloseController();

    HCI_DeregisterFailureCallback(&g_hciFailureCallbacks);
    HCI_Close();

    BtmStopSnoopOutput();
}

static void BtmDisableLeModules()
{
    BtmCloseAclConnectionByTransport(TRANSPORT_LE);

    LOG_DEBUG("ModuleShutdown start");
    for (int i = G_COUNT_OF_LE_MODULES - 1; i >= 0; i--) {
        LOG_DEBUG("ModuleShutdown: %{public}s", g_leModules[i]);
        ModuleShutdown(g_leModules[i]);
    }
    LOG_DEBUG("ModuleShutdown end");

    BtmStopWhiteList();
    BtmStopLeSecurity();
}

int BTM_Enable(int controller)
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    if (controller != BREDR_CONTROLLER && controller != LE_CONTROLLER) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_modeLock);

    if (controller == BREDR_CONTROLLER) {
        if (g_currentMode == MODE_NONE) {
            result = BtmEnableBrEdrAndSharedModules();
        } else if (g_currentMode == MODE_LE) {
            result = BtmEnableBrEdrModules();
        }

        if (result == BT_NO_ERROR) {
            g_currentMode |= MODE_BREDR;
        }
    } else if (controller == LE_CONTROLLER) {
        if (g_currentMode == MODE_NONE) {
            result = BtmEnableLeAndSharedModules();
        } else if (g_currentMode == MODE_BREDR) {
            result = BtmEnableLeModules();
        }

        if (result == BT_NO_ERROR) {
            g_currentMode |= MODE_LE;
        }
    }

#ifdef DEBUG
    BtmOutputCurrentStatus();
#endif

    MutexUnlock(g_modeLock);
    LOG_DEBUG("%{public}s end", __FUNCTION__);
    return result;
}

int BTM_Disable(int controller)
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);
    if (controller != BREDR_CONTROLLER && controller != LE_CONTROLLER) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_modeLock);

    if (controller == BREDR_CONTROLLER) {
        if (g_currentMode == MODE_DUAL) {
            BtmDisableBrEdrModules();
        } else if (g_currentMode == MODE_BREDR) {
            BtmDisableBrEdrAndSharedModules();
        }

        g_currentMode &= (~MODE_BREDR);
    } else if (controller == LE_CONTROLLER) {
        if (g_currentMode == MODE_DUAL) {
            BtmDisableLeModules();
        } else if (g_currentMode == MODE_LE) {
            BtmDisableLeAndSharedModules();
        }

        g_currentMode &= (~MODE_LE);
    }

#ifdef DEBUG
    BtmOutputCurrentStatus();
#endif

    MutexUnlock(g_modeLock);
    LOG_DEBUG("%{public}s end", __FUNCTION__);
    return BT_NO_ERROR;
}

bool BTM_IsEnabled(int controller)
{
    if (!IS_INITIALIZED()) {
        return false;
    }

    bool isEnabled = false;

    if (controller == BREDR_CONTROLLER) {
        MutexLock(g_modeLock);
        isEnabled = !!(g_currentMode & MODE_BREDR);
        MutexUnlock(g_modeLock);
    } else if (controller == LE_CONTROLLER) {
        MutexLock(g_modeLock);
        isEnabled = !!(g_currentMode & MODE_LE);
        MutexUnlock(g_modeLock);
    }

    return isEnabled;
}

int BTM_RegisterCallbacks(const BtmCallbacks *callbacks, void *context)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmCallbacksBlock *block = BtmAllocCallbacksBlock(callbacks, context);
    if (block == NULL) {
        return BT_NO_MEMORY;
    }

    MutexLock(g_btmCallbackListLock);
    ListAddLast(g_btmCallbackList, block);
    MutexUnlock(g_btmCallbackListLock);

    return BT_NO_ERROR;
}

int BTM_DeregisterCallbacks(const BtmCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_btmCallbackListLock);

    BtmCallbacksBlock *block = NULL;
    ListNode *node = ListGetFirstNode(g_btmCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block != NULL) {
            if (block->callbacks == callbacks) {
                ListRemoveNode(g_btmCallbackList, block);
                break;
            }
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_btmCallbackListLock);

    return BT_NO_ERROR;
}

static void BtmOnHCIFailure()
{
    BtmCallbacksBlock *block = NULL;
    MutexLock(g_btmCallbackListLock);
    ListNode *node = ListGetFirstNode(g_btmCallbackList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->callbacks != NULL && block->callbacks->hciFailure != NULL) {
            block->callbacks->hciFailure(block->context);
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_btmCallbackListLock);
}

static HciFailureCallbacks g_hciFailureCallbacks = {
    .onCmdTimeout = BtmOnHCIFailure,
};
