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

#ifndef RESOURCE_MANAGER_TEST_COMMON_H
#define RESOURCE_MANAGER_TEST_COMMON_H

#include "hap_resource.h"
#include "hilog_wrapper.h"

#include <map>
#include <string>
#include <vector>

// this is relative path
static const char *g_resFilePath = "all/assets/entry/resources.index";

namespace OHOS {
namespace Global {
namespace Resource {
std::string FormatFullPath(const char *fileRelativePath);

void PrintIdValues(const HapResource::IdValues *idValues);

void PrintMapString(const std::map<std::string, std::string> &value);

void PrintVectorString(const std::vector<std::string> &value);

ResConfig *CreateResConfig(const char *language, const char *script, const char *region);
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
