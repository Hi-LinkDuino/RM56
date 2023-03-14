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

#ifndef RESOURCE_MANAGER_COMMON_H
#define RESOURCE_MANAGER_COMMON_H

#include <cstdint>

namespace OHOS {
namespace Global {
namespace Resource {
static constexpr int KILO = 1000;
static constexpr int MILLION = 1000000;
static constexpr uint8_t SCRIPT_LEN = 4;
static constexpr uint8_t SCRIPT_ARRAY_LEN = 5;
static constexpr uint8_t MAX_DEPTH_REF_SEARCH = 20;
static constexpr char DASH_SEP = '-';
static constexpr char UNDERLINE_SEP = '_';
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif // RESOURCE_MANAGER_COMMON_H
