/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef REGULATOR_TREE_MGR_H
#define REGULATOR_TREE_MGR_H

#include "regulator_core.h"
#include "hdf_log.h"
#include "hdf_base.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct RegulatorChildNode {
    struct RegulatorNode *child;
    struct DListHead node;
};

struct RegulatorTreeInfo {
    const char *name;                  /* regulator name */
    struct RegulatorNode *parent;    /* regulator parent info */
    struct DListHead node;
    struct DListHead childHead;      /* next level child regulator list */
};

struct RegulatorTreeManager {
    struct DListHead treeMgrHead;
    struct OsalMutex lock;
};

int RegulatorTreeManagerInit(void);
int RegulatorTreeManagerDestory(void);
int RegulatorTreeNodeRemoveAll(void);
int RegulatorTreeSet(const char *name, struct RegulatorNode *child, struct RegulatorNode *parent);
void RegulatorTreePrint(void);
struct RegulatorNode *RegulatorTreeGetParent(const char *name);
int32_t RegulatorTreeChildForceDisable(struct RegulatorNode *node);
bool RegulatorTreeIsAllChildDisable(const char *name);
bool RegulatorTreeIsChildAlwayson(const char *name);
bool RegulatorTreeIsChildStatusOn(const char *name);
bool RegulatorTreeIsUpNodeComplete(const char *name);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* REGULATOR_TREE_MGR_H */
