/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "perm_operate.h"
#include <string.h>
#include "hal_pms.h"

#define RET_OK 0
#define RET_NOK (-1)

int PermissionIsGranted(const TList *list, int uid, const char *permission)
{
    TNode *cur = list->head;
    while (cur != NULL) {
        if (cur->uid != uid) {
            cur = cur->next;
            continue;
        }
        for (int i = 0; i < cur->permNum; i++) {
            if (strcmp(cur->permList[i].name, permission) == 0) {
                return (int)cur->permList[i].granted;
            }
        }
        return RET_NOK;
    }
    return RET_NOK;
}

int ModifyPermission(TNode *node, const char *permission, const enum IsGranted granted)
{
    if (node == NULL || permission == NULL) {
        return RET_NOK;
    }

    for (int i = 0; i < node->permNum; i++) {
        if (strcmp(node->permList[i].name, permission) == 0) {
            node->permList[i].granted = granted;
            return RET_OK;
        }
    }
    return RET_NOK;
}

void AddTask(TList *list, TNode *node)
{
    if (list->head == NULL) {
        list->head = node;
    } else {
        node->next = list->head;
        list->head = node;
    }
}

void DeleteTask(TList *list, int uid)
{
    TNode *cur = list->head;
    TNode *pre = NULL;
    while (cur != NULL) {
        if (cur->uid == uid) {
            if (pre == NULL) {
                list->head = cur->next;
            } else {
                pre->next = cur->next;
            }
            HalFree(cur->permList);
            HalFree(cur);
            return;
        }
        pre = cur;
        cur = cur->next;
    }
}

TNode *GetTaskWithUid(TList *list, int uid)
{
    TNode *cur = list->head;
    while (cur != NULL) {
        if (cur->uid == uid) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

TNode *GetTaskWithPkgName(TList *list, const char *pkgName)
{
    TNode *cur = list->head;
    while (cur != NULL) {
        if (strcmp(cur->pkgName, pkgName) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}
