/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_generate_tree.h"
#include "hcs_blob_if.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hcs_generate_tree

static struct DeviceResourceNode *GetParentNode(int32_t offset, const struct TreeStack *treeStack,
    int32_t *treeLayer, int32_t configOffset)
{
    int32_t parentLayer = *treeLayer;
    while ((parentLayer > 0) && (configOffset + offset > 0)) {
        if (treeStack[parentLayer].offset >= (uint32_t)(configOffset + offset)) {
            *treeLayer = parentLayer;
            return treeStack[parentLayer].node;
        }
        parentLayer--;
    }
    return NULL;
}

static struct DeviceResourceNode *CreateTreeNode(const char *start, int32_t offset,
    struct DeviceResourceNode *parentNode, char **treeMem)
{
    struct DeviceResourceNode *newNode = (struct DeviceResourceNode *)(*treeMem);
    struct DeviceResourceNode *curNode = NULL;
    *treeMem += sizeof(struct DeviceResourceNode);
    newNode->name = start + offset + HCS_PREFIX_LENGTH;
    newNode->hashValue = (uint32_t)offset + sizeof(struct HbcHeader);
    if (parentNode != NULL) {
        newNode->parent = parentNode;
        curNode = parentNode->child;
        while ((curNode != NULL) && (curNode->sibling != NULL)) {
            curNode = curNode->sibling;
        }
        if (curNode == NULL) {
            parentNode->child = newNode;
        } else {
            curNode->sibling = newNode;
        }
    }
    return newNode;
}

static bool UpdateTreeStack(struct TreeStack **treeStack, int32_t *treeLayer, struct DeviceResourceNode *newNode,
    uint32_t offset)
{
    if (*treeLayer >= (TREE_STACK_MAX - 1)) {
        HDF_LOGE("%s failed, the treeLayer error, treeLayer: %d", __func__, *treeLayer);
        return false;
    }
    (*treeLayer)++;
    (*treeStack)[*treeLayer].node = newNode;
    (*treeStack)[*treeLayer].offset = offset;
    return true;
}

static bool AddAttrInNode(const char *start, struct DeviceResourceNode *parentNode, char **treeMem)
{
    struct DeviceResourceAttr *newAttr = NULL;
    struct DeviceResourceAttr *curAttr = NULL;
    if (parentNode == NULL) {
        HDF_LOGE("%s failed, the parentNode is NULL", __func__);
        return false;
    }
    newAttr = (struct DeviceResourceAttr *)(*treeMem);
    *treeMem += sizeof(struct DeviceResourceAttr);
    newAttr->name = start + HCS_PREFIX_LENGTH;
    newAttr->value = start + HCS_PREFIX_LENGTH + HCS_STRING_LENGTH(newAttr->name);
    curAttr = parentNode->attrData;
    parentNode->attrData = newAttr;
    newAttr->next = curAttr;
    return true;
}

static int32_t ParseByteCode(const char *treeStart, int32_t offset, char **treeMem,
    struct TreeStack **treeStack, int32_t *treeLayerOrMemLen)
{
    int32_t termOffset = HcsGetNodeOrAttrLength(treeStart + offset);
    struct DeviceResourceNode *parentOrCurNode = NULL;
    struct DeviceResourceNode *newNode = NULL;
    uint32_t newNodeOffset;
    if (termOffset <= 0) {
        HDF_LOGE("%s failed, HcsGetNodeOrAttrLength error, errno: %d", __func__, termOffset);
        return HDF_FAILURE;
    }

    switch (HcsGetPrefix(treeStart + offset)) {
        case CONFIG_NODE:
            if (*treeMem == NULL) {
                *treeLayerOrMemLen += sizeof(struct DeviceResourceNode);
                break;
            }
            parentOrCurNode = GetParentNode(offset, *treeStack, treeLayerOrMemLen, termOffset);
            newNode = CreateTreeNode(treeStart, offset, parentOrCurNode, treeMem);
            (void)HcsSwapToUint32(&newNodeOffset, treeStart + offset + HCS_STRING_LENGTH(newNode->name) +
                HCS_PREFIX_LENGTH, CONFIG_DWORD);
            newNodeOffset += (uint32_t)(offset + termOffset);
            if (!UpdateTreeStack(treeStack, treeLayerOrMemLen, newNode, newNodeOffset)) {
                return HDF_FAILURE;
            }
            break;
        case CONFIG_ATTR:
            if (*treeMem == NULL) {
                *treeLayerOrMemLen += sizeof(struct DeviceResourceAttr);
                break;
            }
            parentOrCurNode = GetParentNode(offset, *treeStack, treeLayerOrMemLen, termOffset);
            if (!AddAttrInNode(treeStart + offset, parentOrCurNode, treeMem)) {
                HDF_LOGE("%s failed, AddAttrInNode error", __func__);
                return HDF_FAILURE;
            }
            break;
        default:
            termOffset = HDF_FAILURE;
            break;
    }
    return termOffset;
}

int32_t GenerateCfgTree(const char *treeStart, int32_t length, char *treeMem, struct DeviceResourceNode **root)
{
    int32_t offset = 0;
    int32_t treeLayerOrMemLen = 0;

    struct TreeStack *treeStack = (struct TreeStack *)OsalMemCalloc(sizeof(struct TreeStack) * TREE_STACK_MAX);
    if (treeStack == NULL) {
        HDF_LOGE("%s failed, treeStack malloc error", __func__);
        return HDF_FAILURE;
    }

    while ((offset < length) && (offset >= 0)) {
        int32_t eachOffset = ParseByteCode(treeStart, offset, &treeMem, &treeStack, &treeLayerOrMemLen);
        if (eachOffset <= 0) {
            HDF_LOGE("%s failed, ParseByteCode error", __func__);
            treeLayerOrMemLen = eachOffset;
            break;
        }
        offset += eachOffset;
    }
    if ((treeMem != NULL) && (root != NULL) && (treeLayerOrMemLen > 0)) {
        // The treeStack[1] is root
        *root = treeStack[1].node;
    }
    OsalMemFree(treeStack);
    return treeLayerOrMemLen;
}
