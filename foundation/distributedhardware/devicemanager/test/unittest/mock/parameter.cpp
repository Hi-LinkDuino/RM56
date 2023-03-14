/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "parameter.h"

int GetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    (void)key;
    (void)def;
    (void)value;
    (void)len;
    return 0;
}

int SetParameter(const char *key, const char *value)
{
    (void)key;
    (void)value;
    return 0;
}

int WatchParameter(const char *keyprefix, ParameterChgPtr callback, void *context)
{
    (void)keyprefix;
    (void)context;
    return 0;
}

int GetDevUdid(char *udid, int size)
{
    (void)udid;
    (void)size;
    return 0;
}
