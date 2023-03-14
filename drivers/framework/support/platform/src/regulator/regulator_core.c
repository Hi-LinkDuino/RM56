/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "regulator_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "regulator_tree_mgr.h"

#define HDF_LOG_TAG regulator_core
struct RegulatorManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct DListHead regulatorHead;
    struct OsalMutex lock;
};

static struct RegulatorManager *g_regulatorManager = NULL;

struct RegulatorNode *RegulatorNodeOpen(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, NULL);
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;

    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, NULL);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeOpen: lock regulator manager fail!");
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        if (strcmp(name, pos->regulatorInfo.name) == 0) {
            if ((pos->ops->open != NULL) && pos->ops->open(pos) != HDF_SUCCESS) {
                (void)OsalMutexUnlock(&manager->lock);
                HDF_LOGE("RegulatorNodeOpen: open regulator[%s] fail!", name);
                return NULL;
            }
            (void)OsalMutexUnlock(&manager->lock);
            return pos;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGE("RegulatorNodeOpen: No %s regulator exist", name);
    return NULL;
}

int32_t RegulatorNodeClose(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_OBJECT);
    return HDF_SUCCESS;
}

void RegulatorNodeListPrint(void)
{
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;

    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN(manager);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeListPrint: lock regulator manager fail!");
        return;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        HDF_LOGI("RegulatorNodeListPrint: name[%s], [%d][%d][%d], [%d][%d]--[%d][%d]",
            pos->regulatorInfo.name, pos->regulatorInfo.status,
            pos->regulatorInfo.constraints.alwaysOn, pos->regulatorInfo.constraints.mode,
            pos->regulatorInfo.constraints.minUv, pos->regulatorInfo.constraints.maxUv,
            pos->regulatorInfo.constraints.minUa, pos->regulatorInfo.constraints.maxUa);
        if ((pos->regulatorInfo.parentName != NULL) && (strlen(pos->regulatorInfo.parentName) > 0)) {
            HDF_LOGI("RegulatorNodeListPrint:parentName[%s]", pos->regulatorInfo.parentName);
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    RegulatorTreePrint();
}

int32_t RegulatorNodeSetParent(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeSetParent: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }
    // parent set
    if ((node->regulatorInfo.parentName != NULL)
        && (strlen(node->regulatorInfo.parentName) > 0)) {
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
            if (strcmp(node->regulatorInfo.parentName, pos->regulatorInfo.name) == 0) {
                if (RegulatorTreeSet(node->regulatorInfo.name, node, pos) != HDF_SUCCESS) {
                    HDF_LOGE("%s: RegulatorTreeSet failed", __func__);
                    (void)OsalMutexUnlock(&manager->lock);
                    return HDF_FAILURE;
                } else {
                    HDF_LOGI("%s:regulator [%s] RegulatorTreeSet success", __func__, node->regulatorInfo.name);
                    (void)OsalMutexUnlock(&manager->lock);
                    return HDF_SUCCESS;
                }
            }
        }

        HDF_LOGE("%s: RegulatorTreeSet find %s parent %s error",
            __func__, node->regulatorInfo.name, node->regulatorInfo.parentName);
        (void)OsalMutexUnlock(&manager->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGD("%s: the node %s no need to set parent", __func__, node->regulatorInfo.name);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeSetChild(struct RegulatorNode *parent)
{
    CHECK_NULL_PTR_RETURN_VALUE(parent, HDF_ERR_INVALID_PARAM);
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeSetChild: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        if ((pos->regulatorInfo.parentName != NULL) &&
            (strcmp(parent->regulatorInfo.name, pos->regulatorInfo.parentName) == 0)) {
            HDF_LOGD("%s: node[%s] parent is %s, tree info process", __func__,
                pos->regulatorInfo.parentName, parent->regulatorInfo.name);
            if (RegulatorTreeSet(pos->regulatorInfo.name, pos, parent) != HDF_SUCCESS) {
                HDF_LOGE("%s: RegulatorTreeSet failed", __func__);
                (void)OsalMutexUnlock(&manager->lock);
                return HDF_FAILURE;
            }
        }
    }

    HDF_LOGD("%s: the node %s child info process success", __func__, parent->regulatorInfo.name);
    (void)OsalMutexUnlock(&manager->lock);
    return HDF_SUCCESS;
}

void RegulatorNodeInitEnable(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN(node);
    if (node->regulatorInfo.status == REGULATOR_STATUS_ON) {
        HDF_LOGD("RegulatorNodeInitEnable: %s status on", node->regulatorInfo.name);
        return;
    }

    bool isChildAlwayson = RegulatorTreeIsChildAlwayson(node->regulatorInfo.name);
    bool isChildOn = RegulatorTreeIsChildStatusOn(node->regulatorInfo.name);
    // if regulator alwaysOn is true or there is child's alwaysOn is true or there is child's status on, then process
    if ((node->regulatorInfo.constraints.alwaysOn) || (isChildAlwayson) || (isChildOn)) {
        // RegulatorTreeIsUpNodeComplete is false, call RegulatorNodeEnable will fail.
        if (RegulatorTreeIsUpNodeComplete(node->regulatorInfo.name)) {
            HDF_LOGD("RegulatorNodeInitEnable: %s UpNodeComplete", node->regulatorInfo.name);
            RegulatorNodeEnable(node);
            return;
        }
        if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
            HDF_LOGE("RegulatorNodeInitEnable: lock regulator %s fail!", node->regulatorInfo.name);
            return;
        }
        // first enable
        if (node->ops->enable(node) != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE("RegulatorNodeInitEnable: %s failed", node->regulatorInfo.name);
            return;
        }
        node->regulatorInfo.status = REGULATOR_STATUS_ON;
        RegulatorNodeStatusCb(node);

        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGD("RegulatorNodeInitEnable: %s success", node->regulatorInfo.name);
        return;
    }
}

