/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OSAL_GET_CASE_H
#define OSAL_GET_CASE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int OsaTestBegin(void);
int OsaTestEnd(void);
int OsaTestALLResult(void);
int OsalGetTestResult(uint32_t cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_GET_CASE_H */
