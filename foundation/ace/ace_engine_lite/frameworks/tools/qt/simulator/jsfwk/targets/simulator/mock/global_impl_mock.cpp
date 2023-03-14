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

#include "global.h"

#include <cstring>
#include <securec.h>

void GLOBAL_ConfigLanguage(const char *appLanguage)
{
    (void)(appLanguage);
}

int32_t GLOBAL_GetLanguage(char *language, uint8_t len)
{
    if (language == nullptr || len == 0) {
        return -1;
    }

    // language must be the element 0;
    return (strncpy_s(language, len, "zh", MAX_LANGUAGE_LENGTH - 1) != EOK) ? -1 : 0;
}

int32_t GLOBAL_GetRegion(char *region, uint8_t len)
{
    if (region == nullptr || len == 0) {
        return -1;
    }

    // region must be the element 1
    return (strncpy_s(region, len, "CN", MAX_REGION_LENGTH - 1) != EOK) ? -1 : 0;
}


int32_t GLOBAL_GetValueById(uint32_t id, const char *path, char **value)
{
    (void)(id);
    (void)(path);
    (void)(value);
    return 0;
}

int32_t GLOBAL_GetValueByName(const char *name, const char *path, char **value)
{
    (void)(name);
    (void)(path);
    (void)(value);
    return 0;
}
