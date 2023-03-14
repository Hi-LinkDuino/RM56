/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef DBM_ERRNO_H
#define DBM_ERRNO_H
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
/* --------------------------------------------------------------------------------------------*
 * Defintion of status code. The status codes are refer to > L1
 *-------------------------------------------------------------------------------------------- */
enum DBLiteStatus {
    DBM_ERROR = -1,             /* Execution failed */
    DBM_OK = 0,                 /* OK or No error */
    DBM_NOT_FOUND = 2,          /* Data not found */
    DBM_INVALID_ARGS = 3,       /* Invalid argument */
    DBM_NOT_SUPPORT = 5,        /* unsupported this function */
    DBM_OVER_MAX_LIMITS = 7,    /* Over max limits */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif // DBM_ERRNO_H
