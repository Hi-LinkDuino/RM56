/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_MACRO_CASES_H
#define HCS_MACRO_CASES_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int HcsMacroGetOneNode(void);
int HcsMacroCheckNodeExists(void);
int HcsMacroCheckNodeNotExists(void);
int HcsMacroCheckPropExists(void);
int HcsMacroCheckPropNotExists(void);
int HcsMacroGetOneNodeProp(void);
int HcsMacroTraversalOneNode(void);
int HcsMacroGetArraySize(void);
int HcsMacroGetArrayElement(void);
int HcsMacroTraversalOneArray(void);
int HcsMacroTraversalAllArray(void);
int HcsMacroTraversalOneNodeChild(void);
int HcsMacroTraversalOneNodeChildVargs(void);
int HcsMacroGetOneFile(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HCS_MACRO_CASES_H */
