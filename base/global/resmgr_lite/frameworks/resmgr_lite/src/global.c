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

#include "global.h"

#include <fcntl.h>
#include <limits.h>
#include <securec.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "global_utils.h"

/*
 * locale format as below, use '-' or '_' to link, e.g. en_Latn_US
 * and needn't have all 3 elements, so max length is 13 including '\0'
 * |   language  |   script  |        region          |
 * | ----------- | --------- | ---------------------- |
 * | 2-3 letters | 4 letters | 2 letters or 3 numbers |
 */

#define MAX_LOCALE_LENGTH 13
#define UI_LOCALE_ELEMENT_NUM 2
#define MAX_SCRIPT_LENGTH 5
static char g_locale[MAX_LOCALE_LENGTH] = {0};

void GLOBAL_ConfigLanguage(const char *appLanguage)
{
    if (appLanguage == NULL) {
        return;
    }
    // appLanguage is same as g_locale, needn't strcpy_s
    if (strcmp(appLanguage, g_locale) == 0) {
        return;
    }
    if (strcpy_s(g_locale, MAX_LOCALE_LENGTH, appLanguage) != EOK) {
        return;
    }
}

int32_t GLOBAL_GetLanguage(char *language, uint8_t len)
{
    if (language == NULL || len == 0) {
        return MC_FAILURE;
    }
    char *localeArray[LOCALE_ELEMENT_NUM] = {NULL};
    char tempLocale[MAX_LOCALE_LENGTH] = {'\0'};
    int32_t ret = strcpy_s(tempLocale, MAX_LOCALE_LENGTH, g_locale);
    if (ret != EOK) {
        return MC_FAILURE;
    }
    int32_t count = 0;
    ret = GetGlobalUtilsImpl()->SplitLocale(tempLocale, localeArray, &count);
    if (ret == MC_FAILURE || count != UI_LOCALE_ELEMENT_NUM) {
        return MC_FAILURE;
    }

    // language must be the element 0;
    return (strncpy_s(language, len, localeArray[0], MAX_LANGUAGE_LENGTH - 1) != EOK) ? MC_FAILURE : MC_SUCCESS;
}

int32_t GLOBAL_IsRTL(void)
{
    char *localeArray[LOCALE_ELEMENT_NUM] = { NULL };
    char tempLocale[MAX_LOCALE_LENGTH] = { '\0' };
    int32_t ret = strcpy_s(tempLocale, MAX_LOCALE_LENGTH, g_locale);
    if (ret != EOK) {
        return 0;
    }
    int32_t count = 0;
    ret = GetGlobalUtilsImpl()->SplitLocale(tempLocale, localeArray, &count);
    if (ret == MC_FAILURE || count != UI_LOCALE_ELEMENT_NUM) {
        return 0;
    }
    char script[MAX_SCRIPT_LENGTH] = { 0 };
    if (strncpy_s(script, MAX_SCRIPT_LENGTH, localeArray[1], MAX_SCRIPT_LENGTH - 1) != EOK) {
        return 0;
    }
    // if script is set and script != arab or script != hebr, return false;
    if ((strlen(script) == MAX_SCRIPT_LENGTH - 1) &&
        (strcmp(script, "Arab") != 0) && (strcmp(script, "Hebr") != 0)) {
        return 0;
    }
    char language[MAX_LANGUAGE_LENGTH] = { 0 };
    if (strncpy_s(language, MAX_LANGUAGE_LENGTH, localeArray[0], MAX_LANGUAGE_LENGTH - 1) != EOK) {
        return 0;
    }
    if ((strcmp(language, "fa") == 0) || (strcmp(language, "ar") == 0) || (strcmp(language, "ur") == 0) ||
        (strcmp(language, "ug") == 0) || (strcmp(language, "he") == 0) || (strcmp(language, "iw") == 0)) {
        return 1;
    }
    return 0;
}

int32_t GLOBAL_GetRegion(char *region, uint8_t len)
{
    if (region == NULL || len == 0) {
        return MC_FAILURE;
    }
    char *localeArray[LOCALE_ELEMENT_NUM] = {NULL};
    char tempLocale[MAX_LOCALE_LENGTH] = {'\0'};
    int32_t ret = strcpy_s(tempLocale, MAX_LOCALE_LENGTH, g_locale);
    if (ret != EOK) {
        return MC_FAILURE;
    }
    int32_t count = 0;
    ret = GetGlobalUtilsImpl()->SplitLocale(tempLocale, localeArray, &count);
    if (ret == MC_FAILURE || count != UI_LOCALE_ELEMENT_NUM) {
        return MC_FAILURE;
    }

    // region must be the element 1
    return (strncpy_s(region, len, localeArray[1], MAX_REGION_LENGTH - 1) != EOK) ? MC_FAILURE : MC_SUCCESS;
}

static void FreeIdItem(IdItem *idItem)
{
    if (idItem == NULL) {
        return;
    }
    free(idItem->value);
    free(idItem->name);
    idItem->value = NULL;
    idItem->name = NULL;
}

static void FreeValue(char **value)
{
    if (*value != NULL) {
        free(*value);
        *value = NULL;
    }
}

