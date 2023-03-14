/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_tree_if.h"
#include "hcs_blob_if.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hcs_tree_if

static struct DeviceResourceAttr *GetAttrInNode(const struct DeviceResourceNode *node, const char *attrName)
{
    struct DeviceResourceAttr *attr = NULL;
    if ((node == NULL) || (attrName == NULL)) {
        return NULL;
    }
    for (attr = node->attrData; attr != NULL; attr = attr->next) {
        if ((attr->name != NULL) && (strcmp(attr->name, attrName) == 0)) {
            break;
        }
    }
    return attr;
}

bool HcsGetBool(const struct DeviceResourceNode *node, const char *attrName)
{
    uint8_t value;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    if ((attr == NULL) || (attr->value == NULL)) {
        HDF_LOGE("%s failed, the node or attrName is NULL", __func__);
        return false;
    }

    if (!HcsSwapToUint8(&value, attr->value + HCS_PREFIX_LENGTH, HcsGetPrefix(attr->value))) {
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return false;
    }
    return value ? true : false;
}

#define RETURN_DEFAULT_VALUE(attr, attrName, value, def) do {                                              \
    if (((attr) == NULL) || ((attr)->value == NULL) || ((value) == NULL)) {                                \
        HDF_LOGE("%s failed, the attr of %s is NULL, or the value is NULL, the value is default value",    \
                __func__, ((attrName) == NULL) ? "error attrName" : (attrName));                           \
        if ((value) != NULL) {                                                                             \
            *(value) = (def);                                                                              \
        }                                                                                                  \
        return HDF_FAILURE;                                                                                \
    }                                                                                                      \
} while (0)

int32_t HcsGetUint8(const struct DeviceResourceNode *node, const char *attrName, uint8_t *value, uint8_t def)
{
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    if (!HcsSwapToUint8(value, attr->value + HCS_PREFIX_LENGTH, HcsGetPrefix(attr->value))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint16(const struct DeviceResourceNode *node, const char *attrName, uint16_t *value, uint16_t def)
{
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    if (!HcsSwapToUint16(value, attr->value + HCS_PREFIX_LENGTH, HcsGetPrefix(attr->value))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint32(const struct DeviceResourceNode *node, const char *attrName, uint32_t *value, uint32_t def)
{
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    if (!HcsSwapToUint32(value, attr->value + HCS_PREFIX_LENGTH, HcsGetPrefix(attr->value))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint64(const struct DeviceResourceNode *node, const char *attrName, uint64_t *value, uint64_t def)
{
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    if (!HcsSwapToUint64(value, attr->value + HCS_PREFIX_LENGTH, HcsGetPrefix(attr->value))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static const char *GetArrayElem(const struct DeviceResourceAttr *attr, uint32_t index)
{
    int32_t offset = HCS_WORD_LENGTH + HCS_PREFIX_LENGTH;
    uint16_t count;
    uint32_t i;
    if ((HcsGetPrefix(attr->value) != CONFIG_ARRAY) ||
        !HcsSwapToUint16(&count, attr->value + HCS_PREFIX_LENGTH, CONFIG_WORD)) {
        HDF_LOGE("%s failed, the attr of %s is not array", __func__, attr->name);
        return NULL;
    }
    if (index >= count) {
        HDF_LOGE("%s failed, index: %u >= count: %u", __func__, index, count);
        return NULL;
    }
    for (i = 0; i < index; i++) {
        int32_t result = HcsGetDataTypeOffset(attr->value + offset);
        if (result < 0) {
            return NULL;
        }
        offset += result;
    }
    return attr->value + offset;
}

int32_t HcsGetUint8ArrayElem(const struct DeviceResourceNode *node, const char *attrName, uint32_t index,
    uint8_t *value, uint8_t def)
{
    const char *realValue = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    realValue = GetArrayElem(attr, index);
    if (realValue == NULL) {
        *value = def;
        HDF_LOGE("%s failed, the realValue is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HcsSwapToUint8(value, realValue + HCS_PREFIX_LENGTH, HcsGetPrefix(realValue))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint16ArrayElem(const struct DeviceResourceNode *node, const char *attrName, uint32_t index,
    uint16_t *value, uint16_t def)
{
    const char *realValue = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    realValue = GetArrayElem(attr, index);
    if (realValue == NULL) {
        *value = def;
        HDF_LOGE("%s failed, the realValue is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HcsSwapToUint16(value, realValue + HCS_PREFIX_LENGTH, HcsGetPrefix(realValue))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint32ArrayElem(const struct DeviceResourceNode *node, const char *attrName, uint32_t index,
    uint32_t *value, uint32_t def)
{
    const char *realValue = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    realValue = GetArrayElem(attr, index);
    if (realValue == NULL) {
        *value = def;
        HDF_LOGE("%s failed, the realValue is NULL", __func__);
        return HDF_FAILURE;
    }
    if (!HcsSwapToUint32(value, realValue + HCS_PREFIX_LENGTH, HcsGetPrefix(realValue))) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

int32_t HcsGetUint64ArrayElem(const struct DeviceResourceNode *node, const char *attrName, uint32_t index,
    uint64_t *value, uint64_t def)
{
    const char *realValue = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    realValue = GetArrayElem(attr, index);
    if ((realValue == NULL) || !HcsSwapToUint64(value, realValue + HCS_PREFIX_LENGTH, HcsGetPrefix(realValue))) {
        *value = def;
        HDF_LOGE("%s failed, invalid realValue (NULL) or incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#define CONTINUE_RETURN_DIFFERENT_ERRNO(ret, result) do {              \
    if ((result) == HDF_ERR_INVALID_OBJECT) {                          \
        (ret) = HDF_ERR_INVALID_OBJECT;                                \
        HDF_LOGE("%s failed, the result is %d", __func__, (result));   \
        continue;                                                      \
    }                                                                  \
    if ((result) != HDF_SUCCESS) {                                     \
        HDF_LOGE("%s failed, the result is %d", __func__, (result));   \
        return result;                                                 \
    }                                                                  \
} while (0)

int32_t HcsGetUint8Array(const struct DeviceResourceNode *node, const char *attrName, uint8_t *value, uint32_t len,
    uint8_t def)
{
    int32_t ret = HDF_SUCCESS;
    uint32_t i;
    if ((value == NULL) || (len == 0)) {
        HDF_LOGE("%s failed, parameter error, len: %u", __func__, len);
        return HDF_FAILURE;
    }

    for (i = 0; i < len; i++) {
        int32_t result = HcsGetUint8ArrayElem(node, attrName, i, value + i, def);
        // If the error type is HDF_ERR_INVALID_OBJECT, the error is recorded and returned after the loop exits.
        CONTINUE_RETURN_DIFFERENT_ERRNO(ret, result);
    }
    return ret;
}

int32_t HcsGetUint16Array(const struct DeviceResourceNode *node, const char *attrName, uint16_t *value, uint32_t len,
    uint16_t def)
{
    int32_t ret = HDF_SUCCESS;
    uint32_t i;
    if ((value == NULL) || (len == 0)) {
        HDF_LOGE("%s failed, parameter error, len: %u", __func__, len);
        return HDF_FAILURE;
    }

    for (i = 0; i < len; i++) {
        int32_t result = HcsGetUint16ArrayElem(node, attrName, i, value + i, def);
        // If the error type is HDF_ERR_INVALID_OBJECT, the error is recorded and returned after the loop exits.
        CONTINUE_RETURN_DIFFERENT_ERRNO(ret, result);
    }
    return ret;
}

int32_t HcsGetUint32Array(const struct DeviceResourceNode *node, const char *attrName, uint32_t *value, uint32_t len,
    uint32_t def)
{
    int32_t ret = HDF_SUCCESS;
    uint32_t i;
    if ((value == NULL) || (len == 0)) {
        HDF_LOGE("%s failed, parameter error, len: %u", __func__, len);
        return HDF_FAILURE;
    }

    for (i = 0; i < len; i++) {
        int32_t result = HcsGetUint32ArrayElem(node, attrName, i, value + i, def);
        // If the error type is HDF_ERR_INVALID_OBJECT, the error is recorded and returned after the loop exits.
        CONTINUE_RETURN_DIFFERENT_ERRNO(ret, result);
    }
    return ret;
}

int32_t HcsGetUint64Array(const struct DeviceResourceNode *node, const char *attrName, uint64_t *value, uint32_t len,
    uint64_t def)
{
    uint32_t i;
    if ((value == NULL) || (len == 0)) {
        HDF_LOGE("%s failed, parameter error, len: %u", __func__, len);
        return HDF_FAILURE;
    }

    for (i = 0; i < len; i++) {
        int32_t result = HcsGetUint64ArrayElem(node, attrName, i, value + i, def);
        if (result != HDF_SUCCESS) {
            HDF_LOGE("%s failed, the ret is %d", __func__, result);
            return result;
        }
    }
    return HDF_SUCCESS;
}

int32_t HcsGetStringArrayElem(const struct DeviceResourceNode *node, const char *attrName, uint32_t index,
    const char **value, const char *def)
{
    const char *realValue = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);

    realValue = GetArrayElem(attr, index);
    if ((realValue == NULL) || (HcsGetPrefix(realValue) != CONFIG_STRING)) {
        *value = def;
        HDF_LOGE("%s failed, %s attr is default value", __func__, attrName);
        return HDF_FAILURE;
    }
    *value = realValue + HCS_PREFIX_LENGTH;
    return HDF_SUCCESS;
}

int32_t HcsGetString(const struct DeviceResourceNode *node, const char *attrName, const char **value, const char *def)
{
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    RETURN_DEFAULT_VALUE(attr, attrName, value, def);
    if (HcsGetPrefix(attr->value) != CONFIG_STRING) {
        *value = def;
        HDF_LOGE("%s failed, incorrect prefix", __func__);
        return HDF_FAILURE;
    }
    *value = attr->value + HCS_PREFIX_LENGTH;
    return HDF_SUCCESS;
}

int32_t HcsGetElemNum(const struct DeviceResourceNode *node, const char *attrName)
{
    uint16_t num;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    if ((attr == NULL) || (attr->value == NULL) || (HcsGetPrefix(attr->value) != CONFIG_ARRAY)) {
        HDF_LOGE("%s failed, %s attr error", __func__, (attrName == NULL) ? "error attrName" : attrName);
        return HDF_FAILURE;
    }

    (void)HcsSwapToUint16(&num, attr->value + HCS_PREFIX_LENGTH, CONFIG_WORD);
    return num;
}

static struct DeviceResourceAttr *GetAttrValueInNode(const struct DeviceResourceNode *node, const char *attrValue)
{
    struct DeviceResourceAttr *attr = NULL;
    if ((node == NULL) || (attrValue == NULL)) {
        return NULL;
    }
    for (attr = node->attrData; attr != NULL; attr = attr->next) {
        if ((attr->value != NULL) && (strcmp(attr->value + HCS_PREFIX_LENGTH, attrValue) == 0) &&
            (attr->name != NULL) && (strcmp(attr->name, HCS_MATCH_ATTR) == 0)) {
            break;
        }
    }
    return attr;
}

static const struct DeviceResourceNode *TraverseTreeNode(const struct DeviceResourceNode *curNode)
{
    const struct DeviceResourceNode *nextNode = curNode;
    while (nextNode->parent && !nextNode->sibling) {
        nextNode = nextNode->parent;
    }
    nextNode = nextNode->sibling;
    return nextNode;
}

const struct DeviceResourceNode *HcsGetNodeByMatchAttr(const struct DeviceResourceNode *node, const char *attrValue)
{
    const struct DeviceResourceNode *curNode = NULL;
    struct DeviceResourceIface *instance = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if ((attrValue == NULL) || (instance == NULL) || (instance->GetRootNode == NULL)) {
        HDF_LOGE("%s failed, attrValue or instance error", __func__);
        return NULL;
    }
    curNode = (node != NULL) ? node : instance->GetRootNode();
    while (curNode != NULL) {
        if (GetAttrValueInNode(curNode, attrValue) != NULL) {
            break;
        }
        curNode = (curNode->child != NULL) ? curNode->child : TraverseTreeNode(curNode);
    }
    return curNode;
}

const struct DeviceResourceNode *HcsGetChildNode(const struct DeviceResourceNode *node, const char *nodeName)
{
    struct DeviceResourceNode *child = NULL;
    if ((node == NULL) || (nodeName == NULL)) {
        HDF_LOGE("%s failed, the node or nodeName is NULL", __func__);
        return NULL;
    }

    for (child = node->child; child != NULL; child = child->sibling) {
        if ((child->name != NULL) && (strcmp(nodeName, child->name) == 0)) {
            break;
        }
    }
    return child;
}

const struct DeviceResourceNode *HcsGetNodeByRefAttr(const struct DeviceResourceNode *node, const char *attrName)
{
    uint32_t attrValue;
    struct DeviceResourceIface *instance = NULL;
    const struct DeviceResourceNode *curNode = NULL;
    struct DeviceResourceAttr *attr = GetAttrInNode(node, attrName);
    if ((attr == NULL) || (attr->value == NULL) || (HcsGetPrefix(attr->value) != CONFIG_REFERENCE)) {
        HDF_LOGE("%s failed, %s attr error", __func__, (attrName == NULL) ? "error attrName" : attrName);
        return NULL;
    }

    (void)HcsSwapToUint32(&attrValue, attr->value + HCS_PREFIX_LENGTH, CONFIG_DWORD);
    instance = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if ((instance == NULL) || (instance->GetRootNode == NULL)) {
        HDF_LOGE("%s failed, DeviceResourceGetIfaceInstance error", __func__);
        return NULL;
    }
    curNode = instance->GetRootNode();
    while (curNode != NULL) {
        if (curNode->hashValue == attrValue) {
            break;
        }
        curNode = (curNode->child != NULL) ? curNode->child : TraverseTreeNode(curNode);
    }
    return curNode;
}
