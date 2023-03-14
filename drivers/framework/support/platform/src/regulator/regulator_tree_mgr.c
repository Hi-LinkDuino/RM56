/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "regulator_tree_mgr.h"
#include "osal_mem.h"
#include "securec.h"

static struct RegulatorTreeManager *g_regulatorTreeManager = NULL;
static int RegulatorChildNodeAdd(struct RegulatorTreeInfo *pRegulator, struct RegulatorNode *child)
{
    CHECK_NULL_PTR_RETURN_VALUE(pRegulator, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(child, HDF_ERR_INVALID_PARAM);

    struct RegulatorChildNode *nodeInfo = NULL;

    nodeInfo = (struct RegulatorChildNode *)OsalMemCalloc(sizeof(*nodeInfo));
    if (nodeInfo == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_FAILURE;
    }
    nodeInfo->child = child;

    DListInsertTail(&nodeInfo->node, &pRegulator->childHead);
    HDF_LOGI("RegulatorChildNodeAdd: add %s child node success!", pRegulator->name);
    return HDF_SUCCESS;
}

void RegulatorChildListDestroy(struct RegulatorTreeInfo *pRegulator)
{
    CHECK_NULL_PTR_RETURN(pRegulator);
    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, &pRegulator->childHead, struct RegulatorChildNode, node) {
        DListRemove(&nodeInfo->node);
        OsalMemFree(nodeInfo);
    }
}

// name:the regulator node name; fun :get the node parent struct RegulatorNode
struct RegulatorNode *RegulatorTreeGetParent(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, NULL);

    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, NULL);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeGetParent: lock regulator manager fail!");
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        if (strcmp(pos->name, name) == 0) {
            if (pos->parent == NULL) {
                (void)OsalMutexUnlock(&manager->lock);
                HDF_LOGI("RegulatorTreeGetParent: %s no parent", name);
                return NULL;
            }
            (void)OsalMutexUnlock(&manager->lock);
            HDF_LOGI("RegulatorTreeGetParent: get %s parent %s success!",
                name, pos->parent->regulatorInfo.name);
            return pos->parent;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("RegulatorTreeGetParent: no %s Tree node", name);
    return NULL;
}

// next level child
static struct DListHead *RegulatorTreeGetChild(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, NULL);

    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, NULL);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeGetChild: lock regulator manager fail!");
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        if (strcmp(pos->name, name) == 0) {
            (void)OsalMutexUnlock(&manager->lock);
            HDF_LOGI("RegulatorTreeGetChild: get %s child success!", name);
            return (&pos->childHead);
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGE("RegulatorTreeGetChild: get %s child failed!", name);
    return NULL;
}

// name:the regulator node name
bool RegulatorTreeIsUpNodeComplete(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, false);

    struct RegulatorNode *node = RegulatorTreeGetParent(name);
    CHECK_NULL_PTR_RETURN_VALUE(node, false);

    if ((node->regulatorInfo.parentName == NULL) || (strlen(node->regulatorInfo.parentName) == 0)) {
        return true;
    } else {
        return RegulatorTreeIsUpNodeComplete(node->regulatorInfo.name);
    }
}

bool RegulatorTreeIsChildAlwayson(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, false);
    struct DListHead *pList = RegulatorTreeGetChild(name);
    CHECK_NULL_PTR_RETURN_VALUE(pList, false);

    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, pList, struct RegulatorChildNode, node) {
        if (nodeInfo->child->regulatorInfo.constraints.alwaysOn) {
            HDF_LOGD("RegulatorTreeIsChildAlwayson:%s's child %s alwaysOn true!",
                name, nodeInfo->child->regulatorInfo.name);
            return true;
        }
    }

    HDF_LOGD("RegulatorTreeIsChildAlwayson:%s's all child alwaysOn false!", name);
    return false;
}

bool RegulatorTreeIsChildStatusOn(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, false);
    struct DListHead *pList = RegulatorTreeGetChild(name);
    CHECK_NULL_PTR_RETURN_VALUE(pList, false);

    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, pList, struct RegulatorChildNode, node) {
        if (nodeInfo->child->regulatorInfo.status == REGULATOR_STATUS_ON) {
            HDF_LOGD("RegulatorTreeIsChildAlwayson:%s's child %s status on!",
                name, nodeInfo->child->regulatorInfo.name);
            return true;
        }
    }

    HDF_LOGD("RegulatorTreeIsChildAlwayson:%s's all child status off!", name);
    return false;
}