static int32_t GLOBAL_GetValueByIdInternal(uint32_t id, const char *path, const char *locale, char **value)
{
    char realResourcePath[PATH_MAX] = {'\0'};
    GlobalUtilsImpl *utilsImpl = GetGlobalUtilsImpl();
    if (utilsImpl->CheckFilePath(path, realResourcePath, PATH_MAX) == MC_FAILURE) {
        return MC_FAILURE;
    }

    uint32_t idHeaderOffset = utilsImpl->GetOffsetByLocale(realResourcePath, locale, MAX_LOCALE_LENGTH);
    IdHeader idHeader = {0, NULL};
    int32_t file = open(realResourcePath, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if (file < 0) {
        return MC_FAILURE;
    }
    int32_t ret = utilsImpl->GetIdHeaderByOffset(file, idHeaderOffset, &idHeader);
    if (ret != MC_SUCCESS) {
        close(file);
        return ret;
    }

    int32_t result = MC_FAILURE;
    IdItem idItem = {0, INVALID_RES_TYPE, 0, 0, NULL, 0, NULL};
    for (uint32_t i = 0; i < idHeader.count; i++) {
        if (idHeader.idParams[i].id == id) {
            ret = utilsImpl->GetIdItem(file, idHeader.idParams[i].offset, &idItem);
            if (ret != MC_SUCCESS) {
                close(file);
                free(idHeader.idParams);
                return ret;
            }
            *value = (char *)malloc(idItem.valueLen);
            if (*value == NULL || strcpy_s(*value, idItem.valueLen, idItem.value) != EOK) {
                close(file);
                free(idHeader.idParams);
                FreeIdItem(&idItem);
                FreeValue(value);
                return MC_FAILURE;
            }
            result = MC_SUCCESS;
            break;
        }
    }
    close(file);
    free(idHeader.idParams);
    FreeIdItem(&idItem);
    return result;
}

int32_t GLOBAL_GetValueById(uint32_t id, const char *path, char **value)
{
    if (path == NULL || path[0] == '\0' || value == NULL) {
        return MC_FAILURE;
    }

    char tempLocale[MAX_LOCALE_LENGTH] = {'\0'};
    int32_t ret = strcpy_s(tempLocale, MAX_LOCALE_LENGTH, g_locale);
    if (ret != EOK) {
        return MC_FAILURE;
    }

    char *locales[] = {tempLocale, ""};
    for (int i = 0; i < 2; i++) { // 2 current language and the default.
        if (GLOBAL_GetValueByIdInternal(id, path, locales[i], value) == MC_SUCCESS) {
            return MC_SUCCESS;
        }
    }
    return MC_FAILURE;
}

static int32_t GLOBAL_GetValueByNameInternal(const char *name, const char *path, const char *locale, char **value)
{
    char realResourcePath[PATH_MAX] = {'\0'};
    GlobalUtilsImpl *utilsImpl = GetGlobalUtilsImpl();
    if (utilsImpl->CheckFilePath(path, realResourcePath, PATH_MAX) == MC_FAILURE) {
        return MC_FAILURE;
    }
    uint32_t idHeaderOffset = utilsImpl->GetOffsetByLocale(realResourcePath, locale, MAX_LOCALE_LENGTH);
    IdHeader idHeader = {0, NULL};
    int32_t file = open(realResourcePath, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if (file < 0) {
        return MC_FAILURE;
    }
    int32_t ret = utilsImpl->GetIdHeaderByOffset(file, idHeaderOffset, &idHeader);
    if (ret != MC_SUCCESS) {
        close(file);
        return ret;
    }

    int32_t result = MC_FAILURE;
    IdItem idItem = {0, INVALID_RES_TYPE, 0, 0, NULL, 0, NULL};
    for (uint32_t i = 0; i < idHeader.count; i++) {
        ret = utilsImpl->GetIdItem(file, idHeader.idParams[i].offset, &idItem);
        if (ret != MC_SUCCESS) {
            close(file);
            free(idHeader.idParams);
            return ret;
        }
        if (strcmp(name, idItem.name) != 0) {
            FreeIdItem(&idItem);
            continue;
        }
        *value = (char *)malloc(idItem.valueLen);
        if (*value == NULL || strcpy_s(*value, idItem.valueLen, idItem.value) != EOK) {
            close(file);
            free(idHeader.idParams);
            FreeIdItem(&idItem);
            FreeValue(value);
            return MC_FAILURE;
        }
        result = MC_SUCCESS;
        break;
    }

    close(file);
    free(idHeader.idParams);
    FreeIdItem(&idItem);
    return result;
}

int32_t GLOBAL_GetValueByName(const char *name, const char *path, char **value)
{
    if (name == NULL || strlen(name) == 0 || path == NULL || strlen(path) == 0 || value == NULL) {
        return MC_FAILURE;
    }

    char tempLocale[MAX_LOCALE_LENGTH] = {'\0'};
    int32_t ret = strcpy_s(tempLocale, MAX_LOCALE_LENGTH, g_locale);
    if (ret != EOK) {
        return MC_FAILURE;
    }

    char *locales[] = {tempLocale, ""};
    for (int i = 0; i < 2; i++) { // 2 current language and the default.
        if (GLOBAL_GetValueByNameInternal(name, path, locales[i], value) == MC_SUCCESS) {
            return MC_SUCCESS;
        }
    }
    return MC_FAILURE;
}
