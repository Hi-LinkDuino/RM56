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
#ifndef DBM_DEF_H
#define DBM_DEF_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* default max len of KV */
#define MAX_KEY_LEN 32      // the suffix is 7 bytes
#ifndef MAX_VALUE_LEN
#define MAX_VALUE_LEN 512
#endif

/*
 * Defintion of macro using in KVStore.
 */

#define DBM_SW_VERSION              "1.0"
#define DBM_SW_VERSION_NUM          0x10

#define DBM_CTRL_SET_LOCK           0x0         /* set lock function control command */
#define DBM_CTRL_SET_UNLOCK         0x1         /* set unlock function control command */

/* log function: default DBM_PRINT macro is printf */
#ifndef DBM_PRINT
#define DBM_PRINT(...)              printf(__VA_ARGS__)
#endif
#define DBM_LOG_PREFIX1()           DBM_PRINT("[DataMgrKVDB]"DBM_LOG_TAG)
#define DBM_LOG_PREFIX2()           DBM_PRINT(" ")
#define DBM_LOG_PREFIX()            DBM_LOG_PREFIX1();DBM_LOG_PREFIX2()
#ifdef DBM_DEBUG_ENABLE
#define DBM_DEBUG(...)              DBM_LOG_PREFIX();DBM_PRINT(__VA_ARGS__);DBM_PRINT("\n")
#else
#define DBM_DEBUG(...)
#endif

#ifdef DBM_RUNTIME_LOG_ENABLE
/* routine print function. Must be implement by user. */
#define DBM_INFO(...)               DBM_LOG_PREFIX();DBM_PRINT(__VA_ARGS__);DBM_PRINT("\n")
#else
#define DBM_INFO(...)
#endif

/* assert for developer */
#define DBM_ASSERT(EXPR)                                            \
if (!(EXPR))                                                        \
{                                                                   \
    DBM_DEBUG("%s has assert failed at %s.", #EXPR, __FUNCTION__);  \
    while (1);                                                      \
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DBM_DEF_H */
