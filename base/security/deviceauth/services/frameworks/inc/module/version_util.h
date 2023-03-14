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

#ifndef VERSION_UTIL_H
#define VERSION_UTIL_H

#include "json_utils.h"
#include "common_defs.h"
#include "protocol_common.h"

#define MAJOR_VERSION_NO 2
#define TMP_VERSION_STR_LEN 15

typedef struct {
    uint32_t first;
    uint32_t second;
    uint32_t third;
} VersionStruct;

int32_t VersionToString(const VersionStruct *version, char *verStr, uint32_t len);
int32_t StringToVersion(const char* verStr, VersionStruct* version);

int32_t AddSingleVersionToJson(CJson *jsonObj, const VersionStruct *version);
int32_t GetSingleVersionFromJson(const CJson* jsonObj, VersionStruct *version);
void InitGroupAndModuleVersion(VersionStruct *version);

#endif
