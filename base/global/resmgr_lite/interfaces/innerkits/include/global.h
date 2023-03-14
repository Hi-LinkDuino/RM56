/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_H
#define OHOS_GLOBAL_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif // __cplusplus

#define RES_CONFIG_NUM_OFFSET 132
#define INDEX_DEFAULT_OFFSET  4
#define VALUE_LENGTH_OFFSET   2
#define OFFSET_VALUE_STEP     8
#define ALL_PARAM_LENGTH      4
#define INVALID_OFFSET        0
#define MAX_LANGUAGE_LENGTH   4
#define MAX_REGION_LENGTH     4

int32_t GLOBAL_GetValueById(uint32_t id, const char *path, char **value);
int32_t GLOBAL_GetValueByName(const char *name, const char *path, char **value);
void GLOBAL_ConfigLanguage(const char *appLanguage);
int32_t GLOBAL_GetLanguage(char *language, uint8_t len);
int32_t GLOBAL_GetRegion(char *region, uint8_t len);
int32_t GLOBAL_IsRTL(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif // __cplusplus

#endif // OHOS_GLOBAL_H
