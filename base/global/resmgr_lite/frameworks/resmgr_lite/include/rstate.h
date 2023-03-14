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

#ifndef OHOS_RESOURCE_MANAGER_RSTATE_H
#define OHOS_RESOURCE_MANAGER_RSTATE_H

namespace OHOS {
namespace Global {
namespace Resource {
enum RState {
    SUCCESS = 0,
    NOT_SUPPORT_SEP = 1,
    INVALID_BCP47_STR_LEN_TOO_SHORT = 2,
    INVALID_BCP47_LANGUAGE_SUBTAG = 3,
    INVALID_BCP47_SCRIPT_SUBTAG = 4,
    INVALID_BCP47_REGION_SUBTAG = 5,
    HAP_INIT_FAILED = 6,
    NOT_FOUND = 7,
    INVALID_FORMAT = 8,
    LOCALEINFO_IS_NULL = 9,
    NOT_ENOUGH_MEM = 10,
    ERROR = 10000
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif