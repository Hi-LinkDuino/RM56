/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_parser_test.h"
#include "hcs_config_test.h"
#include "hcs_macro_cases.h"

// add test case entry
HdfTestCaseList g_hdfConfigTestCaseList[] = {
    { HDF_CREATE_DM_HCS_TO_TREE_001, HcsTestCreateDMHcsToTree },
    { HDF_GET_NODE_BY_ATTR_VALUE_001, HcsTestGetNodeByMatchAttrSuccess },
    { HDF_GET_NODE_BY_ATTR_VALUE_002, HcsTestGetNodeByMatchAttrFail },
    { HDF_GET_BOOL_ATTR_VALUE_001, HcsTestGetBoolAttrValueSuccess },
    { HDF_GET_BOOL_ATTR_VALUE_002, HcsTestGetBoolAttrValueFail },
    { HDF_GET_UINT8_ATTR_VALUE_001, HcsTestGetUint8AttrValueSuccess },
    { HDF_GET_UINT8_ATTR_VALUE_002, HcsTestGetUint8AttrValueFail },
    { HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_001, HcsTestGetUint8ArrayElemSuccess },
    { HDF_GET_UINT8_ARRAY_ELEM_ATTR_VALUE_002, HcsTestGetUint8ArrayElemFail },
    { HDF_GET_UINT8_ARRAY_ATTR_VALUE_001, HcsTestGetUint8ArraySuccess },
    { HDF_GET_UINT8_ARRAY_ATTR_VALUE_002, HcsTestGetUint8ArrayFail },
    { HDF_GET_UINT16_ATTR_VALUE_001, HcsTestGetUint16AttrValueSuccess },
    { HDF_GET_UINT16_ATTR_VALUE_002, HcsTestGetUint16AttrValueFail },
    { HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_001, HcsTestGetUint16ArrayElemSuccess },
    { HDF_GET_UINT16_ARRAY_ELEM_ATTR_VALUE_002, HcsTestGetUint16ArrayElemFail },
    { HDF_GET_UINT16_ARRAY_ATTR_VALUE_001, HcsTestGetUint16ArraySuccess },
    { HDF_GET_UINT16_ARRAY_ATTR_VALUE_002, HcsTestGetUint16ArrayFail },
    { HDF_GET_UINT32_ATTR_VALUE_001, HcsTestGetUint32AttrValueSuccess },
    { HDF_GET_UINT32_ATTR_VALUE_002, HcsTestGetUint32AttrValueFail },
    { HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_001, HcsTestGetUint32ArrayElemSuccess },
    { HDF_GET_UINT32_ARRAY_ELEM_ATTR_VALUE_002, HcsTestGetUint32ArrayElemFail },
    { HDF_GET_UINT32_ARRAY_ATTR_VALUE_001, HcsTestGetUint32ArraySuccess },
    { HDF_GET_UINT32_ARRAY_ATTR_VALUE_002, HcsTestGetUint32ArrayFail },
    { HDF_GET_UINT64_ATTR_VALUE_001, HcsTestGetUint64AttrValueSuccess },
    { HDF_GET_UINT64_ATTR_VALUE_002, HcsTestGetUint64AttrValueFail },
    { HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_001, HcsTestGetUint64ArrayElemSuccess },
    { HDF_GET_UINT64_ARRAY_ELEM_ATTR_VALUE_002, HcsTestGetUint64ArrayElemFail },
    { HDF_GET_UINT64_ARRAY_ATTR_VALUE_001, HcsTestGetUint64ArraySuccess },
    { HDF_GET_UINT64_ARRAY_ATTR_VALUE_002, HcsTestGetUint64ArrayFail },
    { HDF_GET_ELEM_NUM_VALUE_001, HcsTestGetElemNumSuccess },
    { HDF_GET_ELEM_NUM_VALUE_002, HcsTestGetElemNumFail },
    { HDF_GET_CHILD_NODE_001, HcsTestGetChildNodeSuccess },
    { HDF_GET_CHILD_NODE_002, HcsTestGetChildNodeFail },
    { HDF_TRAVERSE_ATTR_IN_NODE_001, HcsTestTraverseAttrInNodeSuccess },
    { HDF_TRAVERSE_ATTR_IN_NODE_002, HcsTestTraverseAttrInNodeFail },
    { HDF_GET_STRING_ATTR_VALUE_001, HcsTestGetStringSuccess },
    { HDF_GET_STRING_ATTR_VALUE_002, HcsTestGetStringFail },
    { HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_001, HcsTestGetStringArrayElemSuccess },
    { HDF_GET_STRING_ARRAY_ELEM_ATTR_VALUE_002, HcsTestGetStringArrayElemFail },
    { HDF_GET_NODE_BY_ATTR_REF_001, HcsTestGetNodeAttrRefSuccess },
    { HDF_GET_NODE_BY_ATTR_REF_002, HcsTestGetNodeAttrRefFail },
    { HDF_MACRO_GET_ONE_NODE, HcsMacroGetOneNode },
    { HDF_MACRO_CHECK_NODE_EXISTS, HcsMacroCheckNodeExists },
    { HDF_MACRO_CHECK_NODE_NOT_EXISTS, HcsMacroCheckNodeNotExists },
    { HDF_MACRO_GET_ONE_NODE_PROP, HcsMacroCheckPropExists },
    { HDF_MACRO_CHECK_NODE_PROP_EXISTS, HcsMacroCheckPropNotExists },
    { HDF_MACRO_CHECK_NODE_PROP_NOT_EXISTS, HcsMacroGetOneNodeProp },
    { HDF_MACRO_TRAVERSAL_ONE_NODE, HcsMacroTraversalOneNode },
    { HDF_MACRO_GET_ARRAY_SIZE, HcsMacroGetArraySize },
    { HDF_MACRO_GET_ONE_ARRAY_ELEMENT, HcsMacroGetArrayElement },
    { HDF_MACRO_TRAVERSAL_ONE_ARRAY, HcsMacroTraversalOneArray },
    { HDF_MACRO_TRAVERSAL_ALL_ARRAY, HcsMacroTraversalAllArray },
    { HDF_MACRO_TRAVERSAL_NODE_CHILD, HcsMacroTraversalOneNodeChild },
    { HDF_MACRO_TRAVERSAL_NODE_CHILD_VARGS, HcsMacroTraversalOneNodeChildVargs },
    { HDF_MACRO_GET_ONE_FILE, HcsMacroGetOneFile },
};

int32_t HdfConfigEntry(HdfTestMsg *msg)
{
    int i;
    if (msg == NULL) {
        return HDF_FAILURE;
    }

    for (i = 0; i < sizeof(g_hdfConfigTestCaseList) / sizeof(g_hdfConfigTestCaseList[0]); ++i) {
        if (msg->subCmd != g_hdfConfigTestCaseList[i].subCmd) {
            continue;
        }
        if (g_hdfConfigTestCaseList[i].testFunc == NULL) {
            msg->result = HDF_FAILURE;
            return HDF_FAILURE;
        }
        msg->result = g_hdfConfigTestCaseList[i].testFunc();
        if (msg->result != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}