/*
* complete up and down management topology
* process alwayson
*/
int32_t RegulatorNodeInitProcess(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);

    // parent or child set
    if (RegulatorNodeSetParent(node) != HDF_SUCCESS) {
        HDF_LOGD("%s: node %s RegulatorNodeSetParent fail, parent not add", __func__, node->regulatorInfo.name);
    }

    if (RegulatorNodeSetChild(node) != HDF_SUCCESS) {
        HDF_LOGE("%s: node %s RegulatorNodeSetChild fail", __func__, node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    if (node->regulatorInfo.constraints.mode == REGULATOR_CHANGE_CURRENT) {
        RegulatorNodeSetCurrent(node, node->regulatorInfo.constraints.minUa, node->regulatorInfo.constraints.maxUa);
    } else if (node->regulatorInfo.constraints.mode == REGULATOR_CHANGE_VOLTAGE) {
        RegulatorNodeSetVoltage(node, node->regulatorInfo.constraints.minUv, node->regulatorInfo.constraints.maxUv);
    }

    RegulatorNodeInitEnable(node);

    HDF_LOGD("%s: the node %s init success", __func__, node->regulatorInfo.name);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeAdd(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(node->ops, HDF_ERR_INVALID_PARAM);
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        if (strcmp(node->regulatorInfo.name, pos->regulatorInfo.name) == 0) {
            HDF_LOGE("%s: regulatorInfo[%s] existed", __func__, node->regulatorInfo.name);
            return HDF_FAILURE;
        }
    }

    // init node info
    node->regulatorInfo.cb = NULL;
    node->regulatorInfo.useCount = 0;
    node->regulatorInfo.status = REGULATOR_STATUS_OFF;
    node->regulatorInfo.useCount = 0;
    if (OsalMutexInit(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexInit %s failed", __func__, node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorManagerAddNode: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }
    DListInsertTail(&node->node, &manager->regulatorHead);
    (void)OsalMutexUnlock(&manager->lock);

    if (RegulatorNodeInitProcess(node) != HDF_SUCCESS) {
        HDF_LOGE("%s: node %s RegulatorNodeInitProcess fail", __func__, node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: add regulator name[%s] success", __func__, node->regulatorInfo.name);

    return HDF_SUCCESS;
}

int32_t RegulatorNodeRemove(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_ERR_INVALID_PARAM);

    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    struct RegulatorManager *manager = g_regulatorManager;
    if (manager == NULL) {
        HDF_LOGE("RegulatorNodeRemoveAll: regulator manager null!");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeRemoveAll: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        if (strcmp(pos->regulatorInfo.name, name) == 0) {
            if ((pos->ops->release != NULL) && pos->ops->release(pos) != HDF_SUCCESS) {
                HDF_LOGE("RegulatorNodeRemoveAll: release regulator[%s] fail!", pos->regulatorInfo.name);
            }
            DListRemove(&pos->node);
            (void)OsalMutexDestroy(&pos->lock);
            OsalMemFree(pos);
            break;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove regulator %s success", __func__, name);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeRemoveAll(void)
{
    if (RegulatorTreeNodeRemoveAll() != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeRemoveAll: RegulatorTreeNodeRemoveAll failed");
        return HDF_FAILURE;
    }

    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    struct RegulatorManager *manager = g_regulatorManager;
    if (manager == NULL) {
        HDF_LOGE("RegulatorNodeRemoveAll: regulator manager null!");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeRemoveAll: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
        if ((pos->ops->release != NULL) && pos->ops->release(pos) != HDF_SUCCESS) {
            HDF_LOGE("RegulatorNodeRemoveAll: release regulator[%s] fail!", pos->regulatorInfo.name);
        }
        DListRemove(&pos->node);
        (void)OsalMutexDestroy(&pos->lock);
        OsalMemFree(pos);
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove all regulator success", __func__);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeStatusCb(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(node->regulatorInfo.cb, HDF_SUCCESS);
    struct RegulatorStatusChangeInfo info;
    (void)memset_s(&info, sizeof(struct RegulatorStatusChangeInfo), 0, sizeof(struct RegulatorStatusChangeInfo));

    info.status = node->regulatorInfo.status;
    info.name = node->regulatorInfo.name;
    HDF_LOGI("%s: Cb %s %d", __func__, info.name, info.status);

    return node->regulatorInfo.cb(&info);
}

int32_t RegulatorNodeEnable(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    if (node->regulatorInfo.status == REGULATOR_STATUS_ON) {
        HDF_LOGD("RegulatorNodeEnable: %s status on", node->regulatorInfo.name);
        return HDF_SUCCESS;
    }

    if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeEnable: lock regulator %s fail!", node->regulatorInfo.name);
        return HDF_ERR_DEVICE_BUSY;
    }

    if ((node->regulatorInfo.parentName != NULL) && (strlen(node->regulatorInfo.parentName) > 0)) {
        struct RegulatorNode *parent = RegulatorTreeGetParent(node->regulatorInfo.name);
        if (parent == NULL) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE("RegulatorNodeEnable: %s failed", node->regulatorInfo.name);
            return HDF_FAILURE;
        }
        if (RegulatorNodeEnable(parent) != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE("RegulatorNodeEnable: %s failed", parent->regulatorInfo.name);
            return HDF_FAILURE;
        }
    }

    // first enable
    if (node->ops->enable(node) != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeEnable:enable: %s failed", node->regulatorInfo.name);
        return HDF_FAILURE;
    }
    RegulatorNodeStatusCb(node);

    (void)OsalMutexUnlock(&node->lock);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeDisable(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    if ((node->regulatorInfo.status == REGULATOR_STATUS_OFF) || (node->regulatorInfo.constraints.alwaysOn)) {
        HDF_LOGI("RegulatorNodeDisable: %s [%d][%d], unsatisfied closing adjusment",
            node->regulatorInfo.name, node->regulatorInfo.status, node->regulatorInfo.constraints.alwaysOn);
        return HDF_SUCCESS;
    }

    if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeDisable: lock regulator %s fail!", node->regulatorInfo.name);
        return HDF_ERR_DEVICE_BUSY;
    }

    if (!RegulatorTreeIsAllChildDisable(node->regulatorInfo.name)) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeDisable:there is %s child not disable, so disable node failed",
            node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    // the regulator no user, can disable

    if (node->ops->disable(node) != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeDisable:disable %s failed", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    RegulatorNodeStatusCb(node);

    // set parent
    if ((node->regulatorInfo.parentName != NULL) && (strlen(node->regulatorInfo.parentName) > 0)) {
        struct RegulatorNode *parent = RegulatorTreeGetParent(node->regulatorInfo.name);
        if (parent == NULL) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE("RegulatorNodeDisable: %s failed", node->regulatorInfo.name);
            return HDF_FAILURE;
        }
        if (RegulatorNodeDisable(parent) != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGD("RegulatorNodeDisable: disable %s's parent %s failed",
                node->regulatorInfo.name, parent->regulatorInfo.name);
            return HDF_SUCCESS;
        }
    }

    (void)OsalMutexUnlock(&node->lock);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeForceDisable(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE(": lock regulator %s fail!", node->regulatorInfo.name);
        return HDF_ERR_DEVICE_BUSY;
    }

    if (node->regulatorInfo.status == REGULATOR_STATUS_OFF) {
        (void)OsalMutexUnlock(&node->lock);
        return HDF_SUCCESS;
    }

    // if the regulator force disable ,set all child node disable
    if (RegulatorTreeChildForceDisable(node)) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeForceDisable--RegulatorTreeConsumerForceDisable: %s failed", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    if (node->ops->forceDisable != NULL) {
        if (node->ops->forceDisable(node) != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE("RegulatorNodeForceDisable:forceDisable %s failed", node->regulatorInfo.name);
            return HDF_FAILURE;
        }
    } else if (node->ops->disable(node) != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeForceDisable:disable %s failed", node->regulatorInfo.name);
        return HDF_FAILURE;
    }
    node->regulatorInfo.status = REGULATOR_STATUS_OFF;
    RegulatorNodeStatusCb(node);
    HDF_LOGI("RegulatorNodeForceDisable:regulator %s force disable success", node->regulatorInfo.name);

    // set parent
    if ((node->regulatorInfo.parentName != NULL) && (strlen(node->regulatorInfo.parentName) > 0)) {
        struct RegulatorNode *parent = RegulatorTreeGetParent(node->regulatorInfo.name);
        if (parent == NULL) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGE(": %s failed", node->regulatorInfo.name);
            return HDF_FAILURE;
        }
        if (RegulatorNodeDisable(parent) != HDF_SUCCESS) {
            (void)OsalMutexUnlock(&node->lock);
            HDF_LOGD("RegulatorNodeDisable: disable %s's parent %s failed",
                node->regulatorInfo.name, parent->regulatorInfo.name);
            return HDF_SUCCESS;
        }
    }

    (void)OsalMutexUnlock(&node->lock);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeSetVoltage(struct RegulatorNode *node, uint32_t minUv, uint32_t maxUv)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    if (node->regulatorInfo.constraints.mode != REGULATOR_CHANGE_VOLTAGE) {
        HDF_LOGE("RegulatorNodeSetVoltage: %s mode %d invalid!",
            node->regulatorInfo.name, node->regulatorInfo.constraints.mode);
        return HDF_FAILURE;
    }
    
    if (minUv == node->regulatorInfo.minUv && maxUv == node->regulatorInfo.maxUv) {
        return HDF_SUCCESS;
    }

    if ((minUv > maxUv) ||
        (minUv < node->regulatorInfo.constraints.minUv ||
        maxUv > node->regulatorInfo.constraints.maxUv)) {
        HDF_LOGE("RegulatorNodeSetVoltage: %s Uv [%d, %d] invalid!",
            node->regulatorInfo.name, minUv, maxUv);
        return HDF_FAILURE;
    }

    if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeSetVoltage: lock regulator %s fail!", node->regulatorInfo.name);
        return HDF_ERR_DEVICE_BUSY;
    }

    int ret = node->ops->setVoltage(node, minUv, maxUv);
    if (ret != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeSetVoltage: setVoltage %s fail!", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    node->regulatorInfo.minUv = minUv;
    node->regulatorInfo.maxUv = maxUv;
    (void)OsalMutexUnlock(&node->lock);

    return HDF_SUCCESS;
}

int32_t RegulatorNodeGetVoltage(struct RegulatorNode *node, uint32_t *voltage)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(voltage, HDF_ERR_INVALID_PARAM);

    if (node->regulatorInfo.constraints.mode != REGULATOR_CHANGE_VOLTAGE) {
        HDF_LOGE("RegulatorNodeSetVoltage: %s mode %d invalid!",
            node->regulatorInfo.name, node->regulatorInfo.constraints.mode);
        return HDF_FAILURE;
    }

    int ret = node->ops->getVoltage(node, voltage);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeGetVoltage: getVoltage %s fail!", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorNodeSetCurrent(struct RegulatorNode *node, uint32_t minUA, uint32_t maxUA)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    if (node->regulatorInfo.constraints.mode != REGULATOR_CHANGE_CURRENT) {
        HDF_LOGE("RegulatorNodeSetVoltage: %s mode %d invalid!",
            node->regulatorInfo.name, node->regulatorInfo.constraints.mode);
        return HDF_FAILURE;
    }

    if (minUA == node->regulatorInfo.minUa && maxUA == node->regulatorInfo.maxUa) {
        return HDF_SUCCESS;
    }

    if ((minUA > maxUA) ||
        (minUA < node->regulatorInfo.constraints.minUa ||
        maxUA > node->regulatorInfo.constraints.maxUa)) {
        HDF_LOGE("RegulatorNodeSetCurrent: %s UA [%u, %u] invalid!",
            node->regulatorInfo.name, minUA, maxUA);
        return HDF_FAILURE;
    }

    if (OsalMutexLock(&node->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeSetCurrent: lock regulator %s fail!", node->regulatorInfo.name);
        return HDF_ERR_DEVICE_BUSY;
    }

    int ret = node->ops->setCurrent(node, minUA, maxUA);
    if (ret != HDF_SUCCESS) {
        (void)OsalMutexUnlock(&node->lock);
        HDF_LOGE("RegulatorNodeSetCurrent: setCurrent %s fail!", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    node->regulatorInfo.minUa = minUA;
    node->regulatorInfo.maxUa = maxUA;
    (void)OsalMutexUnlock(&node->lock);
    return HDF_SUCCESS;
}

int32_t RegulatorNodeGetCurrent(struct RegulatorNode *node, uint32_t *regCurrent)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(regCurrent, HDF_ERR_INVALID_OBJECT);
    if (node->regulatorInfo.constraints.mode != REGULATOR_CHANGE_CURRENT) {
        HDF_LOGE("RegulatorNodeGetCurrent: %s mode %d invalid!",
            node->regulatorInfo.name, node->regulatorInfo.constraints.mode);
        return HDF_FAILURE;
    }

    int ret = node->ops->getCurrent(node, regCurrent);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeGetCurrent: getCurrent %s fail!", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorNodeGetStatus(struct RegulatorNode *node, uint32_t *status)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(status, HDF_ERR_INVALID_OBJECT);
    if (node->ops->getStatus(node, status) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeGetStatus: getStatus %s fail!", node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorNodeRegisterStatusChangeCb(struct RegulatorNode *node, RegulatorStatusChangecb cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_OBJECT);
    node->regulatorInfo.cb = cb;
    return HDF_SUCCESS;
}

int32_t RegulatorTreeInfoInit(struct RegulatorNode *node)
{
    struct RegulatorNode *pos = NULL;
    struct RegulatorNode *tmp = NULL;
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_OBJECT);
    struct RegulatorManager *manager = g_regulatorManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_OBJECT);

    if ((node->regulatorInfo.parentName != NULL)
        && (strlen(node->regulatorInfo.parentName) > 0)) {
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->regulatorHead, struct RegulatorNode, node) {
            if (strcmp(node->regulatorInfo.parentName, pos->regulatorInfo.name) == 0) {
                if (RegulatorTreeSet(node->regulatorInfo.name, node, pos) != HDF_SUCCESS) {
                    HDF_LOGE("%s: RegulatorTreeSet failed", __func__);
                    return HDF_FAILURE;
                } else {
                    HDF_LOGI("%s:regulator [%s] RegulatorTreeSet success", __func__, node->regulatorInfo.name);
                    return HDF_SUCCESS;
                }
            }
        }

        HDF_LOGE("%s: RegulatorTreeSet find %s parent %s error",
            __func__, node->regulatorInfo.name, node->regulatorInfo.parentName);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t RegulatorManagerBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct RegulatorManager *manager = NULL;

    HDF_LOGI("RegulatorManagerBind: Enter!");
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_OBJECT);

    manager = (struct RegulatorManager *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("RegulatorManagerBind: malloc manager fail!");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalMutexInit(&manager->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("RegulatorManagerBind: mutex init fail:%d", ret);
        OsalMemFree(manager);
        return HDF_FAILURE;
    }

    manager->device = device;
    device->service = &manager->service;
    DListHeadInit(&manager->regulatorHead);
    g_regulatorManager = manager;

    if (RegulatorTreeManagerInit() != HDF_SUCCESS) {
        HDF_LOGE("RegulatorManagerBind: RegulatorTreeManagerInit init fail");
        OsalMemFree(manager);
        return HDF_FAILURE;
    }
    HDF_LOGI("RegulatorManagerBind: success!");
    return HDF_SUCCESS;
}

static int32_t RegulatorManagerInit(struct HdfDeviceObject *device)
{
    (void)device;

    HDF_LOGI("RegulatorManagerInit: success!");
    return HDF_SUCCESS;
}

static void RegulatorManagerRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("RegulatorManagerRelease: enter");
    CHECK_NULL_PTR_RETURN(device);

    if (RegulatorNodeRemoveAll() != HDF_SUCCESS) {
        HDF_LOGE("RegulatorNodeRemoveAll failed");
        return;
    }

    struct RegulatorManager *manager = (struct RegulatorManager *)device->service;
    CHECK_NULL_PTR_RETURN(manager);
    OsalMutexDestroy(&manager->lock);
    OsalMemFree(manager);
    g_regulatorManager = NULL;

    if (RegulatorTreeManagerDestory() != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeManagerDestory failed");
        return;
    }
}

struct HdfDriverEntry g_regulatorManagerEntry = {
    .moduleVersion = 1,
    .Bind = RegulatorManagerBind,
    .Init = RegulatorManagerInit,
    .Release = RegulatorManagerRelease,
    .moduleName = "HDF_PLATFORM_REGULATOR_MANAGER",
};

HDF_INIT(g_regulatorManagerEntry);
