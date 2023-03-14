/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_LITE_HOTA_HAL_VERIFY_H
#define OHOS_LITE_HOTA_HAL_VERIFY_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define BUFFR_LENGTH            128
#define HASH_LENGTH             32

void HotaHashInit(void);

void HotaHashCalc(const uint8 *buffer, uint32 length);

int32 HotaGetHash(uint8 *buffer, uint32 length);

/* Verify sign between two sign */
int32 HotaSignVerify(uint8 *image, uint32 imgLen, uint8 *imageSign, uint32 signLen);

/* Get Pubkey */
uint8 *HotaGetPubKey(uint32 *length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_HAL_VERIFY_H */
