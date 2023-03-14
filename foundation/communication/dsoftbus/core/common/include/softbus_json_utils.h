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
#ifndef SOFTBUS_JSON_UTILS_H
#define SOFTBUS_JSON_UTILS_H

#include <stdint.h>

#include "cJSON.h"
#include "string.h"
#include "stdbool.h"
#include "softbus_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

bool GetJsonObjectStringItem(const cJSON *json, const char * const string, char *target, uint32_t targetLen);

bool GetJsonObjectNumberItem(const cJSON *json, const char * const string, int *target);

bool GetJsonObjectSignedNumberItem(const cJSON *json, const char * const string, int *target);

bool GetJsonObjectNumber64Item(const cJSON *json, const char * const string, int64_t *target);

bool GetJsonObjectDoubleItem(const cJSON *json, const char * const string, double *target);

bool GetJsonObjectBoolItem(const cJSON *json, const char * const string, bool *target);

bool AddStringToJsonObject(cJSON *json, const char * const string, const char *value);

bool AddNumberToJsonObject(cJSON *json, const char * const string, int num);

bool AddNumber64ToJsonObject(cJSON *json, const char * const string, int64_t num);

bool AddBoolToJsonObject(cJSON *json, const char * const string, bool value);

bool GetJsonObjectInt32Item(const cJSON *json, const char * const string, int32_t *target);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* SOFTBUS_JSON_UTILS_H */