bool RegulatorTreeIsAllChildDisable(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, true);
    struct DListHead *pList = RegulatorTreeGetChild(name);
    CHECK_NULL_PTR_RETURN_VALUE(pList, true);

    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, pList, struct RegulatorChildNode, node) {
        if (nodeInfo->child->regulatorInfo.status == REGULATOR_STATUS_ON) {
            HDF_LOGI("RegulatorTreeIsAllChildDisable:%s's child %s on!", name, nodeInfo->child->regulatorInfo.name);
            return false;
        }
    }

    HDF_LOGI("RegulatorTreeIsAllChildDisable:%s's all child off!", name);
    return true;
}

int32_t RegulatorTreeChildForceDisable(struct RegulatorNode *node)
{
    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    struct DListHead *pList = RegulatorTreeGetChild(node->regulatorInfo.name);
    CHECK_NULL_PTR_RETURN_VALUE(pList, HDF_SUCCESS);
    
    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, pList, struct RegulatorChildNode, node) {
        if (RegulatorTreeChildForceDisable(nodeInfo->child) != HDF_SUCCESS) {
            HDF_LOGE("RegulatorTreeChildForceDisable: %s fail!", nodeInfo->child->regulatorInfo.name);
            return HDF_FAILURE;
        }

        if (OsalMutexLock(&nodeInfo->child->lock) != HDF_SUCCESS) {
            HDF_LOGE("RegulatorTreeChildForceDisable: lock regulator %s fail!", node->regulatorInfo.name);
            return HDF_ERR_DEVICE_BUSY;
        }
        if (nodeInfo->child->regulatorInfo.status == REGULATOR_STATUS_ON) {
            nodeInfo->child->ops->disable(nodeInfo->child);
            nodeInfo->child->regulatorInfo.status = REGULATOR_STATUS_OFF;
            RegulatorNodeStatusCb(nodeInfo->child);
        }
        (void)OsalMutexUnlock(&nodeInfo->child->lock);
        HDF_LOGI("RegulatorTreeChildForceDisable: child %s ForceDisable success!", nodeInfo->child->regulatorInfo.name);
    }

    return HDF_SUCCESS;
}

// if Tree regulator node not exist, then add
static int32_t RegulatorTreeManagerNodeInit(const char *name)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_FAILURE);

    struct RegulatorTreeInfo *nodeInfo = NULL;
    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeManagerNodeInit: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        if (strcmp(pos->name, name) == 0) {
            HDF_LOGI("RegulatorTreeManagerNodeInit: node %s has exists!", name);
            (void)OsalMutexUnlock(&manager->lock);
            return HDF_SUCCESS;
        }
    }

    nodeInfo = (struct RegulatorTreeInfo *)OsalMemCalloc(sizeof(*nodeInfo));
    CHECK_NULL_PTR_RETURN_VALUE(nodeInfo, HDF_FAILURE);

    DListHeadInit(&nodeInfo->childHead);
    nodeInfo->name = name;

    DListInsertTail(&nodeInfo->node, &manager->treeMgrHead);
    (void)OsalMutexUnlock(&manager->lock);
    
    HDF_LOGI("RegulatorTreeManagerNodeInit: init %s node success!", name);
    return HDF_SUCCESS;
}

// set regulator (name)'s (parent) info
static int RegulatorTreeSetParent(const char *name, struct RegulatorNode *parent)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(parent, HDF_ERR_INVALID_PARAM);
    
    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSetParent: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        if (strcmp(pos->name, name) == 0) {
            pos->parent = parent;
            (void)OsalMutexUnlock(&manager->lock);
            HDF_LOGI("RegulatorTreeSetParent: set %s parent success!", name);
            return HDF_SUCCESS;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("RegulatorTreeSetParent:%s does not exist!", name);
    return HDF_FAILURE;
}

// add (child) to regulator (name)'s childHead
static int RegulatorTreeSetChild(const char *name, struct RegulatorNode *child)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(child, HDF_ERR_INVALID_PARAM);

    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSetChild: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        if (strcmp(pos->name, name) == 0) {
            if (RegulatorChildNodeAdd(pos, child) != HDF_SUCCESS) {
                HDF_LOGE("RegulatorTreeSetChild: RegulatorChildNodeAdd fail!");
                (void)OsalMutexUnlock(&manager->lock);
                return HDF_FAILURE;
            }
            (void)OsalMutexUnlock(&manager->lock);
            HDF_LOGI("RegulatorTreeSetChild: set %s child success!", name);
            return HDF_SUCCESS;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("RegulatorTreeSetChild:%s does not exist!", name);
    return HDF_FAILURE;
}

