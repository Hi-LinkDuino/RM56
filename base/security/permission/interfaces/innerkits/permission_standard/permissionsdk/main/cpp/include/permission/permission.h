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

#ifndef INTERFACES_INNER_KITS_PERMISSION_PERMISSION_H
#define INTERFACES_INNER_KITS_PERMISSION_PERMISSION_H

namespace OHOS {
namespace Security {
namespace Permission {
enum PermissionKitRet {
    RET_FAILED = -1,
    RET_SUCCESS = 0,
};

typedef enum TypePermissionState {
    PERMISSION_NOT_GRANTED = -1,
    PERMISSION_GRANTED = 0,
} PermissionState;

typedef enum TypeGrantMode {
    USER_GRANT = 0,
    SYSTEM_GRANT = 1,
} GrantMode;

typedef enum TypeAvailableScope {
    AVAILABLE_SCOPE_ALL = 1 << 0,
    AVAILABLE_SCOPE_SIGNATURE = 1 << 1,
    AVAILABLE_SCOPE_RESTRICTED = 1 << 2
} AvailableScope;
} // namespace Permission
} // namespace Security
} // namespace OHOS

#endif // INTERFACES_INNER_KITS_PERMISSION_PERMISSION_H