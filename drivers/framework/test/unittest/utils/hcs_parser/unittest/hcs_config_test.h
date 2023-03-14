/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_CONFIG_TEST_H
#define HCS_CONFIG_TEST_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int HcsTestCreateDMHcsToTree(void);
int HcsTestGetNodeByMatchAttrSuccess(void);
int HcsTestGetNodeByMatchAttrFail(void);
int HcsTestGetBoolAttrValueSuccess(void);
int HcsTestGetBoolAttrValueFail(void);
int HcsTestGetUint8AttrValueSuccess(void);
int HcsTestGetUint8AttrValueFail(void);
int HcsTestGetUint8ArrayElemSuccess(void);
int HcsTestGetUint8ArrayElemFail(void);
int HcsTestGetUint8ArraySuccess(void);
int HcsTestGetUint8ArrayFail(void);
int HcsTestGetUint16AttrValueSuccess(void);
int HcsTestGetUint16AttrValueFail(void);
int HcsTestGetUint16ArrayElemSuccess(void);
int HcsTestGetUint16ArrayElemFail(void);
int HcsTestGetUint16ArraySuccess(void);
int HcsTestGetUint16ArrayFail(void);
int HcsTestGetUint32AttrValueSuccess(void);
int HcsTestGetUint32AttrValueFail(void);
int HcsTestGetUint32ArrayElemSuccess(void);
int HcsTestGetUint32ArrayElemFail(void);
int HcsTestGetUint32ArraySuccess(void);
int HcsTestGetUint32ArrayFail(void);
int HcsTestGetUint64AttrValueSuccess(void);
int HcsTestGetUint64AttrValueFail(void);
int HcsTestGetUint64ArrayElemSuccess(void);
int HcsTestGetUint64ArrayElemFail(void);
int HcsTestGetUint64ArraySuccess(void);
int HcsTestGetUint64ArrayFail(void);
int HcsTestGetElemNumSuccess(void);
int HcsTestGetElemNumFail(void);
int HcsTestGetChildNodeSuccess(void);
int HcsTestGetChildNodeFail(void);
int HcsTestTraverseAttrInNodeSuccess(void);
int HcsTestTraverseAttrInNodeFail(void);
int HcsTestGetStringSuccess(void);
int HcsTestGetStringFail(void);
int HcsTestGetStringArrayElemSuccess(void);
int HcsTestGetStringArrayElemFail(void);
int HcsTestGetNodeAttrRefSuccess(void);
int HcsTestGetNodeAttrRefFail(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HCS_CONFIG_TEST_H */
