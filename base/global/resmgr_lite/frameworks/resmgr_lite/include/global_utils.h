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

#ifndef OHOS_GLOBAL_UTILS_H
#define OHOS_GLOBAL_UTILS_H

#include "global.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif // __cplusplus

#define SCRIPT_LENGTH         4
#define LOCALE_ELEMENT_NUM 3

typedef enum KeyType {
    LANGUAGES   = 0,
    REGION      = 1,
    RESOLUTION  = 2,
    ORIENTATION = 3,
    DEVICETYPE  = 4,
    SCRIPT      = 5,
    KEY_TYPE_MAX
} KeyType;

typedef enum ResType {
    VALUES    = 0,
    ANIMATION = 1,
    DRAWABLE  = 2,
    LAYOUT    = 3,
    MENU      = 4,
    MIPMAP    = 5,
    RAW       = 6,
    XML       = 7,

    INTEGER          = 8,
    STRING           = 9,
    STRINGARRAY      = 10,
    INTARRAY         = 11,
    BOOLEAN_         = 12,
    DIMEN            = 13,
    COLOR            = 14,
    ID               = 15,
    THEME            = 16,
    PLURALS          = 17,
    MY_FLOAT         = 18,
    MEDIA            = 19,
    PROF             = 20,
    SVG              = 21,
    INVALID_RES_TYPE = -1
} ResType;

typedef struct KeyParam {
    KeyType type;
    uint32_t value;
} KeyParam;

typedef struct Key {
    uint32_t offset;
    uint32_t keysCount;
    KeyParam *keyParams;
} Key;

typedef struct IdParam {
    uint32_t id;
    uint32_t offset;
} IdParam;

typedef struct IdHeader {
    uint32_t count;
    IdParam *idParams;
} IdHeader;

typedef struct IdItem {
    uint32_t size;
    ResType resType;
    uint32_t id;
    uint16_t valueLen;
    char *value;
    uint16_t nameLen;
    char *name;
} IdItem;

typedef struct GlobalUtilsImpl {
    uint32_t (*GetOffsetByLocale)(const char *path, const char *locale, uint32_t length);
    uint32_t (*GetDefaultOffsetValue)(int32_t file);
    uint32_t (*GetKeyValue)(int32_t file);
    int32_t (*GetIdItem)(int32_t file, uint32_t offset, IdItem *idItem);
    uint32_t (*GetIdHeaderOffsetByLocale)(const char *locale, const Key *keys, uint32_t configNum);
    int32_t (*GetIdHeaderByOffset)(int32_t file, uint32_t offset, IdHeader *idHeader);
    int32_t (*SplitLocale)(const char *src, char **dest, int32_t *num);
    int32_t (*CheckFilePath)(const char *path, char *realResourcePath, int32_t length);
} GlobalUtilsImpl;

typedef struct LocaleItem {
    uint32_t language;
    uint32_t script;
    uint32_t region;
} LocaleItem;

GlobalUtilsImpl *GetGlobalUtilsImpl(void);

#define MC_FAILURE (-1)
#define MC_SUCCESS 0

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif // __cplusplus

#endif // OHOS_GLOBAL_UTILS_H