/*
* name: child regulator node name
* child: child regulator node info
* parent: parent regulator node info
* process: set regultor (name)'s RegulatorTreeInfo:parent,and add (name) to (parent)'s RegulatorTreeInfo:childHead
*/
int RegulatorTreeSet(const char *name, struct RegulatorNode *child, struct RegulatorNode *parent)
{
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(child, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(parent, HDF_ERR_INVALID_PARAM);

    if (RegulatorTreeManagerNodeInit(name) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSet: RegulatorTreeManagerNodeInit %s fail!", name);
        return HDF_FAILURE;
    }
    if (RegulatorTreeSetParent(name, parent) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSet: RegulatorTreeSetParent %s fail!", name);
        return HDF_FAILURE;
    }

    if (RegulatorTreeManagerNodeInit(parent->regulatorInfo.name) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSet: RegulatorTreeManagerNodeInit %s fail!", parent->regulatorInfo.name);
        return HDF_FAILURE;
    }
    if (RegulatorTreeSetChild(parent->regulatorInfo.name, child) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeSet: RegulatorTreeSetChild %s fail!", name);
        return HDF_FAILURE;
    }

    HDF_LOGI("RegulatorTreeSet: set [%s], parent[%s] and child info success!",
        name, parent->regulatorInfo.name);
    return HDF_SUCCESS;
}

static void RegulatorTreePrintChild(const char *name, struct DListHead *childHead)
{
    CHECK_NULL_PTR_RETURN(childHead);
    CHECK_NULL_PTR_RETURN(name);

    struct RegulatorChildNode *nodeInfo = NULL;
    struct RegulatorChildNode *tmp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, childHead, struct RegulatorChildNode, node) {
        HDF_LOGI("RegulatorTreePrintChild: %s's child %s !",
            name, nodeInfo->child->regulatorInfo.name);
    }
}

void RegulatorTreePrint(void)
{
    struct RegulatorTreeInfo *pos = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN(manager);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreePrint: lock regulator manager fail!");
        return;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        HDF_LOGI("RegulatorTreePrint %s info IN ---->", pos->name);
        if (pos->parent != NULL) {
            HDF_LOGI("RegulatorTreePrint %s info, parent name[%s]",
            pos->name, pos->parent->regulatorInfo.name);
        }

        RegulatorTreePrintChild(pos->name, &pos->childHead);
        HDF_LOGI("RegulatorTreePrint %s info out---->", pos->name);
    }
    (void)OsalMutexUnlock(&manager->lock);
}

int RegulatorTreeNodeRemoveAll(void)
{
    struct RegulatorTreeInfo *nodeInfo = NULL;
    struct RegulatorTreeInfo *tmp = NULL;
    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeNodeRemoveAll: lock regulator manager fail!");
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(nodeInfo, tmp, &manager->treeMgrHead, struct RegulatorTreeInfo, node) {
        RegulatorChildListDestroy(nodeInfo);
        DListRemove(&nodeInfo->node);
        OsalMemFree(nodeInfo);
    }

    (void)OsalMutexUnlock(&manager->lock);
    return HDF_SUCCESS;
}

int RegulatorTreeManagerDestory(void)
{
    if (RegulatorTreeNodeRemoveAll() != HDF_SUCCESS) {
        HDF_LOGE("func:%s RegulatorTreeNodeRemoveAll failed", __func__);
    }

    struct RegulatorTreeManager *manager = g_regulatorTreeManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);
    OsalMutexDestroy(&manager->lock);
    OsalMemFree(manager);
    g_regulatorTreeManager = NULL;
    return HDF_SUCCESS;
}

int RegulatorTreeManagerInit(void)
{
    struct RegulatorTreeManager *manager =
        (struct RegulatorTreeManager *)OsalMemCalloc(sizeof(struct RegulatorTreeManager));
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexInit(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("RegulatorTreeManagerInit: mutex init fail");
        OsalMemFree(manager);
        return HDF_FAILURE;
    }

    DListHeadInit(&manager->treeMgrHead);
    g_regulatorTreeManager = manager;
    return HDF_SUCCESS;
}
