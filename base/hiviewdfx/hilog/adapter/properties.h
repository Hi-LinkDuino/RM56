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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <cstdint>
#include <cstdlib>
#include <string>

static const int HILOG_PROP_VALUE_MAX = 92;

using PropType = enum {
    PROP_PRIVATE = 0x01,
    PROP_PROCESS_FLOWCTRL,
    PROP_DOMAIN_FLOWCTRL,
    PROP_GLOBAL_LOG_LEVEL,
    PROP_DOMAIN_LOG_LEVEL,
    PROP_TAG_LOG_LEVEL,
    PROP_SINGLE_DEBUG,
    PROP_KMSG,
    PROP_PERSIST_DEBUG,
    PROP_BUFFER_SIZE,
};

std::string GetPropertyName(uint32_t propType);
std::string GetProgName();
uint16_t GetTagLevel(const std::string& tag);
void PropertyGet(const std::string &key, char *value, int len);
void PropertySet(const std::string &key, const char* value);
bool IsDebugOn();
bool IsSingleDebugOn();
bool IsPersistDebugOn();
bool IsPrivateSwitchOn();
bool IsProcessSwitchOn();
bool IsDomainSwitchOn();
bool IsKmsgSwitchOn();
uint16_t GetGlobalLevel();
uint16_t GetDomainLevel(uint32_t domain);
size_t GetBufferSize(uint16_t type, bool persist);
void SetBufferSize(uint16_t type, bool persist, size_t size);
#endif
