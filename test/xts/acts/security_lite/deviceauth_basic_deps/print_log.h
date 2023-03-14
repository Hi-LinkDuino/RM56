/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef PRINT_LOG_H
#define PRINT_LOG_H

#include <stdint.h>
#include <stdio.h>

#define LOGI(fmt, ...) printf("[I][%s][%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGD(fmt, ...) printf("[D][%s][%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("[E][%s][%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

void PrintBuffer(const char *tag, const uint8_t *ptr, int32_t len);

#ifdef __cplusplus
}
#endif

#endif // PRINT_LOG_H