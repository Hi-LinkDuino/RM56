/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_LITE_HOTA_PARTITION_H
#define OHOS_LITE_HOTA_PARTITION_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define PARTITION_NAME_LENGTH   16
#define PARTITION_ERROR         -1
#define PARTITION_INFO_COMP     1    /* The info component of ota package. */
#define SIGN_RSA2048_LEN       256
#define SIGN_RSA3072_LEN       384

#define ABILITY_DIFF_UPDATE      (0x1 << 0)
#define ABILITY_PATCH_UPDATE     (0x1 << 1)
#define ABILITY_PKG_SEARCH       (0x1 << 2)
#define ABILITY_PKG_DLOAD        (0x1 << 3)
#define ABILITY_UPDATE_AUTH      (0x1 << 4)
#define ABILITY_AUTO_UPDATE      (0x1 << 5)
#define ABILITY_FLOW_INSTALL     (0x1 << 6)
#define ABILITY_AB_PART_INSTALL  (0x1 << 7)

typedef enum {
    SIGN_ARITHMETIC_RSA2048 = 0x0001,
    SIGN_ARITHMETIC_RSA3072 = 0x0011
} HotaSignArithmetic;

typedef struct {
    int id;
    const char *componentName;
    const char *imgPath;
    unsigned char abFlag;
} ComponentTableInfo;

typedef struct {
    unsigned char updateMode;
    unsigned char runningPartition;
    unsigned char updatePartition;
    unsigned char runningStatus;
    unsigned char otaStatus;
    unsigned char rebootStatus;
    unsigned char updateStatus;
} UpdateMetaData;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_PARTITION_H */
