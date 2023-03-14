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

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef cJSON CJson;

/* Need to call FreeJson to free the returned pointer when it's no longer in use. */
CJson *CreateJsonFromString(const char *jsonStr);
/* Need to call FreeJson to free the returned pointer when it's no longer in use. */
CJson *CreateJson(void);
/* Need to call FreeJson to free the returned pointer when it's no longer in use. */
CJson *CreateJsonArray(void);
/* Need to call FreeJson to free the returned pointer when it's no longer in use. */
CJson *DuplicateJson(const CJson *jsonObj);
void FreeJson(CJson *jsonObj);

void DeleteItemFromJson(CJson *jsonObj, const char *key);
void DeleteAllItemExceptOne(CJson *jsonObj, const char *key);
void DeleteAllItem(CJson *jsonObj);
CJson *DetachItemFromJson(CJson *jsonObj, const char *key);

/* Need to call FreeJsonString to free the returned pointer when it's no longer in use. */
char *PackJsonToString(const CJson *jsonObj);
void FreeJsonString(char *jsonStr);

int GetItemNum(const CJson *jsonObj);
/*
 * Can't release the returned pointer, otherwise, an exception may occur.
 * It refers to the parent object(param--jsonObj)'s memory.
 * It will be recycled along with jsonObj when jsonObj is released.
 */
const char *GetItemKey(const CJson *item);

/*
 * Can't release the returned pointer, otherwise, an exception may occur.
 * It refers to the parent object(param--jsonObj)'s memory.
 * It will be recycled along with jsonObj when jsonObj is released.
 */
CJson *GetObjFromJson(const CJson *jsonObj, const char *key);

/*
 * Can't release the returned pointer, otherwise, an exception may occur.
 * It refers to the parent object(param--jsonObj)'s memory.
 * It will be recycled along with jsonObj when jsonObj is released.
 */
CJson *GetItemFromArray(const CJson *jsonArr, int index);

/*
 * Can't release the returned pointer, otherwise, an exception may occur.
 * It refers to the parent object(param--jsonObj)'s memory.
 * It will be recycled along with jsonObj when jsonObj is released.
 */
const char *GetStringFromJson(const CJson *jsonObj, const char *key);

/*
 * The byte in jsonObj must be in the form of hex string.
 * This function will convert the hex string to byte, and then put it in param--byte in the form of byte.
 */
int32_t GetByteFromJson(const CJson *jsonObj, const char *key, uint8_t *byte, uint32_t len);
int32_t GetIntFromJson(const CJson *jsonObj, const char *key, int32_t *value);
int32_t GetInt64FromJson(const CJson *jsonObj, const char *key, int64_t *value);
int32_t GetBoolFromJson(const CJson *jsonObj, const char *key, bool *value);
char *GetStringValue(const CJson *item);

int32_t AddObjToJson(CJson *jsonObj, const char *key, const CJson *childObj);
int32_t AddObjToArray(CJson *jsonArr, CJson *item);
int32_t AddStringToJson(CJson *jsonObj, const char *key, const char *value);
int32_t AddStringToArray(CJson *jsonArr, const char *string);
/* The function will convert the byte to hex string, and then add it to object. */
int32_t AddByteToJson(CJson *jsonObj, const char *key, const uint8_t *byte, uint32_t len);
int32_t AddBoolToJson(CJson *jsonObj, const char *key, bool value);
int32_t AddIntToJson(CJson *jsonObj, const char *key, int value);
int32_t AddInt64StringToJson(CJson *jsonObj, const char *key, int64_t value);
int32_t AddStringArrayToJson(CJson *jsonObj, const char *key, const char * const *stringArray, uint32_t arrayLen);
void ClearSensitiveStringInJson(CJson *jsonObj, const char *key);
void ClearAndFreeJsonString(char *jsonStr);
int32_t GetUnsignedIntFromJson(const CJson *jsonObj, const char *key, uint32_t *value);

#ifdef __cplusplus
}
#endif
#endif
