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

#include "hks_type.h"

#ifndef HKS_TEST_FILE_OPERATOR_H
#define HKS_TEST_FILE_OPERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksIsFileExist(const char *path, const char *fileName);

uint32_t HksTestFileSize(const char *path, const char *fileName);

uint32_t HksTestFileRead(const char *path, const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len);

int32_t HksTestFileWrite(const char *path, const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len);

int32_t HksTestFileRemove(const char *path, const char *fileName);
#ifdef __cplusplus
}
#endif

#endif /* HKS_TEST_FILE_OPERATOR_H */
