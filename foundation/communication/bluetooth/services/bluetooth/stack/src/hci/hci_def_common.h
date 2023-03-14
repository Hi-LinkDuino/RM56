/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HCI_DEF_COMMON_H
#define HCI_DEF_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct {
    uint8_t status;
} HciStatusParam;

#define BD_ADDR_LENGTH 6

typedef struct {
    uint8_t raw[BD_ADDR_LENGTH];
} HciBdAddr;

#define LMP_FEATURES_SIZE 8

typedef struct {
    uint8_t raw[LMP_FEATURES_SIZE];
} HciLmpFeatures;

#define MAX_EXTENED_FEATURES_PAGE_NUMBER 2
#define MAX_EXTENED_FEATURES_PAGE_COUNT (MAX_EXTENED_FEATURES_PAGE_NUMBER + 1)

typedef struct {
    uint8_t page[MAX_EXTENED_FEATURES_PAGE_COUNT][LMP_FEATURES_SIZE];
} HciExtendedLmpFeatures;

#define LE_FEATURES_SIZE 8

typedef struct {
    uint8_t raw[LE_FEATURES_SIZE];
} HciLeFeatures;

#pragma pack(0)

#ifdef __cplusplus
}
#endif

#endif